/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
 
#include <k_api.h>
#include "k_arch.h"

void k_cpu_icache_enable()
{
    uint32_t reg;
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

void k_cpu_icache_disable()
{
    uint32_t reg;
    reg  = os_get_SCTLR();
    /* 
    SCTLR.I, bit[12]  Instruction cache enable.
    0  Instruction caches disabled.
    1  Instruction caches enabled.
    */
    reg &= 0x1000;
    os_set_SCTLR(reg);
    OS_ISB();
}

void k_cpu_icache_invalidate_all()
{
    os_set_ICIALLU(0);
    OS_DSB();
    OS_ISB();
}

void k_cpu_dcache_enable()
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
    OS_DSB();
}

void k_cpu_dcache_disable()
{
    uint32_t reg;
    reg  = os_get_SCTLR();
    /* 
    SCTLR.C, bit[2]  Cache enable. 
    0  Data and unified caches disabled.
    1  Data and unified caches enabled.
    */
    reg &= 0x4;
    os_set_SCTLR(reg);
    OS_DSB();
}

void k_cpu_dcache_clean(unsigned int buffer, unsigned int size)
{
	unsigned int ptr;

	ptr = buffer & ~(RHINO_CACHE_LINE_SIZE - 1);

	while (ptr < buffer + size)
	{
	    os_set_DCCMVAC(ptr);
		ptr += RHINO_CACHE_LINE_SIZE;
	}
}

void k_cpu_dcache_invalidate(unsigned int buffer, unsigned int size)
{
	unsigned int ptr;

	ptr = buffer & ~(RHINO_CACHE_LINE_SIZE - 1);

	while (ptr < buffer + size)
	{
	    os_set_DCIMVAC(ptr);
		ptr += RHINO_CACHE_LINE_SIZE;
	}
}

void k_cpu_dcache_clean_invalidate(unsigned int buffer, unsigned int size)
{
	unsigned int ptr;

	ptr = buffer & ~(RHINO_CACHE_LINE_SIZE - 1);

	while(ptr < buffer + size)
	{
		os_set_DCCIMVAC(ptr);
		ptr += RHINO_CACHE_LINE_SIZE;
	}
}

