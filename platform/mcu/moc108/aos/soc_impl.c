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
    return *(volatile uint64_t *)0xc0000120;
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

extern void         *heap_start;
extern void         *heap_end;
extern void         *heap_len;

k_mm_region_t g_mm_region[] = {(uint8_t*)&heap_start,(size_t)&heap_len};
int           g_region_num  = sizeof(g_mm_region)/sizeof(k_mm_region_t);


#if (RHINO_CONFIG_MM_LEAKCHECK > 0 )

extern int _bss_start, _bss_end, _data_ram_begin, _data_ram_end;

void aos_mm_leak_region_init(void)
{
    krhino_mm_leak_region_init(&_bss_start, &_bss_end);
    krhino_mm_leak_region_init(&_data_ram_begin, &_data_ram_end);
}

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
#endif

static void soc_print_stack()
{

    uint32_t offset = 0;
    kstat_t  rst    = RHINO_SUCCESS;
    void    *cur, *end;
    int      i=0;
    int     *p;

    end   = krhino_cur_task_get()->task_stack_base + krhino_cur_task_get()->stack_size;
    cur = soc_get_cur_sp();
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
    printf("panic %d!\r\n",err);
    soc_print_stack();
    assert(0);
}

krhino_err_proc_t g_err_proc = soc_err_proc;

