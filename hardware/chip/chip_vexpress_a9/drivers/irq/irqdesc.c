/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#include <aos/list.h>
#include <aos/kernel.h>
#include <k_api.h>
#include "k_compiler.h"
#include <irq/irqdesc.h>
#include <irqchip.h>
#if 0
#include "callback.h"
#endif
#include <errno.h>

extern void irq_handle_action(irq_desc_t *desc);
extern void os_irq_handle(void);
extern void os_fiq_handle(void);
extern void __irq_disable_svc(int irq);
extern void* aos_free_irq(int irq, void *dev_id);
extern void aos_free_allirq(int irq);
extern int __unregister_irq_global(int irq, void *dev_id);
extern int __unregister_allirq_global(int irq);
extern void irq_set_vector(int irq, void* addr);
extern int irq_set_vector_before_init(int irq, void* addr);
static irq_desc_t g_irq_desc[NR_IRQ_LINE];

static uint32_t k_asid_get(void)
{
    return 0;
}

int __register_irq_global(int irq, irq_handler_t handler,
                          irq_handler_t thread_fn, unsigned long irqflags,
                          const char *devname, void *dev_id)
{
    return 0;
}

int __unregister_irq_global(int irq, void *dev_id)
{
    return 0;
}

int __unregister_allirq_global(int irq)
{
    return 0;
}

#ifdef AOS_COMPILE_APP
//static kmutex_t g_irq_mutex;  // used by userspace
static aos_mutex_t g_irq_mutex;
#else
static kspinlock_t g_irq_lock;
#endif

irq_desc_t *irq_to_desc(int irq)
{
    unsigned int index;

    index = irq_to_desc_index(irq);
    if (index < NR_IRQ_LINE){
        return &g_irq_desc[index];
    }

    return NULL;
}

/**
irq_desc init
irqaction

**/
int irq_setup_action(int irq, irq_desc_t *desc, irq_action_t *newaction, int usermode)
{
    irq_action_t * tmpaction;
    if(NULL == desc || NULL == newaction){
        return -1;
    }
#ifdef AOS_COMPILE_APP
    newaction->pid = 0;
#else
    newaction->pid = k_asid_get();
#endif

    if(usermode){
        /* user irq set level mode */
        newaction->flags |= IRQ_TYPE_LEVEL_BOTH;
    }
    if(dlist_empty(&(desc->action_head))){
        dlist_add(&(newaction->action_list), &(desc->action_head));
        irq_set_vector(irq, os_irq_handle);
        return 0;
    }
    dlist_for_each_entry(&(desc->action_head), tmpaction, irq_action_t, action_list){
        if((tmpaction->flags != newaction->flags) || (0 == (tmpaction->flags & newaction->flags & IRQF_SHARED))){
            /* share/unshare, edge/level */
            PRINTF_E("irq:%d, flags are not shared,%d-%d!\r\n", irq, tmpaction->flags,newaction->flags);
            return -1;
        }
        /* shared: devid not same; or pid not same; or usermode not same */
        if((tmpaction->dev_id == newaction->dev_id) && (tmpaction->pid == newaction->pid)
            && (tmpaction->usermode == newaction->usermode)){
            PRINTF_E("irq shared:%d, but repeated registered,%p-%d-%d-0x%x-0x%x!\r\n", irq, tmpaction->dev_id,tmpaction->pid
                ,tmpaction->usermode, (unsigned int)tmpaction->handler,(unsigned int)newaction->handler);
            return -1;
        }
    }
    dlist_add(&(newaction->action_list), &(desc->action_head));

    return 0;
}

irq_action_t *irq_free_action(int irq, irq_desc_t *desc, void *dev_id, int usermode)
{
    uint32_t pid;
    dlist_t *tmp_list = NULL;
    irq_action_t * tmpaction = NULL;

#ifdef AOS_COMPILE_APP
    pid = 0;
#else
    pid = k_asid_get();
#endif

    dlist_for_each_entry_safe(&(desc->action_head), tmp_list, tmpaction, irq_action_t, action_list){
        if(tmpaction->dev_id == dev_id && tmpaction->pid == pid && tmpaction->usermode == usermode){
            dlist_del(&(tmpaction->action_list));
            return tmpaction;
        }
    }

    return NULL;
}

void irq_free_all_action(int irq, irq_desc_t *desc, int usermode)
{
    uint32_t pid;
    dlist_t *tmp_list = NULL;
    irq_action_t * tmpaction = NULL;

#ifdef AOS_COMPILE_APP
    pid = 0;
#else
    pid = k_asid_get();
#endif

    dlist_for_each_entry_safe(&(desc->action_head), tmp_list, tmpaction, irq_action_t, action_list){
        if(tmpaction->pid == pid && tmpaction->usermode == usermode){
            dlist_del(&(tmpaction->action_list));
            aos_free(tmpaction);
        }
    }

    return;
}

void irq_free_action_bypid(int irq, irq_desc_t *desc, uint32_t pid)
{
    (void)irq;
    dlist_t *tmp_list = NULL;
    irq_action_t * tmpaction = NULL;

    dlist_for_each_entry_safe(&(desc->action_head), tmp_list, tmpaction, irq_action_t, action_list){
        if(tmpaction->pid == pid){
            dlist_del(&(tmpaction->action_list));
            aos_free(tmpaction);
        }
    }

    return;
}

void irq_desc_init(void)
{
    uint32_t index;
    int irq;
    for (index = 0; index < NR_IRQ_LINE; index++) {
        g_irq_desc[index].name = "";
        g_irq_desc[index].depth = 0;
        g_irq_desc[index].handle_irq = (irq_flow_handler_t)irq_handle_action;
        dlist_init(&(g_irq_desc[index].action_head));
        g_irq_desc[index].count = 0;
        irq = desc_index_to_irq(index);
        irq_chip_init(&(g_irq_desc[index].irq_data),irq);
/*
#ifdef AOS_COMPILE_APP
        krhino_mutex_create(&g_irq_desc[index].irq_mutex, "irq_mutex");
#else
        krhino_spin_lock_init(&g_irq_desc[index].irq_lock);
#endif
*/
    }
#ifdef AOS_COMPILE_APP
    //krhino_mutex_create(&g_irq_mutex, "irq_mutex");
    aos_mutex_new(&g_irq_mutex);
#else
    (void)g_irq_lock;
    krhino_spin_lock_init(&g_irq_lock);
#endif
}

void irq_desc_uninit(void)
{
    uint32_t index;
    int irq;

    for (index = 0; index < NR_IRQ_LINE; index++) {
        if(!dlist_empty(&(g_irq_desc[index].action_head))){
            irq = desc_index_to_irq(index);
            aos_free_allirq(irq);
        }
    }
}

void aos_free_irq_bypid(int irq, uint32_t pid)
{
    irq_desc_t *irqdesc = irq_to_desc(irq);
    irq_free_action_bypid(irq, irqdesc, pid);
    return;
}

/* used for process exit in kernel mode */
void aos_irq_exit_bypid(uint32_t pid)
{
#ifdef AOS_COMPILE_APP
    return;
#else
    uint32_t index;
    int irq;
    CPSR_ALLOC();
    IRQ_CRITICAL_ENTER(&g_irq_lock,cpsr);

    for (index = 0; index < NR_IRQ_LINE; index++) {
        if(!dlist_empty(&(g_irq_desc[index].action_head))){
            irq = desc_index_to_irq(index);
            aos_free_irq_bypid(irq, pid);
        }
    }
    IRQ_CRITICAL_EXIT(&g_irq_lock,cpsr);
#endif
}

irq_desc_t *irq_get_desc_lock(int irq, unsigned int *flags)
{
    CPSR_ALLOC();
    cpsr = 0;

    irq_desc_t *desc = irq_to_desc(irq);

#ifdef AOS_COMPILE_APP
    IRQ_CRITICAL_ENTER(&g_irq_mutex,cpsr);
#else
    IRQ_CRITICAL_ENTER(&g_irq_lock,cpsr);
#endif

    *flags = cpsr;

    return desc;
}

void irq_put_desc_unlock(irq_desc_t *desc, unsigned int flags)
{
    //IRQ_CRITICAL_EXIT(desc, flags);
    (void)desc;
#ifdef AOS_COMPILE_APP
    IRQ_CRITICAL_EXIT(&g_irq_mutex,flags);
#else
    IRQ_CRITICAL_EXIT(&g_irq_lock,flags);
#endif
}

void irq_usapce_general_handle(int irq, void* args)
{
    irq_desc_t * desc;
    irq_data_t *irqdata;
    desc = irq_to_desc(irq);
    irq_action_t * tmpaction = NULL;

    if(dlist_empty(&(desc->action_head))){
        return;
    }
    irqdata = &(desc->irq_data);
    /* after jump to user mode, active state will be cleared in NVIC */
    irq_set_user_active(irqdata);
    /* traverse irqaction */
    dlist_for_each_entry(&(desc->action_head), tmpaction, irq_action_t, action_list){
        if(tmpaction->thread_fn){
            tmpaction->thread_fn(irq, NULL);
        }
    }
    irq_clear_user_active(irqdata);
    irqdata->chip->irq_enable(irqdata);
    //__irq_enable_svc(irq);
}

void irq_send_to_usermode(int irq, irq_action_t* action)
{
#if 0
#ifndef AOS_COMPILE_APP
    kbuf_queue_t *send_queue;
    cb_call_msg_t call_msg;
    uint32_t pid = action->pid;
    task_group_t* group = task_group_get_by_pid(pid);
    send_queue = group->cb_call_buf_queue;
    //recv_queue = group->cb_ret_buf_queue;
    call_msg.func_ptr     = action->thread_fn;
    call_msg.arg_cnt      = 1;
    call_msg.has_ret      = 0;
    call_msg.arg_value[0] = (void*)irq;
    cb_call_buf_queue_push(send_queue, &call_msg);
#endif
    return;
#endif
}

void handle_edge_irq(int irq, irq_action_t* action)
{
    if(!(action->usermode)){
        action->handler(irq, NULL);
    }else{
        /* send to user mode */
        irq_send_to_usermode(irq, action);
    }
}
void handle_level_irq(int irq, irq_action_t* action)
{
    if(!(action->usermode)){
        action->handler(irq, NULL);
    }else{
        __irq_disable_svc(irq);
        /* send to user mode */
        irq_send_to_usermode(irq, action);
    }
}

void irq_handle_action(irq_desc_t *desc)
{
    irq_data_t *irqdata;
    int irq;
    //irq_action_t *tmp_action;
    //irq_action_t *next_action;
    irq_action_t * tmpaction = NULL;
    irqdata = &(desc->irq_data);
    irq = irqdata->irq;

    if(dlist_empty(&(desc->action_head))){
        __irq_disable_svc(irq);
        return;
    }

    dlist_for_each_entry(&(desc->action_head), tmpaction, irq_action_t, action_list){
        if(tmpaction->flags & IRQ_TYPE_EDGE_BOTH){
            handle_edge_irq(irq, tmpaction);
        }else{
            handle_level_irq(irq, tmpaction);
        }
    }

    return;
}

/*
desc->depth != 0 : not enable irq automatically
or enable irq automatically
*/
extern int aos_irq_system_init(void);

int aos_request_threaded_irq (int irq, irq_handler_t handler,
        irq_handler_t thread_fn, unsigned long irqflags,
        const char *devname, void *dev_id)
{
    CPSR_ALLOC();
    irq_desc_t * irqdesc;
    int ret;

    if(!aos_irq_is_init()){
        aos_irq_system_init();
    }
    ret = irq_set_vector_before_init(irq, handler);
    if(ret){
        return 0;
    }

    if(irq < IRQ_NUM_START && irq >= IRQ_NUM_MAX){
        return -1;
    }

    if(NULL == handler && NULL == thread_fn){
        return -1;
    }

    /* Kernel space operate kernel data, user space operate user data. */
    if (((irqflags & IRQF_SHARED) && !dev_id)){
        return -EINVAL;
    }

    irqdesc = irq_get_desc_lock(irq, &cpsr);
    if(dlist_empty(&(irqdesc->action_head)))
    {
        /* first init */
        irq_set_vector(irq, os_irq_handle);
    }

    /* reg irqaction */
    irq_action_t* irqaction_new = aos_malloc(sizeof(irq_action_t));
    if(NULL == irqaction_new){
        irq_put_desc_unlock(irqdesc,cpsr);
        return -1;
    }

    irqaction_new->handler = handler;
    irqaction_new->thread_fn = thread_fn;
    irqaction_new->devname = (void *)devname;
    irqaction_new->dev_id = dev_id;
    irqaction_new->flags = irqflags;
    irqaction_new->usermode = cpu_in_user_mode();
    irqaction_new->pid = 0;
    dlist_init(&(irqaction_new->action_list));
    //irqaction_new->irq_q = NULL;
    ret = irq_setup_action(irq, irqdesc, irqaction_new, irqaction_new->usermode);
    if(ret < 0){
        aos_free(irqaction_new);
        irq_put_desc_unlock(irqdesc,cpsr);
        return ret;
    }
    ret = __register_irq_global (irq, handler, thread_fn, irqflags, devname, dev_id);
    if(ret < 0){
        irq_free_action(irq, irqdesc, dev_id, irqaction_new->usermode);
        aos_free(irqaction_new);
        irq_put_desc_unlock(irqdesc,cpsr);
        return ret;
    }
    /* edge or level */
    if(irqdesc->irq_data.chip->irq_set_type){
        irqdesc->irq_data.chip->irq_set_type(&irqdesc->irq_data, irqflags);
    }
    /* enable
    if(0 == desc->depth){
        desc->chip->irq_enable(&desc->irq_data);
    }*/

    irq_put_desc_unlock(irqdesc,cpsr);

    return 0;
}

void *aos_free_irq(int irq, void *dev_id)
{
    CPSR_ALLOC();
    irq_desc_t * irqdesc;
    int ret;
    char *devname;
    irq_action_t * irqaction;

    if(irq < IRQ_NUM_START && irq >= IRQ_NUM_MAX){
        return NULL;
    }

    irqdesc = irq_get_desc_lock(irq, &cpsr);
    ret = __unregister_irq_global (irq, dev_id);
    if(ret < 0){
        irq_put_desc_unlock(irqdesc,cpsr);
        return NULL;
    }

    /* rm irqaction */
    irqaction = irq_free_action(irq, irqdesc, dev_id, cpu_in_user_mode());
    if(NULL == irqaction){
        irq_put_desc_unlock(irqdesc,cpsr);
        return NULL;
    }
    devname = irqaction->devname;
    aos_free(irqaction);

    irq_put_desc_unlock(irqdesc,cpsr);

    return devname;
}

void aos_free_allirq(int irq)
{
    CPSR_ALLOC();
    irq_desc_t * irqdesc;
    int ret;

    if(irq < IRQ_NUM_START || irq >= IRQ_NUM_MAX){
        return;
    }
    irqdesc = irq_get_desc_lock(irq, &cpsr);

    ret = __unregister_allirq_global(irq);
    if(ret < 0){
        irq_put_desc_unlock(irqdesc,cpsr);
        return;
    }

    /* rm irqaction */
    irq_free_all_action(irq, irqdesc, cpu_in_user_mode());

    irq_put_desc_unlock(irqdesc,cpsr);

    return;
}

void aos_enable_irq(int irq)
{
    /*
    CPSR_ALLOC();
    irq_desc * irqdesc;

    if(irq < IRQ_NUM_START && irq >= IRQ_NUM_MAX){
        return;
    }
    irqdesc = irq_get_desc_lock(irq, &cpsr);

    if(0 == irqdesc->depth){
        if(irqdesc->irq_data.chip->irq_enable){
            irqdesc->irq_data.chip->irq_enable(&(irqdesc->irq_data));
        }
    }else{
        irqdesc->depth--;
    }

    irq_put_desc_unlock(irqdesc,cpsr);*/
    if(irq < IRQ_NUM_START && irq >= IRQ_NUM_MAX){
        return;
    }

    irq_desc_t * irqdesc = irq_to_desc(irq);
    if(NULL == irqdesc){
        return;
    }
    if(!aos_irq_is_init()){
        aos_irq_system_init();
    }
    if(irqdesc->irq_data.chip->irq_enable){
        irqdesc->irq_data.chip->irq_enable(&(irqdesc->irq_data));
    }
    return;
}

void aos_disable_irq(int irq)
{
    /*
    irq_action_t * irqaction;
    CPSR_ALLOC();
    irq_desc * irqdesc;

    if(irq < IRQ_NUM_START && irq >= IRQ_NUM_MAX){
        return ;
    }
    irqdesc = irq_get_desc_lock(irq, &cpsr);
    if(!irqdesc->depth++){
        if(irqdesc->irq_data.chip->irq_disable){
            irqdesc->irq_data.chip->irq_disable(&(irqdesc->irq_data));
        }
    }

    irq_put_desc_unlock(irqdesc,cpsr);*/
    if(irq < IRQ_NUM_START && irq >= IRQ_NUM_MAX){
        return;
    }
    irq_desc_t * irqdesc = irq_to_desc(irq);
    if(NULL == irqdesc){
        return;
    }
    if(!aos_irq_is_init()){
        aos_irq_system_init();
    }
    if(irqdesc->irq_data.chip->irq_disable){
        irqdesc->irq_data.chip->irq_disable(&(irqdesc->irq_data));
    }

    return;
}

void aos_set_nmi_irq(int irq)
{
    /*
    CPSR_ALLOC();
    irq_desc * irqdesc;
    int ret;

    if(irq < IRQ_NUM_START && irq >= IRQ_NUM_MAX){
        return ;
    }
    irqdesc = irq_get_desc_lock(irq, &cpsr);

    if(irqdesc->irq_data.chip->irq_set_nmi){
        irqdesc->irq_data.chip->irq_set_nmi(&irqdesc->irq_data);
    }

    irq_put_desc_unlock(irqdesc,cpsr);*/

    if(irq < IRQ_NUM_START && irq >= IRQ_NUM_MAX){
        return;
    }
    irq_desc_t * irqdesc = irq_to_desc(irq);
    if(NULL == irqdesc){
        return;
    }
    if(!aos_irq_is_init()){
        aos_irq_system_init();
    }
    if(irqdesc->irq_data.chip->irq_set_nmi){
        irqdesc->irq_data.chip->irq_set_nmi(&irqdesc->irq_data);
    }

    return;
}

void aos_send_mask_ipi(int irq, unsigned int cpumask)
{
    /*
    CPSR_ALLOC();
    irq_desc * irqdesc;
    int ret;

    if(irq < IRQ_NUM_START && irq >= IRQ_NUM_MAX){
        return ;
    }
    irqdesc = irq_get_desc_lock(irq, &cpsr);

    if(irqdesc->irq_data.chip->ipi_send_mask){
        irqdesc->irq_data.chip->ipi_send_mask(&irqdesc->irq_data, cpumask);
    }

    irq_put_desc_unlock(irqdesc,cpsr);*/

    if(irq < IRQ_NUM_START && irq >= IRQ_NUM_MAX){
        return;
    }
    irq_desc_t * irqdesc = irq_to_desc(irq);
    if(NULL == irqdesc){
        return;
    }
    if(!aos_irq_is_init()){
        aos_irq_system_init();
    }
    if(irqdesc->irq_data.chip->ipi_send_mask){
        irqdesc->irq_data.chip->ipi_send_mask(&irqdesc->irq_data, cpumask);
    }

    return;
}

int aos_irq_is_active(int irq)
{
    /*
    CPSR_ALLOC();
    irq_desc * irqdesc;
    int ret;

    if(irq < IRQ_NUM_START && irq >= IRQ_NUM_MAX){
        return false;
    }
    irqdesc = irq_get_desc_lock(irq, &cpsr);

    if(irqdesc->irq_data.chip->irq_is_active){
        ret = irqdesc->irq_data.chip->irq_is_active(&irqdesc->irq_data);
    }else{
        PRINTF_E("irq:%d,aos_irq_is_active is not support!\r\n", irq);
        return false;
    }

    irq_put_desc_unlock(irqdesc,cpsr);*/

    int ret;
    if(irq < IRQ_NUM_START && irq >= IRQ_NUM_MAX){
        return false;
    }
    irq_desc_t * irqdesc = irq_to_desc(irq);
    if(NULL == irqdesc){
        return false;
    }
    if(!aos_irq_is_init()){
        aos_irq_system_init();
    }
    if(irqdesc->irq_data.chip->irq_is_active){
        ret = irqdesc->irq_data.chip->irq_is_active(&irqdesc->irq_data);
    }else{
        PRINTF_E("irq:%d,aos_irq_is_active is not support!\r\n", irq);
        return false;
    }
    return ret;
}

int aos_irq_set_pending(int irq)
{
    int ret;
    if(irq < IRQ_NUM_START && irq >= IRQ_NUM_MAX){
        return false;
    }
    irq_desc_t * irqdesc = irq_to_desc(irq);
    if(NULL == irqdesc){
        return false;
    }
    if(!aos_irq_is_init()){
        aos_irq_system_init();
    }
    if(irqdesc->irq_data.chip->irq_set_pending){
        ret = irqdesc->irq_data.chip->irq_set_pending(&irqdesc->irq_data);
    }else{
        PRINTF_E("irq:%d,aos_irq_set_pending is not support!\r\n", irq);
        return false;
    }
    return ret;
}

int aos_irq_clear_pending(int irq)
{
    int ret;
    if(irq < IRQ_NUM_START && irq >= IRQ_NUM_MAX){
        return false;
    }
    irq_desc_t * irqdesc = irq_to_desc(irq);
    if(NULL == irqdesc){
        return false;
    }
    if(!aos_irq_is_init()){
        aos_irq_system_init();
    }

    if(irqdesc->irq_data.chip->irq_clear_pending){
        ret = irqdesc->irq_data.chip->irq_clear_pending(&irqdesc->irq_data);
    }else{
        PRINTF_E("irq:%d,aos_irq_clear_pending is not support!\r\n", irq);
        return false;
    }
    return ret;
}

int aos_set_priority_irq(int irq, unsigned int priority)
{
    /*
    CPSR_ALLOC();
    irq_desc * irqdesc;
    int ret = -1;

    if(irq < IRQ_NUM_START && irq >= IRQ_NUM_MAX){
        return false;
    }
    irqdesc = irq_get_desc_lock(irq, &cpsr);

    if(irqdesc->irq_data.chip->irq_set_priority){
        ret = irqdesc->irq_data.chip->irq_set_priority(&irqdesc->irq_data, unsigned int priority);
    }

    irq_put_desc_unlock(irqdesc,cpsr);*/
    int ret = -1;

    if(irq < IRQ_NUM_START && irq >= IRQ_NUM_MAX){
        return -1;
    }
    irq_desc_t * irqdesc = irq_to_desc(irq);
    if(NULL == irqdesc){
        return -1;
    }
    if(!aos_irq_is_init()){
        aos_irq_system_init();
    }
    if(irqdesc->irq_data.chip->irq_set_priority){
        ret = irqdesc->irq_data.chip->irq_set_priority(&irqdesc->irq_data, priority);
    }else{
        PRINTF_E("irq:%d,aos_set_priority_irq is not support!\r\n", irq);
        return false;
    }
    return ret;
}

int aos_get_active_irq(void)
{
    return __irq_get_active_irq_svc();
}

