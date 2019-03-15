/*
 * Copyright (c) 2008-2016 Allwinner Technology Co. Ltd.
 * All rights reserved.
 *
 * File : cdx_log.h
 * Description : Log
 * History :
 *
 */

#ifndef CDX_LOG_H
#define CDX_LOG_H

#ifndef LOG_TAG
#define LOG_TAG "awplayer"
#endif

#include "cdx_malloc_dbg.h"

#define CDX_DEBUG 1

/* temp define to develop */
#define VIDEO_SUPPORT 0		//might be needed in the camera application. but it's not realized.
#define SUBTITLE_SUPPORT 0	//almost impossible
#define ONLINE_SUPPORT 0	//should be change to cache support
#define FD_SUPPORT 0		//might not support in future
#define ID3_IOT_IMPLEMENT 1	//revise for IOT.
#define LIVEMODE_VIDEO 0	//almost impossible
#define SOUNDCTRL_NULL 0	//only for test
#define CAPTURECTRL_NULL 0	//only for test
#define CDX_IOT_AWPOOL 0	//revise for IOT, but can be used for revise memory allocating.
#define CDX_IOT_OLD_SOCKET 0	//revise for IOT, 0: connect every time.
#define CDX_IOT_DNS_CACHE 0	//revise for IOT, 0: using Lwip Dns. 1: using Cedarx Dns.
#define CDX_IOT_CMCC_LOG 0	//log string callback to user.
#define CDX_LWIP_SELECT_ERR 0	//Lwip not support error teller function.
#define SET_SPEED_SUPPORT 0
#define SECURE_BUFFER_SUPPORT 0

#ifndef DEF_CDX_LOG_LEVEL_TYPE
#define DEF_CDX_LOG_LEVEL_TYPE
enum CDX_LOG_LEVEL_TYPE {
    LOG_LEVEL_VERBOSE = 2,
    LOG_LEVEL_DEBUG = 3,
    LOG_LEVEL_INFO = 4,
    LOG_LEVEL_WARNING = 5,
    LOG_LEVEL_ERROR = 6,
};
#endif

#include <stdarg.h>
int printf_lock_init();
int wrap_printf(const char *fmt, ...);
int printf_lock_deinit();

int log_file_reset(const char *path);
int log_file(const char *path, unsigned char *buf, unsigned int len);

void __printf_time(const char *f, unsigned int l);
#define printf_time() __printf_time(__func__, __LINE__)

#define printf wrap_printf

extern enum CDX_LOG_LEVEL_TYPE GLOBAL_LOG_LEVEL;

#ifndef CONFIG_LOG_LEVEL
#define CONFIG_LOG_LEVEL    (0xFFFF)
#endif

#ifdef __ANDROID__
#include <cutils/log.h>

#define CDX_LOG_ORDER \
    ((unsigned)LOG_LEVEL_ERROR   ==  (unsigned)ANDROID_LOG_ERROR) && \
    ((unsigned)LOG_LEVEL_WARNING ==  (unsigned)ANDROID_LOG_WARN) && \
    ((unsigned)LOG_LEVEL_INFO    ==  (unsigned)ANDROID_LOG_INFO) && \
    ((unsigned)LOG_LEVEL_DEBUG   ==  (unsigned)ANDROID_LOG_DEBUG) && \
    ((unsigned)LOG_LEVEL_VERBOSE ==  (unsigned)ANDROID_LOG_VERBOSE)

typedef char CHECK_LOG_LEVEL_EQUAL_TO_ANDROID[CDX_LOG_ORDER > 0 ? 1 : -1];

#define AWLOG(level, fmt, arg...)  \
    do { \
        if (level >= GLOBAL_LOG_LEVEL || level >= CONFIG_LOG_LEVEL) \
            LOG_PRI(level, LOG_TAG, "<%s:%u>: " fmt, __FUNCTION__, __LINE__, ##arg); \
    } while (0)

#define CDX_TRACE() \
    CDX_LOGI("<%s:%u> tid(%d)", __FUNCTION__, __LINE__, gettid())

/*check when realease version*/
#define CDX_FORCE_CHECK(e) \
        LOG_ALWAYS_FATAL_IF(                        \
                !(e),                               \
                "<%s:%d>CDX_CHECK(%s) failed.",     \
                __FUNCTION__, __LINE__, #e)

#define CDX_TRESPASS() \
        LOG_ALWAYS_FATAL("Should not be here.")

#define CDX_LOG_FATAL(fmt, arg...)                          \
        LOG_ALWAYS_FATAL("<%s:%d>" fmt,                      \
            __FUNCTION__, __LINE__, ##arg)

#define CDX_LOG_CHECK(e, fmt, arg...)                           \
    LOG_ALWAYS_FATAL_IF(                                        \
            !(e),                                               \
            "<%s:%d>check (%s) failed:" fmt,                    \
            __FUNCTION__, __LINE__, #e, ##arg)

#ifdef AWP_DEBUG
#define CDX_CHECK(e)                                            \
    LOG_ALWAYS_FATAL_IF(                                        \
            !(e),                                               \
            "<%s:%d>CDX_CHECK(%s) failed.",                     \
            __FUNCTION__, __LINE__, #e)

#else
#define CDX_CHECK(e)
#endif

#else

#include <stdio.h>
#include <string.h>
//#include <assert.h>
#include "unistd.h"

extern const char *CDX_LOG_LEVEL_NAME[];

#if CDX_DEBUG
#define AWLOG(level, fmt, arg...)  \
    do { \
        if ((level >= GLOBAL_LOG_LEVEL || level >= CONFIG_LOG_LEVEL) && \
                level <= LOG_LEVEL_ERROR) \
            printf("%s: %s <%s:%u>: " fmt "\n", \
                    CDX_LOG_LEVEL_NAME[level], LOG_TAG, __FUNCTION__, __LINE__, ##arg); \
    } while (0)
#else
#define AWLOG(level, fmt, arg...)
#endif

#define CDX_TRESPASS()

#define CDX_FORCE_CHECK(e) CDX_CHECK(e)

#define CDX_LOG_CHECK(e, fmt, arg...)                           \
    do {                                                        \
        if (!(e))                                               \
        {                                                       \
            CDX_LOGE("check (%s) failed:"fmt, #e, ##arg);       \
        }                                                       \
    } while (0)

#ifdef AWP_DEBUG
#define CDX_CHECK(e)                                            \
    do {                                                        \
        if (!(e))                                               \
        {                                                       \
            CDX_LOGE("check (%s) failed.", #e);                 \
            assert(0);                                          \
        }                                                       \
    } while (0)
#else
#define CDX_CHECK(e) (void)(e)
#endif

#endif

#define CDX_LOGV(fmt, arg...) logv(fmt, ##arg)
#define CDX_LOGD(fmt, arg...) logd(fmt, ##arg)
#define CDX_LOGI(fmt, arg...) logi(fmt, ##arg)
#define CDX_LOGW(fmt, arg...) logw(fmt, ##arg)
#define CDX_LOGE(fmt, arg...) loge(fmt, ##arg)

#if CDX_DEBUG
#define CDX_BUF_DUMP(buf, len) \
    do { \
        char *_buf = (char *)buf;\
        char str[1024] = {0};\
        unsigned int index = 0, _len;\
        _len = (unsigned int)len;\
        snprintf(str, 1024, ":%d:[", _len);\
        for (index = 0; index < _len; index++)\
        {\
            snprintf(str + strlen(str), 1024 - strlen(str), "%02hhx ", _buf[index]);\
        }\
        str[strlen(str) - 1] = ']';\
        CDX_LOGD("%s", str);\
    }while (0)
#else
#define CDX_BUF_DUMP(buf, len)
#endif

#define CDX_ITF_CHECK(base, itf)    \
    CDX_CHECK(base);                \
    CDX_CHECK(base->ops);           \
    CDX_CHECK(base->ops->itf)

#define CDX_UNUSE(param) (void)param
#define CEDARX_UNUSE(param) (void)param

#define logd(fmt, arg...) //AWLOG(LOG_LEVEL_DEBUG, fmt, ##arg)
#define loge(fmt, arg...) AWLOG(LOG_LEVEL_ERROR, "\033[40;31m" fmt "\033[0m", ##arg)
#define logw(fmt, arg...) //AWLOG(LOG_LEVEL_WARNING, fmt, ##arg)
#define logi(fmt, arg...) //AWLOG(LOG_LEVEL_INFO, fmt, ##arg)
#define logv(fmt, arg...) //AWLOG(LOG_LEVEL_VERBOSE, fmt, ##arg)

#define CDX_ENTRY() //PRINTF("[%s entry] line %d\n", __func__, __LINE__)
#define CDX_EXIT(ret) //PRINTF("[%s exit] line %d, return %d\n", __func__, __LINE__, (int)ret)

#define CDX_FAILED() //PRINTF("[%s failed] line %d\n", __func__, __LINE__)


#ifdef __cplusplus
extern "C"
{
#endif

void log_set_level(unsigned level);

#ifdef __cplusplus
}
#endif

#endif
