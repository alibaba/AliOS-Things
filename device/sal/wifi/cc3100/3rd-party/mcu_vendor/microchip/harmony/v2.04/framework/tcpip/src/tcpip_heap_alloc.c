/*******************************************************************************
  TCPIP Heap Allocation Manager

  Summary:
    
  Description:
*******************************************************************************/

/*******************************************************************************
File Name:  tcpip_heap_alloc.c
Copyright © 2012 released Microchip Technology Inc.  All rights
reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED “AS IS” WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
*******************************************************************************/

#include <string.h>
#include <stdlib.h>

#if !defined(__PIC32C__)
#include <sys/kmem.h>
#endif


#include "tcpip/src/tcpip_private.h"



// cache helpers
#if defined(__PIC32MZ__)
// flushes a data cache line/address
static __inline__ void __attribute__((always_inline)) _TCPIP_HEAP_DataLineFlush(void* address)
{   // issue a hit-writeback invalidate order
    __asm__ __volatile__ ("cache 0x15, 0(%0)" ::"r"(address));
}


// invalidates a range of addresses from the cache
static void _TCPIP_HEAP_DataCacheInvalidate(const void* address, size_t nBytes)
{
    int ix;

    uint8_t*    pLine = (uint8_t*)((uint32_t)address & 0xfffffff0); // start on an even cache line
    nBytes += (uint8_t*)address - pLine;
    int nLines = (nBytes + 15) / 16;   // round up the number of taken cache lines 

    for(ix = 0; ix < nLines; ix ++)
    {
        _TCPIP_HEAP_DataLineFlush(pLine);
        pLine += 16;
    }

    __asm__ __volatile__ ("sync");
}


const void* _TCPIP_HEAP_BufferMapNonCached(const void* buffer, size_t buffSize)
{
    if(IS_KVA0(buffer))
    {   // map in K1
        _TCPIP_HEAP_DataCacheInvalidate(buffer, buffSize);
        buffer = (const void*)KVA0_TO_KVA1(buffer);
    }

    return buffer;
}

const void* _TCPIP_HEAP_PointerMapCached(const void* ptr)
{
    if(IS_KVA1(ptr))
    {   // map in K1
        ptr = (const void*)KVA1_TO_KVA0(ptr);
    }

    return ptr;
}

#else
const void* _TCPIP_HEAP_BufferMapNonCached(const void* buffer, size_t buffSize)
{
    return buffer;
}
const void* _TCPIP_HEAP_PointerMapCached(const void* ptr)
{
    return ptr;
}

#endif  // defined(__PIC32MZ__)


#if !defined(TCPIP_STACK_DRAM_DEBUG_ENABLE) 

// functions needed when debugging is not enabled

TCPIP_STACK_HEAP_HANDLE TCPIP_HEAP_Create(const TCPIP_STACK_HEAP_CONFIG* initData, TCPIP_STACK_HEAP_RES* pRes)
{
    if(initData != 0)
    {
        switch (initData->heapType)
        {
#if defined (TCPIP_STACK_USE_INTERNAL_HEAP)
            case TCPIP_STACK_HEAP_TYPE_INTERNAL_HEAP:
                return TCPIP_HEAP_CreateInternal((const TCPIP_STACK_HEAP_INTERNAL_CONFIG*)initData, pRes);
#endif  // defined (TCPIP_STACK_USE_INTERNAL_HEAP)

#if defined (TCPIP_STACK_USE_EXTERNAL_HEAP)
            case TCPIP_STACK_HEAP_TYPE_EXTERNAL_HEAP:
                return TCPIP_HEAP_CreateExternal((const TCPIP_STACK_HEAP_EXTERNAL_CONFIG*)initData, pRes);
#endif  // defined (TCPIP_STACK_USE_EXTERNAL_HEAP)

#if defined (TCPIP_STACK_USE_INTERNAL_HEAP_POOL)
            case TCPIP_STACK_HEAP_TYPE_INTERNAL_HEAP_POOL:
                return TCPIP_HEAP_CreateInternalPool((const TCPIP_STACK_HEAP_POOL_CONFIG*)initData, pRes);
#endif  // defined (TCPIP_STACK_USE_INTERNAL_HEAP_POOL)

            default:
                break;
        }
    }

    return 0;
}

// functions needed when not inlined
void* TCPIP_HEAP_MallocOutline(TCPIP_STACK_HEAP_HANDLE h, size_t nBytes)
{
    return (*((TCPIP_HEAP_OBJECT*)h)->TCPIP_HEAP_Malloc)(h, nBytes);
}

void* TCPIP_HEAP_CallocOutline(TCPIP_STACK_HEAP_HANDLE h, size_t nElems, size_t elemSize)
{
    return (*((TCPIP_HEAP_OBJECT*)h)->TCPIP_HEAP_Calloc)(h, nElems, elemSize);
}

size_t TCPIP_HEAP_FreeOutline(TCPIP_STACK_HEAP_HANDLE h, const void* ptr)
{
    return (*((TCPIP_HEAP_OBJECT*)h)->TCPIP_HEAP_Free)(h, ptr);
}

#else   // defined(TCPIP_STACK_DRAM_DEBUG_ENABLE) 
// debug functionality

#if defined(TCPIP_STACK_DRAM_TRACE_ENABLE) 
    #define _TCPIP_STACK_DRAM_TRACE_ENABLE
    #define _TCPIP_STACK_DRAM_TRACE_NDP_COLLAPSE    1       // collapse NDP info into IPv6 since NDP allocated packets are freed by IPv6
#else
    #undef  _TCPIP_STACK_DRAM_TRACE_ENABLE
#endif

#if defined(TCPIP_STACK_DRAM_DIST_ENABLE) 
    #define _TCPIP_STACK_DRAM_DIST_ENABLE

    // distribution block sizes that are monitored
    // Note: there's no way (yet) of allowing the user to specify the requested distribution
    static const int _tcpip_heap_dist_sizes[] = 
    {
        0,      32,     64,     128,    256,    512,    640,    768,
        896,    1024,   1152,   1280,   1408,   1536,   1792,   2000,
        10000
    };

#else
    #undef  _TCPIP_STACK_DRAM_DIST_ENABLE
#endif



typedef struct
{
    TCPIP_STACK_HEAP_HANDLE     heapH;          // heap handle; 0 means slot is free
    TCPIP_STACK_HEAP_TYPE       heapType;       // type of heap
    TCPIP_STACK_HEAP_FLAGS      heapFlags;      // creation flags
    TCPIP_HEAP_NO_MEM_HANDLER   noMemHandler;   // out of memory handler for this heap
#if defined(_TCPIP_STACK_DRAM_TRACE_ENABLE)
    TCPIP_HEAP_TRACE_ENTRY      _heapTraceTbl[TCPIP_STACK_DRAM_TRACE_SLOTS];
#endif
#if defined(_TCPIP_STACK_DRAM_DIST_ENABLE)
    TCPIP_HEAP_DIST_ENTRY _tcpip_heap_dist_array[sizeof(_tcpip_heap_dist_sizes)/sizeof(*_tcpip_heap_dist_sizes) - 1];
#endif
}TCPIP_HEAP_DBG_DCPT;

// the heap debug descriptor
static TCPIP_HEAP_DBG_DCPT      _tcpipHeapDbgDcpt[TCPIP_STACK_SUPPORTED_HEAPS] = { {0} };


#define _heapFailMessage "Heap allocation of %d bytes failed in module %d line %d\r\n"

#if defined(_TCPIP_STACK_DRAM_TRACE_ENABLE)
    static TCPIP_HEAP_TRACE_ENTRY* TCPIP_HEAP_FindEntry(TCPIP_HEAP_DBG_DCPT* hDcpt, int moduleId, bool addNewSlot);
    static void TCPIP_HEAP_AddToEntry(TCPIP_HEAP_DBG_DCPT* hDcpt, int moduleId, size_t nBytes, void* ptr);
    static void TCPIP_HEAP_RemoveFromEntry(TCPIP_HEAP_DBG_DCPT* hDcpt, int moduleId, size_t nBytes);
#endif  // defined(_TCPIP_STACK_DRAM_TRACE_ENABLE)

#if defined(_TCPIP_STACK_DRAM_DIST_ENABLE)
static int TCPIP_HEAP_DistCompare(const void *a, const void *b);
static void TCPIP_HEAP_DistAdd(TCPIP_HEAP_DBG_DCPT* hDcpt, int moduleId, size_t nBytes);
static void TCPIP_HEAP_DistRem(TCPIP_HEAP_DBG_DCPT* hDcpt, int moduleId, size_t nBytes);
#endif  // defined(_TCPIP_STACK_DRAM_DIST_ENABLE)


// API

TCPIP_STACK_HEAP_HANDLE TCPIP_HEAP_Create(const TCPIP_STACK_HEAP_CONFIG* initData, TCPIP_STACK_HEAP_RES* pRes)
{
    TCPIP_STACK_HEAP_HANDLE newH;
    TCPIP_STACK_HEAP_FLAGS  flags;

    if(initData == 0)
    {
        return 0;
    }

    switch (initData->heapType)
    {
#if defined (TCPIP_STACK_USE_INTERNAL_HEAP)
        case TCPIP_STACK_HEAP_TYPE_INTERNAL_HEAP:
            newH = TCPIP_HEAP_CreateInternal((const TCPIP_STACK_HEAP_INTERNAL_CONFIG*)initData, pRes);
            flags = initData->heapFlags;
            break;
#endif  // defined (TCPIP_STACK_USE_INTERNAL_HEAP)

#if defined (TCPIP_STACK_USE_EXTERNAL_HEAP)
        case TCPIP_STACK_HEAP_TYPE_EXTERNAL_HEAP:
            newH = TCPIP_HEAP_CreateExternal((TCPIP_STACK_HEAP_EXTERNAL_CONFIG*)initData, pRes);
            flags = initData->heapFlags;
            break;
#endif  // defined (TCPIP_STACK_USE_EXTERNAL_HEAP)
            
#if defined (TCPIP_STACK_USE_INTERNAL_HEAP_POOL)
        case TCPIP_STACK_HEAP_TYPE_INTERNAL_HEAP_POOL:
            newH = TCPIP_HEAP_CreateInternalPool((const TCPIP_STACK_HEAP_POOL_CONFIG*)initData, pRes);
            flags = initData->heapFlags;
            break;
#endif  // defined (TCPIP_STACK_USE_INTERNAL_HEAP_POOL)

        default:
            return 0;
    }

    if(newH != 0)
    {
        int hIx;
        TCPIP_HEAP_DBG_DCPT* pDcpt = _tcpipHeapDbgDcpt;
        for(hIx = 0; hIx < sizeof(_tcpipHeapDbgDcpt) / sizeof(*_tcpipHeapDbgDcpt); hIx++, pDcpt++)
        {
            if(pDcpt->heapH == 0)
            {   // found slot
                pDcpt->heapH = newH;
                pDcpt->heapType = initData->heapType;
                pDcpt->heapFlags = flags;
#if defined(_TCPIP_STACK_DRAM_TRACE_ENABLE)
                memset(pDcpt->_heapTraceTbl, 0, sizeof(pDcpt->_heapTraceTbl)); // clear entries
#endif
#if defined(_TCPIP_STACK_DRAM_DIST_ENABLE)
                // initialize the distribution sizes array
                int ix;
                TCPIP_HEAP_DIST_ENTRY* pEntry;
                const int* pSize;

                pSize = _tcpip_heap_dist_sizes;
                pEntry = pDcpt->_tcpip_heap_dist_array;
                for(ix = 0; ix < sizeof(_tcpip_heap_dist_sizes)/sizeof(*_tcpip_heap_dist_sizes) - 1; ix++, pSize++, pEntry++)
                {
                    memset(pEntry, 0, sizeof(*pEntry)); 
                    pEntry->lowLimit = *pSize;
                    pEntry->highLimit = *(pSize + 1);
                }
#endif
                break;
            }
        }
    }

    return newH;
}

static TCPIP_HEAP_DBG_DCPT* _TCPIP_HEAP_FindDcpt(TCPIP_STACK_HEAP_HANDLE heapH)
{
    int hIx;
    TCPIP_HEAP_DBG_DCPT* pDcpt = _tcpipHeapDbgDcpt;
    for(hIx = 0; hIx < sizeof(_tcpipHeapDbgDcpt) / sizeof(*_tcpipHeapDbgDcpt); hIx++, pDcpt++)
    {
        if(pDcpt->heapH == heapH)
        {   // found slot
            return pDcpt;
        }
    }

    return 0;
}

void* TCPIP_HEAP_MallocDebug(TCPIP_STACK_HEAP_HANDLE heapH, size_t nBytes, int moduleId, int lineNo)
{
    TCPIP_HEAP_OBJECT* hObj = (TCPIP_HEAP_OBJECT*)heapH;
    TCPIP_HEAP_DBG_DCPT* pDcpt = _TCPIP_HEAP_FindDcpt(heapH);

    void* ptr = (*hObj->TCPIP_HEAP_Malloc)(hObj, nBytes);

    if(ptr == 0)
    {
        if(pDcpt != 0)
        {
            if((pDcpt->heapFlags & TCPIP_STACK_HEAP_FLAG_NO_WARN_MESSAGE) == 0)
            {
                SYS_ERROR_PRINT(SYS_ERROR_WARNING, _heapFailMessage, nBytes, moduleId, lineNo);
            }

            if(pDcpt->noMemHandler != 0)
            {
                (*pDcpt->noMemHandler)(heapH, nBytes, moduleId, lineNo);
            }
        }
    }
#if defined(_TCPIP_STACK_DRAM_TRACE_ENABLE)
    if(pDcpt != 0)
    {
        if(ptr != 0)
        {
            nBytes = (*hObj->TCPIP_HEAP_AllocSize)(hObj, ptr);
        }
        TCPIP_HEAP_AddToEntry(pDcpt, moduleId, nBytes, ptr);
    }
#endif  // defined(_TCPIP_STACK_DRAM_TRACE_ENABLE)

#if defined(_TCPIP_STACK_DRAM_DIST_ENABLE)
    if(pDcpt != 0)
    {
        TCPIP_HEAP_DistAdd(pDcpt, moduleId, nBytes);
    }
#endif  // defined(_TCPIP_STACK_DRAM_DIST_ENABLE)

    return ptr;
}

void* TCPIP_HEAP_CallocDebug(TCPIP_STACK_HEAP_HANDLE heapH, size_t nElems, size_t elemSize, int moduleId, int lineNo)
{
    TCPIP_HEAP_OBJECT* hObj = (TCPIP_HEAP_OBJECT*)heapH;
    TCPIP_HEAP_DBG_DCPT* pDcpt = _TCPIP_HEAP_FindDcpt(heapH);

    size_t nBytes = nElems * elemSize;
    void* ptr = (*hObj->TCPIP_HEAP_Calloc)(hObj, nElems, elemSize);

    if(ptr == 0)
    {
        if(pDcpt != 0)
        {
            if((pDcpt->heapFlags & TCPIP_STACK_HEAP_FLAG_NO_WARN_MESSAGE) == 0)
            {
                SYS_ERROR_PRINT(SYS_ERROR_WARNING, _heapFailMessage, nBytes, moduleId, lineNo);
            }

            if(pDcpt->noMemHandler != 0)
            {
                (*pDcpt->noMemHandler)(heapH, nBytes, moduleId, lineNo);
            }
        }
    }

#if defined(_TCPIP_STACK_DRAM_TRACE_ENABLE)
    if(pDcpt != 0)
    {
        if(ptr != 0)
        {
            nBytes = (*hObj->TCPIP_HEAP_AllocSize)(hObj, ptr);
        }
        TCPIP_HEAP_AddToEntry(pDcpt, moduleId, nBytes, ptr);
    }
#endif  // defined(_TCPIP_STACK_DRAM_TRACE_ENABLE)

#if defined(_TCPIP_STACK_DRAM_DIST_ENABLE)
    if(pDcpt != 0)
    {
        TCPIP_HEAP_DistAdd(pDcpt, moduleId, nBytes);
    }
#endif  // defined(_TCPIP_STACK_DRAM_DIST_ENABLE)

    return ptr;
}

size_t TCPIP_HEAP_FreeDebug(TCPIP_STACK_HEAP_HANDLE heapH,  const void* pBuff, int moduleId)
{
    TCPIP_HEAP_OBJECT* hObj = (TCPIP_HEAP_OBJECT*)heapH;
#if defined(_TCPIP_STACK_DRAM_TRACE_ENABLE) || defined(_TCPIP_STACK_DRAM_DIST_ENABLE)
    TCPIP_HEAP_DBG_DCPT* pDcpt = _TCPIP_HEAP_FindDcpt(heapH);
#endif

    int nBytes = (*hObj->TCPIP_HEAP_Free)(hObj, pBuff);

#if defined(_TCPIP_STACK_DRAM_TRACE_ENABLE)
    if(pDcpt && nBytes)
    {
        TCPIP_HEAP_RemoveFromEntry(pDcpt, moduleId, nBytes);
    }
#endif  // defined(_TCPIP_STACK_DRAM_TRACE_ENABLE)
#if defined(_TCPIP_STACK_DRAM_DIST_ENABLE)
    if(pDcpt && nBytes)
    {
        TCPIP_HEAP_DistRem(pDcpt, moduleId, nBytes);
    }
#endif  // defined(_TCPIP_STACK_DRAM_DIST_ENABLE)

    return nBytes;
}

TCPIP_STACK_HEAP_RES TCPIP_HEAP_SetNoMemHandler(TCPIP_STACK_HEAP_HANDLE heapH, TCPIP_HEAP_NO_MEM_HANDLER handler)
{
    TCPIP_HEAP_DBG_DCPT* pDcpt = _TCPIP_HEAP_FindDcpt(heapH);

    if(pDcpt != 0)
    {
        pDcpt->noMemHandler = handler;
        return TCPIP_STACK_HEAP_RES_OK;
    }

    return TCPIP_STACK_HEAP_RES_NO_HEAP;
}

#if defined(_TCPIP_STACK_DRAM_TRACE_ENABLE)
static TCPIP_HEAP_TRACE_ENTRY* TCPIP_HEAP_FindEntry(TCPIP_HEAP_DBG_DCPT* hDcpt, int moduleId, bool addNewSlot)
{
    int ix;
    TCPIP_HEAP_TRACE_ENTRY    *freeEntry,*pEntry;
#if _TCPIP_STACK_DRAM_TRACE_NDP_COLLAPSE
    if(moduleId == TCPIP_MODULE_NDP)
    {
        moduleId = TCPIP_MODULE_IPV6;
    }
#endif  // _TCPIP_STACK_DRAM_TRACE_NDP_COLLAPSE

    freeEntry = 0;
    for(ix = 0, pEntry = hDcpt->_heapTraceTbl; ix < sizeof(hDcpt->_heapTraceTbl)/sizeof(*hDcpt->_heapTraceTbl); ix++, pEntry++)
    {
        if(pEntry->moduleId == moduleId)
        {
            return pEntry;
        }
        else if(addNewSlot && freeEntry == 0 && pEntry->moduleId == 0)
        {
            freeEntry = pEntry;
        }
    }

    if(freeEntry)
    {
        memset(freeEntry, 0x0, sizeof(*freeEntry));
        freeEntry->moduleId = moduleId;
    }

    return freeEntry;
}

static void TCPIP_HEAP_AddToEntry(TCPIP_HEAP_DBG_DCPT* hDcpt, int moduleId, size_t nBytes, void* ptr)
{
    TCPIP_HEAP_TRACE_ENTRY *pEntry = TCPIP_HEAP_FindEntry(hDcpt, moduleId, true);

    if(pEntry)
    {
        if(ptr)
        {   // successful
            pEntry->totAllocated += nBytes;
            pEntry->currAllocated += nBytes;
        }
        else
        {
            pEntry->totFailed += nBytes;
            if(nBytes > pEntry->maxFailed)
            {
                pEntry->maxFailed = nBytes;
            }
        }
    }
}

static void TCPIP_HEAP_RemoveFromEntry(TCPIP_HEAP_DBG_DCPT* hDcpt, int moduleId, size_t nBytes)
{
    TCPIP_HEAP_TRACE_ENTRY *pEntry = TCPIP_HEAP_FindEntry(hDcpt, moduleId, false);

    if(pEntry)
    {
        pEntry->currAllocated -= nBytes;
    }
}

bool TCPIP_HEAP_TraceGetEntry(TCPIP_STACK_HEAP_HANDLE heapH, unsigned int entryIx, TCPIP_HEAP_TRACE_ENTRY* tEntry)
{
    TCPIP_HEAP_TRACE_ENTRY *pEntry;
    TCPIP_HEAP_DBG_DCPT* pDcpt = _TCPIP_HEAP_FindDcpt(heapH);

    if(pDcpt)
    {
        if(entryIx < sizeof(pDcpt->_heapTraceTbl)/sizeof(*pDcpt->_heapTraceTbl))
        {   // valid index
            pEntry = pDcpt->_heapTraceTbl + entryIx;
            if(pEntry->moduleId > 0)
            {
                *tEntry = *pEntry;
                return true;
            }
        }
    }

    return false;
}

unsigned int TCPIP_HEAP_TraceGetEntriesNo(TCPIP_STACK_HEAP_HANDLE heapH, bool getUsed)
{
    TCPIP_HEAP_DBG_DCPT* pDcpt = _TCPIP_HEAP_FindDcpt(heapH);

    if(getUsed)
    {
        TCPIP_HEAP_TRACE_ENTRY *pEntry;
        int ix, nUsed;

        nUsed = 0;

        for(ix = 0, pEntry = pDcpt->_heapTraceTbl; ix < sizeof(pDcpt->_heapTraceTbl)/sizeof(*pDcpt->_heapTraceTbl); ix++, pEntry++)
        {
            if(pEntry->moduleId > 0)
            {
                nUsed++;
            }
        }

        return nUsed;
    }

    return sizeof(pDcpt->_heapTraceTbl)/sizeof(*pDcpt->_heapTraceTbl);
}


#else

bool TCPIP_HEAP_TraceGetEntry(TCPIP_STACK_HEAP_HANDLE heapH, unsigned int entryIx, TCPIP_HEAP_TRACE_ENTRY* tEntry)
{
    return false;
}

unsigned int TCPIP_HEAP_TraceGetEntriesNo(TCPIP_STACK_HEAP_HANDLE heapH, bool getUsed)
{
    return 0;
}

#endif  // defined(_TCPIP_STACK_DRAM_TRACE_ENABLE)


#if defined (_TCPIP_STACK_DRAM_DIST_ENABLE)
bool TCPIP_HEAP_DistGetEntry(TCPIP_STACK_HEAP_HANDLE heapH, unsigned int entryIx, TCPIP_HEAP_DIST_ENTRY* pEntry)
{
    TCPIP_HEAP_DBG_DCPT* pDcpt = _TCPIP_HEAP_FindDcpt(heapH);

    if(pDcpt && pEntry)
    {
        if(entryIx < sizeof(pDcpt->_tcpip_heap_dist_array)/sizeof(*pDcpt->_tcpip_heap_dist_array))
        {   // valid index
           *pEntry =  *(pDcpt->_tcpip_heap_dist_array + entryIx);
           return true;
        }
    }

    return false;
}

unsigned int TCPIP_HEAP_DistGetEntriesNo(TCPIP_STACK_HEAP_HANDLE heapH)
{
    TCPIP_HEAP_DBG_DCPT* pDcpt = _TCPIP_HEAP_FindDcpt(heapH);

    return pDcpt == 0 ? 0 : sizeof(pDcpt->_tcpip_heap_dist_array)/sizeof(*pDcpt->_tcpip_heap_dist_array);
}


static int TCPIP_HEAP_DistCompare(const void *a, const void *b)
{
    TCPIP_HEAP_DIST_ENTRY* pA = (TCPIP_HEAP_DIST_ENTRY*)a;
    TCPIP_HEAP_DIST_ENTRY* pB = (TCPIP_HEAP_DIST_ENTRY*)b;

    if(pA->lowLimit > pB->highLimit)
    {
        return 1;
    }
    else if(pA->highLimit < pB->lowLimit)
    {
        return -1;
    }

    return 0;
}

static void TCPIP_HEAP_DistAdd(TCPIP_HEAP_DBG_DCPT* hDcpt, int moduleId, size_t nBytes)
{
    int     modIx;
    TCPIP_HEAP_DIST_ENTRY* pEntry;
    TCPIP_HEAP_DIST_ENTRY nEntry =
    {
        .lowLimit = nBytes,
        .highLimit = nBytes,
        { {0} },
        0
    };
    
    pEntry = bsearch(&nEntry, hDcpt->_tcpip_heap_dist_array, sizeof(hDcpt->_tcpip_heap_dist_array)/sizeof(*hDcpt->_tcpip_heap_dist_array), sizeof(*hDcpt->_tcpip_heap_dist_array), TCPIP_HEAP_DistCompare);

    pEntry->currHits++;
    // search for module
    struct moduleDist* pFreeMod = 0;
    struct moduleDist* pmDist = pEntry->modDist;
    for(modIx = 0; modIx < sizeof(pEntry->modDist)/sizeof(*pEntry->modDist); modIx++, pmDist++)
    {
        if(pmDist->modId == moduleId)
        {   // found module slot
            pmDist->modHits++;
            return;
        }
        else if(pmDist->modId == 0 && pFreeMod == 0)
        {
            pFreeMod = pmDist;
        }
    }

    // module slot not found
    if(pFreeMod)
    {
        pFreeMod->modId = moduleId;
        pFreeMod->modHits = 1;
    }
    else
    {   // all slots occupied, use the generic entry
        pEntry->gHits++;
    }
}


static void TCPIP_HEAP_DistRem(TCPIP_HEAP_DBG_DCPT* hDcpt, int moduleId, size_t nBytes)
{
    TCPIP_HEAP_DIST_ENTRY* pEntry;
    TCPIP_HEAP_DIST_ENTRY nEntry =
    {
        .lowLimit = nBytes,
        .highLimit = nBytes,
        { {0} },
        0
    };
    
    pEntry = bsearch(&nEntry, hDcpt->_tcpip_heap_dist_array, sizeof(hDcpt->_tcpip_heap_dist_array)/sizeof(*hDcpt->_tcpip_heap_dist_array), sizeof(*hDcpt->_tcpip_heap_dist_array), TCPIP_HEAP_DistCompare);

    pEntry->currHits--;
}

#else

bool TCPIP_HEAP_DistGetEntry(TCPIP_STACK_HEAP_HANDLE heapH, unsigned int entryIx, TCPIP_HEAP_DIST_ENTRY* pEntry)
{
    return false;
}

unsigned int TCPIP_HEAP_DistGetEntriesNo(TCPIP_STACK_HEAP_HANDLE heapH)
{
    return 0;
}

#endif  // defined (_TCPIP_STACK_DRAM_DIST_ENABLE)


#endif  // defined(TCPIP_STACK_DRAM_DEBUG_ENABLE) 

