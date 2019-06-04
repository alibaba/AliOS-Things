/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#ifndef K_MMPAGE_H
#define K_MMPAGE_H

/* free block head */
typedef struct mmpage_blk_tag
{
    /* free memory block list, order by address*/
	struct mmpage_blk_tag *next;
	size_t size; 
} mmpage_blk_t;

/* block head, before each blocks */
typedef struct
{
    uintptr_t vir_addr;
    uintptr_t phy_addr;
    size_t    total_size;
    /* Size for statistic. */
    size_t    free_size;
    size_t    free_size_min;
    /* free block list: order by address, from low to high */
	mmpage_blk_t freelist; 
} mmpage_info_t;

uintptr_t os_mmpage_vir2phy(mmpage_info_t *info, uintptr_t vaddr);
uintptr_t os_mmpage_phy2vir(mmpage_info_t *info, uintptr_t paddr);
void os_mmpage_init(mmpage_info_t *info, uintptr_t vaddr, uintptr_t paddr, size_t len);
void os_mmpage_remove(mmpage_info_t *info, void *remove_buffer, size_t remove_size);
void *os_mmpage_malloc(mmpage_info_t *info, size_t alloc_size, size_t align_size);
void os_mmpage_free(mmpage_info_t *info, void *free_buffer, size_t free_size);

#endif /* K_MMU_ASID_H */
