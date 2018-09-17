/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */



#ifndef TSL_FORMAT_EXPORT_H
#define TSL_FORMAT_EXPORT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "common.h"

int tsl_format_convert(const char *src_filename, const char *dst_filename, int is_slim);

int tsl_format_dump(const char *filename);

#ifdef __cplusplus
}
#endif
#endif
