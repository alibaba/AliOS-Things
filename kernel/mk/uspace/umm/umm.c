/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stddef.h>
#include <stdio.h>
#include "pool/mm_pool.h"

static mm_pool_t *mm_pool = NULL;

void *umm_init(void *mem, size_t size)
{
    void   *ptr;

    if ((mem == NULL) || (size <= sizeof(mm_pool_t))) {
        return NULL;
    }

    /* umm is initialized??? */
    if (mm_pool != NULL) {
        return NULL;
    }

    ptr = (void*)((char*)mem + sizeof(mm_pool_t));
    ptr = (void*)MM_POOL_ALIGN((unsigned long)ptr, MM_POOL_MM_POOL_ALIGNMENT);
    size -= (size_t)((char*)ptr - (char*)mem);
    if (mm_pool_init(mem, "umm_pool", ptr, size) == 0)
        mm_pool = mem;

    return mm_pool;
}

void *umm_alloc(size_t size)
{
    return mm_pool_malloc(mm_pool, size);
}

void *umm_realloc(void *ptr, size_t size)
{
    return mm_pool_realloc(mm_pool, ptr, size);
}

void umm_free(void *ptr)
{
    mm_pool_free(mm_pool, ptr);
}

size_t umm_total_size(void)
{
    return mm_pool_total_size(mm_pool);
}

size_t umm_free_size(void)
{
    return mm_pool_free_size(mm_pool);
}

size_t umm_used_size(void)
{
    return mm_pool_total_size(mm_pool) - mm_pool_free_size(mm_pool);
}

size_t umm_free_size_min(void)
{
    return mm_pool_free_size_min(mm_pool);
}

size_t umm_max_free_block_size(void)
{
    return mm_max_free_block_size(mm_pool);
}

void umm_show(void)
{
    printf("\r\n TotalSize |   FreeSize |   UsedSize |  MinFreeSize |  MaxFreeBlkSize\r\n");
    printf("%10d | %10d | %10d |   %10d | %10d\r\n",
            umm_total_size(),
            umm_free_size(),
            umm_used_size(),
            umm_free_size_min(),
            umm_max_free_block_size());
}

