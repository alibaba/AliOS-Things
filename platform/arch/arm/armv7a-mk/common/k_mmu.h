/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#ifndef K_MMU_H
#define K_MMU_H

/************************* MMU configurations *************************/
/* Where to put mmu page tables
   It is recommended to place "mmu_tbl" in the place as the vaddr same with paddr */
#define K_CONFIG_MMU_TABLE_REGION           __attribute__((section(".mmu_tbl")))

/* max number of page table, one table match one process
   K_CONFIG_MMU_TABLE_MAX can be set 1~16
   when == 1: always in kernel mode
   when > 1 : kernel + user mode */
#ifndef K_CONFIG_MMU_TABLE_MAX
#define K_CONFIG_MMU_TABLE_MAX              16
#endif

/* Whether the secondary page table is supported */
#ifndef K_CONFIG_MMU_LVL2
#define K_CONFIG_MMU_LVL2                   1
#endif

#ifndef K_CONFIG_MMU_LVL2_TABLES
/* number of level2 page tables, 1 table takes 1KB ram, cover 1MB memory */
#define K_CONFIG_MMU_LVL2_TABLES            16
#endif
/*********************************************************************/

#if (K_CONFIG_MMU_LVL2 == 1)
#define K_MMU_PAGE_ALIGN                    0x1000
#else
#define K_MMU_PAGE_ALIGN                    0x100000
#endif

/******************** mmu_mem_attr_t for app: ***********************/
typedef enum {
    /********** mem_attr for MMIO (e.g. regs, dma ...) **********/
    /* Strongly-ordered MMIO
       Non-Executable + Full access + Non-cacheable */
    MMU_ATTR_SO,
    /* Shareable Device MMIO
       Non-Executable + Full access + Non-cacheable */
    MMU_ATTR_DEV,
    /* Non-Shareable Device MMIO
       Non-Executable + Full access + Non-cacheable */
    MMU_ATTR_DEV_NSH,
    /* Non-cacheable Memory (DMA)
       Non-Executable + Full access + Non-cacheable */
    MMU_ATTR_NON_CACHE,

    /********** mem_attr for Normal memory (e.g. ddr, sram ...) **********/
    /* Normal memory for text / rodata
       Executable + Read-only + Cache write back */
    MMU_ATTR_EXEC,
    /* Normal memory for data / bss
       Non-Executable + Read-Write + Cache write back */
    MMU_ATTR_DATA,
    /* Normal memory for read-only data
       Non-Executable + Read-only + Cache write back */
    MMU_ATTR_DATA_RO,
    /* Normal memory for all (mixing text and data)
       Executable + Read-Write + Cache write back */
    MMU_ATTR_EXEC_DATA,

    /********** mem_attr for Normal memory in multicore system  **********/
    /* in multicore system, when ACTLR.SMP == 1,
       mem_attr should be with "shareable", for Cache coherency */
    /* Normal memory for text / rodata, shareable
       Executable + Read-only + Cache write back */
    MMU_ATTR_EXEC_SH,
    /* Normal memory for data / bss, shareable
       Non-Executable + Read-Write + Cache write back */
    MMU_ATTR_DATA_SH,
    /* Normal memory for read-only data, shareable
       Non-Executable + Read-only + Cache write back */
    MMU_ATTR_DATA_RO_SH,
    /* Normal memory for all (mixing text and data), shareable
       Executable + Read-Write + Cache write back */
    MMU_ATTR_EXEC_DATA_SH,

    /* memory for More combinations, add below */

    MMU_ATTR_BUTT
} mmu_mem_attr_t;

typedef void os_mmu_func_t(uintptr_t vaddr, uintptr_t paddr, size_t len, int32_t isKenrel);

#if (K_CONFIG_MMU_TABLE_MAX > 1)
void k_mmu_switch(uint32_t asid);
int32_t k_mmu_copy_kernel_pgtbl(uint32_t asid);
int32_t k_mmu_munmap_all(uint32_t asid, os_mmu_func_t handler);
#endif
uintptr_t k_mmu_vir2phy(uintptr_t vaddr);

void k_mmu_disable(void);
void k_mmu_enable(void);

/* MMU Map the "vaddr" to "paddr" with mem_attr,

when K_CONFIG_MMU_LVL2 switch off:
   Supersection / Section format,
   vaddr" "paddr" "len" should be aligned with 1M

when K_CONFIG_MMU_LVL2 switch on:
   Supersection / Section / Large Page / Small Page format,
   "vaddr" "paddr" "len" should be aligned with 4K */
int32_t k_mmap(uintptr_t vaddr,
               uintptr_t paddr,
               size_t    len,
               mmu_mem_attr_t  mem_attr);

int32_t k_mmu_create_mapping(uintptr_t vaddr, uintptr_t paddr,
                             size_t size, mmu_mem_attr_t mem_attr, int32_t pid);

int32_t k_mmu_remove_mapping(uintptr_t vaddr, uintptr_t paddr,
                             size_t len, int32_t pid);

uintptr_t k_mmu_lookup_phyaddr(uintptr_t vaddr);

void k_mmu_show(void);

#endif /* K_MMU_H */
