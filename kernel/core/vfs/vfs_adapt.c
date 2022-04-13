/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdint.h>

#include "k_api.h"

#include "vfs_adapt.h"
#include "aos/kernel.h"

void *vfs_lock_create(void)
{
    int32_t   ret;
    aos_hdl_t *m;

    m = aos_malloc(sizeof(aos_hdl_t));
    if (m == NULL) {
        return NULL;
    }

    ret = aos_mutex_new(m);
    if (ret != RHINO_SUCCESS) {
        aos_free(m);
        return NULL;
    }

    return (void *)m;
}

int32_t vfs_lock_free(void *lock)
{
    aos_hdl_t *m = (aos_hdl_t *)lock;

    if (m == NULL) {
        return -1;
    }

    aos_mutex_free(m);
    aos_free(m);

    return 0;
}

int32_t vfs_lock(void *lock)
{
    return aos_mutex_lock((aos_hdl_t *)lock, RHINO_WAIT_FOREVER);
}

int32_t vfs_unlock(void *lock)
{
    return aos_mutex_unlock((aos_hdl_t *)lock);
}

void *vfs_malloc(uint32_t size)
{
    return aos_malloc(size);
}

void vfs_free(void *ptr)
{
    aos_free(ptr);
}
