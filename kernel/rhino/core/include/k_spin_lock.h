/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef K_SPIN_LOCK_H
#define K_SPIN_LOCK_H

typedef struct {
#if (RHINO_CONFIG_CPU_NUM > 1)
    volatile uint32_t owner;  /* cpu index of owner */
#endif
    cpu_cpsr_t        cpsr;   /* the interrupt key for this lock */
} kspinlock_t;

/* Be careful nested spin lock is not supported */
#if (RHINO_CONFIG_CPU_NUM > 1)

#define KRHINO_SPINLOCK_FREE_VAL             0xB33FFFFFu

/* SMP spin lock */
#define krhino_spin_lock(lock)               do {                                       \
                                                 cpu_spin_lock((lock));                 \
                                             } while (0)                                \

#define krhino_spin_unlock(lock)             do {                                       \
                                                 cpu_spin_unlock((lock));               \
                                             } while (0)

#define krhino_spin_lock_irq_save(lock)      do {                                       \
                                                 kspinlock_t *s = (kspinlock_t *)(lock);\
                                                 s->cpsr        = cpu_intrpt_save();    \
                                                 cpu_spin_lock((lock));                 \
                                             } while (0)

#define krhino_spin_unlock_irq_restore(lock) do {                                       \
                                                 kspinlock_t *s = (kspinlock_t *)(lock);\
                                                 cpu_spin_unlock((lock));               \
                                                 cpu_intrpt_restore(s->cpsr);           \
                                             } while (0)

#define krhino_spin_lock_init(lock)          do {                                       \
                                                 kspinlock_t *s = (kspinlock_t *)(lock);\
                                                 s->owner       = KRHINO_SPINLOCK_FREE_VAL;\
                                             } while(0)
#else
/* UP spin lock */
#define krhino_spin_lock(lock)              krhino_sched_disable();
#define krhino_spin_unlock(lock)            krhino_sched_enable();

#define krhino_spin_lock_irq_save(lock)      do {                                       \
                                                 kspinlock_t *s = (kspinlock_t *)(lock);\
                                                 s->cpsr        = cpu_intrpt_save();    \
                                             } while (0)

#define krhino_spin_unlock_irq_restore(lock) do {                                       \
                                                 kspinlock_t *s = (kspinlock_t *)(lock);\
                                                 cpu_intrpt_restore(s->cpsr);           \
                                             } while (0)

#define krhino_spin_lock_init(lock)
#endif

#endif

