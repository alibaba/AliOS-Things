/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef PORT_H
#define PORT_H

#if (RHINO_CONFIG_CPU_NUM > 1)
#include "smp_port.h"
#endif

size_t cpu_intrpt_save(void);
void   cpu_intrpt_restore(size_t cpsr);
void   cpu_intrpt_switch(void);
void   cpu_task_switch(void);
void   cpu_first_task_start(void);
void * cpu_task_stack_init(cpu_stack_t *base, size_t size, void *arg, task_entry_t entry);

extern int cpu_get_current(void);


#define CPSR_ALLOC() size_t cpsr

#if (RHINO_CONFIG_CPU_NUM > 1)

extern void k_cpu_spin_lock(kspinlock_t *lock);
extern void k_cpu_spin_unlock(kspinlock_t *lock);

#define cpu_spin_lock       k_cpu_spin_lock
#define cpu_spin_unlock     k_cpu_spin_unlock


/* normal int lock (can not lock the NMI) */
#define RHINO_CPU_INTRPT_DISABLE() { cpsr = cpu_intrpt_save();krhino_spin_lock(&g_sys_lock); }
#define RHINO_CPU_INTRPT_ENABLE()  { krhino_spin_unlock(&g_sys_lock); cpu_intrpt_restore(cpsr);}

#else
#define RHINO_CPU_INTRPT_DISABLE() { cpsr = cpu_intrpt_save(); }
#define RHINO_CPU_INTRPT_ENABLE()  { cpu_intrpt_restore(cpsr); }
#endif



#if (RHINO_CONFIG_CPU_NUM > 1)
void cpu_signal(uint8_t cpu_num);

RHINO_INLINE uint8_t cpu_cur_get(void)
{
    return cpu_get_current();
}

static inline void osPortCompareSet(volatile uint32_t *addr, uint32_t compare, uint32_t *set)
{
	int oldval;
	unsigned long res;

	do {
		__asm__ __volatile__("@ atomic_cmpxchg\n"
		"ldrex	%1, [%3]\n"
		"mov	%0, #0\n"
		"teq	%1, %4\n"
		"strexeq %0, %5, [%3]\n"
		    : "=&r" (res), "=&r" (oldval), "+Qo" (*addr)
		    : "r" (addr), "Ir" (compare), "r" (*set)
		    : "cc");
	} while (res);

    *set = oldval;
	return ;
}

#else
RHINO_INLINE uint8_t cpu_cur_get(void)
{
    return 0;
}
#endif

#endif /* PORT_H */
