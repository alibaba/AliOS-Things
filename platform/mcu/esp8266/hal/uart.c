/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdint.h>
#include "aos/hal/uart.h"
#include "k_api.h"
#include "driver/uart.h"

int32_t uart_receive_buffer_queue(uint8_t *data);
#define UART0_BUF_QUEUE_BYTES 128

kbuf_queue_t g_buf_queue_uart;
char g_buf_uart[UART0_BUF_QUEUE_BYTES];

int32_t hal_uart_send(uart_dev_t *uart, const void *data, uint32_t size, uint32_t timeout)
{
    int i = 0;
    char* pdata = (char *)data;

    for(i = 0; i < size; i++)
    {
        uart0_write_char(pdata[i]);
    }

    return 0;
}

int32_t hal_uart_recv_II(uart_dev_t *uart, void *data, uint32_t expect_size,
                      uint32_t *recv_size, uint32_t timeout)
{
    uint8_t *pdata = (uint8_t *)data;
    int i = 0;
    uint32_t rx_count = 0;
    int32_t ret = -1;

    if ((uart == NULL) || (data == NULL)) {
        return -1;
    }

    for (i = 0; i < expect_size; i++)
    {
        ret = uart_receive_buffer_queue(&pdata[i]);
        if (ret == 0) {
            rx_count++;
        } else {
            break;
        }
    }
    if(recv_size)
    {
        *recv_size = rx_count;
    }

    if(rx_count != 0)
    {
        ret = 0;
    }
    else
    {
        ret = -1;
    }

    return ret;
}

int32_t hal_uart_init(uart_dev_t *uart)
{
    int32_t ret = -1;

    if (uart == NULL) 
    {
        return -1;
    }

    uart_init_new(uart);
    krhino_buf_queue_create(&g_buf_queue_uart, "buffer_queue_uart0", g_buf_uart, UART0_BUF_QUEUE_BYTES,1);
    ret = 0;

    return ret;
}

int32_t hal_uart_finalize(uart_dev_t *uart)
{
    return 0;
}


int32_t uart_receive_buffer_queue(uint8_t *data)
{
    size_t rev_size = 0;
    int32_t ret = 0;
    char uart_data = 0;

    ret = krhino_buf_queue_recv(&g_buf_queue_uart, 0x10, data, &rev_size);

    return ret;
}
