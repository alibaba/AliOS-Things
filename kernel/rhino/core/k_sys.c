/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>

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

#if (RHINO_CONFIG_USER_HOOK > 0)
    krhino_init_hook();
#endif

    runqueue_init(&g_ready_queue);

    tick_list_init();

#if (RHINO_CONFIG_SYSTEM_STATS > 0)
    kobj_list_init();
#endif

#if (RHINO_CONFIG_MM_TLF > 0)
    k_mm_init();
#endif

#if (RHINO_CONFIG_KOBJ_DYN_ALLOC > 0)
    klist_init(&g_res_list);
    krhino_sem_create(&g_res_sem, "res_sem", 0);
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

#if (RHINO_CONFIG_CPU_USAGE_STATS > 0)
    cpu_usage_stats_start();
#endif

    rhino_stack_check_init();

    return RHINO_SUCCESS;
}

kstat_t krhino_start(void)
{
    if (g_sys_stat == RHINO_STOPPED) {
#if (RHINO_CONFIG_CPU_NUM > 1)
        for (uint8_t i = 0; i < RHINO_CONFIG_CPU_NUM; i++) {
            preferred_cpu_ready_task_get(&g_ready_queue, i);
            g_active_task[i] = g_preferred_ready_task[i];
            g_active_task[i]->cur_exc = 1;
        }
#else
        preferred_cpu_ready_task_get(&g_ready_queue, 0);
        g_active_task[0] = g_preferred_ready_task[0];
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

kstat_t krhino_intrpt_enter(void)
{
    CPSR_ALLOC();

#if (RHINO_CONFIG_INTRPT_STACK_OVF_CHECK > 0)
    krhino_intrpt_stack_ovf_check();
#endif

    RHINO_CPU_INTRPT_DISABLE();

#if (RHINO_CONFIG_CPU_PWR_MGMT > 0)
    cpu_pwr_up();
#endif

    g_intrpt_nested_level[cpu_cur_get()]++;

    RHINO_CPU_INTRPT_ENABLE();

    return RHINO_SUCCESS;
}

void krhino_intrpt_exit(void)
{
    CPSR_ALLOC();
    uint8_t cur_cpu_num;

#if (RHINO_CONFIG_INTRPT_STACK_OVF_CHECK > 0)
    krhino_intrpt_stack_ovf_check();
#endif

    RHINO_CPU_INTRPT_DISABLE();

    cur_cpu_num = cpu_cur_get();

    g_intrpt_nested_level[cur_cpu_num]--;

    if (g_intrpt_nested_level[cur_cpu_num] > 0u) {
        RHINO_CPU_INTRPT_ENABLE();
        return;
    }

    if (g_sched_lock[cur_cpu_num] > 0u) {
        RHINO_CPU_INTRPT_ENABLE();
        return;
    }

    preferred_cpu_ready_task_get(&g_ready_queue, cur_cpu_num);

    if (g_preferred_ready_task[cur_cpu_num] == g_active_task[cur_cpu_num]) {
        RHINO_CPU_INTRPT_ENABLE();
        return;
    }

    TRACE_INTRPT_TASK_SWITCH(g_active_task[cur_cpu_num], g_preferred_ready_task[cur_cpu_num]);

#if (RHINO_CONFIG_CPU_NUM > 1)
    g_active_task[cur_cpu_num]->cur_exc = 0;
#endif

    cpu_intrpt_switch();

    RHINO_CPU_INTRPT_ENABLE();
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

#if (RHINO_CONFIG_SYSTEM_STATS > 0)
    mem += sizeof(g_kobj_list);
#endif

    return mem;
}

uint32_t krhino_version_get(void)
{
    return RHINO_VERSION;
}

