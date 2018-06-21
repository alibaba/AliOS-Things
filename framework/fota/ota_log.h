/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef _OTA_LOG_H_
#define _OTA_LOG_H_

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#undef LOG_TAG
#define LOG_TAG "OTA"

#include "aos/aos.h"

#define OTA_LOG_D(...)	OTA_LOG_D_help(__VA_ARGS__, "")
#define OTA_LOG_I(...)	OTA_LOG_I_help(__VA_ARGS__, "")
#define OTA_LOG_W(...)	OTA_LOG_W_help(__VA_ARGS__, "")
#define OTA_LOG_E(...)	OTA_LOG_E_help(__VA_ARGS__, "")

#define OTA_LOG_D_help(format, ...) LOGD(LOG_TAG, format"\r\n",__VA_ARGS__)
#define OTA_LOG_I_help(format, ...) LOGI(LOG_TAG, format"\r\n",__VA_ARGS__)
#define OTA_LOG_W_help(format, ...) LOGW(LOG_TAG, format"\r\n",__VA_ARGS__)
#define OTA_LOG_E_help(format, ...) LOGE(LOG_TAG, format"\r\n",__VA_ARGS__)


#endif  // _OTA_LOG_H_
