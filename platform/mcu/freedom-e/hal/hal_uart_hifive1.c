/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include "k_api.h"
#include "hal/soc/uart.h"
#include <platform.h>
#include "encoding.h"
#include <stdarg.h>
#include <stdio.h>


#define MAX_BUF_UART_BYTES   32

kbuf_queue_t g_buf_queue_uart;
char g_buf_uart[MAX_BUF_UART_BYTES];


/*Entry Point for Machine Uart Interrupt Handler*/
/*called from bsp/env/ventry.s          */
void handle_m_uart_interrupt(){

    int32_t val = (int32_t) UART0_REG(UART_REG_RXFIFO);
    char c = 0;
    if (val > 0) {
      c =  val & 0xFF;
      krhino_buf_queue_send(&g_buf_queue_uart, &c, 1);
    }
        
    return ;

}


int32_t hal_uart_init(uart_dev_t* uart )
{
    //uart_init(115200);
    int32_t err_code;
    uart_init(115200);

    err_code = hal_interpt_install(INT_UART0_BASE, handle_m_uart_interrupt, NULL,  NULL);

    err_code |= hal_interpt_umask(INT_UART0_BASE);
    
    err_code |= krhino_buf_queue_create(&g_buf_queue_uart, "buf_queue_uart",
      g_buf_uart, MAX_BUF_UART_BYTES, 1);
    return err_code;
}


int32_t hal_uart_send(uart_dev_t *uart, const void *data, uint32_t size, uint32_t timeout) {

    int i;
    char *cdata = (char *)data;
    for(i = 0;i < size;i++ )
    {
        //uart_putchar(g_debug_uart_port,cdata++);
		_putc(*cdata++);
    }
    return 0;
}


int32_t hal_uart_recv(uart_dev_t *uart, void *data, uint32_t expect_size, uint32_t timeout)
{
    uint8_t *pdata = (uint8_t *)data;
    int i = 0;
    uint32_t rx_count = 0;
    int32_t ret = -1;
    int32_t rev_size;

    if (data == NULL) {
        return -1;
    }

    for (i = 0; i < expect_size; i++)
    {
        ret = krhino_buf_queue_recv(&g_buf_queue_uart, RHINO_WAIT_FOREVER, &pdata[i], &rev_size);
        if((ret == 0) && (rev_size == 1))
        {
            rx_count++;
        }else {
            break;
        }

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


int32_t hal_uart_recv_II(uart_dev_t *uart, void *data, uint32_t expect_size,
                      uint32_t *recv_size, uint32_t timeout)
{
    uint8_t *pdata = (uint8_t *)data;
    int i = 0;
    uint32_t rx_count = 0;
    int32_t ret = -1;
    int32_t rev_size;

    if (data == NULL) {
        return -1;
    }

    for (i = 0; i < expect_size; i++)
    {
        ret = krhino_buf_queue_recv(&g_buf_queue_uart, RHINO_WAIT_FOREVER, &pdata[i], &rev_size);
        if((ret == 0) && (rev_size == 1))
        {
            rx_count++;
        }else {
            break;
        }

    }
    
    if (recv_size)
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


