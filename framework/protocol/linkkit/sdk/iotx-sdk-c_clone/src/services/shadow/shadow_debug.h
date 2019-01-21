/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */




#ifndef __SHAD_DEBUG_H__
#define __SHAD_DEBUG_H__

#include "iotx_log.h"

#define shadow_emerg(...)    log_emerg("shadow", __VA_ARGS__)
#define shadow_crit(...)     log_crit("shadow", __VA_ARGS__)
#define shadow_err(...)      log_err("shadow", __VA_ARGS__)
#define shadow_warning(...)  log_warning("shadow", __VA_ARGS__)
#define shadow_info(...)     log_info("shadow", __VA_ARGS__)
#define shadow_debug(...)    log_debug("shadow", __VA_ARGS__)

#endif  /* __SHAD_DEBUG_H__ */
