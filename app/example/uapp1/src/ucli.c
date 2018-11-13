/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <hal/hal.h>
#include <k_api.h>

int cli_getchar(char *ch)
{
    int        ret;
    uint32_t   recv_size;
    uart_dev_t uart;

    memset(&uart, 0, sizeof(uart));
    uart.port = 0;

    ret = hal_uart_recv_II(&uart, ch, 1, &recv_size, HAL_WAIT_FOREVER);

    if ((ret == 0) && (recv_size == 1)) {
        return 1;
    } else {
        return 0;
    }
}

int cli_init(void)
{
    int        ret;
    uart_dev_t uart;

    memset(&uart, 0, sizeof(uart));
    uart.port                = 0;
    uart.config.baud_rate    = 115200;
    uart.config.data_width   = DATA_WIDTH_8BIT;
    uart.config.flow_control = FLOW_CONTROL_DISABLED;
    uart.config.mode         = MODE_TX_RX;
    uart.config.parity       = NO_PARITY;
    uart.config.stop_bits    = STOP_BITS_1;

    ret = hal_uart_init(&uart);

    return ret;
}

int cli_deinit(void)
{
    int ret;
    uart_dev_t uart;

    memset(&uart, 0, sizeof(uart));
    uart.port = 0;

    ret = hal_uart_finalize(&uart);

    return ret;
}

void cli_loop(void)
{
    while (1) {
        printf("waitting for input\r\n");
        char ch;
        if (cli_getchar(&ch)) {
            printf("read char: %c\r\n", ch);
        }
    }
}

