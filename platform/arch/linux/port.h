/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef K_CPU_H
#define K_CPU_H

#include <signal.h>

#define cpu_bitmap_clz(val)    __builtin_clz((val))

sigset_t cpu_intrpt_save(void);
void   cpu_intrpt_restore(sigset_t cpsr);
void   cpu_intrpt_switch(void);
void   cpu_first_task_start(void);
void  *cpu_task_stack_init(cpu_stack_t *base, size_t size, void *arg, task_entry_t entry);
void   cpu_task_switch(void);

#define CPSR_ALLOC()    sigset_t cpsr

#if (RHINO_CONFIG_CPU_NUM > 1)
#define RHINO_CPU_INTRPT_DISABLE() { cpu_intrpt_save(); krhino_spin_lock(&g_sys_lock);}
#define RHINO_CPU_INTRPT_ENABLE()  { krhino_spin_unlock(&g_sys_lock);cpu_intrpt_restore(cpsr);}

void cpu_signal(uint8_t cpu_num);
uint8_t cpu_cur_get(void);
void cpu_spin_lock(kspinlock_t *lock);
void cpu_spin_unlock(kspinlock_t *lock);
#else
#define RHINO_CPU_INTRPT_DISABLE() { cpu_intrpt_save();}
#define RHINO_CPU_INTRPT_ENABLE()  { cpu_intrpt_restore(cpsr);}

RHINO_INLINE uint8_t cpu_cur_get(void)
{
    return 0;
}
#endif

void   cpu_idle_hook(void);
void   cpu_init_hook(void);
void   cpu_start_hook(void);
void   cpu_task_create_hook(ktask_t *tcb);
void   cpu_task_del_hook(ktask_t *tcb, res_free_t *arg);

#endif /* K_CPU_H */

