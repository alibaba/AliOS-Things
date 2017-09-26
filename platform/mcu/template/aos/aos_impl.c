/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>

#if (RHINO_CONFIG_HW_COUNT > 0)
void soc_hw_timer_init(void)
{
}

hr_timer_t soc_hr_hw_cnt_get(void)
{
    return 0;
}

lr_timer_t soc_lr_hw_cnt_get(void)
{
    return 0;
}
#endif /* RHINO_CONFIG_HW_COUNT */

#if (RHINO_CONFIG_INTRPT_GUARD > 0)
void soc_intrpt_guard(void)
{
}
#endif

#if (RHINO_CONFIG_INTRPT_STACK_REMAIN_GET > 0)
size_t soc_intrpt_stack_remain_get(void)
{
    return 0;
}
#endif

#if (RHINO_CONFIG_INTRPT_STACK_OVF_CHECK > 0)
void soc_intrpt_stack_ovf_check(void)
{
}
#endif

#if (RHINO_CONFIG_DYNTICKLESS > 0)
void soc_tick_interrupt_set(tick_t next_ticks,tick_t elapsed_ticks)
{
}

tick_t soc_elapsed_ticks_get(void)
{
    return 0;
}
#endif

k_mm_region_t      g_mm_region;
k_mm_region_head_t g_mm_region_head;

void soc_sys_mem_init(void)
{
    g_mm_region.start = **********
    g_mm_region.len   = **********

    krhino_mm_region_init(&g_mm_region_head, &g_mm_region, 1);
}

void *soc_mm_alloc(size_t size)
{
    kstat_t ret;
    void   *mem;

    ret = krhino_mm_bf_alloc(&g_mm_region_head, &mem, size);
    if (ret != RHINO_SUCCESS) {
        return NULL;
    }

    return mem;
}

void soc_mm_free(void *mem)
{
    krhino_mm_bf_free(&g_mm_region_head, mem);
}

void soc_err_proc(kstat_t err)
{
}

krhino_err_proc_t g_err_proc = soc_err_proc;
