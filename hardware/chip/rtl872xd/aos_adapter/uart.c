/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <objects.h>
#include <pinmap.h>
#include <drv/uart.h>
#include <aos/tty_core.h>

typedef struct {
    aos_tty_t tty;
    UART_InitTypeDef params;
    uint8_t ier;
} tty_uart_t;

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
