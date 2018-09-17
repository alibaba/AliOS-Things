/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"
#include "tsl_opt_impl.h"
#include "tsl_format_export.h"
#include "cJSON.h"

int tsl_format_convert(const char *src_filename, const char *dst_filename, int is_slim)
{
    return tsl_format_opt_convert(src_filename, dst_filename, is_slim);
}


int tsl_format_dump(const char *filename)
{
    return tsl_format_opt_dump(filename);
}
