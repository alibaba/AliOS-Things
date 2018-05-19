/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef K_MM_BLK_H
#define K_MM_BLK_H

typedef struct {
    const name_t *pool_name;
    void         *pool_end;     /* end address */
    void         *pool_start;   /* start address */
    size_t        blk_size;
    size_t        blk_avail;    /* num of available(free) blk */
    size_t        blk_whole;    /* num of all blk */
    uint8_t      *avail_list;
    kspinlock_t   blk_lock;
#if (RHINO_CONFIG_SYSTEM_STATS > 0)
    klist_t       mblkpool_stats_item;
#endif
} mblk_pool_t;

/**
 * This function will init a blk-pool
 * @param[in]  pool        pointer to the pool
 * @param[in]  name        name of the pool
 * @param[in]  pool_start  start addr of the pool
 * @param[in]  blk_size    size of the blk
 * @param[in]  pool_size   size of the pool
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_mblk_pool_init(mblk_pool_t *pool, const name_t *name,
                              void *pool_start,
                              size_t blk_size, size_t pool_size);

/**
 * This function will alloc a blk-pool
 * @param[in]  pool  pointer to a pool
 * @param[in]  blk   pointer to a blk
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_mblk_alloc(mblk_pool_t *pool, void **blk);

/**
 * This function will free a blk-pool
 * @param[in]  pool  pointer to the pool
 * @param[in]  blk   pointer to the blk
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_mblk_free(mblk_pool_t *pool, void *blk);

/**
 * is blk in pool? 
 * @param[in]  pool  pointer to the pool
 * @param[in]  blk   pointer to the blk
 * @return  yes return 1, no reture 0 
 */
#define krhino_mblk_check(pool, blk)    \
        ((pool) != NULL                 \
        && ((void *)(blk) >= ((mblk_pool_t*)(pool))->pool_start)    \
        && ((void *)(blk) <  ((mblk_pool_t*)(pool))->pool_end))

#endif /* K_MM_BLK_H */

