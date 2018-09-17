/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>

#if (RHINO_CONFIG_SYSTEM_STATS > 0)
void kobj_list_init(void)
{
    klist_init(&(g_kobj_list.task_head));
    klist_init(&(g_kobj_list.mutex_head));

#if (RHINO_CONFIG_MM_BLK > 0)
    klist_init(&(g_kobj_list.mblkpool_head));
#endif

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
    cpu_stack_t *stack_start;
    uint8_t      i;

    stack_start = g_active_task[cpu_cur_get()]->task_stack_base;

    for (i = 0; i < RHINO_CONFIG_STK_CHK_WORDS; i++) {
        if (*stack_start++ != RHINO_TASK_STACK_OVF_MAGIC) {
            k_err_proc(RHINO_TASK_STACK_OVF);
        }
    }

    if ((cpu_stack_t *)(g_active_task[cpu_cur_get()]->task_stack) < stack_start) {
        k_err_proc(RHINO_TASK_STACK_OVF);
    }
}

#else

void krhino_stack_ovf_check(void)
{
    cpu_stack_t *stack_start;
    cpu_stack_t *stack_end;
    uint8_t      i;

    stack_start = g_active_task[cpu_cur_get()]->task_stack_base;
    stack_end   = stack_start + g_active_task[cpu_cur_get()]->stack_size
                  - RHINO_CONFIG_STK_CHK_WORDS;

    for (i = 0; i < RHINO_CONFIG_STK_CHK_WORDS; i++) {
        if (*stack_end++ != RHINO_TASK_STACK_OVF_MAGIC) {
            k_err_proc(RHINO_TASK_STACK_OVF);
        }
    }

    if ((cpu_stack_t *)(g_active_task[cpu_cur_get()]->task_stack) > stack_end) {
        k_err_proc(RHINO_TASK_STACK_OVF);
    }
}
#endif
#endif

#if (RHINO_CONFIG_TASK_SCHED_STATS > 0)
void krhino_task_sched_stats_reset(void)
{
    lr_timer_t cur_time;
    uint32_t   i;

#if (RHINO_CONFIG_INTRPT_STATS > 0)
    g_cur_intrpt_disable_max_time = 0;
#endif

#if (RHINO_CONFIG_SCHED_STATS > 0)
    g_cur_sched_disable_max_time = 0;
#endif

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

#if (RHINO_CONFIG_INTRPT_STATS > 0)
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
#endif

#if (RHINO_CONFIG_SCHED_STATS > 0)

    if (g_active_task[cpu_cur_get()]->task_sched_disable_time_max < g_cur_sched_disable_max_time) {
        g_active_task[cpu_cur_get()]->task_sched_disable_time_max = g_cur_sched_disable_max_time;
    }

    g_cur_sched_disable_max_time = 0;
#endif

    /* Keep track of new task and total system context switch times */
    g_preferred_ready_task[cpu_cur_get()]->task_ctx_switch_times++;
    g_sys_ctx_switch_times++;

    cur_time   = (lr_timer_t)LR_COUNT_GET();
    exec_time  = cur_time - g_active_task[cpu_cur_get()]->task_time_start;

    g_active_task[cpu_cur_get()]->task_time_total_run += (sys_time_t)exec_time;

    g_preferred_ready_task[cpu_cur_get()]->task_time_start = cur_time;
}
#endif /* RHINO_CONFIG_TASK_SCHED_STATS */

#if (RHINO_CONFIG_INTRPT_STATS > 0)
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

#if (RHINO_CONFIG_TASK_SCHED_STATS > 0)

static uint32_t task_cpu_usage_period = 0;

void krhino_task_cpu_usage_stats()
{
    klist_t *taskhead = &g_kobj_list.task_head;
    klist_t *taskend  = taskhead;
    klist_t *tmp;
    ktask_t *task;

    static lr_timer_t stats_start = 0;
    lr_timer_t stats_end;

    CPSR_ALLOC();
    RHINO_CPU_INTRPT_DISABLE();
    for (tmp = taskhead->next; tmp != taskend; tmp = tmp->next) {
        task = krhino_list_entry(tmp, ktask_t, task_stats_item);
        task->task_exec_time =
            task->task_time_total_run - task->task_time_total_run_prev;
        task->task_time_total_run_prev = task->task_time_total_run;
    }
    RHINO_CPU_INTRPT_ENABLE();

    stats_end = (lr_timer_t)LR_COUNT_GET();
    task_cpu_usage_period = stats_end - stats_start;
    stats_start = stats_end;
}

void krhino_total_cpu_usage_show()
{
    klist_t *taskhead = &g_kobj_list.task_head;
    klist_t *taskend  = taskhead;
    klist_t *tmp;
    ktask_t *task;
    char* task_name;
    lr_timer_t task_cpu_usage;
    uint32_t total_cpu_usage;

    total_cpu_usage = krhino_total_cpu_usage_get();
    printf("-----------------------\n");
    printf("CPU usage :%3d.%02d%%  \n", total_cpu_usage/100, total_cpu_usage%100);
    printf("-----------------------\n");

    printf("Name               %CPU\n");
    printf("-----------------------\n");

    krhino_sched_disable();

    for (tmp = taskhead->next; tmp != taskend; tmp = tmp->next) {
        task = krhino_list_entry(tmp, ktask_t, task_stats_item);

        if (task->task_name != NULL) {
            task_name = task->task_name;
        } else {
            task_name = "anonym";
        }
        task_cpu_usage = krhino_task_cpu_usage_get(task);
        printf("%-19s%3d.%02d\n", task_name, task_cpu_usage/100, task_cpu_usage%100);
    }
    printf("-----------------------\n");
    krhino_sched_enable();
}

/* one in ten thousand */
uint32_t krhino_task_cpu_usage_get(ktask_t *task)
{
    if (task_cpu_usage_period == 0) {
        return 0;
    }

    return ((uint64_t)(task->task_exec_time) * 10000 / task_cpu_usage_period);
}

/* one in ten thousand */
uint32_t krhino_total_cpu_usage_get()
{
    printf("cpu usage period = %d\n", task_cpu_usage_period/80000000);
    return (10000 - krhino_task_cpu_usage_get(&g_idle_task[0]));
}

#if (RHINO_CONFIG_CPU_USAGE_PERIOD > 0)
static ktimer_t cpu_usage_timer;

static void cpu_usage_timer_handler(void* timer, void* args)
{
    krhino_task_cpu_usage_stats();
}

kstat_t krhino_task_cpu_usage_init()
{
    kstat_t ret = RHINO_SUCCESS;
    sys_time_t cpu_usage_period = krhino_ms_to_ticks(RHINO_CONFIG_CPU_USAGE_PERIOD);

    if ( cpu_usage_period == 0) {
        printf("cpu usage period is too short\n");
        return RHINO_INV_PARAM;
    }

    ret = krhino_timer_create(&cpu_usage_timer, "cpu_usage_timer", cpu_usage_timer_handler, 1, cpu_usage_period, NULL, 1);
    if (ret != RHINO_SUCCESS) {
        printf("task cpu uasge init error\n");
        return ret;
    }

    return ret;
}

#endif

#endif

