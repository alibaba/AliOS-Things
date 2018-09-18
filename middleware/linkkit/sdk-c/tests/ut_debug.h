/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */



#ifndef __UT_DEBUG_H__
#define __UT_DEBUG_H__

#include "iotx_log.h"

#define ut_emerg(...)    log_emerg("ut", __VA_ARGS__)
#define ut_crit(...)     log_crit("ut", __VA_ARGS__)
#define ut_err(...)      log_err("ut", __VA_ARGS__)
#define ut_warning(...)  log_warning("ut", __VA_ARGS__)
#define ut_info(...)     log_info("ut", __VA_ARGS__)
#define ut_debug(...)    log_debug("ut", __VA_ARGS__)

#endif  /* __UT_DEBUG_H__ */
