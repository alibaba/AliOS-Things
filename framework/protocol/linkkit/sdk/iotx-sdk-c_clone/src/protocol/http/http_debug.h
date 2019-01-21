/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */




#ifndef __HTTP_DEBUG_H__
#define __HTTP_DEBUG_H__

#include "iotx_log.h"

#define http_emerg(...)    log_emerg("HTTP", __VA_ARGS__)
#define http_crit(...)     log_crit("HTTP", __VA_ARGS__)
#define http_err(...)      log_err("HTTP", __VA_ARGS__)
#define http_warning(...)  log_warning("HTTP", __VA_ARGS__)
#define http_info(...)     log_info("HTTP", __VA_ARGS__)
#define http_debug(...)    log_debug("HTTP", __VA_ARGS__)

#endif  /* __HTTP_DEBUG_H__ */
