/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include "amp_boot.h"

static uart_dev_t g_boot_uart = {0};

void amp_boot_uart_send_byte(unsigned char c)
{
    if ((aos_hal_uart_send(&g_boot_uart, &c, 1, osWaitForever)) != 0) {
        //aos_printf("amp_boot_uart_send_byte error\n");
    }
}

void amp_boot_uart_send_str(char *str)
{
    if ((aos_hal_uart_send(&g_boot_uart, str, strlen(str), osWaitForever)) != 0) {
        //aos_printf("amp_boot_uart_send_str %s error\n", str);
    }
}

unsigned char amp_boot_uart_recv_byte(unsigned char *c)
{
    int read_byte = 0;
    if (aos_hal_uart_recv_poll(&g_boot_uart, c, 1, &read_byte) != 0) {
        return 0;
    }
    return read_byte;
}

int amp_boot_uart_recv_line(unsigned char *str_line, int lens, int timeout_ms)
{
    uint64_t begin_time = (uint64_t)aos_now_ms();
    int32_t  read_byte  = 0;
    int32_t  str_num    = 0;
    char c = 0;

    while (1) {
        c = 0;
        aos_hal_uart_recv_poll(&g_boot_uart, &c, 1, &read_byte);
        if (read_byte == 1) {
            str_line[str_num] = c;
            if(c == '\n') {
                if ((str_num > 0) && (str_line[str_num - 1] == '\r')) {
                    str_num --;
                }
                return str_num;
            }
            if (str_num >= lens) {
                return 0;
            }
            str_num ++;
        }

        if((timeout_ms != osWaitForever) && (begin_time + timeout_ms < aos_now_ms())) {
            return 0;
        }
    }

    return 0;
}

void amp_boot_uart_init(void)
{
    g_boot_uart.port = AMP_BOOT_UART_PORT;
    g_boot_uart.config.baud_rate = AMP_BOOT_UART_BAUDRATE;
    g_boot_uart.config.data_width = DATA_WIDTH_8BIT;
    g_boot_uart.config.flow_control = FLOW_CONTROL_DISABLED;
    g_boot_uart.config.mode = MODE_TX_RX;
    g_boot_uart.config.parity = NO_PARITY;
    g_boot_uart.config.stop_bits = STOP_BITS_1;
    int ret = aos_hal_uart_init(&g_boot_uart);
    if (ret) {
        return;
    }
}
