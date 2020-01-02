/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include "k_api.h"
#include "debug_api.h"

#if (RHINO_CONFIG_SYS_STATS > 0)

typedef struct {
    char     task_name[32];
    uint32_t task_cpu_usage;
} task_cpuusage_info;

static uint32_t task_cpu_usage_period = 0;

void debug_task_cpu_usage_stats(void)
{
    uint32_t   i;
    klist_t    *taskhead = &g_kobj_list.task_head;
    klist_t    *taskend  = taskhead;
    klist_t    *tmp;
    ktask_t    *task;
    lr_timer_t cur_time;
    lr_timer_t exec_time;
    lr_timer_t stats_end;

    static lr_timer_t stats_start = 0;

    (void)i; /* to avoid compiler warning */

    CPSR_ALLOC();
    RHINO_CRITICAL_ENTER();
#if (RHINO_CONFIG_CPU_NUM > 1)
    for (i = 0; i < RHINO_CONFIG_CPU_NUM; i++) {
        cur_time  = LR_COUNT_GET();
        exec_time = cur_time - g_active_task[i]->task_time_start;

        g_active_task[i]->task_time_total_run += (uint64_t)exec_time;
        g_active_task[i]->task_time_start     = cur_time;
    }
#else
    cur_time  = LR_COUNT_GET();
    exec_time = cur_time - g_active_task[0]->task_time_start;

    g_active_task[0]->task_time_total_run += (uint64_t)exec_time;
    g_active_task[0]->task_time_start     = cur_time;
#endif

    for (tmp = taskhead->next; tmp != taskend; tmp = tmp->next) {
        task = krhino_list_entry(tmp, ktask_t, task_stats_item);
        task->task_exec_time = (lr_timer_t)(task->task_time_total_run -
                               task->task_time_total_run_prev);
        task->task_time_total_run_prev = task->task_time_total_run;
    }
    RHINO_CRITICAL_EXIT();

    stats_end             = LR_COUNT_GET();
    task_cpu_usage_period = stats_end - stats_start;
    stats_start           = stats_end;
}

void debug_total_cpu_usage_show(void)
{
    uint32_t    i;
    klist_t    *taskhead = &g_kobj_list.task_head;
    klist_t    *taskend  = taskhead;
    klist_t    *tmp;
    ktask_t    *task;
    const char *task_name;
    lr_timer_t  task_cpu_usage;
    uint32_t    total_cpu_usage[RHINO_CONFIG_CPU_NUM];

    uint32_t            tasknum = 0;
    task_cpuusage_info *taskinfo;
    task_cpuusage_info *taskinfoeach;

    printf("-----------------------\n");
#if (RHINO_CONFIG_CPU_NUM > 1)
    for (i = 0; i < RHINO_CONFIG_CPU_NUM; i++) {
        total_cpu_usage[i] = debug_total_cpu_usage_get(i);
        printf("CPU%d usage :%3d.%02d%%  \n", i, (int)total_cpu_usage[i]/100,
               (int)total_cpu_usage[i]%100);
    }
#else
    total_cpu_usage[0] = debug_total_cpu_usage_get(0);
    printf("CPU usage :%3d.%02d%%  \n", (int)total_cpu_usage[0] / 100,
           (int)total_cpu_usage[0] % 100);
#endif

    printf("-----------------------\n");
    printf("Name               %%CPU\n");
    printf("-----------------------\n");

    krhino_sched_disable();
    for (tmp = taskhead->next; tmp != taskend; tmp = tmp->next) {
        tasknum ++;
    }
    taskinfo = krhino_mm_alloc(tasknum * sizeof(task_cpuusage_info));
    if (taskinfo ==  NULL) {
        krhino_sched_enable();
    }
    memset(taskinfo, 0, tasknum * sizeof(task_cpuusage_info));

    taskinfoeach = taskinfo;

    for (tmp = taskhead->next; tmp != taskend; tmp = tmp->next) {
        task = krhino_list_entry(tmp, ktask_t, task_stats_item);

        if (task->task_name != NULL) {
            task_name = task->task_name;
        } else {
            task_name = "anonym";
        }
        taskinfoeach->task_cpu_usage = debug_task_cpu_usage_get(task);
        strncpy(taskinfoeach->task_name, task_name, sizeof(taskinfoeach->task_name) - 1);
        taskinfoeach++;
    }
    krhino_sched_enable();

    for (i = 0; i < tasknum; i++) {
        taskinfoeach   = taskinfo + i;
        task_name      = taskinfoeach->task_name;
        task_cpu_usage = taskinfoeach->task_cpu_usage;

        printf("%-19s%3d.%02d\n", task_name, (int)task_cpu_usage / 100,
               (int)task_cpu_usage % 100);
    }

    krhino_mm_free(taskinfo);

    printf("-----------------------\n");
}

/* one in ten thousand */
uint32_t debug_task_cpu_usage_get(ktask_t *task)
{
    uint32_t task_cpu_usage = 0;

    if (task_cpu_usage_period == 0) {
        return 0;
    }

    task_cpu_usage = (uint64_t)(task->task_exec_time) * 10000 / task_cpu_usage_period;
    if (task_cpu_usage > 10000) {
        task_cpu_usage = 10000;
    }

    return task_cpu_usage;
}

/* one in ten thousand */
uint32_t debug_total_cpu_usage_get(uint32_t cpuid)
{
    uint32_t total_cpu_usage = 0;
    total_cpu_usage          = 10000 - debug_task_cpu_usage_get(&g_idle_task[cpuid]);
    return total_cpu_usage;
}

#endif /* (RHINO_CONFIG_SYS_STATS > 0) */
