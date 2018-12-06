#include <stdio.h>
#include <k_api.h>

void os_mmu_setup(uint32_t vaddr_start, uint32_t vaddr_end,uint32_t paddr_start, mmu_mem_attr_t mem_attr)
{
	k_mmap(vaddr_start, paddr_start, vaddr_end + 1 - vaddr_start, mem_attr);
}

void mmu_init(void)
{
    k_icache_invalidate_all();
    k_icache_disable();

    k_dcache_invalidate_all();
    k_dcache_disable();

    k_mmu_disable();

    /* set page table */
	os_mmu_setup(0x08000000, 0x08FFFFFF, 0x00000000, MMU_ATTR_SO);
	os_mmu_setup(0x10000000, 0x1FFFFFFF, 0x10000000, MMU_ATTR_SO);
	os_mmu_setup(0x20000000, 0x2FFFFFFF, 0x20000000, MMU_ATTR_SO);
	os_mmu_setup(0x40000000, 0x43FFFFFF, 0x40000000, MMU_ATTR_SO);
	//os_mmu_setup(0x50000000, 0x5000FFFF, 0x50000000, MMU_ATTR_SO);
	//os_mmu_setup(0x50010000, 0x5001FFFF, 0x50010000, MMU_ATTR_SO);
    os_mmu_setup(0x50000000, 0x500FFFFF, 0x50000000, MMU_ATTR_SO);

	os_mmu_setup(0x80000000, 0x81ffffff, 0x80000000, MMU_ATTR_EXEC_DATA);
	os_mmu_setup(0x82000000, 0x83ffffff, 0x82000000, MMU_ATTR_EXEC_DATA);

	k_mmu_enable();

	k_icache_enable();
	k_dcache_enable();
}
