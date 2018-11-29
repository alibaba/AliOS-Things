/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>

void krhino_tick_proc(void)
{
#if (RHINO_CONFIG_INTRPT_GUARD > 0)
    soc_intrpt_guard();
#endif

#if (RHINO_CONFIG_USER_HOOK > 0)
    krhino_tick_hook();
#endif

    tick_list_update(1);

#if (RHINO_CONFIG_SCHED_RR > 0)
    time_slice_update();
#endif
}

sys_time_t krhino_sys_tick_get(void)
{
    CPSR_ALLOC();

    sys_time_t tick_tmp;

    RHINO_CPU_INTRPT_DISABLE();
    tick_tmp = g_tick_count;
    RHINO_CPU_INTRPT_ENABLE();

    return tick_tmp;
}

sys_time_t krhino_sys_time_get(void)
{
    return (sys_time_t)(krhino_sys_tick_get() * 1000 / RHINO_CONFIG_TICKS_PER_SECOND);
}

tick_t krhino_ms_to_ticks(sys_time_t ms)
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

