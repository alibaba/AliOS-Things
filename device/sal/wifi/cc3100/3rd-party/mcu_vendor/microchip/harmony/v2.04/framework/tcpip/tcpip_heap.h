/*******************************************************************************
  Microchip TCP/IP Stack Heap Definitions

  Company:
    Microchip Technology Inc.
    
  File Name:
    tcpip_heap.h

  Summary:
    This is the TCP/IP heap definitions.

  Description:
    The TCP/IP heap provides access to the stack heaps, configuration and status.

*******************************************************************************/
//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright 2012-2015 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY KIND,
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
//DOM-IGNORE-END

#ifndef __TCPIP_HEAP_H_
#define __TCPIP_HEAP_H_

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END  






// *****************************************************************************
/*
  Type:
    TCPIP_STACK_HEAP_HANDLE

  Summary:
    Defines a TCP/IP stack heap handle type.

  Description:
    Definition of the heap handle which clients
    use to get access to heap statistics functions.
	
  Remarks:
    None.

*/

typedef const void* TCPIP_STACK_HEAP_HANDLE;

// *****************************************************************************
/*
  Type:
    TCPIP_STACK_HEAP_TYPE

  Summary:
    Defines a TCP/IP stack heap type.

  Description:
    Definition of the heap type which clients
    could use to get access to heap statistics functions.

 Remarks:
    New types could be added in the future.
*/

typedef enum
{
    /* invalid, unknown heap type */
    TCPIP_STACK_HEAP_TYPE_NONE   = 0,

    /* internally implemented heap */
    /* Currently the default heap type */
    /* The default internal memory heap consists of block of data that is */
    /* allocated in chunks using a first fit algorithm. */
    /* Allocation and deallocation operations are not very fast */
    /* (but reasonably fast). */
    /* However, the allocation granularity is pretty good. */
    /* This type of heap can become fragmented over time */
    /* There is no data maintained in this heap on a per client basis. */
    /* What this means is that is OK to pass the heap handle to other */
    /* clients (software modules) to use it. */
    /* Note: this is a private TCPIP heap */
    /* and multi-threaded protection is provided internally. */
    TCPIP_STACK_HEAP_TYPE_INTERNAL_HEAP,      
                                          
    /* internally implemented pool heap */
    /* Very fast allocation and deallocation without fragmentation */
    /* However the allocated chunks are fixed size
       so it can result in a not very efficient memory usage */
    /* Note: this is a private TCPIP heap */
    /* and multi-threaded protection is provided internally. */
    TCPIP_STACK_HEAP_TYPE_INTERNAL_HEAP_POOL, 
                          
    /* external/system maintained heap */
    /* The heap is maintained externally and allocation
       is done using the supplied functions.
       The TCP/IP stack heap can provide OSAL synchronization
       protection if needed */
    /* Not supported yet */
    TCPIP_STACK_HEAP_TYPE_EXTERNAL_HEAP,              
}TCPIP_STACK_HEAP_TYPE;

// *****************************************************************************
/*
  Type:
    TCPIP_STACK_HEAP_USAGE

  Summary:
    Defines a TCP/IP stack heap purpose.

  Description:
    Definition of the heap usage/purpose.
    It defines what modules in the TCP/IP stack will use
    this heap.

 Remarks:
    This is an advanced definition that is currently not used.
    Multiple heaps usage will be added in the future.
    For now only one heap type is supported and its used
    for all allocation operations.

    New usages could be eventually added.

    Multiple usage flags can be simultaneously set.
*/

typedef enum
{
    /* Default heap usage */
    TCPIP_STACK_HEAP_USE_DEFAULT    = 0x00,      

    /* Generic heap usage */
    TCPIP_STACK_HEAP_USE_GENERIC    = 0x01,      
                                          
    /* Heap for packet allocations */
    TCPIP_STACK_HEAP_USE_PACKET     = 0x02, 
                          
    /* Heap for TCP sockets */
    TCPIP_STACK_HEAP_USE_SOCKET_TCP = 0x04,              

    /* Heap for UDP sockets */
    TCPIP_STACK_HEAP_USE_SOCKET_UDP = 0x08,              

    /* Heap for UDP TX pool buffers */
    TCPIP_STACK_HEAP_USE_POOL_UDP   = 0x10,


}TCPIP_STACK_HEAP_USAGE;

// *****************************************************************************
/*
  Type:
    TCPIP_STACK_HEAP_RES

  Summary:
    Defines the results of a TCP/IP heap error function.

  Description:
    Defines the results that could be returned by the TCP/IP stack heap
    error function.

 Remarks:
    New results could be added in the future.

    Some results are relevant for specific heap types only.
*/

typedef enum
{
    // everything OK
    TCPIP_STACK_HEAP_RES_OK               = 0,   
    
    /* error codes */

    /* invalid initialization data */
    TCPIP_STACK_HEAP_RES_INIT_ERR         = -1,   

    /* could not create a heap */
    TCPIP_STACK_HEAP_RES_CREATE_ERR       = -2,   

    /* could not create a synchronization object or synchronization error */
    TCPIP_STACK_HEAP_RES_SYNCH_ERR        = -3,   

    /* supplied heap buffer too small */
    TCPIP_STACK_HEAP_RES_BUFF_SIZE_ERR    = -4,   
    
    /* supplied heap buffer not properly aligned */
    TCPIP_STACK_HEAP_RES_BUFF_ALIGN_ERR   = -5,   
    
    /* heap is out of memory */
    TCPIP_STACK_HEAP_RES_NO_MEM           = -6,   

    /* heap in use, cannot be deallocated */
    TCPIP_STACK_HEAP_RES_IN_USE           = -7,   
    
    /* corrupt pointer */
    TCPIP_STACK_HEAP_RES_PTR_ERR          = -8, 

    /* no such heap exists */
    TCPIP_STACK_HEAP_RES_NO_HEAP          = -9,   

    /* this size can not be allocated for a pool heap */
    TCPIP_STACK_HEAP_RES_SIZE_ERR         = -10,   

}TCPIP_STACK_HEAP_RES;   


// *****************************************************************************
/*
  Type:
    TCPIP_STACK_HEAP_FLAGS

  Summary:
    Defines the flags supported by a TCP/IP heap.

  Description:
    Defines the flags supported by a TCP/IP heap creation function.

 Remarks:
    New flags could be added in the future.

    16 bit only flags supported!

*/

typedef enum
{
    TCPIP_STACK_HEAP_FLAG_NONE              = 0x00,

    // the heap needs to provide non cached buffers
    // default is enabled
    // Note: the TCP/IP stack always uses non-cached buffers
    // for the network packets.
    // This flag will be set internally whenever needed. 
    TCPIP_STACK_HEAP_FLAG_ALLOC_UNCACHED    = 0x01,
    

    // TCPIP_STACK_HEAP_TYPE_EXTERNAL_HEAP type specific flag:
    // when an external heap functions are used
    // this flags suppresses the multi-threaded synchronization
    // directives and assumes that the protection is done by the
    // external heap manager 
    // default is disabled
    TCPIP_STACK_HEAP_FLAG_NO_MTHREAD_SYNC   = 0x02,

    // TCPIP_STACK_HEAP_TYPE_EXTERNAL_HEAP type specific flag:
    // when an external heap functions are used
    // this flags suppresses the internal implementation of buffer alignment
    // required by the TCP/IP stack.
    // Therefore the external allocation functions need to provide aligned buffers
    // (usually on the cache line on cached platforms).
    // The allocation function will return failure if the flag is turned on
    // and the returned buffer is not properly aligned.
    // default is disabled
    TCPIP_STACK_HEAP_FLAG_ALLOC_UNALIGN     = 0x04,

    // TCPIP_STACK_HEAP_TYPE_INTERNAL_HEAP_POOL type specific flag:
    // If the strict flag is enabled, the allocation will be tried
    // strictly from the pool entry that matches the requested size.
    // Otherwise, all the pool entries that have blocks larger than the requested size
    // will be tried.
    TCPIP_STACK_HEAP_FLAG_POOL_STRICT       = 0x08,


    // when debugging is enabled, do not issue a warning when
    // a memory allocation operation fails
    // default is disabled
    TCPIP_STACK_HEAP_FLAG_NO_WARN_MESSAGE   = 0x10,


}TCPIP_STACK_HEAP_FLAGS;


//*******************************************************************************
/* TCP/IP Heap Configuration Data

  Summary:
    Defines the data required to initialize the TCP/IP stack heap.

  Description:
    This data type defines the data required to initialize the TCP/IP stack heap.

  Remarks:
    The TCPIP_STACK_HEAP_FLAG_ALLOC_UNCACHED heap flag will be internally set as needed 

    The heapUsage field is currently not used but it's reserved for further development.

    The malloc_fnc/calloc_fnc/free_fnc are used to allocate the heap objects themselves
    plus the actual heap space (for internal and pool heap types).
    For the external heap type these functions are directly used to perform the memory allocation operations.

*/
typedef struct
{
    TCPIP_STACK_HEAP_TYPE   heapType;       // type of this heap: TCPIP_STACK_HEAP_TYPE_INTERNAL_HEAP, TCPIP_STACK_HEAP_TYPE_EXTERNAL_HEAP, etc.
    TCPIP_STACK_HEAP_FLAGS  heapFlags;      // heap creation flags
                                            //
    TCPIP_STACK_HEAP_USAGE  heapUsage;      // the usage intended for                                            
    void* (*malloc_fnc)(size_t bytes);      // malloc style allocation function
    void* (*calloc_fnc)(size_t nElems, size_t elemSize);      // calloc style allocation function
    void  (*free_fnc)(void* ptr);           // free style allocation function

    // specific heap parameters per heap type

}TCPIP_STACK_HEAP_CONFIG;

//*******************************************************************************
/* Internal Heap Configuration Data

  Summary:
    Defines the data required to initialize the TCP/IP stack internal heap.

  Description:
    This data type defines the data required to initialize the TCP/IP stack internal heap.

  Remarks:
    None.
*/
typedef struct
{
    // the TCPIP_STACK_HEAP_CONFIG members
    TCPIP_STACK_HEAP_TYPE   heapType;       // type of this heap: TCPIP_STACK_HEAP_TYPE_INTERNAL_HEAP
    TCPIP_STACK_HEAP_FLAGS  heapFlags;      // heap creation flags
                                            // TCPIP_STACK_HEAP_FLAG_ALLOC_UNCACHED will be always internally set 
                                            //
    TCPIP_STACK_HEAP_USAGE  heapUsage;      // currently not used                                            
    void* (*malloc_fnc)(size_t bytes);      // malloc style function for allocating the internal heap itself
    void* (*calloc_fnc)(size_t nElems, size_t elemSize);      // calloc style function for allocating the internal heap itself
    void  (*free_fnc)(void* ptr);           // free style function for releasing the allocated internal heap

    // specific internal heap parameters
    size_t                  heapSize;       // size of the internal heap to be created and maintained


}TCPIP_STACK_HEAP_INTERNAL_CONFIG;

//*******************************************************************************
/* External Heap Configuration Data

  Summary:
    Defines the data required to initialize the TCP/IP stack external heap.

  Description:
    This data type defines the data required to initialize the TCP/IP stack external heap.

  Remarks:
    Due to the internal alignment  restrictions the externally supplied calloc function is not currently used.
    Instead, a less efficient approach is taken: the malloc function is used and the resulted buffer is zeroed out.
*/
typedef struct
{
    // the TCPIP_STACK_HEAP_CONFIG members
    TCPIP_STACK_HEAP_TYPE   heapType;       // type of this heap: TCPIP_STACK_HEAP_TYPE_EXTERNAL_HEAP
    TCPIP_STACK_HEAP_FLAGS  heapFlags;      // heap creation flags
                                            // TCPIP_STACK_HEAP_FLAG_ALLOC_UNCACHED will be always internally set 
                                            // TCPIP_STACK_HEAP_FLAG_NO_MTHREAD_SYNC should be cleared if the external heap functions
                                            // provide multi-threaded synchronization. Should be set if the synchronization needs to be 
                                            // provided by the TCP/IP heap implementation.
                                            // TCPIP_STACK_HEAP_FLAG_ALLOC_UNALIGN should be set if the external heap functions
                                            // provide cache line aligned buffers.
                                            //
    TCPIP_STACK_HEAP_USAGE  heapUsage;      // currently not used                                            
    void* (*malloc_fnc)(size_t bytes);      // malloc style function to be called when allocation is needed
    void* (*calloc_fnc)(size_t nElems, size_t elemSize);      // calloc style function to be called when allocation is needed
    void  (*free_fnc)(void* ptr);           // free style function for releasing the allocated memory

    // specific external heap parameters


}TCPIP_STACK_HEAP_EXTERNAL_CONFIG;


//*******************************************************************************
/* Internal Pool Configuration Data

  Summary:
    Defines the data required to initialize the TCP/IP stack internal memory pool.

  Description:
    This data type defines the data required to initialize the TCP/IP stack internal memory pool.

  Remarks:
  
    Pool entries should follow these 2 rules:
        - They should have distinct block sizes: entrySize.
          Multiple entries having the same entrySize is not supported.
        - Pool entries should have a non zero block size.
          Entries with 0 block size are not supported.
    If these conditions are not observed the heap creation will fail!


    To expand/dynamically allocate at run time the expansionHeapSize needs to be != 0;
    If nExpBlks == 0, then no allocation will be tried for this particular entry.
*/

typedef struct
{
    // size of the blocks in this pool entry
    uint16_t    entrySize;
    // number of blocks to create for this entry
    uint16_t    nBlocks;
    // number of blocks to expand/allocate when an allocation in this entry failed
    uint8_t     nExpBlks;
}TCPIP_STACK_HEAP_POOL_ENTRY;


typedef struct
{
    // the TCPIP_STACK_HEAP_CONFIG members
    TCPIP_STACK_HEAP_TYPE   heapType;       // type of this heap: TCPIP_STACK_HEAP_TYPE_INTERNAL_HEAP_POOL
    TCPIP_STACK_HEAP_FLAGS  heapFlags;      // heap creation flags
                                            // TCPIP_STACK_HEAP_FLAG_ALLOC_UNCACHED will be always internally set 
                                            //
    TCPIP_STACK_HEAP_USAGE heapUsage;       // not used                                            
    void* (*malloc_fnc)(size_t bytes);      // malloc style function for allocating the pool heap itself
    void* (*calloc_fnc)(size_t nElems, size_t elemSize);      // calloc style function for allocating the pool heap itself
    void  (*free_fnc)(void* ptr);           // free style function for releasing the allocated pool heap

    // specific pool heap parameters
    uint16_t                nPoolEntries;   // number of pool entries that the pool will contain
                                            // (this cannot be changed after the creation)
    TCPIP_STACK_HEAP_POOL_ENTRY *pEntries;  // pointer to array of TCPIP_STACK_HEAP_POOL_ENTRY specifying
                                            // each entry size and blocks
    uint16_t                expansionHeapSize; // size of the heap to be allocated separately
                                            // and used at run time for allocation of new blocks - in bytes
                                            // This is heap space besides the size resulting from the pEntries array
                                            // This expansionHeapSize block will be used for allocations on new blocks when
                                            // an entry runs out of blocks and nExpBlks != 0.
}TCPIP_STACK_HEAP_POOL_CONFIG;

//*******************************************************************************
/*
  Function:
    TCPIP_STACK_HEAP_HANDLE TCPIP_STACK_HeapHandleGet(TCPIP_STACK_HEAP_TYPE heapType, 
	                                                  int heapIndex)

  Summary:
    Returns the current TCP/IP stack heap handle.

  Description:
    This function returns the current stack heap handle.
    It provides access to the heap statistics and monitoring functions.

  Precondition:
    None.

  Parameters:
    heapType    - type of heap to request a handle for
                    Currently only one type of heap is supported at run time
    heapIndex   - index of the heap; used when multiple heaps of the same type are created
                    Currently not used and it should be 0


  Returns:
    A handle to the current TCP/IP stack heap of the requested type; otherwise, 0 if an error
	occurred.

  Remarks:
    None.
  */
TCPIP_STACK_HEAP_HANDLE TCPIP_STACK_HeapHandleGet(TCPIP_STACK_HEAP_TYPE heapType, int heapIndex);

//*****************************************************************************
/* Function:
     size_t TCPIP_STACK_HEAP_Size(TCPIP_STACK_HEAP_HANDLE heapH);
	 
   Summary:
    Returns the size of the heap.
   
   Description:
    This function returns the size of the heap. This is the size that was 
	specified when the heap was created.
   
   Preconditions:
    heapH       - valid heap handle
   
   Parameters:
    heapH       - handle to a heap
   
   Returns:
    The size of heap that was specified when it was created.
   
   Remarks:
    None.
*/
size_t                 TCPIP_STACK_HEAP_Size(TCPIP_STACK_HEAP_HANDLE heapH);

//*****************************************************************************
/* Function:
    size_t TCPIP_STACK_HEAP_MaxSize(TCPIP_STACK_HEAP_HANDLE heapH);
	 
   Summary:
    Returns the maximum size of a block that can be currently allocated from this heap.
   
   Description:
    The function returns the maximum size of a block that can be currently allocated 
	from this heap.
   
   Preconditions:
    heapH       - valid heap handle
   
   Parameters:
    heapH       - handle to a heap
   
   Returns:
    The maximum size of a block that can be allocated from the heap.
   
   Remarks:
    This is info only. It can change is the heap has multiple clients. 
	The call could be expensive.
    The entire heap has to be traversed to find the maximum.
    If the heap is really fragmented this might take some time.
*/
size_t                 TCPIP_STACK_HEAP_MaxSize(TCPIP_STACK_HEAP_HANDLE heapH);

//*****************************************************************************
/* Function:
    size_t TCPIP_STACK_HEAP_FreeSize(TCPIP_STACK_HEAP_HANDLE heapH);
	 
   Summary:
    Returns the size of the space currently available in the heap.
   
   Description:
    The function returns the size of the space currently available in the heap.
   
   Preconditions:
    heapH       - valid heap handle
   
   Parameters:
    heapH       - handle to a heap
   
   Returns:
    The size of the available space in the heap.
   
   Remarks:
     None.
*/ 
size_t                 TCPIP_STACK_HEAP_FreeSize(TCPIP_STACK_HEAP_HANDLE heapH);

//*****************************************************************************
/* Function:      
    TCPIP_STACK_HEAP_RES TCPIP_STACK_HEAP_LastError(TCPIP_STACK_HEAP_HANDLE heapH)
	
   Summary:
    Returns the last error encountered in a heap operation and clears the value
	of the last error variable.
   
   Description:
    The function returns the last error encountered in a heap operation.
    It clears the value of the last error variable.
	
   Precondition:    
    heapH       - valid heap handle
 
   Parameters:           
    heapH       - handle of a heap

   Returns:
    The last error encountered in an operation or TCPIP_STACK_HEAP_RES_OK if no 
	error occurred.

   Remarks:
     The heap holds an error variable storing the last error encountered in an operation.
     This should be consulted by the caller after each operation
     that returns an invalid result for checking what the error condition
     was.
          
*/
TCPIP_STACK_HEAP_RES      TCPIP_STACK_HEAP_LastError(TCPIP_STACK_HEAP_HANDLE heapH);

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif  // __TCPIP_HEAP_H_

