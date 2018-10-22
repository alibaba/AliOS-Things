/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef K_CPUUSAGE_H
#define K_CPUUSAGE_H

#ifdef __cplusplus
extern "C"
{
#endif

#if (RHINO_CONFIG_BACKTRACE > 0)

void     krhino_task_cpu_usage_stats();
uint32_t krhino_task_cpu_usage_get(ktask_t *task);
uint32_t krhino_total_cpu_usage_get(uint32_t cpuid);
void     krhino_total_cpu_usage_show();

#if (RHINO_CONFIG_CPU_USAGE_PERIOD > 0)
kstat_t  krhino_task_cpu_usage_init();
#endif

#endif

#ifdef __cplusplus
}
#endif

#endif /* K_CPUUSAGE_H */
