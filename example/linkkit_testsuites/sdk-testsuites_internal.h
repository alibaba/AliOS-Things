/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */



#ifndef __SDK_TESTSUITES_INTERNAL_H__
#define __SDK_TESTSUITES_INTERNAL_H__
#if defined(__cplusplus)
extern "C" {
#endif

#include "iot_import.h"
#include "iot_export.h"

#include "cut.h"
#include "iotx_log.h"
#include "iotx_utils.h"
#include "ut_devcert.h"

#define ut_emerg(...)    log_emerg("ut", __VA_ARGS__)
#define ut_crit(...)     log_crit("ut", __VA_ARGS__)
#define ut_err(...)      log_err("ut", __VA_ARGS__)
#define ut_warning(...)  log_warning("ut", __VA_ARGS__)
#define ut_info(...)     log_info("ut", __VA_ARGS__)
#define ut_debug(...)    log_debug("ut", __VA_ARGS__)

#if defined(__cplusplus)
}
#endif
#endif  /* __SDK_TESTSUITES_INTERNAL_H__ */