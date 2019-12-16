/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>

#include "k_api.h"

#if (RHINO_CONFIG_KOBJ_LIST > 0)
void kobj_list_init(void)
{
    klist_init(&(g_kobj_list.task_head));
    klist_init(&(g_kobj_list.mutex_head));

#if (RHINO_CONFIG_SEM > 0)
    klist_init(&(g_kobj_list.sem_head));
#endif

#if (RHINO_CONFIG_QUEUE > 0)
    klist_init(&(g_kobj_list.queue_head));
#endif

#if (RHINO_CONFIG_BUF_QUEUE > 0)
    klist_init(&(g_kobj_list.buf_queue_head));
#endif

#if (RHINO_CONFIG_EVENT_FLAG > 0)
    klist_init(&(g_kobj_list.event_head));
#endif
}
#endif

#if (RHINO_CONFIG_TASK_STACK_OVF_CHECK > 0)
#if (RHINO_CONFIG_CPU_STACK_DOWN > 0)
void krhino_stack_ovf_check(void)
{
    ktask_t     *cur;
    cpu_stack_t *stack_start;
    uint8_t      i;

    cur = g_active_task[cpu_cur_get()];
    stack_start = cur->task_stack_base;

    for (i = 0; i < RHINO_CONFIG_STK_CHK_WORDS; i++) {
        if (*stack_start++ != RHINO_TASK_STACK_OVF_MAGIC) {
            k_err_proc(RHINO_TASK_STACK_OVF);
        }
    }

    if ((cpu_stack_t *)(cur->task_stack) < stack_start) {
        k_err_proc(RHINO_TASK_STACK_OVF);
    }

#if (RHINO_CONFIG_USER_SPACE > 0)
    if (cur->pid == 0) {
        return;
    }

    stack_start = cur->task_ustack_base;

    for (i = 0; i < RHINO_CONFIG_STK_CHK_WORDS; i++) {
        if (*stack_start++ != RHINO_TASK_STACK_OVF_MAGIC) {
            k_err_proc(RHINO_TASK_STACK_OVF);
        }
    }

    if ((cpu_stack_t *)(cur->task_ustack) < stack_start) {
        k_err_proc(RHINO_TASK_STACK_OVF);
    }
#endif
}

#else

void krhino_stack_ovf_check(void)
{
    ktask_t     *cur;
    cpu_stack_t *stack_start;
    cpu_stack_t *stack_end;
    uint8_t      i;

    cur = g_active_task[cpu_cur_get()];

    stack_start = cur->task_stack_base;
    stack_end   = stack_start + cur->stack_size
                  - RHINO_CONFIG_STK_CHK_WORDS;

    for (i = 0; i < RHINO_CONFIG_STK_CHK_WORDS; i++) {
        if (*stack_end++ != RHINO_TASK_STACK_OVF_MAGIC) {
            k_err_proc(RHINO_TASK_STACK_OVF);
        }
    }

    if ((cpu_stack_t *)(cur->task_stack) > stack_end) {
        k_err_proc(RHINO_TASK_STACK_OVF);
    }

#if (RHINO_CONFIG_USER_SPACE > 0)
    if (cur->pid == 0) {
        return;
    }

    stack_start = cur->task_ustack_base;
    stack_end   = stack_start + cur->ustack_size
                  - RHINO_CONFIG_STK_CHK_WORDS;

    for (i = 0; i < RHINO_CONFIG_STK_CHK_WORDS; i++) {
        if (*stack_end++ != RHINO_TASK_STACK_OVF_MAGIC) {
            k_err_proc(RHINO_TASK_STACK_OVF);
        }
    }

    if ((cpu_stack_t *)(cur->task_ustack) > stack_end) {
        k_err_proc(RHINO_TASK_STACK_OVF);
    }
#endif
}
#endif
#endif

#if (RHINO_CONFIG_SYS_STATS > 0)
void krhino_task_sched_stats_reset(void)
{
    lr_timer_t cur_time;
    uint32_t   i;

    g_cur_intrpt_disable_max_time = 0;
    g_cur_sched_disable_max_time  = 0;

    /* system first task starting time should be measured otherwise not correct */
    cur_time = (lr_timer_t)LR_COUNT_GET();
    for (i = 0; i < RHINO_CONFIG_CPU_NUM; i++) {
        g_preferred_ready_task[i]->task_time_start = cur_time;
    }
}

void krhino_task_sched_stats_get(void)
{
    lr_timer_t cur_time;
    lr_timer_t exec_time;

    hr_timer_t intrpt_disable_time;

    if (g_cur_intrpt_disable_max_time > g_sys_measure_waste) {
        intrpt_disable_time = g_cur_intrpt_disable_max_time - g_sys_measure_waste;
    } else {
        intrpt_disable_time = 0;
    }

    if (g_active_task[cpu_cur_get()]->task_intrpt_disable_time_max < intrpt_disable_time) {
        g_active_task[cpu_cur_get()]->task_intrpt_disable_time_max = intrpt_disable_time;
    }

    g_cur_intrpt_disable_max_time = 0;


    if (g_active_task[cpu_cur_get()]->task_sched_disable_time_max < g_cur_sched_disable_max_time) {
        g_active_task[cpu_cur_get()]->task_sched_disable_time_max = g_cur_sched_disable_max_time;
    }

    g_cur_sched_disable_max_time = 0;

    /* Keep track of new task and total system context switch times */
    g_preferred_ready_task[cpu_cur_get()]->task_ctx_switch_times++;
    g_sys_ctx_switch_times++;

    cur_time  = (lr_timer_t)LR_COUNT_GET();
    exec_time = cur_time - g_active_task[cpu_cur_get()]->task_time_start;

    g_active_task[cpu_cur_get()]->task_time_total_run += (sys_time_t)exec_time;
    if (g_active_task[cpu_cur_get()]->task_state == K_RDY) {
        g_active_task[cpu_cur_get()]->task_time_this_run += (sys_time_t)exec_time;
    }
    else {
         g_active_task[cpu_cur_get()]->task_time_this_run = 0u;
    }
    g_preferred_ready_task[cpu_cur_get()]->task_time_start = cur_time;
}

void intrpt_disable_measure_start(void)
{
    g_intrpt_disable_times++;

    /* start measure interrupt disable time */
    if (g_intrpt_disable_times == 1u) {
        g_intrpt_disable_time_start = HR_COUNT_GET();
    }
}

void intrpt_disable_measure_stop(void)
{
    hr_timer_t diff;

    g_intrpt_disable_times--;

    if (g_intrpt_disable_times == 0u) {
        diff = HR_COUNT_GET() - g_intrpt_disable_time_start;

        if (g_intrpt_disable_max_time < diff) {
            g_intrpt_disable_max_time = diff;
        }

        if (g_cur_intrpt_disable_max_time < diff) {
            g_cur_intrpt_disable_max_time = diff;
        }
    }
}
#endif

#if (RHINO_CONFIG_HW_COUNT > 0)
void krhino_overhead_measure(void)
{
    hr_timer_t diff;
    hr_timer_t m1;
    hr_timer_t m2;

    m1 = HR_COUNT_GET();
    m2 = HR_COUNT_GET();

    diff = m2 - m1;

    /* measure time overhead */
    g_sys_measure_waste = diff;
}
#endif

