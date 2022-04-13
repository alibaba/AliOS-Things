/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef DBG_API_H
#define DBG_API_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

#include "k_api.h"
#include "debug_config.h"
#include "debug_infoget.h"
#include "debug_overview.h"
#include "debug_panic.h"
#include "debug_backtrace.h"
#include "debug_cpuusage.h"
#include "debug_lastword.h"
#include "debug_print.h"
#include "debug_test.h"
#include "debug_trace.h"
#include "debug_cli_cmd.h"
#include "debug_dumpsys.h"
#include "debug_cpuusage.h"

/* system reboot reason description */
#define DEBUG_REBOOT_REASON_WD_RST    0x01 /**< Watchdog reset */
#define DEBUG_REBOOT_REASON_PANIC     0x02 /**< System panic */
#define DEBUG_REBOOT_REASON_REPOWER   0x03 /**< System repower */
#define DEBUG_REBOOT_REASON_FATAL_ERR 0x04 /**< System fatal error */

#ifdef __cplusplus
}
#endif

#endif /* DBG_API_H */
