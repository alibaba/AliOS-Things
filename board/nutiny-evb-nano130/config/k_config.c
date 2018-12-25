/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <assert.h>
#include <stdio.h>

void soc_hw_timer_init()
{
}

#if (RHINO_CONFIG_USER_HOOK > 0)
void krhino_idle_pre_hook(void)
{
}

void krhino_idle_hook(void)
{
}

void krhino_init_hook(void)
{
#if (RHINO_CONFIG_HW_COUNT > 0)
    soc_hw_timer_init();
#endif
}
#endif

void krhino_start_hook(void)
{
#if (RHINO_CONFIG_TASK_SCHED_STATS > 0)
    krhino_task_sched_stats_reset();
#endif
}

void krhino_task_create_hook(ktask_t *task)
{
#if XCHAL_CP_NUM > 0
    krhino_task_info_set(task, 0, (void *)((uint32_t)task->task_stack + XT_STK_FRMSZ));
#endif
}

void krhino_task_del_hook(ktask_t *task, res_free_t *arg)
{
     printf("%s del success!\n\r", task->task_name);
            
}

void krhino_task_switch_hook(ktask_t *orgin, ktask_t *dest)
{
    (void)orgin;
    (void)dest;
}

void krhino_tick_hook(void)
{
}

void krhino_task_abort_hook(ktask_t *task)
{
    (void)task;
}

void krhino_mm_alloc_hook(void *mem, size_t size)
{
    (void)mem;
    (void)size;
}

#if (RHINO_CONFIG_MM_TLF > 0)

#if defined (__CC_ARM) /* Keil / armcc */
    #define DEF_IRAM_END                (0x20000000+0x00004000)
    extern unsigned int Image$$RW_IRAM1$$ZI$$Limit;
    int           g_region_num = 1;
    k_mm_region_t g_mm_region[1];
    void aos_heap_set()
    {
        g_mm_region[0].start = (uint8_t*)&Image$$RW_IRAM1$$ZI$$Limit;
        g_mm_region[0].len   = (DEF_IRAM_END - (size_t)&Image$$RW_IRAM1$$ZI$$Limit);
    }
#elif defined (__ICCARM__)/* IAR */
    #define HEAP_BUFFER_SIZE 1024*6
    int           g_region_num = 1;
    uint8_t g_heap_buf[HEAP_BUFFER_SIZE];
    k_mm_region_t g_mm_region[] = {{g_heap_buf, HEAP_BUFFER_SIZE}};
    void aos_heap_set()
    {
        g_mm_region[0].start = g_heap_buf;
        g_mm_region[0].len   = HEAP_BUFFER_SIZE;
    }

#else /* GCC */
    extern uint32_t __aos_heap_start;
    extern uint32_t __aos_heap_size;

    k_mm_region_t g_mm_region[] = {
        { (uint8_t *)&__aos_heap_start, (uint32_t)&__aos_heap_size},
    };
    int g_region_num = 1;   
    void aos_heap_set() { }
#endif //GCC

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

#endif //#if (RHINO_CONFIG_MM_TLF > 0)

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
