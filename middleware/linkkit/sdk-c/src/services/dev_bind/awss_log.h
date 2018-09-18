/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef _AWSS_LOG_H_
#define _AWSS_LOG_H_

#include "iotx_log.h"

#if 0
#define awss_debug(fmt, args...) log_debug(fmt, ##args)
#define awss_info(fmt, args...)  log_info(fmt, ##args)
#define awss_warn(fmt, args...)  log_warning(fmt, ##args)
#define awss_err(fmt, args...)   log_err(fmt, ##args)
#define awss_crit(fmt, args...)  log_crit(fmt, ##args)
#define awss_emerg(fmt, args...) log_emerg(fmt, ##args)
#define awss_trace(fmt, args...) log_err(fmt, ##args)
#else
#define awss_debug(...) log_debug("awss", __VA_ARGS__)
#define awss_info(...)  log_info("awss", __VA_ARGS__)
#define awss_warn(...)  log_warning("awss", __VA_ARGS__)
#define awss_err(...)   log_err("awss", __VA_ARGS__)
#define awss_crit(...)  log_crit("awss", __VA_ARGS__)
#define awss_emerg(...) log_emerg("awss", __VA_ARGS__)
#define awss_trace(...) log_crit("awss", __VA_ARGS__)
#endif

#endif
