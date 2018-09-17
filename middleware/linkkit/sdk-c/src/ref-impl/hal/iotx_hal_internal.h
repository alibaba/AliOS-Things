/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */




#ifndef __PLATFORM_DEBUG_H__
#define __PLATFORM_DEBUG_H__

#include "iotx_log.h"

#define hal_emerg(...)    log_emerg("hal", __VA_ARGS__)
#define hal_crit(...)     log_crit("hal", __VA_ARGS__)
#define hal_err(...)      log_err("hal", __VA_ARGS__)
#define hal_warning(...)  log_warning("hal", __VA_ARGS__)
#define hal_info(...)     log_info("hal", __VA_ARGS__)
#define hal_debug(...)    log_debug("hal", __VA_ARGS__)

#endif  /* __PLATFORM_DEBUG_H__ */
