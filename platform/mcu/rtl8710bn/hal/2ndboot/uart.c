/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include "2ndboot.h"
#include "serial_api.h"

#define UART_BUFF_LEN 1032

static unsigned char uart_buf[UART_BUFF_LEN];
static unsigned int  uart_buf_write_index = 0;
static unsigned int  uart_buf_read_index  = 0;

void uart_printf(const char *fmt, ...)
{
    (void)VSprintf(0, fmt, ((const int *)&fmt)+1);
    return;
}

static void uart_log_irq()
{
    unsigned char nc   = 0;
    unsigned int IrqEn = DiagGetIsrEnReg();

    DiagSetIsrEnReg(0);

    nc = DiagGetChar(0);

    uart_buf[uart_buf_write_index] = nc;
    uart_buf_write_index ++;
    if(uart_buf_write_index == UART_BUFF_LEN) {
        uart_buf_write_index = 0;
    }
    DiagSetIsrEnReg(IrqEn);
}

static int uart0_recv_byte(unsigned char *c)
{
    unsigned int IrqEn = DiagGetIsrEnReg();
    DiagSetIsrEnReg(0);
    if(uart_buf_read_index != uart_buf_write_index) {
        *c = uart_buf[uart_buf_read_index];
        uart_buf_read_index ++;
        if(uart_buf_read_index == UART_BUFF_LEN) {
            uart_buf_read_index = 0;
        }
        DiagSetIsrEnReg(IrqEn);
        return 1;
    }
    DiagSetIsrEnReg(IrqEn);
    return 0;
}

unsigned char uart_recv_byte(unsigned char *c)
{
    unsigned char tc = 0;
    if(uart0_recv_byte(&tc)) {
        *c = tc;
        return 1;
    }
    return 0;
}

void uart_init()
{
    memset(uart_buf, 0xFF, UART_BUFF_LEN);
    DIAG_UartReInit((IRQ_FUN) uart_log_irq);
}

void uart_send_byte(unsigned char buff)
{
    DiagPutChar(buff);
}

void uart_send_string(char *buff)
{
    int i;

    if(NULL == buff) {
        return;
    }
    uart_printf(buff);
    return;
}

