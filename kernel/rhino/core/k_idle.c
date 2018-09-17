/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>

void idle_task(void *arg)
{
    CPSR_ALLOC();

    /* avoid warning */
    (void)arg;

#if (RHINO_CONFIG_USER_HOOK > 0)
    krhino_idle_pre_hook();
#endif

    while (RHINO_TRUE) {
        RHINO_CPU_INTRPT_DISABLE();

        g_idle_count[cpu_cur_get()]++;

        RHINO_CPU_INTRPT_ENABLE();

#if (RHINO_CONFIG_USER_HOOK > 0)
        krhino_idle_hook();
#endif

#if (RHINO_CONFIG_CPU_PWR_MGMT > 0)
        cpu_pwr_down();
#endif
    }
}

