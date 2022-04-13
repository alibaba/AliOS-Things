/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "k_api.h"

void krhino_tick_proc(void)
{
#if (RHINO_CONFIG_USER_HOOK > 0)
    krhino_tick_hook();
#endif

    tick_list_update(1);

#if (RHINO_CONFIG_SCHED_RR > 0)
    time_slice_update();
#endif
}

void krhino_tick_proc_cnt(tick_i_t cnt_add)
{
#if (RHINO_CONFIG_USER_HOOK > 0)
    krhino_tick_hook();
#endif

    tick_list_update(cnt_add);

#if (RHINO_CONFIG_SCHED_RR > 0)
    time_slice_update();
#endif
}

tick_t krhino_sys_tick_get(void)
{
    tick_t tick_tmp;
    cpu_cpsr_t cpsr;

    cpsr = cpu_intrpt_save();
    tick_tmp = g_tick_count;
    cpu_intrpt_restore(cpsr);

    return tick_tmp;
}

sys_time_t krhino_sys_time_get(void)
{
    return (sys_time_t)(krhino_sys_tick_get() * 1000 / RHINO_CONFIG_TICKS_PER_SECOND);
}

tick_t krhino_ms_to_ticks(sys_time_t ms)
{
    tick_t ticks;
    ticks = (ms * RHINO_CONFIG_TICKS_PER_SECOND + 999) / 1000;
    return ticks;
}

sys_time_t krhino_ticks_to_ms(tick_t ticks)
{
    sys_time_t time;
    time = (ticks * 1000 + RHINO_CONFIG_TICKS_PER_SECOND - 1) / RHINO_CONFIG_TICKS_PER_SECOND;
    return time;
}

