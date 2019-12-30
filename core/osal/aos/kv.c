/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include <stdint.h>

#include "aos/kv.h"
#include "aos/errno.h"

#include "kv_api.h"

static int _kv_to_aos_res(int res)
{
    switch (res) {
        case KV_OK:
            return 0;
        case KV_ERR_NO_SPACE:
            return -ENOSPC;
        case KV_ERR_INVALID_PARAM:
            return -EINVAL;
        case KV_ERR_MALLOC_FAILED:
            return -ENOMEM;
        case KV_ERR_NOT_FOUND:
            return -ENOENT;
        case KV_ERR_FLASH_READ:
        case KV_ERR_FLASH_WRITE:
        case KV_ERR_FLASH_ERASE:
            return -EIO;
        case KV_ERR_NOT_SUPPORT:
            return -ENOSYS;
        default:
            return -EBUSY;
    }
}

int aos_kv_init(void)
{
    return _kv_to_aos_res(kv_init());
}

void aos_kv_deinit(void)
{
    kv_deinit();
}

int aos_kv_set(const char *key, const void *value, int len, int sync)
{
    return _kv_to_aos_res(kv_item_set(key, value, len));
}

int aos_kv_get(const char *key, void *buffer, int *buffer_len)
{
    return _kv_to_aos_res(kv_item_get(key, buffer, buffer_len));
}

int aos_kv_del(const char *key)
{
    return _kv_to_aos_res(kv_item_delete(key));
}

int aos_kv_del_by_prefix(const char *prefix)
{
    return _kv_to_aos_res(kv_item_delete_by_prefix(prefix));
}

int aos_kv_secure_set(const char *key, const void *value, int len, int sync)
{
    return _kv_to_aos_res(kv_item_secure_set(key, value, len));
}

int aos_kv_secure_get(const char *key, void *buffer, int *buffer_len)
{
    return _kv_to_aos_res(kv_item_secure_get(key, buffer, buffer_len));
}

