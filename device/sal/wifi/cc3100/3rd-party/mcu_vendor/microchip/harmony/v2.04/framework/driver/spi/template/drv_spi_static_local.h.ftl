/*******************************************************************************
  SPI Driver Local Data Structures for static implementation

  Company:
    Microchip Technology Inc.

  File Name:
    drv_spi_static_local.h

  Summary:
    SPI Driver Local Data Structures for static implementation

  Description:
    Driver Local Data Structures for static implementation
*******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2015 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute Software
only when embedded on a Microchip microcontroller or digital  signal  controller
that is integrated into your product or third party  product  (pursuant  to  the
sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED AS IS  WITHOUT  WARRANTY  OF  ANY  KIND,
EITHER EXPRESS  OR  IMPLIED,  INCLUDING  WITHOUT  LIMITATION,  ANY  WARRANTY  OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A  PARTICULAR  PURPOSE.
IN NO EVENT SHALL MICROCHIP OR  ITS  LICENSORS  BE  LIABLE  OR  OBLIGATED  UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION,  BREACH  OF  WARRANTY,  OR
OTHER LEGAL  EQUITABLE  THEORY  ANY  DIRECT  OR  INDIRECT  DAMAGES  OR  EXPENSES
INCLUDING BUT NOT LIMITED TO ANY  INCIDENTAL,  SPECIAL,  INDIRECT,  PUNITIVE  OR
CONSEQUENTIAL DAMAGES, LOST  PROFITS  OR  LOST  DATA,  COST  OF  PROCUREMENT  OF
SUBSTITUTE  GOODS,  TECHNOLOGY,  SERVICES,  OR  ANY  CLAIMS  BY  THIRD   PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE  THEREOF),  OR  OTHER  SIMILAR  COSTS.
*******************************************************************************/
//DOM-IGNORE-END

#ifndef _DRV_SPI_STATIC_LOCAL_H
#define _DRV_SPI_STATIC_LOCAL_H


// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
#include "system_config.h"
#include "driver/spi/drv_spi.h"
<#if CONFIG_USE_3RDPARTY_RTOS ==true>
#include "osal/osal.h"
</#if>
#include "system/int/sys_int.h"
#include "system/debug/sys_debug.h"
#include "driver/spi/static/src/drv_spi_static_sys_queue.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Data Type Definitions
// *****************************************************************************
// *****************************************************************************
#define MAX(a,b) ((a<b) ? b : a)
#define MIN(a,b) ((b<a) ? b : a)

#define PLIB_SPI_RX_8BIT_FIFO_SIZE(index)    16
#define PLIB_SPI_RX_8BIT_HW_MARK(index)      8
#define PLIB_SPI_RX_16BIT_FIFO_SIZE(index)   8
#define PLIB_SPI_RX_16BIT_HW_MARK(index)     4
#define PLIB_SPI_RX_32BIT_FIFO_SIZE(index)   4
#define PLIB_SPI_RX_32BIT_HW_MARK(index)     2


#define PLIB_SPI_TX_8BIT_FIFO_SIZE(index)   16
#define PLIB_SPI_TX_8BIT_LW_MARK(index)     8
#define PLIB_SPI_TX_16BIT_FIFO_SIZE(index)  8
#define PLIB_SPI_TX_16BIT_LW_MARK(index)    4
#define PLIB_SPI_TX_32BIT_FIFO_SIZE(index)  4
#define PLIB_SPI_TX_32BIT_LW_MARK(index)    2

struct DRV_SPI_OBJ;

<#if CONFIG_DRV_SPI_USE_DMA == true>
/* DMA related data. */
#define PLIB_DMA_MAX_TRF_SIZE 2048
extern uint8_t sSPITxDummyBuffer[DRV_SPI_DMA_DUMMY_BUFFER_SIZE];
extern uint8_t sSPIRxDummyBuffer[DRV_SPI_DMA_DUMMY_BUFFER_SIZE];

extern uint8_t sDrvSpiRxDummy[DRV_SPI_DMA_DUMMY_BUFFER_SIZE];
extern uint8_t sDrvSpiTxDummy[DRV_SPI_DMA_DUMMY_BUFFER_SIZE];

typedef enum {
    DRV_SPI_DMA_NONE = 0,
    DRV_SPI_DMA_DATA_INPROGRESS,
    DRV_SPI_DMA_DUMMY_INPROGRESS,
    DRV_SPI_DMA_COMPLETE,
} DRV_SPI_DMA_STATE;

</#if>
/* SPI Job object. */
typedef struct _DRV_SPI_JOB_OBJECT
{
    uint8_t *txBuffer;
    uint8_t *rxBuffer;
    size_t dataTxed;
    size_t dataRxed;
    size_t dataLeftToTx;
    size_t dummyLeftToTx;
    size_t dataLeftToRx;
    size_t dummyLeftToRx;
<#if CONFIG_DRV_SPI_USE_DMA == true>
    DRV_SPI_DMA_STATE txDMAProgressStage;
    DRV_SPI_DMA_STATE rxDMAProgressStage;
</#if>
    DRV_SPI_BUFFER_EVENT status;
    DRV_SPI_BUFFER_EVENT_HANDLER completeCB;
    void * context;
}DRV_SPI_JOB_OBJECT;

// *****************************************************************************
/* SPI Static Driver Instance Object

  Summary:
    Object used to keep any data required for the static SPI driver.

  Description:
    This object is used to keep track of any data that must be maintained to
    manage the SPI static driver.

  Remarks:
    None.
*/

typedef struct DRV_SPI_OBJ
{
<#if CONFIG_DRV_SPI_USE_STANDARD_BUFFER_MODE && CONFIG_DRV_SPI_USE_POLLED_MODE>
    uint8_t                                     numTrfsSmPolled;
</#if>
    bool                                        errEnabled;
<#if CONFIG_DRV_SPI_USE_CLIENT_CONFIG_FUCNTION == true>
    DRV_SPI_BUFFER_EVENT_HANDLER                operationStarting;
    DRV_SPI_BUFFER_EVENT_HANDLER                operationEnded;
</#if>
    DRV_SPI_SYS_QUEUE_HANDLE                    queue;
    DRV_SPI_JOB_OBJECT *                        currentJob;
    bool                                        txEnabled;
    bool                                        rxEnabled;
    uint8_t                                     symbolsInProgress;
    uint32_t                                    dummyByteValue;
<#if CONFIG_DRV_SPI_USE_DMA == true>
    SYS_DMA_CHANNEL_TRANSFER_EVENT_HANDLER      sendDMAHander;
    SYS_DMA_CHANNEL_TRANSFER_EVENT_HANDLER      receiveDMAHander;
    SYS_DMA_CHANNEL_HANDLE                      txDmaChannelHandle;
    /* Threshold for the minimum number of bytes to send to use DMA*/
    uint8_t                                     txDmaThreshold;
    SYS_DMA_CHANNEL_HANDLE                      rxDmaChannelHandle;
    /* Threshold for the minimum number of bytes to receive to use DMA*/
    uint8_t                                     rxDmaThreshold;
</#if>
<#if CONFIG_USE_3RDPARTY_RTOS ==true>
    OSAL_MUTEX_DECLARE(mutexDriverInstance);
    OSAL_SEM_DECLARE(semaphoreDriverInstance);
</#if>
} DRV_SPI_OBJ;

// *****************************************************************************
// *****************************************************************************
// Section: Queue related Data Type Definitions
// *****************************************************************************
// *****************************************************************************
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
<#if CONFIG_USE_3RDPARTY_RTOS ==true>
    OSAL_SEM_HANDLE_TYPE semaphore;
    bool createdSemaphore;
</#if>
    struct _DRV_SPI_SYS_QUEUE_QUEUE_DATA * pNext;
    size_t numEnqueued;
    size_t numAllocOps;
    size_t numFreeOps;
    size_t numDequeueOps;
    size_t numEnqueueOps;
    size_t numReserveLW;
    size_t numAllocHW;
    size_t numEnqueuedHW;
    size_t outOfMemoryErrors;
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
<#if CONFIG_USE_3RDPARTY_RTOS ==true>
    OSAL_SEM_HANDLE_TYPE semaphore;
    bool createdSemaphore;
</#if>
    size_t numFreeElements;
    size_t numAllocOps;
    size_t numFreeOps;
    size_t numQueueCreateOps;
    size_t numQueueDestroyOps;
    size_t freeElementsLW;
    size_t reserveElementsLW;
    size_t outOfMemoryErrors;
    uint8_t numQueues;
    uint8_t numQueuesHW;
}DRV_SPI_SYS_QUEUE_QUEUE_MANAGER_DATA;

// *****************************************************************************
// *****************************************************************************
// Section: Local functions.
// *****************************************************************************
// *****************************************************************************
<#macro DRV_SPI_STATIC_FUNCTIONS DRV_INSTANCE>
<#assign DRV_MODE="Master">
<#assign DRV_ISR="ISR">
<#assign DRV_BUFFER="RM">
<#assign DRV_BIT_WIDTH="8">
<#if .vars["CONFIG_DRV_SPI_SPI_MODE_SLAVE_IDX${DRV_INSTANCE}"] == true>
    <#assign DRV_MODE="Slave">
</#if>
<#if .vars["CONFIG_DRV_SPI_TASK_MODE_POLLED_IDX${DRV_INSTANCE}"] == true>
    <#assign DRV_ISR="Polled">
</#if>
<#if .vars["CONFIG_DRV_SPI_BUFFER_ENHANCED_IDX${DRV_INSTANCE}"] == true>
    <#assign DRV_BUFFER="EBM">
</#if>
<#if .vars["CONFIG_DRV_SPI_COMM_WIDTH_16_BIT_IDX${DRV_INSTANCE}"] == true>
    <#assign DRV_BIT_WIDTH="16">
</#if>
<#if .vars["CONFIG_DRV_SPI_COMM_WIDTH_32_BIT_IDX${DRV_INSTANCE}"] == true>
    <#assign DRV_BIT_WIDTH="32">
</#if>

int32_t DRV_SPI${DRV_INSTANCE}_${DRV_ISR}${DRV_MODE}${DRV_BUFFER}${DRV_BIT_WIDTH}BitTasks ( struct DRV_SPI_OBJ * dObj );
int32_t DRV_SPI${DRV_INSTANCE}_${DRV_ISR}ErrorTasks(struct DRV_SPI_OBJ * dObj);
int32_t DRV_SPI${DRV_INSTANCE}_${DRV_MODE}${DRV_BUFFER}Send${DRV_BIT_WIDTH}Bit${DRV_ISR}( struct DRV_SPI_OBJ * dObj );
int32_t DRV_SPI${DRV_INSTANCE}_${DRV_MODE}${DRV_BUFFER}Receive${DRV_BIT_WIDTH}Bit${DRV_ISR}( struct DRV_SPI_OBJ * dObj );
<#if CONFIG_DRV_SPI_USE_DMA == true>
/*Local functions for DMA*/
void DRV_SPI${DRV_INSTANCE}_${DRV_ISR}DMA${DRV_MODE}SendEventHandler${DRV_BIT_WIDTH}bit(SYS_DMA_TRANSFER_EVENT event, SYS_DMA_CHANNEL_HANDLE handle, uintptr_t contextHandle);
void DRV_SPI${DRV_INSTANCE}_${DRV_ISR}DMA${DRV_MODE}ReceiveEventHandler${DRV_BIT_WIDTH}bit(SYS_DMA_TRANSFER_EVENT event, SYS_DMA_CHANNEL_HANDLE handle, uintptr_t contextHandle);
void DRV_SPI${DRV_INSTANCE}_SetupDMA( struct DRV_SPI_OBJ * dObj);
</#if>
</#macro>
<#if CONFIG_DRV_SPI_IDX0 == true>
    <@DRV_SPI_STATIC_FUNCTIONS DRV_INSTANCE="0"/>
</#if>
<#if CONFIG_DRV_SPI_IDX1 == true>
    <@DRV_SPI_STATIC_FUNCTIONS DRV_INSTANCE="1"/>
</#if>
<#if CONFIG_DRV_SPI_IDX2 == true>
    <@DRV_SPI_STATIC_FUNCTIONS DRV_INSTANCE="2"/>
</#if>
<#if CONFIG_DRV_SPI_IDX3 == true>
    <@DRV_SPI_STATIC_FUNCTIONS DRV_INSTANCE="3"/>
</#if>
<#if CONFIG_DRV_SPI_IDX4 == true>
    <@DRV_SPI_STATIC_FUNCTIONS DRV_INSTANCE="4"/>
</#if>
<#if CONFIG_DRV_SPI_IDX5 == true>
    <@DRV_SPI_STATIC_FUNCTIONS DRV_INSTANCE="5"/>
</#if>
// DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
// DOM-IGNORE-END

#endif //#ifndef _DRV_SPI_STATIC_LOCAL_H

/*******************************************************************************
 End of File
*/

