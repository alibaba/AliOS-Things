/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include "frxt/xtensa_config.h"

void soc_hw_timer_init()
{
}

#if (RHINO_CONFIG_USER_HOOK > 0)
#if (RHINO_CONFIG_CPU_NUM > 1)
extern volatile uint64_t g_cpu_flag;
#endif
void krhino_idle_pre_hook(void)
{
    #if (RHINO_CONFIG_CPU_NUM > 1)
    CPSR_ALLOC();
    uint8_t cpu;

    RHINO_CPU_INTRPT_DISABLE();
    cpu = cpu_cur_get();
    g_cpu_flag |= (1UL << cpu);
    
    RHINO_CPU_INTRPT_ENABLE();
    #endif
}

#define OS_RHINO_IDLE_HOOK_NUM		10

typedef void (*os_hook_cb_t)();

os_hook_cb_t g_rhino_idle_hook[RHINO_CONFIG_CPU_NUM][OS_RHINO_IDLE_HOOK_NUM] = {0};


void krhino_idle_add_hook(void * fun, int cpuid)
{
	if(fun == 0 || cpuid > RHINO_CONFIG_CPU_NUM){
		return;
	}

	for(int i = 0; i< OS_RHINO_IDLE_HOOK_NUM;i++){
		if(g_rhino_idle_hook[cpuid][i] == NULL){
			g_rhino_idle_hook[cpuid][i] = fun;
		}
	}
	
}

void krhino_idle_del_hook(void * fun, int cpuid)
{
	if(fun == 0 || cpuid > RHINO_CONFIG_CPU_NUM){
		return;
	}

	for(int i = 0; i< OS_RHINO_IDLE_HOOK_NUM;i++){
		if(g_rhino_idle_hook[cpuid][i] == fun){
			g_rhino_idle_hook[cpuid][i] = NULL;
		}
	}

	
}


void krhino_idle_hook(void)
{
	int cpuid = cpu_cur_get();
	for(int i = 0; i< OS_RHINO_IDLE_HOOK_NUM;i++){
		if(g_rhino_idle_hook[cpuid][i]){
			g_rhino_idle_hook[cpuid][i]();
		}
	}

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


#define OS_RHINO_TICK_HOOK_NUM		10

os_hook_cb_t g_rhino_tick_hook[RHINO_CONFIG_CPU_NUM][OS_RHINO_TICK_HOOK_NUM] = {0};


void krhino_tick_add_hook(void * fun, int cpuid)
{
	if(fun == 0 || cpuid > RHINO_CONFIG_CPU_NUM){
		return;
	}

	for(int i = 0; i< OS_RHINO_TICK_HOOK_NUM;i++){
		if(g_rhino_tick_hook[cpuid][i] == NULL){
			g_rhino_tick_hook[cpuid][i] = fun;
		}
	}
	
}

void krhino_tick_del_hook(void * fun, int cpuid)
{
	if(fun == 0 || cpuid > RHINO_CONFIG_CPU_NUM){
		return;
	}

	for(int i = 0; i< OS_RHINO_TICK_HOOK_NUM;i++){
		if(g_rhino_tick_hook[cpuid][i] == fun){
			g_rhino_tick_hook[cpuid][i] = NULL;
		}
	}

	
}


void krhino_tick_hook(void)
{
	int cpuid = cpu_cur_get();
	for(int i = 0; i< OS_RHINO_TICK_HOOK_NUM;i++){
		if(g_rhino_tick_hook[cpuid][i]){
			g_rhino_tick_hook[cpuid][i]();
		}
	}

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

