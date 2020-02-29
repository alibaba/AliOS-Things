/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <errno.h>
#include "aos/kv.h"
#include "ls_hal.h"

static uint32_t _kv_to_hal_res(int err)
{
    uint32_t ret;

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

uint32_t ls_hal_kv_init(void)
{
    return SST_HAL_SUCCESS;
}

void ls_hal_kv_deinit(void)
{
    return;
}

uint32_t ls_hal_kv_set(const char *key, const void *value, uint32_t len)
{
    uint32_t ret = aos_kv_set(key, value, len, 1);
    return _kv_to_hal_res(ret);
}

uint32_t ls_hal_kv_get(const char *key, void *buffer, uint32_t *buffer_len)
{
    int ret = 0;

    if (!buffer_len) {
        return SST_HAL_ERROR_BAD_PARAMETERS;
    }

    //buffer is NULL *buffer is 0 should return SST_HAL_ERROR_SHORT_BUFFER
    if (!buffer && *buffer_len == 0) {
        //for aos cannot support buffer is NULL
        *buffer_len = 1;
        ret = aos_kv_get(key, buffer_len, (int *)buffer_len);
        if (!ret) {
            *buffer_len = 1;
            return SST_HAL_ERROR_SHORT_BUFFER;
        }
    } else {
        ret = aos_kv_get(key, buffer, (int *)buffer_len);
    }

    if (ret == -ENOSPC && *buffer_len) {
        return SST_HAL_ERROR_SHORT_BUFFER;
    }

    return _kv_to_hal_res(ret);
}

uint32_t ls_hal_kv_del(const char *key)
{
    uint32_t ret = 0;

    if (!key) {
        return SST_HAL_ERROR_BAD_PARAMETERS;
    }

    ret = aos_kv_del(key);

    return _kv_to_hal_res(ret);
}

