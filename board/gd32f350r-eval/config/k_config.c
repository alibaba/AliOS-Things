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
#if (RHINO_CONFIG_SYS_STATS > 0)
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

#if defined ( __GNUC__ )
extern void         *heap_start;
extern void         *heap_end;
extern void         *heap_len;
/* heap_start and heap_len is set by linkscript(*.ld) */
k_mm_region_t g_mm_region[] = {{(uint8_t*)&heap_start,(size_t)&heap_len}};
#else
#define HEAP_BUFFER_SIZE 1024*5
uint8_t g_heap_buf[HEAP_BUFFER_SIZE];
k_mm_region_t g_mm_region[] = {g_heap_buf, HEAP_BUFFER_SIZE};
#endif /* __GNUC__ */

int g_region_num  = sizeof(g_mm_region)/sizeof(k_mm_region_t);

#endif /* RHINO_CONFIG_MM_TLF */

void soc_err_proc(kstat_t err)
{
    (void)err;

    assert(0);
}

krhino_err_proc_t g_err_proc = soc_err_proc;
