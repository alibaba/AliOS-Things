/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef K_CRITICAL_H
#define K_CRITICAL_H

typedef struct {
#if (RHINO_CONFIG_CPU_NUM > 1)
    uint32_t    owner;  /* cpu index of owner */
#endif
    uint32_t    cnt;
    cpu_cpsr_t  cpsr;   /* the int key for this lock */
} kspinlock_t;

#if (RHINO_CONFIG_CPU_NUM > 1)
/* SMP spin lock */
#define krhino_spin_lock(lock)              do {                                       \
                                                kspinlock_t *s = (kspinlock_t*)(lock); \
                                                cpu_spin_lock((lock));                 \
                                                s->cnt++;                              \
                                            } while (0)                                \

#define krhino_spin_unlock(lock)            do {                                       \
                                                kspinlock_t *s = (kspinlock_t*)(lock); \
                                                s->cnt--;                              \
                                                if (s->cnt == 0u) {                    \
                                                    cpu_spin_unlock((lock));           \
                                                }                                      \
                                            } while (0)

#define krhino_spin_lock_irq_save(lock)     do {                                       \
                                                kspinlock_t *s = (kspinlock_t*)(lock); \
                                                s->cpsr        = cpu_intrpt_save();    \
                                                cpu_spin_lock((lock));                 \
                                                s->cnt++;                              \
                                            } while (0)

#define krhino_spin_unlock_irq_restore(lock) do {                                      \
                                                kspinlock_t *s = (kspinlock_t*)(lock); \
                                                s->cnt--;                              \
                                                if (s->cnt == 0u) {                    \
                                                    cpu_spin_unlock((lock));           \
                                                    cpu_intrpt_restore(s->cpsr);       \
                                                }                                      \
                                            } while (0)

#define krhino_spin_init(lock)              do{                                        \
                                                kspinlock_t *s = (kspinlock_t*)(lock); \
                                                s->owner = (uint32_t)-1;               \
                                                s->cnt   = 0u;                         \
                                            } while(0)
#else
/* UP spin lock */
#define krhino_spin_lock(lock)              krhino_sched_disable();
#define krhino_spin_unlock(lock)            krhino_sched_enable();

#define krhino_spin_lock_irq_save(lock)     do {                                       \
                                                kspinlock_t *s = (kspinlock_t*)(lock); \
                                                s->cpsr        = cpu_intrpt_save();    \
                                                s->cnt++;                              \
                                            } while (0)

#define krhino_spin_unlock_irq_restore(lock) do {                                      \
                                                kspinlock_t *s = (kspinlock_t*)(lock); \
                                                s->cnt--;                              \
                                                if (s->cnt == 0u) {                    \
                                                    cpu_intrpt_restore(s->cpsr);       \
                                                }                                      \
                                            } while (0)

#define krhino_spin_init(lock)
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

