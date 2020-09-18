/**
 * Copyright (C) 2018 Alibaba.inc, All rights reserved.
 *
 * @file AGLog.h
 * @brief log interface
 * @author lifen.lf@alibaba-inc.com
 * @data 2018/04/07
 * @version 1.0
 */

#ifndef AG_LOG_H
#define AG_LOG_H

#if defined(__ANDROID__) && !defined(__ALIOS__)
#include <android/log.h>
#endif
#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#ifndef _AG_LogPriority_
#define _AG_LogPriority_
enum AG_LogPriority {
    AG_LOG_UNKNOWN = 0,
    AG_LOG_DEBUG,
    AG_LOG_WARN,
    AG_LOG_ERROR,
};
#endif

int __ag_log_print(int prio, const char *tag,  const char *fmt, ...);
#ifndef RTOS_TRACE
#ifdef ALI_MCU_DUMP_PCM_ENABLE
#define RTOS_TRACE
#else
#define RTOS_TRACE printf("@@%s, %d\n", __func__, __LINE__);
#endif
#endif

#ifdef _T_HEAD_
#include <yoc_config.h>
#include <aos/aos.h>
#define TAG "aligenie"

#define AGLOGD(format, ...) LOGD(TAG, format, ##__VA_ARGS__)
#define AGLOGW(format, ...) LOGW(TAG, format, ##__VA_ARGS__)
#define AGLOGE(format, ...) LOGE(TAG, format, ##__VA_ARGS__)
#endif

#ifndef AGLOG
#ifdef ALI_MCU_DUMP_PCM_ENABLE
#define AGLOG(priority, tag, ...)
#else
#define AGLOG(priority, tag, ...) \
    printf(__VA_ARGS__)
    //__ag_log_print(AG_##priority, tag, __VA_ARGS__)
#endif
#endif

#ifndef AGLOGD
#define AGLOGD(...) AGLOG(LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#endif

#ifndef AGLOGW
#define AGLOGW(...) AGLOG(LOG_WARN, LOG_TAG, __VA_ARGS__)
#endif

#ifndef AGLOGE
#define AGLOGE(...) AGLOG(LOG_ERROR, LOG_TAG, __VA_ARGS__)
#endif

#define __fopen(a, b) fopen(a, b);AGLOGE("@@%s, %d, %s\n", __func__, __LINE__, a); 

#ifdef __cplusplus
}
#endif

// for c files include AGLog.h
#ifdef __cplusplus
class AGLog
{
public:
    virtual ~AGLog(){}
    /**
     * @brief Write log to log system
     * @param[in] prio Log priority
     * @param[in] tag Log tag
     * @param[in] text Log content
     * @return int 0 : success; other: fail
     */
    virtual int write(int prio, const char *tag, const char *text)=0;
};
#endif

#endif

