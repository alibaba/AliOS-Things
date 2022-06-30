/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2017-08-24     chinesebear  first version
 */

#include "synopGMAC_plat.h"
#include "synopGMAC_Dev.h"
#include <aos/kernel.h>
#include <aos/debug.h>
#include <ls1c_cache.h>

static inline void flush_cache(unsigned long start, unsigned long size)
{
    dcache_writeback_invalidate_range(start, start + size);
}

dma_addr_t __attribute__((weak)) gmac_dmamap(unsigned long va, u32 size)
{
	return VA_TO_PA (va);
	//return UNCACHED_TO_PHYS(va);
}



/**
  * This is a wrapper function for Memory allocation routine. In linux Kernel 
  * it it kmalloc function
  * @param[in] bytes in bytes to allocate
  */

void *plat_alloc_memory(u32 bytes)
{
//return (void*)malloc((size_t)bytes, M_DEVBUF, M_DONTWAIT);
	void *buf = aos_malloc((u32)bytes);

	flush_cache((unsigned long)buf, bytes);
	return buf;
}

/**
  * This is a wrapper function for consistent dma-able Memory allocation routine. 
  * In linux Kernel, it depends on pci dev structure
  * @param[in] bytes in bytes to allocate
  */

//void *plat_alloc_consistent_dmaable_memory(struct synopGMACdevice *dev, u32 size, u32 *addr) 
void *plat_alloc_consistent_dmaable_memory(synopGMACdevice *pcidev, u32 size, u32 *addr) 
{
	void *buf;
	buf = aos_malloc((u32)(size+16));
	//CPU_IOFlushDCache( buf,size, SYNC_W);
	unsigned long i = (unsigned long)buf;
//	aos_debug_printf("size = %d\n", size);
//	aos_debug_printf("bufaddr = %p\n", buf);
//	aos_debug_printf("i%%16 == %d\n", i%16);
	if(i%16 == 8){
		i += 8;
	}
	else if(i%16 == 4){
		i += 12;
	}
	else if(i%16 == 12){
		i += 4;
	}

	flush_cache(i, size);
	*addr =gmac_dmamap(i, size);
	buf = (unsigned char *)CACHED_TO_UNCACHED(i);
//	aos_debug_printf("bufaddr = %p\n", buf);
	return buf;
}


/**
  * This is a wrapper function for freeing consistent dma-able Memory.
  * In linux Kernel, it depends on pci dev structure
  * @param[in] bytes in bytes to allocate
  */


//void plat_free_consistent_dmaable_memory(void * addr) 
void plat_free_consistent_dmaable_memory(synopGMACdevice *pcidev, u32 size, void * addr,u32 dma_addr) 
{
    aos_free((void*)PHYS_TO_CACHED(UNCACHED_TO_PHYS(addr)));
}



/**
  * This is a wrapper function for Memory free routine. In linux Kernel 
  * it it kfree function
  * @param[in] buffer pointer to be freed
  */
void plat_free_memory(void *buffer) 
{
    aos_free(buffer);
}



dma_addr_t plat_dma_map_single(void *hwdev, void *ptr, u32 size)
{
    unsigned long addr = (unsigned long) ptr;
    //CPU_IOFlushDCache(addr,size, direction);
    flush_cache(addr, size);
    return gmac_dmamap(addr, size);
}

/**
  * This is a wrapper function for platform dependent delay 
  * Take care while passing the argument to this function 
  * @param[in] buffer pointer to be freed
  */
void plat_delay(u32 delay)
{
	while (delay--);
	return;
}
