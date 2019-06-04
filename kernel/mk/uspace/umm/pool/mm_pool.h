/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef MM_POOL
#define MM_POOL

#include <stdio.h>
#include <stdint.h>

#define MM_POOL_ALIGN(x,a)          (((x) + (a) - 1) & ~((a) - 1))
#define MM_POOL_MM_POOL_ALIGNMENT   8

/* At the beginning of allocated memory */
#define MM_POOL_BLK_HEAD_SIZE       MM_POOL_ALIGN(sizeof(mm_pool_blk_t), MM_POOL_MM_POOL_ALIGNMENT)
/* Block sizes must not get too small. */
#define MM_POOL_BLK_MIN	            ((MM_POOL_BLK_HEAD_SIZE) << 1)

/* blk belong to APP, magic set
   blk belong to free list, magic clear */
#define MM_POOL_MAGIC               (0xF0000000)

#define MM_POOL_CRITICAL_ENTER()    krhino_sched_disable()
#define MM_POOL_CRITICAL_EXIT()     krhino_sched_enable()

/* block head, before each blocks */
typedef struct mm_pool_blk_tag
{
    /* free memory block list, order by address*/
	struct mm_pool_blk_tag *next;
	/* when blk in APP: MM_POOL_MAGIC & size
	   when blk in freelist: only size
	   size include mm_pool_blk_t self */
	size_t magic_size;
} mm_pool_blk_t;

/* pool control struct */
typedef struct
{
    void     *pool_addr;
    size_t    pool_len;
    char     *name;
    /* free block list: order by address, from low to high */
    /* first free block. */
    mm_pool_blk_t  freelist_head;
    /* point to last free block. */
    mm_pool_blk_t *freelist_tail;
    /* For statistic. */
    size_t total_size;
    size_t free_size;
    size_t free_size_min;
} mm_pool_t;


/* Init pool, with one free block
   pool_addr should align to MM_POOL_MM_POOL_ALIGNMENT
   pool_len should be smaller than ~MM_POOL_MAGIC
*/
int32_t mm_pool_init(mm_pool_t *pool, char *name, void *pool_addr, size_t pool_len);

void   *mm_pool_malloc(mm_pool_t *pool, size_t alloc_size);
void   *mm_pool_realloc(mm_pool_t *pool, void *ptr, size_t alloc_size);

int32_t mm_pool_free(mm_pool_t *pool, void *pfree);

size_t mm_pool_total_size(mm_pool_t *pool);
size_t mm_pool_free_size(mm_pool_t *pool);
size_t mm_pool_free_size_min(mm_pool_t *pool);
size_t mm_max_free_block_size(mm_pool_t *pool);

/* if addr is in pool, return 1. else return 0*/
int32_t mm_pool_check_addr(mm_pool_t *pool, void* addr);

#endif
