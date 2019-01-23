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

#if (RHINO_CONFIG_KOBJ_DYN_ALLOC > 0)
extern void *heap_start;
extern void *heap_len;
k_mm_region_t g_mm_region[] = {{(uint8_t *)&heap_start, (size_t)&heap_len}};
int g_region_num = sizeof(g_mm_region) / sizeof(k_mm_region_t);


void soc_sys_mem_init(void)
{
    extern void *heap_start;
    extern void *heap_len;

    g_mm_region[0].start = &heap_start;
    g_mm_region[0].len   = &heap_len;
    g_region_num = 1;
}


#endif

void soc_err_proc(kstat_t err)
{
}

krhino_err_proc_t g_err_proc = soc_err_proc;
