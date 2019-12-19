/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <assert.h>

#if (RHINO_CONFIG_HW_COUNT > 0)
void soc_hw_timer_init(void)
{
}

hr_timer_t soc_hr_hw_cnt_get(void)
{
    return 0;
    //return *(volatile uint64_t *)0xc0000120;
}

lr_timer_t soc_lr_hw_cnt_get(void)
{
    return 0;
}
#endif /* RHINO_CONFIG_HW_COUNT */

void soc_err_proc(kstat_t err)
{
    (void)err;
    assert(0);
}

krhino_err_proc_t g_err_proc = soc_err_proc;

#include "k_api.h"
extern void *_pvHeapStart;
extern void *_pvHeapLimit;
extern uint8_t _vHeap2Base[];
k_mm_region_t g_mm_region[] = {
{
   (uint8_t *)&_pvHeapStart, (uint32_t)0xE800},
   {_vHeap2Base, 0x1800,},
};

int g_region_num = sizeof(g_mm_region)/sizeof(k_mm_region_t);

void SysTick_Handler(void)
{
    krhino_intrpt_enter();
    krhino_tick_proc();
    krhino_intrpt_exit();
}
