/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>

#if (RHINO_CONFIG_DYNTICKLESS > 0)
void krhino_tickless_proc(void)
{
    CPSR_ALLOC();

#if (RHINO_CONFIG_INTRPT_GUARD > 0)
    soc_intrpt_guard();
#endif

#if (RHINO_CONFIG_USER_HOOK > 0)
    krhino_tick_hook();
#endif

    tick_list_update();

}
#else
void krhino_tick_proc(void)
{
#if (RHINO_CONFIG_INTRPT_GUARD > 0)
    soc_intrpt_guard();
#endif

#if (RHINO_CONFIG_USER_HOOK > 0)
    krhino_tick_hook();
#endif

    tick_list_update();

#if (RHINO_CONFIG_SCHED_RR > 0)
    time_slice_update(g_active_task[cpu_cur_get()]->prio);
#endif
}
#endif

sys_time_t krhino_sys_tick_get(void)
{
    sys_time_t tick_tmp;

#if (RHINO_CONFIG_DYNTICKLESS > 0)
    tick_t elapsed_ticks;

    CPSR_ALLOC();

    RHINO_CPU_INTRPT_DISABLE();

    elapsed_ticks = soc_elapsed_ticks_get();
    tick_tmp = g_sys_time_tick + (sys_time_t)(elapsed_ticks);

    RHINO_CPU_INTRPT_ENABLE();
#else
    tick_tmp = g_sys_time_tick;
#endif

    return tick_tmp;
}

sys_time_t krhino_sys_time_get(void)
{
    return (sys_time_t)(krhino_sys_tick_get() * 1000 /
                        RHINO_CONFIG_TICKS_PER_SECOND);
}

tick_t     krhino_ms_to_ticks(sys_time_t ms)
{
    uint16_t  padding;
    uint16_t  surplus;
    tick_t    ticks;

    surplus = ms % 1000;
    ticks = (ms / 1000) * RHINO_CONFIG_TICKS_PER_SECOND;
    padding = 1000 / RHINO_CONFIG_TICKS_PER_SECOND;
    padding = (padding > 0) ? (padding - 1) : 0;

    ticks += ((surplus + padding) * RHINO_CONFIG_TICKS_PER_SECOND) / 1000;

    return ticks;
}

sys_time_t krhino_ticks_to_ms(tick_t ticks)
{
    uint32_t   padding;
    uint32_t   surplus;
    sys_time_t time;

    surplus = ticks % RHINO_CONFIG_TICKS_PER_SECOND;
    time = (ticks / RHINO_CONFIG_TICKS_PER_SECOND) * 1000;
    padding = RHINO_CONFIG_TICKS_PER_SECOND / 1000;
    padding = (padding > 0) ? (padding - 1) : 0;

    time += ((surplus + padding) * 1000) / RHINO_CONFIG_TICKS_PER_SECOND;

    return time;
}

