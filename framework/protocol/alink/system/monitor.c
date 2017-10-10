/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "work_queue.h"

extern int os_malloc_get_peak_usage(void);
extern int os_malloc_get_current_usage(void);
extern void os_malloc_dump_memory_usage(void);

void system_monitor(void);

#define MONITOR_WORK_CYCLE      (60 * 1000)
static struct work_struct monitor_work = {
    .func = (work_func_t) &system_monitor,
    .prio = DEFAULT_WORK_PRIO,
    .name = "system monitor",
};

void system_monitor_init(void)
{
    queue_work(&monitor_work);
}

void system_monitor_exit(void)
{
    cancel_work(&monitor_work);
}

void system_monitor(void)
{
    int memory_peak_usage = os_malloc_get_peak_usage();
    int memory_cur_usage = os_malloc_get_current_usage();

    log_info("sdk memory usage: peak %d, current %d",
             memory_peak_usage, memory_cur_usage);

    os_malloc_dump_memory_usage();

    queue_delayed_work(&monitor_work, MONITOR_WORK_CYCLE);
}
