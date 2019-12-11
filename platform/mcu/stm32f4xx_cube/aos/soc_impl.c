/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <assert.h>
#include <stdio.h>
#include <sys/time.h>

extern void hal_reboot(void);

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

#if (RHINO_CONFIG_INTRPT_STACK_OVF_CHECK > 0)
void soc_intrpt_stack_ovf_check(void)
{
}
#endif

#if (RHINO_CONFIG_MM_TLF > 0)

#if defined (__CC_ARM) /* Keil / armcc */
extern unsigned int Image$$RW_IRAM1$$ZI$$Limit;
//comes from component board, may different to each other as hardware resource
extern size_t g_iram1_start;
extern size_t g_iram1_total_size;
k_mm_region_t g_mm_region[1];
int           g_region_num = 1;
void aos_heap_set()
{
    g_mm_region[0].start = (uint8_t*)&Image$$RW_IRAM1$$ZI$$Limit;
    g_mm_region[0].len   =
        (g_iram1_start + g_iram1_total_size - (size_t)&Image$$RW_IRAM1$$ZI$$Limit);
}

#elif defined (__ICCARM__)/* IAR */
#define HEAP_BUFFER_SIZE 1024*136
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
    hal_reboot();
}

krhino_err_proc_t g_err_proc = soc_err_proc;

