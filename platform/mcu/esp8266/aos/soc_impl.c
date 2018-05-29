/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

size_t soc_get_cur_sp()
{
    volatile size_t dummy = (size_t)&dummy;
    return dummy;
}

#if (RHINO_CONFIG_HW_COUNT > 0)
hr_timer_t soc_hr_hw_cnt_get(void)
{
    return 0;
}
#endif

/* iram use as memory heap: */
#define IRAM_AS_HEAP_ADDR   ((void*)0x40108000)
#define IRAM_AS_HEAP_LEN    0x4000

extern uint8_t _heap_start, _heap_size;
const k_mm_region_t g_mm_region[] = {
    {&_heap_start, (uint32_t)&_heap_size},
    //{IRAM_AS_HEAP_ADDR, IRAM_AS_HEAP_LEN}
};
int g_region_num  = sizeof(g_mm_region)/sizeof(k_mm_region_t);

void soc_err_proc(kstat_t err)
{
    // printf("kernel panic,err %d!\n",err);
    // assert(0);
}

krhino_err_proc_t g_err_proc = soc_err_proc;

