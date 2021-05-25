/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef K_MM_BLK_H
#define K_MM_BLK_H

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup aos_rhino mm
 *  Pool memory management. Pool memory can be part of heap.
 *  Pool is used to manage the little fixed-size block
 *
 *  @{
 */

#define MM_BLK_SLICE_BIT            10
#define MM_BLK_SLICE_SIZE           (1<<MM_BLK_SLICE_BIT)
#define MM_BLK_SLICE_NUM            (RHINO_CONFIG_MM_TLF_BLK_SIZE/MM_BLK_SLICE_SIZE)

#define MM_BLK_SIZE2TYPE(size)      (32 - krhino_clz32((uint32_t)(size) - 1))
#define MM_BLK_TYPE2SIZE(type)      (1 << (type))

/**
 * memory pool info
 */
typedef struct {
    size_t        blk_size;
    uint32_t      fail_cnt;     /* alloc fail */
    uint32_t      freelist_cnt;
    uint32_t      nofree_cnt;   /* alloc but not free yet */
    uintptr_t     slice_cnt;
    uintptr_t     slice_addr;
    size_t        slice_offset;
    uintptr_t     free_head;
} mblk_list_t;

typedef struct {
    kspinlock_t   blk_lock;
    const name_t *pool_name;
    uintptr_t     pool_start;
    uintptr_t     pool_end;
    uint32_t      slice_cnt;  /* slice have bean used */
    char          slice_type[MM_BLK_SLICE_NUM];
    mblk_list_t   blk_list[MM_BLK_SLICE_BIT];
} mblk_pool_t;

typedef struct {
    const name_t *pool_name;
    size_t        pool_size;
    size_t        used_size;
    size_t        max_used_size;
    size_t        max_blk_size;
} mblk_info_t;

/**
 * Init a pool.
 *
 * @param[in]  pool        pointer to the pool
 * @param[in]  name        name of the pool
 * @param[in]  pool_start  start addr of the pool
 * @param[in]  pool_size   size of the pool
 *
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_mblk_pool_init(mblk_pool_t *pool, const name_t *name,
                              void *pool_start, size_t pool_size);

/**
 * Memory block alloc from the pool.
 *
 * @param[in]  pool  pointer to a pool
 * @param[in]  blk_size    need size, and alloced size
 *
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
void *krhino_mblk_alloc(mblk_pool_t *pool, uint32_t size);
void *krhino_mblk_alloc_nolock(mblk_pool_t *pool, uint32_t size);

/**
 * Memory block free to the pool.
 *
 * @param[in]  pool  pointer to the pool
 * @param[in]  blk   pointer to the blk
 *
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_mblk_free(mblk_pool_t *pool, void *blk);
kstat_t krhino_mblk_free_nolock(mblk_pool_t *pool, void *blk);

/**
 * This function will get information of the pool
 * @param[in]  pool  pointer to the pool
 * @param[out] info  info of pool
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_mblk_info(mblk_pool_t *pool, mblk_info_t *info);
kstat_t krhino_mblk_info_nolock(mblk_pool_t *pool, mblk_info_t *info);

/**
 * Check if this a pool block.
 *
 * @param[in]  pool  pointer to the pool
 * @param[in]  blk   pointer to the blk
 *
 * @return  yes return 1, no reture 0
 */
#define krhino_mblk_check(pool, blk)                             \
        ((pool) != NULL                                          \
        && ((uintptr_t)(blk) >= ((mblk_pool_t*)(pool))->pool_start) \
        && ((uintptr_t)(blk) <  ((mblk_pool_t*)(pool))->pool_end))

/**
 * get blk size, should followed by krhino_mblk_check
 * @param[in]  pool  pointer to the pool
 * @param[in]  blk   pointer to the blk
 * @return  the len of blk
 */
RHINO_INLINE size_t krhino_mblk_get_size(mblk_pool_t *pool, void *blk)
{
    uint32_t     slice_idx;
    uint32_t     blk_type;
    mblk_list_t *blk_list;

    slice_idx = ((uintptr_t)blk - pool->pool_start) >> MM_BLK_SLICE_BIT;
    blk_type = pool->slice_type[slice_idx];
    blk_list = &(pool->blk_list[blk_type]);

    return blk_list->blk_size;
}

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* K_MM_BLK_H */

