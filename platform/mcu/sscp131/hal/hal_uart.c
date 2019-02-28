/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "lib_uart.h"

#include "aos/hal/uart.h"
#include "aos/errno.h"

uart_dev_t uart_0;

#define UART UART1

int32_t hal_uart_send(uart_dev_t *uart, const void *data, uint32_t size, uint32_t timeout)
{
    uint32_t count = 0;
    FlagStatus status = RESET;

    do {
        UART_SendByte(UART, *((unsigned char *)data));
        do
        {
            status = UART_GetFlagStatus(UART, UART_FLAG_TB);
            count++;
        } while((status == RESET)&&(count != 0x1CE2));

        if(count == 0x1CE2)
            return -EIO;

        size--;
    } while (size > 0);

    return 0;
}
