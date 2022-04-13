/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

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

void k_icache_invalidate(uintptr_t text, uintptr_t size)
{
    uint32_t reg;
    uint32_t line;
    uintptr_t ptr;

    /* get the cache line size from CCSIDR:
       LineSize, bits[2:0], (Log2(Number of words in cache line)) -2 */
    os_set_CSSELR(1);
    reg  = os_get_CCSIDR();
    line = 1 << ((reg & 0x7) + 4);

    ptr = text & ~(line - 1);
    OS_DSB();
    while (ptr < text + size) {
        os_set_ICIMVAU(ptr);
        os_set_BPIMVA(ptr);
        ptr += line;
    }
    OS_DSB();
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
    uint32_t reg;
    uint32_t line;
    uintptr_t ptr;

    /* get the cache line size from CCSIDR:
       LineSize, bits[2:0], (Log2(Number of words in cache line)) -2 */
    os_set_CSSELR(0);
    reg  = os_get_CCSIDR();
    line = 1 << ((reg & 0x7) + 4);

    ptr = buffer & ~(line - 1);

    while (ptr < buffer + size) {
        os_set_DCCMVAC(ptr);
        ptr += line;
    }
    OS_DMB();
}

void k_dcache_invalidate(uintptr_t buffer, uintptr_t size)
{
    uint32_t reg;
    uint32_t line;
    uintptr_t ptr;

    /* get the cache line size from CCSIDR:
       LineSize, bits[2:0], (Log2(Number of words in cache line)) -2 */
    os_set_CSSELR(0);
    reg  = os_get_CCSIDR();
    line = 1 << ((reg & 0x7) + 4);

    ptr = buffer & ~(line - 1);

    while (ptr < buffer + size) {
        os_set_DCIMVAC(ptr);
        ptr += line;
    }
    OS_DMB();
}

void k_dcache_clean_invalidate(uintptr_t buffer, uintptr_t size)
{
    uint32_t reg;
    uint32_t line;
    uintptr_t ptr;

    /* get the cache line size from CCSIDR:
       LineSize, bits[2:0], (Log2(Number of words in cache line)) -2 */
    os_set_CSSELR(0);
    reg  = os_get_CCSIDR();
    line = 1 << ((reg & 0x7) + 4);

    ptr = buffer & ~(line - 1);

    while (ptr < buffer + size) {
        os_set_DCCIMVAC(ptr);
        ptr += line;
    }
    OS_DMB();
}

void k_l1dcache_invalidate_all(void)
{
    uint32_t l1d_CCSIDR;
    int32_t sets;
    int32_t ways;
    int32_t line;

    /* get cache info */
    os_set_CSSELR(0);
    l1d_CCSIDR = os_get_CCSIDR();

    // Fill number of sets  and number of ways from csid register  This walues are decremented by 1
    /* [12:3] Associativity: Indicates the associativity of cache - 1 */
    ways = (l1d_CCSIDR >> 3) & 0x3FF;
    /* [2:0] LineSize: Indicates the (log2 (number of words in cache line)) - 2 */
    line = l1d_CCSIDR & 0x7;

    // Invalidation all lines (all Sets in all ways)
    while (ways >= 0) {
        /* [27:13] NumSets: Indicates the number of sets in cache - 1 */
        sets = (l1d_CCSIDR >> 13) & 0x7FFF;
        while (sets >= 0) {
            /* Invalidate data or unified cache line by set/way. */
            os_set_DCISW((sets << (line + 4)) | (ways << 30));
            sets--;
        }
        ways--;
    }

    OS_DMB();
}

