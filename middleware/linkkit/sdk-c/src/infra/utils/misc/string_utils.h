/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */




#ifndef __COMMON_UTILS_H__
#define __COMMON_UTILS_H__

#include "iotx_utils_internal.h"


int LITE_get_randstr(_OU_ char *random, _IN_ int length);

int8_t utils_hb2hex(uint8_t hb);

#endif  /* __COMMON_UTILS_H__ */

