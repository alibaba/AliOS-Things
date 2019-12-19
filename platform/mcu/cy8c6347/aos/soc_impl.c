/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <assert.h>
#include <stdio.h>
#include <sys/time.h>

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

#if (RHINO_CONFIG_MM_TLF > 0)

#if !defined (__CC_ARM) /* Keil / armcc */
extern char __HeapLimit[];
extern char __HeapBase[];

#elif defined (__CC_ARM)
extern char __heap_base[];
extern char __heap_limit[];
#endif


#if defined (__CC_ARM) /* Keil / armcc */
k_mm_region_t g_mm_region[] = {{(uint8_t *)__heap_base, 0x19000}};
#else
k_mm_region_t g_mm_region[] = {{(uint8_t *)__HeapBase, 0x19000}};
#endif
int           g_region_num  = sizeof(g_mm_region)/sizeof(k_mm_region_t);

#endif

void soc_err_proc(kstat_t err)
{
    (void)err;

    assert(0);
}

krhino_err_proc_t g_err_proc = soc_err_proc;

