/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include "k_api.h"
#include "k_arch.h"

void k_icache_enable()
{
    uint32_t reg;

    OS_DSB();
    reg  = os_get_SCTLR();
    /*
    SCTLR.I, bit[12]  Instruction cache enable.
    0  Instruction caches disabled.
    1  Instruction caches enabled.
    */
    reg |= 0x1000;
    os_set_SCTLR(reg);
    OS_ISB();
}

void k_icache_disable()
{
    uint32_t reg;

    OS_DSB();
    reg  = os_get_SCTLR();
    /*
    SCTLR.I, bit[12]  Instruction cache enable.
    0  Instruction caches disabled.
    1  Instruction caches enabled.
    */
    reg &= ~0x1000;
    os_set_SCTLR(reg);
    OS_ISB();
}

void k_icache_invalidate_all()
{
    OS_DSB();
    os_set_ICIALLU(0);
    os_set_BPIALL(0);
    OS_DSB();
    OS_ISB();
}

void k_dcache_enable()
{
    uint32_t reg;
    reg  = os_get_SCTLR();
    /*
    SCTLR.C, bit[2]  Cache enable.
    0  Data and unified caches disabled.
    1  Data and unified caches enabled.
    */
    reg |= 0x4;
    os_set_SCTLR(reg);
    OS_ISB();
}

void k_dcache_disable()
{
    uint32_t reg;
    reg  = os_get_SCTLR();
    /*
    SCTLR.C, bit[2]  Cache enable.
    0  Data and unified caches disabled.
    1  Data and unified caches enabled.
    */
    reg &= ~0x4;
    os_set_SCTLR(reg);
    OS_ISB();
}

void k_dcache_clean(uintptr_t buffer, uintptr_t size)
{
	unsigned int ptr;

	ptr = buffer & ~(RHINO_CACHE_LINE_SIZE - 1);

	while (ptr < buffer + size)
	{
	    os_set_DCCMVAC(ptr);
		ptr += RHINO_CACHE_LINE_SIZE;
	}
    OS_DMB();
}

void k_dcache_invalidate(uintptr_t buffer, uintptr_t size)
{
	unsigned int ptr;

	ptr = buffer & ~(RHINO_CACHE_LINE_SIZE - 1);

	while (ptr < buffer + size)
	{
	    os_set_DCIMVAC(ptr);
		ptr += RHINO_CACHE_LINE_SIZE;
	}
    OS_DMB();
}

void k_dcache_clean_invalidate(uintptr_t buffer, uintptr_t size)
{
	unsigned int ptr;

	ptr = buffer & ~(RHINO_CACHE_LINE_SIZE - 1);

	while(ptr < buffer + size)
	{
		os_set_DCCIMVAC(ptr);
		ptr += RHINO_CACHE_LINE_SIZE;
	}
    OS_DMB();
}

