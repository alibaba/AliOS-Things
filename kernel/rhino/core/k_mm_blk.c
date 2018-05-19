/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>

#if (RHINO_CONFIG_MM_BLK > 0)
kstat_t krhino_mblk_pool_init(mblk_pool_t *pool, const name_t *name,
                              void *pool_start,
                              size_t blk_size, size_t pool_size)
{
    CPSR_ALLOC();

    uint32_t blks;            /* max blocks mem pool offers */
    uint8_t *blk_cur;         /* block pointer for traversing */
    uint8_t *blk_next;        /* next block pointe for traversing */
    uint8_t *pool_end;        /* mem pool end */
    uint8_t  addr_align_mask; /* address alignment */

    NULL_PARA_CHK(pool);
    NULL_PARA_CHK(name);
    NULL_PARA_CHK(pool_start);

    /* over one block at least */
    if (pool_size < (blk_size << 1u)) {
        return RHINO_BLK_POOL_SIZE_ERR;
    }

    /* check address & size alignment */
    addr_align_mask = sizeof(void *) - 1u;

    if (((size_t)pool_start & addr_align_mask) > 0u) {
        return RHINO_INV_ALIGN;
    }

    if ((blk_size & addr_align_mask) > 0u) {
        return RHINO_INV_ALIGN;
    }

    if ((pool_size & addr_align_mask) > 0u) {
        return RHINO_INV_ALIGN;
    }

    krhino_spin_lock_init(&pool->blk_lock);

    pool_end = (uint8_t *)pool_start + pool_size;
    blks     = 0u;
    blk_cur  = (uint8_t *)pool_start;
    blk_next = blk_cur + blk_size;

    while (blk_next < pool_end) {
        blks++;
        /* use initial 4 byte point to next block */
        *(uint8_t **)blk_cur = blk_next;
        blk_cur              = blk_next;
        blk_next             = blk_cur + blk_size;
    }

    if (blk_next == pool_end) {
        blks++;
    }

    /* the last one */
    *((uint8_t **)blk_cur) = NULL;

    pool->pool_name  = name;
    pool->pool_start = pool_start;
    pool->pool_end   = pool_end;
    pool->blk_whole  = blks;
    pool->blk_avail  = blks;
    pool->blk_size   = blk_size;
    pool->avail_list = (uint8_t *)pool_start;

#if (RHINO_CONFIG_SYSTEM_STATS > 0)
    RHINO_CRITICAL_ENTER();
    klist_insert(&(g_kobj_list.mblkpool_head), &pool->mblkpool_stats_item);
    RHINO_CRITICAL_EXIT();
#endif

    TRACE_MBLK_POOL_CREATE(krhino_cur_task_get(), pool);

    return RHINO_SUCCESS;
}

kstat_t krhino_mblk_alloc(mblk_pool_t *pool, void **blk)
{
    kstat_t  status;
    uint8_t *avail_blk;

    NULL_PARA_CHK(pool);
    NULL_PARA_CHK(blk);

    krhino_spin_lock_irq_save(&pool->blk_lock);

    if (pool->blk_avail > 0u) {
        avail_blk          = pool->avail_list;
        *((uint8_t **)blk) = avail_blk;
        /* the first 4 byte is the pointer for next block */
        pool->avail_list   = *(uint8_t **)(avail_blk);
        pool->blk_avail--;
        status = RHINO_SUCCESS;
    } else {
        *((uint8_t **)blk) = NULL;
        status = RHINO_NO_MEM;
    }

    krhino_spin_unlock_irq_restore(&pool->blk_lock);

    return status;
}

kstat_t krhino_mblk_free(mblk_pool_t *pool, void *blk)
{
    NULL_PARA_CHK(pool);
    NULL_PARA_CHK(blk);

    krhino_spin_lock_irq_save(&pool->blk_lock);

    /* use the first 4 byte of the free block point to head of avail list */
    *((uint8_t **)blk) = pool->avail_list;
    pool->avail_list   = blk;
    pool->blk_avail++;

    krhino_spin_unlock_irq_restore(&pool->blk_lock);

    return RHINO_SUCCESS;
}
#endif /* RHINO_CONFIG_MM_BLK */

