/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */



#ifndef TSL_OPT_IMPL_H
#define TSL_OPT_IMPL_H

#ifdef __cplusplus
extern "C" {
#endif

int tsl_format_opt_convert(const char *src_filename, const char *dst_filename, int is_slim);

int tsl_format_opt_dump(const char *filename);

#ifdef __cplusplus
}
#endif

#endif
