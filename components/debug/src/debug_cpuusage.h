/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef DEBUG_CPUUSAGE_H
#define DEBUG_CPUUSAGE_H

#ifdef __cplusplus
extern "C" {
#endif

#define CPU_USAGE_MAX_TASKNAME_LEN 32

struct cpuusage_data {
    char taskname[CPU_USAGE_MAX_TASKNAME_LEN];
    float *cpuusage;
};

void     debug_task_cpu_usage_stats(void);
uint32_t debug_task_cpu_usage_get(ktask_t *task);
uint32_t debug_total_cpu_usage_get(uint32_t cpuid);
void     debug_total_cpu_usage_show(struct cpuusage_data *cpuusage_record, int32_t record_len, int32_t index);
kstat_t  debug_task_cpu_usage_init(void);
void     debug_cpuusage_cmds_register(void);

#ifdef __cplusplus
}
#endif

#endif /* DEBUG_CPUUSAGE_H */
