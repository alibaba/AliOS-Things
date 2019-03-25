/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef PORT_H
#define PORT_H

#include <signal.h>

cpu_cpsr_t cpu_intrpt_save(void);
void   cpu_intrpt_restore(cpu_cpsr_t cpsr);
void   cpu_intrpt_switch(void);
void   cpu_first_task_start(void);
void  *cpu_task_stack_init(cpu_stack_t *base, size_t size, void *arg, task_entry_t entry);
void   cpu_task_switch(void);

#if (RHINO_CONFIG_CPU_NUM > 1)
#define CPSR_ALLOC()                cpu_cpsr_t cpsr
#define RHINO_CPU_INTRPT_DISABLE()  \
        do{cpsr = cpu_intrpt_save(); krhino_spin_lock(&g_sys_lock);}while(0)
#define RHINO_CPU_INTRPT_ENABLE()   \
        do{krhino_spin_unlock(&g_sys_lock);cpu_intrpt_restore(cpsr);}while(0)

static inline void cpu_atomic_compare_set(volatile uint32_t *addr, uint32_t compare, uint32_t *set)
{
	return ;
}

void cpu_signal(uint8_t cpu_num);
uint8_t cpu_cur_get(void);
void cpu_spin_lock(kspinlock_t *lock);
void cpu_spin_unlock(kspinlock_t *lock);

#else
#define CPSR_ALLOC()                cpu_cpsr_t cpsr
#define RHINO_CPU_INTRPT_DISABLE()  do{cpsr = cpu_intrpt_save();}while(0)
#define RHINO_CPU_INTRPT_ENABLE()   do{cpu_intrpt_restore(cpsr);}while(0)

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

#endif /* PORT_H */

