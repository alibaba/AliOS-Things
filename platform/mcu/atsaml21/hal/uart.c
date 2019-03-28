/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <compiler.h>

#include "k_api.h"

#include "aos/hal/uart.h"
#include "aos/hal/timer.h"

#include "saml21.h"
#include "driver_init.h"
#include "atcmd_config_platform.h"


uart_dev_t uart_0 =
{
    .port = 0,
};

extern struct usart_async_descriptor USART_AT;
static struct io_descriptor *io_at;
static volatile bool tx_complete = true;

static void rx_cb_USART_AT(const struct usart_async_descriptor *const io_descr)
{
    /* Driver will store the received data to its buffer */
}

static void tx_cb_USART_AT(const struct usart_async_descriptor *const io_descr)
{
    tx_complete = true;
}

int32_t hal_uart_init(uart_dev_t *uart)
{
    if ( uart->port == AT_UART_PORT) {
        usart_async_register_callback(&USART_AT, USART_ASYNC_RXC_CB, rx_cb_USART_AT);
        usart_async_register_callback(&USART_AT, USART_ASYNC_TXC_CB, tx_cb_USART_AT);
        usart_async_get_io_descriptor(&USART_AT, &io_at);
        usart_async_enable(&USART_AT);
    }
    return 0;
}

int32_t hal_uart_send(uart_dev_t *uart, const void *data, uint32_t size, uint32_t timeout)
{
    if ( uart->port == AT_UART_PORT) {
        io_write(io_at, (uint8_t *)data, size);
        tx_complete = false;
        while (tx_complete == false) {
            krhino_task_yield();
        }
    }
    else if ( uart->port == 0 ) {
        stdio_io_write((const uint8_t *)data, size);
    }
    return 0;
}

int32_t hal_uart_recv_II(uart_dev_t *uart, void *data, uint32_t expect_size, uint32_t *recv_size, uint32_t timeout)
{
    uint32_t current = (uint32_t)aos_now_ms();
    uint32_t n = 0;

    if ( uart->port == AT_UART_PORT) {
        *recv_size = 0;
        do {
            n = io_read(io_at, data + *recv_size, expect_size);
            *recv_size += n;
            expect_size -= n;
        } while((aos_now_ms() - current) < timeout && expect_size );
    }
    else if ( uart->port == 0 ) {
        *recv_size = expect_size;
        stdio_io_read((const uint8_t *)data, expect_size);
    }
    return 0;
}

int32_t hal_uart_finalize(uart_dev_t *uart)
{
   return 0;
}
