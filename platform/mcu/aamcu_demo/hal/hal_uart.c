/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include "aos/hal/uart.h"

#ifdef HAL_UART_MODULE_ENABLED

int32_t hal_uart_init(uart_dev_t *uart)
{

}

int32_t hal_uart_send(uart_dev_t *uart, const void *data, uint32_t size, uint32_t timeout)
{

}

int32_t hal_uart_recv_II(uart_dev_t *uart, void *data, uint32_t expect_size,
                         uint32_t *recv_size, uint32_t timeout)
{

}

int32_t hal_uart_finalize(uart_dev_t *uart)
{

}

#endif /* HAL_UART_MODULE_ENABLED */

