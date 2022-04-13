/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include "hal_trace.h"
#include "hal_gpio.h"
#include "hal_sleep.h"
#include "hal_timer.h"
#include "hwtimer_list.h"
#include "board.h"
#include "watchdog.h"

#ifdef AOS_COMP_OSAL_POSIX
#include "posix/pthread.h"
extern pthread_key_list_t pthread_key_list_head;
#endif

#if RHINO_CONFIG_USER_HOOK
extern const k_mm_region_t g_mm_region[];
extern int g_region_num;
static int mem_in_heap(uint32_t addr)
{
    int i;
    for(i = 0; i < g_region_num; i++){
        if(addr > (uint32_t)g_mm_region[i].start && addr < (uint32_t)g_mm_region[i].start + g_mm_region[i].len){
            return 1;
	}
    }
    return 0;
}

void krhino_tick_hook(void)
{
    static unsigned int idx = 0;
    if((idx % (RHINO_CONFIG_TICKS_PER_SECOND / 2) ) == 0) {
        watchdog_feeddog();
    }
    idx ++;
}

HWTIMER_ID krhino_sleep_timer;

void krhino_sleep(void)
{
    hal_sleep_enter_sleep();
}

static int idle_sleep = 1; // swd will set it
void krhino_idle_hook_onoff(int onoff)
{
    idle_sleep = onoff;
}
void krhino_idle_hook(void)
{
    uint32_t suspend_time;
    enum E_HWTIMER_T ret;
    uint32_t sleep_ms;

    if (idle_sleep == 0)
        return ;

    if ((hal_sleep_light_sleep() == HAL_SLEEP_STATUS_DEEP)) {
        tick_t tick = krhino_next_sleep_ticks_get();
        sleep_ms = krhino_ticks_to_ms(tick);
        if (sleep_ms > 60*1000)
            sleep_ms = 60*1000;
        ret = hwtimer_start(krhino_sleep_timer, MS_TO_HWTICKS(sleep_ms));
        if (ret == E_HWTIMER_OK) {
            soc_systick_stop();
            suspend_time = hal_sys_timer_get();
            krhino_sleep();
            hwtimer_stop(krhino_sleep_timer);
            suspend_time = TICKS_TO_MS(hal_sys_timer_get() - suspend_time);
            soc_systick_start();
            tick_list_update(krhino_ms_to_ticks(suspend_time));
        }
    }
}

void aos_trace_crash_notify()
{
    abort();
}

void krhino_idle_pre_hook(void)
{
    hal_trace_crash_dump_register(HAL_TRACE_CRASH_DUMP_MODULE_SYS, aos_trace_crash_notify);
    krhino_sleep_timer = hwtimer_alloc(NULL, NULL);
    ASSERT(krhino_sleep_timer, "IdleTask: Failed to alloc sleep timer");
}

void krhino_start_hook(void)
{
}

void krhino_task_create_hook(ktask_t *task)
{
}

void krhino_task_del_hook(ktask_t *task, res_free_t *arg)
{
    /*free task->task_sem_obj*/
    void * task_sem = task->task_sem_obj;
    g_sched_lock[cpu_cur_get()]++;
    if(task_sem) {
        krhino_task_sem_del(task);
        if(mem_in_heap((uint32_t)task_sem)){
            aos_free(task_sem);
        }
        task->task_sem_obj = NULL;
    }
    g_sched_lock[cpu_cur_get()]--;

    return;
}


void krhino_init_hook(void)
{
}

void krhino_task_switch_hook(ktask_t *orgin, ktask_t *dest)
{
}

void krhino_mm_alloc_hook(void *mem, size_t size)
{
}

void krhino_task_abort_hook(ktask_t *task)
{
}
#endif
