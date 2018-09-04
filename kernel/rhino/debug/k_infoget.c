/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "k_dbg_api.h"

ktask_t *krhino_task_find(char *name)
{
    klist_t *listnode;
    ktask_t *task;

    for (listnode = g_kobj_list.task_head.next;
         listnode != &g_kobj_list.task_head; listnode = listnode->next) {
        task = krhino_list_entry(listnode, ktask_t, task_stats_item);
        if (0 == strcmp(name, task->task_name)) {
            return task;
        }
    }

    return NULL;
}

int krhino_is_task_ready(ktask_t *task)
{
    return (task->task_state == K_RDY);
}

void *krhino_task_stack_bottom(ktask_t *task)
{
    if (task == NULL) {
        task = g_active_task[cpu_cur_get()];
    }

    if (task->task_state == K_DELETED) {
        return NULL;
    }

#if (RHINO_CONFIG_CPU_STACK_DOWN > 0)
    return task->task_stack_base + task->stack_size;
#else
    return task->task_stack_base;
#endif
}

#if (RHINO_CONFIG_TASK_SCHED_STATS > 0)

ktask_t     task_cpu_stat;
cpu_stack_t task_cpu_stat_stack[RHINO_CONFIG_CPU_USAGE_TASK_STACK];
static void task_cpu_usage_stats_entry(void *arg)
{
    (void)arg;

    klist_t *taskhead = &g_kobj_list.task_head;
    klist_t *taskend  = taskhead;
    klist_t *tmp;
    ktask_t *task;

    while (1) {
        krhino_task_sleep(RHINO_CONFIG_TICKS_PER_SECOND / 2);
        CPSR_ALLOC();
        RHINO_CPU_INTRPT_DISABLE();

        for (tmp = taskhead->next; tmp != taskend; tmp = tmp->next) {
            task = krhino_list_entry(tmp, ktask_t, task_stats_item);
            task->task_exec_time =
              task->task_time_total_run - task->task_time_total_run_prev;
            task->task_time_total_run_prev = task->task_time_total_run;
        }

        RHINO_CPU_INTRPT_ENABLE();
    }
}

void krhino_task_cpu_stats_start(void)
{
    krhino_task_create(&task_cpu_stat, "task_cpu_usage", 0,
                       RHINO_CONFIG_CPU_USAGE_TASK_PRI, 0, task_cpu_stat_stack,
                       RHINO_CONFIG_CPU_USAGE_TASK_STACK,
                       task_cpu_usage_stats_entry, 1);
}
#endif
