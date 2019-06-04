/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include "k_arch.h"
#include "k_phymm.h"

/* add physical memory region */
int32_t os_phymm_init(kphymm_info_t *info, uintptr_t paddr, size_t len)
{
    kphymm_blk_t *blk_free;

    if ( info == NULL )
    {
        return -__LINE__;
    }

    info->phy_addr      = paddr;
    info->total_size    = len;
    info->free_size     = len;
    info->free_size_min = len;
    info->freelist.next = NULL;
    info->freelist.paddr_start = 0;
    info->freelist.paddr_end   = 0;

    /* Only one block when init. */
    blk_free = (kphymm_blk_t *)krhino_mm_alloc(sizeof(kphymm_blk_t));

    blk_free->next      = NULL;
    blk_free->paddr_start  = paddr;
    blk_free->paddr_end = paddr + len;

    info->freelist.next = blk_free;

    return 0;
}

/* remove the block from freelist */
int32_t os_phymm_remove(kphymm_info_t *info, uintptr_t paddr_remove, size_t remove_size)
{
    kphymm_blk_t *blk_find;    /* the block contain the blk_remove  */
    kphymm_blk_t *blk_prev;    /* the block before blk_find in freelist */
    kphymm_blk_t *blk_left;

    if ( info == NULL || info->freelist.next == NULL )
    {
        return -__LINE__;
    }

    if ( paddr_remove - info->phy_addr > info->total_size
      || paddr_remove + remove_size - info->phy_addr > info->total_size )
    {
        return -__LINE__;
    }

    /* find a free block bigger than paddr_remove */
    blk_prev = &(info->freelist);
    blk_find = blk_prev->next;
    while (blk_find != NULL)
    {
        if ( paddr_remove >= blk_find->paddr_start
          && paddr_remove + remove_size <= blk_find->paddr_end )
        {
            /* find success */
            break;
        }
        blk_prev = blk_find;
        blk_find = blk_prev->next;
    }
    if (blk_find == NULL)
    {
        return -__LINE__;
    }

    if (paddr_remove == blk_find->paddr_start)
    {
        if ( paddr_remove + remove_size == blk_find->paddr_end )
        {
            /* delete blk_find from freelist */
            blk_prev->next = blk_find->next;
            krhino_mm_free(blk_find);
        }
        else
        {
            /* update blk_find */
            blk_find->paddr_start = paddr_remove + remove_size;
        }
    }
    else
    {
        /* split the end part of blk_find */
        if ( paddr_remove + remove_size < blk_find->paddr_end )
        {
            /* new a free block */
            blk_left = (kphymm_blk_t *)krhino_mm_alloc(sizeof(kphymm_blk_t));
            if ( blk_left == NULL )
            {
                return -__LINE__;
            }
            blk_left->paddr_start = paddr_remove + remove_size;
            blk_left->paddr_end   = blk_find->paddr_end;

            /* insert blk_left to freelist */
            blk_left->next = blk_find->next;
            blk_find->next = blk_left;
        }

        /* split the begin part of blk_find */
        blk_find->paddr_end = paddr_remove;
    }

    /* update statistic */
    info->free_size -= remove_size;
    if (info->free_size < info->free_size_min)
    {
        info->free_size_min = info->free_size;
    }

    return 0;
}

/* alloc block from freelist, physical address is aligned
   return the physical address */
uintptr_t os_phymm_malloc(kphymm_info_t *info, size_t alloc_size, size_t align_size)
{
    uintptr_t paddr_alloc;
    kphymm_blk_t *blk_find;    /* the block get from freelist,  */
    kphymm_blk_t *blk_prev;    /* the block before blk_find in freelist */
    kphymm_blk_t *blk_left = NULL;

    //printf("[os_phymm_malloc] in: %p, %p\n", alloc_size, align_size);

    if ( info == NULL )
    {
        return 0;
    }

    if ( alloc_size < sizeof(kphymm_blk_t)
      || align_size == 0
      || alloc_size % align_size != 0
      || (align_size & (align_size - 1)) != 0 )
    {
        return 0;
    }

    /* find a free block bigger than alloc_size */
    blk_prev = &(info->freelist);
    blk_find = blk_prev->next;
    while (blk_find != NULL)
    {
        paddr_alloc = ALIGN_UP(blk_find->paddr_start, align_size);
        if ( paddr_alloc + alloc_size <= blk_find->paddr_end )
        {
            /* alloc success */
            break;
        }
        blk_prev = blk_find;
        blk_find = blk_prev->next;
    }

    if (blk_find == NULL)
    {
        return 0;
    }

    if (paddr_alloc == blk_find->paddr_start)
    {
        if ( paddr_alloc + alloc_size == blk_find->paddr_end )
        {
            /* delete blk_find from freelist */
            blk_prev->next = blk_find->next;
            krhino_mm_free(blk_find);
        }
        else
        {
            /* update blk_find */
            blk_find->paddr_start = paddr_alloc + alloc_size;
        }
    }
    else
    {
        /* split the end part of blk_find */
        if ( paddr_alloc + alloc_size < blk_find->paddr_end )
        {
            /* new a free block */
            blk_left = (kphymm_blk_t *)krhino_mm_alloc(sizeof(kphymm_blk_t));
            if ( blk_left == NULL )
            {
                return 0;
            }
            blk_left->paddr_start = paddr_alloc + alloc_size;
            blk_left->paddr_end   = blk_find->paddr_end;

            /* insert blk_left to freelist */
            blk_left->next = blk_find->next;
            blk_find->next = blk_left;
        }

        /* split the begin part of blk_find */
        blk_find->paddr_end = paddr_alloc;
    }

    /* update statistic */
    info->free_size -= alloc_size;
    if (info->free_size < info->free_size_min)
    {
        info->free_size_min = info->free_size;
    }

    //printf("[os_phymm_malloc] out: %p\n", paddr_alloc);

    return paddr_alloc;
}

/* free block */
int32_t os_phymm_free(kphymm_info_t *info, uintptr_t paddr_free, size_t free_size)
{
    kphymm_blk_t *blk_free;
    kphymm_blk_t *blk_before; /* before the blk_free */
    kphymm_blk_t *blk_after;  /* after  the blk_free */

    //printf("[os_phymm_free] in: %p, %p\n", paddr_free, free_size);

    if ( info == NULL )
    {
        return -__LINE__;
    }

    if ( info->freelist.next == NULL )
    {
        blk_free = (kphymm_blk_t *)krhino_mm_alloc(sizeof(kphymm_blk_t));
        if ( info == NULL )
        {
            return -__LINE__;
        }
        blk_free->next      = NULL;
        blk_free->paddr_start  = paddr_free;
        blk_free->paddr_end = paddr_free + free_size;

        info->freelist.next = blk_free;

        info->free_size += free_size;
        return 0;
    }

    if ( paddr_free - info->phy_addr > info->total_size
      || paddr_free + free_size - info->phy_addr > info->total_size )
    {
        return -__LINE__;
    }

    /* freelist is ordered by address, find blk_before */
    blk_before = &(info->freelist);
    while ( blk_before->next != NULL )
    {
        if ( blk_before->next->paddr_start > paddr_free )
        {
            /* find position */
            break;
        }
        blk_before = blk_before->next;
    }
    blk_after = blk_before->next;

    /* now: blk_before->paddr_start < paddr_free < blk_after->paddr_start */

    if (blk_before->paddr_end == paddr_free)
    {
        /* merge blk_before and paddr_free */
        blk_before->paddr_end = paddr_free + free_size;

        if (blk_after != NULL &&
            blk_before->paddr_end == blk_after->paddr_start)
        {
            blk_before->paddr_end = blk_after->paddr_end;
            blk_before->next = blk_after->next;
            krhino_mm_free(blk_after);
        }
    }
    else
    {
        /* try to merge blk_free and blk_after */
        if (blk_after != NULL &&
            paddr_free + free_size == blk_after->paddr_start)
        {
            blk_after->paddr_start = paddr_free;
        }
        else
        {
            blk_free = (kphymm_blk_t *)krhino_mm_alloc(sizeof(kphymm_blk_t));
            if ( blk_free == NULL )
            {
                return -__LINE__;
            }
            /* add a new block to freelist */
            blk_free->paddr_start = paddr_free;
            blk_free->paddr_end   = paddr_free + free_size;
            blk_free->next   = blk_after;
            blk_before->next = blk_free;

            /* now: blk_before -> blk_free -> blk_after */
        }
    }

    info->free_size += free_size;

    //printf("[os_phymm_free] out\n");
    return 0;
}

