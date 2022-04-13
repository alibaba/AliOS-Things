/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "k_api.h"
#include "object_api.h"

#if (AOS_KERNEL_OBJECT > 0)
#include "object_api.h"
#endif

RHINO_INLINE void rhino_stack_check_init(void)
{
#if (RHINO_CONFIG_INTRPT_STACK_OVF_CHECK > 0)
#if (RHINO_CONFIG_CPU_STACK_DOWN > 0)
    *g_intrpt_stack_bottom = RHINO_INTRPT_STACK_OVF_MAGIC;
#else
    *g_intrpt_stack_top = RHINO_INTRPT_STACK_OVF_MAGIC;
#endif
#endif /* RHINO_CONFIG_INTRPT_STACK_OVF_CHECK */

#if (RHINO_CONFIG_STACK_OVF_CHECK_HW != 0)
    cpu_intrpt_stack_protect();
#endif
}

kstat_t krhino_init(void)
{
    g_sys_stat = RHINO_STOPPED;
    memset(g_task_id, 0, sizeof(g_task_id));

#if (RHINO_CONFIG_CPU_NUM > 1)
    krhino_spin_lock_init(&g_sys_lock);
    klist_init(&g_task_del_head);
    extern struct _reent g_impure_reent_int[];
    for (uint8_t i = 0; i < RHINO_CONFIG_CPU_NUM - 1; i++) {
        /* use newlibc reent */
        _REENT_INIT_PTR(&(g_impure_reent_int[i]));
    }
#endif

    runqueue_init(&g_ready_queue);

    tick_list_init();

#if (RHINO_CONFIG_KOBJ_LIST > 0)
    kobj_list_init();
#endif

#if (RHINO_CONFIG_USER_HOOK > 0)
    krhino_init_hook();
#endif

#if (RHINO_CONFIG_MM_TLF > 0)
    k_mm_init();
#endif

#if (RHINO_CONFIG_KOBJ_DYN_ALLOC > 0)
    klist_init(&g_res_list);
    (void)krhino_sem_create(&g_res_sem, "res_sem", 0);
    dyn_mem_proc_task_start();
#endif

#if (RHINO_CONFIG_CPU_NUM > 1)
    for (uint8_t i = 0; i < RHINO_CONFIG_CPU_NUM; i++) {
        krhino_task_cpu_create(&g_idle_task[i], "idle_task", NULL, RHINO_IDLE_PRI, 0,
                               &g_idle_task_stack[i][0], RHINO_CONFIG_IDLE_TASK_STACK_SIZE,
                               idle_task, i, 1u);
    }
#else
    krhino_task_create(&g_idle_task[0], "idle_task", NULL, RHINO_IDLE_PRI, 0,
                       &g_idle_task_stack[0][0], RHINO_CONFIG_IDLE_TASK_STACK_SIZE,
                       idle_task, 1u);
#endif

#if (RHINO_CONFIG_WORKQUEUE > 0)
    workqueue_init();
#endif

#if (RHINO_CONFIG_TIMER > 0)
    ktimer_init();
#endif

    rhino_stack_check_init();

#if (AOS_KERNEL_OBJECT > 0)
    obj_init_all();
#endif
    krhino_futex_init();
    return RHINO_SUCCESS;
}

kstat_t krhino_start(void)
{
    ktask_t *preferred_task;

    if (g_sys_stat == RHINO_STOPPED) {
#if (RHINO_CONFIG_CPU_NUM > 1)
        for (uint8_t i = 0; i < RHINO_CONFIG_CPU_NUM; i++) {
            preferred_task            = preferred_cpu_ready_task_get(&g_ready_queue, i);
            preferred_task->cpu_num   = i;
            preferred_task->cur_exc   = 1;
            g_preferred_ready_task[i] = preferred_task;
            g_active_task[i]          = g_preferred_ready_task[i];
            g_active_task[i]->cur_exc = 1;
        }
#else
        preferred_task = preferred_cpu_ready_task_get(&g_ready_queue, 0);
        g_preferred_ready_task[0] = preferred_task;
        g_active_task[0] = preferred_task;
#endif

#if (RHINO_CONFIG_USER_HOOK > 0)
        krhino_start_hook();
#endif

        g_sys_stat = RHINO_RUNNING;
        cpu_first_task_start();

        /* should not be here */
        return RHINO_SYS_FATAL_ERR;
    }

    return RHINO_RUNNING;
}


#if (RHINO_CONFIG_INTRPT_STACK_OVF_CHECK > 0)
#if (RHINO_CONFIG_CPU_STACK_DOWN > 0)
void krhino_intrpt_stack_ovf_check(void)
{
    if (*g_intrpt_stack_bottom != RHINO_INTRPT_STACK_OVF_MAGIC) {
        k_err_proc(RHINO_INTRPT_STACK_OVF);
    }
}
#else
void krhino_intrpt_stack_ovf_check(void)
{
    if (*g_intrpt_stack_top != RHINO_INTRPT_STACK_OVF_MAGIC) {
        k_err_proc(RHINO_INTRPT_STACK_OVF);
    }
}
#endif
#endif /* RHINO_CONFIG_INTRPT_STACK_OVF_CHECK */

#if (RHINO_CONFIG_FIQ_AS_NMI > 0)
    extern cpu_cpsr_t cpu_intrpt_all_save(void);
    extern void cpu_intrpt_all_restore(cpu_cpsr_t cpsr);
    #define NMI_ALLOC()    cpu_cpsr_t _cpsr_
    #define NMI_DISABLE()  do{_cpsr_ = cpu_intrpt_all_save();}while(0)
    #define NMI_ENABLE()   do{cpu_intrpt_all_restore(_cpsr_);}while(0)
#else
    #define NMI_ALLOC()
    #define NMI_DISABLE()
    #define NMI_ENABLE()
#endif

kstat_t krhino_intrpt_enter(void)
{
    CPSR_ALLOC();
    NMI_ALLOC();

#if (RHINO_CONFIG_INTRPT_STACK_OVF_CHECK > 0)
    krhino_intrpt_stack_ovf_check();
#endif

    RHINO_CPU_INTRPT_DISABLE();
    NMI_DISABLE();
    g_intrpt_nested_level[cpu_cur_get()]++;
    NMI_ENABLE();
    RHINO_CPU_INTRPT_ENABLE();

#if (RHINO_CONFIG_PWRMGMT > 0)
    cpu_pwr_up();
#endif

    return RHINO_SUCCESS;
}

void krhino_intrpt_exit(void)
{
    CPSR_ALLOC();
    NMI_ALLOC();
    uint8_t  cur_cpu_num;
    ktask_t *preferred_task;

#if (RHINO_CONFIG_INTRPT_STACK_OVF_CHECK > 0)
    krhino_intrpt_stack_ovf_check();
#endif

    RHINO_CPU_INTRPT_DISABLE();

    cur_cpu_num = cpu_cur_get();

    NMI_DISABLE();
    g_intrpt_nested_level[cur_cpu_num]--;
    NMI_ENABLE();

#if (RHINO_CONFIG_CPU_NUM > 1)
    if (g_sched_needed[cur_cpu_num] == 0u) {
        g_sched_needed[cur_cpu_num] = 1u;
        RHINO_CPU_INTRPT_ENABLE();
        return;
    }
#endif

    if (g_intrpt_nested_level[cur_cpu_num] > 0u) {
        RHINO_CPU_INTRPT_ENABLE();
        return;
    }

    if (g_sched_lock[cur_cpu_num] > 0u) {
        RHINO_CPU_INTRPT_ENABLE();
        return;
    }

    preferred_task = preferred_cpu_ready_task_get(&g_ready_queue, cur_cpu_num);
#if (RHINO_CONFIG_SCHED_CFS > 0)
    preferred_task = preferred_cfs_ready_task_get(preferred_task, cur_cpu_num);
    if (preferred_task == NULL) {
        RHINO_CPU_INTRPT_ENABLE();
        return;
    }
#endif
    if (preferred_task == g_active_task[cur_cpu_num]) {
        RHINO_CPU_INTRPT_ENABLE();
        return;
    }

#if (RHINO_CONFIG_USER_HOOK > 0)
    krhino_task_switch_hook(g_active_task[cur_cpu_num], preferred_task);
#endif

    TRACE_INTRPT_TASK_SWITCH(g_active_task[cur_cpu_num], preferred_task);

#if (RHINO_SCHED_NONE_PREEMPT > 0)
    if (g_active_task[cur_cpu_num] == &g_idle_task[cur_cpu_num]) {
#endif
#if (RHINO_CONFIG_CPU_NUM > 1)
        g_active_task[cur_cpu_num]->cur_exc = 0;
        preferred_task->cpu_num             = cur_cpu_num;
        preferred_task->cur_exc             = 1;
#endif
        g_preferred_ready_task[cur_cpu_num] = preferred_task;
        cpu_intrpt_switch();
#if (RHINO_SCHED_NONE_PREEMPT > 0)
    }
#endif
    RHINO_CPU_INTRPT_ENABLE();
}

tick_t krhino_next_sleep_ticks_get(void)
{
    CPSR_ALLOC();

    klist_t *tick_head;
    ktask_t *tcb;
    klist_t *iter;
    tick_t   ticks;

    tick_head = &g_tick_head;

    RHINO_CRITICAL_ENTER();
    if (tick_head->next == &g_tick_head) {
        RHINO_CRITICAL_EXIT();
        return RHINO_WAIT_FOREVER;
    }

    iter  = tick_head->next;
    tcb   = krhino_list_entry(iter, ktask_t, tick_list);
    ticks = tcb->tick_match - g_tick_count;
    RHINO_CRITICAL_EXIT();

    return ticks;
}


size_t krhino_global_space_get(void)
{
    size_t mem;

    mem = sizeof(g_sys_stat) + sizeof(g_idle_task_spawned) + sizeof(g_ready_queue)
          + sizeof(g_sched_lock) + sizeof(g_intrpt_nested_level) + sizeof(g_preferred_ready_task)
          + sizeof(g_active_task) + sizeof(g_idle_task) + sizeof(g_idle_task_stack)
          + sizeof(g_tick_head) + sizeof(g_tick_count) + sizeof(g_idle_count);

#if (RHINO_CONFIG_TIMER > 0)
    mem += sizeof(g_timer_head) + sizeof(g_timer_count)
           + sizeof(g_timer_task) + sizeof(g_timer_task_stack)
           + sizeof(g_timer_queue) + sizeof(timer_queue_cb);
#endif

#if (RHINO_CONFIG_KOBJ_LIST > 0)
    mem += sizeof(g_kobj_list);
#endif

    return mem;
}

uint32_t krhino_version_get(void)
{
    return RHINO_VERSION;
}

