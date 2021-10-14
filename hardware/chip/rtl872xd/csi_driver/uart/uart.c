/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include "objects.h"
#include "pinmap.h"
#include <string.h>
#include <drv/uart.h>
#include <aos/tty_core.h>

#define EVENT_RX_READY  ((uint32_t)1 << 0)

typedef struct {
    UART_InitTypeDef init_data;
    aos_event_t event;
} uart_priv_t;

static const PinMap CSI_PinMap_UART_TX[] = {
    { PA_12,    UART_3, PIN_DATA(PullUp, PINMUX_FUNCTION_UART), },
    { PA_18,    UART_0, PIN_DATA(PullUp, PINMUX_FUNCTION_UART), },
    { PA_7,     UART_2, PIN_DATA(PullUp, PINMUX_FUNCTION_LOGUART), },
    { NC,       NC,     0, },
};

static const PinMap CSI_PinMap_UART_RX[] = {
    { PA_13,    UART_3, PIN_DATA(PullUp, PINMUX_FUNCTION_UART), },
    { PA_19,    UART_0, PIN_DATA(PullUp, PINMUX_FUNCTION_UART), },
    { PA_8,     UART_2, PIN_DATA(PullUp, PINMUX_FUNCTION_LOGUART), },
    { NC,       NC,     0, },
};

static PinName uart_tx_pin_get(uint32_t uart_idx, const PinMap *map)
{
    PinName tx;
    while (map->peripheral != NC) {
        if (map->peripheral == uart_idx) {
            tx = map->pin;
            break;
        }
        map++;
    }
    return tx;
}

static PinName uart_rx_pin_get(uint32_t uart_idx, const PinMap *map)
{
    PinName rx;
    while (map->peripheral != NC) {
        if (map->peripheral == uart_idx) {
            rx = map->pin;
            break;
        }
        map++;
    }
    return rx;
}

static uint32_t uart_idx_get(uint32_t idx)
{
    if (idx == 0)
        return UART_0;
    else if (idx == 2)
        return UART_2;
    else if (idx == 3)
        return UART_3;
    else
        assert_param(0);
    return UART_1;
}

static uint32_t uart_irqhandler(void *data)
{
    volatile uint8_t reg_iir;
    uint8_t IntId;
    uint32_t RegValue;

    csi_uart_t *uart = (csi_uart_t *)data;
    UART_TypeDef *UARTx = UART_DEV_TABLE[uart->dev.idx].UARTx;
    uart_priv_t *priv = (uart_priv_t *)uart->priv;

    reg_iir = UART_IntStatus(UARTx);
    if ((reg_iir & RUART_IIR_INT_PEND) != 0) {
        /* No pending IRQ */
        return 0;
    }

    IntId = (reg_iir & RUART_IIR_INT_ID) >> 1;

    switch (IntId) {
    case RUART_LP_RX_MONITOR_DONE:
        RegValue = UART_RxMonitorSatusGet(UARTx);
        break;

    case RUART_MODEM_STATUS:
        RegValue = UART_ModemStatusGet(UARTx);
        break;

    case RUART_RECEIVE_LINE_STATUS:
        RegValue = UART_LineStatusGet(UARTx);
        break;

    case RUART_TX_FIFO_EMPTY:
        if (UART_GetTxFlag(uart->dev.idx)) {
            int32_t cnt = 16;
            while (cnt > 0 && uart->tx_size > 0) {
                UART_CharPut(UARTx, *uart->tx_data);
                uart->tx_size--;
                uart->tx_data++;
                cnt--;
            }

            if (0 == uart->tx_size) {
                /* Mask UART TX FIFO empty */
                UART_INTConfig(UARTx, RUART_IER_ETBEI, DISABLE);
                UART_SetTxFlag(uart->dev.idx, 0);
                if (uart->callback != NULL) {
                    uart->callback(uart, UART_EVENT_SEND_COMPLETE, NULL);
                }
            }
        } else {
            UART_INTConfig(UARTx, RUART_IER_ETBEI, DISABLE);
        }
        break;

    case RUART_RECEIVER_DATA_AVAILABLE:
    case RUART_TIME_OUT_INDICATION:
        if (UART_GetRxFlag(uart->dev.idx) == STATERX_INT) {
            uint32_t TransCnt = 0;

            TransCnt = UART_ReceiveDataTO(UARTx, uart->rx_data, uart->rx_size, 1);
            uart->rx_size -= TransCnt;
            uart->rx_data += TransCnt;

            if (uart->rx_size == 0) {
                /* Disable RX Interrupt */
                UART_INTConfig(UARTx, (RUART_IER_ERBI | RUART_IER_ELSI | RUART_IER_ETOI), DISABLE);
                UART_SetRxFlag(uart->dev.idx, 0);
                if (uart->callback != NULL) {
                    uart->callback(uart, UART_EVENT_RECEIVE_COMPLETE, NULL);
                }
            }
        } else {
            /* Call Rx data ready callback */
            RegValue = (UART_LineStatusGet(UARTx));
            if (RegValue & RUART_LINE_STATUS_REG_DR) {
                (void)aos_event_set(&priv->event, EVENT_RX_READY, AOS_EVENT_OR);
                UART_INTConfig(UARTx, (RUART_IER_ERBI | RUART_IER_ELSI | RUART_IER_ETOI), DISABLE);
                UART_SetRxFlag(uart->dev.idx, 0);
            }
        }
        break;

    default:
        printf("Unknown Interrupt \n");
        break;
    }

    return 0;
}

csi_error_t csi_uart_init(csi_uart_t *uart, uint32_t idx)
{
    if (!uart)
        return CSI_ERROR;

    UART_TypeDef *UARTx = UART_DEV_TABLE[idx].UARTx;
    IRQn_Type IrqNum = UART_DEV_TABLE[idx].IrqNum;

    uart->priv = malloc(sizeof(uart_priv_t));

    uart_priv_t *priv = (uart_priv_t *)uart->priv;
    UART_InitTypeDef *UART_InitStruct = &priv->init_data;

    if (aos_event_new(&priv->event, 0)) {
        free(uart->priv);
        uart->priv = NULL;
        return CSI_ERROR;
    }

    uart->dev.idx = idx;
    uart->dev.irq_num = IrqNum;

    /* Configure the UART pins */
    uint32_t uart_idx = uart_idx_get(idx);

    printf("uart_idx:%x\n", uart_idx);

    PinName tx = uart_tx_pin_get(uart_idx, CSI_PinMap_UART_TX);
    PinName rx = uart_rx_pin_get(uart_idx, CSI_PinMap_UART_RX);
    pinmap_pinout(tx, CSI_PinMap_UART_TX);
    pinmap_pinout(rx, CSI_PinMap_UART_RX);
    pin_mode(tx, PullUp);
    pin_mode(rx, PullUp);

    printf("tx:%d,rx:%d\n", tx, rx);

    UART_StructInit(UART_InitStruct);
    UART_Init(UARTx, UART_InitStruct);

    InterruptRegister((IRQ_FUN)uart_irqhandler, IrqNum, (uint32_t)uart, 5);
    InterruptEn(IrqNum, 5);

    return CSI_OK;
}

void csi_uart_uninit(csi_uart_t *uart)
{
    uint32_t uart_idx = uart->dev.idx;
    UART_TypeDef *UARTx = UART_DEV_TABLE[uart_idx].UARTx;
    IRQn_Type IrqNum = UART_DEV_TABLE[uart_idx].IrqNum;
    uart_priv_t *priv = (uart_priv_t *)uart->priv;

    UART_DeInit(UARTx);
    InterruptDis(IrqNum);
    InterruptUnRegister(IrqNum);

    if (uart && uart->priv) {
        aos_event_free(&priv->event);
        free(uart->priv);
        uart->priv = NULL;
    }
}

csi_error_t csi_uart_baud(csi_uart_t *uart, uint32_t baud)
{
    uint32_t uart_idx = uart->dev.idx;
    UART_TypeDef *UARTx = UART_DEV_TABLE[uart_idx].UARTx;

    RCC_PeriphClockSource_UART(UARTx, UART_RX_CLK_XTAL_40M);

    UART_SetBaud(UARTx, baud);
    UART_RxCmd(UARTx, ENABLE);

    if (baud <= 500000) {
        if (uart_config[uart_idx].LOW_POWER_RX_ENABLE) {
            UART_MonitorParaConfig(UARTx, 100, ENABLE);
            UART_RxMonitorCmd(UARTx, ENABLE);
            RCC_PeriphClockSource_UART(UARTx, UART_RX_CLK_OSC_LP);
            UART_LPRxBaudSet(UARTx, baud, 2000000);
            UART_RxCmd(UARTx, ENABLE);
        }
    }
    return CSI_OK;
}

csi_error_t csi_uart_format(csi_uart_t *uart, csi_uart_data_bits_t data_bits, csi_uart_parity_t parity,
                            csi_uart_stop_bits_t stop_bits)
{
    uint32_t uart_idx = uart->dev.idx;
    UART_TypeDef *UARTx = UART_DEV_TABLE[uart_idx].UARTx;
    UART_InitTypeDef *UART_InitStruct = &((uart_priv_t *)uart->priv)->init_data;

    UART_RxCmd(UARTx, DISABLE);

    if (data_bits == UART_DATA_BITS_8) {
        UART_InitStruct->WordLen = RUART_WLS_8BITS;
    } else {
        UART_InitStruct->WordLen = RUART_WLS_7BITS;
    }

    switch (parity) {
    case UART_PARITY_ODD:
        UART_InitStruct->Parity = RUART_PARITY_ENABLE;
        UART_InitStruct->ParityType = RUART_ODD_PARITY;
        break;
    case UART_PARITY_EVEN:
        UART_InitStruct->Parity = RUART_PARITY_ENABLE;
        UART_InitStruct->ParityType = RUART_EVEN_PARITY;
        break;
    default:
        UART_InitStruct->Parity = RUART_PARITY_DISABLE;
        break;
    }

    if (stop_bits == UART_STOP_BITS_2) {
        UART_InitStruct->StopBit = RUART_STOP_BIT_2;
    } else {
        UART_InitStruct->StopBit = RUART_STOP_BIT_1;
    }

    UARTx->LCR = ((UART_InitStruct->WordLen) | (UART_InitStruct->StopBit << 2) | (UART_InitStruct->Parity << 3) |
                  (UART_InitStruct->ParityType << 4) | (UART_InitStruct->StickParity << 5));

    UART_RxCmd(UARTx, ENABLE);

    return CSI_OK;
}

csi_error_t csi_uart_attach_callback(csi_uart_t *uart, void *callback, void *arg)
{
    uint32_t uart_idx = uart->dev.idx;
    UART_TypeDef *UARTx = UART_DEV_TABLE[uart_idx].UARTx;

    uart->callback = callback;

    UART_INTConfig(UARTx, RUART_IER_ERBI | RUART_IER_ELSI | RUART_IER_ETOI, ENABLE);
    UART_INTConfig(UARTx, RUART_IER_ETBEI, ENABLE);

    return CSI_OK;
}

void csi_uart_detach_callback(csi_uart_t *uart)
{
    uart->callback = NULL;
}

int32_t csi_uart_send(csi_uart_t *uart, const void *data, uint32_t size, uint32_t timeout)
{
    uint32_t uart_idx = uart->dev.idx;
    UART_TypeDef *UARTx = UART_DEV_TABLE[uart_idx].UARTx;
    uint32_t cnt = 0;
    uint32_t startcount = SYSTIMER_TickGet();

    uart->tx_size = size;
    while (1) {
        if (UART_Writable(UARTx)) {
            UART_CharPut(UARTx, *(uint8_t *)data);
            data++;
            cnt++;
        } else {
            aos_msleep(1);
        }

        if (cnt == size) {
            break;
        }

        if (SYSTIMER_GetPassTime(startcount) > timeout) {
            break;
        }
    }

    return cnt;
}

int32_t csi_uart_receive(csi_uart_t *uart, void *data, uint32_t size, uint32_t timeout)
{
    uint32_t uart_idx = uart->dev.idx;
    UART_TypeDef *UARTx = UART_DEV_TABLE[uart_idx].UARTx;
    uart_priv_t *priv = (uart_priv_t *)uart->priv;
    uint32_t cnt = 0;
    uint32_t startcount = SYSTIMER_TickGet();

    uart->rx_size = size;

    while (1) {
        if (UART_Readable(UARTx)) {
            UART_CharGet(UARTx, (uint8_t *)data);
            data++;
            cnt++;
        } else {
            uint32_t val;

            if (cnt > 0)
                break;

            UART_INTConfig(UARTx, RUART_IER_ERBI | RUART_IER_ELSI | RUART_IER_ETOI, ENABLE);
            (void)aos_event_get(&priv->event, EVENT_RX_READY, AOS_EVENT_OR_CLEAR, &val, 20);
        }

        if (cnt == size) {
            break;
        }

        if (SYSTIMER_GetPassTime(startcount) > timeout) {
            break;
        }
    }

    UART_INTConfig(UARTx, RUART_IER_ERBI | RUART_IER_ELSI | RUART_IER_ETOI, DISABLE);
    (void)aos_event_set(&priv->event, ~EVENT_RX_READY, AOS_EVENT_AND);

    return cnt;
}

csi_error_t csi_uart_send_async(csi_uart_t *uart, const void *data, uint32_t size)
{
    uint32_t uart_idx = uart->dev.idx;
    UART_TypeDef *UARTx = UART_DEV_TABLE[uart_idx].UARTx;
    int32_t ret = 0;
    int32_t cnt = 16;
    uint8_t *ptxbuf = (uint8_t *)data;

    assert_param(ptxbuf != NULL);
    assert_param(size != 0);

    if (UART_GetTxFlag(uart_idx)) {
        printf("uart int tx: busy\n");
        return CSI_BUSY;
    }
    uart->tx_size = size;
    uart->tx_data = ptxbuf;

    UART_SetTxFlag(uart_idx, STATETX_INT);

    while (cnt > 0 && uart->tx_size > 0) {
        UART_CharPut(UARTx, *uart->tx_data);
        uart->tx_size--;
        uart->tx_data++;
        cnt--;
    }

    if (0 == uart->tx_size) {
        UART_INTConfig(UARTx, RUART_IER_ETBEI, DISABLE);
        UART_SetTxFlag(uart_idx, 0);
        if (uart->callback != NULL) {
            uart->callback(uart, UART_EVENT_SEND_COMPLETE, NULL);
        }
    } else {
        /* Enable Tx FIFO empty interrupt */
        UART_INTConfig(UARTx, RUART_IER_ETBEI, ENABLE);
    }
    return CSI_OK;
}

csi_error_t csi_uart_receive_async(csi_uart_t *uart, void *data, uint32_t size)
{
    uint32_t uart_idx = uart->dev.idx;
    UART_TypeDef *UARTx = UART_DEV_TABLE[uart_idx].UARTx;
    uint32_t TransCnt = 0;
    uint8_t *prxbuf = (uint8_t *)data;

    assert_param(prxbuf != NULL);
    assert_param(size != 0);

    if (UART_GetRxFlag(uart_idx)) {
        printf("uart int rx: busy\n");
        return CSI_BUSY;
    }
    uart->rx_size = size;
    uart->rx_data = (uint8_t *)data;

    UART_SetRxFlag(uart_idx, STATERX_INT);

    /* Could be the RX FIFO has some data already */
    TransCnt = UART_ReceiveDataTO(UARTx, uart->rx_data, uart->rx_size, 1);
    uart->rx_size -= TransCnt;
    uart->rx_data += TransCnt;

    if (uart->rx_size == 0) {
        UART_INTConfig(UARTx, RUART_IER_ERBI | RUART_IER_ELSI | RUART_IER_ETOI, DISABLE);
        UART_SetRxFlag(uart_idx, 0);
        if (uart->callback != NULL) {
            uart->callback(uart, UART_EVENT_RECEIVE_COMPLETE, NULL);
        }
    } else {
        /* Enable RX Interrupt */
        UART_INTConfig(UARTx, RUART_IER_ERBI | RUART_IER_ELSI | RUART_IER_ETOI, ENABLE);
    }

    return CSI_OK;
}

typedef struct {
    aos_tty_t tty;
    UART_InitTypeDef params;
    uint8_t ier;
} tty_uart_t;

static aos_status_t termios2params(const struct termios *termios, UART_InitTypeDef *params, uint32_t *baudrate)
{
    switch (termios->c_cflag & CSIZE) {
    case CS7:
        params->WordLen = RUART_WLS_7BITS;
        break;
    case CS8:
        params->WordLen = RUART_WLS_8BITS;
        break;
    default:
        return -EINVAL;
    }

    if (termios->c_cflag & CSTOPB)
        params->StopBit = RUART_STOP_BIT_2;
    else
        params->StopBit = RUART_STOP_BIT_1;

    if (termios->c_cflag & PARENB) {
        params->Parity = RUART_PARITY_ENABLE;
        params->ParityType = (termios->c_cflag & PARODD) ? RUART_ODD_PARITY : RUART_EVEN_PARITY;
    } else {
        params->Parity = RUART_PARITY_DISABLE;
    }

    switch (termios->c_cflag & CBAUD) {
    case B50:
        *baudrate = 50;
        break;
    case B75:
        *baudrate = 75;
        break;
    case B110:
        *baudrate = 110;
        break;
    case B134:
        *baudrate = 134;
        break;
    case B150:
        *baudrate = 150;
        break;
    case B200:
        *baudrate = 200;
        break;
    case B300:
        *baudrate = 300;
        break;
    case B600:
        *baudrate = 600;
        break;
    case B1200:
        *baudrate = 1200;
        break;
    case B1800:
        *baudrate = 1800;
        break;
    case B2400:
        *baudrate = 2400;
        break;
    case B4800:
        *baudrate = 4800;
        break;
    case B9600:
        *baudrate = 9600;
        break;
    case B19200:
        *baudrate = 19200;
        break;
    case B38400:
        *baudrate = 38400;
        break;
    case B57600:
        *baudrate = 57600;
        break;
    case B115200:
        *baudrate = 115200;
        break;
    case B230400:
        *baudrate = 230400;
        break;
    case B460800:
        *baudrate = 460800;
        break;
    case B500000:
        *baudrate = 500000;
        break;
    case B576000:
        *baudrate = 576000;
        break;
    case B921600:
        *baudrate = 921600;
        break;
    case B1000000:
        *baudrate = 1000000;
        break;
    case B1152000:
        *baudrate = 1152000;
        break;
    case B1500000:
        *baudrate = 1500000;
        break;
    case B2000000:
        *baudrate = 2000000;
        break;
    case B2500000:
        *baudrate = 2500000;
        break;
    case B3000000:
        *baudrate = 3000000;
        break;
    case B3500000:
        *baudrate = 3500000;
        break;
    case B4000000:
        *baudrate = 4000000;
        break;
    default:
        return -EINVAL;
    }

    return 0;
}

static void uart_rx_irq_handler(tty_uart_t *tty_uart)
{
    aos_tty_t *tty = &tty_uart->tty;
    UART_TypeDef *uart = UART_DEV_TABLE[tty->dev.id].UARTx;
    const size_t rx_fifo_depth = 16;
    uint8_t buf[rx_fifo_depth];
    size_t i;

    for (i = 0; i < rx_fifo_depth; i++) {
        if (!UART_Readable(uart))
            break;
        UART_CharGet(uart, &buf[i]);
    }

    if (i > 0)
        (void)aos_tty_rx_buffer_produce(tty, buf, i);
}

static uint32_t uart_irq_handler(void *data)
{
    tty_uart_t *tty_uart = (tty_uart_t *)data;
    aos_tty_t *tty = &tty_uart->tty;
    UART_TypeDef *uart = UART_DEV_TABLE[tty->dev.id].UARTx;
    uint8_t iir;
    uint8_t val;
    aos_irqsave_t flags;

    flags = aos_spin_lock_irqsave(&tty->lock);

    iir = UART_IntStatus(uart);
    if (iir & RUART_IIR_INT_PEND) {
        aos_spin_unlock_irqrestore(&tty->lock, flags);
        return 0;
    }

    switch ((iir & RUART_IIR_INT_ID) >> 1) {
    case RUART_LP_RX_MONITOR_DONE:
        if (!(tty_uart->ier & RUART_IER_EDMI))
            break;
        val = UART_RxMonitorSatusGet(uart);
        break;
    case RUART_MODEM_STATUS:
        if (!(tty_uart->ier & RUART_IER_EDSSI))
            break;
        val = UART_ModemStatusGet(uart);
        break;
    case RUART_RECEIVE_LINE_STATUS:
        if (!(tty_uart->ier & RUART_IER_ELSI))
            break;
        val = UART_LineStatusGet(uart);
        break;
    case RUART_TX_FIFO_EMPTY:
        if (!(tty_uart->ier & RUART_IER_ETBEI))
            break;
        if (UART_LineStatusGet(uart) & RUART_LINE_STATUS_REG_THRE) {
            const size_t tx_fifo_depth = 16;
            uint8_t buf[tx_fifo_depth];
            size_t count = aos_tty_tx_buffer_consume(tty, buf, tx_fifo_depth);
            for (size_t i = 0; i < count; i++)
                UART_CharPut(uart, buf[i]);
            if (count == 0) {
                tty_uart->ier &= ~RUART_IER_ETBEI;
                UART_INTConfig(uart, RUART_IER_ETBEI, DISABLE);
            }
        }
        break;
    case RUART_RECEIVER_DATA_AVAILABLE:
        if (!(tty_uart->ier & RUART_IER_ERBI))
            break;
        uart_rx_irq_handler(tty_uart);
        break;
    case RUART_TIME_OUT_INDICATION:
        if (!(tty_uart->ier & RUART_IER_ETOI))
            break;
        uart_rx_irq_handler(tty_uart);
        break;
    default:
        break;
    }

    aos_spin_unlock_irqrestore(&tty->lock, flags);

    return 0;
}

static void uart_unregister(aos_tty_t *tty)
{
}

static aos_status_t uart_startup(aos_tty_t *tty)
{
    tty_uart_t *tty_uart = aos_container_of(tty, tty_uart_t, tty);
    UART_TypeDef *uart = UART_DEV_TABLE[tty->dev.id].UARTx;
    IRQn_Type irq = UART_DEV_TABLE[tty->dev.id].IrqNum;
    uint32_t uart_idx = uart_idx_get(tty->dev.id);
    PinName tx_pin = uart_tx_pin_get(uart_idx, CSI_PinMap_UART_TX);
    PinName rx_pin = uart_rx_pin_get(uart_idx, CSI_PinMap_UART_RX);
    UART_InitTypeDef params;
    uint32_t baudrate;
    aos_status_t ret;

    UART_StructInit(&params);
    ret = termios2params(&tty->termios, &params, &baudrate);
    if (ret)
        return ret;

    tty_uart->params = params;
    pinmap_pinout(tx_pin, CSI_PinMap_UART_TX);
    pinmap_pinout(rx_pin, CSI_PinMap_UART_RX);
    pin_mode(tx_pin, PullUp);
    pin_mode(rx_pin, PullUp);
    UART_Init(uart, &params);
    UART_SetRxLevel(uart, UART_RX_FIFOTRIG_LEVEL_8BYTES);
    uart->LCR = (params.WordLen << 0) | (params.StopBit << 2) | (params.Parity << 3) |
                (params.ParityType << 4) | (params.StickParity << 5);
    RCC_PeriphClockSource_UART(uart, UART_RX_CLK_XTAL_40M);
    UART_SetBaud(uart, baudrate);

    if (baudrate <= 500000 && uart_config[tty->dev.id].LOW_POWER_RX_ENABLE) {
        UART_MonitorParaConfig(uart, 100, ENABLE);
        UART_RxMonitorCmd(uart, ENABLE);
        RCC_PeriphClockSource_UART(uart, UART_RX_CLK_OSC_LP);
        UART_LPRxBaudSet(uart, baudrate, 2000000);
    }

    tty_uart->ier = 0;
    InterruptRegister(uart_irq_handler, irq, (uint32_t)tty_uart, 5);
    InterruptEn(irq, 5);

    return 0;
}

static void uart_shutdown(aos_tty_t *tty)
{
    UART_TypeDef *uart = UART_DEV_TABLE[tty->dev.id].UARTx;
    IRQn_Type irq = UART_DEV_TABLE[tty->dev.id].IrqNum;

    InterruptDis(irq);
    InterruptUnRegister(irq);
    UART_DeInit(uart);
}

static aos_status_t uart_set_attr(aos_tty_t *tty)
{
    tty_uart_t *tty_uart = aos_container_of(tty, tty_uart_t, tty);
    UART_TypeDef *uart = UART_DEV_TABLE[tty->dev.id].UARTx;
    UART_InitTypeDef params = tty_uart->params;
    uint32_t baudrate;
    aos_status_t ret;

    ret = termios2params(&tty->termios, &params, &baudrate);
    if (ret)
        return ret;

    tty_uart->params = params;
    uart->LCR = (params.WordLen << 0) | (params.StopBit << 2) | (params.Parity << 3) |
                (params.ParityType << 4) | (params.StickParity << 5);
    RCC_PeriphClockSource_UART(uart, UART_RX_CLK_XTAL_40M);
    UART_SetBaud(uart, baudrate);

    if (baudrate <= 500000 && uart_config[tty->dev.id].LOW_POWER_RX_ENABLE) {
        UART_MonitorParaConfig(uart, 100, ENABLE);
        UART_RxMonitorCmd(uart, ENABLE);
        RCC_PeriphClockSource_UART(uart, UART_RX_CLK_OSC_LP);
        UART_LPRxBaudSet(uart, baudrate, 2000000);
    }

    return 0;
}

static void uart_enable_rx(aos_tty_t *tty)
{
    tty_uart_t *tty_uart = aos_container_of(tty, tty_uart_t, tty);
    UART_TypeDef *uart = UART_DEV_TABLE[tty->dev.id].UARTx;
    aos_irqsave_t flags;

    UART_RxCmd(uart, ENABLE);
    flags = aos_spin_lock_irqsave(&tty->lock);
    tty_uart->ier |= RUART_IER_ERBI | RUART_IER_ELSI | RUART_IER_ETOI;
    UART_INTConfig(uart, RUART_IER_ERBI | RUART_IER_ELSI | RUART_IER_ETOI, ENABLE);
    aos_spin_unlock_irqrestore(&tty->lock, flags);
}

static void uart_disable_rx(aos_tty_t *tty)
{
    tty_uart_t *tty_uart = aos_container_of(tty, tty_uart_t, tty);
    UART_TypeDef *uart = UART_DEV_TABLE[tty->dev.id].UARTx;
    aos_irqsave_t flags;

    flags = aos_spin_lock_irqsave(&tty->lock);
    tty_uart->ier &= ~(RUART_IER_ERBI | RUART_IER_ELSI | RUART_IER_ETOI);
    UART_INTConfig(uart, RUART_IER_ERBI | RUART_IER_ELSI | RUART_IER_ETOI, DISABLE);
    aos_spin_unlock_irqrestore(&tty->lock, flags);
    UART_RxCmd(uart, DISABLE);
    UART_ClearRxFifo(uart);
}

static void uart_start_tx(aos_tty_t *tty)
{
    tty_uart_t *tty_uart = aos_container_of(tty, tty_uart_t, tty);
    UART_TypeDef *uart = UART_DEV_TABLE[tty->dev.id].UARTx;

    tty_uart->ier |= RUART_IER_ETBEI;
    UART_INTConfig(uart, RUART_IER_ETBEI, ENABLE);
}

static void uart_stop_tx(aos_tty_t *tty)
{
    tty_uart_t *tty_uart = aos_container_of(tty, tty_uart_t, tty);
    UART_TypeDef *uart = UART_DEV_TABLE[tty->dev.id].UARTx;
    aos_irqsave_t flags;

    flags = aos_spin_lock_irqsave(&tty->lock);
    tty_uart->ier &= ~RUART_IER_ETBEI;
    UART_INTConfig(uart, RUART_IER_ETBEI, DISABLE);
    aos_spin_unlock_irqrestore(&tty->lock, flags);

    for (int i = 0; i < 10; i++) {
        if (UART_LineStatusGet(uart) & RUART_LINE_STATUS_REG_TEMT)
            break;
        aos_msleep(20);
    }
}

static const aos_tty_ops_t tty_uart_ops = {
    .unregister = uart_unregister,
    .startup    = uart_startup,
    .shutdown   = uart_shutdown,
    .set_attr   = uart_set_attr,
    .enable_rx  = uart_enable_rx,
    .disable_rx = uart_disable_rx,
    .start_tx   = uart_start_tx,
    .stop_tx    = uart_stop_tx,
};

static aos_status_t tty_uart_register(tty_uart_t *tty_uart)
{
    if (!tty_uart)
        return -EINVAL;

    tty_uart->tty.ops = &tty_uart_ops;
    tty_uart->tty.flags = 0;

    return aos_tty_register(&tty_uart->tty);
}

static aos_status_t tty_uart_unregister(uint32_t id)
{
    return aos_tty_unregister(id);
}

static int tty_uart_init(void)
{
    static tty_uart_t tty_uarts[4];
    int ret;

    tty_uarts[0].tty.dev.id = 0;
    ret = (int)tty_uart_register(&tty_uarts[0]);
    if (ret)
        return ret;

    tty_uarts[3].tty.dev.id = 3;
    ret = (int)tty_uart_register(&tty_uarts[3]);
    if (ret) {
        (void)tty_uart_unregister(0);
        return ret;
    }

    return 0;
}

LEVEL1_DRIVER_ENTRY(tty_uart_init)
