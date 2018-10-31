/* Created by plibgen $Revision: 1.31 $ */

#ifndef _PCACHE_P32MZ2048EFM144_H
#define _PCACHE_P32MZ2048EFM144_H

/* Section 1 - Enumerate instances, define constants, VREGs */

#include <xc.h>
#include <stdbool.h>

#include "peripheral/peripheral_common_32bit.h"

/* Default definition used for all API dispatch functions */
#ifndef PLIB_INLINE_API
    #define PLIB_INLINE_API extern inline
#endif

/* Default definition used for all other functions */
#ifndef PLIB_INLINE
    #define PLIB_INLINE extern inline
#endif

typedef enum {

    PCACHE_ID_0 = _PCACHE_BASE_ADDRESS,
    PCACHE_NUMBER_OF_MODULES = 1

} PCACHE_MODULE_ID;

typedef enum {

    PLIB_PCACHE_PREFETCH_DISABLE = 0x00,
    PLIB_PCACHE_PREFETCH_ENABLE_CPU_INST = 0x01,
    PLIB_PCACHE_PREFETCH_ENABLE_CPU_INST_DATA = 0x02,
    PLIB_PCACHE_PREFETCH_ENABLE_ALL = 0x03

} PLIB_PCACHE_PREFETCH_ENABLE;

typedef enum {

    PLIB_PCACHE_DATA_ENABLE_NONE

} PLIB_PCACHE_DATA_ENABLE;

typedef enum {

    PCACHE_CACHE_TYPE_NONE

} PCACHE_CACHE_TYPE;

/* Section 2 - Feature variant inclusion */

#define PLIB_TEMPLATE PLIB_INLINE
#include "../templates/pcache_WaitState_MZ.h"
#include "../templates/pcache_PFMAddressWaitStateEnable_Unsupported.h"
#include "../templates/pcache_CachePerformanceCountersEnable_Unsupported.h"
#include "../templates/pcache_PrefetchEnable_MZ.h"
#include "../templates/pcache_DataCacheEnable_Unsupported.h"
#include "../templates/pcache_CacheEnable_Unsupported.h"
#include "../templates/pcache_FlashSECInt_Default.h"
#include "../templates/pcache_FlashDEDStatus_Default.h"
#include "../templates/pcache_FlashSECStatus_Default.h"
#include "../templates/pcache_FlashSECCount_Default.h"
#include "../templates/pcache_InvalidateOnPFMProgram_Unsupported.h"
#include "../templates/pcache_InvalidateCache_Unsupported.h"
#include "../templates/pcache_CacheLineSelect_Unsupported.h"
#include "../templates/pcache_CacheLineType_Unsupported.h"
#include "../templates/pcache_CacheLineLock_Unsupported.h"
#include "../templates/pcache_CacheLineValid_Unsupported.h"
#include "../templates/pcache_CacheLineAddr_Unsupported.h"
#include "../templates/pcache_CacheLineFlashType_Unsupported.h"
#include "../templates/pcache_CacheLineMask_Unsupported.h"
#include "../templates/pcache_Word_Unsupported.h"
#include "../templates/pcache_LeastRecentlyUsedState_Unsupported.h"
#include "../templates/pcache_CacheHit_Unsupported.h"
#include "../templates/pcache_CacheMiss_Unsupported.h"
#include "../templates/pcache_PrefetchAbort_Unsupported.h"

/* Section 3 - PLIB dispatch function definitions */

PLIB_INLINE_API bool PLIB_PCACHE_ExistsWaitState(PCACHE_MODULE_ID index)
{
     return PCACHE_ExistsWaitState_MZ(index);
}

PLIB_INLINE_API void PLIB_PCACHE_WaitStateSet(PCACHE_MODULE_ID index, uint32_t clocks)
{
     PCACHE_WaitStateSet_MZ(index, clocks);
}

PLIB_INLINE_API uint32_t PLIB_PCACHE_WaitStateGet(PCACHE_MODULE_ID index)
{
     return PCACHE_WaitStateGet_MZ(index);
}

PLIB_INLINE_API bool PLIB_PCACHE_ExistsPFMAddressWaitStateEnable(PCACHE_MODULE_ID index)
{
     return PCACHE_ExistsPFMAddressWaitStateEnable_Unsupported(index);
}

PLIB_INLINE_API void _PLIB_UNSUPPORTED PLIB_PCACHE_PFMAddressWaitStateEnable(PCACHE_MODULE_ID index)
{
     PCACHE_PFMAddressWaitStateEnable_Unsupported(index);
}

PLIB_INLINE_API void _PLIB_UNSUPPORTED PLIB_PCACHE_PFMAddressWaitStateDisable(PCACHE_MODULE_ID index)
{
     PCACHE_PFMAddressWaitStateDisable_Unsupported(index);
}

PLIB_INLINE_API bool _PLIB_UNSUPPORTED PLIB_PCACHE_PFMAddressWaitStateIsEnabled(PCACHE_MODULE_ID index)
{
     return PCACHE_PFMAddressWaitStateIsEnabled_Unsupported(index);
}

PLIB_INLINE_API bool PLIB_PCACHE_ExistsCachePerformanceCountersEnable(PCACHE_MODULE_ID index)
{
     return PCACHE_ExistsCachePerformanceCountersEnable_Unsupported(index);
}

PLIB_INLINE_API void _PLIB_UNSUPPORTED PLIB_PCACHE_CachePerformanceCountersEnable(PCACHE_MODULE_ID index)
{
     PCACHE_CachePerformanceCountersEnable_Unsupported(index);
}

PLIB_INLINE_API void _PLIB_UNSUPPORTED PLIB_PCACHE_CachePerformanceCountersDisable(PCACHE_MODULE_ID index)
{
     PCACHE_CachePerformanceCountersDisable_Unsupported(index);
}

PLIB_INLINE_API bool _PLIB_UNSUPPORTED PLIB_PCACHE_CachePerformanceCountersIsEnabled(PCACHE_MODULE_ID index)
{
     return PCACHE_CachePerformanceCountersIsEnabled_Unsupported(index);
}

PLIB_INLINE_API bool PLIB_PCACHE_ExistsPrefetchEnable(PCACHE_MODULE_ID index)
{
     return PCACHE_ExistsPrefetchEnable_MZ(index);
}

PLIB_INLINE_API void PLIB_PCACHE_PrefetchEnableSet(PCACHE_MODULE_ID index, PLIB_PCACHE_PREFETCH_ENABLE region)
{
     PCACHE_PrefetchEnableSet_MZ(index, region);
}

PLIB_INLINE_API PLIB_PCACHE_PREFETCH_ENABLE PLIB_PCACHE_PrefetchEnableGet(PCACHE_MODULE_ID index)
{
     return PCACHE_PrefetchEnableGet_MZ(index);
}

PLIB_INLINE_API bool PLIB_PCACHE_ExistsDataCacheEnable(PCACHE_MODULE_ID index)
{
     return PCACHE_ExistsDataCacheEnable_Unsupported(index);
}

PLIB_INLINE_API void _PLIB_UNSUPPORTED PLIB_PCACHE_DataCacheEnableSet(PCACHE_MODULE_ID index, PLIB_PCACHE_DATA_ENABLE dcache_en)
{
     PCACHE_DataCacheEnableSet_Unsupported(index, dcache_en);
}

PLIB_INLINE_API PLIB_PCACHE_DATA_ENABLE _PLIB_UNSUPPORTED PLIB_PCACHE_DataCacheEnableGet(PCACHE_MODULE_ID index)
{
     return PCACHE_DataCacheEnableGet_Unsupported(index);
}

PLIB_INLINE_API bool PLIB_PCACHE_ExistsCacheEnable(PCACHE_MODULE_ID index)
{
     return PCACHE_ExistsCacheEnable_Unsupported(index);
}

PLIB_INLINE_API void _PLIB_UNSUPPORTED PLIB_PCACHE_CacheEnable(PCACHE_MODULE_ID index, PCACHE_CACHE_TYPE cache_type)
{
     PCACHE_CacheEnable_Unsupported(index, cache_type);
}

PLIB_INLINE_API void _PLIB_UNSUPPORTED PLIB_PCACHE_CacheDisable(PCACHE_MODULE_ID index, PCACHE_CACHE_TYPE cache_type)
{
     PCACHE_CacheDisable_Unsupported(index, cache_type);
}

PLIB_INLINE_API bool _PLIB_UNSUPPORTED PLIB_PCACHE_CacheIsEnabled(PCACHE_MODULE_ID index, PCACHE_CACHE_TYPE cache_type)
{
     return PCACHE_CacheIsEnabled_Unsupported(index, cache_type);
}

PLIB_INLINE_API bool PLIB_PCACHE_ExistsFlashSECInt(PCACHE_MODULE_ID index)
{
     return PCACHE_ExistsFlashSECInt_Default(index);
}

PLIB_INLINE_API void PLIB_PCACHE_FlashSECIntEnable(PCACHE_MODULE_ID index)
{
     PCACHE_FlashSECIntEnable_Default(index);
}

PLIB_INLINE_API void PLIB_PCACHE_FlashSECIntDisable(PCACHE_MODULE_ID index)
{
     PCACHE_FlashSECIntDisable_Default(index);
}

PLIB_INLINE_API bool PLIB_PCACHE_ExistsFlashDEDStatus(PCACHE_MODULE_ID index)
{
     return PCACHE_ExistsFlashDEDStatus_Default(index);
}

PLIB_INLINE_API bool PLIB_PCACHE_FlashDEDStatusGet(PCACHE_MODULE_ID index)
{
     return PCACHE_FlashDEDStatusGet_Default(index);
}

PLIB_INLINE_API void PLIB_PCACHE_FlashDEDStatusClear(PCACHE_MODULE_ID index)
{
     PCACHE_FlashDEDStatusClear_Default(index);
}

PLIB_INLINE_API bool PLIB_PCACHE_ExistsFlashSECStatus(PCACHE_MODULE_ID index)
{
     return PCACHE_ExistsFlashSECStatus_Default(index);
}

PLIB_INLINE_API bool PLIB_PCACHE_FlashSECStatusGet(PCACHE_MODULE_ID index)
{
     return PCACHE_FlashSECStatusGet_Default(index);
}

PLIB_INLINE_API void PLIB_PCACHE_FlashSECStatusSet(PCACHE_MODULE_ID index)
{
     PCACHE_FlashSECStatusSet_Default(index);
}

PLIB_INLINE_API void PLIB_PCACHE_FlashSECStatusClear(PCACHE_MODULE_ID index)
{
     PCACHE_FlashSECStatusClear_Default(index);
}

PLIB_INLINE_API bool PLIB_PCACHE_ExistsFlashSECCount(PCACHE_MODULE_ID index)
{
     return PCACHE_ExistsFlashSECCount_Default(index);
}

PLIB_INLINE_API void PLIB_PCACHE_FlashSECCountSet(PCACHE_MODULE_ID index, uint8_t count)
{
     PCACHE_FlashSECCountSet_Default(index, count);
}

PLIB_INLINE_API uint8_t PLIB_PCACHE_FlashSECCountGet(PCACHE_MODULE_ID index)
{
     return PCACHE_FlashSECCountGet_Default(index);
}

PLIB_INLINE_API bool PLIB_PCACHE_ExistsInvalidateOnPFMProgram(PCACHE_MODULE_ID index)
{
     return PCACHE_ExistsInvalidateOnPFMProgram_Unsupported(index);
}

PLIB_INLINE_API void _PLIB_UNSUPPORTED PLIB_PCACHE_InvalidateOnPFMProgramAll(PCACHE_MODULE_ID index)
{
     PCACHE_InvalidateOnPFMProgramAll_Unsupported(index);
}

PLIB_INLINE_API void _PLIB_UNSUPPORTED PLIB_PCACHE_InvalidateOnPFMProgramUnlocked(PCACHE_MODULE_ID index)
{
     PCACHE_InvalidateOnPFMProgramUnlocked_Unsupported(index);
}

PLIB_INLINE_API bool PLIB_PCACHE_ExistsInvalidateCache(PCACHE_MODULE_ID index)
{
     return PCACHE_ExistsInvalidateCache_Unsupported(index);
}

PLIB_INLINE_API void _PLIB_UNSUPPORTED PLIB_PCACHE_ForceInvalidateCacheEnable(PCACHE_MODULE_ID index, PCACHE_CACHE_TYPE cache_type)
{
     PCACHE_ForceInvalidateCacheEnable_Unsupported(index, cache_type);
}

PLIB_INLINE_API bool _PLIB_UNSUPPORTED PLIB_PCACHE_ForceInvalidateCacheIsEnabled(PCACHE_MODULE_ID index, PCACHE_CACHE_TYPE cache_type)
{
     return PCACHE_ForceInvalidateCacheIsEnabled_Unsupported(index, cache_type);
}

PLIB_INLINE_API void _PLIB_UNSUPPORTED PLIB_PCACHE_AutoInvalidateCacheEnable(PCACHE_MODULE_ID index, PCACHE_CACHE_TYPE cache_type)
{
     PCACHE_AutoInvalidateCacheEnable_Unsupported(index, cache_type);
}

PLIB_INLINE_API void _PLIB_UNSUPPORTED PLIB_PCACHE_AutoInvalidateCacheDisable(PCACHE_MODULE_ID index, PCACHE_CACHE_TYPE cache_type)
{
     PCACHE_AutoInvalidateCacheDisable_Unsupported(index, cache_type);
}

PLIB_INLINE_API bool _PLIB_UNSUPPORTED PLIB_PCACHE_AutoInvalidateCacheIsEnabled(PCACHE_MODULE_ID index, PCACHE_CACHE_TYPE cache_type)
{
     return PCACHE_AutoInvalidateCacheIsEnabled_Unsupported(index, cache_type);
}

PLIB_INLINE_API bool PLIB_PCACHE_ExistsCacheLine(PCACHE_MODULE_ID index)
{
     return PCACHE_ExistsCacheLine_Unsupported(index);
}

PLIB_INLINE_API void _PLIB_UNSUPPORTED PLIB_PCACHE_CacheLineSelect(PCACHE_MODULE_ID index, uint32_t cache_line)
{
     PCACHE_CacheLineSelect_Unsupported(index, cache_line);
}

PLIB_INLINE_API void _PLIB_UNSUPPORTED PLIB_PCACHE_CacheLineDeselect(PCACHE_MODULE_ID index, uint32_t cache_line)
{
     PCACHE_CacheLineDeselect_Unsupported(index, cache_line);
}

PLIB_INLINE_API void _PLIB_UNSUPPORTED PLIB_PCACHE_CacheLineData(PCACHE_MODULE_ID index)
{
     PCACHE_CacheLineData_Unsupported(index);
}

PLIB_INLINE_API void _PLIB_UNSUPPORTED PLIB_PCACHE_CacheLineInst(PCACHE_MODULE_ID index)
{
     PCACHE_CacheLineInst_Unsupported(index);
}

PLIB_INLINE_API bool _PLIB_UNSUPPORTED PLIB_PCACHE_CacheLineIsInst(PCACHE_MODULE_ID index)
{
     return PCACHE_CacheLineIsInst_Unsupported(index);
}

PLIB_INLINE_API bool PLIB_PCACHE_ExistsCacheLineType(PCACHE_MODULE_ID index)
{
     return PCACHE_ExistsCacheLineType_Unsupported(index);
}

PLIB_INLINE_API void _PLIB_UNSUPPORTED PLIB_PCACHE_CacheLineLock(PCACHE_MODULE_ID index)
{
     PCACHE_CacheLineLock_Unsupported(index);
}

PLIB_INLINE_API void _PLIB_UNSUPPORTED PLIB_PCACHE_CacheLineUnlock(PCACHE_MODULE_ID index)
{
     PCACHE_CacheLineUnlock_Unsupported(index);
}

PLIB_INLINE_API bool _PLIB_UNSUPPORTED PLIB_PCACHE_CacheLineIsLocked(PCACHE_MODULE_ID index)
{
     return PCACHE_CacheLineIsLocked_Unsupported(index);
}

PLIB_INLINE_API bool PLIB_PCACHE_ExistsCacheLineLock(PCACHE_MODULE_ID index)
{
     return PCACHE_ExistsCacheLineLock_Unsupported(index);
}

PLIB_INLINE_API void _PLIB_UNSUPPORTED PLIB_PCACHE_CacheLineValid(PCACHE_MODULE_ID index)
{
     PCACHE_CacheLineValid_Unsupported(index);
}

PLIB_INLINE_API void _PLIB_UNSUPPORTED PLIB_PCACHE_CacheLineInvalid(PCACHE_MODULE_ID index)
{
     PCACHE_CacheLineInvalid_Unsupported(index);
}

PLIB_INLINE_API bool _PLIB_UNSUPPORTED PLIB_PCACHE_CacheLineIsValid(PCACHE_MODULE_ID index)
{
     return PCACHE_CacheLineIsValid_Unsupported(index);
}

PLIB_INLINE_API bool PLIB_PCACHE_ExistsCacheLineValid(PCACHE_MODULE_ID index)
{
     return PCACHE_ExistsCacheLineValid_Unsupported(index);
}

PLIB_INLINE_API void _PLIB_UNSUPPORTED PLIB_PCACHE_CacheLineAddrSet(PCACHE_MODULE_ID index, uint32_t addr)
{
     PCACHE_CacheLineAddrSet_Unsupported(index, addr);
}

PLIB_INLINE_API uint32_t _PLIB_UNSUPPORTED PLIB_PCACHE_CacheLineAddrGet(PCACHE_MODULE_ID index)
{
     return PCACHE_CacheLineAddrGet_Unsupported(index);
}

PLIB_INLINE_API bool PLIB_PCACHE_ExistsCacheLineAddr(PCACHE_MODULE_ID index)
{
     return PCACHE_ExistsCacheLineAddr_Unsupported(index);
}

PLIB_INLINE_API void _PLIB_UNSUPPORTED PLIB_PCACHE_CacheLineFlashTypeBoot(PCACHE_MODULE_ID index)
{
     PCACHE_CacheLineFlashTypeBoot_Unsupported(index);
}

PLIB_INLINE_API void _PLIB_UNSUPPORTED PLIB_PCACHE_CacheLineFlashTypeInst(PCACHE_MODULE_ID index)
{
     PCACHE_CacheLineFlashTypeInst_Unsupported(index);
}

PLIB_INLINE_API bool _PLIB_UNSUPPORTED PLIB_PCACHE_CacheLineFlashTypeIsInst(PCACHE_MODULE_ID index)
{
     return PCACHE_CacheLineFlashTypeIsInst_Unsupported(index);
}

PLIB_INLINE_API bool PLIB_PCACHE_ExistsCacheLineFlashType(PCACHE_MODULE_ID index)
{
     return PCACHE_ExistsCacheLineFlashType_Unsupported(index);
}

PLIB_INLINE_API void _PLIB_UNSUPPORTED PLIB_PCACHE_CacheLineMaskSet(PCACHE_MODULE_ID index, uint32_t mask)
{
     PCACHE_CacheLineMaskSet_Unsupported(index, mask);
}

PLIB_INLINE_API uint32_t _PLIB_UNSUPPORTED PLIB_PCACHE_CacheLineMaskGet(PCACHE_MODULE_ID index)
{
     return PCACHE_CacheLineMaskGet_Unsupported(index);
}

PLIB_INLINE_API bool PLIB_PCACHE_ExistsCacheLineMask(PCACHE_MODULE_ID index)
{
     return PCACHE_ExistsCacheLineMask_Unsupported(index);
}

PLIB_INLINE_API uint32_t _PLIB_UNSUPPORTED PLIB_PCACHE_WordRead(PCACHE_MODULE_ID index, uint32_t word)
{
     return PCACHE_WordRead_Unsupported(index, word);
}

PLIB_INLINE_API void _PLIB_UNSUPPORTED PLIB_PCACHE_WordWrite(PCACHE_MODULE_ID index, uint32_t word, uint32_t data)
{
     PCACHE_WordWrite_Unsupported(index, word, data);
}

PLIB_INLINE_API bool PLIB_PCACHE_ExistsWord(PCACHE_MODULE_ID index)
{
     return PCACHE_ExistsWord_Unsupported(index);
}

PLIB_INLINE_API uint32_t _PLIB_UNSUPPORTED PLIB_PCACHE_LeastRecentlyUsedStateRead(PCACHE_MODULE_ID index)
{
     return PCACHE_LeastRecentlyUsedStateRead_Unsupported(index);
}

PLIB_INLINE_API bool PLIB_PCACHE_ExistsLeastRecentlyUsedState(PCACHE_MODULE_ID index)
{
     return PCACHE_ExistsLeastRecentlyUsedState_Unsupported(index);
}

PLIB_INLINE_API uint32_t _PLIB_UNSUPPORTED PLIB_PCACHE_CacheHitRead(PCACHE_MODULE_ID index)
{
     return PCACHE_CacheHitRead_Unsupported(index);
}

PLIB_INLINE_API void _PLIB_UNSUPPORTED PLIB_PCACHE_CacheHitWrite(PCACHE_MODULE_ID index, uint32_t data)
{
     PCACHE_CacheHitWrite_Unsupported(index, data);
}

PLIB_INLINE_API bool PLIB_PCACHE_ExistsCacheHit(PCACHE_MODULE_ID index)
{
     return PCACHE_ExistsCacheHit_Unsupported(index);
}

PLIB_INLINE_API uint32_t _PLIB_UNSUPPORTED PLIB_PCACHE_CacheMissRead(PCACHE_MODULE_ID index)
{
     return PCACHE_CacheMissRead_Unsupported(index);
}

PLIB_INLINE_API void _PLIB_UNSUPPORTED PLIB_PCACHE_CacheMissWrite(PCACHE_MODULE_ID index, uint32_t data)
{
     PCACHE_CacheMissWrite_Unsupported(index, data);
}

PLIB_INLINE_API bool PLIB_PCACHE_ExistsCacheMiss(PCACHE_MODULE_ID index)
{
     return PCACHE_ExistsCacheMiss_Unsupported(index);
}

PLIB_INLINE_API uint32_t _PLIB_UNSUPPORTED PLIB_PCACHE_PrefetchAbortRead(PCACHE_MODULE_ID index)
{
     return PCACHE_PrefetchAbortRead_Unsupported(index);
}

PLIB_INLINE_API void _PLIB_UNSUPPORTED PLIB_PCACHE_PrefetchAbortWrite(PCACHE_MODULE_ID index, uint32_t data)
{
     PCACHE_PrefetchAbortWrite_Unsupported(index, data);
}

PLIB_INLINE_API bool PLIB_PCACHE_ExistsPrefetchAbort(PCACHE_MODULE_ID index)
{
     return PCACHE_ExistsPrefetchAbort_Unsupported(index);
}

#endif
