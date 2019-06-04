/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef HAL_UART_KSYSCALL_H
#define HAL_UART_KSYSCALL_H

int32_t sys_hal_uart_init_stub(void *arg);

int32_t sys_hal_uart_send_stub(void *arg);

/* this function is almost discarded by kernel */
int32_t sys_hal_uart_recv_stub(void *arg);

int32_t sys_hal_uart_recv_II_stub(void *arg);

int32_t sys_hal_uart_finalize_stub(void *arg);

#endif /* HAL_UART_KSYSCALL_H */

