/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef _OTA_LOG_H_
#define _OTA_LOG_H_

#include <stdarg.h>
#include <stdio.h>

#if defined(OTA_ALIOS)
#include "aos/log.h"
#if defined(LOG_SIMPLE)
#define OTA_LOG_D(fmt, ...) LOGD("uota","file %s, line %d", __FILE__, __LINE__)
#define OTA_LOG_I(fmt, ...) LOGE("uota",fmt,##__VA_ARGS__)
#define OTA_LOG_W(fmt, ...) LOGW("uota","file %s, line %d", __FILE__, __LINE__)
#define OTA_LOG_E(fmt, ...) LOGE("uota",fmt,##__VA_ARGS__)
#else
#define OTA_LOG_D(fmt, ...) LOGD("uota",fmt,##__VA_ARGS__)
#define OTA_LOG_I(fmt, ...) LOGI("uota",fmt,##__VA_ARGS__)
#define OTA_LOG_W(fmt, ...) LOGW("uota",fmt,##__VA_ARGS__)
#define OTA_LOG_E(fmt, ...) LOGE("uota",fmt,##__VA_ARGS__)
#endif
#elif defined(CSP_LINUXHOST)
#define OTA_LOG_D(fmt, ...) printf("[%s:%d]"fmt"\r\n" ,__FUNCTION__,__LINE__,##__VA_ARGS__)
#define OTA_LOG_I(fmt, ...) printf("[%s:%d]"fmt"\r\n" ,__FUNCTION__,__LINE__,##__VA_ARGS__)
#define OTA_LOG_W(fmt, ...) printf("[%s:%d]"fmt"\r\n" ,__FUNCTION__,__LINE__,##__VA_ARGS__)
#define OTA_LOG_E(fmt, ...) printf("[%s:%d]"fmt"\r\n" ,__FUNCTION__,__LINE__,##__VA_ARGS__)
#elif defined(OTA_WITH_LINKKIT)
#include "iotx_log.h"
#define OTA_LOG_D(...)      log_debug("uota", __VA_ARGS__)
#define OTA_LOG_I(...)      log_info("uota", __VA_ARGS__)
#define OTA_LOG_W(...)      log_warning("uota", __VA_ARGS__)
#define OTA_LOG_E(...)      log_err("uota", __VA_ARGS__)
#else
#define OTA_LOG_D(fmt, ...) printf("[%s:%d]"fmt"\r\n" ,__FUNCTION__,__LINE__,##__VA_ARGS__)
#define OTA_LOG_I(fmt, ...) printf("[%s:%d]"fmt"\r\n" ,__FUNCTION__,__LINE__,##__VA_ARGS__)
#define OTA_LOG_W(fmt, ...) printf("[%s:%d]"fmt"\r\n" ,__FUNCTION__,__LINE__,##__VA_ARGS__)
#define OTA_LOG_E(fmt, ...) printf("[%s:%d]"fmt"\r\n" ,__FUNCTION__,__LINE__,##__VA_ARGS__)
#endif
#endif  // _OTA_LOG_H_
