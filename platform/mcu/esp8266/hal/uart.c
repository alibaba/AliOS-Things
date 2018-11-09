/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdint.h>
#include <hal.h>
#include "k_api.h"
#define uart_dev_t aos_uart_dev_t
#define uart_config_t aos_uart_config_t
#include <hal/soc/uart.h>
#undef uart_dev_t
#undef uart_config_t
#include <hal/soc/uart.h>
int32_t uart_receive_buffer_queue(uint8_t *data);

extern void uart_init_new(void);
extern void uart0_write_char(char c);

#define UART0_BUF_QUEUE_BYTES 128

kbuf_queue_t g_buf_queue_uart;
char g_buf_uart[UART0_BUF_QUEUE_BYTES];

int32_t hal_uart_send(aos_uart_dev_t *uart, const void *data, uint32_t size, uint32_t timeout)
{
    int i = 0;
    char* pdata = (char *)data;

    for(i = 0; i < size; i++)
    {
        uart0_write_char(pdata[i]);
    }

    return 0;
}

int32_t hal_uart_recv_II(aos_uart_dev_t *uart, void *data, uint32_t expect_size,
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

int32_t hal_uart_init(aos_uart_dev_t *uart)
{
    int32_t ret = -1;

	if (uart == NULL) 
	{
	    return -1;
	}
	
	if(uart->port == 0)
	{
	    uart_init_new();
       krhino_buf_queue_create(&g_buf_queue_uart, "buffer_queue_uart0", g_buf_uart, UART0_BUF_QUEUE_BYTES,1);
       ret = 0;
	}

   return ret;
}

int32_t hal_uart_finalize(aos_uart_dev_t *uart)
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
