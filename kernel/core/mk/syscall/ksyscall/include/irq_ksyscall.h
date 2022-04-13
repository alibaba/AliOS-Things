/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef IRQ_KSYSCALL_H
#define IRQ_KSYSCALL_H

int sys_register_irq_global_stub(void *arg);
int sys_unregister_irq_global_stub(void *arg);
int sys_unregister_allirq_global_stub(void *arg);
int sys_ipi_send_stub(void *arg);
void sys_irq_enable_stub(void *arg);
void sys_irq_disable_stub(void *arg);
int sys_irq_is_active_stub(void *arg);
int sys_irq_set_priority_stub(void *arg);
int sys_irq_set_pending_stub(void *arg);
int sys_irq_clear_pending_stub(void *arg);
int sys_irq_get_active_irq_stub(void *arg);
int sys_irq_end_interrupt_stub(void *arg);
int sys_irq_set_type_stub(void *arg);
int sys_irq_set_nmi_stub(void *arg);
int sys_irq_set_affinity_stub(void *arg);
uint32_t sys_aos_int_lock_stub(void *arg);
void sys_aos_int_unlock_stub(void *arg);

#endif /* IRQ_KSYSCALL_H */

