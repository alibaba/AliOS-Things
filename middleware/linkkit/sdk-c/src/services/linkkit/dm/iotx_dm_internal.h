/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */



#ifndef _IOTX_DM_INTERNAL_H_
#define _IOTX_DM_INTERNAL_H_

#ifndef _IN_
#define _IN_
#endif

#ifndef _OU_
#define _OU_
#endif

#ifndef DM_READ_ONLY
#define DM_READ_ONLY
#endif

#include "iot_import.h"
#include "iotx_utils.h"
#include "lite-list.h"
#include "lite-cjson.h"
#include "dm_utils.h"
#include "iotx_dm.h"

#define DM_SUPPORT_MEMORY_MAGIC
#ifdef DM_SUPPORT_MEMORY_MAGIC
#define DM_malloc(size) LITE_malloc(size, MEM_MAGIC, "DM")
#else
#define DM_malloc(size) LITE_malloc(size)
#endif
#define DM_free(ptr)   {LITE_free(ptr);ptr = NULL;}

#define dm_log_emerg(...)     log_emerg("DM", __VA_ARGS__)
#define dm_log_crit(...)      log_crit("DM", __VA_ARGS__)
#define dm_log_err(...)       log_err("DM", __VA_ARGS__)
#define dm_log_warning(...)   log_warning("DM", __VA_ARGS__)
#define dm_log_info(...)      log_info("DM", __VA_ARGS__)
#define dm_log_debug(...)     log_debug("DM", __VA_ARGS__)

#endif
