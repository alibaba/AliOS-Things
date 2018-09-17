/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <assert.h>
#include <stdio.h>

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

#if (RHINO_CONFIG_MM_TLF > 0)


#define HEAP_BUFFER_SIZE 1024*100
uint8_t g_heap_buf[HEAP_BUFFER_SIZE];
k_mm_region_t g_mm_region[] = {{g_heap_buf, HEAP_BUFFER_SIZE}};
int           g_region_num  = sizeof(g_mm_region)/sizeof(k_mm_region_t);
#endif

#if (RHINO_CONFIG_MM_LEAKCHECK > 0 )

extern int __bss_start__, __bss_end__, _sdata, _edata;

void aos_mm_leak_region_init(void)
{
#if (RHINO_CONFIG_MM_DEBUG > 0)
    krhino_mm_leak_region_init(&__bss_start__, &__bss_end__);
    krhino_mm_leak_region_init(&_sdata, &_edata);
#endif
}

#endif


#if (RHINO_CONFIG_TASK_STACK_CUR_CHECK > 0)
size_t soc_get_cur_sp()
{
    size_t sp = 0;

    return sp;
}
static void soc_print_stack()
{
    printf("\r\n");
    return;
}
#endif


void soc_err_proc(kstat_t err)
{
    (void)err;
    
    assert(0);
}

krhino_err_proc_t g_err_proc = soc_err_proc;
