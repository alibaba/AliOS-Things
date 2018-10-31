/*******************************************************************************
  Device Control System Service Implementation

  Company:
    Microchip Technology Inc.

  File Name:
    sys_devcon.c

  Summary:
    Device Control System Service implementation.

  Description:
    The DEVCON system service provides a simple interface to manage the Device 
    Control module on Microchip microcontrollers. This file Implements the core
    interface routines for the Device Control system service. While building 
    the system service from source, ALWAYS include this file in the build.
*******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright 2017 Microchip Technology Incorporated and its subsidiaries.

Permission is hereby granted, free of charge, to any person obtaining a copy of 
this software and associated documentation files (the "Software"), to deal in 
the Software without restriction, including without limitation the rights to 
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies 
of the Software, and to permit persons to whom the Software is furnished to do 
so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all 
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE 
SOFTWARE
*******************************************************************************/
//DOM-IGNORE-END


// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <xc.h>
#include <cp0defs.h>
#include "sys_devcon_local.h"
#include "peripheral/int/plib_int.h"
#include "peripheral/pcache/plib_pcache.h"

// *****************************************************************************
// *****************************************************************************
// Section: File Scope or Global Constants
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
// *****************************************************************************
// Section: Variable Definitions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
// *****************************************************************************
// Section: SYS DEVCON Client Setup Routines
// *****************************************************************************
// *****************************************************************************


// *****************************************************************************
/* Function:
    void SYS_DEVCON_PerformanceConfig( unsigned int sysclk )

  Summary:
    Configures the PFM wait states and prefetch (cache) module for maximum 
    performance.

  Description:
    This function configures the PFM wait states and prefetch (cache) module 
    for maximum performance.

  Remarks:
    None.
*/

void __attribute__((nomips16)) SYS_DEVCON_PerformanceConfig( unsigned int sysclk )
{
    bool int_flag = false;
    bool ecc;

    /* Set the PFM wait states based on the system clock and ECC setting */
    #if defined(PLIB_PCACHE_ExistsWaitState)    
    if (PLIB_PCACHE_ExistsWaitState(PCACHE_ID_0))
    {
        int ws; /* number of wait states */

        /* Is ECC enabled? */
        /* TODO: replace register read with plib when available */
        ecc = (((CFGCON & 0x00000030) >> 4) < 2) ? true : false;
        if (sysclk <= (ecc ? 60000000 : 74000000))
            ws = 0;
        else if (sysclk <= (ecc ? 120000000 : 140000000))
            ws = 1;
        else if (sysclk <= 200000000)
            ws = 2;
        else
            ws = 4;

        /* Interrupts must be disabled when changing wait states */
        int_flag = (bool)(PLIB_INT_GetStateAndDisable( INT_ID_0 ) & 0x01);

        PLIB_PCACHE_WaitStateSet(PCACHE_ID_0, ws);

        if (int_flag)
        {
            PLIB_INT_Enable(INT_ID_0);
            int_flag = false;
        }
    }
    #endif // defined(PLIB_PCACHE_ExistsWaitState)
    /* Interrupts must be disabled when enabling the Prefetch Cache Module */
    int_flag = (bool)(PLIB_INT_GetStateAndDisable( INT_ID_0 ) & 0x01);

    /* Enable Prefetch Cache Module */
    #if defined(PLIB_PCACHE_ExistsPrefetchEnable)
    if (PLIB_PCACHE_ExistsPrefetchEnable(PCACHE_ID_0))
    {
        PLIB_PCACHE_PrefetchEnableSet(PCACHE_ID_0, PLIB_PCACHE_PREFETCH_ENABLE_ALL);
    }
    #endif
    
    if (int_flag)
    {
        PLIB_INT_Enable(INT_ID_0);
    }
}


// *****************************************************************************
/* Function:
    void SYS_DEVCON_CacheInit(SYS_CACHE_COHERENCY cacheCoherency)

  Summary:
    Initializes the L1 cache. 

  Description:
    Initializes both instruction and data caches. Invalidates all entries and
    zeroes all tags. Sets coherency attribute for kseg0.

  Remarks:
    Use with caution. Invalidates all cache lines without writing data
    back to memory. Any dirty data in the cache will be lost.
*/

void SYS_DEVCON_CacheInit(SYS_CACHE_COHERENCY cacheCoherency)
{
    _pic32_init_cache(cacheCoherency);
}


// *****************************************************************************
/* Function:
    void SYS_DEVCON_CacheFlush(void)

  Summary:
    Flushes the L1 cache. 

  Description:
    Flushes both instruction and data caches. Invalidate entire instruction
    cache; write back and invalidate entire data cache.

  Remarks:
    Simplest way to sychnronize caches with memory, but not necessarily
    the most efficient.
*/

void SYS_DEVCON_CacheFlush(void)
{
    _pic32_flush_cache();
}


// *****************************************************************************
/* Function:
    void SYS_DEVCON_DataCacheFlush(void)

  Summary:
    Flushes the L1 data cache. 

  Description:
    Write back and invalidate the entire data cache.

  Remarks:
    None.
*/

void SYS_DEVCON_DataCacheFlush(void)
{
    _pic32_flush_dcache();
}


// *****************************************************************************
/* Function:
    void SYS_DEVCON_InstructionCacheFlush(void)

  Summary:
    Flushes (invalidates) the L1 instruction cache.

  Description:
    Invalidate entire instruction cache.

  Remarks:
    None.
*/

void SYS_DEVCON_InstructionCacheFlush(void)
{
    _pic32_flush_icache();
}


// *****************************************************************************
/* Function:
    void SYS_DEVCON_CacheClean(uint32_t addr, size_t len)

  Summary:
    Write back and invalidate an address range in either cache.

  Description:
    Write back (data) and invalidate (data and address) an address range 
    in either cache.

  Remarks:
    None.
*/

void SYS_DEVCON_CacheClean(uint32_t addr, size_t len)
{
    _pic32_clean_cache(addr, len);
}


// *****************************************************************************
/* Function:
    void SYS_DEVCON_DataCacheClean(uint32_t addr, size_t len)

  Summary:
    Write back and invalidate an address range in the data cache.

  Description:
    Write back and invalidate an address range in the data cache.

  Remarks:
    None.
*/

void SYS_DEVCON_DataCacheClean(uint32_t addr, size_t len)
{
    _pic32_clean_dcache(addr, len);
}


// *****************************************************************************
/* Function:
    void SYS_DEVCON_DataCacheInvalidate(uint32_t addr, size_t len)

  Summary:
    Invalidate an address range in the data cache.

  Description:
    Invalidate an address range in the data cache.

  Remarks:
    None.
*/

void SYS_DEVCON_DataCacheInvalidate(uint32_t addr, size_t len)
{
    _pic32_clean_dcache_nowrite(addr, len);
}


// *****************************************************************************
/* Function:
    void SYS_DEVCON_InstructionCacheInvalidate(uint32_t addr, size_t len)

  Summary:
    Invalidate an address range in the instruction cache.

  Description:
    Invalidate an address range in the instruction cache.

  Remarks:
    None.
*/

void SYS_DEVCON_InstructionCacheInvalidate(uint32_t addr, size_t len)
{
    _pic32_clean_icache(addr, len);
}


// *****************************************************************************
/* Function:
    void SYS_DEVCON_InstructionCacheLock(uint32_t addr, size_t len)

  Summary:
    Fetch and lock a block of instructions in the instruction cache.

  Description:
    Fetch and lock a block of instructions in the instruction cache.

  Remarks:
    Global flush functions will invalidate and unlock any cache lines
    locked with this function.
*/

void SYS_DEVCON_InstructionCacheLock(uint32_t addr, size_t len)
{
    _pic32_lock_icache(addr, len);
}


// *****************************************************************************
/* Function:
    void SYS_DEVCON_DataCacheLock(uint32_t addr, size_t len)

  Summary:
    Fetch and lock a block of data in the data cache.

  Description:
    Fetch and lock a block of data in the data cache.

  Remarks:
    Global flush functions will invalidate and unlock any cache lines
    locked with this function.
*/

void SYS_DEVCON_DataCacheLock(uint32_t addr, size_t len)
{
    _pic32_lock_dcache(addr, len);
}


// *****************************************************************************
/* Function:
    void SYS_DEVCON_CacheSync(uint32_t addr, size_t len)

  Summary:
    Synchronize the instruction and data caches.

  Description:
    Synchronize the instruction and data caches. Used when modifying
    the instruction stream (breakpoints, self-modifying code, relocating
    executable code to RAM). Flushes an address range from the data cache 
    and invalidates that same range from the instruction cache.

  Remarks:
    None.
*/

void SYS_DEVCON_CacheSync(uint32_t addr, size_t len)
{
    _pic32_sync_icache(addr, len);
}


// *****************************************************************************
/* Function:
    void SYS_DEVCON_CacheCoherencySet(SYS_CACHE_COHERENCY cacheCoherency)

  Summary:
    Set the cache coherency attribute for kseg0.

  Description:
    Set the cache coherency attribute for kseg0.

  Remarks:
    Use with caution. May cause unpredictable behavior in a running
    system.
*/

void SYS_DEVCON_CacheCoherencySet(SYS_CACHE_COHERENCY cacheCoherency)
{
    uint32_t cp0;
    /*LDRA_INSPECTED 496 S */ /*Deviation Reference: MH-4490*/
    cp0 = _mfc0(16, 0);
    cp0 &= ~0x07;
    cp0 |= cacheCoherency;
    /*LDRA_INSPECTED 496 S */ /*Deviation Reference: MH-4491*/
    _mtc0(16, 0, cp0);
}


// *****************************************************************************
/* Function:
    SYS_CACHE_COHERENCY SYS_DEVCON_CacheCoherencyGet(void)

  Summary:
    Returns the current cache coherency attribute for kseg0.

  Description:
    Returns the current cache coherency attribute for kseg0.

  Remarks:
    None.
*/

SYS_CACHE_COHERENCY SYS_DEVCON_CacheCoherencyGet(void)
{
    /*LDRA_INSPECTED 496 S */ /*Deviation Reference: MH-4492*/
    return _mfc0(16, 0) & 0x03;
}


// *****************************************************************************
/* Function:
    size_t SYS_DEVCON_DataCacheAssociativityGet(void)

  Summary:
    Returns the number of ways in the data cache.

  Description:
    Returns the number of ways in the data cache.

  Remarks:
    None.
*/

size_t SYS_DEVCON_DataCacheAssociativityGet(void)
{
    return _pic32_get_dcache_associativity();
}


// *****************************************************************************
/* Function:
    size_t SYS_DEVCON_InstructionCacheAssociativityGet(void)

  Summary:
    Returns the number of ways in the instruction cache.

  Description:
    Returns the number of ways in the instruction cache.

  Remarks:
    None.
*/

size_t SYS_DEVCON_InstructionCacheAssociativityGet(void)
{
    return _pic32_get_icache_associativity();
}


// *****************************************************************************
/* Function:
    size_t SYS_DEVCON_DataCacheLineSizeGet(void)

  Summary:
    Returns the data cache line size.

  Description:
    Returns the data cache line size.

  Remarks:
    None.
*/

size_t SYS_DEVCON_DataCacheLineSizeGet(void)
{
    return _pic32_get_dcache_linesize();
}


// *****************************************************************************
/* Function:
    size_t SYS_DEVCON_InstructionCacheLineSizeGet(void)

  Summary:
    Returns the instruction cache line size.

  Description:
    Returns the instruction cache line size.

  Remarks:
    None.
*/

size_t SYS_DEVCON_InstructionCacheLineSizeGet(void)
{
    return _pic32_get_icache_linesize();
}


// *****************************************************************************
/* Function:
    size_t SYS_DEVCON_DataCacheLinesPerWayGet(void)

  Summary:
    Returns the number of lines per way in the data cache.

  Description:
    Returns the number of lines per way in the data cache.

  Remarks:
    None.
*/

size_t SYS_DEVCON_DataCacheLinesPerWayGet(void)
{
    return _pic32_get_dcache_lines_per_way();
}


// *****************************************************************************
/* Function:
    size_t SYS_DEVCON_InstructionCacheLinesPerWayGet(void)

  Summary:
    Returns the number of lines per way in the instruction cache.

  Description:
    Returns the number of lines per way in the instruction cache.

  Remarks:
    None.
*/

size_t SYS_DEVCON_InstructionCacheLinesPerWayGet(void)
{
    return _pic32_get_icache_lines_per_way();
}


// *****************************************************************************
/* Function:
    size_t SYS_DEVCON_DataCacheSizeGet(void)

  Summary:
    Returns the total number of bytes in the data cache.

  Description:
    Returns the total number of bytes in the data cache.

  Remarks:
    None.
*/

size_t SYS_DEVCON_DataCacheSizeGet(void)
{
    return _pic32_get_dcache_size();
}


// *****************************************************************************
/* Function:
    size_t SYS_DEVCON_InstructionCacheSizeGet(void)

  Summary:
    Returns the total number of bytes in the instruction cache.

  Description:
    Returns the total number of bytes in the instruction cache.

  Remarks:
    None.
*/

size_t SYS_DEVCON_InstructionCacheSizeGet(void)
{
    return _pic32_get_icache_size();
}

/*******************************************************************************
 End of File
*/

