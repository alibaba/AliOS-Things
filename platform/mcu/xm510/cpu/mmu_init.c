#include <stdio.h>
#include <k_api.h>

/* level1 page table, entry size 1MB memory. */
uint32_t MMUTable[4*1024] __attribute__((section("mmu_tbl"))) __attribute__((aligned(16*1024)));

void mmu_init(void)
{
    k_cpu_icache_invalidate_all();
    k_cpu_dcache_invalidate_all();
    k_cpu_tlb_invalidate();
    k_cpu_bp_invalidate();

    k_cpu_dcache_disable();
    k_cpu_icache_disable();
    k_cpu_mmu_disable();

    /* set page table */
	k_cpu_mmu_section(MMUTable, 0x08000000, 0x08FFFFFF, 0x00000000, MMU_ATTR_NX_RW_SO);
	k_cpu_mmu_section(MMUTable, 0x10000000, 0x1FFFFFFF, 0x10000000, MMU_ATTR_NX_RW_SO);
	k_cpu_mmu_section(MMUTable, 0x20000000, 0x2FFFFFFF, 0x20000000, MMU_ATTR_NX_RW_SO);
	k_cpu_mmu_section(MMUTable, 0x40000000, 0x43FFFFFF, 0x40000000, MMU_ATTR_NX_RW_SO);
	k_cpu_mmu_section(MMUTable, 0x50000000, 0x5000FFFF, 0x50000000, MMU_ATTR_NX_RW_SO);
	k_cpu_mmu_section(MMUTable, 0x50010000, 0x5001FFFF, 0x50010000, MMU_ATTR_NX_RW_SO);

	k_cpu_mmu_section(MMUTable, 0x80000000, 0x81ffffff, 0x80000000, MMU_ATTR_X_RW_WB);
	k_cpu_mmu_section(MMUTable, 0x82000000, 0x83ffffff, 0x82000000, MMU_ATTR_X_RW_WB);

    /* become clients for all domains */
	k_cpu_mmu_domain_set(0xffffffff);
    k_cpu_mmu_ttb_set(MMUTable);

	k_cpu_mmu_enable();

	k_cpu_icache_enable();
	k_cpu_dcache_enable();
}
