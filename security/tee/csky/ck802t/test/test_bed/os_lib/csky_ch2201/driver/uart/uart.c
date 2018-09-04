/**
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "uart.h"

volatile uint32_t *uart_base = (volatile uint32_t *)CK_CONS_UART_BASE;

uint8_t ck_uart_getchar(void)
{
    uint8_t tmp = 0;

    while (!(uart_base[CK_UART_LSR] & LSR_DATA_READY))
        ;

    tmp = uart_base[CK_UART_RBR];
    return tmp;
}

void ck_uart_putchar(uint8_t ch)
{
    /* wait uart available */
    while ((!(uart_base[CK_UART_LSR] & CK_LSR_TRANS_EMPTY)))
        ;

    if (ch == '\n') {
        uart_base[CK_UART_THR] = '\r';
        /* wait again because we will send '\n' later */
        while ((!(uart_base[CK_UART_LSR] & CK_LSR_TRANS_EMPTY)))
            ;
    }
    uart_base[CK_UART_THR] = ch;
}
