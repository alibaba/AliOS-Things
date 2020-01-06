/**
 * @file k_spin_lock.h
 *
 * @copyright Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef K_SPIN_LOCK_H
#define K_SPIN_LOCK_H

/** @addtogroup aos_rhino spinlock
 *  Spinlock can be used for mutual exclusion between multi-cores.
 *
 *  @{
 */

/**
 * spinlock object
 */
typedef struct {
#if (RHINO_CONFIG_CPU_NUM > 1)
    volatile uint32_t owner;  /* cpu index of owner */
#endif
} kspinlock_t;

/* Be careful nested spin lock is not supported */
#if (RHINO_CONFIG_CPU_NUM > 1)

/**
 * Waiting for all cores.
 * Can only be used once for multi-core synchronization after initialization.
 *
 * @param[in]   NULL
 *
 * @return  NULL
 */
extern void k_wait_allcores(void);


#define KRHINO_SPINLOCK_FREE_VAL                        0xB33FFFFFu

/**
 * Lock a spinlock, recursive locking is allowed.
 *
 * @param[in]   lock    the spinlock
 *
 * @return  NULL
 */
#define krhino_spin_lock(lock)                          do {                                            \
                                                            cpu_spin_lock((lock));                      \
                                                        } while (0)                                     \

/**
 * Unlock a spinlock, recursive locking is allowed.
 *
 * @param[in]   lock    the spinlock
 *
 * @return  NULL
 */
#define krhino_spin_unlock(lock)                        do {                                            \
                                                            cpu_spin_unlock((lock));                    \
                                                        } while (0)

/**
 * Lock a spinlock and mask interrupt, recursive locking is allowed.
 *
 * @param[in]   lock    the spinlock
 * @param[out]  flags   the irq status before locking
 *
 * @return  NULL
 */
#define krhino_spin_lock_irq_save(lock, flags)          do {                                            \
                                                            flags          = cpu_intrpt_save();         \
                                                            cpu_spin_lock((lock));                      \
                                                        } while (0)

/**
 * Unlock a spinlock and restore interrupt masking status, recursive locking is allowed.
 *
 * @param[in]   lock    the spinlock
 * @param[in]   flags   the irq status before locking
 *
 * @return  NULL
 */
#define krhino_spin_unlock_irq_restore(lock, flags)     do {                                            \
                                                            cpu_spin_unlock((lock));                    \
                                                            cpu_intrpt_restore(flags);                  \
                                                        } while (0)

/**
 * Init a spinlock.
 *
 * @param[in]   lock    the spinlock
 *
 * @return  NULL
 */
#define krhino_spin_lock_init(lock)                     do {                                            \
                                                            kspinlock_t *s = (kspinlock_t *)(lock);     \
                                                            s->owner       = KRHINO_SPINLOCK_FREE_VAL;  \
                                                        } while(0)
#else
/* single-core spinlock */
#define krhino_spin_lock(lock)                          krhino_sched_disable();
#define krhino_spin_unlock(lock)                        krhino_sched_enable();

#define krhino_spin_lock_irq_save(lock, flags)          do {                                            \
                                                            (void)lock;                                 \
                                                            flags          = cpu_intrpt_save();         \
                                                        } while (0)

#define krhino_spin_unlock_irq_restore(lock, flags)     do {                                            \
                                                            (void)lock;                                 \
                                                            cpu_intrpt_restore(flags);                  \
                                                        } while (0)

#define krhino_spin_lock_init(lock)
#endif  /* RHINO_CONFIG_CPU_NUM > 1 */

/** @} */

#endif  /* K_SPIN_LOCK_H */

