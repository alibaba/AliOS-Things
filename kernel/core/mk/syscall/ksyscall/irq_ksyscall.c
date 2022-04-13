/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <aos/kernel.h>
#include <k_api.h>
#include <irq/irqdesc.h>
#include <syscall_no.h>
#include <uirq_syscall_arg.h>

extern irq_desc_t *irq_get_desc_lock(int irq, unsigned long *flags);
extern void irq_put_desc_unlock(irq_desc_t *desc, unsigned long flags);
extern int irq_setup_action(int irq, irq_desc_t *desc, irq_action_t *newaction, int usermode);
extern irq_action_t *irq_free_action(int irq, irq_desc_t *desc, void *dev_id, int usermode);
extern void irq_free_all_action(int irq, irq_desc_t *desc, int usermode);
extern uint32_t __intlock_userspace_save(void);


int __register_irq_userlevel(int irq, irq_handler_t handler,
                             irq_handler_t thread_fn, unsigned long irqflags,
                             const char *devname, void *dev_id)
{
    int ret;
    CPSR_ALLOC();
    irq_desc_t *irqdesc;
    irqdesc = irq_get_desc_lock(irq, (unsigned long *)&cpsr);

    /* reg irqaction */
    irq_action_t *irqaction_new = aos_malloc(sizeof(irq_action_t));
    if (NULL == irqaction_new) {
        irq_put_desc_unlock(irqdesc, (unsigned long)cpsr);
        return -1;
    }
    irqaction_new->handler = handler;
    irqaction_new->thread_fn = thread_fn;
    irqaction_new->devname = (void *)devname;
    irqaction_new->dev_id = dev_id;
    irqaction_new->flags = irqflags;
    irqaction_new->usermode = true;
    irqaction_new->pid = k_asid_get();
    dlist_init(&(irqaction_new->action_list));

    ret = irq_setup_action(irq, irqdesc, irqaction_new, irqaction_new->usermode);
    if (ret < 0) {
        aos_free(irqaction_new);
        irq_put_desc_unlock(irqdesc, (unsigned long)cpsr);
        return ret;
    }

    irq_put_desc_unlock(irqdesc, (unsigned long)cpsr);
    return 0;
}

int __unregister_irq_userlevel(int irq, void *dev_id)
{
    CPSR_ALLOC();
    irq_desc_t *irqdesc;
    irq_action_t *irqaction;

    irqdesc = irq_get_desc_lock(irq, (unsigned long *)&cpsr);
    irqaction = irq_free_action(irq, irqdesc, dev_id, true);
    if (NULL == irqaction) {
        irq_put_desc_unlock(irqdesc, (unsigned long)cpsr);
        return -1;
    }
    aos_free(irqaction);
    irq_put_desc_unlock(irqdesc, (unsigned long)cpsr);
    return 0;
}

int __unregister_allirq_userlevel(int irq)
{
    CPSR_ALLOC();
    irq_desc_t *irqdesc;

    irqdesc = irq_get_desc_lock(irq, (unsigned long *)&cpsr);
    irq_free_all_action(irq, irqdesc, true);
    irq_put_desc_unlock(irqdesc, (unsigned long)cpsr);
    return 0;
}

int __register_irq_global(int irq, irq_handler_t handler,
                          irq_handler_t thread_fn, unsigned long irqflags,
                          const char *devname, void *dev_id)
{
    return 0 ;
}

int __unregister_irq_global(int irq, void *dev_id)
{
    return 0;
}

int __unregister_allirq_global(int irq)
{
    return 0;
}

int sys_register_irq_global_stub(void *arg)
{
    u_irq_register_syscall_arg_t *_arg = arg;
    return __register_irq_userlevel(_arg->irq, _arg->handler,
                                    _arg->thread_fn, _arg->irqflags,
                                    _arg->devname, _arg->dev_id);
}

int sys_unregister_irq_global_stub(void *arg)
{
    u_irq_unregister_syscall_arg_t *_arg = arg;

    return __unregister_irq_userlevel(_arg->irq, _arg->dev_id);
}

int sys_unregister_allirq_global_stub(void *arg)
{
    u_irq_one_syscall_arg_t *_arg = arg;

    return __unregister_allirq_userlevel(_arg->irq);
}

