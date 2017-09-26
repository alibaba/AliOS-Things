/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef K_CRITICAL_H
#define K_CRITICAL_H

#if (RHINO_CONFIG_CPU_NUM > 1)
typedef struct {
    arch_spinlock_t lock;
} kspinlock_t;

#define krhino_spin_lock(lock)                     cpu_spin_lock((lock));
#define krhino_spin_unlock(lock)                   cpu_spin_unlock((lock));

#define krhino_spin_lock_irq_save(lock, flag)      do {\
                                                        flag = cpu_intrpt_save();    \
                                                        cpu_spin_lock((lock));       \
                                                    } while (0)

#define krhino_spin_lock_irq_restore(lock, flag)   do {\
                                                        cpu_spin_unlock((lock));     \
                                                        cpu_intrpt_restore((flag));  \
                                                    } while (0)
#else
typedef struct {
    uint8_t dummy;
} kspinlock_t;

#define krhino_spin_lock(lock)                     krhino_sched_disable();
#define krhino_spin_unlock(lock)                   krhino_sched_enable();

#define krhino_spin_lock_irq_save(lock, flag)      do {\
                                                        flag = cpu_intrpt_save();    \
                                                   } while (0)

#define krhino_spin_lock_irq_restore(lock, flag)   do {\
                                                        cpu_intrpt_restore((flag));  \
                                                    } while (0)

#endif

#if (RHINO_CONFIG_DISABLE_INTRPT_STATS > 0)
#define RHINO_CRITICAL_ENTER()          \
    do {                                \
        RHINO_CPU_INTRPT_DISABLE();     \
        intrpt_disable_measure_start(); \
    } while (0)

#define RHINO_CRITICAL_EXIT()           \
    do {                                \
        intrpt_disable_measure_stop();  \
        RHINO_CPU_INTRPT_ENABLE();      \
    } while (0)

#if (RHINO_CONFIG_CPU_NUM > 1)
#define RHINO_CRITICAL_EXIT_SCHED()         \
        do {                                \
            intrpt_disable_measure_stop();  \
            core_sched();                   \
            RHINO_CPU_INTRPT_ENABLE();      \
        } while (0)
#else
#define RHINO_CRITICAL_EXIT_SCHED()         \
        do {                                \
            intrpt_disable_measure_stop();  \
            RHINO_CPU_INTRPT_ENABLE();      \
            core_sched();                   \
        } while (0)
#endif

#else /* RHINO_CONFIG_DISABLE_INTRPT_STATS */
#define RHINO_CRITICAL_ENTER()          \
    do {                                \
        RHINO_CPU_INTRPT_DISABLE();     \
    } while (0)

#define RHINO_CRITICAL_EXIT()           \
    do {                                \
        RHINO_CPU_INTRPT_ENABLE();      \
    } while (0)

#if (RHINO_CONFIG_CPU_NUM > 1)
#define RHINO_CRITICAL_EXIT_SCHED()     \
        do {                            \
            core_sched();               \
            RHINO_CPU_INTRPT_ENABLE();  \
        } while (0)
#else
#define RHINO_CRITICAL_EXIT_SCHED()     \
        do {                            \
            RHINO_CPU_INTRPT_ENABLE();  \
            core_sched();               \
        } while (0)
#endif

#endif /* RHINO_CONFIG_DISABLE_INTRPT_STATS */

#endif /* K_CRITICAL_H */

