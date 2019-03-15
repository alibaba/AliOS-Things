/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef __LOG_H__
#define __LOG_H__

#include "config.h"
#include "ls_osa.h"

#define irot_pal_log    ls_osa_print

#if ID2_DEBUG
#define id2_log_debug(fmt, args...)		    irot_pal_log(fmt, ##args)
#define id2_log_hex_data(name, in_data, in_len)	    id2_log_hex_dump(name, in_data, in_len)
#else
#define id2_log_debug(fmt, args...)
#define id2_log_hex_data(name, in_data, in_len)
#endif

#define id2_log_error(fmt, args...)	irot_pal_log(fmt, ##args)

void id2_log_hex_dump(const char* name, const uint8_t* in_data, uint32_t in_len);

#endif
