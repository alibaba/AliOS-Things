/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include "k_api.h"

/* g_memmaps is defined in board code */
extern k_mmu_regions_t g_memmaps[];
extern uint32_t g_memmaps_size;

void k_mmu_region_map(void)
{
    int idx = 0;
    for (idx = 0; idx < g_memmaps_size; idx ++) {
        k_mmap(g_memmaps[idx].vaddr, g_memmaps[idx].paddr, g_memmaps[idx].len, g_memmaps[idx].mem_attr);
    }
}

#if (RHINO_CONFIG_CPU_NUM == 1)
void k_mmu_init()
{
    k_icache_disable();
    k_dcache_disable();
    k_mmu_disable();

    k_icache_invalidate_all();
    k_dcache_invalidate_all();

    k_mmu_table_init();
    k_mmu_region_map();

    k_mmu_enable();
    k_icache_enable();
    k_dcache_enable();
}
#else
void k_mmu_init()
{
    k_icache_disable();
    k_dcache_disable();
    k_mmu_disable();

    k_icache_invalidate_all();

    if (0 == cpu_cur_get()) {
        k_dcache_invalidate_all();

        k_mmu_table_init();
        k_mmu_region_map();

        k_cache_scu_enable();
    } else {
        k_l1dcache_invalidate_all();
    }

    k_cache_scu_invalidate(cpu_cur_get(), 0xf);
    k_cpu_smp_enable();

    k_mmu_enable();
    k_icache_enable();
    k_dcache_enable();
}
#endif


