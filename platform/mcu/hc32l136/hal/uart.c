/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include "aos/hal/uart.h"

uart_dev_t uart_0;

int32_t hal_uart_send(uart_dev_t *uart, const void *data, uint32_t size, uint32_t timeout)
{
    if (size == 1) {
        UART0_SendChar(*((uint8_t *)data));
        return 0;
    } else {
        return -1;
    }
}

