/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef BE_LOG_H
#define BE_LOG_H

#include <stddef.h>

#if defined(__cplusplus)
extern "C" {
#endif

#define BE_LOG_MAX_MESSAGE 256

enum BE_LOG_LEVEL {
    BE_LOG_LEVEL_DEBUG = (1 << 0),
    BE_LOG_LEVEL_INFO  = (1 << 1),
    BE_LOG_LEVEL_WARN  = (1 << 2),
    BE_LOG_LEVEL_ERROR = (1 << 3),
};

typedef void (*be_log_callback)(const char *tag, int level, const char *msg,
                                int len);
void be_set_log(be_log_callback cb);

void be_log(const char *tag, int level, const char *format, ...);

#define be_debug(tag, fmt, agrs...) be_log(tag, BE_LOG_LEVEL_DEBUG, fmt, ##agrs)
#define be_info(tag, fmt, agrs...) be_log(tag, BE_LOG_LEVEL_INFO, fmt, ##agrs)
#define be_warn(tag, fmt, agrs...) be_log(tag, BE_LOG_LEVEL_WARN, fmt, ##agrs)
#define be_error(tag, fmt, agrs...) be_log(tag, BE_LOG_LEVEL_ERROR, fmt, ##agrs)

#ifndef LOG_NDEBUG
#ifdef _DEBUG
#define LOG_NDEBUG 0
#else
#define LOG_NDEBUG 1
#endif
#endif

#if LOG_NDEBUG
#define debug(fmt, args...) ((void)0)
#else
#define debug(fmt, args...) be_log(NULL, BE_LOG_LEVEL_DEBUG, fmt, ##args)
#endif

#define info(fmt, args...) be_log(NULL, BE_LOG_LEVEL_INFO, fmt, ##args)
#define warn(fmt, args...) be_log(NULL, BE_LOG_LEVEL_WARN, fmt, ##args)
#define error(fmt, args...) be_log(NULL, BE_LOG_LEVEL_ERROR, fmt, ##args)

#if defined(__cplusplus)
}
#endif

#endif /* BE_LOG_H */
