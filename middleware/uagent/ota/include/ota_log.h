/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef OTA_LOG_H_
#define OTA_LOG_H_
#include <stdarg.h>
#include <stdio.h>
#include "ulog/ulog.h"

#define OTA_LOG_D(fmt, ...) LOGD("uota",fmt,##__VA_ARGS__)
#define OTA_LOG_I(fmt, ...) LOGI("uota",fmt,##__VA_ARGS__)
#define OTA_LOG_W(fmt, ...) LOGW("uota",fmt,##__VA_ARGS__)
#define OTA_LOG_E(fmt, ...) LOGE("uota",fmt,##__VA_ARGS__)
#endif  /*OTA_LOG_H_*/
