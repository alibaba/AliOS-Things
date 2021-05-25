/**
 * Copyright (c) 2015, Realsil Semiconductor Corporation. All rights reserved.
 *
 */

#include <stdio.h>
#include <stdarg.h>
#include "trace_uart.h"
#include "bt_board.h"

extern uint8_t  hci_tp_lgc_efuse[];

typedef struct _TraceUartBuffer
{
    uint8_t  *tx_buffer;
    uint16_t tx_len;
    uint8_t  tx_busy;
    UART_TX_CB  tx_cb;

    bool     tx_switch;
    GDMA_InitTypeDef    UARTTxGdmaInitStruct;
} TRACE_UART_INFO;

TRACE_UART_INFO   g_uart_obj;

#ifdef TRACE_UART_TX_IRQ
static inline void traceuart_stop_tx(TRACE_UART_INFO *trace_pointer)
{
    if (trace_pointer->tx_busy)
    {
        trace_pointer->tx_busy = 0;
        UART_INTConfig(TRACE_UART_DEV, RUART_IER_ETBEI, DISABLE);
    }
}

static inline void transmit_log(TRACE_UART_INFO *trace_pointer)
{
    int count;
    if (trace_pointer->tx_len == 0)
    {
        traceuart_stop_tx(trace_pointer);
        if(trace_pointer->tx_cb)
            trace_pointer->tx_cb();
        return;
    }

    count = TRACE_COUNT;
    do
    {
        UART_CharPut(TRACE_UART_DEV,*trace_pointer->tx_buffer);
        trace_pointer->tx_buffer++;
        trace_pointer->tx_len--;
    }
    while (--count > 0 && trace_pointer->tx_len > 0);
}

uint32_t traceuart_irq(void *data)
{
    volatile uint8_t reg_iir;
    uint8_t int_id;
    uint32_t reg_val;
    TRACE_UART_INFO *trace_pointer = (TRACE_UART_INFO *) data;

    reg_iir = UART_IntStatus(TRACE_UART_DEV);
    if ((reg_iir & RUART_IIR_INT_PEND) != 0)
    {
        /* No pending IRQ */
        return 0;
    }

    int_id = (reg_iir & RUART_IIR_INT_ID) >> 1;

    switch (int_id)
    {
        case RUART_TX_FIFO_EMPTY:
            transmit_log(trace_pointer);
            break;
        case RUART_RECEIVE_LINE_STATUS:
            reg_val = (UART_LineStatusGet(TRACE_UART_DEV));
            platform_debug("traceuart_irq: LSR interrupt, reg_val=%x\r\n",reg_val);
            break;
        default:
            platform_debug("traceuart_irq: Unknown interrupt type %x", int_id);
            break;
    }

    return 0;
}
#endif

bool trace_uart_init(void)
{
    if(!CHECK_SW(EFUSE_SW_TRACE_SWITCH))
    {
        //0
    platform_debug("trace_uart_init:  TRACE OPEN");
    g_uart_obj.tx_switch = true;

    //gloabal_init
    g_uart_obj.tx_busy = 0;

    Pinmux_Config(TRACE_UART_TX, PINMUX_FUNCTION_UART);
    //  Pinmux_Config(TRACE_UART_RX, PINMUX_FUNCTION_UART)

    PAD_PullCtrl(TRACE_UART_TX, GPIO_PuPd_UP);
    // PAD_PullCtrl(TRACE_UART_RX, GPIO_PuPd_UP);

    UART_InitTypeDef    UARTStruct;
    UART_InitTypeDef    *pUARTStruct = &UARTStruct;

    UART_StructInit(pUARTStruct);
    pUARTStruct->WordLen = RUART_WLS_8BITS;
    pUARTStruct->StopBit = RUART_STOP_BIT_1;
    pUARTStruct->Parity = RUART_PARITY_DISABLE;
    pUARTStruct->ParityType = RUART_EVEN_PARITY;
    pUARTStruct->StickParity = RUART_STICK_PARITY_DISABLE;

    UART_Init(TRACE_UART_DEV, pUARTStruct);
    UART_SetBaud(TRACE_UART_DEV, TRACE_UART_BAUDRATE);
    UART_RxCmd(TRACE_UART_DEV, DISABLE);


#ifdef TRACE_UART_TX_IRQ
    InterruptDis(TRACE_UART_IRQ);
    InterruptUnRegister(TRACE_UART_IRQ);
    InterruptRegister((IRQ_FUN)traceuart_irq,
            TRACE_UART_IRQ, (uint32_t)&g_uart_obj,
            TRACEUART_IRQ_PRIO);
    InterruptEn(TRACE_UART_IRQ, TRACEUART_IRQ_PRIO);
#endif
    }
    else
    {
        //default 1 close
        g_uart_obj.tx_switch = false;
    }

    return true;
}

bool trace_uart_deinit(void)
{

    if(!CHECK_SW(EFUSE_SW_TRACE_SWITCH))
    {
        if (g_uart_obj.tx_switch == true) {
            //has already close
            UART_DeInit(TRACE_UART_DEV);
#ifdef TRACE_UART_TX_IRQ
            InterruptDis(TRACE_UART_IRQ);
            InterruptUnRegister(TRACE_UART_IRQ);
#endif
            g_uart_obj.tx_switch = false;
            return true;
        }
        else {
            hci_board_debug("\r\n:trace_uart_deinit: no need\r\n");
            return false;
        }
    }
    return true;
}



#ifdef TRACE_UART_DMA
static uint32_t traceuart_dma_tx_complete(void *data)
{
    TRACE_UART_INFO *t = (TRACE_UART_INFO *)data;
    PGDMA_InitTypeDef GDMA_InitStruct;
    uint8_t  IsrTypeMap;

    platform_debug("TRACE DMA Tx complete ISR");

    GDMA_InitStruct = &t->UARTTxGdmaInitStruct;

    GDMA_Cmd(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, DISABLE);
    IsrTypeMap = GDMA_ClearINT(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum);
    GDMA_ChnlFree(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum);
    UART_TXDMACmd(TRACE_UART_DEV, DISABLE);
    UART_RXDMACmd(TRACE_UART_DEV, DISABLE);
    /* Wake task to continue tx */
    t->tx_busy = 0;
    if(t->tx_cb)
        t->tx_cb();
    return 0;
}
#endif
bool trace_uart_tx(uint8_t *pstr,uint16_t len, UART_TX_CB tx_cb)
{
    if(g_uart_obj.tx_switch == false)
    {
        //printf("\r\n trace_uart_deinit has been called please call init \r\n");
        if(tx_cb)
            tx_cb();
        return true;
    }

#ifdef TRACE_UART_TX_WHILE
    UART_SendData(TRACE_UART_DEV, pstr, len);
    if(tx_cb)
        tx_cb();
#else

    TRACE_UART_INFO   *p_uart_obj= &g_uart_obj;

    if (!(p_uart_obj->tx_busy))
    {
        p_uart_obj->tx_busy = 1;
#ifdef TRACE_UART_TX_IRQ
        p_uart_obj->tx_buffer = pstr;
        p_uart_obj->tx_len  = len;
        p_uart_obj->tx_cb   = tx_cb;
        UART_INTConfig(TRACE_UART_DEV, RUART_IER_ETBEI, ENABLE);
#endif

#ifdef TRACE_UART_DMA
        int ret;
        UART_TXDMAConfig(TRACE_UART_DEV, 8);
        UART_TXDMACmd(TRACE_UART_DEV, ENABLE);
        ret = UART_TXGDMA_Init(TRACE_UART_INDEX, &p_uart_obj->UARTTxGdmaInitStruct,(void *) p_uart_obj, traceuart_dma_tx_complete,pstr,len);
        NVIC_SetPriority(GDMA_GetIrqNum(0, p_uart_obj->UARTTxGdmaInitStruct.GDMA_ChNum), TRACEUART_DMA_PRIO);

        if (!ret ) {
            platform_debug("%s Error(%d)\n", __FUNCTION__, ret);
            p_uart_obj->tx_busy = 0;
        }

#endif
    }
    else
    {
        platform_debug("!!!TX not finished, can't send");
        return false;
    }

#endif
    return true;
}

void bt_trace_set_switch(bool flag)
{
    g_uart_obj.tx_switch = flag;
}


