/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
// #include "hal_gpio.h"
// #include "hal_sleep.h"
// #include "hal_timer.h"
#include "board.h"
//#include "watchdog.h"

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
    // do nothing
}

// HWTIMER_ID krhino_sleep_timer;

// void krhino_sleep(void)
// {
//     hal_sleep_enter_sleep();
// }

static int idle_sleep = 1; // swd will set it
void krhino_idle_hook_onoff(int onoff)
{
    idle_sleep = onoff;
}
void krhino_idle_hook(void)
{

}

void aos_trace_crash_notify()
{
    abort();
}

void krhino_idle_pre_hook(void)
{

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
