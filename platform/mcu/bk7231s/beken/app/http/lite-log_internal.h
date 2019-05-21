/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef __LITE_LOG_INTERNAL_H__
#define __LITE_LOG_INTERNAL_H__

#include <stdarg.h>
#include "lite-log.h"


//#define LITE_LOG_ENABLED

#define LOG_MSG_MAXLEN                  (255)
#define LOG_MOD_NAME_LEN                (7)
#define LOG_PREFIX_FMT                  "[%s] %s(%d): "
#define HEXDUMP_SEP_LINE                "+" \
    "-----------------------" \
    "-----------------------" \
    "-----------------------"

#if defined(_PLATFORM_IS_LINUX_)
#undef  LOG_MSG_MAXLEN
#define LOG_MSG_MAXLEN                  (1023)
#endif

typedef struct {
    char            name[LOG_MOD_NAME_LEN + 1];
    int             priority;
    char            text_buf[LOG_MSG_MAXLEN + 1];
} log_client;

#endif  /* __LITE_LOG_INTERNAL_H__ */
