/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#include "debug_api.h"

typedef int32_t (*BT_PRINT)(const char *fmt, ...);

void aos_debug_backtrace_now(int32_t (*print_func)(const char *fmt, ...))
{
    BT_PRINT bt_printf = print_func ?  print_func : printf;

    debug_backtrace_now(bt_printf);
}

void aos_debug_backtrace_task(char *taskname, int32_t (*print_func)(const char *fmt, ...))
{
    BT_PRINT bt_printf = print_func ?  print_func : printf;

    debug_backtrace_task(taskname, bt_printf);
}

void aos_debug_mm_overview(int32_t (*print_func)(const char *fmt, ...))
{
    BT_PRINT bt_printf = print_func ?  print_func : printf;

    debug_mm_overview(bt_printf);
}

void aos_debug_task_overview(int32_t (*print_func)(const char *fmt, ...))
{
    BT_PRINT bt_printf = print_func ?  print_func : printf;

    debug_task_overview(bt_printf);
}

void aos_debug_buf_queue_overview(int32_t (*print_func)(const char *fmt, ...))
{
    BT_PRINT bt_printf = print_func ?  print_func : printf;

    debug_buf_queue_overview(bt_printf);
}

void aos_debug_queue_overview(int32_t (*print_func)(const char *fmt, ...))
{
    BT_PRINT bt_printf = print_func ?  print_func : printf;

    debug_queue_overview(bt_printf);
}

void aos_debug_sem_overview(int32_t (*print_func)(const char *fmt, ...))
{
    BT_PRINT bt_printf = print_func ?  print_func : printf;

    debug_sem_overview(bt_printf);
}

void aos_debug_mutex_overview(int32_t (*print_func)(const char *fmt, ...))
{
    BT_PRINT bt_printf = print_func ?  print_func : printf;

    debug_mutex_overview(bt_printf);
}

void aos_debug_overview(int32_t (*print_func)(const char *fmt, ...))
{
    BT_PRINT bt_printf = print_func ?  print_func : printf;

    debug_overview(bt_printf);
}

#if (RHINO_CONFIG_SYS_STATS > 0)
void aos_debug_task_cpu_usage_stats(void)
{
    debug_task_cpu_usage_stats();
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
    debug_total_cpu_usage_show(NULL, 0, 0);
}
#endif

void aos_debug_fatal_error(kstat_t err, char *file, int32_t line)
{
    debug_fatal_error(err, file, line);
}

void aos_debug_init(void)
{
    debug_init();
}