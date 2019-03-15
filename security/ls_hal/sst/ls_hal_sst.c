/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <errno.h>
#include "aos/kv.h"
#include "ls_hal.h"

static int _kv_to_hal_res(int err)
{
    int ret;

    switch (err) {
        case 0:
            ret = SST_HAL_SUCCESS;
            break;
        case -ENOENT:
            ret = SST_HAL_ERROR_ITEM_NOT_FOUND;
            break;
        case -ENOSPC:
            ret = SST_HAL_ERROR_STORAGE_NO_SPACE;
            break;
        case -EINVAL:
            ret = SST_HAL_ERROR_BAD_PARAMETERS;
            break;
        case -ENOMEM:
            ret = SST_HAL_ERROR_OUT_OF_MEMORY;
            break;
        default:
            ret = SST_HAL_ERROR_GENERIC;
            break;
    }

    return ret;
}

int ls_hal_kv_init(void)
{
    int ret = aos_kv_init();
    return _kv_to_hal_res(ret);
}

void ls_hal_kv_deinit(void)
{
    aos_kv_deinit();
}

int ls_hal_kv_set(const char *key, const void *value, int len, int sync)
{
    int ret = aos_kv_set(key, value, len, sync);
    return _kv_to_hal_res(ret);
}

int ls_hal_kv_get(const char *key, void *buffer, int *buffer_len)
{
    int ret = aos_kv_get(key, buffer, buffer_len);
    return _kv_to_hal_res(ret);
}

int ls_hal_kv_del(const char *key)
{
    int ret = 0;

    if (!key) {
        return SST_HAL_ERROR_BAD_PARAMETERS;
    }

    ret = aos_kv_del(key);

    return _kv_to_hal_res(ret);
}

