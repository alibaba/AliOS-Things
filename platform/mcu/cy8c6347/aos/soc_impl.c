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

#if (RHINO_CONFIG_INTRPT_STACK_OVF_CHECK > 0)
void soc_intrpt_stack_ovf_check(void)
{
}
#endif

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

#if (RHINO_CONFIG_TASK_STACK_CUR_CHECK > 0)
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

    #if (RHINO_CONFIG_TASK_STACK_CUR_CHECK > 0)
    soc_print_stack();
    #endif
    assert(0);
}

krhino_err_proc_t g_err_proc = soc_err_proc;

