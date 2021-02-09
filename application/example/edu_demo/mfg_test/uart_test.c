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

#define  TEST_DATA     "hello"

static int32_t uart_test_common(uint8_t port, uint32_t baud)
{
    int32_t ret = 0;
    int32_t i = 0;
    static int32_t init_flag[3] = {0};
    uint32_t recv_size = 0;
    uint8_t recv_data[10] = {0};
    uart_dev_t uart_ttl = {0};

    uart_ttl.port = port;  /*ttl use uart 2*/
    uart_ttl.config.baud_rate = baud;
    uart_ttl.config.data_width = DATA_WIDTH_8BIT;
    uart_ttl.config.flow_control = FLOW_CONTROL_DISABLED;
    uart_ttl.config.mode = MODE_TX_RX;
    uart_ttl.config.parity = NO_PARITY;
    uart_ttl.config.stop_bits = STOP_BITS_1;

    printf("\r\n ====start uart %d test baudrate %d ====\r\n", port, baud);

    if (port >= 3) {
        printf("\r\n ======= Invalid uart part %d =====\r\n", port);
    }

    if (init_flag[port] == 0) {
        ret = hal_uart_init(&uart_ttl);
        hal_uart_finalize(&uart_ttl);
        ret |= hal_uart_init(&uart_ttl);
        init_flag[port] = 1;
    }
    if (ret) {
        printf("=====uart test : uart %d dev init fail =====\r\n", port);
        return -1;
    }

    for (i = 0; i < 3; i++) {
        ret = hal_uart_send(&uart_ttl, TEST_DATA, strlen(TEST_DATA), 1000);
        if (ret) {
            printf("=====uart test : uart %d fail to send test data=====\r\n", port);
            continue;
        }

        memset(recv_data, 0, sizeof(recv_data));
        aos_msleep(1000);

        ret = hal_uart_recv_II(&uart_ttl, recv_data, strlen(TEST_DATA), &recv_size, 2000);
        if (ret || recv_size != strlen(TEST_DATA)) {
            printf("=====uart test : uart %d fail to recv test data, %d %d, %s =====\r\n", port, ret, recv_size, recv_data);
            continue;
        }

        printf("=====uart test : uart %d send test data is %s recv data is %s =====\r\n", port, TEST_DATA, recv_data);

        aos_msleep(100);

        if (strcmp(recv_data, TEST_DATA) == 0) {
            break;
        }
    }

    if (i >= 3) {
        printf("=====uart test : uart %d test fail =====\r\n", uart_ttl.port);
        return -1;
    }

    printf("=====uart test : uart %d test successed =====\r\n", uart_ttl.port);
    return 0;

}

int uart_ttl_test_internal()
{
    int32_t ret = 0;

    printf("\r\n\r\n");
    printf("***************************************************************\r\n");
    printf("************************* UART TTL Test ***********************\r\n");
    printf("***************************************************************\r\n");
    printf("********* How to test: connect PIN10 and PIN12 ****************\r\n");
    printf("***************************************************************\r\n");
    printf("=====UART TTL test : Start=====\r\n");

    ret = uart_test_common(2, 115200);
    if (ret) {
        printf("\r\n=====uart test : ttl test fail ===\r\n");
        return -1;
    }

    printf("\r\n=====uart test : ttl test successed ===\r\n");
    return 0;
}

void uart_232_test_internal()
{
    int32_t ret = 0;

    printf("\r\n=====uart test : begin 232 test ===\r\n");

    ret = uart_test_common(0, 115200);
    if (ret) {
        printf("\r\n=====uart test : 232 test fail ===\r\n");
        return ;
    }

    printf("\r\n=====uart test : 232 test successed ===\r\n");
}

static void handle_uarttest_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    if (argc < 2) {
        printf("Usage: uarttest ttl/232 \n");
        return;
    }

    if (strcmp(argv[1], "ttl") == 0) {
        uart_ttl_test_internal();
    } else if (strcmp(argv[1], "232") == 0) {
        uart_232_test_internal();
    } else {
        printf("Usage: uarttest ttl/232 \n");
        return;
    }
}

static struct cli_command uarttest_cmd = {
    .name     = "uarttest",
    .help     = "uarttest [ttl/232]",
    .function = handle_uarttest_cmd
};

int uart_ttl_test()
{
    aos_cli_register_command(&uarttest_cmd);
    return uart_ttl_test_internal();
}

void uart_232_test()
{
    uart_232_test_internal();
}
