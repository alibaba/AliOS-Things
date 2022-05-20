/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef AOS_LOG_H
#define AOS_LOG_H

#include <stdarg.h>
#include <string.h>

typedef enum {
    AOS_LOG_NONE,   /* disable log */
    AOS_LOG_FATAL,  /* fatal log will output */
    AOS_LOG_ERROR,  /* fatal + error log will output */
    AOS_LOG_WARN,   /* fatal + warn + error log will output(default level) */
    AOS_LOG_INFO,   /* info + warn + error log will output */
    AOS_LOG_DEBUG,  /* debug + info + warn + error + fatal log will output */
    AOS_LOG_VERBOSE,
} haas_log_level_t;

/**
 * Function prototype for syncronized log text, Recommed using below brief API LOGX instead of this.
 *
 * @param[in]  s         Serverity of Log
 * @param[in]  tag       Usually File name
 * @param[in]  fmt, ...  Variable Parameter, support format print to log
 *
 * @return  0 on success, negative error on failure.
 */
int  aos_log(const unsigned char s, const char *tag, const char *fmt, ...);


/**
 * Set the log level.
 *
 * @param[in]  log_level  level to be set,must be one of AOS_LL_NONE,AOS_LL_FATAL,
 * AOS_LL_ERROR,AOS_LL_WARN,AOS_LL_INFO or AOS_LL_DEBUG.
 *
 * @return  0 on success, negative error on failure.
 */
int  aos_log_set_level(haas_log_level_t log_level);


#define AOS_LOGF(tag, ...)  aos_log(AOS_LOG_FATAL, tag, __VA_ARGS__)

#define AOS_LOGE(tag, ...)  aos_log(AOS_LOG_ERROR, tag, __VA_ARGS__)

#define AOS_LOGW(tag, ...)  aos_log(AOS_LOG_WARN, tag, __VA_ARGS__)

#define AOS_LOGI(tag, ...)  aos_log(AOS_LOG_INFO, tag, __VA_ARGS__)

#define AOS_LOGD(tag, ...)  aos_log(AOS_LOG_DEBUG, tag, __VA_ARGS__)

#define AOS_LOGV(tag, ...)  aos_log(AOS_LOG_VERBOSE, tag, __VA_ARGS__)


#endif /* AOS_LOG_H */

