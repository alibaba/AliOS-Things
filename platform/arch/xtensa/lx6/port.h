/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef CPU_H
#define CPU_H
#include <xtensa/tie/xt_core.h>
#include <xtensa/hal.h>
#include <xtensa/config/core.h>
#include <xtensa/config/system.h>       /* required for XSHAL_CLIB */
#include <xtensa/xtruntime.h>

void   cpu_intrpt_switch(void);
void   cpu_task_switch(void);
void   cpu_first_task_start(void);
void  *cpu_task_stack_init(cpu_stack_t *base, size_t size, void *arg, task_entry_t entry);

/* int lock for spinlock */
#define cpu_intrpt_save()           XTOS_SET_INTLEVEL(XCHAL_EXCM_LEVEL)
#define cpu_intrpt_restore(cpsr)    XTOS_RESTORE_JUST_INTLEVEL(cpsr)

/* normal int lock (can not lock the NMI) */
#define CPSR_ALLOC() size_t cpsr
#define RHINO_CPU_INTRPT_DISABLE() { cpsr = cpu_intrpt_save(); }
#define RHINO_CPU_INTRPT_ENABLE()  { cpu_intrpt_restore(cpsr); }

RHINO_INLINE uint8_t cpu_cur_get(void)
{
    return 0;
}
#endif /* CPU_H */

