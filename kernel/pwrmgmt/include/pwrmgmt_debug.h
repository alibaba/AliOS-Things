/*
 * Copyright (C) 2018 Alibaba Group Holding Limited
 */

#ifndef PWRMGMT_DEBUG_H
#define PWRMGMT_DEBUG_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdio.h>
#include <stdarg.h>

#include <k_api.h>

#include "pwrmgmt_default_config.h"

/*
 * when use debug, do not close uart when turn into pwr down state
 */
#define PWRMGMT_LOG_LEVEL PWRMGMT_LOG_INFO

typedef enum {
    PWRMGMT_LOG_OFF  = 0,
    PWRMGMT_LOG_DBG  = 1,
    PWRMGMT_LOG_INFO = 2,
    PWRMGMT_LOG_WARN = 3,
    PWRMGMT_LOG_ERR  = 4,
} pwrmgmt_log_level_t;

void pwrmgmt_log(pwrmgmt_log_level_t debug_level, const char *fmt_str, ...);

#if (PWRMGMT_CONFIG_DEBUG > 0)
#define PWRMGMT_LOG pwrmgmt_log
#else
#define PWRMGMT_LOG(lvl, ...)
#endif /* PWRMGMT_CONFIG_DEBUG > DBG_OFF */

#if (PWRMGMT_CONFIG_SHOW > 0)
void cpu_pwr_state_show(void);
void cpu_pwr_info_show(void);
#endif /* PWRMGMT_CONFIG_SHOW */

#ifdef __cplusplus
}
#endif

#endif /* PWRMGMT_DEBUG_H */
