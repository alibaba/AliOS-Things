/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdint.h>
#include "k_config.h"
#include "syscall.h"
#include "hal_uart_syscall_arg.h"
#include "syscall_no.h"

int32_t hal_uart_init(uart_dev_t *uart)
{
    hal_uart_init_syscall_arg_t _arg;

    _arg.uart = uart;

    return (int32_t)SYSCALL(SYS_HAL_UART_INIT, (void*)&_arg);
}

int32_t hal_uart_send(uart_dev_t *uart, const void *data, uint32_t size,
                      uint32_t timeout)
{
    hal_uart_send_syscall_arg_t _arg;

    _arg.uart    = uart;
    _arg.data    = data;
    _arg.size    = size;
    _arg.timeout = timeout;

    return (int32_t)SYSCALL(SYS_HAL_UART_SEND, (void*)&_arg);
}

int32_t hal_uart_recv(uart_dev_t *uart, void *data, uint32_t expect_size,
                      uint32_t timeout)
{
    hal_uart_recv_syscall_arg_t _arg;

    _arg.uart        = uart;
    _arg.data        = data;
    _arg.expect_size = expect_size;
    _arg.timeout     = timeout;

    return (int32_t)SYSCALL(SYS_HAL_UART_RECV, (void*)&_arg);
}

int32_t hal_uart_recv_II(uart_dev_t *uart, void *data, uint32_t expect_size,
                         uint32_t *recv_size, uint32_t timeout)
{
    hal_uart_recv_II_syscall_arg_t _arg;

    _arg.uart        = uart;
    _arg.data        = data;
    _arg.expect_size = expect_size;
    _arg.recv_size   = recv_size;
    _arg.timeout     = timeout;

    return (int32_t)SYSCALL(SYS_HAL_UART_RECV_II, (void*)&_arg);
}

int32_t hal_uart_finalize(uart_dev_t *uart)
{
    hal_uart_finalize_syscall_arg_t _arg;

    _arg.uart = uart;

    return (int32_t)SYSCALL(SYS_HAL_UART_FINALIZE, (void*)&_arg);
}

