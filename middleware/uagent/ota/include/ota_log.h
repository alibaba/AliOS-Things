/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef __OTA_LOG_H__
#define __OTA_LOG_H__
#include <stdarg.h>
#include <stdio.h>
#include "ulog/ulog.h"

#define OTA_LOG_D(fmt, ...) LOGD("uota",fmt,##__VA_ARGS__)
#define OTA_LOG_I(fmt, ...) LOGE("uota",fmt,##__VA_ARGS__)
#define OTA_LOG_W(fmt, ...) LOGW("uota",fmt,##__VA_ARGS__)
#define OTA_LOG_E(fmt, ...) LOGE("uota",fmt,##__VA_ARGS__)
#endif  /*__OTA_LOG_H__*/
