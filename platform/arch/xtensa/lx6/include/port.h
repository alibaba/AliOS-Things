/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef PORT_H
#define PORT_H
#include <xtensa/tie/xt_core.h>
#include <xtensa/hal.h>
#include <xtensa/config/core.h>
#include <xtensa/config/system.h>       /* required for XSHAL_CLIB */
#include <xtensa/xtruntime.h>
#include <xtensa_api.h>

#if (RHINO_CONFIG_CPU_NUM > 1)
#include "smp_port.h"
#endif

void   cpu_intrpt_switch(void);
void   cpu_task_switch(void);
void   cpu_first_task_start(void);
void  *cpu_task_stack_init(cpu_stack_t *base, size_t size, void *arg, task_entry_t entry);

/* int lock for spinlock */
#define cpu_intrpt_save()           XTOS_SET_INTLEVEL(XCHAL_EXCM_LEVEL)
#define cpu_intrpt_restore(cpsr)    XTOS_RESTORE_JUST_INTLEVEL(cpsr)

#if (RHINO_CONFIG_CPU_NUM > 1)
extern void k_cpu_spin_lock(kspinlock_t *lock);
extern void k_cpu_spin_unlock(kspinlock_t *lock);

#define cpu_spin_lock               k_cpu_spin_lock
#define cpu_spin_unlock             k_cpu_spin_unlock

#define CPSR_ALLOC()                cpu_cpsr_t cpsr
#define RHINO_CPU_INTRPT_DISABLE()  \
        do{cpsr = cpu_intrpt_save();krhino_spin_lock(&g_sys_lock);}while(0)
#define RHINO_CPU_INTRPT_ENABLE()   \
        do{krhino_spin_unlock(&g_sys_lock); cpu_intrpt_restore(cpsr);}while(0)

#else
/* normal int lock (can not lock the NMI) */
#define CPSR_ALLOC()                cpu_cpsr_t cpsr
#define RHINO_CPU_INTRPT_DISABLE()  \
        do{cpsr = cpu_intrpt_save();}while(0)
#define RHINO_CPU_INTRPT_ENABLE()   \
        do{cpu_intrpt_restore(cpsr);}while(0)
#endif

#if (RHINO_CONFIG_CPU_NUM > 1)
void cpu_signal(uint8_t cpu_num);

RHINO_INLINE uint8_t cpu_cur_get(void)
{
    return xPortGetCoreID();
}

static inline void cpu_atomic_compare_set(volatile uint32_t *addr, uint32_t compare, uint32_t *set) {
    __asm__ __volatile__(
        "WSR 	    %2,SCOMPARE1 \n"
        "ISYNC      \n"
        "S32C1I     %0, %1, 0	 \n"
        :"=r"(*set)
        :"r"(addr), "r"(compare), "0"(*set)
        );
}

#else
RHINO_INLINE uint8_t cpu_cur_get(void)
{
    return 0;
}
#endif

#endif /* PORT_H */

