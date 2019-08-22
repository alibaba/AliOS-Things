/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdlib.h>

#include "k_api.h"
#include "debug_api.h"

#if (RHINO_CONFIG_SYS_STATS > 0)

#define CPU_USAGE_DEFAULT_PRI 7
#define CPU_USAGE_STACK       2048

ktask_t *cpuusage_task;

void cpuusage_statistics(void* arg)
{
    ktask_t *cur_task;
    kstat_t  status;
    uint32_t period = (uint32_t)arg;

    printf("start to statistics CPU utilization, period %d\r\n", period);
    debug_task_cpu_usage_stats();
    status = krhino_task_sleep(period * RHINO_CONFIG_TICKS_PER_SECOND);
    if (status == RHINO_TASK_CANCELED) {
        krhino_task_cancel_clr();
    }

    while(1) {
        if (krhino_task_cancel_chk() == RHINO_TRUE) {
            cur_task= krhino_cur_task_get();
            krhino_task_dyn_del(cur_task);
        }
        debug_task_cpu_usage_stats();
        status = krhino_task_sleep(period * RHINO_CONFIG_TICKS_PER_SECOND);
        if (status == RHINO_TASK_CANCELED) {
            krhino_task_cancel_clr();
        } else {
            debug_total_cpu_usage_show();
        }
    }
}

void cpuusage_cmd(char *buf, int32_t len, int32_t argc, char **argv)
{
    uint32_t period = 1;
    ktask_t *task_tmp = NULL;

    if (argc == 3 && 0 == strcmp(argv[1], "-d")) {
        period = atoi(argv[2]);
        if ((period > 20) || (period == 0)) {
            period = 1;
        }
    }

    task_tmp = debug_task_find("cpuusage");
    if (task_tmp != NULL) {
        return;
    }

    krhino_task_dyn_create(&cpuusage_task, "cpuusage", (void*)period, (uint8_t)CPU_USAGE_DEFAULT_PRI, 0, CPU_USAGE_STACK, cpuusage_statistics, 1);
}

#endif

