/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */




#ifndef __LITE_LOG_INTERNAL_H__
#define __LITE_LOG_INTERNAL_H__

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "iotx_log_config.h"
#include "iotx_log.h"
#include "iotx_utils.h"
#include "iot_import.h"

typedef struct {
    char            name[LOG_MOD_NAME_LEN + 1];
    int             priority;
    char            text_buf[LOG_MSG_MAXLEN + 1];
} log_client;

#endif  /* __LITE_LOG_INTERNAL_H__ */
