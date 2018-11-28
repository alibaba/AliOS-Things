/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdint.h>

#include "k_api.h"

#include "vfs_adapt.h"

void *vfs_lock_create(void)
{
    int32_t   ret;
    kmutex_t *m;

    m = krhino_mm_alloc(sizeof(kmutex_t));
    if (m == NULL) {
        return NULL;
    }

    ret = krhino_mutex_create(m, "VFS");
    if (ret != RHINO_SUCCESS) {
        krhino_mm_free(m);
        return NULL;
    }

    return (void *)m;
}

int32_t vfs_lock_free(void *lock)
{
    int32_t ret;

    kmutex_t *m = (kmutex_t *)lock;

    if (m == NULL) {
        return -1;
    }

    ret = krhino_mutex_del(m);
    if (ret != RHINO_SUCCESS) {
        return ret;
    }

    krhino_mm_free(m);

    return ret;
}

int32_t vfs_lock(void *lock)
{
    return krhino_mutex_lock((kmutex_t *)lock, RHINO_WAIT_FOREVER);
}

int32_t vfs_unlock(void *lock)
{
    return krhino_mutex_unlock((kmutex_t *)lock);
}

void *vfs_malloc(uint32_t size)
{
    return krhino_mm_alloc(size);
}

void vfs_free(void *ptr)
{
    krhino_mm_free(ptr);
}
