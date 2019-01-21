/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef __LITE_UTILS_INTERNAL_H__
#define __LITE_UTILS_INTERNAL_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "iotx_log.h"

#include "iotx_utils_config.h"
#include "iotx_utils.h"

#define utils_emerg(...)    log_emerg("util", __VA_ARGS__)
#define utils_crit(...)     log_crit("util", __VA_ARGS__)
#define utils_err(...)      log_err("util", __VA_ARGS__)
#define utils_warning(...)  log_warning("util", __VA_ARGS__)
#define utils_info(...)     log_info("util", __VA_ARGS__)
#define utils_debug(...)    log_debug("util", __VA_ARGS__)

#endif  /* __LITE_UTILS_INTERNAL_H__ */
