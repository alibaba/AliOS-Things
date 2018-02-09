/*
 * Copyright (C) 2016 YunOS Project. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <csi_config.h>
#include <stdlib.h>
#include <stdio.h>
#include <k_api.h>
#include <drv_timer.h>
#include <soc.h>
#include <csi_core.h>

#define THIS_MODULE MODULE_NONE

#ifndef RHINO_CONFIG_STD_MALLOC
static k_mm_region_head_t hobbit_mm_region_head;
#endif

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
#endif
uint8_t g_heap_buf[CONFIG_MM_KERNEL_HEAPSIZE];

const k_mm_region_t g_mm_region[] = {
    {g_heap_buf, CONFIG_MM_KERNEL_HEAPSIZE},
};
int g_region_num  = sizeof(g_mm_region)/sizeof(k_mm_region_t);

#if (RHINO_CONFIG_INTRPT_GUARD > 0)
void yoc_intrpt_guard(void)
{
    return;
}
#endif

#if (RHINO_CONFIG_INTRPT_STACK_REMAIN_GET > 0)
size_t yoc_intrpt_stack_remain_get(void)
{
    return 0;
}
#endif

#if (RHINO_CONFIG_INTRPT_STACK_OVF_CHECK > 0)
void yoc_intrpt_stack_ovf_check(void)
{
    return;
}
#endif

#if (RHINO_CONFIG_DYNTICKLESS > 0)

#define DYNTICKLESS_LDCNT_PER_TICK  (SYSTEM_CLOCK / RHINO_CONFIG_TICKS_PER_SECOND)
#define DYNTICKLESS_TICKS_DEFAULT   (10 * RHINO_CONFIG_TICKS_PER_SECOND)

extern volatile uint32_t g_prev_cnt;
tick_t ticks_from_last_interrupt = DYNTICKLESS_TICKS_DEFAULT;

tick_t soc_elapsed_ticks_get()
{
    uint32_t cur_cnt;
    uint32_t max_cnt;
    tick_t   elapsed_ticks;

#ifdef CONFIG_LPM_TICKLESS_SLEEP
        extern timer_handle_t count_timer;
        csi_timer_get_current_value(count_timer, &cur_cnt);
        elapsed_ticks = (~cur_cnt - ~g_prev_cnt) / DYNTICKLESS_LDCNT_PER_TICK;
#else
    if (krhino_bsp_intc_get_pending_irq(DYNTICKLESS_TIMER)) {
        elapsed_ticks = ticks_from_last_interrupt;
    } else {
        cur_cnt = dyntickless_get_current_value();
        max_cnt = dyntickless_get_load_value();
        elapsed_ticks = (~cur_cnt - ~max_cnt) / DYNTICKLESS_LDCNT_PER_TICK;
    }
#endif
    return elapsed_ticks;
}

void soc_tick_interrupt_set(tick_t next_ticks, tick_t elapsed_ticks)
{

#ifdef CONFIG_LPM_TICKLESS_SLEEP
    extern timer_handle_t system_timer;
    //extern timer_handle_t count_timer;
    if (next_ticks == (tick_t) - 1) {
        ticks_from_last_interrupt = DYNTICKLESS_TICKS_DEFAULT;
        csi_timer_stop(system_timer);
        csi_timer_set_timeout(system_timer, ticks_from_last_interrupt * (1000000 / RHINO_CONFIG_TICKS_PER_SECOND));
        csi_timer_start(system_timer);
        ticks_from_last_interrupt += elapsed_ticks;
    } else {
        if (next_ticks >= DYNTICKLESS_TICKS_DEFAULT) {
            ticks_from_last_interrupt = DYNTICKLESS_TICKS_DEFAULT - elapsed_ticks;
            csi_timer_stop(system_timer);
            csi_timer_set_timeout(system_timer, ticks_from_last_interrupt * (1000000 / RHINO_CONFIG_TICKS_PER_SECOND));
            csi_timer_start(system_timer);
            ticks_from_last_interrupt += elapsed_ticks;
        } else {
            ticks_from_last_interrupt = next_ticks - elapsed_ticks;
            csi_timer_stop(system_timer);
            csi_timer_set_timeout(system_timer, ticks_from_last_interrupt * (1000000 / RHINO_CONFIG_TICKS_PER_SECOND));
            csi_timer_start(system_timer);
            ticks_from_last_interrupt += elapsed_ticks;
        }
    }
#else
    if (next_ticks == (tick_t) - 1) {
        ticks_from_last_interrupt = DYNTICKLESS_TICKS_DEFAULT;
        dyntickless_timer_stop();
        dyntickless_set_load_value(ticks_from_last_interrupt * DYNTICKLESS_LDCNT_PER_TICK);
        dyntickless_timer_start();
    } else {
        if (next_ticks >= DYNTICKLESS_TICKS_DEFAULT) {
            ticks_from_last_interrupt = DYNTICKLESS_TICKS_DEFAULT - elapsed_ticks;
            dyntickless_timer_stop();
            dyntickless_set_load_value(ticks_from_last_interrupt * DYNTICKLESS_LDCNT_PER_TICK);
            dyntickless_timer_start();
        } else {
            ticks_from_last_interrupt = next_ticks - elapsed_ticks;
            dyntickless_timer_stop();
            dyntickless_set_load_value(ticks_from_last_interrupt * DYNTICKLESS_LDCNT_PER_TICK);
            dyntickless_timer_start();
        }
    }
#endif
    return;
}
#endif

#if (RHINO_CONFIG_KOBJ_DYN_ALLOC > 0)

#ifdef RHINO_CONFIG_STD_MALLOC
#define SYS_DYN_POOL_SIZE 0x0
#else
#define CONFIG_MM_KERNEL_HEAPSIZE 0x2000
#define SYS_DYN_POOL_SIZE CONFIG_MM_KERNEL_HEAPSIZE
size_t sys_pool_start[SYS_DYN_POOL_SIZE / sizeof(size_t)];
int g_max_used_heap;
int g_cur_used_heap;
#endif

void soc_sys_mem_init(void)
{
#ifndef RHINO_CONFIG_STD_MALLOC
    k_mm_region_t regions[2];
    regions[0].start = (uint8_t *)sys_pool_start;
    regions[0].len   = SYS_DYN_POOL_SIZE;
    krhino_mm_region_init(&hobbit_mm_region_head,regions, 1);
#endif
}

#ifndef RHINO_CONFIG_STD_MALLOC
void *soc_mm_alloc(size_t size)
{
    void    *ptmp = NULL;
    kstat_t  err = RHINO_SUCCESS;
    size_t   free_mem = 0;
    uint32_t flags;

    size_t  alloctor = (size_t)__builtin_return_address(1);
    err = krhino_mm_bf_alloc(&hobbit_mm_region_head,&ptmp,size,alloctor);

    free_mem = krhino_mm_region_get_free_size(&hobbit_mm_region_head);

    flags = cpu_intrpt_save();
    g_cur_used_heap = SYS_DYN_POOL_SIZE - free_mem;
    if (g_cur_used_heap > g_max_used_heap) {
        g_max_used_heap = g_cur_used_heap;
    }
    cpu_intrpt_restore(flags);

#ifdef CONFIG_YOC_RHINO_MMREGION
    if (NULL == ptmp) {
        dumpsys_mm_info_func(NULL, 0);
    }
#endif

    return ptmp;
}

void soc_mm_free(void *mem)
{
    krhino_mm_bf_free(&hobbit_mm_region_head,mem);
}
#endif//#ifndef RHINO_CONFIG_STD_MALLOC
#endif//#if (RHINO_CONFIG_KOBJ_DYN_ALLOC > 0)

uint32_t soc_get_cur_sp(void)
{
    uint32_t result;

    asm volatile("mov %0, sp" : "=r"(result));
    return (result);
}

void yoc_get_first_frame_info(size_t c_frame, size_t * n_frame, size_t *pc)
{
}

void yoc_get_subs_frame_info(size_t c_frame, size_t *n_frame, size_t *pc)
{
}

void yoc_err_proc(kstat_t err)
{
    printf("stack overflow: %s!\n", g_active_task[0]->task_name);
    while(1);
}

krhino_err_proc_t g_err_proc = yoc_err_proc;

