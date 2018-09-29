/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>

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

