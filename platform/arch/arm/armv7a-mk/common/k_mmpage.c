/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include "k_arch.h"
#include "k_mmpage.h"

#ifndef ALIGN_UP
#define ALIGN_UP(x, align)      (((x) + (align) - 1) & ~((align) - 1))
#endif 

#ifndef ALIGN_DOWN
#define ALIGN_DOWN(x, align)    ((x) & ~((align) - 1))
#endif

uintptr_t os_mmpage_vir2phy(mmpage_info_t *info, uintptr_t vaddr)
{
    if ( vaddr - info->vir_addr > info->total_size )
    {
        return 0;
    }
    return vaddr - info->vir_addr + info->phy_addr;
}

uintptr_t os_mmpage_phy2vir(mmpage_info_t *info, uintptr_t paddr)
{
    if ( paddr - info->phy_addr > info->total_size )
    {
        return 0;
    }
    return paddr - info->phy_addr + info->vir_addr;
}

/* add a region to mmpage */
void os_mmpage_init(mmpage_info_t *info, uintptr_t vaddr, uintptr_t paddr, size_t len)
{
    void *region;
    mmpage_blk_t *free_blk;

    region = (void *)vaddr;

    if ( info == NULL )
    {
        return;
    }

    /* Only one block when init. */
	free_blk = (mmpage_blk_t *)region;
	free_blk->next = NULL;
	free_blk->size = len;

    info->vir_addr      = vaddr;
    info->phy_addr      = paddr;
	info->total_size    = len;
	info->free_size     = len;
	info->free_size_min = len;

	info->freelist.next = free_blk;
	info->freelist.size = 0;
}

/* remove the block from freelist */
void os_mmpage_remove(mmpage_info_t *info, void *remove_buffer, size_t remove_size)
{
    mmpage_blk_t *blk_remove;  /* the block to remove */
    mmpage_blk_t *blk_find;    /* the block contain the blk_remove  */
    mmpage_blk_t *blk_prev;    /* the block before blk_find in freelist */
    mmpage_blk_t *blk_left;

    if ( info == NULL )
    {
        return;
    }

    /* Caller guarantee:
       1. alloc_size % align_size == 0
       2. align_size is power of 2*/

    if ( remove_buffer == NULL || remove_size < sizeof(mmpage_blk_t) )
    {
        return;
    }

    blk_remove = (mmpage_blk_t *)remove_buffer;

    /* find a free block bigger than alloc_size */
	blk_prev = &(info->freelist);
    blk_find = blk_prev->next;
	while (blk_find != NULL)
	{
	    if ( (char *)blk_remove >= (char *)blk_find
	      && (char *)blk_remove + remove_size <= (char *)blk_find + blk_find->size )
	    {
	        /* find success */
	        break;
	    }
    	blk_prev = blk_find;
    	blk_find = blk_prev->next;
	}
	if (blk_find == NULL)
    {
        return;
    }

    /* split the end part of blk_find */
	if ( (char *)blk_remove + remove_size < (char *)blk_find + blk_find->size )
    {
        /* new a free block */
        blk_left = (mmpage_blk_t *)((char *)blk_remove + remove_size);
    	blk_left->size = (char *)blk_find + blk_find->size - (char *)blk_left;
    	
        /* insert blk_left to freelist */
    	blk_left->next = blk_find->next;
        blk_find->next = blk_left;
    }

    /* split the begin part of blk_find */
	if (blk_remove != blk_find)
    {
        /* update blk_find info */
    	blk_find->size = (char *)blk_remove - (char *)blk_find;
    }
    else
    {
        /* delete blk_find from freelist */
        blk_prev->next = blk_find->next;
    }
    
    /* update statistic */
	info->free_size -= remove_size;
	if (info->free_size < info->free_size_min)
    {
    	info->free_size_min = info->free_size;
    }

	return;	
}

/* alloc block from freelist, physical address is aligned
   return the virtual address */
void *os_mmpage_malloc(mmpage_info_t *info, size_t alloc_size, size_t align_size)
{
    uintptr_t offset, phyaddr;
    mmpage_blk_t *blk_alloc;   /* the block for return */
    mmpage_blk_t *blk_find;    /* the block get from freelist,  */
    mmpage_blk_t *blk_prev;    /* the block before blk_find in freelist */
    mmpage_blk_t *blk_left;

    if ( info == NULL )
    {
        return NULL;
    }

    if ( alloc_size < sizeof(mmpage_blk_t)
      || align_size == 0
      || alloc_size % align_size != 0
      || (align_size & (align_size - 1)) != 0 )
    {
        return NULL;
    }

    /* find a free block bigger than alloc_size */
	blk_prev = &(info->freelist);
    blk_find = blk_prev->next;
	while (blk_find != NULL)
	{
	    phyaddr = os_mmpage_vir2phy(info, (uintptr_t)blk_find);
	    offset  = ALIGN_UP(phyaddr, align_size) - phyaddr;
	    blk_alloc = (mmpage_blk_t *)((uintptr_t)blk_find + offset);
	    if ( (char *)blk_alloc + alloc_size <= (char *)blk_find + blk_find->size )
	    {
	        /* alloc success */
	        break;
	    }
    	blk_prev = blk_find;
    	blk_find = blk_prev->next;
	}
	if (blk_find == NULL)
    {
        return NULL;
    }

    /* split the end part of blk_find */
	if ( (char *)blk_alloc + alloc_size < (char *)blk_find + blk_find->size )
    {
        /* new a free block */
        blk_left = (mmpage_blk_t *)((char *)blk_alloc + alloc_size);
    	blk_left->size = (char *)blk_find + blk_find->size - (char *)blk_left;
    	
        /* insert blk_left to freelist */
    	blk_left->next = blk_find->next;
        blk_find->next = blk_left;
    }

    /* split the begin part of blk_find */
	if (blk_alloc != blk_find)
    {
        /* update blk_find info */
    	blk_find->size = (char *)blk_alloc - (char *)blk_find;
    }
    else
    {
        /* delete blk_find from freelist */
        blk_prev->next = blk_find->next;
    }

    /* update statistic */
	info->free_size -= alloc_size;
	if (info->free_size < info->free_size_min)
    {
    	info->free_size_min = info->free_size;
    }

	return blk_alloc;
}

/* free block */
void os_mmpage_free(mmpage_info_t *info, void *free_buffer, size_t free_size)
{
    mmpage_blk_t *blk_free;
    mmpage_blk_t *blk_before; /* before the blk_free */
    mmpage_blk_t *blk_after;  /* after  the blk_free */

    if ( info == NULL )
    {
        return;
    }


	if ( free_buffer == NULL || free_size < sizeof(mmpage_blk_t) )
    {
        return;
    }

    blk_free = (mmpage_blk_t *)free_buffer;

    /* freelist is ordered by address, find blk_before */
    for (blk_before = &(info->freelist); blk_before->next < blk_free; blk_before = blk_before->next)
    {
        if (blk_before->next == NULL)
        {
            break;
        }
    }
    blk_after = blk_before->next;

    /* now: blk_before < blk_free < blk_after */

    /* add a new block to freelist */
    blk_free->size   = free_size;
    blk_free->next   = blk_after;
    blk_before->next = blk_free;

    /* now: blk_before -> blk_free -> blk_after */

    /* try to merge blk_free and blk_after */
	if (blk_after != NULL &&
	    (char *)blk_free + blk_free->size == (char *)blk_after)
    {
    	blk_free->size += blk_after->size;
    	blk_free->next = blk_after->next;
    }

    /* try to merge blk_before and blk_free */
	if ((char *)blk_before + blk_before->size == (char *)blk_free)
    {
    	blk_before->size += blk_free->size;
    	blk_before->next = blk_free->next;
    }

	info->free_size += free_size;    	
}

