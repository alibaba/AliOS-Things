/*
 * Copyright (C) 2018 Alibaba Group Holding Limited
 */

#ifndef K_PWR_DEBUG_H
#define K_PWR_DEBUG_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdio.h>
#include <stdarg.h>

#include "cpu_pwr_default_config.h"

/*
 * when use debug, do not close uart when turn into pwr down state
 */
#define PWR_DEBUG_LEVEL DBG_INFO

typedef enum {
    DBG_OFF  = 0x00000000,
    DBG_INFO = 0x00000001,
    DBG_WARN = 0x00000002,
    DBG_ERR  = 0x00000003,
} pwr_debug_level_t;

void pwr_debug(pwr_debug_level_t debug_level, const char *fmt_str, ...);

#if (RHINO_CONFIG_CPU_PWR_DEBUG > 0)
#define PWR_DBG pwr_debug
#else
#define PWR_DBG(lvl, ...)
#endif /* RHINO_CONFIG_CPU_PWR_DEBUG > DBG_OFF */

#ifdef __cplusplus
}
#endif

#endif /* K_PWR_DEBUG_H */
