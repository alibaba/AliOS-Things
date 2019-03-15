/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef DEBUG_CONFIG_H
#define DEBUG_CONFIG_H

/* If AliOS task over the Exception/Fatal Error */
#ifndef DEBUG_CONFIG_PANIC
#define DEBUG_CONFIG_PANIC 0
#endif

/* If AliOS task over the backtrace feature */
#ifndef DEBUG_CONFIG_BACKTRACE
#define DEBUG_CONFIG_BACKTRACE 0
#endif

/* If AliOS task over the cpu usage feature */
#ifndef DEBUG_CONFIG_CPU_USAGE_PERIOD
#define DEBUG_CONFIG_CPU_USAGE_PERIOD 0
#endif

#if ((RHINO_CONFIG_TIMER == 0) && (DEBUG_CONFIG_CPU_USAGE_PERIOD >= 1))
#error "you need enable RHINO_CONFIG_TIMER as well."
#endif

#if ((RHINO_CONFIG_SYS_STATS == 0) && (DEBUG_CONFIG_CPU_USAGE_PERIOD >= 1))
#error "you need enable RHINO_CONFIG_SYS_STATS as well."
#endif

#endif /* DEBUG_CONFIG_H */
