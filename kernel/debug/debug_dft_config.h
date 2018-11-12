/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef DEBUG_DFTDBG_CONFIG_H
#define DEBUG_DFTDBG_CONFIG_H

#ifndef RHINO_CONFIG_BACKTRACE
#define RHINO_CONFIG_BACKTRACE 0
#endif

/* If AliOS task over the Exception/Fatal Error */
#ifndef RHINO_CONFIG_PANIC
#define RHINO_CONFIG_PANIC 0
#endif

#if (RHINO_CONFIG_PANIC > 0)
/* If the mcu printf depends on isr */
#ifndef RHINO_CONFIG_PANIC_PRT_INT
#define RHINO_CONFIG_PANIC_PRT_INT 0
#endif

#ifndef RHINO_CONFIG_PANIC_OVERVIEW
#define RHINO_CONFIG_PANIC_OVERVIEW 1
#endif
#endif

#ifndef RHINO_CONFIG_NORMAL_PRT
#define RHINO_CONFIG_NORMAL_PRT 1
#endif

#ifndef RHINO_CONFIG_CPU_USAGE_PERIOD
#define RHINO_CONFIG_CPU_USAGE_PERIOD 0
#endif

#if ((RHINO_CONFIG_TIMER == 0) && (RHINO_CONFIG_CPU_USAGE_PERIOD >= 1))
#error "you need enable RHINO_CONFIG_TIMER as well."
#endif

#if ((RHINO_CONFIG_TASK_SCHED_STATS == 0) && (RHINO_CONFIG_CPU_USAGE_PERIOD >= 1))
#error "you need enable RHINO_CONFIG_TASK_SCHED_STATS as well."
#endif

#endif /* DEBUG_DFTDBG_CONFIG_H */
