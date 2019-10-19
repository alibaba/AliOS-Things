/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef __RWS_STRING_H__
#define __RWS_STRING_H__ 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rws_common.h"

#if defined(_MSC_VER)
#define rws_sprintf(s,l,f,...) sprintf_s(s,l,f,__VA_ARGS__)
#define rws_sscanf(s,f,...) sscanf_s(s,f,__VA_ARGS__)
#define rws_strerror(e) strerror(e)
#else
#define rws_sprintf(s,l,f,...) sprintf(s,f,__VA_ARGS__)
#define rws_sscanf(s,f,...) sscanf(s,f,__VA_ARGS__)
#define rws_strerror(e) strerror(e)
#endif

char * rws_string_copy(const char * str);

char * rws_string_copy_len(const char * str, const size_t len);

void rws_string_delete(char * str);

void rws_string_delete_clean(char ** str);

#endif
