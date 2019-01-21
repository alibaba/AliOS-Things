/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */




#ifndef __SUBDEV_DEBUG_H__
#define __SUBDEV_DEBUG_H__

#include "iotx_log.h"

#define subdev_emerg(...)    log_emerg("subdev", __VA_ARGS__)
#define subdev_crit(...)     log_crit("subdev", __VA_ARGS__)
#define subdev_err(...)      log_err("subdev", __VA_ARGS__)
#define subdev_warning(...)  log_warning("subdev", __VA_ARGS__)
#define subdev_info(...)     log_info("subdev", __VA_ARGS__)
#define subdev_debug(...)    log_debug("subdev", __VA_ARGS__)

#endif  /* __SUBDEV_DEBUG_H__ */
