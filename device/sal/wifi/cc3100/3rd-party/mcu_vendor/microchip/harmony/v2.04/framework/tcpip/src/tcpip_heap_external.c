/*******************************************************************************
  TCPIP Heap Allocation Manager

  Summary:
    
  Description:
*******************************************************************************/

/*******************************************************************************
File Name:  tcpip_heap_external.c
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

#include <sys/kmem.h>


#include "tcpip/src/tcpip_private.h"
// definitions


// min heap alignment
// always power of 2
#if defined(__PIC32MX__)
typedef uint32_t _heap_Align;
#elif defined(__PIC32MZ__) || defined(__PIC32WK__)
typedef struct __attribute__((aligned(16)))
{
    uint64_t     pad[2];
}_heap_Align;
#endif  // defined(__PIC32MX__) || defined(__PIC32MZ__)

typedef union __attribute__((aligned(16))) _tag_headNode
{
    _heap_Align x;
    struct
    {
        union _tag_headNode*	allocPtr;   // pointer as returned by the external allocation function
    };
}_headNode;



typedef struct
{
    TCPIP_STACK_HEAP_EXTERNAL_CONFIG    heapConfig;         // configuration data save
    size_t                              allocatedBytes;     // how many bytes allocated out there

    OSAL_SEM_HANDLE_TYPE                _heapSemaphore;     // protection semaphore
    TCPIP_STACK_HEAP_RES                _lastHeapErr;       // last error encountered

    // run time flags - faster access
    uint8_t                             heapDoProtect;      // protect external memory access
    uint8_t                             heapDoMap;          // map buffers to non-cached
    uint8_t                             heapDoAlign;        // align buffers internally
    uint8_t                             heapPad[1];         // padding, not used
}TCPIP_HEAP_EXT_DCPT; // descriptor of a heap


// local data
//

static TCPIP_STACK_HEAP_RES   _TCPIP_HEAP_Delete(TCPIP_STACK_HEAP_HANDLE heapH);
static void*            _TCPIP_HEAP_Malloc(TCPIP_STACK_HEAP_HANDLE heapH, size_t nBytes);
static void*            _TCPIP_HEAP_Calloc(TCPIP_STACK_HEAP_HANDLE heapH, size_t nElems, size_t elemSize);
static size_t           _TCPIP_HEAP_Free(TCPIP_STACK_HEAP_HANDLE heapH, const void* pBuff);

static size_t           _TCPIP_HEAP_Size(TCPIP_STACK_HEAP_HANDLE heapH);
static size_t           _TCPIP_HEAP_MaxSize(TCPIP_STACK_HEAP_HANDLE heapH);
static size_t           _TCPIP_HEAP_FreeSize(TCPIP_STACK_HEAP_HANDLE heapH);
static TCPIP_STACK_HEAP_RES   _TCPIP_HEAP_LastError(TCPIP_STACK_HEAP_HANDLE heapH);
#if defined(TCPIP_STACK_DRAM_DEBUG_ENABLE) 
static size_t           _TCPIP_HEAP_AllocSize(TCPIP_STACK_HEAP_HANDLE heapH, const void* ptr);
#endif  // defined(TCPIP_STACK_DRAM_DEBUG_ENABLE) 

// maps a buffer to non cached memory
const void*   _TCPIP_HEAP_BufferMapNonCached(const void* buffer, size_t buffSize);

// maps a pointer back to cached memory
const void*   _TCPIP_HEAP_PointerMapCached(const void* ptr);



// the heap object
static const TCPIP_HEAP_OBJECT      _tcpip_heap_object = 
{
    _TCPIP_HEAP_Delete,
    _TCPIP_HEAP_Malloc,
    _TCPIP_HEAP_Calloc,
    _TCPIP_HEAP_Free,
    _TCPIP_HEAP_Size,
    _TCPIP_HEAP_MaxSize,
    _TCPIP_HEAP_FreeSize,
    _TCPIP_HEAP_LastError,
#if defined(TCPIP_STACK_DRAM_DEBUG_ENABLE) 
    _TCPIP_HEAP_AllocSize,
#endif  // defined(TCPIP_STACK_DRAM_DEBUG_ENABLE) 
};

typedef struct
{
    TCPIP_HEAP_OBJECT   heapObj;    // heap object API
    TCPIP_HEAP_EXT_DCPT heapDcpt;   // private heap object data
}TCPIP_HEAP_EXT_OBJ_INSTANCE;



// local prototypes
//
static __inline__ bool __attribute__((always_inline)) _TCPIP_HEAP_IsPtrAligned(const void* ptr)
{
    return (uintptr_t)(ptr) == ((uintptr_t)(ptr) & (~(sizeof(_headNode) - 1)));
}

// returns the TCPIP_HEAP_EXT_OBJ_INSTANCE associated with a heap handle
// null if invalid
static __inline__ TCPIP_HEAP_EXT_OBJ_INSTANCE* __attribute__((always_inline)) _TCPIP_HEAP_ObjInstance(TCPIP_STACK_HEAP_HANDLE heapH)
{
#if defined(TCPIP_STACK_DRAM_DEBUG_ENABLE) 
    if(heapH)
    {
        TCPIP_HEAP_EXT_OBJ_INSTANCE* pInst = (TCPIP_HEAP_EXT_OBJ_INSTANCE*)heapH;
        if(pInst->heapObj.TCPIP_HEAP_Delete == _TCPIP_HEAP_Delete)
        {
            return pInst;
        }
    }
    return 0;
#else
    return (heapH == 0) ? 0 : (TCPIP_HEAP_EXT_OBJ_INSTANCE*)heapH;
#endif  // defined(TCPIP_STACK_DRAM_DEBUG_ENABLE) 

}

// returns the TCPIP_HEAP_EXT_DCPT associated with a heap handle
// null if invalid
static __inline__ TCPIP_HEAP_EXT_DCPT* __attribute__((always_inline)) _TCPIP_HEAP_ObjDcpt(TCPIP_STACK_HEAP_HANDLE heapH)
{
    TCPIP_HEAP_EXT_OBJ_INSTANCE* hInst = _TCPIP_HEAP_ObjInstance(heapH);

    return (hInst == 0) ? 0 : &hInst->heapDcpt;
}


// API

TCPIP_STACK_HEAP_HANDLE TCPIP_HEAP_CreateExternal(const TCPIP_STACK_HEAP_EXTERNAL_CONFIG* pHeapConfig, TCPIP_STACK_HEAP_RES* pRes)
{
    TCPIP_HEAP_EXT_DCPT* hDcpt;
    TCPIP_HEAP_EXT_OBJ_INSTANCE* hInst;
    TCPIP_STACK_HEAP_RES  res;
    

    while(true)
    {
        hDcpt =0;
        hInst = 0;

        if( pHeapConfig == 0)
        {
            res = TCPIP_STACK_HEAP_RES_INIT_ERR;
            break;
        }

        hInst = (TCPIP_HEAP_EXT_OBJ_INSTANCE*)(*pHeapConfig->calloc_fnc)(1, sizeof(*hInst));

        if(hInst == 0)
        {
            res = TCPIP_STACK_HEAP_RES_CREATE_ERR;
            break;
        }

        hDcpt = &hInst->heapDcpt;
        if((pHeapConfig->heapFlags & TCPIP_STACK_HEAP_FLAG_NO_MTHREAD_SYNC) == 0)
        {
            if(OSAL_SEM_Create(&hDcpt->_heapSemaphore, OSAL_SEM_TYPE_BINARY, 1, 1) != OSAL_RESULT_TRUE)
            {
                (*pHeapConfig->free_fnc)(hInst);
                hInst = 0;
                res = TCPIP_STACK_HEAP_RES_SYNCH_ERR;
                break;
            }

            hDcpt->heapDoProtect = true;
        }

        // save the configuration parameters
        hDcpt->heapConfig = *pHeapConfig;
        // create the object
        hInst->heapObj = _tcpip_heap_object;

        // check if mapping needed; always alloc uncached!
        // if((hDcpt->heapConfig.heapFlags & TCPIP_STACK_HEAP_FLAG_ALLOC_UNCACHED) != 0)
        {
            const void* testPtr = _TCPIP_HEAP_BufferMapNonCached(hInst, sizeof(*hInst));
            if(hInst != testPtr)
            {
                hDcpt->heapDoMap = true;
            }
        }
        // check if alignmnt needed
        if((hDcpt->heapConfig.heapFlags & TCPIP_STACK_HEAP_FLAG_ALLOC_UNALIGN) == 0)
        {
            hDcpt->heapDoAlign = true;
        }

        res = TCPIP_STACK_HEAP_RES_OK;
        break;
    }

    if(pRes)
    {
        *pRes = res;
    }

    return hInst;
    
}

// internal functions
//
// deallocates the heap
// NOTE: no check is done if some blocks are still in use!
static TCPIP_STACK_HEAP_RES _TCPIP_HEAP_Delete(TCPIP_STACK_HEAP_HANDLE heapH)
{
    TCPIP_HEAP_EXT_OBJ_INSTANCE*   hInst = _TCPIP_HEAP_ObjInstance(heapH);

    if(hInst)
    {
        OSAL_SEM_Delete(&hInst->heapDcpt._heapSemaphore);
        // invalidate it
        memset(&hInst->heapObj, 0, sizeof(hInst->heapObj));
        (*hInst->heapDcpt.heapConfig.free_fnc)(hInst);
        return TCPIP_STACK_HEAP_RES_OK;
    }

    return TCPIP_STACK_HEAP_RES_NO_HEAP; 
}


static void* _TCPIP_HEAP_Malloc(TCPIP_STACK_HEAP_HANDLE heapH, size_t nBytes)
{
	size_t      mapBytes;
	_headNode*  ptr;
    TCPIP_HEAP_EXT_DCPT*   hDcpt = _TCPIP_HEAP_ObjDcpt(heapH);
    void* alignPtr = 0;

    
	if(hDcpt == 0 || nBytes == 0)
    {
        return 0;
    }

    if(hDcpt->heapDoAlign)
    {
        nBytes = ((nBytes + sizeof(_headNode) - 1) / sizeof(_headNode) + 1) * sizeof(_headNode);	// allocate multiple of units + 1 entry to store the pointer  
    }

    if (hDcpt->heapDoProtect)
    {
        OSAL_SEM_Pend(&hDcpt->_heapSemaphore, OSAL_WAIT_FOREVER);
    }

	while(true)
    {
        ptr = (_headNode*)(*hDcpt->heapConfig.malloc_fnc)(nBytes);

        if(ptr == 0)
        {   // failed
            hDcpt->_lastHeapErr = TCPIP_STACK_HEAP_RES_NO_MEM;
            break;
        }

        if(hDcpt->heapDoAlign)
        {   
            if(_TCPIP_HEAP_IsPtrAligned(ptr))
            {   // 1st unit is for storing the allocated pointer
                alignPtr = ptr + 1;
            }
            else
            {   // always returned an aligned pointer; advance to next aligned address 
                alignPtr = (void*)(((uintptr_t)ptr + sizeof(_headNode) - 1) & ~(sizeof(_headNode) - 1));
            }

           // store the modified pointer
            *((uintptr_t*)alignPtr - 1) = (uintptr_t)ptr;
            mapBytes = nBytes - sizeof(_headNode);
        }
        else
        {
            if(!_TCPIP_HEAP_IsPtrAligned(ptr))
            {
                hDcpt->_lastHeapErr = TCPIP_STACK_HEAP_RES_BUFF_ALIGN_ERR;
                break;
            }
            alignPtr = ptr;
            mapBytes = nBytes;
        }

        if(hDcpt->heapDoMap)
        {   // map to non-cached
            alignPtr = (void*)_TCPIP_HEAP_BufferMapNonCached(alignPtr, mapBytes);
        }

        // update stats
        hDcpt->allocatedBytes += nBytes;

        break;
    }

    if (hDcpt->heapDoProtect)
    {
        OSAL_SEM_Post(&hDcpt->_heapSemaphore);
    }

    return alignPtr;
}

static void* _TCPIP_HEAP_Calloc(TCPIP_STACK_HEAP_HANDLE heapH, size_t nElems, size_t elemSize)
{
    size_t nBytes = nElems * elemSize;

    // redirect to malloc

    void* ptr = _TCPIP_HEAP_Malloc(heapH, nBytes);
    if(ptr)
    {
        memset(ptr, 0, nBytes);
    }

    return ptr;
}

static size_t _TCPIP_HEAP_Free(TCPIP_STACK_HEAP_HANDLE heapH, const void* ptr)
{
    TCPIP_HEAP_EXT_DCPT*   hDcpt = _TCPIP_HEAP_ObjDcpt(heapH);
  
	if(hDcpt != 0 && ptr != 0)
    {
        if (hDcpt->heapDoProtect)
        {
            OSAL_SEM_Pend(&hDcpt->_heapSemaphore, OSAL_WAIT_FOREVER);
        }

        if(hDcpt->heapDoMap)
        {   // map back to cached
            ptr = _TCPIP_HEAP_PointerMapCached(ptr);
        }

        if(hDcpt->heapDoAlign)
        {   // restore the allocated pointer
            ptr = (void*)*((uintptr_t*)ptr - 1);
        }

        (*hDcpt->heapConfig.free_fnc)((void*)ptr);

        // update stats
        // no info on how many de-allocated bytes here!

        if (hDcpt->heapDoProtect)
        {
            OSAL_SEM_Post(&hDcpt->_heapSemaphore);
        }
    }

    return 0;
}


static size_t _TCPIP_HEAP_Size(TCPIP_STACK_HEAP_HANDLE heapH)
{
    TCPIP_HEAP_EXT_DCPT*      hDcpt;

    hDcpt = _TCPIP_HEAP_ObjDcpt(heapH);

    if(hDcpt)
    {
        return 0;   
    }

    return -1;
}

static size_t _TCPIP_HEAP_FreeSize(TCPIP_STACK_HEAP_HANDLE heapH)
{
    TCPIP_HEAP_EXT_DCPT*      hDcpt;

    hDcpt = _TCPIP_HEAP_ObjDcpt(heapH);


    if(hDcpt)
    {
        return 0;   
    }
    return -1;
}


static size_t _TCPIP_HEAP_MaxSize(TCPIP_STACK_HEAP_HANDLE heapH)
{
    TCPIP_HEAP_EXT_DCPT*      hDcpt;

    hDcpt = _TCPIP_HEAP_ObjDcpt(heapH);


    if(hDcpt)
    {
        return 0;   
    }
    return -1;
}


static TCPIP_STACK_HEAP_RES _TCPIP_HEAP_LastError(TCPIP_STACK_HEAP_HANDLE heapH)
{
    TCPIP_HEAP_EXT_DCPT*      hDcpt;
    TCPIP_STACK_HEAP_RES  res;

    hDcpt = _TCPIP_HEAP_ObjDcpt(heapH);

    if(hDcpt)
    {
        res = hDcpt->_lastHeapErr;
        hDcpt->_lastHeapErr = TCPIP_STACK_HEAP_RES_OK;
    }
    else
    {
        res = TCPIP_STACK_HEAP_RES_NO_HEAP; 
    }

    return res;
}

#if defined(TCPIP_STACK_DRAM_DEBUG_ENABLE) 
static size_t _TCPIP_HEAP_AllocSize(TCPIP_STACK_HEAP_HANDLE heapH, const void* ptr)
{
    TCPIP_HEAP_EXT_DCPT*   hDcpt = _TCPIP_HEAP_ObjDcpt(heapH);

    if(hDcpt)
    {
        return 0;
    }

    return -1;
}
#endif  // defined(TCPIP_STACK_DRAM_DEBUG_ENABLE) 


