/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include "k_arch.h"

/*
Short-descriptor format:
    It uses 32-bit descriptor entries in the translation tables, and provides:
    • Up to two levels of address lookup.
    • 32-bit input addresses.
    • Output addresses of up to 40 bits.
    • Support for PAs of more than 32 bits by use of supersections, with 16MB granularity.
    • Support for No access, Client, and Manager domains.
    • 32-bit table entries.

Size:
    Supersections   Consist of 16MB blocks of memory. 
    Sections        Consist of 1MB blocks of memory.
    Large pages     Consist of 64KB blocks of memory.
    Small pages     Consist of 4KB blocks of memory.
*/

/* Invalidate all unified TLB */
void k_cpu_tlb_invalidate()
{
    os_set_TLBIALL(0);
    OS_DSB();
    OS_ISB();
}

/* Invalidate all branch predictors */
void k_cpu_bp_invalidate()
{
    os_set_BPIALL(0);
    OS_DSB();
    OS_ISB();
}

void k_cpu_mmu_enable()
{
    uint32_t reg;
    reg  = os_get_SCTLR();
    /* 
    SCTLR.M, bit[0]   MMU enable.
    0  PL1&0 stage 1 MMU disabled.
    1  PL1&0 stage 1 MMU enabled.
    */
    reg |= 0x1;
    os_set_SCTLR(reg);
    OS_ISB();
}

void k_cpu_mmu_disable()
{
    uint32_t reg;
    reg  = os_get_SCTLR();
    /* 
    SCTLR.M, bit[0]   MMU enable.
    0  PL1&0 stage 1 MMU disabled.
    1  PL1&0 stage 1 MMU enabled.
    */
    reg &= ~0x1;
    os_set_SCTLR(reg);
    OS_ISB();
}

void *k_cpu_mmu_ttb_get()
{
    uint32_t reg;
    reg = os_get_TTBR0();
    return (void *)reg;
}

void k_cpu_mmu_ttb_set(uint32_t table[])
{
    os_set_TTBR0(table);
    OS_ISB();
}

void k_cpu_mmu_domain_set(uint32_t domain_set)
{
    os_set_DACR(domain_set);
    OS_ISB();
}

uint32_t k_cpu_mmu_domain_get()
{
    return os_get_DACR();
}

void k_cpu_mmu_section(uint32_t table[], 
              uintptr_t vaddr_start, uintptr_t vaddr_end, uintptr_t paddr_start, uint32_t mem_attr)
{
    volatile uint32_t *p_entry;
    volatile int32_t entry_num;
    volatile int32_t i;
    
    p_entry  = (uint32_t *)table + (vaddr_start >> 20);
    entry_num = (vaddr_end >> 20) - (vaddr_start >> 20);
    for(i = 0; i <= entry_num; i++)
    {
        *p_entry =  (((paddr_start >> 20) + i) << 20) | mem_attr;
        p_entry++;
    }
}

