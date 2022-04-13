/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <aos/kernel.h>
#include <k_api.h>
#include <irq/irqdesc.h>
#include <syscall_no.h>
#include <board.h>
#include <uirq_syscall_arg.h>

void __send_mask_ipi_svc(int irq, unsigned int cpumask, int flags);
void __irq_enable_svc(int irq);
void __irq_disable_svc(int irq);
int __irq_is_active_svc(int irq);
int __irq_set_priority_svc(int irq, unsigned int priority);
int __irq_set_pending_svc(int irq);
int __irq_clear_pending_svc(int irq);
int __irq_get_active_irq_svc();

int sys_ipi_send_stub(void *arg)
{
    u_irq_ipi_send_syscall_arg_t *_arg = arg;
    __send_mask_ipi_svc(_arg->irq, _arg->cpumask, _arg->flags);
    return 0;
}

void sys_irq_enable_stub(void *arg)
{
    u_irq_one_syscall_arg_t *_arg = arg;
    __irq_enable_svc(_arg->irq);

    return;
}

void sys_irq_disable_stub(void *arg)
{
    u_irq_one_syscall_arg_t *_arg = arg;
    __irq_disable_svc(_arg->irq);

    return;
}

int sys_irq_is_active_stub(void *arg)
{
    u_irq_one_syscall_arg_t *_arg = arg;

    return __irq_is_active_svc(_arg->irq);
}

int sys_irq_set_priority_stub(void *arg)
{
    u_irq_set_priority_syscall_arg_t *_arg = arg;
    return __irq_set_priority_svc(_arg->irq, _arg->priority);
}

int sys_irq_set_pending_stub(void *arg)
{
    u_irq_one_syscall_arg_t *_arg = arg;

    return __irq_set_pending_svc(_arg->irq);
}

int sys_irq_clear_pending_stub(void *arg)
{
    u_irq_one_syscall_arg_t *_arg = arg;

    return __irq_clear_pending_svc(_arg->irq);
}

int sys_irq_get_active_irq_stub(void *arg)
{
    (void)arg;
    return __irq_get_active_irq_svc();
}

int sys_irq_end_interrupt_stub(void *arg)
{
    printk("irq fun:%s not support\r\n", __func__);
    return 0;
}
int sys_irq_set_type_stub(void *arg)
{
    printk("irq fun:%s not support\r\n", __func__);
    return 0;
}
int sys_irq_set_nmi_stub(void *arg)
{
    printk("irq fun:%s not support\r\n", __func__);
    return 0;
}
int sys_irq_set_affinity_stub(void *arg)
{
    printk("irq fun:%s not support\r\n", __func__);
    return 0;
}

uint32_t sys_aos_int_lock_stub(void *arg)
{
    (void)arg;
    extern uint32_t __intlock_userspace_save(void);
    return __intlock_userspace_save();
}

void sys_aos_int_unlock_stub(void *arg)
{
    extern void __intlock_userspace_restore(uint32_t pri);
    uint32_t flag = *(uint32_t *)arg;
    __intlock_userspace_restore(flag);
}

