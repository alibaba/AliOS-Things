/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include "k_arch.h"

void soc_hw_timer_init()
{
}

#if (RHINO_CONFIG_USER_HOOK > 0)
#if (RHINO_CONFIG_CPU_NUM > 1)
extern volatile uint64_t g_cpu_flag;
#endif
void krhino_idle_pre_hook(void)
{
    #if (RHINO_CONFIG_CPU_NUM > 1)
    CPSR_ALLOC();
    uint8_t cpu;

    RHINO_CPU_INTRPT_DISABLE();
    cpu = cpu_cur_get();
    g_cpu_flag |= (1UL << cpu);

    RHINO_CPU_INTRPT_ENABLE();
    #endif
}
void krhino_idle_hook(void)
{

}

void krhino_init_hook(void)
{
#if (RHINO_CONFIG_HW_COUNT > 0)
    soc_hw_timer_init();
#endif
}
#endif

void krhino_start_hook(void)
{
#if (RHINO_CONFIG_SYS_STATS > 0)
    krhino_task_sched_stats_reset();
#endif
}

void krhino_task_create_hook(ktask_t *task)
{

}

void krhino_task_del_hook(ktask_t *task, res_free_t *arg)
{
}

void krhino_task_switch_hook(ktask_t *orgin, ktask_t *dest)
{
    (void)orgin;
    (void)dest;
}

void krhino_tick_hook(void)
{
}

void krhino_task_abort_hook(ktask_t *task)
{
    (void)task;
}

void krhino_mm_alloc_hook(void *mem, size_t size)
{
    (void)mem;
    (void)size;
}

