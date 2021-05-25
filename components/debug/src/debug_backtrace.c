/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include "k_api.h"

/* follow functions should defined by arch\...\backtrace.c */
extern int backtrace_now(int (*print_func)(const char *fmt, ...));
extern int backtrace_task(void *task, int (*print_func)(const char *fmt, ...));

void debug_backtrace_now(int32_t (*print_func)(const char *fmt, ...))
{
    backtrace_now(print_func);
}

void debug_backtrace_task(char *taskname, int32_t (*print_func)(const char *fmt, ...))
{
    klist_t *listnode;
    ktask_t *task;

    for (listnode = g_kobj_list.task_head.next;
         listnode != &g_kobj_list.task_head; listnode = listnode->next) {
        task = krhino_list_entry(listnode, ktask_t, task_stats_item);
        if (taskname) {
            if (0 == strcmp(taskname, "all")) {
                print_func("task name \"%s\" \r\n",
                       ((ktask_t *)task)->task_name ? ((ktask_t *)task)->task_name : "anonym");
                backtrace_task(task, print_func);
            } else {
                if (0 == strcmp(taskname, task->task_name)) {
                    backtrace_task(task, print_func);
                }
            }
        }
    }
}

void backtrace_handle(char *PC, int *SP, char *LR, int (*print_func)(const char *fmt, ...));
void debug_panic_backtrace(char *PC, int *SP, char *LR,
                           int (*print_func)(const char *fmt, ...))
{
    backtrace_handle(PC, SP, LR, print_func);
}
