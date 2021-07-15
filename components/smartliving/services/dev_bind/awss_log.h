/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef __AWSS_LOG_H__
#define __AWSS_LOG_H__
#include "ulog/ulog.h"
#include "iotx_log.h"

// #define awss_flow(...)      log_flow("awss", __VA_ARGS__)
// #define awss_debug(...)     log_debug("awss", __VA_ARGS__)
// #define awss_info(...)      log_info("awss", __VA_ARGS__)
// #define awss_warn(...)      log_warning("awss", __VA_ARGS__)
// #define awss_err(...)       log_err("awss", __VA_ARGS__)
// #define awss_crit(...)      log_crit("awss", __VA_ARGS__)
// #define awss_emerg(...)     log_emerg("awss", __VA_ARGS__)
// #define awss_trace(...)     log_crit("awss", __VA_ARGS__)
#if (CONFIG_BLDTIME_MUTE_DBGLOG)
#define awss_flow(...)
#define awss_info(...)
#define awss_debug(...)
#define awss_warn(...)
#define awss_err(...) 
#define awss_crit(...) 
#define awss_emerg(...) 
#define awss_trace(...)
#else
#define awss_flow(...)      log_flow("awss", __VA_ARGS__)
#define awss_debug(...)     log_debug("awss", __VA_ARGS__)
#define awss_info(...)      log_info("awss",  __VA_ARGS__)
#define awss_warn(...)      log_warning("awss",  __VA_ARGS__)
#define awss_err(...)       log_err("awss",  __VA_ARGS__)
#define awss_crit(...)      log_crit("awss",  __VA_ARGS__)
#define awss_emerg(...)     log_emerg("awss",  __VA_ARGS__)
#define awss_trace(...)     log_debug("awss",  __VA_ARGS__)
#endif
#endif
