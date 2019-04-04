/**
 * Copyright (C) 2017 The YunOS Project. All rights reserved.
 */

#ifndef _PLAT_DBG_H_
#define _PLAT_DBG_H_

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define _DEPRESS_UNUSED_WARNING(_x) do { (_x) = (_x); } while (0)

#define PLAT_TAG "PL_LOG"

#ifndef PLATFORM_ANDROID
#define PL_ERR(_f, _a ...) printf("PL ERR %s %d: "_f, __FUNCTION__, __LINE__, ##_a)
#else
#include <android/log.h>
#define LOG_ERR(...)            __android_log_print(        \
                                        ANDROID_LOG_ERROR,   \
                                        PLAT_TAG,        \
                                        __VA_ARGS__)
#define LOG_INF(...)            __android_log_print(        \
                                        ANDROID_LOG_INFO,   \
                                        PLAT_TAG,        \
                                        __VA_ARGS__)
#define PL_ERR(_f, _a ...) LOG_ERR("ERR %s %d: "_f, __FUNCTION__, __LINE__, ##_a)
#endif /* PLATFORM_ANDROID */

#ifdef CONFIG_PL_DBG
void dump_data(char *name, uint8_t *data, uint32_t len);
#ifndef PLATFORM_ANDROID
#define PL_INF(_f, _a ...) printf("PL ERR %s %d: "_f, __FUNCTION__, __LINE__, ##_a)
#else
#define PL_INF(_f, _a ...) LOG_INF("%s %d: "_f, __FUNCTION__, __LINE__, ##_a)
#endif /* PLATFORM_ANDROID */
#else
#define PL_INF(_f, _a ...)
#endif

#define pl_malloc malloc
#define pl_free   free

#endif /* _PLAT_DBG_H_ */

