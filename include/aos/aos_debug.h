/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef AOS_DEBUG_H
#define AOS_DEBUG_H

#ifdef __cplusplus
extern "C" {
#endif

#include <k_api.h>

    void aos_debug_task_cpu_usage_stats();
    uint32_t aos_debug_total_cpu_usage_get(uint32_t cpuid);
    uint32_t aos_debug_task_cpu_usage_get(ktask_t *task);
    ktask_t *aos_debug_task_find(char *name);

#ifdef __cplusplus
}
#endif

#endif /* AOS_DEBUG_H */

