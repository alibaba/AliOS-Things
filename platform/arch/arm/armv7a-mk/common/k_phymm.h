/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
#ifndef K_PHYMM_H
#define K_PHYMM_H

#ifndef ALIGN_UP
#define ALIGN_UP(x, align)      (((x) + (align) - 1) & ~((align) - 1))
#endif

#ifndef ALIGN_DOWN
#define ALIGN_DOWN(x, align)    ((x) & ~((align) - 1))
#endif

/* free block head */
typedef struct kphymm_blk_tag
{
    /* free memory block list, order by address*/
	struct kphymm_blk_tag *next;
	uintptr_t paddr_start;
	uintptr_t paddr_end;
} kphymm_blk_t;

/* block head, before each blocks */
typedef struct
{
    uintptr_t phy_addr;
    size_t    total_size;
    /* Size for statistic. */
    size_t    free_size;
    size_t    free_size_min;
    /* free block list: order by phy_addr, from low to high */
	kphymm_blk_t freelist; 
} kphymm_info_t;

int32_t os_phymm_init(kphymm_info_t *info, uintptr_t paddr, size_t len);
int32_t os_phymm_remove(kphymm_info_t *info, uintptr_t paddr_remove, size_t remove_size);
uintptr_t os_phymm_malloc(kphymm_info_t *info, size_t alloc_size, size_t align_size);
int32_t os_phymm_free(kphymm_info_t *info, uintptr_t paddr_free, size_t free_size);

#endif /* K_PHYMM_H */
