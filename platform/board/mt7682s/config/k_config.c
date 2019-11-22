/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <assert.h>
#include <stdio.h>
#include "exception_handler.h"
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
#if 1
size_t soc_get_cur_sp()
{
    size_t sp = 0;
#if defined (__GNUC__)&&!defined(__CC_ARM)
    asm volatile(
        "mov %0,sp\n"
        :"=r"(sp));
#endif
    return sp;
}


static void soc_print_stack()
{
    void    *cur, *end;
    int      i=0;
    int     *p;

    end   = krhino_cur_task_get()->task_stack_base + krhino_cur_task_get()->stack_size;
    cur = (void *)soc_get_cur_sp();
    p = (int*)cur;
    while(p < (int*)end) {
        if(i%4==0) {
            printf("\r\n%08x:",(uint32_t)p);
        }
        printf("%08x ", *p);
        i++;
        p++;
    }
    printf("\r\n");
    return;
}
#endif
void soc_err_proc(kstat_t err)
{
    (void)err;
    soc_print_stack();
    //assert(0);
}

krhino_err_proc_t g_err_proc = soc_err_proc;

#include "memory_attribute.h"

#if defined (__GNUC__)&&!defined(__CC_ARM)

extern void* _heap_size;
extern void* heap_start;

k_mm_region_t g_mm_region[] = {{(uint8_t*)&heap_start,(uint8_t*)&_heap_size}};

#elif defined (__CC_ARM)

#define HEAP_BUFFER_SIZE 1024*190
uint8_t g_heap_buf[HEAP_BUFFER_SIZE];
k_mm_region_t g_mm_region[] = {{g_heap_buf, HEAP_BUFFER_SIZE}};

#else
#error "Tool chain not supported!"
#endif
int g_region_num = sizeof(g_mm_region)/sizeof(k_mm_region_t);

#if (RHINO_CONFIG_MM_LEAKCHECK > 0 )

extern int _cached_sysram_bss_start, _cached_sysram_bss_end;

void aos_mm_leak_region_init(void)
{
#if (RHINO_CONFIG_MM_DEBUG > 0)
    krhino_mm_leak_region_init(&_cached_sysram_bss_start, &_cached_sysram_bss_end);
#endif
}

#endif

#if 0
void SysTick_Handler(void)
{
    krhino_intrpt_enter();
    krhino_tick_proc();
    krhino_intrpt_exit();
}
#endif

void print_heap_info()
{
    printf("heap start:%x,heap len:%x\r\n",(uint32_t)&heap_start,(uint32_t)&_heap_size);
}
