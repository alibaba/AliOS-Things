/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "sys/xip.h"
#include "sys/dbgcon.h"

#include "soc_defs.h"

#include "drv_uart.h"

void uart_rx_isr(void);

void board_init(void)
{
    xip_init();
    xip_enter();
    flash_init();

    //dbgcon_init(UART_SPR_BAUD_921600);
    drv_uart_init();
    drv_uart_set_fifo(UART_INT_RXFIFO_TRGLVL_1, 0x0);
    drv_uart_set_format(921600, UART_WORD_LEN_8, UART_STOP_BIT_1, UART_PARITY_DISABLE);
    drv_uart_register_isr(UART_DATA_RDY_IE, uart_rx_isr);
}
