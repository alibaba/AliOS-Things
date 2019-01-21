/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */




#ifndef __HTTP_DEBUG_H__
#define __HTTP_DEBUG_H__

#include "iotx_log.h"

#define h2_emerg(...)    log_emerg("h2", __VA_ARGS__)
#define h2_crit(...)     log_crit("h2", __VA_ARGS__)
#define h2_err(...)      log_err("h2", __VA_ARGS__)
#define h2_warning(...)  log_warning("h2", __VA_ARGS__)
#define h2_info(...)     log_info("h2", __VA_ARGS__)
#define h2_debug(...)    log_debug("h2", __VA_ARGS__)

#endif  /* __HTTP_DEBUG_H__ */
