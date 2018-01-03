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

int32_t hal_uart_send(aos_uart_dev_t *uart, const void *data, uint32_t size, uint32_t timeout)
{
    return 0;
}

int32_t hal_uart_recv(aos_uart_dev_t *uart, void *data, uint32_t expect_size, uint32_t *recv_size, uint32_t timeout)
{
}

int32_t hal_uart_init(aos_uart_dev_t *uart)
{
    return 0;
}

int32_t hal_uart_finalize(aos_uart_dev_t *uart)
{
    return 0;
}


