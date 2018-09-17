/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include "aos_adapter_timeout.h"


void soc_hw_timer_init()
{
}

#if (RHINO_CONFIG_USER_HOOK > 0)
#if (RHINO_CONFIG_CPU_NUM > 1)
volatile uint64_t cpu_flag = 0;
#endif
void krhino_idle_pre_hook(void)
{
    #if (RHINO_CONFIG_CPU_NUM > 1)
    CPSR_ALLOC();
    uint8_t cpu;

    RHINO_CPU_INTRPT_DISABLE();
    cpu = cpu_cur_get();
    cpu_flag |= (1UL << cpu);
    
    RHINO_CPU_INTRPT_ENABLE();
    #endif
}
void krhino_idle_hook(void)
{
    /*alios idle should sleep sxr to push switch*/
#ifndef RHINO_ONLY
/*
    extern void sxr_Sleep (u32 Period);
    sxr_Sleep(10); */
    #ifdef OSAL_SXR_MUTIOS
    extern  sxr_rhino_sema g_rhino_sxs_sem;
    (void)sxr_rhino_SemaTryTake(&g_rhino_sxs_sem, 20);

    /*should run rhino task switch*/
    krhino_task_yield();
    #endif
#endif

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
#if (RHINO_CONFIG_TASK_SCHED_STATS > 0)
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

