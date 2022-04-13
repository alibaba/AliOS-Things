/** @defgroup ota_log_api
 * @{
 *
 * This is an include file of OTA log interface.
 *
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */
#ifndef OTA_LOG_H
#define OTA_LOG_H
#include <stdarg.h>
#include <stdio.h>
#include "ulog/ulog.h"

#ifdef __cplusplus
extern "C" {
#endif

#define OTA_LOG_D(fmt, ...) LOGD("ota",fmt,##__VA_ARGS__)
#define OTA_LOG_I(fmt, ...) LOGI("ota",fmt,##__VA_ARGS__)
#define OTA_LOG_W(fmt, ...) LOGW("ota",fmt,##__VA_ARGS__)
#define OTA_LOG_E(fmt, ...) LOGE("ota",fmt,##__VA_ARGS__)
/**
 * @}
 */
#ifdef __cplusplus
}
#endif
#endif/*OTA_LOG_H*/
