/*******************************************************************************
  SPI Driver Interface Implementation

  Company:
    Microchip Technology Inc.

  File Name:
   drv_spi.c

  Summary:
 SPI Driver implementation

  Description:
    The SPI Driver provides a interface to access the SPI hardware on the PIC32
    microcontroller.  This file implements the SPI Driver. This file
    should be included in the project if SPI driver functionality is needed.
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
//DOM-IGNORE-END

#include <string.h>
#include "driver/spi/src/dynamic/drv_spi_internal.h"

/* Statically Allocated stuff*/

static DRV_SPI_MANAGER_OBJECT sSPIManager;
static struct DRV_SPI_DRIVER_OBJECT sSPIDriverInstances[DRV_SPI_INSTANCES_NUMBER];
static DRV_SPI_CLIENT_OBJECT sSPIClientInstances[DRV_SPI_CLIENTS_NUMBER];

static uint8_t sDrvSPIQueueArea[DRV_SPI_SYS_QUEUE_BUFFER_SIZE(DRV_SPI_INSTANCES_NUMBER, sizeof(DRV_SPI_JOB_OBJECT), DRV_SPI_INSTANCES_NUMBER * DRV_SPI_ELEMENTS_PER_QUEUE)];
static uint8_t sNumDriverInit = 0;



// Bring in the API functions
#include "driver/spi/drv_spi.h"



static DRV_SPI_SYS_QUEUE_MANAGER_SETUP qmInitData = {
    sDrvSPIQueueArea,
    sizeof(sDrvSPIQueueArea),
    DRV_SPI_INSTANCES_NUMBER,
    sizeof(DRV_SPI_JOB_OBJECT),
    DRV_SPI_SYS_QUEUE_Fifo,
    false,
    0
};

static DRV_SPI_SYS_QUEUE_SETUP qInitData =
{
    0,
    10,
    0,
    false,
    0
};

static int32_t _DRV_SPI_RootDriverInitialization()
{
    memset(&sSPIManager, 0, sizeof(sSPIManager));
    memset(&sSPIDriverInstances, 0, sizeof(sSPIDriverInstances));
    memset(&sSPIClientInstances, 0, sizeof(sSPIClientInstances));
    //memset(&testDebugBuffer, 0, sizeof(testDebugBuffer));

    if (OSAL_SEM_Create(&sSPIManager.managerSemaphore, OSAL_SEM_TYPE_BINARY, 1, 1) != OSAL_RESULT_TRUE) {/*Report Error*/}
    qmInitData.semaphoreToUse = sSPIManager.managerSemaphore;
    if (DRV_SPI_SYS_QUEUE_Initialize(&qmInitData, &sSPIManager.hQueueManager) != DRV_SPI_SYS_QUEUE_SUCCESS)
    {
        SYS_ASSERT(false, "\r\nSPI Driver: Could not create queuing system.");
        return -1;
    }

    int8_t counter;
    for (counter = DRV_SPI_INSTANCES_NUMBER - 1; counter >= 0; counter--)
    {
        sSPIDriverInstances[counter].spiId = 0xff;
    }

    for (counter = DRV_SPI_CLIENTS_NUMBER - 1; counter >= 0; counter--)
    {
        sSPIClientInstances[counter].pNext = sSPIManager.pFreeClientHead;
        sSPIManager.pFreeClientHead = &sSPIClientInstances[counter];
    }
    return 0;

}

static int32_t _DRV_SPI_RootDriverDeinitialization()
{
    if (DRV_SPI_SYS_QUEUE_Deinitialize(sSPIManager.hQueueManager) != DRV_SPI_SYS_QUEUE_SUCCESS)
    {
        SYS_ASSERT(false, "\r\nSPI Driver: Could not destroy queuing system.");
        return -1;
    }
    if (OSAL_SEM_Delete(&(sSPIManager.managerSemaphore)) != OSAL_RESULT_TRUE) {/*Report Error*/}
    return 0;
}

SYS_MODULE_OBJ DRV_SPI_Initialize( const SYS_MODULE_INDEX index,
                                           const SYS_MODULE_INIT * const init )
{
    if (sNumDriverInit == 0)
    {
        if (_DRV_SPI_RootDriverInitialization() != 0)
        {
            return  SYS_MODULE_OBJ_INVALID;
        }
    }

    if (index >= DRV_SPI_INSTANCES_NUMBER)
    {
        SYS_ASSERT(false, "\r\nSPI Driver: Attempting to initialize an instance number greater than the max.");
        return SYS_MODULE_OBJ_INVALID;
    }

    struct DRV_SPI_DRIVER_OBJECT * pDrvInstance = ( struct DRV_SPI_DRIVER_OBJECT *)&sSPIDriverInstances[index];

    if (pDrvInstance->spiId != 0xff)
    {
        SYS_ASSERT(false, "\r\nSPI Driver: Attempting to reinitialize a driver instance that is already in use.");
        return SYS_MODULE_OBJ_INVALID;
    }

    const DRV_SPI_INIT * const pInit = (const DRV_SPI_INIT * const)init;

    uint8_t counter;

    for (counter = 0; counter < DRV_SPI_INSTANCES_NUMBER; counter ++)
    {
        if ((sSPIDriverInstances[counter].spiId == pInit->spiId))
        {
            SYS_ASSERT(false, "\r\nSPI Driver: trying to initialize two drivers with the same spiID.");
            if (sNumDriverInit == 0) _DRV_SPI_RootDriverDeinitialization();
            return SYS_MODULE_OBJ_INVALID;
        }
    }

    if (DRV_SPI_SetVTable(pDrvInstance, pInit) < 0)
    {
        // Assert should have already happened, don't need a second."
        if (sNumDriverInit == 0) _DRV_SPI_RootDriverDeinitialization();
        return SYS_MODULE_OBJ_INVALID;
    }

    if (DRV_SPI_SetupHardware(pDrvInstance, pInit) < 0)
    {
        // Assert should have already happened, don't need a second."
        if (sNumDriverInit == 0) _DRV_SPI_RootDriverDeinitialization();
        return SYS_MODULE_OBJ_INVALID;
    }

    qInitData.maxElements = pInit->queueSize;
    qInitData.reserveElements = pInit->jobQueueReserveSize;
    qInitData.semaphoreToUse = sSPIManager.managerSemaphore;

    if (DRV_SPI_SYS_QUEUE_CreateQueue(sSPIManager.hQueueManager, &qInitData, &pDrvInstance->queue) != DRV_SPI_SYS_QUEUE_SUCCESS)
    {
        SYS_ASSERT(false, "\r\nSPI Driver: Could not set up driver instance queue.");
        if (sNumDriverInit == 0) _DRV_SPI_RootDriverDeinitialization();
        return SYS_MODULE_OBJ_INVALID;

    }

    sNumDriverInit++;
    //By this point all the tests should have passed.
    pDrvInstance->spiId = pInit->spiId;
    pDrvInstance->status = SYS_STATUS_READY;
    pDrvInstance->txInterruptSource = pInit->txInterruptSource;
    pDrvInstance->rxInterruptSource = pInit->rxInterruptSource;
    pDrvInstance->errInterruptSource = pInit->errInterruptSource;
    pDrvInstance->operationStarting = pInit->operationStarting;
    pDrvInstance->operationEnded = pInit->operationEnded;
    pDrvInstance->taskMode = pInit->taskMode;
    pDrvInstance->numTrfsSmPolled = pInit->numTrfsSmPolled;
    pDrvInstance->commWidth = pInit->commWidth;
    pDrvInstance->baudRate = pInit->baudRate;
    pDrvInstance->spiClk = pInit->spiClk;
    pDrvInstance->dummyByteValue = pInit->dummyByteValue;
#if DRV_SPI_DMA
    pDrvInstance->txDmaChannel = pInit->txDmaChannel;
    pDrvInstance->txDmaThreshold = pInit->txDmaThreshold;
    pDrvInstance->txDmaChannelHandle = SYS_DMA_CHANNEL_HANDLE_INVALID;
    pDrvInstance->rxDmaChannel = pInit->rxDmaChannel;
    pDrvInstance->rxDmaThreshold = pInit->rxDmaThreshold;
    pDrvInstance->rxDmaChannelHandle = SYS_DMA_CHANNEL_HANDLE_INVALID;

#endif
    pDrvInstance->spiMode = pInit->spiMode;

#if DRV_SPI_DMA
    uint32_t i;

    /* initialize the array with user configured dummy value */
    for (i=0; i< DRV_SPI_DMA_DUMMY_BUFFER_SIZE; i++)
    {
        sDrvSpiTxDummy[i]= pDrvInstance->dummyByteValue;
    }
#endif

    return (SYS_MODULE_OBJ)pDrvInstance;

}

void DRV_SPI_Deinitialize ( SYS_MODULE_OBJ object )
{
    struct DRV_SPI_DRIVER_OBJECT * pDrvInstance = (struct DRV_SPI_DRIVER_OBJECT *)object;

    if (sNumDriverInit == 0)
    {
        SYS_ASSERT(false, "\r\nSPI Driver: Trying to deinitialize a handle when there are no handles initialized.")
        return;

    }

    if (pDrvInstance->spiId == 0xff)
    {
        SYS_ASSERT(false, "\r\nSPI Driver: Trying to deinitialize a handle that is already deinitialized.")
        return;
    }

    if ((pDrvInstance > &sSPIDriverInstances[DRV_SPI_INSTANCES_NUMBER-1]) || (pDrvInstance < &sSPIDriverInstances[0]))
    {
        SYS_ASSERT(false, "\r\nSPI Driver: Trying to deinitialize a handle that is not inside the bounds of the driver handles.")
        return;
    }

    PLIB_SPI_Disable(pDrvInstance->spiId);

    DRV_SPI_SYS_QUEUE_DestroyQueue(pDrvInstance->queue);
    memset(pDrvInstance, 0, sizeof(struct DRV_SPI_DRIVER_OBJECT));
    pDrvInstance->spiId = 0xff;

    sNumDriverInit--;

    if (sNumDriverInit == 0)
    {
        _DRV_SPI_RootDriverDeinitialization();
    }
}

SYS_STATUS DRV_SPI_Status ( SYS_MODULE_OBJ object )
{
    struct DRV_SPI_DRIVER_OBJECT * pDrvInstance = ( struct DRV_SPI_DRIVER_OBJECT *)object;
    return pDrvInstance->status;
}

DRV_HANDLE DRV_SPI_Open ( const SYS_MODULE_INDEX index, const DRV_IO_INTENT intent )
{
    if (index >= DRV_SPI_INSTANCES_NUMBER)
    {
        SYS_ASSERT(false, "\r\nSPI Driver: Attempting to open an instance number greater than the max.");
        return DRV_HANDLE_INVALID;
    }
    struct DRV_SPI_DRIVER_OBJECT * pDrvInstance = ( struct DRV_SPI_DRIVER_OBJECT *)&sSPIDriverInstances[index];

    if (sNumDriverInit == 0)
    {
        SYS_ASSERT(false, "\r\nSPI Driver: Trying to open a client with an invalid driver handle.")
        return DRV_HANDLE_INVALID;

    }

    if (pDrvInstance->spiId == 0xff)
    {
        SYS_ASSERT(false, "\r\nSPI Driver: Trying to open a client to an unused driver handle.")
        return DRV_HANDLE_INVALID;
    }

    if ((pDrvInstance > &sSPIDriverInstances[DRV_SPI_INSTANCES_NUMBER-1]) || (pDrvInstance < &sSPIDriverInstances[0]))
    {
        SYS_ASSERT(false, "\r\nSPI Driver: Trying to open a client to a handle outside the driver handle space.")
        return DRV_HANDLE_INVALID;
    }

    bool setupHardware = false;

    if (OSAL_SEM_Pend(&sSPIManager.managerSemaphore, OSAL_WAIT_FOREVER) != OSAL_RESULT_TRUE)
    {
        /* Report Error*/
    }

    if (pDrvInstance->numClients != 0)
    {
        if ((intent & DRV_IO_INTENT_EXCLUSIVE) == DRV_IO_INTENT_EXCLUSIVE)
        {
            SYS_ASSERT(false, "\r\nSPI Driver: Trying to open an exclusive access client to a driver that already has a client.");
            if (OSAL_SEM_Post(&sSPIManager.managerSemaphore) != OSAL_RESULT_TRUE)
            {
                /* Report Error*/
            }
            return DRV_HANDLE_INVALID;
        }
        if (pDrvInstance->isExclusive)
        {
            SYS_ASSERT(false, "\r\nSPI Driver: Trying to open a second client to a driver that is in exclusive mode.");
            if (OSAL_SEM_Post(&sSPIManager.managerSemaphore) != OSAL_RESULT_TRUE)
            {
                /* Report Error*/
            }
            return DRV_HANDLE_INVALID;
        }
    }
    else
    {
        setupHardware = true;
    }


    DRV_SPI_CLIENT_OBJECT * pClient = sSPIManager.pFreeClientHead;
    if (pClient == NULL)
    {
        SYS_ASSERT(false, "\r\nSPI Driver: Couldn't find a free client to open.");
        if (OSAL_SEM_Post(&sSPIManager.managerSemaphore) != OSAL_RESULT_TRUE)
        {
            /* Report Error*/
        }
        return DRV_HANDLE_INVALID;
    }

    pClient->driverObject = pDrvInstance;
    pClient->intent = intent;
    if ((intent & DRV_IO_INTENT_EXCLUSIVE) == DRV_IO_INTENT_EXCLUSIVE)
    {
        pDrvInstance->isExclusive = true;
    }
    pDrvInstance->numClients++;
    sSPIManager.pFreeClientHead = pClient->pNext;
    pClient->pNext = NULL;
    pClient->operationStarting = pDrvInstance->operationStarting;
    pClient->operationEnded = pDrvInstance->operationEnded;
    pClient->baudRate = pDrvInstance->baudRate;

    if (setupHardware)
    {
        if (pDrvInstance->taskMode == DRV_SPI_TASK_MODE_ISR)
        {
            SYS_INT_SourceDisable(pDrvInstance->txInterruptSource);
            SYS_INT_SourceDisable(pDrvInstance->rxInterruptSource);
            SYS_INT_SourceEnable(pDrvInstance->errInterruptSource);
            SYS_INT_SourceStatusClear(pDrvInstance->txInterruptSource);
            SYS_INT_SourceStatusClear(pDrvInstance->rxInterruptSource);
            SYS_INT_SourceStatusClear(pDrvInstance->errInterruptSource);
        }
        PLIB_SPI_Enable(pDrvInstance->spiId);
#if DRV_SPI_DMA
        DRV_SPI_SetupDMA(pDrvInstance);
#endif

    }

    if (OSAL_SEM_Post(&sSPIManager.managerSemaphore) != OSAL_RESULT_TRUE)
    {
        /* Report Error*/
    }
    return (DRV_HANDLE)pClient;
}

void DRV_SPI_Close ( DRV_HANDLE handle )
{
    DRV_SPI_CLIENT_OBJECT * pClient = (DRV_SPI_CLIENT_OBJECT *)handle;
    struct DRV_SPI_DRIVER_OBJECT * pDrvObject = pClient->driverObject;

    if ((pClient > &sSPIClientInstances[DRV_SPI_CLIENTS_NUMBER-1]) || (pClient < &sSPIClientInstances[0]))
    {
         SYS_ASSERT(false, "\r\nSPI Driver: Trying to close a client to a driver that is outside the range of client handles.")
         return;
    }

    if (pClient->pNext != NULL)
    {
        SYS_ASSERT(false, "\r\nSPI Driver: Trying to close a client to a driver that isn't being used.");
        return;
    }

    if (OSAL_SEM_Pend(&sSPIManager.managerSemaphore, OSAL_WAIT_FOREVER) != OSAL_RESULT_TRUE)
    {
        /* Report Error*/
    }

    pDrvObject->numClients--;
    if (pDrvObject->numClients == 0)
    {
        PLIB_SPI_Disable(pDrvObject->spiId);

        pDrvObject->isExclusive = 0;
        while (!DRV_SPI_SYS_QUEUE_IsEmpty(pDrvObject->queue))
        {
            void * queueEntry = NULL;
            if (DRV_SPI_SYS_QUEUE_Dequeue(pDrvObject->queue, &queueEntry) != DRV_SPI_SYS_QUEUE_SUCCESS)
            {
                SYS_ASSERT(false, "\r\nSPI Driver: Dequeue error.");
                if (OSAL_SEM_Post(&sSPIManager.managerSemaphore) != OSAL_RESULT_TRUE)
                {
                    /* Report Error*/
                }
                return;
            }
            if (DRV_SPI_SYS_QUEUE_FreeElement(pDrvObject->queue, queueEntry) != DRV_SPI_SYS_QUEUE_SUCCESS)
            {
                SYS_ASSERT(false, "\r\nSPI Driver: Queue free element error.");
                if (OSAL_SEM_Post(&sSPIManager.managerSemaphore) != OSAL_RESULT_TRUE)
                {
                    /* Report Error*/
                }
                return;
            }
        }
        if (pDrvObject->taskMode == DRV_SPI_TASK_MODE_ISR)
        {
            SYS_INT_SourceDisable(pDrvObject->txInterruptSource);
            SYS_INT_SourceDisable(pDrvObject->rxInterruptSource);
            SYS_INT_SourceDisable(pDrvObject->errInterruptSource);
            SYS_INT_SourceStatusClear(pDrvObject->txInterruptSource);
            SYS_INT_SourceStatusClear(pDrvObject->rxInterruptSource);
            SYS_INT_SourceStatusClear(pDrvObject->errInterruptSource);
        }
    }
    memset(pClient, 0, sizeof(DRV_SPI_CLIENT_OBJECT));
    pClient->pNext = sSPIManager.pFreeClientHead;
    sSPIManager.pFreeClientHead = pClient;
    if (OSAL_SEM_Post(&sSPIManager.managerSemaphore) != OSAL_RESULT_TRUE)
    {
        /* Report Error*/
    }
    return;
}

void DRV_SPI_Tasks ( SYS_MODULE_OBJ object )
{
    struct DRV_SPI_DRIVER_OBJECT * pDrvObject = (struct DRV_SPI_DRIVER_OBJECT *)object;
    (*pDrvObject->vfMainTask)(pDrvObject);

}

DRV_SPI_BUFFER_HANDLE DRV_SPI_BufferAddRead2 ( DRV_HANDLE handle,
                                              void *rxBuffer,
                                              size_t size,
                                              DRV_SPI_BUFFER_EVENT_HANDLER completeCB,
                                              void * context, DRV_SPI_BUFFER_HANDLE * jobHandle)
{
    DRV_SPI_CLIENT_OBJECT * pClient = (DRV_SPI_CLIENT_OBJECT *)handle;
    struct DRV_SPI_DRIVER_OBJECT * pDrvObject = pClient->driverObject;

     /* Check if Driver was open for READ or not */
    if(!(pClient->intent & DRV_IO_INTENT_READ))
    {
        /* Driver is not open in READ mode */
        SYS_ASSERT(false, "\r\nSPI Driver: Driver is not open in READ mode.");
        return (DRV_SPI_BUFFER_HANDLE)DRV_SPI_BUFFER_HANDLE_INVALID;
    }

    if ((pClient > &sSPIClientInstances[DRV_SPI_CLIENTS_NUMBER-1]) || (pClient < &sSPIClientInstances[0]))
    {
         SYS_ASSERT(false, "\r\nSPI Driver: Trying to access a client to a driver that is outside the range of client handles.")
         return (DRV_SPI_BUFFER_HANDLE)DRV_SPI_BUFFER_HANDLE_INVALID;
    }

    if (pClient->pNext != NULL)
    {
        SYS_ASSERT(false, "\r\nSPI Driver: Trying to access a client to a driver that isn't being used.");
        return (DRV_SPI_BUFFER_HANDLE)DRV_SPI_BUFFER_HANDLE_INVALID;
    }


    switch (pDrvObject->commWidth)
    {
        case SPI_COMMUNICATION_WIDTH_8BITS:
            break;
        case SPI_COMMUNICATION_WIDTH_16BITS:
            if ((size & 0x1) != 0)
            {
                SYS_ASSERT(false, "\r\nSPI Driver: invalid input size, must be a multiple of 16 bits.");
                return (DRV_SPI_BUFFER_HANDLE)DRV_SPI_BUFFER_HANDLE_INVALID;

            }
            break;
        case SPI_COMMUNICATION_WIDTH_32BITS:
            if ((size & 0x3) != 0)
            {
                SYS_ASSERT(false, "\r\nSPI Driver: invalid input size, must be a multiple of 32 bits.");
                return (DRV_SPI_BUFFER_HANDLE)DRV_SPI_BUFFER_HANDLE_INVALID;
            }
            break;
    }

    DRV_SPI_JOB_OBJECT * pJob = NULL;
    if (DRV_SPI_SYS_QUEUE_AllocElementLock(pDrvObject->queue, (void **)&pJob) != DRV_SPI_SYS_QUEUE_SUCCESS)
    {
        SYS_ASSERT(false, "\r\nSPI Driver: Error trying to get a free entry.");
        return (DRV_SPI_BUFFER_HANDLE)DRV_SPI_BUFFER_HANDLE_INVALID;
    }

    memset(pJob, 0, sizeof(DRV_SPI_JOB_OBJECT));
    pJob->rxBuffer = rxBuffer;
    pJob->dataLeftToRx = size;
    if (pDrvObject->spiMode == DRV_SPI_MODE_MASTER)
    {
        pJob->dummyLeftToTx = size;
    }
    pJob->completeCB = completeCB;
    pJob->context = context;
    pJob->status = DRV_SPI_BUFFER_EVENT_PENDING;
    pJob->pClient = pClient;
    if (jobHandle != NULL )
    {
        *jobHandle = (DRV_SPI_BUFFER_HANDLE)pJob;
    }

	/* Set a flag if it is first task in the queue */
    _DRV_SPI_QUEUE_STATUS_CHECK(pDrvObject->queue);

    if (DRV_SPI_SYS_QUEUE_EnqueueLock(pDrvObject->queue, (void*)pJob) != DRV_SPI_SYS_QUEUE_SUCCESS)
    {
        SYS_ASSERT(false, "\r\nSPI Driver: Error enqueing new job.");
        return (DRV_SPI_BUFFER_HANDLE)DRV_SPI_BUFFER_HANDLE_INVALID;
    }

    if (pDrvObject->taskMode == DRV_SPI_TASK_MODE_ISR)
    {
        pDrvObject->rxEnabled = true;
		SYS_INT_SourceEnable(pDrvObject->rxInterruptSource);
        SYS_INT_SourceEnable(pDrvObject->txInterruptSource);
        SYS_INT_SourceEnable(pDrvObject->errInterruptSource);

        /* Trigger SPI interrupt for the first time for the devices which don't have persistent interrupt */
		_DRV_SPI_INTERRUPT_TRIGGER(pDrvObject->rxInterruptSource);
    }

    return (DRV_SPI_BUFFER_HANDLE)pJob;
}

DRV_SPI_BUFFER_HANDLE DRV_SPI_BufferAddWrite2 ( DRV_HANDLE handle, void *txBuffer,
                                    size_t size,
                                              DRV_SPI_BUFFER_EVENT_HANDLER completeCB,
                                              void * context, DRV_SPI_BUFFER_HANDLE * jobHandle )
{
    DRV_SPI_CLIENT_OBJECT * pClient = (DRV_SPI_CLIENT_OBJECT *)handle;
    struct DRV_SPI_DRIVER_OBJECT * pDrvObject = pClient->driverObject;

    if(!(pClient->intent & DRV_IO_INTENT_WRITE))
    {
        /* Driver is not open in Write mode */
        SYS_ASSERT(false, "\r\nSPI Driver: Driver is not open in WRITE mode.");
        return (DRV_SPI_BUFFER_HANDLE)DRV_SPI_BUFFER_HANDLE_INVALID;
    }

    if ((pClient > &sSPIClientInstances[DRV_SPI_CLIENTS_NUMBER-1]) || (pClient < &sSPIClientInstances[0]))
    {
         SYS_ASSERT(false, "\r\nSPI Driver: Trying to access a client to a driver that is outside the range of client handles.")
         return (DRV_SPI_BUFFER_HANDLE)DRV_SPI_BUFFER_HANDLE_INVALID;
    }

    if (pClient->pNext != NULL)
    {
        SYS_ASSERT(false, "\r\nSPI Driver: Trying to access a client to a driver that isn't being used.");
        return (DRV_SPI_BUFFER_HANDLE)DRV_SPI_BUFFER_HANDLE_INVALID;
    }

    switch (pDrvObject->commWidth)
    {
        case SPI_COMMUNICATION_WIDTH_8BITS:
            break;
        case SPI_COMMUNICATION_WIDTH_16BITS:
            if ((size & 0x1) != 0)
            {
                SYS_ASSERT(false, "\r\nSPI Driver: invalid input size, must be a multiple of 16 bits.");
                return (DRV_SPI_BUFFER_HANDLE)DRV_SPI_BUFFER_HANDLE_INVALID;

            }
            break;
        case SPI_COMMUNICATION_WIDTH_32BITS:
            if ((size & 0x3) != 0)
            {
                SYS_ASSERT(false, "\r\nSPI Driver: invalid input size, must be a multiple of 32 bits.");
                return (DRV_SPI_BUFFER_HANDLE)DRV_SPI_BUFFER_HANDLE_INVALID;

            }
            break;
    }

    DRV_SPI_JOB_OBJECT * pJob = NULL;
    if (DRV_SPI_SYS_QUEUE_AllocElementLock(pDrvObject->queue, (void **)&pJob) != DRV_SPI_SYS_QUEUE_SUCCESS)
    {
        SYS_ASSERT(false, "\r\nSPI Driver: Error trying to get a free entry.");
        return (DRV_SPI_BUFFER_HANDLE)DRV_SPI_BUFFER_HANDLE_INVALID;
    }

    memset(pJob, 0, sizeof(DRV_SPI_JOB_OBJECT));
    pJob->txBuffer = txBuffer;
    pJob->dataLeftToTx = size;
    pJob->dummyLeftToRx = size;
    pJob->completeCB = completeCB;
    pJob->context = context;
    pJob->status = DRV_SPI_BUFFER_EVENT_PENDING;
    pJob->pClient = pClient;
    if (jobHandle != NULL )
    {
        *jobHandle = (DRV_SPI_BUFFER_HANDLE)pJob;
    }

	/* Set a flag if it is first task in the queue */
    _DRV_SPI_QUEUE_STATUS_CHECK(pDrvObject->queue);

    if (DRV_SPI_SYS_QUEUE_EnqueueLock(pDrvObject->queue, (void*)pJob) != DRV_SPI_SYS_QUEUE_SUCCESS)
    {
        SYS_ASSERT(false, "\r\nSPI Driver: Error enqueing new job.");
        return (DRV_SPI_BUFFER_HANDLE)DRV_SPI_BUFFER_HANDLE_INVALID;
    }

    if (pDrvObject->taskMode == DRV_SPI_TASK_MODE_ISR)
    {
        pDrvObject->txEnabled = true;
		pDrvObject->rxEnabled = true;
        SYS_INT_SourceEnable(pDrvObject->txInterruptSource);
        SYS_INT_SourceEnable(pDrvObject->rxInterruptSource);
        SYS_INT_SourceEnable(pDrvObject->errInterruptSource);


		/* Trigger SPI interrupt for the first time for the devices which don't have persistent interrupt */
		_DRV_SPI_INTERRUPT_TRIGGER(pDrvObject->txInterruptSource);
    }

    return (DRV_SPI_BUFFER_HANDLE)pJob;
}

DRV_SPI_BUFFER_HANDLE DRV_SPI_BufferAddWriteRead2 ( DRV_HANDLE handle, void *txBuffer, size_t txSize,
                                                   void *rxBuffer, size_t rxSize,
                                                   DRV_SPI_BUFFER_EVENT_HANDLER completeCB,
                                                   void * context, DRV_SPI_BUFFER_HANDLE * jobHandle )
{
    DRV_SPI_CLIENT_OBJECT * pClient = (DRV_SPI_CLIENT_OBJECT *)handle;

    struct DRV_SPI_DRIVER_OBJECT * pDrvObject = pClient->driverObject;

    if ((pClient > &sSPIClientInstances[DRV_SPI_CLIENTS_NUMBER-1]) || (pClient < &sSPIClientInstances[0]))
    {
        SYS_ASSERT(false, "\r\nSPI Driver: Trying to access a client to a driver that is outside the range of client handles.")
        return (DRV_SPI_BUFFER_HANDLE)DRV_SPI_BUFFER_HANDLE_INVALID;
    }

    if (pClient->pNext != NULL)
    {
        SYS_ASSERT(false, "\r\nSPI Driver: Trying to access a client to a driver that isn't being used.");
        return (DRV_SPI_BUFFER_HANDLE)DRV_SPI_BUFFER_HANDLE_INVALID;
    }

    switch (pDrvObject->commWidth)
    {
        case SPI_COMMUNICATION_WIDTH_8BITS:
            break;
        case SPI_COMMUNICATION_WIDTH_16BITS:
            if (((rxSize & 0x1) != 0) || ((txSize & 0x1) != 0))
            {
                SYS_ASSERT(false, "\r\nSPI Driver: invalid input size, must be a multiple of 16 bits.");
                return (DRV_SPI_BUFFER_HANDLE)DRV_SPI_BUFFER_HANDLE_INVALID;

            }
            break;
        case SPI_COMMUNICATION_WIDTH_32BITS:
            if (((rxSize & 0x3) != 0) || ((txSize & 0x3) != 0))
            {
                SYS_ASSERT(false, "\r\nSPI Driver: invalid input size, must be a multiple of 32 bits.");
                return (DRV_SPI_BUFFER_HANDLE)DRV_SPI_BUFFER_HANDLE_INVALID;

            }
            break;
    }

    DRV_SPI_JOB_OBJECT * pJob = NULL;
    if (DRV_SPI_SYS_QUEUE_AllocElementLock(pDrvObject->queue, (void **)&pJob) != DRV_SPI_SYS_QUEUE_SUCCESS)
    {
        SYS_ASSERT(false, "\r\nSPI Driver: Error trying to get a free entry.");
        return (DRV_SPI_BUFFER_HANDLE)DRV_SPI_BUFFER_HANDLE_INVALID;
    }

    memset(pJob, 0, sizeof(DRV_SPI_JOB_OBJECT));
    pJob->txBuffer = txBuffer;
    pJob->dataLeftToTx = txSize;
    pJob->rxBuffer = rxBuffer;
    pJob->dataLeftToRx = rxSize;
    pJob->pClient = pClient;
    if (jobHandle != NULL )
    {
        *jobHandle = (DRV_SPI_BUFFER_HANDLE)pJob;
    }
    if (pDrvObject->spiMode == DRV_SPI_MODE_MASTER)
    {
        if (rxSize > txSize)
        {
            pJob->dummyLeftToTx = rxSize - txSize;
        }
    }
    if (txSize > rxSize)
    {
        pJob->dummyLeftToRx = txSize - rxSize;
    }
    pJob->completeCB = completeCB;
    pJob->context = context;
    pJob->status = DRV_SPI_BUFFER_EVENT_PENDING;

    if (DRV_SPI_SYS_QUEUE_EnqueueLock(pDrvObject->queue, (void*)pJob) != DRV_SPI_SYS_QUEUE_SUCCESS)
    {
        SYS_ASSERT(false, "\r\nSPI Driver: Error enqueing new job.");
        return (DRV_SPI_BUFFER_HANDLE)DRV_SPI_BUFFER_HANDLE_INVALID;
    }

    if (pDrvObject->taskMode == DRV_SPI_TASK_MODE_ISR)
    {
        pDrvObject->txEnabled = true;
        SYS_INT_SourceEnable(pDrvObject->txInterruptSource);
        SYS_INT_SourceEnable(pDrvObject->rxInterruptSource);
        SYS_INT_SourceEnable(pDrvObject->errInterruptSource);
    }

    return (DRV_SPI_BUFFER_HANDLE)pJob;
}

DRV_SPI_BUFFER_HANDLE DRV_SPI_BufferAddRead ( DRV_HANDLE handle,
                                              void *rxBuffer,
                                              size_t size,
                                              DRV_SPI_BUFFER_EVENT_HANDLER completeCB,
                                              void * context)
{
    return DRV_SPI_BufferAddRead2(handle, rxBuffer, size, completeCB, context, NULL);
}

DRV_SPI_BUFFER_HANDLE DRV_SPI_BufferAddWrite ( DRV_HANDLE handle, void *txBuffer,
                                    size_t size,
                                              DRV_SPI_BUFFER_EVENT_HANDLER completeCB,
                                              void * context )
{
    return DRV_SPI_BufferAddWrite2(handle, txBuffer, size, completeCB, context, NULL);
}

DRV_SPI_BUFFER_HANDLE DRV_SPI_BufferAddWriteRead ( DRV_HANDLE handle, void *txBuffer, size_t txSize,
                                                   void *rxBuffer, size_t rxSize,
                                                   DRV_SPI_BUFFER_EVENT_HANDLER completeCB,
                                                   void * context )
{
    return DRV_SPI_BufferAddWriteRead2(handle, txBuffer, txSize, rxBuffer, rxSize, completeCB, context, NULL);
}


DRV_SPI_BUFFER_EVENT DRV_SPI_BufferStatus ( DRV_SPI_BUFFER_HANDLE bufferHandle )
{
    DRV_SPI_JOB_OBJECT * pJob = (DRV_SPI_JOB_OBJECT *)bufferHandle;

    return pJob->status;
}


#if DRV_SPI_POLLED
int32_t DRV_SPI_PolledErrorTasks(struct DRV_SPI_DRIVER_OBJECT * pDrvInstance)
{

    if (pDrvInstance->currentJob == NULL)
    {
        return 0;
    }
    register SPI_MODULE_ID spiId = pDrvInstance->spiId;
    register DRV_SPI_JOB_OBJECT * currentJob = pDrvInstance->currentJob;

    if (PLIB_SPI_ReceiverHasOverflowed(spiId))
    {
        currentJob->status = DRV_SPI_BUFFER_EVENT_ERROR;
        if (currentJob->completeCB != NULL)
        {
            (*currentJob->completeCB)(DRV_SPI_BUFFER_EVENT_ERROR, (DRV_SPI_BUFFER_HANDLE)currentJob, currentJob->context);
        }
        DRV_SPI_CLIENT_OBJECT * pClient = (DRV_SPI_CLIENT_OBJECT*)currentJob->pClient;
        if (pClient->operationEnded != NULL)
        {
            (*pClient->operationEnded)(DRV_SPI_BUFFER_EVENT_ERROR, (DRV_SPI_BUFFER_HANDLE)currentJob, currentJob->context);
        }
        if (DRV_SPI_SYS_QUEUE_FreeElementLock(pDrvInstance->queue, currentJob) != DRV_SPI_SYS_QUEUE_SUCCESS)
        {
            SYS_ASSERT(false, "\r\nSPI Driver: Queue free element error.");
            return 0;
        }
        pDrvInstance->currentJob = NULL;
        PLIB_SPI_BufferClear(spiId );
        PLIB_SPI_ReceiverOverflowClear (spiId );
    }
    return 0;
}
#endif

#if DRV_SPI_ISR
int32_t DRV_SPI_ISRErrorTasks(struct DRV_SPI_DRIVER_OBJECT * pDrvInstance)
{

    if (pDrvInstance->currentJob == NULL)
    {
        return 0;
    }
    register SPI_MODULE_ID spiId = pDrvInstance->spiId;
    register DRV_SPI_JOB_OBJECT * currentJob = pDrvInstance->currentJob;

    if (PLIB_SPI_ReceiverHasOverflowed(spiId))
    {
        if (currentJob->completeCB != NULL)
        {
            (*currentJob->completeCB)(DRV_SPI_BUFFER_EVENT_ERROR, (DRV_SPI_BUFFER_HANDLE)currentJob, currentJob->context);
        }
        currentJob->status = DRV_SPI_BUFFER_EVENT_ERROR;
        DRV_SPI_CLIENT_OBJECT * pClient = (DRV_SPI_CLIENT_OBJECT*)currentJob->pClient;
        if (pClient->operationEnded != NULL)
        {
            (*pClient->operationEnded)(DRV_SPI_BUFFER_EVENT_ERROR, (DRV_SPI_BUFFER_HANDLE)currentJob, currentJob->context);
        }
        if (DRV_SPI_SYS_QUEUE_FreeElement(pDrvInstance->queue, currentJob) != DRV_SPI_SYS_QUEUE_SUCCESS)
        {
            SYS_ASSERT(false, "\r\nSPI Driver: Queue free element error.");
            return 0;
        }
        pDrvInstance->currentJob = NULL;
        PLIB_SPI_BufferClear(spiId);
        PLIB_SPI_ReceiverOverflowClear (spiId );
        SYS_INT_SourceStatusClear(pDrvInstance->errInterruptSource);

    }
    return 0;
}
#endif

int32_t DRV_SPI_ClientConfigure ( DRV_HANDLE handle,
                                 const DRV_SPI_CLIENT_DATA * cfgData  )
{
    DRV_SPI_CLIENT_OBJECT * pClient = (DRV_SPI_CLIENT_OBJECT *)handle;
    struct DRV_SPI_DRIVER_OBJECT * pDrvObject = pClient->driverObject;
    if (cfgData == NULL)
    {
        pClient->operationStarting = pDrvObject->operationStarting;
        pClient->operationEnded = pDrvObject->operationEnded;
        pClient->baudRate = pDrvObject->baudRate;
        return 0;
    }
    if (cfgData->operationStarting != NULL)
    {
        pClient->operationStarting = cfgData->operationStarting;
    }
    else
    {
        pClient->operationStarting = pDrvObject->operationStarting;
    }
    if (cfgData->operationEnded != NULL)
    {
        pClient->operationEnded = cfgData->operationEnded;
    }
    else
    {
        pClient->operationEnded = pDrvObject->operationEnded;
    }
    if (cfgData->baudRate != 0)
    {
        /* just save the new baud rate value for this client, don't change it here.
           Actual change will happen in the task routine when queue request is serviced */
        pClient->baudRate = cfgData->baudRate;
    }
    else
    {
        pClient->baudRate = pDrvObject->baudRate;
    }
    return 0;
}

