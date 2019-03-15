/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <k_api.h>

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
#if (RHINO_CONFIG_SYS_STATS > 0)
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
1¡¢Heap addr region can be define in link file like linkscript(ld\sct). We recommend that all the left RAM assigned to heap region.
example:
    PROVIDE (heap_start = __stack);                 //end of stack
    __heap_limit = ORIGIN(RAM) + LENGTH(RAM);
    PROVIDE (heap_end = __heap_limit);
    PROVIDE (heap_len = heap_end - heap_start);
2¡¢Heap addr region can also be define in assembler file;
example:
    heap_len      EQU     0x200
    AREA    HEAP, NOINIT, READWRITE, ALIGN=3
    heap_start
    Heap_Mem        SPACE   heap_len
    heap_end
3¡¢Use an array to define a heap region.
example:
    uint8_t heap_start[HEAP_BUFFER_SIZE];
    k_mm_region_t g_mm_region[] = {{g_heap_buf, HEAP_BUFFER_SIZE}};
*/
#if (RHINO_CONFIG_MM_TLF > 0)
#if defined (__CC_ARM)  /* Keil / armcc */
k_mm_region_t g_mm_region[1];
int           g_region_num = 1;

void aos_heap_set()
{
    g_mm_region[0].start = &heap_start;   /*use &heap_start when defined in linkscript(*.ld/*.sct) */
    g_mm_region[0].len   = &heap_len;     /*use &heap_len when defined in linkscript(*.ld/*.sct) */
}
#elif defined (__ICCARM__)  /* IAR */

#define HEAP_BUFFER_SIZE 1024*20
int           g_region_num = 1;
uint8_t g_heap_buf[HEAP_BUFFER_SIZE];
k_mm_region_t g_mm_region[] = {{g_heap_buf, HEAP_BUFFER_SIZE}};
void aos_heap_set()
{
    g_mm_region[0].start = g_heap_buf;    /* heap defined by array*/
    g_mm_region[0].len   = HEAP_BUFFER_SIZE;
}

#else /* GCC */
k_mm_region_t g_mm_region[1];
int           g_region_num = 1;
void aos_heap_set()
{
    g_mm_region[0].start = heap_start;    /* heap defined in assembler file*/
    g_mm_region[0].len   = heap_len;
}
#endif
#endif

/*
Define err handler g_err_proc which will be used in k_err.c
*/
void soc_err_proc(kstat_t err)
{
    (void)err;
    assert(0);
}

krhino_err_proc_t g_err_proc = soc_err_proc;
