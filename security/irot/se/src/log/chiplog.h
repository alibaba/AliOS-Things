/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef __CHIP_LOG_H__
#define __CHIP_LOG_H__

#include "chip_config.h"

void chip_log(const char *fmt, ...);

void chip_log_hex_dump(const char *name, const uint8_t *in_data,
                       uint32_t in_len);

#if (CHIP_DEBUG)
#define chip_log_debug(fmt, args...) chip_log(fmt, ##args)
#define chip_log_hex_data(name, in_data, in_len) \
    chip_log_hex_dump(name, in_data, in_len)
#else
#define chip_log_debug(fmt, args...)
#define chip_log_hex_data(name, in_data, in_len)
#endif

#define chip_log_error(fmt, args...) chip_log(fmt, ##args)

#endif
