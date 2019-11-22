/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <assert.h>
#include <stdio.h>

/**
 * soc api for k_soc.h
 * -- start
 */
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

#if (RHINO_CONFIG_INTRPT_STACK_OVF_CHECK > 0)
void soc_intrpt_stack_ovf_check(void)
{
}
#endif

/**
* soc api for k_soc.h
* -- end
*/

/**
* krhino hook for k_hook.h
* -- start
*/
#if (RHINO_CONFIG_USER_HOOK > 0)
void krhino_idle_hook(void)
{
}

void krhino_init_hook(void)
{
#if (RHINO_CONFIG_HW_COUNT > 0)
    soc_hw_timer_init();
#endif
}

void krhino_start_hook(void)
{
#if (RHINO_CONFIG_TASK_SCHED_STATS > 0)
    krhino_task_sched_stats_reset();
#endif
}

void krhino_task_create_hook(ktask_t *task)
{
    (void)task;
}

void krhino_task_del_hook(ktask_t *task, res_free_t *arg)
{
    (void)task;
    (void)arg;
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

void krhino_idle_pre_hook(void)
{
}
#endif
/**
* krhino hook for k_hook.h
* -- end
*/

/*
* k_mm heap config, assign value to g_mm_region and g_region_num. They will be used in k_mm_init.
*/
/**
1、Heap addr region can be define in link file like linkscript(ld\sct). We recommend that all the left RAM assigned to heap region.
example:
    PROVIDE (heap_start = __stack);                 //end of stack
    __heap_limit = ORIGIN(RAM) + LENGTH(RAM);
    PROVIDE (heap_end = __heap_limit);
    PROVIDE (heap_len = heap_end - heap_start);
2、Heap addr region can also be define in assembler file;
example:
    heap_len      EQU     0x200
    AREA    HEAP, NOINIT, READWRITE, ALIGN=3
    heap_start
    Heap_Mem        SPACE   heap_len
    heap_end
3、Use an array to define a heap region.
example:
    uint8_t heap_start[HEAP_BUFFER_SIZE];
    k_mm_region_t g_mm_region[] = {{g_heap_buf, HEAP_BUFFER_SIZE}};
*/
#if (RHINO_CONFIG_MM_TLF > 0)
#if defined (__CC_ARM)  /* Keil / armcc */
extern unsigned int Image$$RW_IRAM1$$ZI$$Limit;
//extern unsigned int Image$$RW_IRAM1$$Base;
//extern unsigned int Image$$RW_IRAM1$$Limit;

k_mm_region_t g_mm_region[1];
int           g_region_num = 1;

size_t g_iram1_start = 0x20000000;//ram的起始地址
size_t g_iram1_total_size = 0x00020000;//ram的最大值


void aos_heap_set(void)
{
//    g_mm_region[0].start = &heap_start;   /*use &heap_start when defined in linkscript(*.ld/*.sct) */
//    g_mm_region[0].len   = &heap_len;     /*use &heap_len when defined in linkscript(*.ld/*.sct) */
	g_mm_region[0].start = (uint8_t*)&Image$$RW_IRAM1$$ZI$$Limit;
	g_mm_region[0].len   = g_iram1_start + g_iram1_total_size - (size_t)&Image$$RW_IRAM1$$ZI$$Limit;
}
#elif defined (__ICCARM__)  /* IAR */

#define HEAP_BUFFER_SIZE 1024*20
int           g_region_num = 1;
uint8_t g_heap_buf[HEAP_BUFFER_SIZE];
k_mm_region_t g_mm_region[] = {{g_heap_buf, HEAP_BUFFER_SIZE}};
void aos_heap_set(void)
{
    g_mm_region[0].start = g_heap_buf;    /* heap defined by array*/
    g_mm_region[0].len   = HEAP_BUFFER_SIZE;
}

#else /* GCC */
k_mm_region_t g_mm_region[1];
int           g_region_num = 1;
void aos_heap_set(void)
{
    g_mm_region[0].start = heap_start;    /* heap defined in assembler file*/
    g_mm_region[0].len   = heap_len;
}
#endif
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
size_t soc_get_cur_sp(void)
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

/*
Define err handler g_err_proc which will be used in k_err.c
*/
void soc_err_proc(kstat_t err)
{
    (void)err;
		printf("err %d\n",err);
	#if (RHINO_CONFIG_TASK_STACK_CUR_CHECK > 0)
    soc_print_stack();
    #endif

    assert(0);
}

krhino_err_proc_t g_err_proc = soc_err_proc;
