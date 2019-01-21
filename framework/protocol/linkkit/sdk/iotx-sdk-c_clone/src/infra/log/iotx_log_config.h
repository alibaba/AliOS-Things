/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef __LITE_LOG_CONFIG_H__
#define __LITE_LOG_CONFIG_H__

#include "iot_import.h"

#define LITE_printf                     HAL_Printf
#define LITE_snprintf                   HAL_Snprintf
#define LITE_vsnprintf                  HAL_Vsnprintf
#define LITE_LOG_ENABLED

#define LOG_MSG_MAXLEN                  (255)
#define LOG_MOD_NAME_LEN                (7)
#define LOG_PREFIX_FMT                  "[%s] %s(%d): "
#define LOG_PREFIX_FMT_ONLINE           "%ld %s %s "

#define HEXDUMP_SEP_LINE                "+" \
    "-----------------------" \
    "-----------------------" \
    "-----------------------"

#if defined(_PLATFORM_IS_LINUX_)
    #undef  LOG_MSG_MAXLEN
    #define LOG_MSG_MAXLEN              (512)
#endif

#endif  /* __LITE_LOG_CONFIG_H__ */
