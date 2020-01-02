/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "aos/debug.h"
#include "debug_api.h"

void aos_debug_backtrace_now(void)
{
    debug_backtrace_now();
}

void aos_debug_backtrace_task(char *taskname)
{
    debug_backtrace_task(taskname);
}

void aos_debug_mm_overview(int (*print_func)(const char *fmt, ...))
{
    debug_mm_overview(print_func);
}

void aos_debug_task_overview(int (*print_func)(const char *fmt, ...))
{
    debug_task_overview(print_func);
}

void aos_debug_buf_queue_overview(int (*print_func)(const char *fmt, ...))
{
    debug_buf_queue_overview(print_func);
}

void aos_debug_queue_overview(int (*print_func)(const char *fmt, ...))
{
    debug_queue_overview(print_func);
}

void aos_debug_sem_overview(int (*print_func)(const char *fmt, ...))
{
    debug_sem_overview(print_func);
}

void aos_debug_overview(void)
{
    debug_overview();
}

#if (RHINO_CONFIG_SYS_STATS > 0)
void aos_debug_task_cpu_usage_stats(void)
{
    aos_debug_task_cpu_usage_stats();
}

int32_t aos_debug_task_cpu_usage_get(char *taskname)
{
    ktask_t *task;
    task = krhino_task_find(taskname);
    if (task == NULL) {
        return -1;
    }

    return debug_task_cpu_usage_get(task);
}

uint32_t aos_debug_total_cpu_usage_get(uint32_t cpuid)
{
    return debug_total_cpu_usage_get(cpuid);
}

void aos_debug_total_cpu_usage_show(void)
{
    debug_total_cpu_usage_show();
}
#endif

unsigned int aos_debug_reboot_reason_get(void)
{
    return debug_reboot_reason_get();
}

