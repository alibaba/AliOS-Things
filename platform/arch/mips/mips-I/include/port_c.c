/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>

#include "hal_sys.h"

void *cpu_task_stack_init(cpu_stack_t *stack_base, size_t stack_size,
                          void *arg, task_entry_t entry)
{
    cpu_stack_t *stk;
    //    uint32_t     sr_val;
    //    uint32_t     gp_val;

    uint32_t temp = (uint32_t)(stack_base + stack_size - 1);

    /* stack aligned by 8 byte */
    temp &= 0xfffffff8;
    stk = (cpu_stack_t *)temp;

    *stk--  = (uint32_t)1;                   /* GPR[31]                                              */
    *stk--  = (uint32_t)entry;             /* GPR[30]                                              */
    *stk--  = (uint32_t)arg;                           /* GPR[28]                                              */
    *stk--  = (uint32_t)0;             /* GPR[27]                                              */
    *stk = (uint32_t)0;             /* GPR[26]                                              */
    return stk;
}

size_t cpu_intrpt_save(void)
{
    return (size_t)hal_SysEnterCriticalSection();
}

void   cpu_intrpt_restore(size_t cpsr)
{
    hal_SysExitCriticalSection((uint32_t)cpsr);
}

void cpu_intrpt_switch(void)
{
    ktask_t     *old = g_active_task[0];
    g_active_task[0] = g_preferred_ready_task[0];
    SVC_TASK_SWITCH(old, g_preferred_ready_task[0]);
}

void cpu_task_switch(void)
{
    ktask_t     *old = g_active_task[0];
    g_active_task[0] = g_preferred_ready_task[0];
    SVC_TASK_SWITCH(old, g_preferred_ready_task[0]);
}

