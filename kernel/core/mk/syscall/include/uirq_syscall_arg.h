/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef UIRQ_SYSCALL_H
#define UIRQ_SYSCALL_H

typedef struct {
    int irq;
    irq_handler_t handler;
    irq_handler_t thread_fn;
    unsigned long irqflags;
    const char *devname;
    void *dev_id;
} u_irq_register_syscall_arg_t;

typedef struct {
    int irq;
    void *dev_id;
} u_irq_unregister_syscall_arg_t;

typedef struct {
    int irq;
    uint32_t  cpumask;
    int flags;
} u_irq_ipi_send_syscall_arg_t;

typedef struct {
    int irq;
} u_irq_one_syscall_arg_t;

typedef struct {
    int irq;
    uint32_t  priority;
} u_irq_set_priority_syscall_arg_t;

typedef struct {
    int irq;
    uint32_t  gp_flags;/*group 0 or 1*/
} u_irq_end_interrupt_syscall_arg_t;

typedef struct {
    int irq;
    uint32_t  edge_flags;/*edge or level*/
} u_irq_set_type_syscall_arg_t;

typedef struct {
    int irq;
    uint32_t  cpumask;
} u_irq_set_affinity_syscall_arg_t;

#endif /* UIRQ_SYSCALL_H */

