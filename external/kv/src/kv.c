/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include "kv.h"

#include <stdint.h>
#include "lwip/errno.h"

int aos_kv_init(void)
{
    return 0;
}

void aos_kv_deinit(void)
{
}

int aos_kv_set(const char *key, const void *value, int len, int sync)
{
    return 0;
}

int aos_kv_get(const char *key, void *buffer, int *buffer_len)
{
    return 0;
}

int aos_kv_del(const char *key)
{
    return 0;
}

int aos_kv_del_by_prefix(const char *prefix)
{
    return 0;
}

int aos_kv_secure_set(const char *key, const void *value, int len, int sync)
{
    return 0;
}

int aos_kv_secure_get(const char *key, void *buffer, int *buffer_len)
{
    return 0;
}
