/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <time.h>

#include <k_api.h>
#include <aos/errno.h>
#include <aos/kernel.h>

#include "rhino_p.h"

#if (RHINO_CONFIG_KOBJ_DYN_ALLOC == 0)
#warning "RHINO_CONFIG_KOBJ_DYN_ALLOC is disabled!"
#endif

void *aos_zalloc(size_t size)
{
    void *tmp = NULL;

    if (size == 0) {
        return NULL;
    }

#if (RHINO_CONFIG_MM_DEBUG > 0u)
    tmp = krhino_mm_alloc(size | AOS_UNSIGNED_INT_MSB);
    krhino_owner_return_addr(tmp);
#else
    tmp = krhino_mm_alloc(size);
#endif

    if (tmp) {
        memset(tmp, 0, size);
    }

    return tmp;
}

void *aos_malloc(size_t size)
{
    void *tmp = NULL;

    if (size == 0) {
        return NULL;
    }

#if (RHINO_CONFIG_MM_DEBUG > 0u)
    tmp = krhino_mm_alloc(size | AOS_UNSIGNED_INT_MSB);
    krhino_owner_return_addr(tmp);
#else
    tmp = krhino_mm_alloc(size);
#endif

    return tmp;
}

void *aos_calloc(size_t nitems, size_t size)
{
    void *tmp = NULL;
    size_t len = (size_t)nitems * size;

    if (len == 0) {
        return NULL;
    }

#if (RHINO_CONFIG_MM_DEBUG > 0u)
    tmp = krhino_mm_alloc(len | AOS_UNSIGNED_INT_MSB);
    krhino_owner_return_addr(tmp);
#else
    tmp = krhino_mm_alloc(len);
#endif

    if (tmp) {
        memset(tmp, 0, len);
    }

    return tmp;
}

void *aos_realloc(void *mem, size_t size)
{
    void *tmp = NULL;

#if (RHINO_CONFIG_MM_DEBUG > 0u)
    tmp = krhino_mm_realloc(mem, size | AOS_UNSIGNED_INT_MSB);
    krhino_owner_return_addr(tmp);
#else
    tmp = krhino_mm_realloc(mem, size);
#endif

    return tmp;
}

void aos_alloc_trace(void *addr, uintptr_t allocator)
{
#if (RHINO_CONFIG_MM_DEBUG > 0u)
    krhino_owner_attach(addr, allocator);
#endif
}

void aos_free(void *mem)
{
    if (mem == NULL) {
        return;
    }

    krhino_mm_free(mem);
}
