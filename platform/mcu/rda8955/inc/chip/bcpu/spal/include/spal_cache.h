//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
/// @file spal_cache.h
///
/// Header for cache flush.
///
/// @date 12.12.2016
/// @author deqikong
//-----------------------------------------------------------------------------

#ifndef SPAL_CACHE_H
#define SPAL_CACHE_H

// include
#include "cs_types.h"
#include "global_macros.h"
#define BCPU_CACHE_LINE_NB_BIT (4)
#define BCPU_CACHE_LINE_SIZE    EXP2(BCPU_CACHE_LINE_NB_BIT)
#define BCPU_CACHE_LINE_MASK  (BCPU_CACHE_LINE_SIZE-1)

#define BCPU_CACHE_CODE_POS     (0)
#define BCPU_CACHE_CODE_NB_BIT (4)
#define BCPU_CACHE_CODE_SIZE    EXP2(BCPU_CACHE_CODE_NB_BIT)
#define BCPU_CACHE_CODE_MASK  (BCPU_CACHE_CODE_SIZE-1)

#define BCPU_CACHE_LINE_INDEX_POS  (BCPU_CACHE_CODE_POS+BCPU_CACHE_CODE_NB_BIT)
#define BCPU_CACHE_LINE_INDEX_NB_BIT (4)
#define BCPU_CACHE_LINE_INDEX_SIZE  EXP2(BCPU_CACHE_LINE_INDEX_NB_BIT)
#define BCPU_CACHE_LINE_INDEX_MASK ((BCPU_CACHE_LINE_INDEX_SIZE - 1) << BCPU_CACHE_LINE_INDEX_POS)
#define BCPU_CACHE_LINE_INDEX(addr)   (((addr)&BCPU_CACHE_LINE_INDEX_MASK)>>BCPU_CACHE_LINE_INDEX_POS)

#define BCPU_CACHE_WAY_POS  (BCPU_CACHE_LINE_INDEX_POS+BCPU_CACHE_LINE_INDEX_NB_BIT)
#define BCPU_CACHE_WAY_NB_BIT (2)
#define BCPU_CACHE_WAY_SIZE  EXP2(BCPU_CACHE_WAY_NB_BIT)
#define BCPU_CACHE_WAY_MASK ((BCPU_CACHE_WAY_SIZE - 1) << BCPU_CACHE_WAY_POS)
#define BCPU_CACHE_WAY(addr)   (((addr)&BCPU_CACHE_WAY_MASK)>>BCPU_CACHE_WAY_POS)

#define BCPU_CACHE_SIZE   EXP2(BCPU_CACHE_LINE_NB_BIT+BCPU_CACHE_LINE_INDEX_NB_BIT)


// =============================================================================
// spal_DcacheFlushAddrRange
// -----------------------------------------------------------------------------
/// BOOT flush DCache by address 
/// @param startAddr define the start address
/// @param endAddr define the end address
/// @param invalid used to invalid the cache or not(FALSE by default)
// =============================================================================
VOID spal_DcacheFlushAddrRange(UINT32 startAddr, UINT32 endAddr, BOOL invalid);


// =============================================================================
// spal_FlushDCache
// -----------------------------------------------------------------------------
///  flush All DCache 
/// @param invalid used to invalid the cache or not(FALSE by default)
// =============================================================================
VOID spal_FlushDCache(BOOL invalid);

#endif

