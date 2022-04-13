/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <aos/kernel.h>
#include <irq/irqdesc.h>
#include <syscall_no.h>
#include "syscall.h"
#include <uirq_syscall_arg.h>
extern void irq_usapce_general_handle(int irq, void *args);

int __register_irq_global(int irq, irq_handler_t handler, irq_handler_t thread_fn, unsigned long irqflags,
                          const char *devname, void *dev_id)
{
    u_irq_register_syscall_arg_t _arg;

    _arg.irq = irq;
    //_arg.handler = handler;
    //_arg.thread_fn = thread_fn;
    _arg.handler = (irq_handler_t)irq_usapce_general_handle;
    _arg.thread_fn = (irq_handler_t)irq_usapce_general_handle;
    _arg.irqflags = irqflags;
    _arg.devname = devname;
    _arg.dev_id = dev_id;
    return (int)SYSCALL(SYS_AOS_IRQ_REG, (void *)&_arg);
}

int __unregister_irq_global(int irq, void *dev_id)
{
    u_irq_unregister_syscall_arg_t _arg;

    _arg.irq = irq;
    _arg.dev_id = dev_id;

    return (int)SYSCALL(SYS_AOS_IRQ_UNREG, (void *)&_arg);
}

int __unregister_allirq_global(int irq)
{
    u_irq_one_syscall_arg_t _arg;

    _arg.irq = irq;

    return (int)SYSCALL(SYS_AOS_IRQ_ALL_UNREG, (void *)&_arg);
}

void __send_mask_ipi_svc(int irq, unsigned int cpumask, int flags)
{
    u_irq_ipi_send_syscall_arg_t _arg;

    _arg.irq = irq;
    _arg.cpumask = cpumask;
    _arg.flags = flags;
    SYSCALL(SYS_AOS_IRQ_IPI_SEND, (void *)&_arg);
}

void __irq_enable_svc(int irq)
{
    u_irq_one_syscall_arg_t _arg;
    _arg.irq = irq;
    SYSCALL(SYS_AOS_IRQ_ENABLE, (void *)&_arg);
    return;
}

void __irq_disable_svc(int irq)
{
    u_irq_one_syscall_arg_t _arg;
    _arg.irq = irq;
    SYSCALL(SYS_AOS_IRQ_DISABLE, (void *)&_arg);
    return;
}

int __irq_is_active_svc(int irq)
{
    u_irq_one_syscall_arg_t _arg;
    _arg.irq = irq;
    return (int)SYSCALL(SYS_AOS_IRQ_IS_ACTIVE, (void *)&_arg);
}

int __irq_set_priority_svc(int irq, unsigned int priority)
{
    u_irq_set_priority_syscall_arg_t _arg;
    _arg.irq = irq;
    _arg.priority = priority;
    return (int)SYSCALL(SYS_AOS_IRQ_SET_PRIORITY, (void *)&_arg);
}

int __irq_set_pending_svc(int irq)
{
    u_irq_one_syscall_arg_t _arg;
    _arg.irq = irq;
    return (int)SYSCALL(SYS_AOS_IRQ_SET_PENDING, (void *)&_arg);
}

int __irq_clear_pending_svc(int irq)
{
    u_irq_one_syscall_arg_t _arg;
    _arg.irq = irq;
    return (int)SYSCALL(SYS_AOS_IRQ_CLEAR_PENDING, (void *)&_arg);
}

int __irq_get_active_irq_svc()
{
    return (int)SYSCALL(SYS_AOS_IRQ_GET_ACTIVE, NULL);
}

int __irq_end_interrupt_svc(int irq, uint32_t  gp_flags)
{
    u_irq_end_interrupt_syscall_arg_t _arg;
    _arg.irq = irq;
    _arg.gp_flags = gp_flags;
    return (kstat_t)SYSCALL(SYS_AOS_IRQ_END_INTERRUPT, (void *)&_arg);
}

int __irq_set_type_svc(int irq, uint32_t  edge_flags)
{
    u_irq_set_type_syscall_arg_t _arg;
    _arg.irq = irq;
    _arg.edge_flags = edge_flags;
    return (kstat_t)SYSCALL(SYS_AOS_IRQ_SET_TYPE, (void *)&_arg);
}

int __irq_set_nmi_svc(int irq)
{
    u_irq_one_syscall_arg_t _arg;
    _arg.irq = irq;
    return (kstat_t)SYSCALL(SYS_AOS_IRQ_SET_NMI, (void *)&_arg);
}

int __irq_set_affinity_global(int irq, uint32_t  cpumask)
{
    u_irq_set_affinity_syscall_arg_t _arg;
    _arg.irq = irq;
    _arg.cpumask = cpumask;
    return (kstat_t)SYSCALL(SYS_AOS_IRQ_SET_AFFINITY, (void *)&_arg);
}

uint32_t __int_lock(void)
{
    return (uint32_t)SYSCALL(SYS_AOS_INT_LOCK, (void *)NULL);
}

void __int_unlock(uint32_t flag)
{
    SYSCALL(SYS_AOS_INT_UNLOCK, (void *)&flag);
}

kstat_t krhino_intrpt_enter(void)
{
    return 0;
}

void krhino_intrpt_exit(void)
{
    return;
}

