/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef _SST_DBG_H_
#define _SST_DBG_H_

#include "stdint.h"
#include "stdio.h"
#include "sst_osa.h"

#define _DEPRESS_UNUSED_WARNING(_x) do { (_x) = (_x); } while (0)

#define SST_TAG "SST"

#ifndef PLATFORM_ANDROID
#define SST_ERR(_f, _a ...) ls_osa_print("SST %s %d: "_f, __FUNCTION__, __LINE__, ##_a)

#define SST_VER(_f, _a ...) ls_osa_print("SST %s %d: "_f, __FUNCTION__, __LINE__, ##_a)
#else
#include <android/log.h>
#define LOG_ERR(...)            __android_log_print(        \
                                        ANDROID_LOG_ERROR,   \
                                        SST_TAG,        \
                                        __VA_ARGS__)
#define LOG_INF(...)            __android_log_print(        \
                                        ANDROID_LOG_INFO,   \
                                        SST_TAG,        \
                                        __VA_ARGS__)
#define SST_ERR(_f, _a ...) LOG_ERR("ERR %s %d: "_f, __FUNCTION__, __LINE__, ##_a)
#define SST_VER(_f, _a ...) LOG_INF(_f, ##_a)
#endif /* PLATFORM_ANDROID */

#ifdef CONFIG_SST_DBG
#ifndef PLATFORM_ANDROID
#define SST_INF(_f, _a ...) sst_printf("SST %s %d: "_f, __FUNCTION__, __LINE__, ##_a)
#else
#define SST_INF(_f, _a ...) LOG_INF("%s %d: "_f, __FUNCTION__, __LINE__, ##_a)
#endif /* PLATFORM_ANDROID */
#else
#define SST_INF(_f, _a ...)
#endif

#endif /* _SST_DBG_H_ */

