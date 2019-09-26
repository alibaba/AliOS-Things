/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "k_api.h"

#include "ramfs_adapt.h"

void *ramfs_mm_alloc(uint32_t size)
{
    return krhino_mm_alloc(size);
}

void ramfs_mm_free(void *ptr)
{
    krhino_mm_free(ptr);
}

void *ramfs_mm_realloc(void *oldmem, uint32_t newsize)
{
    return krhino_mm_realloc(oldmem, newsize);
}

