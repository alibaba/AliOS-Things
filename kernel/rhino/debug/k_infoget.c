/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "k_dbg_api.h"

#if (RHINO_CONFIG_SYSTEM_STATS > 0)
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
#endif

int krhino_task_is_running(ktask_t *task)
{
    if (  g_active_task[cpu_cur_get()] == task
       && g_intrpt_nested_level[cpu_cur_get()] == 0 ) {
        return 1;
    }
    else {
        return 0;
    }
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

