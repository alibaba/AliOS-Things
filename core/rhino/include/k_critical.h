/**
 * @file k_critical.h
 *
 * @copyright Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef K_CRITICAL_H
#define K_CRITICAL_H

/** @addtogroup aos_rhino critical
 *  Critical zone protection, os use internally.
 *
 *  @{
 */

#if (RHINO_CONFIG_SYS_STATS > 0)
/* For measure max int disable count */
#define RHINO_INTDIS_MEAS_START() intrpt_disable_measure_start()
#define RHINO_INTDIS_MEAS_STOP()  intrpt_disable_measure_stop()
#else
#define RHINO_INTDIS_MEAS_START()
#define RHINO_INTDIS_MEAS_STOP()
#endif

/**
 * Critical zone begin.
 *
 * @return none
 */
#define RHINO_CRITICAL_ENTER()      \
    do {                            \
        RHINO_CPU_INTRPT_DISABLE(); \
        RHINO_INTDIS_MEAS_START();  \
    } while (0)

/**
 * Critical zone end.
 *
 * @return none
 */
#define RHINO_CRITICAL_EXIT()       \
    do {                            \
        RHINO_INTDIS_MEAS_STOP();   \
        RHINO_CPU_INTRPT_ENABLE();  \
    } while (0)

/**
 * Critical zone end and trigger scheduling.
 *
 * @return none
 */
#if (RHINO_CONFIG_CPU_NUM > 1)
#define RHINO_CRITICAL_EXIT_SCHED() \
    do {                            \
        RHINO_INTDIS_MEAS_STOP();   \
        core_sched();               \
        cpu_intrpt_restore(cpsr);   \
    } while (0)
#else
#define RHINO_CRITICAL_EXIT_SCHED() \
    do {                            \
        RHINO_INTDIS_MEAS_STOP();   \
        core_sched();               \
        RHINO_CPU_INTRPT_ENABLE();  \
    } while (0)
#endif

/** @} */

#endif /* K_CRITICAL_H */

