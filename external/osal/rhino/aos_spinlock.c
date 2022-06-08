/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <time.h>

#include <k_api.h>
#include <aos/errno.h>
#include <aos/kernel.h>

#include "rhino_p.h"

#if (RHINO_CONFIG_KOBJ_DYN_ALLOC == 0)
#warning "RHINO_CONFIG_KOBJ_DYN_ALLOC is disabled!"
#endif

void aos_spin_lock_init(aos_spinlock_t *spinlock)
{
    krhino_spin_lock_init((kspinlock_t *)spinlock);
}

void aos_spin_lock(aos_spinlock_t *spinlock)
{
    krhino_spin_lock((kspinlock_t *)spinlock);
}

void aos_spin_unlock(aos_spinlock_t *spinlock)
{
    krhino_spin_unlock((kspinlock_t *)spinlock);
}

aos_irqsave_t aos_spin_lock_irqsave(aos_spinlock_t *spinlock)
{
    aos_irqsave_t flag;
    krhino_spin_lock_irq_save((kspinlock_t *)spinlock, flag);
    return flag;
}

void aos_spin_unlock_irqrestore(aos_spinlock_t *spinlock, aos_irqsave_t flag)
{
    krhino_spin_unlock_irq_restore((kspinlock_t *)spinlock, flag);
}
