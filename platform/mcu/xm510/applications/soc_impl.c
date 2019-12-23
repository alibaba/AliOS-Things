/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <k_api.h>
#include <assert.h>
#include "aos/hal/uart.h"

#if (RHINO_CONFIG_HW_COUNT > 0)
extern unsigned long long system_clock(void);
void soc_hw_timer_init(void)
{
}

hr_timer_t soc_hr_hw_cnt_get(void)
{
    return system_clock();
}

lr_timer_t soc_lr_hw_cnt_get(void)
{
    /* Translate ns to ms */
    return (system_clock() / 1000000);
}
#endif /* RHINO_CONFIG_HW_COUNT */

extern void         *heap_start;
extern void         *heap_len;

k_mm_region_t g_mm_region[] = {{(uint8_t*)&heap_start,(size_t)&heap_len}};

int           g_region_num  = sizeof(g_mm_region)/sizeof(k_mm_region_t);


void soc_err_proc(kstat_t err)
{
    (void)err;
    printf("panic %d!\r\n",err);
    assert(0);
}

krhino_err_proc_t g_err_proc = soc_err_proc;

