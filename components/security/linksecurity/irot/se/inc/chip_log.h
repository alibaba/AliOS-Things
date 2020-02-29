/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef __CHIP_LOG_H__
#define __CHIP_LOG_H__

#include "ls_osa.h"
#include "chip_config.h"

#define __TAG__     "ID2=>"

#if defined(CONFIG_CHIP_DEBUG)
#define chip_log_debug(_f, ...)             ls_osa_print("%s "_f, __TAG__, ##__VA_ARGS__)        
#else
#define chip_log_debug(_f, ...)
#endif

#define chip_log_info(_f, ...)              ls_osa_print("%s "_f, __TAG__, ##__VA_ARGS__)
#define chip_log_error(_f, ...)             ls_osa_print("ERROR: %s "_f, __TAG__, ##__VA_ARGS__)

void chip_log_hex_dump(const char* name, const uint8_t* data, uint32_t len);

#endif  /* __CHIP_LOG_H__ */
