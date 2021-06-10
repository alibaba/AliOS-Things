/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include "objects.h"
#include "pinmap.h"
#include <string.h>
#include <drv/uart.h>
#include <aos/tty_csi.h>

static const PinMap CSI_PinMap_UART_TX[] = {
    {PA_12,     UART_3,        PIN_DATA(PullUp, PINMUX_FUNCTION_UART)},
    {PA_18,     UART_0,        PIN_DATA(PullUp , PINMUX_FUNCTION_UART)},
    {PA_7,      UART_2,        PIN_DATA(PullUp, PINMUX_FUNCTION_LOGUART)},
    {NC,        NC,            0}
};

static const PinMap CSI_PinMap_UART_RX[] = {
    {PA_13,     UART_3,        PIN_DATA(PullUp, PINMUX_FUNCTION_UART)},
    {PA_19,     UART_0,        PIN_DATA(PullUp, PINMUX_FUNCTION_UART)},
    {PA_8,      UART_2,        PIN_DATA(PullUp, PINMUX_FUNCTION_LOGUART)},
    {NC,        NC,            0}
};

static PinName uart_tx_pin_get(uint32_t uart_idx, const PinMap* map)
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

static PinName uart_rx_pin_get(uint32_t uart_idx, const PinMap* map)
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
    if(idx == 0)
        return UART_0;
    else if(idx == 2)
        return UART_2;
    else if(idx == 3)
        return UART_3;
    else
        assert_param(0);
    return UART_1;
}

static uint32_t
uart_irqhandler(
        IN VOID *Data
)
{
    volatile uint8_t reg_iir;
    uint8_t IntId;
    uint32_t RegValue;

    csi_uart_t *uart = (csi_uart_t *) Data;
    UART_TypeDef* UARTx = UART_DEV_TABLE[uart->dev.idx].UARTx;

    reg_iir = UART_IntStatus(UARTx);
    if ((reg_iir & RUART_IIR_INT_PEND) != 0) {
        // No pending IRQ
        return 0;
    }

    IntId = (reg_iir & RUART_IIR_INT_ID) >> 1;

    switch (IntId) {
    case RUART_LP_RX_MONITOR_DONE:
        RegValue = UART_RxMonitorSatusGet(UARTx);
    break;

    case RUART_MODEM_STATUS:
        RegValue =  UART_ModemStatusGet(UARTx);
    break;

    case RUART_RECEIVE_LINE_STATUS:
        RegValue = UART_LineStatusGet(UARTx);
    break;

    case RUART_TX_FIFO_EMPTY:
        if (UART_GetTxFlag(uart->dev.idx)) {
            int32_t cnt=16;
            while(cnt>0&&uart->tx_size>0){
                UART_CharPut(UARTx, *uart->tx_data);
                uart->tx_size --;
                uart->tx_data ++;
                cnt--;
            }

            if (0 == uart->tx_size) {
                // Mask UART TX FIFO empty
                UART_INTConfig(UARTx, RUART_IER_ETBEI, DISABLE);
                UART_SetTxFlag(uart->dev.idx, 0);
                if(uart->callback != NULL) {
                    uart->callback(uart,UART_EVENT_SEND_COMPLETE,NULL);
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

            TransCnt = UART_ReceiveDataTO(UARTx, uart->rx_data,
                uart->rx_size, 1);
            uart->rx_size -= TransCnt;
            uart->rx_data += TransCnt;

            if (uart->rx_size == 0) {
                // Disable RX Interrupt
                UART_INTConfig(UARTx, (RUART_IER_ERBI | RUART_IER_ELSI | RUART_IER_ETOI), DISABLE);
                UART_SetRxFlag(uart->dev.idx, 0);
                if(uart->callback != NULL){
                    uart->callback(uart,UART_EVENT_RECEIVE_COMPLETE,NULL);
                }
            }
        } else {
            // Call Rx data ready callback
            RegValue = (UART_LineStatusGet(UARTx));
            if (RegValue & RUART_LINE_STATUS_REG_DR) {
                //
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
    if(!uart)
        return CSI_ERROR;

    UART_TypeDef* UARTx = UART_DEV_TABLE[idx].UARTx;
    IRQn_Type   IrqNum = UART_DEV_TABLE[idx].IrqNum;

    uart->priv = (UART_InitTypeDef*)malloc(sizeof(UART_InitTypeDef));

    UART_InitTypeDef* UART_InitStruct = (UART_InitTypeDef*)uart->priv;

    uart->dev.idx = idx;
    uart->dev.irq_num = IrqNum;

    /* Configure the UART pins */
    uint32_t uart_idx = uart_idx_get(idx);

    printf("uart_idx:%x\n",uart_idx);

    PinName tx = uart_tx_pin_get(uart_idx,CSI_PinMap_UART_TX);
    PinName rx = uart_rx_pin_get(uart_idx,CSI_PinMap_UART_RX);
    pinmap_pinout(tx, CSI_PinMap_UART_TX);
    pinmap_pinout(rx, CSI_PinMap_UART_RX);
    pin_mode(tx, PullUp);
    pin_mode(rx, PullUp);

    printf("tx:%d,rx:%d\n",tx,rx);

    UART_StructInit(UART_InitStruct);
    UART_Init(UARTx, UART_InitStruct);

    InterruptRegister((IRQ_FUN)uart_irqhandler, IrqNum, (uint32_t)uart, 5);
    InterruptEn(IrqNum, 5);

    return CSI_OK;
}

void csi_uart_uninit(csi_uart_t *uart)
{
    uint32_t uart_idx = uart->dev.idx;
    UART_TypeDef* UARTx = UART_DEV_TABLE[uart_idx].UARTx;
    IRQn_Type   IrqNum = UART_DEV_TABLE[uart_idx].IrqNum;

    UART_DeInit(UARTx);
    InterruptDis(IrqNum);
    InterruptUnRegister(IrqNum);

    if(uart && uart->priv){
        free(uart->priv);
        uart->priv = NULL;
    }
}

csi_error_t csi_uart_baud(csi_uart_t *uart, uint32_t baud)
{
    uint32_t uart_idx = uart->dev.idx;
    UART_TypeDef* UARTx = UART_DEV_TABLE[uart_idx].UARTx;

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

csi_error_t csi_uart_format(csi_uart_t *uart,  csi_uart_data_bits_t data_bits, csi_uart_parity_t parity, csi_uart_stop_bits_t stop_bits)
{
    uint32_t uart_idx = uart->dev.idx;
    UART_TypeDef* UARTx = UART_DEV_TABLE[uart_idx].UARTx;
    UART_InitTypeDef* UART_InitStruct = (UART_InitTypeDef*)uart->priv;

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
        default: // ParityNone
            UART_InitStruct->Parity = RUART_PARITY_DISABLE;
        break;
    }

    if (stop_bits == UART_STOP_BITS_2) {
        UART_InitStruct->StopBit = RUART_STOP_BIT_2;
    } else {
        UART_InitStruct->StopBit = RUART_STOP_BIT_1;
    }

    UARTx->LCR = ((UART_InitStruct->WordLen) |
        (UART_InitStruct->StopBit << 2) |
        (UART_InitStruct->Parity << 3) |
        (UART_InitStruct->ParityType << 4) |
        (UART_InitStruct->StickParity << 5));

    UART_RxCmd(UARTx, ENABLE);
    return CSI_OK;
}

csi_error_t csi_uart_attach_callback(csi_uart_t *uart, void *callback, void *arg)
{
    uint32_t uart_idx = uart->dev.idx;
    UART_TypeDef* UARTx = UART_DEV_TABLE[uart_idx].UARTx;

    uart->callback = callback;

    UART_INTConfig(UARTx, RUART_IER_ERBI | RUART_IER_ELSI | RUART_IER_ETOI, ENABLE);
    UART_INTConfig(UARTx, RUART_IER_ETBEI,ENABLE);

    return CSI_OK;
}

void csi_uart_detach_callback(csi_uart_t *uart)
{
    uart->callback = NULL;
}

int32_t csi_uart_send(csi_uart_t *uart, const void *data, uint32_t size, uint32_t timeout)
{
    uint32_t uart_idx = uart->dev.idx;
    UART_TypeDef* UARTx = UART_DEV_TABLE[uart_idx].UARTx;
    uint32_t cnt = 0;
    uint32_t startcount = SYSTIMER_TickGet();

    uart->tx_size = size;
    while (1) {
        if(UART_Writable(UARTx)) {
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
    UART_TypeDef* UARTx = UART_DEV_TABLE[uart_idx].UARTx;
    uint32_t cnt = 0;
    uint32_t startcount = SYSTIMER_TickGet();

    uart->rx_size = size;
    while (1) {
        if(UART_Readable(UARTx)) {
            UART_CharGet(UARTx, (uint8_t *)data);
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

csi_error_t csi_uart_send_async(csi_uart_t *uart, const void *data, uint32_t size)
{
    uint32_t uart_idx = uart->dev.idx;
    UART_TypeDef* UARTx = UART_DEV_TABLE[uart_idx].UARTx;
    int32_t ret = 0;
    int32_t cnt = 16;
    uint8_t* ptxbuf = (uint8_t*)data;

    assert_param(ptxbuf != NULL);
    assert_param(size != 0);

    if (UART_GetTxFlag(uart_idx)) {
        printf("uart int tx: busy\n");
        return CSI_BUSY;
    }
    uart->tx_size = size;
    uart->tx_data = ptxbuf;

    UART_SetTxFlag(uart_idx, STATETX_INT);

    while(cnt>0&&uart->tx_size>0){
        UART_CharPut(UARTx, *uart->tx_data);
        uart->tx_size --;
        uart->tx_data ++;
        cnt--;
    }

    if (0 == uart->tx_size) {
        UART_INTConfig(UARTx, RUART_IER_ETBEI, DISABLE);
        UART_SetTxFlag(uart_idx, 0);
        if(uart->callback != NULL){
            uart->callback(uart,UART_EVENT_SEND_COMPLETE,NULL);
        }
    } else {
        // Enable Tx FIFO empty interrupt
        UART_INTConfig(UARTx, RUART_IER_ETBEI, ENABLE);
    }
    return CSI_OK;
}

csi_error_t csi_uart_receive_async(csi_uart_t *uart, void *data, uint32_t size)
{
    uint32_t uart_idx = uart->dev.idx;
    UART_TypeDef* UARTx = UART_DEV_TABLE[uart_idx].UARTx;
    uint32_t TransCnt = 0;
    uint8_t* prxbuf = (uint8_t*)data;

    assert_param(prxbuf != NULL);
    assert_param(size != 0);

    if (UART_GetRxFlag(uart_idx)) {
        printf("uart int rx: busy\n");
        return CSI_BUSY;
    }
    uart->rx_size = size;
    uart->rx_data = (uint8_t*)data;

    UART_SetRxFlag(uart_idx, STATERX_INT);

    // Could be the RX FIFO has some data already
    TransCnt = UART_ReceiveDataTO(UARTx, uart->rx_data, uart->rx_size, 1);
    uart->rx_size -= TransCnt;
    uart->rx_data += TransCnt;

    if (uart->rx_size == 0) {
        UART_INTConfig(UARTx, RUART_IER_ERBI | RUART_IER_ELSI | RUART_IER_ETOI, DISABLE);
        UART_SetRxFlag(uart_idx, 0);
        if(uart->callback != NULL){
            uart->callback(uart,UART_EVENT_RECEIVE_COMPLETE,NULL);
        }
    } else {
        // Enable RX Interrupt
        UART_INTConfig(UARTx, RUART_IER_ERBI | RUART_IER_ELSI | RUART_IER_ETOI, ENABLE);
    }
    return CSI_OK;
}

static int tty_csi_init(void)
{
    static aos_tty_csi_t tty_csi[2];
    int ret;

    tty_csi[0].tty.dev.id = 0;
    ret = (int)aos_tty_csi_register(&tty_csi[0]);
    if (ret)
        return ret;

    tty_csi[1].tty.dev.id = 3;
    ret = (int)aos_tty_csi_register(&tty_csi[1]);
    if (ret) {
        (void)aos_tty_csi_unregister(0);
        return ret;
    }

    return 0;
}

LEVEL1_DRIVER_ENTRY(tty_csi_init)
