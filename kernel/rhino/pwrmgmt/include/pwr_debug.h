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

#define DBG_OFF 0x00000000
#define DBG_WARN 0x00000001
#define DBG_ERR 0x00000002
#define DBG_INFO 0x00000004
#define DBG_ALL 0xffffffff

    extern int pwr_debug_level;

#undef PWR_DEBUG_LINE_INFO

#define PWR_DBG(lvl, ...)

#ifdef __cplusplus
}
#endif

#endif /* K_PWR_DEBUG_H */
