/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>

#if (RHINO_CONFIG_CPU_USAGE_STATS > 0)
void idle_count_set(idle_count_t value)
{
    CPSR_ALLOC();

    RHINO_CPU_INTRPT_DISABLE();

    g_idle_count[cpu_cur_get()] = value;

    RHINO_CPU_INTRPT_ENABLE();
}

idle_count_t idle_count_get(void)
{
    CPSR_ALLOC();

    idle_count_t idle_count;

    RHINO_CPU_INTRPT_DISABLE();

    idle_count = g_idle_count[cpu_cur_get()];

    RHINO_CPU_INTRPT_ENABLE();

    return idle_count;
}
#endif

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

