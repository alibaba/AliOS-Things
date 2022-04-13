/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#include <board.h>
#include <aos/kernel.h>
#include <k_api.h>
#include <aos/list.h>
#include <irq/irqdesc.h>
#include <irqchip.h>

void __send_mask_ipi_svc(int irq, unsigned int cpumask, int flags)
{
    irq_send_sgi(0, cpumask, irq, flags);
    return;
}

void __irq_enable_svc(int irq)
{
    enable_irq(irq);
    return;
}

void __irq_disable_svc(int irq)
{
    disable_irq(irq);

    return;
}

int __irq_is_active_svc(int irq)
{
    //printk("irq fun:%s not support\r\n", __func__);
    return false;
}

int __irq_set_priority_svc(int irq, unsigned int priority)
{
    set_irq_priority(irq, priority);

    return 0;
}

int __irq_set_pending_svc(int irq)
{
    set_pending_irq(irq);
    //printk("irq fun:%s not support\r\n", __func__);
    return 0;
}

int __irq_clear_pending_svc(int irq)
{
    //printk("irq fun:%s not support\r\n", __func__);
    return 0;
}

int __irq_get_active_irq_svc()
{
    //printk("irq fun:%s not support\r\n", __func__);
    return -1;
}

int __irq_end_interrupt_svc(int irq, uint32_t  gp_flags)
{
    irq_set_eoi(irq, gp_flags);
    return 0;
}

int __irq_set_type_svc(int irq, uint32_t  edge_flags)
{
    return irq_set_type(irq, edge_flags);
}

int __irq_set_nmi_svc(int irq)
{
    irq_set_nmi(irq);
    return 0;
}

int __irq_set_affinity_global(int irq, uint32_t  cpumask)
{
    irq_set_affinity_global(irq, cpumask);
    return 0;
}
