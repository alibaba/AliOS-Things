/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <aos/errno.h>
#include <aos/kernel.h>
#include <k_api.h>
#include "aos/hal/uart.h"
#include "aos/cli.h"

static int32_t uart_rs485_test_process()
{
    int32_t ret = 0;
    int32_t i = 0;
    uint32_t recv_size = 0;
    uint8_t recv_data[100] = {0};
    uart_dev_t uart_rs485 = {0};

    uart_rs485.port = 1;  /* uart1 be used as rs485 */
    uart_rs485.config.baud_rate = 19200;
    uart_rs485.config.data_width = DATA_WIDTH_8BIT;
    uart_rs485.config.flow_control = FLOW_CONTROL_DISABLED;
    uart_rs485.config.mode = MODE_TX_RX;
    uart_rs485.config.parity = NO_PARITY;
    uart_rs485.config.stop_bits = STOP_BITS_1;

    ret = hal_uart_init(&uart_rs485);
    if (ret) {
        printf("=====rs485 test : uart 1 dev init fail =====\r\n");
        return -1;
    }

    for(i=0; i < 2; i++)
    {
        recv_size = 0;
        memset(recv_data, 0, sizeof(recv_data));
        ret = hal_uart_recv_II(&uart_rs485, recv_data, sizeof(recv_data), &recv_size, 8000);
        if (ret) {
            printf("=====rs485 test : uart 1 fail to recv test data=====\r\n");
            continue;
        }
        osDelay(100);

        ret = hal_uart_send(&uart_rs485, recv_data, recv_size, 3000);
        if (ret) {
            printf("=====rs485 test : uart 1 fail to send test data=====\r\n");
            continue;
        }
        osDelay(100);
    }

    hal_uart_finalize(&uart_rs485);

    printf("=====rs485 test : Done=====\r\n");

    return 0;

}

static void rs485_autotest()
{
    int32_t ret = 0;

    printf("\r\n\r\n");
    printf("***************************************************************\r\n");
    printf("************************** RS485 Test *************************\r\n");
    printf("***************************************************************\r\n");
    printf("** Note: 1. pls. connect the 485 terminal by rs485-usb to pc **\r\n");
    printf("** Note: 2. pls. open serial port tool                       **\r\n");
    printf("** -------- The RS485 Baudrate is 19200                      **\r\n");
    printf("** -------- STOP BIT:1                                       **\r\n");
    printf("** How to test: type 'test' in serial port tool and the      **\r\n");
    printf("** board will return the word                                **\r\n");
    printf("***************************************************************\r\n");
    printf("=====rs485 test : Start=====\r\n");

    ret = uart_rs485_test_process();
    if (ret)
    {
        printf("\r\n=====rs485 test : Failed ===\r\n");

    }
}

static void handle_rs485test_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    rs485_autotest();
}

static struct cli_command rs485test_cmd = {
    .name     = "rs485test",
    .help     = "rs485test  !pls. connect the 485 terminal by rs485-usb to pc",
    .function = handle_rs485test_cmd
};

void rs485_test()
{
    aos_cli_register_command(&rs485test_cmd);
    rs485_autotest();
}
