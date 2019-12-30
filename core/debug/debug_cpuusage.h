/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef DEBUG_CPUUSAGE_H
#define DEBUG_CPUUSAGE_H

#ifdef __cplusplus
extern "C" {
#endif

void     debug_task_cpu_usage_stats(void);
uint32_t debug_task_cpu_usage_get(ktask_t *task);
uint32_t debug_total_cpu_usage_get(uint32_t cpuid);
void     debug_total_cpu_usage_show(void);
kstat_t  debug_task_cpu_usage_init(void);

#ifdef __cplusplus
}
#endif

#endif /* DEBUG_CPUUSAGE_H */
