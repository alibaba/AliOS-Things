/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#ifndef RHINO_SMP_PORT_H
#define RHINO_SMP_PORT_H

#include "k_config.h"
#if (RHINO_CONFIG_CPU_NUM > 1)
#if (RHINO_CONFIG_USER_HOOK <= 0)
#error RHINO_CONFIG_USER_HOOK > 0 is needed in smp for g_cpu_flag use in krhino_idle_pre_hook
#endif
#endif

/*crosscore int*/
extern void cpu_signal(uint8_t cpu_num);
/*should use krhino_intrpt_exit to schedule*/
extern void os_crosscore_isr(void *arg);
/*os_crosscore_int_init should be used in all cores' initial*/
extern void os_crosscore_int_init();


/*smp sync*/
/*wait all cores started*/
//void k_wait_allcores(void);
/*spin lock*/

/*
void cpu_spin_lock(kspinlock_t *lock);
void cpu_spin_unlock(kspinlock_t *lock);
*/
void os_unlock_sys_spin(void);

/*load other cores*/
void os_load_slavecpu(void);

#endif
