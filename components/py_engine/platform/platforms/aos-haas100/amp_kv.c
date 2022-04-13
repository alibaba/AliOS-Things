/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include "amp_kv.h"
#include "kv_api.h"

int HAL_Kv_Set(const char *key, const void *val, int len, int sync)
{
    int   ret = 0;
    ret = aos_kv_set(key, val, len, sync);
    return ret;
}

int HAL_Kv_Get(const char *key, void *buffer, int *buffer_len)
{
    int   ret = 0;
    ret = aos_kv_get(key, buffer, buffer_len);
    return ret;
}

int HAL_Kv_Del(const char *key)
{
    int   ret = 0;
    ret = aos_kv_del(key);
    return ret;
}

int HAL_Kv_Init()
{
    int   ret = 0;
    ret = aos_kv_init();
    return ret;
}