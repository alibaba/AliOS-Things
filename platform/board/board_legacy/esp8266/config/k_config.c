/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "frxt/xtensa_config.h"

#define WDT_TIMEOUT_MS  20000

void soc_hw_timer_init()
{
}

#if (RHINO_CONFIG_USER_HOOK > 0)
void krhino_idle_pre_hook(void)
{
}

void krhino_idle_hook(void)
{
    extern void vApplicationIdleHook(void);
    vApplicationIdleHook();
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

size_t soc_get_cur_sp()
{
    volatile size_t dummy = (size_t)&dummy;
    return dummy;
}

#if (RHINO_CONFIG_HW_COUNT > 0)
hr_timer_t soc_hr_hw_cnt_get(void)
{
    return 0;
}
#endif

/* iram use as memory heap: */
#define IRAM_AS_HEAP_ADDR   ((void*)0x40108000)
#define IRAM_AS_HEAP_LEN    0x4000

extern uint8_t _heap_start, _heap_size;
const k_mm_region_t g_mm_region[] = {
    {&_heap_start, (uint32_t)&_heap_size},
    //{IRAM_AS_HEAP_ADDR, IRAM_AS_HEAP_LEN}
};
int g_region_num  = sizeof(g_mm_region)/sizeof(k_mm_region_t);

extern void hal_reboot(void);
void soc_err_proc(kstat_t err)
{
    if ( RHINO_NO_MEM == err )
    {
        /* while mem not enought, reboot */
        hal_reboot();
    }
}

krhino_err_proc_t g_err_proc = soc_err_proc;

