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
}

lr_timer_t soc_lr_hw_cnt_get(void)
{
    return 0;
}
#endif /* RHINO_CONFIG_HW_COUNT */

#if (RHINO_CONFIG_MM_TLF > 0)

#if defined (__CC_ARM) /* Keil / armcc */

#define HEAP_BUFFER_SIZE (MIN_FREE_MEMORY_SIZE+RHINO_CONFIG_MM_TLF_BLK_SIZE)
int           g_region_num = 1;

uint8_t g_heap_buf[HEAP_BUFFER_SIZE];
//k_mm_region_t g_mm_region[1] = {{g_heap_buf, HEAP_BUFFER_SIZE}};
k_mm_region_t g_mm_region[1];
void aos_heap_set()
{
    g_mm_region[0].start = g_heap_buf;
    g_mm_region[0].len   = HEAP_BUFFER_SIZE;
}



#elif defined (__ICCARM__)/* IAR */

#define HEAP_BUFFER_SIZE 1024*20
int           g_region_num = 1;

uint8_t g_heap_buf[HEAP_BUFFER_SIZE];
k_mm_region_t g_mm_region[] = {{g_heap_buf, HEAP_BUFFER_SIZE}};
void aos_heap_set()
{
    g_mm_region[0].start = g_heap_buf;
    g_mm_region[0].len   = HEAP_BUFFER_SIZE;
}

#else /* GCC */
extern void         *_estack;
extern void         *__bss_end__;
/* __bss_end__ and _estack is set by linkscript(*.ld)
   heap and stack begins from __bss_end__ to _estack */
k_mm_region_t g_mm_region[1];
int           g_region_num = 1;
void aos_heap_set()
{
    g_mm_region[0].start = (uint8_t*)&__bss_end__;
    g_mm_region[0].len   =
        ((uint8_t*)&_estack - (uint8_t*)&__bss_end__) - RHINO_CONFIG_SYSTEM_STACK_SIZE;
}
#endif
#endif

void soc_err_proc(kstat_t err)
{
    (void)err;

    assert(0);
}

krhino_err_proc_t g_err_proc = soc_err_proc;


