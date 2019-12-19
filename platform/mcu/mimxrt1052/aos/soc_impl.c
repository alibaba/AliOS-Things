/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <assert.h>
#include <stdio.h>
#include "fsl_device_registers.h"

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

void soc_err_proc(kstat_t err)
{
    (void)err;
    soc_print_stack();
    assert(0);
}

krhino_err_proc_t g_err_proc = soc_err_proc;

#if defined (__GNUC__)&&!defined(__CC_ARM)
extern uint32_t heapStart[];
extern uint32_t heapSize[];
k_mm_region_t g_mm_region[] = {
{
   (uint8_t *)heapStart, (uint32_t)heapSize},
};
#elif defined (__CC_ARM)
extern uint32_t Image$$ARM_LIB_HEAP$$ZI$$Base[];
extern uint32_t Image$$ARM_LIB_HEAP$$ZI$$Length[];
k_mm_region_t g_mm_region[] = {
{
   (uint8_t *)Image$$ARM_LIB_HEAP$$ZI$$Base, (size_t)Image$$ARM_LIB_HEAP$$ZI$$Length},
};
#elif defined (__ICCARM__)
extern uint32_t __heap_start__[];
extern uint32_t __size_heap__[];
k_mm_region_t g_mm_region[] = {
{
   (uint8_t *)__heap_start__, (uint32_t)__size_heap__},
};
#else
#error "Tool chain not supported!"
#endif
int g_region_num = sizeof(g_mm_region)/sizeof(k_mm_region_t);

void SysTick_Handler(void)
{
    krhino_intrpt_enter();
    krhino_tick_proc();
    krhino_intrpt_exit();
}
