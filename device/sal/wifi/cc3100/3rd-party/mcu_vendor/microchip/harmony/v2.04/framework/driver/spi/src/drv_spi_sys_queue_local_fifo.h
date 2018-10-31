/*******************************************************************************
  SPI Driver Interface

  Company:
    Microchip Technology Inc.

  File Name:
    drv_spi_sys_queue_local_fifo.h

  Summary:
    SPI command queue

  Description:
    This file provides the interface to the queue model that the SPI driver uses
*******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2014 released Microchip Technology Inc.  All rights reserved.

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

#ifndef _SPI_DRV_SYS_QUEUE_LOCAL_FIFO_H_
#define _SPI_DRV_SYS_QUEUE_LOCAL_FIFO_H_

#include <stdint.h>
#include <stdbool.h>
#include "system_config.h"
#include "osal/osal.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _SPI_DRV_SYS_QUEUE_TRACKING
#define _SPI_DRV_SYS_QUEUE_TRACKING 0
#endif

typedef struct _DRV_SPI_SYS_QUEUE_FIFO_ELEMENT_DATA
{
    struct _DRV_SPI_SYS_QUEUE_FIFO_ELEMENT_DATA * pNext;
}DRV_SPI_SYS_QUEUE_FIFO_ELEMENT_DATA;

typedef struct _DRV_SPI_SYS_QUEUE_QUEUE_DATA
{
    void * pQueueManager;
    DRV_SPI_SYS_QUEUE_FIFO_ELEMENT_DATA * pHead;
    DRV_SPI_SYS_QUEUE_FIFO_ELEMENT_DATA * pTail;
    DRV_SPI_SYS_QUEUE_INTERUPT_CHANGE fptrIntChange;
    size_t numReserved;
    size_t maxElements;
    size_t numAlloc;
    OSAL_SEM_HANDLE_TYPE semaphore;
    bool createdSemaphore;
    
    struct _DRV_SPI_SYS_QUEUE_QUEUE_DATA * pNext;
    
#if _SPI_DRV_SYS_QUEUE_TRACKING
    size_t numEnqueued;
    size_t numAllocOps;
    size_t numFreeOps;
    size_t numDequeueOps;
    size_t numEnqueueOps;
    size_t numReserveLW;
    size_t numAllocHW;
    size_t numEnqueuedHW;    
    size_t outOfMemoryErrors;
#endif
}DRV_SPI_SYS_QUEUE_QUEUE_DATA;

typedef struct _DRV_SPI_SYS_QUEUE_QUEUE_MANAGER_DATA
{
    DRV_SPI_SYS_QUEUE_QUEUE_DATA * pQueueArea;
    DRV_SPI_SYS_QUEUE_QUEUE_DATA * pFreeQueueHead;
    DRV_SPI_SYS_QUEUE_QUEUE_DATA * pFreeQueueTail;
    
    DRV_SPI_SYS_QUEUE_FIFO_ELEMENT_DATA * pElementArea;
    DRV_SPI_SYS_QUEUE_FIFO_ELEMENT_DATA * pFreeElementHead;
    DRV_SPI_SYS_QUEUE_FIFO_ELEMENT_DATA * pFreeElementTail;
    
    size_t numReserveElements;

    OSAL_SEM_HANDLE_TYPE semaphore;
    bool createdSemaphore;
    size_t numFreeElements;

    
#if _SPI_DRV_SYS_QUEUE_TRACKING   
    size_t numAllocOps;
    size_t numFreeOps;
    size_t numQueueCreateOps;
    size_t numQueueDestroyOps;
    size_t freeElementsLW;
    size_t reserveElementsLW;
    size_t outOfMemoryErrors;
    uint8_t numQueues;
    uint8_t numQueuesHW;
#endif
}DRV_SPI_SYS_QUEUE_QUEUE_MANAGER_DATA;


#ifdef __cplusplus
}
#endif


#endif