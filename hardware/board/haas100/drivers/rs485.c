/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <aos/errno.h>
#include <aos/kernel.h>
#include <k_api.h>
#include "ulog/ulog.h"
#include "aos/hal/uart.h"

/* RS485 Macro definition */
#define HAAS_RS485_UART_IDX  1
#define HAAS_RS485_BD_DFT    19200

/* static variable definition */
static uart_dev_t uart_rs485 = {0};

int32_t haas_rs485_init(uart_dev_t *uart_dev)
{
    int32_t ret = 0;

    if(uart_dev == NULL)
    {
        uart_rs485.port = HAAS_RS485_UART_IDX;
        uart_rs485.config.baud_rate = HAAS_RS485_BD_DFT;
        uart_rs485.config.data_width = DATA_WIDTH_8BIT;
        uart_rs485.config.flow_control = FLOW_CONTROL_DISABLED;
        uart_rs485.config.mode = MODE_TX_RX;
        uart_rs485.config.parity = NO_PARITY;
        uart_rs485.config.stop_bits = STOP_BITS_1;
    }
    else
    {
        memset(&uart_rs485, 0, sizeof(uart_dev_t));
        memcpy(&uart_rs485, uart_dev, sizeof(uart_dev_t));
    }

    ret = hal_uart_init(&uart_rs485);
    if (ret) {
        printf("=====rs485: init fail =====\r\n");
        return ret;
    }

    return ret;
}

int32_t haas_rs485_send(const void *data, uint32_t size, uint32_t timeout)
{
    int32_t ret;

    ret = hal_uart_send(&uart_rs485, data, size, timeout);
    if (ret) {
        printf("=====rs485: fail to send test data======\r\n");
        return -1;
    }

}

int32_t haas_rs485_recv(void *data, uint32_t expect_size,
                         uint32_t *recv_size, uint32_t timeout)
{
    int32_t ret = 0;

    ret = hal_uart_recv_II(&uart_rs485, data, expect_size, recv_size, timeout);
    if (ret)
    {
        printf("=====rs485: recv data fail=====\r\n");
    }
}

int32_t haas_rs485_deinit(uart_dev_t *uart)
{
    int32_t ret = 0;

    ret = hal_uart_finalize(&uart_rs485);

    return ret;
}
