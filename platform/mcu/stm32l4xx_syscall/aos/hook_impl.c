/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>

#define PROC_MSG_NUM 100

#if (RHINO_CONFIG_USER_HOOK > 0)
void krhino_idle_hook(void)
{

}

void krhino_init_hook(void)
{

}

void krhino_start_hook(void)
{
#if (RHINO_CONFIG_HW_COUNT > 0)
    /* TIM2 used for soc_hr_hw_cnt_get() and soc_lr_hw_cnt_get() function*/
    soc_hw_timer_init();
#endif
}

void krhino_task_create_hook(ktask_t *task)
{
    CPSR_ALLOC();

    kqueue_t *queue;
    kstat_t   ret;

    if (task == NULL)
        return;

    if (task->is_proc == 1) {
        ret = krhino_queue_dyn_create(&queue, "cli_queue", PROC_MSG_NUM);
        if (ret != RHINO_SUCCESS) {
            return;
        }

        task->cli_q = queue;

        RHINO_CRITICAL_ENTER();
        klist_insert(&(task->kobj_list.queue_head), &queue->blk_obj.obj_list);
        RHINO_CRITICAL_EXIT();
    }
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

