/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include "debug_api.h"

ktask_t *debug_task_find(char *name)
{
#if (RHINO_CONFIG_KOBJ_LIST > 0)
    klist_t *listnode;
    ktask_t *task;

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

ktask_t *debug_task_find_running(char **name)
{
    ktask_t *task_running;

    task_running = g_active_task[cpu_cur_get()];
    if (task_running != NULL && name != NULL) {
        *name = (char *)task_running->task_name;
    }
    return task_running;
}

ktask_t *debug_task_find_by_id(uint32_t task_id)
{
#if (RHINO_CONFIG_KOBJ_LIST > 0)
    klist_t *listnode;
    ktask_t *task;

    for (listnode = g_kobj_list.task_head.next;
         listnode != &g_kobj_list.task_head; listnode = listnode->next) {
        task = krhino_list_entry(listnode, ktask_t, task_stats_item);
        if (task->task_id == task_id) {
            return task;
        }
    }
#endif

    return NULL;
}

/* return:
   0 not ready
   1 ready but not running
   2 running but interrupted
   3 running*/
int debug_task_is_running(ktask_t *task)
{
    int i;
    for (i = 0; i < RHINO_CONFIG_CPU_NUM; i++) {
        if (g_active_task[i] == task
            && g_intrpt_nested_level[i] == 0) {
            return 3;
        } else if (g_active_task[i] == task) {
            return 2;
        }
    }

    if (task->task_state == K_RDY) {
        return 1;
    } else {
        return 0;
    }
}

uint32_t debug_task_id_now()
{
    if (g_active_task[cpu_cur_get()] == NULL || g_intrpt_nested_level[cpu_cur_get()] != 0) {
        return 0;
    }

    return g_active_task[cpu_cur_get()]->task_id;
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

