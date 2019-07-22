/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef HAL_UART_SYSCALL_ARG_H
#define HAL_UART_SYSCALL_ARG_H

#include "aos/hal/uart.h"

typedef struct {
    uart_dev_t *uart;
} hal_uart_init_syscall_arg_t;

typedef struct {
    uart_dev_t *uart;
    const void *data;
    uint32_t    size;
    uint32_t    timeout;
} hal_uart_send_syscall_arg_t;

typedef struct {
    uart_dev_t *uart;
    void       *data;
    uint32_t    expect_size;
    uint32_t    timeout;
} hal_uart_recv_syscall_arg_t;

typedef struct {
    uart_dev_t *uart;
    void       *data;
    uint32_t    expect_size;
    uint32_t   *recv_size;
    uint32_t    timeout;
} hal_uart_recv_II_syscall_arg_t;

typedef struct {
    uart_dev_t *uart;
} hal_uart_finalize_syscall_arg_t;

#endif /* HAL_UART_SYSCALL_ARG_H */

