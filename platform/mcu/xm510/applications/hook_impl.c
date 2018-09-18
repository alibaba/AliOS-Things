/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include "pthread.h"

#if (RHINO_CONFIG_USER_HOOK > 0)
void krhino_idle_hook(void)
{
	asm volatile(
		"dsb\n"
		"wfi\n"
		);
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
    _pthread_tcb_t *ptcb;
    _pthread_cleanup_t *cleanup;

    g_sched_lock[cpu_cur_get()]++;

    ptcb = _pthread_get_tcb(task);

    if (ptcb == NULL) {
        g_sched_lock[cpu_cur_get()]--;
        return;
    }

    /* excute all cleanup function if existed */
    do {
        cleanup = ptcb->cleanup;
        if (cleanup != NULL ) {
            ptcb->cleanup = cleanup->prev;
            cleanup->cleanup_routine(cleanup->para);
            krhino_mm_free(cleanup);
        }
    } while(ptcb->cleanup != NULL);

    if (ptcb->attr.detachstate == PTHREAD_CREATE_JOINABLE) {
        /* give join sem if is joinable */
        if (ptcb->join_sem != NULL) {
            krhino_sem_give(ptcb->join_sem);
        }
    }

    if (arg == NULL) {
        g_sched_lock[cpu_cur_get()]--;
        return;
    }

    if (ptcb->attr.detachstate == PTHREAD_CREATE_DETACHED) {
        if (ptcb->join_sem != NULL) {
            krhino_sem_dyn_del(ptcb->join_sem);
        }

        klist_insert(&g_res_list, &arg->res_list);
        arg->res[0] = task->task_stack_base;
        arg->res[1] = task;
        arg->res[2] = ptcb;
        arg->cnt += 3;
        krhino_sem_give(&g_res_sem);
    }

    g_sched_lock[cpu_cur_get()]--;
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


