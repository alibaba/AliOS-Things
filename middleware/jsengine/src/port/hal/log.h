/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef BE_LOG_H
#define BE_LOG_H

#include <stddef.h>
#include "ulog/ulog.h"

#if defined(__cplusplus)
extern "C" {
#endif

enum BE_LOG_LEVEL {
    BE_LOG_LEVEL_DEBUG = (1 << 0),
    BE_LOG_LEVEL_INFO  = (1 << 1),
    BE_LOG_LEVEL_WARN  = (1 << 2),
    BE_LOG_LEVEL_ERROR = (1 << 3),
};

#define jse_debug(fmt, ...) LOGI("JSE",fmt,##__VA_ARGS__)
#define jse_info(fmt, ...) LOGI("JSE",fmt,##__VA_ARGS__)
#define jse_warn(fmt, ...) LOGW("JSE",fmt,##__VA_ARGS__)
#define jse_error(fmt, ...) LOGE("JSE",fmt,##__VA_ARGS__)

#if defined(__cplusplus)
}
#endif

#endif /* BE_LOG_H */
