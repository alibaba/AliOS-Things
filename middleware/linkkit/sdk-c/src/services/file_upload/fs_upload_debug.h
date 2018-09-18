/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */




#ifndef __FS_UPLOAD_DEBUG_H__
#define __FS_UPLOAD_DEBUG_H__

#include "iotx_log.h"

#define fsupload_emerg(...)    log_emerg("fsup", __VA_ARGS__)
#define fsupload_crit(...)     log_crit("fsup", __VA_ARGS__)
#define fsupload_err(...)      log_err("fsup", __VA_ARGS__)
#define fsupload_warning(...)  log_warning("fsup", __VA_ARGS__)
#define fsupload_info(...)     log_info("fsup", __VA_ARGS__)
#define fsupload_debug(...)    log_debug("fsup", __VA_ARGS__)

#endif  /* __FS_UPLOAD_DEBUG_H__ */
