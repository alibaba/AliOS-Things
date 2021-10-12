/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef __AWSS_LOG_H__
#define __AWSS_LOG_H__

#include "linkkit/infra/infra_log.h"

#define awss_flow(...)
#define awss_debug(...) printf(__VA_ARGS__)
#define awss_info(...)  printf(__VA_ARGS__)
#define awss_warn(...)  printf(__VA_ARGS__)
#define awss_err(...)   printf(__VA_ARGS__)
#define awss_crit(...)  printf(__VA_ARGS__)
#define awss_emerg(...) printf(__VA_ARGS__)
#define awss_trace(...) printf(__VA_ARGS__)

#endif
