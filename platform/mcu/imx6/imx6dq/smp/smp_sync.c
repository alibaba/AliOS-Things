/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include <stdint.h>
#include <string.h>

#include "k_api.h"

extern int printf(char *fmt, ...);

/*for spin lock unnest,lock->owner should be initialed as 0*/
void cpu_spin_lock_unnest(kspinlock_t *lock)
{
    spinlock_lock(&(lock->owner),0);

}

void cpu_spin_unlock_unnest(kspinlock_t *lock)
{
    spinlock_unlock(&(lock->owner));
}

void os_unlock_sys_spin(void)
{
    //ets_printf("os_unlock_sys_spin,core:%d\r\n",cpu_cur_get());
    krhino_spin_unlock(&g_sys_lock);
}


