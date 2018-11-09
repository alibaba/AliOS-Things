/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include "aos/aos_debug.h"
#include "debug_api.h"

void aos_debug_task_cpu_usage_stats()
{
    debug_task_cpu_usage_stats();
}

uint32_t aos_debug_task_cpu_usage_get(ktask_t *task)
{
    return debug_task_cpu_usage_get(task);
}

uint32_t aos_debug_total_cpu_usage_get(uint32_t cpuid)
{
    return debug_total_cpu_usage_get(cpuid);
}

ktask_t *aos_debug_task_find(char *name)
{
    return debug_task_find(name);
}
