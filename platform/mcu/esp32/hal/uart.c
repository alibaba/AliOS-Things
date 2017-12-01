/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdint.h>

#define uart_dev_t aos_uart_dev_t
#define uart_config_t aos_uart_config_t
#include <hal/soc/uart.h>
#undef uart_dev_t
#undef uart_config_t

#include <rom/ets_sys.h>
#include <driver/uart.h>

int32_t hal_uart_send(aos_uart_dev_t *uart, const void *data, uint32_t size, uint32_t timeout)
{
    uart_write_bytes(uart->port, data, size);
    return 0;
}

int32_t hal_uart_recv(aos_uart_dev_t *uart, void *data, uint32_t expect_size, uint32_t *recv_size, uint32_t timeout)
{
    int ttl_len = 0;
    char *buf = data;
    while (1) {
        char c;
        int ret = uart_read_bytes(uart->port, &c, 1, 100);
        if (ret <= 0)
            break;

        *buf++ = c;
        ttl_len ++;
        if (ttl_len >= expect_size)
            break;
    }

    if (recv_size)
        *recv_size = ttl_len;

    return ttl_len > 0 ? 0 : -1;
}

int32_t hal_uart_init(aos_uart_dev_t *uart)
{
    uart_driver_install(uart->port, 256, 0, 0, NULL, 0);
    return 0;
}

int32_t hal_uart_finalize(aos_uart_dev_t *uart)
{
    return 0;
}


