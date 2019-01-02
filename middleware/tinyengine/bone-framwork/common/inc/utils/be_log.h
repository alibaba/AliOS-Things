/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef _be_log_h
#define _be_log_h

#if defined(__cplusplus)
 extern "C" {
#endif

#include <stddef.h>

#define BE_LOG_MAX_MESSAGE    512

enum BE_LOG_LEVEL {
    BE_LOG_LEVEL_DEBUG = (1 << 0),
    BE_LOG_LEVEL_INFO  = (1 << 1),
    BE_LOG_LEVEL_WARN  = (1 << 2),
    BE_LOG_LEVEL_ERROR = (1 << 3),
};

#define be_debug(tag, fmt, agrs...) be_log(__FILE__, tag, BE_LOG_LEVEL_DEBUG, __LINE__, __func__, fmt, ##agrs)
#define be_info(tag, fmt, agrs...)  be_log(__FILE__, tag, BE_LOG_LEVEL_INFO,  __LINE__, __func__, fmt, ##agrs)
#define be_warn(tag, fmt, agrs...)  be_log(__FILE__, tag, BE_LOG_LEVEL_WARN,  __LINE__, __func__, fmt, ##agrs)
#define be_error(tag, fmt, agrs...) be_log(__FILE__, tag, BE_LOG_LEVEL_ERROR, __LINE__, __func__, fmt, ##agrs)

typedef void (*be_log_callback)(const char* file, const char* tag,
                                    int level, int line, const char* func,
                                    const char* format, ...);

void be_log(const char* file, const char* tag, int level, int line, const char* func,
                const char* format, ...);

void be_log_build(const char* file, const char* tag, int level, int line, const char* func,
                      const char* message, char* msg, size_t msg_len);

void be_set_log(be_log_callback cb);

#if defined(__cplusplus)
}
#endif
#endif /* be_log_h */
