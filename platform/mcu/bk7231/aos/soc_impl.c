/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <assert.h>

#if (RHINO_CONFIG_HW_COUNT > 0)
void soc_hw_timer_init(void)
{
}

hr_timer_t soc_hr_hw_cnt_get(void)
{
    return *(volatile uint64_t *)0xc0000120;
}

lr_timer_t soc_lr_hw_cnt_get(void)
{
    return 0;
}
#endif /* RHINO_CONFIG_HW_COUNT */

#include <k_api.h>

void soc_hw_timer_init()
{
}

#if (RHINO_CONFIG_USER_HOOK > 0)
void krhino_idle_pre_hook(void)
{
}

void krhino_init_hook(void)
{
}

void krhino_start_hook(void)
{
}

void krhino_task_create_hook(ktask_t *task)
{
    (void)task;
}

void krhino_task_del_hook(ktask_t *task, res_free_t *arg)
{
    (void)task;
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


UINT32 global_tick = 0;
extern tick_t	  g_tick_count;
void krhino_idle_hook(void)
{
	UINT32 mcu_ps_tick = 24;
	UINT32 mcu_miss_tick = 0;

#if (NX_POWERSAVE)
    GLOBAL_INT_DECLARATION();
    CPSR_ALLOC();

    if (ke_evt_get() != 0)
    {
        return ;
    }

    if(!bmsg_is_empty())
    {
        return ;
    }

    GLOBAL_INT_DISABLE();
    if((INT32)(global_tick + (UINT32)1 - g_tick_count) <=  0)
	{

	    mcu_miss_tick = mcu_power_save(mcu_ps_tick);
        #if 0
	    RHINO_CPU_INTRPT_DISABLE();
	    g_tick_count += mcu_miss_tick;
	    global_tick = g_tick_count;
	    RHINO_CPU_INTRPT_ENABLE();
        #else
        tick_list_update(mcu_miss_tick);
        global_tick = g_tick_count;
        #endif
	}
    GLOBAL_INT_RESTORE();
#endif //(NX_POWERSAVE)
}
#endif

extern void         *heap_start;
extern void         *heap_end;
extern void         *heap_len;

k_mm_region_t g_mm_region[] = {(uint8_t*)&heap_start,(size_t)&heap_len};
int           g_region_num  = sizeof(g_mm_region)/sizeof(k_mm_region_t);


void soc_err_proc(kstat_t err)
{
    (void)err;
    printf("panic %d!\r\n",err);
    assert(0);
}

krhino_err_proc_t g_err_proc = soc_err_proc;

