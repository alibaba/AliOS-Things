/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#include "irq.h"
#include "irq/irqdesc.h"

int aos_irq_system_init(void);
int aos_irq_system_deinit(void);
int aos_request_threaded_irq (int irq, irq_handler_t handler,
            irq_handler_t thread_fn, unsigned long irqflags,
            const char *devname, void *dev_id);
void *aos_free_irq(int irq, void *dev_id);
void aos_enable_irq(int irq);
void aos_disable_irq(int irq);
void aos_set_nmi_irq(int irq);
void aos_send_mask_ipi(int irq, unsigned int cpumask);
int aos_irq_is_active(int irq);
int aos_irq_set_pending(int irq);
int aos_irq_clear_pending(int irq);
void irq_set_eoi(uint32_t number, uint32_t flags);

irq_error irq_register_isr(uint16_t irq_number, isr_ptr callback)
{
    int ret;
    ret = aos_request_threaded_irq(irq_number, (irq_handler_t)callback, (irq_handler_t)callback, 0, 0, 0);
    if(ret == 0){
        return IRQ_OK;
    }else{
        return IRQ_INVALID_IRQ_ID;
    }
}

int request_irq(uint32_t irq, irq_handler_t handler, uint32_t type, const char *name, void *dev)
{
    return aos_request_threaded_irq(irq, (irq_handler_t)handler, (irq_handler_t)handler, type, name, dev);
}

void gic_init(void)
{
    aos_irq_system_init();
}

void gic_enable_interrupt(uint16_t number)
{
    aos_enable_irq(number);
}

void gic_disable_interrupt(uint16_t number) {
    aos_disable_irq(number);
}

void gic_end_interrupt(uint32_t number)
{
    irq_set_eoi(number,0);
}
