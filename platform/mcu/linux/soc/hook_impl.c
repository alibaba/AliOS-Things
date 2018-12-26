/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>

void soc_hw_timer_init()
{
}

#if (RHINO_CONFIG_USER_HOOK > 0)
volatile uint64_t cpu_flag;
void krhino_idle_pre_hook(void)
{
    CPSR_ALLOC();
    uint8_t cpu;

    RHINO_CPU_INTRPT_DISABLE();
    cpu = cpu_cur_get();
    cpu_flag |= (1 << cpu);
    RHINO_CPU_INTRPT_ENABLE();
}

void krhino_idle_hook(void)
{
    cpu_idle_hook();
}

void krhino_init_hook(void)
{
#if (RHINO_CONFIG_HW_COUNT > 0)
    soc_hw_timer_init();
#endif
    cpu_init_hook();
}
#endif

void krhino_start_hook(void)
{
#if (RHINO_CONFIG_TASK_SCHED_STATS > 0)
    krhino_task_sched_stats_reset();
#endif
    cpu_start_hook();
}

void krhino_task_create_hook(ktask_t *task)
{
    cpu_task_create_hook(task);
}

void krhino_task_del_hook(ktask_t *task, res_free_t *arg)
{
    cpu_task_del_hook(task, arg);
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
