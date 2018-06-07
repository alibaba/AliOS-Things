/* Copyright (C) 2016 RDA Technologies Limited and/or its affiliates("RDA").
* All rights reserved.
*
* This software is supplied "AS IS" without any warranties.
* RDA assumes no responsibility or liability for the use of the software,
* conveys no license or title under any patent, copyright, or mask work
* right to the product. RDA reserves the right to make changes in the
* software without notification.  RDA also make no representation or
* warranty that such application will be suitable for the specified use
* without further testing or modification.
*/








#ifndef __SXR_MEM_H__
#define __SXR_MEM_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "sxs_type.h"
#include "sxr_cnf.h"


#ifdef __SXR_MEM_VAR__
#define DefExtern
#else
#define DefExtern extern
#endif

#define SXR_STD_MEM  0
#define SXR_INT_MEM  1

#ifndef __SXR_CLUSTER__
#define SXR_CLUSTER_LIKE_MEM 0xFF
#endif

#define sxr_NewHeap(a,b)      _sxr_NewHeap(a,b,SXR_STD_MEM)
#define sxr_HMalloc(a)        _sxr_HMalloc(a,SXR_STD_MEM)
#define sxr_IntMalloc(a)      _sxr_HMalloc(a,SXR_INT_MEM)

// =============================================================================
// _sxr_NewHeap
// -----------------------------------------------------------------------------
/// Create a heap memory area.
/// @param  HeapStart   first address of the heap.
/// @param  Size        in bytes.
/// @param  Idx         Heap instance index.
// =============================================================================
void  _sxr_NewHeap    (void *HeapStart, u32 Size, u8 Idx);
// =============================================================================
// sxr_HMalloc
// -----------------------------------------------------------------------------
/// Heap memory allocation.
/// @param  Size    Requered memory size in bytes.
/// @param  Idx     Heap instance index.
/// @return Pointer onto free memory of required size.
// =============================================================================
void *_sxr_HMalloc    (u32 Size, u8 Idx);
// =============================================================================
// _sxr_UpMemCallerAdd
// -----------------------------------------------------------------------------
/// Update the memory \p CallerAdd field, to be used by custom allocation
/// function to actually record the user function and not the custom allocation
/// function.
/// @param  Ptr     pointer to allocated memory
/// @param  callAdd new value of \p CallerAdd field.
// =============================================================================
void  _sxr_UpMemCallerAdd(void *Ptr,  u32 callAdd);


// =============================================================================
// sxr_HFree
// -----------------------------------------------------------------------------
/// Free memory allocated in heap.
/// @param  Ptr on memory.
// =============================================================================
#ifdef __SXR_RELEASED_PTR_TO_NIL__
void _sxr_HFree (void **Ptr);
#define sxr_HFree(a)  _sxr_HFree((void **)&(a))
#else
void  sxr_HFree       (void *MemoryPtr);
#define _sxr_HFree(a)  sxr_HFree(a)
#endif

#define sxr_UpMemCallerAdd(a, b)   \
do{\
   _sxr_UpMemCallerAdd((void *)(a),  b);\
}while(0)


// =============================================================================
// sxr_GetMemSize
// -----------------------------------------------------------------------------
/// Get the allocated size of the memory which start at the specified address.
/// @param  Address Start address of the allocated memory.
/// @return Allocated size.
// =============================================================================
u16   sxr_GetMemSize  (void *Address);

// =============================================================================
// sxr_CheckHeap
// -----------------------------------------------------------------------------
/// Check Heaps load and consistency.
// =============================================================================
void  sxr_CheckHeap   (void);


// =============================================================================
// sxr_NewCluster
// -----------------------------------------------------------------------------
/// This function is used to create a new cluster. The cluster will be composed
/// of NbUnit of Size bytes.
/// @param  Size of units
/// @param  NbUnit number of units
// =============================================================================
void  sxr_NewCluster  (u16 Size, u8 NbUnit);
#ifdef __SXR_CLUSTER__
// =============================================================================
// sxr_Malloc
// -----------------------------------------------------------------------------
/// Memory allocation from cluster.
/// @param  Memory Size
/// @return Pointer onto memory block.
// =============================================================================
void *sxr_Malloc      (u16 Size);
#else
// =============================================================================
// sxr_Malloc
// -----------------------------------------------------------------------------
/// Memory allocation from cluster.
/// @param  a dMemory Size
/// @return Pointer onto memory block.
// =============================================================================
#define sxr_Malloc(a) _sxr_HMalloc(a,SXR_CLUSTER_LIKE_MEM)
#endif


// =============================================================================
// sxr_Link
// -----------------------------------------------------------------------------
/// Link a piece of memory already allocated.
/// @param  Ptr Pointer onto cluster
// =============================================================================
void  sxr_Link        (void *Ptr);


#ifdef __SXR_CLUSTER__
// =============================================================================
// sxr_Free
// -----------------------------------------------------------------------------
/// Free memory.
/// @param  Ptr Pointer onto memory block to be released.
// =============================================================================
#ifdef __SXR_RELEASED_PTR_TO_NIL__
void _sxr_Free (void **Ptr);
#define sxr_Free(a)  _sxr_Free((void *)&(a))
#else
void  sxr_Free        (void *Ptr);
#endif
#else
// =============================================================================
// sxr_Free
// -----------------------------------------------------------------------------
/// Free memory.
/// @param  a Pointer onto memory block to be released.
// =============================================================================
#ifdef __SXR_RELEASED_PTR_TO_NIL__
#define sxr_Free(a)  _sxr_HFree((void *)&(a))
#else
#define sxr_Free(a)   sxr_HFree(a)
#endif
#endif

// =============================================================================
// sxr_GetLink
// -----------------------------------------------------------------------------
/// Get Link counter
/// @param  Ptr on allocated memory.
/// @return Number of link.
// =============================================================================
u8  sxr_GetLink (void *Ptr);

// =============================================================================
// sxr_GetAbsoluteRemainingSize
// -----------------------------------------------------------------------------
/// Return the number of bytes remaining in the identified heap.
/// If the Heap is identified thanks to its index, the parameter Idx is useless.
/// When Idx == SXR_CLUSTER_LIKE_MEM the function will return the size remaining
/// in the "cluster heap" targeting the providing size.
/// @param Size allows to identify the target "cluster heap"
/// @param Idx  heap index or SXR_CLUSTER_LIKE_MEM.
/// @return     number of bytes available in the identified Heap.
// =============================================================================
u32 sxr_GetAbsoluteRemainingSize (u32 Size, u8 Idx);

// =============================================================================
// sxr_GetRelativeRemainingSize
// -----------------------------------------------------------------------------
/// Return the ratio of bytes remaining in the identified memory the total
/// number of bytes dedicated to the heap.
/// If the Heap is identified thanks to its index, the parameter Size is useless.
/// When Idx == SXR_CLUSTER_LIKE_MEM the function will return the size remaining
/// in the "cluster heap" targeting the providing size.
/// @param Size allows to identify the target "cluster heap"
/// @param Idx  heap index or SXR_CLUSTER_LIKE_MEM.
/// @return     percentage of bytes available in the identified Heap.
// =============================================================================
u32 sxr_GetRelativeRemainingSize (u32 Size, u8 Idx);

#define sxr_GetAbsoluteHeapRemainingSize(Idx)     sxr_GetAbsoluteRemainingSize(0, Idx)
#define sxr_GetRelativeHeapRemainingSize(Idx)     sxr_GetRelativeRemainingSize(0, Idx)

#define sxr_GetAbsoluteClusterRemainingSize(Size) sxr_GetAbsoluteRemainingSize(Size, SXR_CLUSTER_LIKE_MEM)
#define sxr_GetRelativeClusterRemainingSize(Size) sxr_GetRelativeRemainingSize(Size, SXR_CLUSTER_LIKE_MEM)

// =============================================================================
// sxr_GetHeapEnd
// -----------------------------------------------------------------------------
/// Return Heap end address.
/// @param  Idx Heap index.
/// @return Address on heap end.
// =============================================================================
void *sxr_GetHeapEnd  (u8 Idx);

// =============================================================================
// sxr_CheckCluster
// -----------------------------------------------------------------------------
/// Check clusters load and consistency.
// =============================================================================
void  sxr_CheckCluster (void);

// =============================================================================
// sxr_CheckClusterRmt
// -----------------------------------------------------------------------------
/// Check clusters load and consistency and send Info to Remote.
// =============================================================================
void  sxr_CheckClusterRmt (void);

// =============================================================================
// sxr_CheckMemTracker
// -----------------------------------------------------------------------------
/// Print the last memory allocation activity.
// =============================================================================
void  sxr_CheckMemTracker (void);

typedef struct
{
    u16 Size;            // Cluster size.
    u16 NbCluster;       // Number of clusters.
    u16 NbAllocation;    // Number of blocks currently allocated.
} sxr_ClusterInfo_t;

// =============================================================================
// sxr_GetClusterInfo
// -----------------------------------------------------------------------------
/// Provide information about the specified cluster.
/// @param  ClusterIdx  Cluster index
/// @param  ClusterInfo Pointer to sxr_ClusterInfo_t
// =============================================================================
void sxr_GetClusterInfo (u8 ClusterIdx, sxr_ClusterInfo_t *ClusterInfo);

#if defined (__SXR_DEBUG__) || defined (__SXR_MEM_CHECK__)
// =============================================================================
// sxr_CheckUpdateHeap
// -----------------------------------------------------------------------------
/// Check heaps load and consistency in debug mode.
/// @param  Block
/// @param  Idx Heap index.
// =============================================================================
void sxr_CheckUpdateHeap (void *Block, u8 Idx); // (sxr_HMemHead_t *PHeap, u8 Idx);

// =============================================================================
// sxr_CheckUpdateCluster
// -----------------------------------------------------------------------------
/// Check clusters load and consistency in debug mode.
/// @param  Ptr
/// @param  ClusterIdx
// =============================================================================
void sxr_CheckUpdateCluster (u8 *Ptr, u8 ClusterIdx);
#else
#define sxr_CheckUpdateHeap(a,b)
#define sxr_CheckUpdateCluster(a,b)
#endif

// =============================================================================
// sxr_MemoryAlertDispatchRegistration
// -----------------------------------------------------------------------------
/// Register a call back for the memory level notification.
/// @param  CallBack
// =============================================================================
void sxr_MemoryAlertDispatchRegistration (void (*CallBack)(u32));

#define SXR_MEMORY_LEVEL_OK  0
#define SXR_MEMORY_LEVEL_LOW 1


#define _malloc(Size) _sxr_HMalloc(Size, PAL_NB_RAM_AREA)
#define _free(p)      sxr_Free(p)

#undef DefExtern

#ifdef __cplusplus
}
#endif

#endif

/// @} // <-- End of sx_mem group
