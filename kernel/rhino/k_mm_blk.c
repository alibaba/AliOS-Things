/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "k_api.h"

kstat_t krhino_mblk_pool_init(mblk_pool_t *pool, const name_t *name,
                              void *pool_start, size_t pool_size)
{
    uint32_t  blk_type;     /* max blocks mem pool offers */
    uint8_t   align_mask;   /* address alignment */
    mblk_list_t *blk_list;

    NULL_PARA_CHK(pool);
    NULL_PARA_CHK(name);
    NULL_PARA_CHK(pool_start);

    memset(pool_start, 0, pool_size);

    /* check address & size alignment */
    align_mask = sizeof(uintptr_t) - 1u;

    if (((size_t)pool_start & align_mask) || (pool_size & align_mask)) {
        return RHINO_INV_ALIGN;
    }

    if (pool_size % MM_BLK_SLICE_SIZE) {
        return RHINO_MM_POOL_SIZE_ERR;
    }

    krhino_spin_lock_init(&pool->blk_lock);

    pool->pool_name  = name;
    pool->pool_start = (uintptr_t)pool_start;
    pool->pool_end   = (uintptr_t)(pool_start + pool_size);
    pool->slice_cnt  = 0;

    memset(pool->slice_type, 0, sizeof(pool->slice_type));

    for (blk_type = 0 ; blk_type < MM_BLK_SLICE_BIT ; blk_type++) {
        blk_list = &pool->blk_list[blk_type];
        memset(blk_list, 0, sizeof(*blk_list));
        blk_list->blk_size = MM_BLK_TYPE2SIZE(blk_type);
    }

    TRACE_MBLK_POOL_CREATE(krhino_cur_task_get(), pool);

    return RHINO_SUCCESS;
}

void *krhino_mblk_alloc_nolock(mblk_pool_t *pool, uint32_t size)
{
    uint32_t     blk_type;
    mblk_list_t *blk_list = NULL;
    uintptr_t    avail_blk = (uintptr_t)NULL;

    if (pool == NULL) {
        return NULL;
    }

    size = size < sizeof(uintptr_t) ? sizeof(uintptr_t) : size;

    blk_type = MM_BLK_SIZE2TYPE(size);

    while (blk_type < MM_BLK_SLICE_BIT)  {
        blk_list = &(pool->blk_list[blk_type]);

        /* try to get from freelist */
        if ((avail_blk = blk_list->free_head) != (uintptr_t)NULL) {
            blk_list->free_head = *(uintptr_t *)avail_blk;
            blk_list->freelist_cnt--;
            break;
        }

        /* check if need new slice */
        if (blk_list->slice_addr == 0 || blk_list->slice_offset == MM_BLK_SLICE_SIZE) {
            if (pool->slice_cnt == MM_BLK_SLICE_NUM) {
                blk_type++;
                continue;
            }

            /* get new slice for this type blks */
            blk_list->slice_addr = pool->pool_start + pool->slice_cnt * MM_BLK_SLICE_SIZE;
            pool->slice_type[pool->slice_cnt] = blk_type;
            blk_list->slice_offset = 0;
            pool->slice_cnt++;
            blk_list->slice_cnt++;
        }

        /* cut blk from slice */
        avail_blk = blk_list->slice_addr + blk_list->slice_offset;
        blk_list->slice_offset += blk_list->blk_size;
        break;
    };

    if (blk_list) {
        (avail_blk == (uintptr_t)0) ? blk_list->fail_cnt++ : blk_list->nofree_cnt++;
    }

    return (void *)avail_blk;
}

kstat_t krhino_mblk_free_nolock(mblk_pool_t *pool, void *blk)
{
    uint32_t     slice_idx;
    uint32_t     blk_type;
    mblk_list_t *blk_list;

    NULL_PARA_CHK(pool);
    NULL_PARA_CHK(blk);

    slice_idx = ((uintptr_t)blk - pool->pool_start) >> MM_BLK_SLICE_BIT;
    if (slice_idx >= MM_BLK_SLICE_NUM) {
        return RHINO_MM_FREE_ADDR_ERR;
    }

    blk_type = pool->slice_type[slice_idx];
    if (blk_type >= MM_BLK_SLICE_BIT) {
        return RHINO_MM_FREE_ADDR_ERR;
    }

    blk_list = &(pool->blk_list[blk_type]);
    /* use the first 4 byte of the free block point to head of free list */
    *((uintptr_t *)blk) = blk_list->free_head;
    blk_list->free_head = (uintptr_t)blk;
    blk_list->nofree_cnt--;
    blk_list->freelist_cnt++;

    return RHINO_SUCCESS;
}

kstat_t krhino_mblk_info_nolock(mblk_pool_t *pool, mblk_info_t *info)
{
    size_t   blk_size = 0;
    uint32_t idx;
    mblk_list_t *blk_list;
    uint32_t size_in_list   = 0;
    uint32_t size_in_slice  = 0;

    NULL_PARA_CHK(pool);
    NULL_PARA_CHK(info);

    /* no data changed, no lock needed. The info may be not absolutely precise */

    for (idx = 0 ; idx < MM_BLK_SLICE_BIT ; idx++) {
        blk_list = &(pool->blk_list[idx]);
        size_in_list  += blk_list->nofree_cnt * blk_list->blk_size;
        if (blk_list->slice_cnt > 0) {
            size_in_slice += (blk_list->slice_cnt - 1) * MM_BLK_SLICE_SIZE + blk_list->slice_offset;
            blk_size = blk_list->blk_size;
        }
    }

    info->pool_name = pool->pool_name;
    info->pool_size = pool->pool_end - pool->pool_start;
    info->used_size = size_in_list;
    info->max_used_size = size_in_slice;
    info->max_blk_size  = blk_size;

    return RHINO_SUCCESS;
}

void *krhino_mblk_alloc(mblk_pool_t *pool, uint32_t size)
{
    uintptr_t    avail_blk;
    cpu_cpsr_t   flags_cpsr;

    if (pool == NULL) {
        return NULL;
    }

    krhino_spin_lock_irq_save(&pool->blk_lock, flags_cpsr);

    avail_blk = (uintptr_t)krhino_mblk_alloc_nolock(pool, size);

    krhino_spin_unlock_irq_restore(&pool->blk_lock, flags_cpsr);

    return (void *)avail_blk;
}

kstat_t krhino_mblk_free(mblk_pool_t *pool, void *blk)
{
    kstat_t ret;
    cpu_cpsr_t   flags_cpsr;

    NULL_PARA_CHK(pool);
    NULL_PARA_CHK(blk);

    krhino_spin_lock_irq_save(&pool->blk_lock, flags_cpsr);

    ret = krhino_mblk_free_nolock(pool, blk);

    krhino_spin_unlock_irq_restore(&pool->blk_lock, flags_cpsr);

    return ret;
}

kstat_t krhino_mblk_info(mblk_pool_t *pool, mblk_info_t *info)
{
    kstat_t ret;
    cpu_cpsr_t   flags_cpsr;

    NULL_PARA_CHK(pool);
    NULL_PARA_CHK(info);

    krhino_spin_lock_irq_save(&pool->blk_lock, flags_cpsr);

    ret = krhino_mblk_info_nolock(pool, info);

    krhino_spin_unlock_irq_restore(&pool->blk_lock, flags_cpsr);

    return ret;
}


