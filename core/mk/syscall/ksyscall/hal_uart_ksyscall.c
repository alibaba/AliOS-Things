/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdint.h>
#include "k_config.h"

#if (RHINO_CONFIG_CPU_NUM > 1)
#include "k_api.h"
#include "smp_port.h"
extern kspinlock_t g_smp_printlock;
#endif

#include "aos/hal/uart.h"
#include "hal_uart_syscall_arg.h"

int32_t sys_hal_uart_init_stub(void *arg)
{
    hal_uart_init_syscall_arg_t *_arg = arg;

    return hal_uart_init(_arg->uart);
}

int32_t sys_hal_uart_send_stub(void *arg)
{
    hal_uart_send_syscall_arg_t *_arg = arg;
    int32_t ret;

#if (RHINO_CONFIG_CPU_NUM > 1)
    cpu_cpsr_t flags_cpsr;
    krhino_spin_lock_irq_save(&g_smp_printlock,flags_cpsr);
#endif

    ret = hal_uart_send(_arg->uart, _arg->data, _arg->size, _arg->timeout);

#if (RHINO_CONFIG_CPU_NUM > 1)
    krhino_spin_unlock_irq_restore(&g_smp_printlock,flags_cpsr);
#endif

    return ret;
}

/* this function is almost discarded by kernel */
int32_t sys_hal_uart_recv_stub(void *arg)
{
    hal_uart_recv_syscall_arg_t *_arg = arg;

    _arg = _arg;

    return 0;
}

int32_t sys_hal_uart_recv_II_stub(void *arg)
{
    hal_uart_recv_II_syscall_arg_t *_arg = arg;

    return hal_uart_recv_II(_arg->uart, _arg->data, _arg->expect_size,
                            _arg->recv_size, _arg->timeout);
}

int32_t sys_hal_uart_finalize_stub(void *arg)
{
    hal_uart_finalize_syscall_arg_t *_arg = arg;

    return hal_uart_finalize(_arg->uart);
}

