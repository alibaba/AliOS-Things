/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef K_CRITICAL_H
#define K_CRITICAL_H

#if (RHINO_CONFIG_SYS_STATS > 0)
#define RHINO_INTDIS_MEAS_START() intrpt_disable_measure_start()
#define RHINO_INTDIS_MEAS_STOP()  intrpt_disable_measure_stop()
#else
#define RHINO_INTDIS_MEAS_START()
#define RHINO_INTDIS_MEAS_STOP()
#endif

#define RHINO_CRITICAL_ENTER()      \
    do {                            \
        RHINO_CPU_INTRPT_DISABLE(); \
        RHINO_INTDIS_MEAS_START();  \
    } while (0)
#define RHINO_CRITICAL_EXIT()       \
    do {                            \
        RHINO_INTDIS_MEAS_STOP();   \
        RHINO_CPU_INTRPT_ENABLE();  \
    } while (0)
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
        RHINO_CPU_INTRPT_ENABLE();  \
        core_sched();               \
    } while (0)
#endif

#endif /* K_CRITICAL_H */

