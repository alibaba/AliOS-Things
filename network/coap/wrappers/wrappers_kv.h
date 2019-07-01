/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef _WRAPPERS_KV_H_
#define _WRAPPERS_KV_H_

#include "infra_types.h"
#include "infra_defs.h"

int HAL_Kv_Set(const char *key, const void *val, int len, int sync);
int HAL_Kv_Get(const char *key, void *val, int *buffer_len);
int HAL_Kv_Del(const char *key);

#endif

