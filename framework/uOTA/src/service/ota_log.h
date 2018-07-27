/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef _OTA_LOG_H_
#define _OTA_LOG_H_

#include <stdarg.h>
#include <stdio.h>
#include "iotx_log.h"

#ifndef _IS_LINKKIT_
#define OTA_LOG_D(format, ...) printf(format"\r\n",##__VA_ARGS__)
#define OTA_LOG_I(format, ...) printf(format"\r\n",##__VA_ARGS__)
#define OTA_LOG_W(format, ...) printf(format"\r\n",##__VA_ARGS__)
#define OTA_LOG_E(format, ...) printf(format"\r\n",##__VA_ARGS__)
#else
#define OTA_LOG_D(...)          log_debug("ota", __VA_ARGS__) 
#define OTA_LOG_I(...)          log_info("ota", __VA_ARGS__)
#define OTA_LOG_W(...)          log_warning("ota", __VA_ARGS__)
#define OTA_LOG_E(...)          log_err("ota", __VA_ARGS__)
#endif

#define OTA_LOG_EMERG(...)      log_emerg("ota", __VA_ARGS__)
#define OTA_LOG_CRIT(...)       log_crit("ota", __VA_ARGS__)
#define OTA_LOG_ERROR(...)      log_err("ota", __VA_ARGS__)
#define OTA_LOG_WRN(...)        log_warning("ota", __VA_ARGS__)
#define OTA_LOG_INFO(...)       log_info("ota", __VA_ARGS__)
#define OTA_LOG_DEBUG(...)      log_debug("ota", __VA_ARGS__)
#endif  // _OTA_LOG_H_
