/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include "bootloader.h"
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

int32_t hal_uart_init(uart_dev_t *uart)
{
    memset(uart_buf, 0xFF, UART_BUFF_LEN);
    (void)DIAG_UartReInit((IRQ_FUN) uart_log_irq);

    return 0;
}

int32_t hal_uart_send(uart_dev_t *uart, const void *data, uint32_t size, uint32_t timeout)
{
    int i = 0;
    char* buff = NULL;

    if (data == NULL) {
        return -1;
    }

    if (size == 0) {
        return -1;
    }

    buff = (char*)data;

    for (i = 0; i < size; i++) {
        DiagPutChar(*(buff + i));
    }

    return 0;
}

int32_t hal_uart_recv_II(uart_dev_t *uart, void *data, uint32_t expect_size, uint32_t *recv_size, uint32_t timeout)
{
    char         *buff = NULL;
    uint32_t     size = 0;
    unsigned int IrqEn = DiagGetIsrEnReg();

    if ((data == NULL) || (recv_size == NULL)) {
        return -1;
    }

    if (expect_size == 0) {
        return -1;
    }

    buff = data;

    DiagSetIsrEnReg(0);

    while (uart_buf_read_index != uart_buf_write_index) {
        *buff = uart_buf[uart_buf_read_index];
        uart_buf_read_index ++;
        if(uart_buf_read_index == UART_BUFF_LEN) {
            uart_buf_read_index = 0;
        }

        size++;
        buff++;
        if (size == expect_size) {
            break;
        }
    }

    *recv_size = size;
    DiagSetIsrEnReg(IrqEn);
    return 0;
}

