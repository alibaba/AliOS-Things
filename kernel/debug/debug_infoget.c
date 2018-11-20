/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include "debug_api.h"

ktask_t *debug_task_find(char *name)
{
    klist_t *listnode;
    ktask_t *task;

#if (RHINO_CONFIG_SYSTEM_STATS > 0)
    for (listnode = g_kobj_list.task_head.next;
         listnode != &g_kobj_list.task_head; listnode = listnode->next) {
        task = krhino_list_entry(listnode, ktask_t, task_stats_item);
        if (0 == strcmp(name, task->task_name)) {
            return task;
        }
    }
#endif

    return NULL;
}

int debug_task_is_ready(ktask_t *task)
{
    return (task->task_state == K_RDY);
}

void *debug_task_stack_bottom(ktask_t *task)
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

