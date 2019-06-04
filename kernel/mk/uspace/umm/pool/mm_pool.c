
#include <stdio.h>
#include "k_api.h"
#include "mm_pool.h"

static kmutex_t *mm_pool_mutex;

/* Init pool, with one free block
   pool_addr should align to MM_POOL_MM_POOL_ALIGNMENT
   pool_len should be smaller than ~MM_POOL_MAGIC
*/
int32_t mm_pool_init(mm_pool_t *pool, char *name, void* pool_addr, size_t pool_len)
{
    mm_pool_blk_t *free_blk;

    if (  pool == NULL
       || ((size_t)pool_addr % MM_POOL_MM_POOL_ALIGNMENT != 0)
       || (pool_len > (~MM_POOL_MAGIC))
       || (pool_len < MM_POOL_BLK_MIN*2) )
    {
        //printf("[mm_pool]: pool parameter invalid!\n");
        return -__LINE__;
    }

    pool->freelist_head.next = (void *)pool_addr;
    pool->freelist_head.magic_size = (size_t) 0;

    pool->freelist_tail = (void *)((size_t)pool_addr + pool_len - MM_POOL_BLK_HEAD_SIZE);
    pool->freelist_tail->next = NULL;
    pool->freelist_tail->magic_size = 0;

    /* Only one block when init. */
    free_blk = (void *)pool_addr;
    free_blk->next = pool->freelist_tail;
    free_blk->magic_size = (size_t)pool->freelist_tail - (size_t) free_blk;

    pool->pool_addr     = pool_addr;
    pool->pool_len      = pool_len;
    pool->name          = name;

    pool->total_size    = free_blk->magic_size;
    pool->free_size     = free_blk->magic_size;
    pool->free_size_min = free_blk->magic_size;

    if (krhino_mutex_dyn_create(&mm_pool_mutex, "mm_pool_mutex") != RHINO_SUCCESS)
        return -__LINE__;

    return 0;
}

size_t mm_pool_total_size(mm_pool_t *pool)
{
    return pool->total_size;
}

size_t mm_pool_free_size(mm_pool_t *pool)
{
    return pool->free_size;
}

size_t mm_pool_free_size_min(mm_pool_t *pool)
{
    return pool->free_size_min;
}

size_t mm_max_free_block_size(mm_pool_t *pool)
{
    mm_pool_blk_t *blk_free;
    size_t         max_block_size = 0;

    blk_free = pool->freelist_head.next;

    while(blk_free != NULL) {
        if (max_block_size < blk_free->magic_size) {
            max_block_size = blk_free->magic_size;
        }
        blk_free = blk_free->next;
    }

    return max_block_size;
}

/* if addr is in pool, return 1. else return 0*/
int32_t mm_pool_check_addr(mm_pool_t *pool, void* addr)
{
    if ((size_t)addr - (size_t)pool->pool_addr < pool->pool_len)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

static void mm_pool_freeblk_insert(mm_pool_t *pool, mm_pool_blk_t *blk_insert)
{
    mm_pool_blk_t *blk_before; /* before the blk_insert */
    mm_pool_blk_t *blk_after;  /* after  the blk_insert */

    /* freelist is ordered by address, find blk_before */
    for (blk_before = &pool->freelist_head; blk_before->next < blk_insert; blk_before = blk_before->next)
    {
        if (blk_before == pool->freelist_tail)
        {
            //printf("[mm_pool]: free pointer invalid! %p %p\n", blk_insert, pool->freelist_tail);
            return;
        }
    }
    blk_after = blk_before->next;

    /* now: blk_before < blk_insert < blk_after */

    /* try to merge blk_before and blk_insert */
    if ((char *)blk_before + blk_before->magic_size == (char *)blk_insert)
    {
        blk_before->magic_size += blk_insert->magic_size;
        blk_insert = blk_before;
    }
    else
    {
        /* do not merge, just insert new node to freelist */
        blk_before->next = blk_insert;
    }

    /* try to merge blk_insert and blk_after */
    if (blk_after != pool->freelist_tail &&
        (char *)blk_insert + blk_insert->magic_size == (char *)blk_after)
    {
        blk_insert->magic_size += blk_after->magic_size;
        blk_insert->next = blk_after->next;
    }
    else
    {
        /* do not merge, just insert new node to freelist */
        blk_insert->next = blk_after;
    }
}

void *mm_pool_malloc(mm_pool_t *pool, size_t alloc_size)
{
    mm_pool_blk_t *blk_alloc;
    mm_pool_blk_t *blk_prev;
    mm_pool_blk_t *blk_left;

    if (pool == NULL || pool->freelist_tail == NULL)
    {
        return NULL;
    }

    krhino_mutex_lock(mm_pool_mutex, RHINO_WAIT_FOREVER);

    if  ((alloc_size == 0)
      || (alloc_size > pool->free_size))
    {
        /* not enough memory */
        krhino_mutex_unlock(mm_pool_mutex);
        return NULL;
    }

    alloc_size += MM_POOL_BLK_HEAD_SIZE;
    alloc_size  = MM_POOL_ALIGN(alloc_size, MM_POOL_MM_POOL_ALIGNMENT);

    /* find a free block bigger than alloc_size */
    blk_prev = &pool->freelist_head;
    blk_alloc = pool->freelist_head.next;
    while ((blk_alloc->magic_size < alloc_size) && (blk_alloc->next != NULL))
    {
        blk_prev = blk_alloc;
        blk_alloc = blk_alloc->next;
    }
    if (blk_alloc->next == NULL)
    {
        /* this means "blk_alloc == pool->freelist_tail" */
        krhino_mutex_unlock(mm_pool_mutex);
        return NULL;
    }

    /* delete blk_alloc from freelist */
    blk_prev->next = blk_alloc->next;

    /* check whether blk_alloc can split */
    if (blk_alloc->magic_size - alloc_size > MM_POOL_BLK_MIN)
    {
        /* split */
        blk_left = (void *)((char *)blk_alloc + alloc_size);
        blk_left->magic_size = blk_alloc->magic_size - alloc_size;

        blk_alloc->magic_size = alloc_size;

        /* Insert the new block into the list of free blocks. */
        mm_pool_freeblk_insert(pool, blk_left);
    }

    /* update statistic */
    pool->free_size -= blk_alloc->magic_size;
    if (pool->free_size < pool->free_size_min)
    {
        pool->free_size_min = pool->free_size;
    }

    krhino_mutex_unlock(mm_pool_mutex);

    /* blk belong to APP, magic set */
    blk_alloc->magic_size |= MM_POOL_MAGIC;
    blk_alloc->next = NULL;

    /* app used addr is after mm_pool_blk_t */
    return (void *)((char *)blk_alloc + MM_POOL_BLK_HEAD_SIZE);
}

void *mm_pool_realloc(mm_pool_t *pool, void *ptr, size_t alloc_size)
{
    mm_pool_blk_t *blk_left;
    mm_pool_blk_t *cur_blk;
    size_t         cur_size;
    size_t         free_size;
    void          *p = NULL;

    if (pool == NULL || pool->freelist_tail == NULL)
    {
        return NULL;
    }

    alloc_size += MM_POOL_BLK_HEAD_SIZE;
    alloc_size  = MM_POOL_ALIGN(alloc_size, MM_POOL_MM_POOL_ALIGNMENT);

    if ((ptr != NULL) && (alloc_size == 0))
    {
        mm_pool_free(pool, ptr);
    }
    else if (ptr == NULL)
    {
        p = mm_pool_malloc(pool, alloc_size);
    }
    else
    {
        /* alloc new buffer, free old buffer, and copy the data to new buffer */
        cur_blk = (mm_pool_blk_t *)((char *)ptr - MM_POOL_BLK_HEAD_SIZE);
        cur_size = cur_blk->magic_size & (~MM_POOL_MAGIC);
        if ((cur_size > alloc_size) && (cur_size - alloc_size > MM_POOL_BLK_MIN))
        {
            /* split cur_blk */
            krhino_mutex_lock(mm_pool_mutex, RHINO_WAIT_FOREVER);

            blk_left = (mm_pool_blk_t*)((char*)cur_blk + alloc_size);
            blk_left->magic_size = cur_size - alloc_size;

            free_size = blk_left->magic_size;

            mm_pool_freeblk_insert(pool, blk_left);

            cur_blk->magic_size = alloc_size | MM_POOL_MAGIC;
            cur_blk->next = NULL;

            p = (void *)((char *)cur_blk + MM_POOL_BLK_HEAD_SIZE);

            pool->free_size += free_size;

            krhino_mutex_unlock(mm_pool_mutex);
        }
        else
        {
            /* alloc new buffer, copy data from old buffer to new buffer */
            p = mm_pool_malloc(pool, alloc_size);
            if (p != NULL)
            {
                memcpy(p, ptr, alloc_size);
                mm_pool_free(pool, ptr);
            }
        }

    }

    return p;
}

int32_t mm_pool_free(mm_pool_t *pool, void *pfree)
{
    mm_pool_blk_t *free_blk;
    size_t         free_size;

    if (pool == NULL || pfree == NULL)
    {
        return -__LINE__;
    }

    if ( !mm_pool_check_addr(pool, pfree) )
    {
        return -__LINE__;
    }

    /* app used addr is after mm_pool_blk_t */
    free_blk = (mm_pool_blk_t *)((char *)pfree - MM_POOL_BLK_HEAD_SIZE);

    /* blk damaged check */
    if (free_blk->next != NULL
      ||(free_blk->magic_size & MM_POOL_MAGIC) != MM_POOL_MAGIC)
    {
        //printf("[mm_pool]: block has been destroyed!\n");
        return -__LINE__;
    }

    /* blk belong to free list, magic clear */
    free_blk->magic_size &= ~MM_POOL_MAGIC;
    free_size             = free_blk->magic_size;

    krhino_mutex_lock(mm_pool_mutex, RHINO_WAIT_FOREVER);

    mm_pool_freeblk_insert(pool, free_blk);

    /* update statistic */
    pool->free_size += free_size;

    krhino_mutex_unlock(mm_pool_mutex);

    return 0;
}

