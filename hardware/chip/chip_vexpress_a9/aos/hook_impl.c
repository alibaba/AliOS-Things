/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "k_api.h"
#include "aos/cli.h"

#if (RHINO_CONFIG_USER_HOOK > 0)
#ifdef CONFIG_GENIE_DEBUG
#define TASK_SWITCH_WATCH_NO 3
extern int arm_install_hw_watchpoint(int i, unsigned int addr);
extern void arm_clear_hw_watchpoint(int i);
#endif

volatile int xSchedulerRunning[RHINO_CONFIG_CPU_NUM] = {0};
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
    xSchedulerRunning[cpu] = 1;

    RHINO_CPU_INTRPT_ENABLE();
#endif
}

void krhino_idle_hook(void)
{
    __asm__ __volatile__ ("wfi");
}

void krhino_init_hook(void)
{
}

#endif /* if (RHINO_CONFIG_USER_HOOK > 0) */

void krhino_start_hook(void)
{
#if (RHINO_CONFIG_SYS_STATS > 0)
    krhino_task_sched_stats_reset();
#endif
}

void krhino_task_create_hook(ktask_t *task)
{
#ifdef CONFIG_AW_MULTIPLE_CONSOLE
    void * cli_task_get_console(void * task_handle);
    void * cli_task_set_console(void * task_handle, void * console);
    cli_task_set_console(task, cli_task_get_console(krhino_cur_task_get()));
#endif
}

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

void krhino_task_switch_hook(ktask_t *orgin, ktask_t *dest)
{
    (void)orgin;
    cpu_stack_t *task_stack_dst = dest->task_stack_base;

#ifdef CONFIG_GENIE_DEBUG
    arm_clear_hw_watchpoint(TASK_SWITCH_WATCH_NO);
    arm_install_hw_watchpoint(TASK_SWITCH_WATCH_NO, (unsigned int)task_stack_dst);
#endif
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
