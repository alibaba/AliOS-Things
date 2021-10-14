/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef AMP_BOOT_UART_H
#define AMP_BOOT_UART_H

void amp_boot_uart_send_byte(unsigned char c);
void amp_boot_uart_send_str(char *str);
unsigned char amp_boot_uart_recv_byte(unsigned char *c);
int amp_boot_uart_recv_line(char *str_line, int lens, int timeout_ms);
void amp_boot_uart_init(void);
void amp_boot_uart_deinit(void);
#endif