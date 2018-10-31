/*******************************************************************************
  SPI Driver Interface Implementation

  Company:
    Microchip Technology Inc.

  File Name:
   drv_spi_dma_pic32c.c

  Summary:
   SPI Driver DMA implementation

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

#include "driver/spi/src/drv_spi_local_dma_pic32c.h"

/* Statically Allocated stuff*/

uint8_t drvSpiRxDummy[DRV_SPI_DMA_DUMMY_BUFFER_SIZE];
uint8_t drvSpiTxDummy[DRV_SPI_DMA_DUMMY_BUFFER_SIZE];
const size_t spiSize = 1;

static DRV_SPI_MANAGER_OBJECT sSPIManager;
static struct DRV_SPI_DRIVER_OBJECT sSPIDriverInstances[DRV_SPI_INSTANCES_NUMBER];
static DRV_SPI_CLIENT_OBJECT sSPIClientInstances[DRV_SPI_CLIENTS_NUMBER];
static uint8_t sDrvSPIQueueArea[DRV_SPI_SYS_QUEUE_BUFFER_SIZE(DRV_SPI_INSTANCES_NUMBER, sizeof(DRV_SPI_JOB_OBJECT), DRV_SPI_QUEUE_DEPTH_COMBINED)];
static uint8_t sNumDriverInit = 0;

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
    int8_t counter;
    
    memset(&sSPIManager, 0, sizeof(sSPIManager));
    memset(&sSPIDriverInstances, 0, sizeof(sSPIDriverInstances));
    memset(&sSPIClientInstances, 0, sizeof(sSPIClientInstances));
    
    if (OSAL_SEM_Create(&sSPIManager.managerSemaphore, OSAL_SEM_TYPE_BINARY, 1, 1) != OSAL_RESULT_TRUE) 
    {
        return -1;
    }
    
    qmInitData.semaphoreToUse = sSPIManager.managerSemaphore;
    
    if (DRV_SPI_SYS_QUEUE_Initialize(&qmInitData, &sSPIManager.hQueueManager) != DRV_SPI_SYS_QUEUE_SUCCESS)
    {
        SYS_ASSERT(false, "\r\nSPI Driver: Could not create queuing system.");
        return -1;
    }

    for (counter = DRV_SPI_INSTANCES_NUMBER - 1; counter >= 0; counter--)
    {
        sSPIDriverInstances[counter].spiId = NULL;
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
    
    if (OSAL_SEM_Delete(&(sSPIManager.managerSemaphore)) != OSAL_RESULT_TRUE) 
    {
        return -1;
    }
    
    return 0;
}


// *****************************************************************************
// *****************************************************************************
// Section: System functions
// *****************************************************************************
// *****************************************************************************

SYS_MODULE_OBJ DRV_SPI_Initialize( const SYS_MODULE_INDEX index,
                                           const SYS_MODULE_INIT * const init )
{  
    struct DRV_SPI_DRIVER_OBJECT * pDrvInstance = NULL;
    const DRV_SPI_INIT * pInit = NULL;
    uint8_t counter;
    
    /* Root driver initialization if the driver is first instance */
    if (sNumDriverInit == 0)
    {
        if (_DRV_SPI_RootDriverInitialization() != 0)
        {
            return  SYS_MODULE_OBJ_INVALID;
        }
    }

    /* Validate the index parameter */
    if (index >= DRV_SPI_INSTANCES_NUMBER)
    {
        SYS_ASSERT(false, "\r\nSPI Driver: Attempting to initialize an instance number greater than the max.");
        
        if (sNumDriverInit == 0) 
        {
            _DRV_SPI_RootDriverDeinitialization();
        }
        return SYS_MODULE_OBJ_INVALID;
    }

    /* Get a local pointer to driver instance object */ 
    pDrvInstance = (struct DRV_SPI_DRIVER_OBJECT *)&sSPIDriverInstances[index];

    /* Check the driver instance status by checking SPI module ID */
    if (pDrvInstance->spiId != NULL)
    {
        SYS_ASSERT(false, "\r\nSPI Driver: Attempting to reinitialize a driver instance that is already in use.");
        return SYS_MODULE_OBJ_INVALID;
    }

    /* Get a local pointer to instance init data */
    pInit = (const DRV_SPI_INIT *)init;

    /* Check if the SPI ID is already used by some other driver instance*/
    for (counter = 0; counter < DRV_SPI_INSTANCES_NUMBER; counter ++)
    {
        if ((sSPIDriverInstances[counter].spiId == (spi_registers_t *)pInit->spiId))
        {
            SYS_ASSERT(false, "\r\nSPI Driver: trying to initialize two drivers with the same spiID.");
            if (sNumDriverInit == 0) _DRV_SPI_RootDriverDeinitialization();
            return SYS_MODULE_OBJ_INVALID;
        }
    }

    /* Initialize SPI hardware */
    if (_DRV_SPI_SetupHardware(pDrvInstance, pInit) != 0)
    {
        /* Assert should have already happened, don't need a second.*/
        if (sNumDriverInit == 0) 
        {
            _DRV_SPI_RootDriverDeinitialization();
        }
        return SYS_MODULE_OBJ_INVALID;
    }

    /* Initialize queue data */
    qInitData.maxElements = pInit->queueSize;
    qInitData.reserveElements = pInit->queueSize;
    qInitData.semaphoreToUse = sSPIManager.managerSemaphore;

    /* Setup driver instance queue */
    if (DRV_SPI_SYS_QUEUE_CreateQueue(sSPIManager.hQueueManager, &qInitData, &pDrvInstance->queue) != DRV_SPI_SYS_QUEUE_SUCCESS)
    {
        SYS_ASSERT(false, "\r\nSPI Driver: Could not set up driver instance queue.");
        if (sNumDriverInit == 0) _DRV_SPI_RootDriverDeinitialization();
        return SYS_MODULE_OBJ_INVALID;

    }

    /* By this point all the tests should have passed. */
    
    /* Increment the count of driver instances active */
    sNumDriverInit++;
    
    /* Update driver object members */
    pDrvInstance->spiId = (spi_registers_t *)pInit->spiId;
    pDrvInstance->status = SYS_STATUS_READY;
    pDrvInstance->interruptSPI = pInit->interruptSPI;
    pDrvInstance->spiMode = pInit->spiMode;
    pDrvInstance->taskMode = pInit->taskMode;
    pDrvInstance->operationStarting = pInit->operationStarting;
    pDrvInstance->operationEnded = pInit->operationEnded;
    pDrvInstance->commWidth = pInit->commWidth;
    pDrvInstance->baudRate = pInit->baudRate;
    pDrvInstance->spiClkSrc = pInit->spiClkSrc;
    pDrvInstance->dummyByteValue = pInit->dummyByteValue;
    pDrvInstance->dmaChannelRead = pInit->rxDmaChannel;
    pDrvInstance->dmaChannelWrite = pInit->txDmaChannel;
    pDrvInstance->dmaChannelHandleWrite = SYS_DMA_CHANNEL_HANDLE_INVALID;
    pDrvInstance->dmaChannelHandleRead = SYS_DMA_CHANNEL_HANDLE_INVALID;
    pDrvInstance->interruptDMA = pInit->interruptDMA;

    /* DMA mode of operation. Allocate a handle for the specified channel.
     * Setup the channel for transfer */
    pDrvInstance->rxDMAState = DRV_SPI_DMA_NONE;
    pDrvInstance->txDMAState = DRV_SPI_DMA_NONE;
    memset(drvSpiTxDummy, pDrvInstance->dummyByteValue, sizeof(drvSpiTxDummy));
    
    /* TX DMA channel setup */
    if( DMA_CHANNEL_NONE != pDrvInstance->dmaChannelWrite )
    {
        pDrvInstance->dmaChannelHandleWrite = SYS_DMA_ChannelAllocate(pDrvInstance->dmaChannelWrite);
        
        if(SYS_DMA_CHANNEL_HANDLE_INVALID != pDrvInstance->dmaChannelHandleWrite)
        {
            /* Setting the channel priority same as the channel number */
            SYS_DMA_ChannelSetup(pDrvInstance->dmaChannelHandleWrite,
                SYS_DMA_CHANNEL_OP_MODE_BASIC, pInit->dmaChannelTransmitTrigger);
                
            /* Setup DMA job event handler */
            SYS_DMA_ChannelTransferEventHandlerSet(pDrvInstance->dmaChannelHandleWrite, _DRV_SPI_DMA_EventHandler, (uintptr_t)pDrvInstance);
        }
    }

    /* RX DMA channel setup */
    if( DMA_CHANNEL_NONE != pDrvInstance->dmaChannelRead )
    {
        pDrvInstance->dmaChannelHandleRead = SYS_DMA_ChannelAllocate(pDrvInstance->dmaChannelRead);
        if(SYS_DMA_CHANNEL_HANDLE_INVALID != pDrvInstance->dmaChannelHandleRead)
        {
            /* Setting the channel priority same as the channel number */
            SYS_DMA_ChannelSetup(pDrvInstance->dmaChannelHandleRead,
                SYS_DMA_CHANNEL_OP_MODE_BASIC, pInit->dmaChannelReceiveTrigger);
                
            /* Setup DMA job event handler */
            SYS_DMA_ChannelTransferEventHandlerSet(pDrvInstance->dmaChannelHandleRead, _DRV_SPI_DMA_EventHandler, (uintptr_t)pDrvInstance);
        }
    }


    
    /* Assign task function */
    if(DRV_SPI_MODE_MASTER == pDrvInstance->spiMode)
    {
        /* Master task */
        pDrvInstance->instanceTask = &_DRV_SPI_MasterTasks;
    }
    else
    {
        /* Slave task */
       pDrvInstance->instanceTask = &_DRV_SPI_SlaveTasks; 
    }

    return (SYS_MODULE_OBJ)pDrvInstance;

}

void DRV_SPI_Deinitialize ( SYS_MODULE_OBJ object )
{
    struct DRV_SPI_DRIVER_OBJECT * pDrvInstance = (struct DRV_SPI_DRIVER_OBJECT *)object;

    if (SYS_MODULE_OBJ_INVALID == object)
    {
        SYS_ASSERT(false, "\r\nSPI Driver: Invalid module object");
        return;
    }

    if (pDrvInstance->spiId == NULL)
    {
        SYS_ASSERT(false, "\r\nSPI Driver: Trying to deinitialize a handle that is already deinitialized.");
        return;
    }

    /* Disable the SPI module associated with this instance */
    pDrvInstance->spiId->SPI_CR.w = SPI_CR_SPIDIS_Msk;

    /* Deallocate the allocated channel handles  */
    if(SYS_DMA_CHANNEL_HANDLE_INVALID != pDrvInstance->dmaChannelHandleWrite)
    {
        SYS_DMA_ChannelRelease(pDrvInstance->dmaChannelHandleWrite);
    }
    if(SYS_DMA_CHANNEL_HANDLE_INVALID != pDrvInstance->dmaChannelHandleRead)
    {
        SYS_DMA_ChannelRelease(pDrvInstance->dmaChannelHandleRead);
    }

    
    /* Destroy instance queue */
    DRV_SPI_SYS_QUEUE_DestroyQueue(pDrvInstance->queue);
    
    /* Reset the instance object */
    memset(pDrvInstance, 0, sizeof(struct DRV_SPI_DRIVER_OBJECT));
    
    /* Mark SPI ID as invalid for the instance status */ 
    pDrvInstance->spiId = NULL;

    /* Decrement the count of driver instances active */
    sNumDriverInit--;

    /* If none of the instances are active then do root deinitialize */
    if (sNumDriverInit == 0)
    {
        _DRV_SPI_RootDriverDeinitialization();
    }
}

SYS_STATUS DRV_SPI_Status ( SYS_MODULE_OBJ object )
{
    struct DRV_SPI_DRIVER_OBJECT * pDrvInstance = (struct DRV_SPI_DRIVER_OBJECT *)object;
    return pDrvInstance->status;
}

// *****************************************************************************
// *****************************************************************************
// Section: Client core functions
// *****************************************************************************
// *****************************************************************************

DRV_HANDLE DRV_SPI_Open ( const SYS_MODULE_INDEX index, const DRV_IO_INTENT intent )
{
    struct DRV_SPI_DRIVER_OBJECT * pDrvInstance = (struct DRV_SPI_DRIVER_OBJECT *)&sSPIDriverInstances[index];
    DRV_SPI_CLIENT_OBJECT * pClient = sSPIManager.pFreeClientHead;
    
    if (index >= DRV_SPI_INSTANCES_NUMBER)
    {
        SYS_ASSERT(false, "\r\nSPI Driver: Attempting to open an instance number greater than the max.");
        return DRV_HANDLE_INVALID;
    }

    if (pDrvInstance->spiId == NULL)
    {
        SYS_ASSERT(false, "\r\nSPI Driver: Trying to open a client to a uninitialized driver instance.");
        return DRV_HANDLE_INVALID;
    }

    if ((pDrvInstance > &sSPIDriverInstances[DRV_SPI_INSTANCES_NUMBER-1]) || (pDrvInstance < &sSPIDriverInstances[0]))
    {
        SYS_ASSERT(false, "\r\nSPI Driver: Trying to open a client to a handle outside the driver handle space.");
        return DRV_HANDLE_INVALID;
    }

    if (OSAL_SEM_Pend(&sSPIManager.managerSemaphore, OSAL_WAIT_FOREVER) != OSAL_RESULT_TRUE)
    {
        SYS_ASSERT(false, "\r\nSPI Driver: Failed to get semaphore.");
        return DRV_HANDLE_INVALID;
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
    
    sSPIManager.pFreeClientHead = pClient->pNext;
    pClient->pNext = NULL;
    pClient->operationStarting = pDrvInstance->operationStarting;
    pClient->operationEnded = pDrvInstance->operationEnded;
    pClient->baudRate = pDrvInstance->baudRate;

    /* Setup hardware steps for the first client */
    if (pDrvInstance->numClients == 0)
    {
        if (pDrvInstance->taskMode == DRV_SPI_TASK_MODE_ISR)
        {
            /* Enable SPI module level interrupt sources.
             * Only error(Overrun and underrun) interrupt is enabled for now.
             * TX and RX will be enabled while adding buffer requests */
            pDrvInstance->spiId->SPI_IER.w = SPI_IER_OVRES_Msk;
            pDrvInstance->spiId->SPI_IER.w = SPI_IER_UNDES_Msk; 
            
            /* Enable Interrupt at system level */
            SYS_INT_SourceEnable(pDrvInstance->interruptSPI);
        }
        
        /* Enable SPI */
        pDrvInstance->spiId->SPI_CR.w = SPI_CR_SPIEN_Msk;
    }
    
    /* Increment client count */
    pDrvInstance->numClients++;
    
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
         SYS_ASSERT(false, "\r\nSPI Driver: Trying to close a client to a driver that is outside the range of client handles.");
         return;
    }

    if (pClient->pNext != NULL)
    {
        SYS_ASSERT(false, "\r\nSPI Driver: Trying to close a client to a driver that isn't being used.");
        return;
    }

    if (OSAL_SEM_Pend(&sSPIManager.managerSemaphore, OSAL_WAIT_FOREVER) != OSAL_RESULT_TRUE)
    {
        SYS_ASSERT(false, "\r\nSPI Driver: Failed to get semaphore.");
        return;
    }

    /* Decrement client count */
    pDrvObject->numClients--;
    
    /* If last client then clear hardware */
    if (pDrvObject->numClients == 0)
    {
        /* Disable SPI */
        pDrvObject->spiId->SPI_CR.w = SPI_CR_SPIDIS_Msk;

        pDrvObject->isExclusive = 0;
        
        /* Reset instance queue */
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
        
        /* Disable interrupts */
        if (pDrvObject->taskMode == DRV_SPI_TASK_MODE_ISR)
        {
            /* Disable all module level sources */
            pDrvObject->spiId->SPI_IDR.w = SPI_IDR_Msk;
            
            /* Disable system level interrupt */
            SYS_INT_SourceDisable(pDrvObject->interruptSPI);
        }
    }
    
    /* Reset client object */
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
    (*pDrvObject->instanceTask)(pDrvObject);

}

// *****************************************************************************
// *****************************************************************************
// Section: Transfer functions
// *****************************************************************************
// *****************************************************************************

DRV_SPI_BUFFER_HANDLE DRV_SPI_BufferAddRead2 ( DRV_HANDLE handle,
                                              void *rxBuffer,
                                              size_t size,
                                              DRV_SPI_BUFFER_EVENT_HANDLER completeCB,
                                              void * context, DRV_SPI_BUFFER_HANDLE * jobHandle)
{
    DRV_SPI_CLIENT_OBJECT * pClient = (DRV_SPI_CLIENT_OBJECT *)handle;
    struct DRV_SPI_DRIVER_OBJECT * pDrvObject = pClient->driverObject;
    DRV_SPI_JOB_OBJECT * pJob = NULL;
    
     /* Check if Driver was open for READ or not */
    if(!(pClient->intent & DRV_IO_INTENT_READ))
    {
        /* Driver is not open in READ mode */
        SYS_ASSERT(false, "\r\nSPI Driver: Driver is not opened in READ mode.");
        return (DRV_SPI_BUFFER_HANDLE)DRV_SPI_BUFFER_HANDLE_INVALID;
    }

    if ((pClient > &sSPIClientInstances[DRV_SPI_CLIENTS_NUMBER-1]) || (pClient < &sSPIClientInstances[0]))
    {
         SYS_ASSERT(false, "\r\nSPI Driver: Trying to access a client to a driver that is outside the range of client handles.");
         return (DRV_SPI_BUFFER_HANDLE)DRV_SPI_BUFFER_HANDLE_INVALID;
    }

    if (pClient->pNext != NULL)
    {
        SYS_ASSERT(false, "\r\nSPI Driver: Trying to access a client to a driver that isn't being used.");
        return (DRV_SPI_BUFFER_HANDLE)DRV_SPI_BUFFER_HANDLE_INVALID;
    }
    
    /* If size is odd number then 16 bit transfer is not possible */
    if( (SPI_COMMUNICATION_WIDTH_16BITS == pDrvObject->commWidth) &&  ((size & 0x1) != 0) )
    {
        SYS_ASSERT(false, "\r\nSPI Driver: invalid input size, must be a multiple of 16 bits.");
        return (DRV_SPI_BUFFER_HANDLE)DRV_SPI_BUFFER_HANDLE_INVALID;  
    }
    
    /* Disable the interrupt, as we are updating the queue */ 
    if (pDrvObject->taskMode == DRV_SPI_TASK_MODE_ISR)
    {
        if(SYS_DMA_CHANNEL_HANDLE_INVALID != pDrvObject->dmaChannelHandleRead)
        {
            SYS_INT_SourceDisable(pDrvObject->interruptDMA);
        }
        SYS_INT_SourceDisable(pDrvObject->interruptSPI);
    }
    
    if (DRV_SPI_SYS_QUEUE_AllocElementLock(pDrvObject->queue, (void **)&pJob) != DRV_SPI_SYS_QUEUE_SUCCESS)
    {
        SYS_ASSERT(false, "\r\nSPI Driver: Error trying to get a free entry.");
        return (DRV_SPI_BUFFER_HANDLE)DRV_SPI_BUFFER_HANDLE_INVALID;
    }

    /* Update buffer job parameters */
    memset(pJob, 0, sizeof(DRV_SPI_JOB_OBJECT));
    pJob->completeCB = completeCB;
    pJob->context = context;
    pJob->status = DRV_SPI_BUFFER_EVENT_PENDING;
    pJob->pClient = pClient;
    if (jobHandle != NULL )
    {
        *jobHandle = (DRV_SPI_BUFFER_HANDLE)pJob;
    }
        
    if(SYS_DMA_CHANNEL_HANDLE_INVALID != pDrvObject->dmaChannelHandleWrite && SYS_DMA_CHANNEL_HANDLE_INVALID != pDrvObject->dmaChannelHandleRead && DRV_SPI_SYS_QUEUE_IsEmpty(pDrvObject->queue) && pDrvObject->rxDMAState == DRV_SPI_DMA_NONE)
    {
        /* Since this is the first buffer in the queue, add it immediately to
         * DMA for processing */
        
        /* Enable SPI error interrupts */
        SYS_INT_SourceEnable(pDrvObject->interruptSPI);
        
        /*Enable DMA system interrupt before adding buffer to 
         * transmit */
        SYS_INT_SourceEnable(pDrvObject->interruptDMA);
        
        /* Call the operation starting function pointer.  
         * This can be used to modify the slave select lines */
        DRV_SPI_CLIENT_OBJECT * pClient = (DRV_SPI_CLIENT_OBJECT*)pJob->pClient;
        if (pClient->operationStarting != NULL)
        {
            (*pClient->operationStarting)(DRV_SPI_BUFFER_EVENT_PROCESSING, (DRV_SPI_BUFFER_HANDLE)pJob, pJob->context);
        }
        /* Check the baud rate.  If its different set the new baud rate*/
        if (pClient->baudRate != pDrvObject->currentBaudRate)
        {
            pDrvObject->spiId->SPI_CSR[0].w = ( (pDrvObject->spiId->SPI_CSR[0].w & (~SPI_CSR_SCBR_Msk)) | (SPI_CSR_SCBR( pDrvObject->spiClkSrc/pClient->baudRate )) );
            pDrvObject->currentBaudRate = pClient->baudRate;
        }
            
        /* List the new job as processing*/
        pJob->status = DRV_SPI_BUFFER_EVENT_PROCESSING;
            
        /* Add data read transfer */
        pDrvObject->rxDMAState = DRV_SPI_DMA_DATA_INPROGRESS;
        SYS_DMA_ChannelTransferAdd(pDrvObject->dmaChannelHandleRead,
                (const void*)&pDrvObject->spiId->SPI_RDR, spiSize, 
                rxBuffer, size, spiSize);
                
        /* Add dummy write transfer */
        pDrvObject->txDMAState = DRV_SPI_DMA_DUMMY_INPROGRESS;
        SYS_DMA_ChannelTransferAdd(pDrvObject->dmaChannelHandleWrite, 
                drvSpiTxDummy, size, (const void*)&pDrvObject->spiId->SPI_TDR, spiSize, spiSize);
    }
    else
    {
        /* Update job elements */
        pJob->rxBuffer = rxBuffer;
        pJob->dataLeftToRx = size;
        pJob->dummyLeftToTx = size;
    }
    
    if (DRV_SPI_SYS_QUEUE_EnqueueLock(pDrvObject->queue, (void*)pJob) != DRV_SPI_SYS_QUEUE_SUCCESS)
    {
        SYS_ASSERT(false, "\r\nSPI Driver: Error enqueing new job.");
        return (DRV_SPI_BUFFER_HANDLE)DRV_SPI_BUFFER_HANDLE_INVALID;
    }
    
    /* Restore/Enable the interrupts */
    if (pDrvObject->taskMode == DRV_SPI_TASK_MODE_ISR)
    {
        /* Enable Receive interrupt source */
        if( (false == pDrvObject->rxIntEnabled) && (SYS_DMA_CHANNEL_HANDLE_INVALID == pDrvObject->dmaChannelHandleRead ) )
        {
            pDrvObject->spiId->SPI_IER.w = SPI_IER_RDRF_Msk;
            pDrvObject->rxIntEnabled = true;
        }
        if( (false == pDrvObject->txIntEnabled) && (SYS_DMA_CHANNEL_HANDLE_INVALID == pDrvObject->dmaChannelHandleWrite ) )
        {
            pDrvObject->spiId->SPI_IER.w = SPI_IER_TDRE_Msk;
            pDrvObject->txIntEnabled = true;
        }
        
        /* Restore system interrupt */
        if(SYS_DMA_CHANNEL_HANDLE_INVALID != pDrvObject->dmaChannelHandleRead)
        {
            SYS_INT_SourceEnable(pDrvObject->interruptDMA);
        }
        
        SYS_INT_SourceEnable(pDrvObject->interruptSPI);
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
    DRV_SPI_JOB_OBJECT * pJob = NULL;

    if(!(pClient->intent & DRV_IO_INTENT_WRITE))
    {
        /* Driver is not open in Write mode */
        SYS_ASSERT(false, "\r\nSPI Driver: Driver is not open in WRITE mode.");
        return (DRV_SPI_BUFFER_HANDLE)DRV_SPI_BUFFER_HANDLE_INVALID;
    }

    if ((pClient > &sSPIClientInstances[DRV_SPI_CLIENTS_NUMBER-1]) || (pClient < &sSPIClientInstances[0]))
    {
         SYS_ASSERT(false, "\r\nSPI Driver: Trying to access a client to a driver that is outside the range of client handles.");
         return (DRV_SPI_BUFFER_HANDLE)DRV_SPI_BUFFER_HANDLE_INVALID;
    }

    if (pClient->pNext != NULL)
    {
        SYS_ASSERT(false, "\r\nSPI Driver: Trying to access a client to a driver that isn't being used.");
        return (DRV_SPI_BUFFER_HANDLE)DRV_SPI_BUFFER_HANDLE_INVALID;
    }
    
    /* If size is odd number then 16 bit transfer is not possible */
    if( (SPI_COMMUNICATION_WIDTH_16BITS == pDrvObject->commWidth) &&  ((size & 0x1) != 0) )
    {
        SYS_ASSERT(false, "\r\nSPI Driver: invalid input size, must be a multiple of 16 bits.");
        return (DRV_SPI_BUFFER_HANDLE)DRV_SPI_BUFFER_HANDLE_INVALID;  
    }
    
    /* Disable the interrupt, as we are updating the queue */ 
    if (pDrvObject->taskMode == DRV_SPI_TASK_MODE_ISR)
    {
        if(SYS_DMA_CHANNEL_HANDLE_INVALID != pDrvObject->dmaChannelHandleWrite)
        {
            SYS_INT_SourceDisable(pDrvObject->interruptDMA);
        }
        SYS_INT_SourceDisable(pDrvObject->interruptSPI);
    }
    
    if (DRV_SPI_SYS_QUEUE_AllocElementLock(pDrvObject->queue, (void **)&pJob) != DRV_SPI_SYS_QUEUE_SUCCESS)
    {
        SYS_ASSERT(false, "\r\nSPI Driver: Error trying to get a free entry.");
        return (DRV_SPI_BUFFER_HANDLE)DRV_SPI_BUFFER_HANDLE_INVALID;
    }

    /* Update buffer job parameters */
    memset(pJob, 0, sizeof(DRV_SPI_JOB_OBJECT));
    pJob->completeCB = completeCB;
    pJob->context = context;
    pJob->status = DRV_SPI_BUFFER_EVENT_PENDING;
    pJob->pClient = pClient;
    if (jobHandle != NULL )
    {
        *jobHandle = (DRV_SPI_BUFFER_HANDLE)pJob;
    }
        
    if(SYS_DMA_CHANNEL_HANDLE_INVALID != pDrvObject->dmaChannelHandleWrite && SYS_DMA_CHANNEL_HANDLE_INVALID != pDrvObject->dmaChannelHandleRead && DRV_SPI_SYS_QUEUE_IsEmpty(pDrvObject->queue) && pDrvObject->rxDMAState == DRV_SPI_DMA_NONE)
    {
        /* Since this is the first buffer in the queue, add it immediately to
         * DMA for processing */
        
        /* Enable SPI error interrupts */
        SYS_INT_SourceEnable(pDrvObject->interruptSPI);
        
        /*Enable DMA system interrupt before adding buffer to 
         * transmit */
        SYS_INT_SourceEnable(pDrvObject->interruptDMA);
        
        /* Call the operation starting function pointer.  
         * This can be used to modify the slave select lines */
        DRV_SPI_CLIENT_OBJECT * pClient = (DRV_SPI_CLIENT_OBJECT*)pJob->pClient;
        if (pClient->operationStarting != NULL)
        {
            (*pClient->operationStarting)(DRV_SPI_BUFFER_EVENT_PROCESSING, (DRV_SPI_BUFFER_HANDLE)pJob, pJob->context);
        }
        /* Check the baud rate.  If its different set the new baud rate*/
        if (pClient->baudRate != pDrvObject->currentBaudRate)
        {
            pDrvObject->spiId->SPI_CSR[0].w = ( (pDrvObject->spiId->SPI_CSR[0].w & (~SPI_CSR_SCBR_Msk)) | (SPI_CSR_SCBR( pDrvObject->spiClkSrc/pClient->baudRate )) );
            pDrvObject->currentBaudRate = pClient->baudRate;
        }
        
        /* List the new job as processing*/
        pJob->status = DRV_SPI_BUFFER_EVENT_PROCESSING;
        
        /* Add dummy read transfer */
        pDrvObject->rxDMAState = DRV_SPI_DMA_DUMMY_INPROGRESS;
        SYS_DMA_ChannelTransferAdd(pDrvObject->dmaChannelHandleRead,
                (const void*)&pDrvObject->spiId->SPI_RDR, spiSize, 
                drvSpiRxDummy, size, spiSize);
                
        /* Add data write transfer */
        pDrvObject->txDMAState = DRV_SPI_DMA_DATA_INPROGRESS;
        SYS_DMA_ChannelTransferAdd(pDrvObject->dmaChannelHandleWrite, 
                txBuffer, size, (const void*)&pDrvObject->spiId->SPI_TDR, spiSize, spiSize);
    }
    else
    {
        /* Update job elements */
        pJob->txBuffer = txBuffer;
        pJob->dataLeftToTx = size;
        pJob->dummyLeftToRx = size;
    }
    
    if (DRV_SPI_SYS_QUEUE_EnqueueLock(pDrvObject->queue, (void*)pJob) != DRV_SPI_SYS_QUEUE_SUCCESS)
    {
        SYS_ASSERT(false, "\r\nSPI Driver: Error enqueing new job.");
        return (DRV_SPI_BUFFER_HANDLE)DRV_SPI_BUFFER_HANDLE_INVALID;
    }
        
    /* Restore/Enable the interrupts */
    if (pDrvObject->taskMode == DRV_SPI_TASK_MODE_ISR)
    {
        /* Enable Transmit interrupt source. */
        if( (false == pDrvObject->rxIntEnabled) && (SYS_DMA_CHANNEL_HANDLE_INVALID == pDrvObject->dmaChannelHandleRead ) )
        {
            pDrvObject->spiId->SPI_IER.w = SPI_IER_RDRF_Msk;
            pDrvObject->rxIntEnabled = true;
        }
        if( (false == pDrvObject->txIntEnabled) && (SYS_DMA_CHANNEL_HANDLE_INVALID == pDrvObject->dmaChannelHandleWrite ) )
        {
            pDrvObject->spiId->SPI_IER.w = SPI_IER_TDRE_Msk;
            pDrvObject->txIntEnabled = true;
        }
        
        /* Restore system interrupt */
        if(SYS_DMA_CHANNEL_HANDLE_INVALID != pDrvObject->dmaChannelHandleWrite)
        {
            SYS_INT_SourceEnable(pDrvObject->interruptDMA);
        }
        
        SYS_INT_SourceEnable(pDrvObject->interruptSPI);
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
    DRV_SPI_JOB_OBJECT * pJob = NULL;

    if ((pClient > &sSPIClientInstances[DRV_SPI_CLIENTS_NUMBER-1]) || (pClient < &sSPIClientInstances[0]))
    {
        SYS_ASSERT(false, "\r\nSPI Driver: Trying to access a client to a driver that is outside the range of client handles.");
        return (DRV_SPI_BUFFER_HANDLE)DRV_SPI_BUFFER_HANDLE_INVALID;
    }

    if (pClient->pNext != NULL)
    {
        SYS_ASSERT(false, "\r\nSPI Driver: Trying to access a client to a driver that isn't being used.");
        return (DRV_SPI_BUFFER_HANDLE)DRV_SPI_BUFFER_HANDLE_INVALID;
    }

    /* If size is odd number then 16 bit transfer is not possible */
    if( (SPI_COMMUNICATION_WIDTH_16BITS == pDrvObject->commWidth) &&  ( ((txSize & 0x1) != 0) || ((rxSize & 0x1) != 0) ) )
    {
        SYS_ASSERT(false, "\r\nSPI Driver: invalid input size, must be a multiple of 16 bits.");
        return (DRV_SPI_BUFFER_HANDLE)DRV_SPI_BUFFER_HANDLE_INVALID;  
    }
    
    /* Disable the interrupt, as we are updating the queue */ 
    if (pDrvObject->taskMode == DRV_SPI_TASK_MODE_ISR)
    {
        if(SYS_DMA_CHANNEL_HANDLE_INVALID != pDrvObject->dmaChannelHandleWrite)
        {
            SYS_INT_SourceDisable(pDrvObject->interruptDMA);
        }
        SYS_INT_SourceDisable(pDrvObject->interruptSPI);
    }
    
    if (DRV_SPI_SYS_QUEUE_AllocElementLock(pDrvObject->queue, (void **)&pJob) != DRV_SPI_SYS_QUEUE_SUCCESS)
    {
        SYS_ASSERT(false, "\r\nSPI Driver: Error trying to get a free entry.");
        return (DRV_SPI_BUFFER_HANDLE)DRV_SPI_BUFFER_HANDLE_INVALID;
    }

    /* Update buffer job parameters */
    memset(pJob, 0, sizeof(DRV_SPI_JOB_OBJECT));
    pJob->pClient = pClient;
    pJob->completeCB = completeCB;
    pJob->context = context;
    pJob->status = DRV_SPI_BUFFER_EVENT_PENDING;
    if (jobHandle != NULL )
    {
        *jobHandle = (DRV_SPI_BUFFER_HANDLE)pJob;
    }
    
    /* Handling dummy bytes needed to queue */

        
        /* If Master, calculate dummy bytes needed */
        if (rxSize > txSize)
        {
            pJob->dummyLeftToTx = rxSize - txSize;
        }
        else if (txSize > rxSize)
        {
            pJob->dummyLeftToRx = txSize - rxSize;
        }
        
    if(SYS_DMA_CHANNEL_HANDLE_INVALID != pDrvObject->dmaChannelHandleWrite && SYS_DMA_CHANNEL_HANDLE_INVALID != pDrvObject->dmaChannelHandleRead && DRV_SPI_SYS_QUEUE_IsEmpty(pDrvObject->queue) && pDrvObject->rxDMAState == DRV_SPI_DMA_NONE)
    {
        /* Since this is the first buffer in the queue, add it immediately to
         * DMA for processing */
        
        /* Enable SPI error interrupts */
        SYS_INT_SourceEnable(pDrvObject->interruptSPI);
        
        /*Enable DMA system interrupt before adding buffer to 
         * transmit */
        SYS_INT_SourceEnable(pDrvObject->interruptDMA);
        
        /* Call the operation starting function pointer.  
         * This can be used to modify the slave select lines */
        DRV_SPI_CLIENT_OBJECT * pClient = (DRV_SPI_CLIENT_OBJECT*)pJob->pClient;
        if (pClient->operationStarting != NULL)
        {
            (*pClient->operationStarting)(DRV_SPI_BUFFER_EVENT_PROCESSING, (DRV_SPI_BUFFER_HANDLE)pJob, pJob->context);
        }
        /* Check the baud rate.  If its different set the new baud rate*/
        if (pClient->baudRate != pDrvObject->currentBaudRate)
        {
            pDrvObject->spiId->SPI_CSR[0].w = ( (pDrvObject->spiId->SPI_CSR[0].w & (~SPI_CSR_SCBR_Msk)) | (SPI_CSR_SCBR( pDrvObject->spiClkSrc/pClient->baudRate )) );
            pDrvObject->currentBaudRate = pClient->baudRate;
        }
        
        /* List the new job as processing*/
        pJob->status = DRV_SPI_BUFFER_EVENT_PROCESSING;
        
        /* Add data read transfer */
        pDrvObject->rxDMAState = DRV_SPI_DMA_DATA_INPROGRESS;
        SYS_DMA_ChannelTransferAdd(pDrvObject->dmaChannelHandleRead,
                (const void*)&pDrvObject->spiId->SPI_RDR, spiSize, 
                rxBuffer, rxSize, spiSize);
                
        /* Add data write transfer */
        pDrvObject->txDMAState = DRV_SPI_DMA_DATA_INPROGRESS;
        SYS_DMA_ChannelTransferAdd(pDrvObject->dmaChannelHandleWrite, 
                txBuffer, txSize, (const void*)&pDrvObject->spiId->SPI_TDR, spiSize, spiSize);
    }
    else
    {
        pJob->txBuffer = txBuffer;
        pJob->dataLeftToTx = txSize;
        pJob->rxBuffer = rxBuffer;
        pJob->dataLeftToRx = rxSize;
    }

    if (DRV_SPI_SYS_QUEUE_EnqueueLock(pDrvObject->queue, (void*)pJob) != DRV_SPI_SYS_QUEUE_SUCCESS)
    {
        SYS_ASSERT(false, "\r\nSPI Driver: Error enqueing new job.");
        return (DRV_SPI_BUFFER_HANDLE)DRV_SPI_BUFFER_HANDLE_INVALID;
    }
        
    /* Restore/Enable the interrupts */
    if (pDrvObject->taskMode == DRV_SPI_TASK_MODE_ISR)
    {
        /* Enable Receive interrupt source */
        if( (false == pDrvObject->txIntEnabled) && (SYS_DMA_CHANNEL_HANDLE_INVALID == pDrvObject->dmaChannelHandleWrite ) )
        {
            pDrvObject->spiId->SPI_IER.w = SPI_IER_TDRE_Msk;
            pDrvObject->txIntEnabled = true;
        }
        if( (false == pDrvObject->rxIntEnabled) && (SYS_DMA_CHANNEL_HANDLE_INVALID == pDrvObject->dmaChannelHandleWrite ) )
        {
            pDrvObject->spiId->SPI_IER.w = SPI_IER_RDRF_Msk;
            pDrvObject->rxIntEnabled = true;
        }
        
        /* Restore system interrupt */
        if(SYS_DMA_CHANNEL_HANDLE_INVALID != pDrvObject->dmaChannelHandleWrite)
        {
            SYS_INT_SourceEnable(pDrvObject->interruptDMA);
        }
        
        SYS_INT_SourceEnable(pDrvObject->interruptSPI);
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


int32_t DRV_SPI_ClientConfigure ( DRV_HANDLE handle,
                                 const DRV_SPI_CLIENT_DATA * cfgData  )
{
    DRV_SPI_CLIENT_OBJECT * pClient = (DRV_SPI_CLIENT_OBJECT *)handle;
    struct DRV_SPI_DRIVER_OBJECT * pDrvObject = pClient->driverObject;
    
    
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
        /* just save the new baud rate value for this client, don't change it 
         * here. Actual change will happen in the task routine when queue 
         * request is serviced */
        pClient->baudRate = cfgData->baudRate;
    }
    else
    {
        pClient->baudRate = pDrvObject->baudRate;
    }
    
    return 0;
}

// *****************************************************************************
// *****************************************************************************
// Section: Local functions
// *****************************************************************************
// *****************************************************************************

int32_t _DRV_SPI_SetupHardware
(
   struct DRV_SPI_DRIVER_OBJECT * driverObject, 
   const DRV_SPI_INIT * const init
)
{
    uint8_t baud_div = 0;
    uint32_t dummy = 0;
    volatile spi_registers_t *spiId = (volatile spi_registers_t *)init->spiId;

    /* Disable and Reset the SPI*/
    spiId->SPI_CR.w = SPI_CR_SPIDIS_Msk;
    spiId->SPI_CR.w = SPI_CR_SWRST_Msk;
    
    /* Set up Master or Slave Mode*/
    if (init->spiMode == DRV_SPI_MODE_MASTER)
    {
        /* Using Chip select line(CSRx) 0 for clock configuration */
	    spiId->SPI_MR.w &= ~SPI_MR_PS_Msk;
        spiId->SPI_MR.w &= ~SPI_MR_PCSDEC_Msk;
	    spiId->SPI_MR.w = ( (spiId->SPI_MR.w & (~SPI_MR_PCS_Msk)) | SPI_MR_PCS(SPI_MR_PCS_PCS0_Val) );
        
        /* Enable Master mode */
        spiId->SPI_MR.w |= SPI_MR_MSTR_Msk;
    }
    else if (init->spiMode == DRV_SPI_MODE_SLAVE)
    {
        /* Enable Slave mode */
        spiId->SPI_MR.w &= ~SPI_MR_MSTR_Msk;
    }
    else
    {
        SYS_ASSERT(false, "\r\nInvalid SPI Configuration.");
        return -1;
    }

     /* Disable Mode Fault Detection. */
    spiId->SPI_MR.w |= SPI_MR_MODFDIS_Msk;
    
    /* Using Chip select line(CSRx) 0 for clock configuration for now */
    /* Set up clock Polarity and data phase */
    spiId->SPI_CSR[0].w = 0;
    spiId->SPI_CSR[0].w |= ( (init->clockMode << SPI_CSR_CPOL_Pos) & SPI_CSR_CPOL_Msk );
    spiId->SPI_CSR[0].w |= ( ((init->clockMode >> 1) << SPI_CSR_NCPHA_Pos) & SPI_CSR_NCPHA_Msk );
    
    /* Communication Width Selection */
    spiId->SPI_CSR[0].w = ( (spiId->SPI_CSR[0].w & (~SPI_CSR_BITS_Msk)) | (SPI_CSR_BITS( init->commWidth )) );    

    /* Baudrate selection */
    /* Set if Master Mode*/
    if (init->spiMode == DRV_SPI_MODE_MASTER)
    {
        /* Calculate baud divider value */
        baud_div = init->spiClkSrc/init->baudRate;
        
        /* check if baud_div is invalid */
        if (0 == baud_div)
        {
            /* Error, baudrate divider cant be 0 */
            SYS_ASSERT(false, "\r\nError, Selected Baudrate is higher than Peripheral clock ");
            return -1;
        }   
        
        /* Write valid baudrate divider into SCBR */
        spiId->SPI_CSR[0].w = ( (spiId->SPI_CSR[0].w & (~SPI_CSR_SCBR_Msk)) | (SPI_CSR_SCBR( baud_div )) );
        
        driverObject->currentBaudRate = init->baudRate;
        driverObject->baudRate = init->baudRate;
    }

    /* Flush the RX buffer */
    dummy = spiId->SPI_RDR.RD;
    (void)dummy;

    /* Note: We do not enable the SPI here, that will be done by 
     * the first client. */
    return 0;
}

int32_t _DRV_SPI_MasterTransmit( struct DRV_SPI_DRIVER_OBJECT * pDrvInstance )
{
    volatile spi_registers_t * spiId = pDrvInstance->spiId;
    register DRV_SPI_JOB_OBJECT * currentJob = pDrvInstance->currentJob;
    
    /* Check to see if we have any more bytes to transmit */
    if (currentJob->dataLeftToTx + currentJob->dummyLeftToTx == 0)
    {
        /* We don't have any more data to send make sure the transmit interrupt 
         * is disabled */
        return 0;
    }
    
    /* Check to see if the transmit buffer is empty*/
    if (!spiId->SPI_SR.TDRE)
    {
        return 0;
    }
    
    /* Make sure that we don't have something in progress and overrun the RX 
     * buffer */
    if (pDrvInstance->symbolsInProgress != 0)
    {
        return 0;
    }
    
    if (currentJob->dataLeftToTx != 0)
    {
        /* Transmit the data & update the counts */
        spiId->SPI_TDR.TD = currentJob->txBuffer[currentJob->dataTxed];
        currentJob->dataTxed++;
        currentJob->dataLeftToTx--;
    }
    else if ( (currentJob->dummyLeftToTx != 0))
    {
        /* Transmit the dummy data & update the counts */
        spiId->SPI_TDR.TD = pDrvInstance->dummyByteValue;
        currentJob->dummyLeftToTx--;
    }
    
    /* We now have a symbol in progress*/
    pDrvInstance->symbolsInProgress = 1;

    return 0;
}

int32_t _DRV_SPI_MasterReceive( struct DRV_SPI_DRIVER_OBJECT * pDrvInstance )

{
    volatile spi_registers_t * spiId = pDrvInstance->spiId;
    register DRV_SPI_JOB_OBJECT * currentJob = pDrvInstance->currentJob;
    uint32_t dummy;

    if (currentJob == NULL)
    {
        return 0;
    }
    
    if (spiId->SPI_SR.RDRF)
    {
        /* We have data waiting in the SPI buffer */
        if (currentJob->dataLeftToRx != 0)
        {
            /* Receive the data and updates the count */
            currentJob->rxBuffer[currentJob->dataRxed] = spiId->SPI_RDR.RD;
            currentJob->dataRxed++;
            currentJob->dataLeftToRx --;
        }
        else if ( (currentJob->dummyLeftToRx != 0))
        {
            /* No Data but dummy data: Note: We cannot just clear the
               buffer because we have to keep track of how many symbols/units we
               have received, and the number may have increased since we checked
               how full the buffer is.*/

            dummy = spiId->SPI_RDR.RD;
            currentJob->dummyLeftToRx--;
            
            (void)dummy;
        }
        
        /* No longer have a symbol in progress */
        pDrvInstance->symbolsInProgress = 0;
    }

    return 0;
}

int32_t _DRV_SPI_SlaveTransmit( struct DRV_SPI_DRIVER_OBJECT * pDrvInstance )
{
    volatile spi_registers_t * spiId = pDrvInstance->spiId;
    register DRV_SPI_JOB_OBJECT * currentJob = pDrvInstance->currentJob;
    
    if((spiId->SPI_SR.TDRE))
    {
        if ( (currentJob->dataLeftToTx != 0) )
        {
            /* Transmit the data & update the counts */
            spiId->SPI_TDR.TD = currentJob->txBuffer[currentJob->dataTxed];
            currentJob->dataTxed++;
            currentJob->dataLeftToTx--;
        }
        else if ( (currentJob->dummyLeftToTx != 0))
        {
            /* Transmit the data & update the counts */
            spiId->SPI_TDR.TD = pDrvInstance->dummyByteValue;
            currentJob->dummyLeftToTx--;
        }
    }

    return 0;
}

int32_t _DRV_SPI_SlaveReceive( struct DRV_SPI_DRIVER_OBJECT * pDrvInstance )
{
    volatile spi_registers_t * spiId = pDrvInstance->spiId;
    register DRV_SPI_JOB_OBJECT * currentJob = pDrvInstance->currentJob;
    uint32_t dummy;
    
    if((spiId->SPI_SR.RDRF))
    {
        if ( (currentJob->dataLeftToRx != 0) )
        {
            /* We have data waiting in the SPI buffer */

            /* Receive the data and updates the count */
            currentJob->rxBuffer[currentJob->dataRxed] = spiId->SPI_RDR.RD;
            currentJob->dataRxed++;
            currentJob->dataLeftToRx --;
        }
        else if ( (currentJob->dummyLeftToRx != 0))
        {
            /* We have dummy waiting in the SPI buffer */

            /* Receive the dummy to avoid receive overflow */
            dummy = spiId->SPI_RDR.RD;
            currentJob->dummyLeftToRx --;
            
            (void)dummy;
        }
    }

    return 0;
}


int32_t _DRV_SPI_ErrorTasks(struct DRV_SPI_DRIVER_OBJECT * pDrvInstance)
{
    volatile spi_registers_t * spiId = pDrvInstance->spiId;
    register DRV_SPI_JOB_OBJECT * currentJob = pDrvInstance->currentJob;

    if (pDrvInstance->currentJob == NULL)
    {
        return 0;
    }
    
    if (spiId->SPI_SR.OVRES)
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
        
        if (DRV_SPI_SYS_QUEUE_FreeElement(pDrvInstance->queue, currentJob) != DRV_SPI_SYS_QUEUE_SUCCESS)
        {
            SYS_ASSERT(false, "\r\nSPI Driver: Queue free element error.");
            return 0;
        }
        
        pDrvInstance->currentJob = NULL;
        
        /* Flush out RX data to clear error */
        spiId->SPI_RDR.RD;
    }
    
    return 0;
}

int32_t _DRV_SPI_MasterTasks(struct DRV_SPI_DRIVER_OBJECT * pDrvInstance )
{
    volatile bool continueLoop;
    DRV_SPI_JOB_OBJECT * currentJob = pDrvInstance->currentJob;
    volatile spi_registers_t * spiId = pDrvInstance->spiId;
    uint32_t dummy;
 
    SYS_INT_SourceDisable(pDrvInstance->interruptSPI);
  
    do {
        
        /* Check for a new task */
        if (pDrvInstance->currentJob == NULL)
        {
            if (DRV_SPI_SYS_QUEUE_Dequeue(pDrvInstance->queue, (void *)&(pDrvInstance->currentJob)) != DRV_SPI_SYS_QUEUE_SUCCESS)
            {
                SYS_ASSERT(false, "\r\nSPI Driver: Error in dequeing.");
                return 0;       
            }
            if (pDrvInstance->currentJob == NULL)
            {
                return 0;
            }
            currentJob = pDrvInstance->currentJob;

            pDrvInstance->symbolsInProgress = 0;

            /* Call the operation starting function pointer.  
             * This can be used to modify the slave select lines */
            DRV_SPI_CLIENT_OBJECT * pClient = (DRV_SPI_CLIENT_OBJECT*)currentJob->pClient;
            if (pClient->operationStarting != NULL)
            {
                (*pClient->operationStarting)(DRV_SPI_BUFFER_EVENT_PROCESSING, (DRV_SPI_BUFFER_HANDLE)currentJob, currentJob->context);
            }
            /* Check the baud rate.  If its different set the new baud rate*/
            if (pClient->baudRate != pDrvInstance->currentBaudRate)
            {
                spiId->SPI_CSR[0].w = ( (spiId->SPI_CSR[0].w & (~SPI_CSR_SCBR_Msk)) | (SPI_CSR_SCBR( pDrvInstance->spiClkSrc/pClient->baudRate )) );
                pDrvInstance->currentBaudRate = pClient->baudRate;
            }
            
            /* List the new job as processing*/
            currentJob->status = DRV_SPI_BUFFER_EVENT_PROCESSING;

            /* Flush out the Receive buffer */
            dummy = spiId->SPI_RDR.RD;
            (void)dummy;
        }
   
        continueLoop = false;
        
        /* Execute the sub tasks */ 
        _DRV_SPI_ErrorTasks(pDrvInstance);
        
        /* Check to see if we have any data left to receive and update the 
         * bytes left. */
        if(currentJob->dataLeftToTx +currentJob->dummyLeftToTx != 0)
        {
            _DRV_SPI_MasterTransmit(pDrvInstance);
        }
        
        if(currentJob->dataLeftToRx + currentJob->dummyLeftToRx != 0)
        {
            _DRV_SPI_MasterReceive(pDrvInstance);
        }
        
        /* Check to see if the job is done.
         * Master task is done when data is completely received from the 
         * shift register to complete the ring. 
         * So, check RX data and dummy count. */
        if (currentJob->dataLeftToRx + currentJob->dummyLeftToRx == 0)
        {
            /* Job is complete*/
            currentJob->status = DRV_SPI_BUFFER_EVENT_COMPLETE;
            
            /* Call the job complete call back*/
            if (currentJob->completeCB != NULL)
            {
                (*currentJob->completeCB)(DRV_SPI_BUFFER_EVENT_COMPLETE, (DRV_SPI_BUFFER_HANDLE)currentJob, currentJob->context);
            }
            
            /* Call the operation complete call back.  This is different than 
             * the job complete callback.  This can be used to modify the Slave 
             * Select line. The job complete callback can be used to free a 
             * client that is blocked waiting for complete*/
            DRV_SPI_CLIENT_OBJECT * pClient = (DRV_SPI_CLIENT_OBJECT*)currentJob->pClient;
            
            if (pClient->operationEnded != NULL)
            {
                (*pClient->operationEnded)(DRV_SPI_BUFFER_EVENT_COMPLETE, (DRV_SPI_BUFFER_HANDLE)currentJob, currentJob->context);
            }
            
            /* Return the job back to the free queue*/
            if (DRV_SPI_SYS_QUEUE_FreeElement(pDrvInstance->queue, currentJob) != DRV_SPI_SYS_QUEUE_SUCCESS)
            {
                SYS_ASSERT(false, "\r\nSPI Driver: Queue free element error.");
                return 0;
            }
            
            /* Clean up */
            pDrvInstance->currentJob = NULL;
            if (!DRV_SPI_SYS_QUEUE_IsEmpty(pDrvInstance->queue))
            {
                continueLoop = true;    
                continue;
            }
            else
            {   
                /* Disable the SPI interrupts as SPI queue is empty now. */
                if (pDrvInstance->taskMode == DRV_SPI_TASK_MODE_ISR)
                {
                    if(true == pDrvInstance->txIntEnabled)
                    {
                        pDrvInstance->spiId->SPI_IDR.w = SPI_IDR_TDRE_Msk;    
                        pDrvInstance->txIntEnabled = false;
                    }
                    if(true == pDrvInstance->rxIntEnabled)
                    {
                        pDrvInstance->spiId->SPI_IDR.w = SPI_IDR_RDRF_Msk;    
                        pDrvInstance->rxIntEnabled = false;
                    }
                    
                    SYS_INT_SourceDisable(pDrvInstance->interruptSPI);
                }
                break;
            }
        }
        
        if (pDrvInstance->taskMode == DRV_SPI_TASK_MODE_ISR)
        {
        /* Check to see if the interrupts would fire again if so just go back 
         * into the loop instead of suffering the interrupt latency of exiting 
         * and re-entering*/
        if (pDrvInstance->currentJob != NULL)
        {
            /* Interrupts should immediately become active again if they're in 
             * a fired condition */
            if ( (pDrvInstance->spiId->SPI_SR.RDRF) || (pDrvInstance->spiId->SPI_SR.TDRE) )
            {
                /* Interrupt would fire again anyway so we should just go back 
                 * to the start*/
                continueLoop = true;
                continue;                            
            }
            /* If we're here then we know that the interrupt should not be 
             * firing again immediately, so re-enable them and exit*/
            SYS_INT_SourceEnable(pDrvInstance->interruptSPI);

            return 0;            
        }
        }
    
    } while(continueLoop);
    
    return 0;
}

int32_t _DRV_SPI_SlaveTasks(struct DRV_SPI_DRIVER_OBJECT * pDrvInstance )
{
    volatile bool continueLoop;
    DRV_SPI_JOB_OBJECT * currentJob = pDrvInstance->currentJob;
    uint32_t dummy;

    SYS_INT_SourceDisable(pDrvInstance->interruptSPI);

    do {
        
        continueLoop = false;
    
        if (pDrvInstance->currentJob == NULL)
        {
            if (DRV_SPI_SYS_QUEUE_Dequeue(pDrvInstance->queue, (void *)&(pDrvInstance->currentJob)) != DRV_SPI_SYS_QUEUE_SUCCESS)
            {
                SYS_ASSERT(false, "\r\nSPI Driver: Error in dequeing.");
                return 0;       
            }
            
            if (pDrvInstance->currentJob == NULL)
            {
                return 0;
            }
            
            currentJob = pDrvInstance->currentJob;
            pDrvInstance->symbolsInProgress = 0;

            /* Call the operation starting function pointer.  
             * This can be used to modify the slave select lines */
            DRV_SPI_CLIENT_OBJECT * pClient = (DRV_SPI_CLIENT_OBJECT*)currentJob->pClient;
            
            if (pClient->operationStarting != NULL)
            {
                (*pClient->operationStarting)(DRV_SPI_BUFFER_EVENT_PROCESSING, (DRV_SPI_BUFFER_HANDLE)currentJob, currentJob->context);
            }
            
            /* List the new job as processing*/
            currentJob->status = DRV_SPI_BUFFER_EVENT_PROCESSING;

            /* Flush out the Receive buffer */
            dummy = pDrvInstance->spiId->SPI_RDR.RD;
            (void)dummy;
        }
        
        /* Check for errors, if any */
        _DRV_SPI_ErrorTasks(pDrvInstance);
        
        if(currentJob->dataLeftToTx +currentJob->dummyLeftToTx != 0)
        {
            _DRV_SPI_SlaveTransmit(pDrvInstance);
        }
        
        if(currentJob->dataLeftToRx + currentJob->dummyLeftToRx != 0)
        {
            _DRV_SPI_SlaveReceive(pDrvInstance);
        }
    
        /* Check to see if the job is done.
         * Slave task is done when data is moved outside shift register to 
         * complete the ring. So, check TX data and dummy count. */
        if (currentJob->dummyLeftToRx + currentJob->dataLeftToRx == 0)
        {
            /* Job is complete*/
            currentJob->status = DRV_SPI_BUFFER_EVENT_COMPLETE;
            
            /* Call the job complete call back*/
            if (currentJob->completeCB != NULL)
            {
                (*currentJob->completeCB)(DRV_SPI_BUFFER_EVENT_COMPLETE, (DRV_SPI_BUFFER_HANDLE)currentJob, currentJob->context);
            }
            
            /* Call the operation complete call back.  This is different than 
             * the job complete callback.  This can be used to modify the Slave 
             * Select line. The job complete callback can be used to free a 
             * client that is blocked waiting for complete*/
            DRV_SPI_CLIENT_OBJECT * pClient = (DRV_SPI_CLIENT_OBJECT*)currentJob->pClient;
            
            if (pClient->operationEnded != NULL)
            {
                (*pClient->operationEnded)(DRV_SPI_BUFFER_EVENT_COMPLETE, (DRV_SPI_BUFFER_HANDLE)currentJob, currentJob->context);
            }
            
            /* Return the job back to the free queue*/
            if (DRV_SPI_SYS_QUEUE_FreeElementLock(pDrvInstance->queue, currentJob) != DRV_SPI_SYS_QUEUE_SUCCESS)
            {
                SYS_ASSERT(false, "\r\nSPI Driver: Queue free element error.");
                return 0;
            }
            
            /* Clean up */
            pDrvInstance->currentJob = NULL;
            
            
            if (!DRV_SPI_SYS_QUEUE_IsEmpty(pDrvInstance->queue))
            {
                continueLoop = true;    
                continue;
            }
            else
            {                
                /* Disable the SPI interrupts as SPI queue is empty now. */
                if (pDrvInstance->taskMode == DRV_SPI_TASK_MODE_ISR)
                {
                    if(true == pDrvInstance->txIntEnabled)
                    {
                        pDrvInstance->spiId->SPI_IDR.w = SPI_IDR_TDRE_Msk;    
                        pDrvInstance->txIntEnabled = false;
                    }
                    if(true == pDrvInstance->rxIntEnabled)
                    {
                        pDrvInstance->spiId->SPI_IDR.w = SPI_IDR_RDRF_Msk;    
                        pDrvInstance->rxIntEnabled = false;
                    }
        
                    SYS_INT_SourceDisable(pDrvInstance->interruptSPI);
                }
                break;
            }
        }
        
        if (pDrvInstance->taskMode == DRV_SPI_TASK_MODE_ISR)
        {
        /* Check to see if the interrupts would fire again if so just go back 
         * into the loop instead of suffering the interrupt latency of exiting 
         * and re-entering*/
        if (pDrvInstance->currentJob != NULL)
        {
            /* Interrupts should immediately become active again if they're in 
             * a fired condition */
            if ( (pDrvInstance->spiId->SPI_SR.RDRF) || (pDrvInstance->spiId->SPI_SR.TDRE) )
            {
                /* Interrupt would fire again anyway so we should just go back 
                 * to the start*/
                continueLoop = true;
                continue;                            
            }
            /* If we're here then we know that the interrupt should not be 
             * firing again immediately, so re-enable them and exit*/
            SYS_INT_SourceEnable(pDrvInstance->interruptSPI);

            return 0;            
        }
        }

    } while(continueLoop);
    
    return 0;
}

// *****************************************************************************
/* Function:
    void _DRV_USART_DMA_EventHandler
    (
        SYS_DMA_TRANSFER_EVENT event,
        SYS_DMA_CHANNEL_HANDLE handle,
        uintptr_t contextHandle
    )

  Summary:
    Callback for the DMA channels

  Description:
    Callback for the DMA channels

  Remarks:
    None
 */

void _DRV_SPI_DMA_EventHandler
(
    SYS_DMA_TRANSFER_EVENT event,
    SYS_DMA_CHANNEL_HANDLE handle,
    uintptr_t contextHandle
)
{
    struct DRV_SPI_DRIVER_OBJECT *pDrvInstance;
    DRV_SPI_JOB_OBJECT *currentJob;

    if(SYS_DMA_CHANNEL_HANDLE_INVALID == handle || 0 == handle || 0 == contextHandle )
    {
        /* This means the handle is invalid */
        SYS_ASSERT(false, "Handle is invalid \r\n");
        return;
    }

    pDrvInstance = (struct DRV_SPI_DRIVER_OBJECT *)contextHandle;
    currentJob = (DRV_SPI_JOB_OBJECT *)pDrvInstance->currentJob;

    switch(event)
    {
        case SYS_DMA_TRANSFER_EVENT_COMPLETE:
        {        
            if(handle == pDrvInstance->dmaChannelHandleWrite)
            {
                pDrvInstance->txDMAState = DRV_SPI_DMA_COMPLETE;
            }
            else if(handle == pDrvInstance->dmaChannelHandleRead)
            {
                pDrvInstance->rxDMAState = DRV_SPI_DMA_COMPLETE;
            }
            break;
        }
        
        case SYS_DMA_TRANSFER_EVENT_ABORT:
        case SYS_DMA_TRANSFER_EVENT_ERROR:
        {
            if(handle == pDrvInstance->dmaChannelHandleWrite)
            {
                pDrvInstance->txDMAState = DRV_SPI_DMA_ERROR;
            }
            else if(handle == pDrvInstance->dmaChannelHandleRead)
            {
                pDrvInstance->rxDMAState = DRV_SPI_DMA_ERROR;
            }
            break;
        }
        
        default:
        {
            break;
        }
    }


    /* Call the DMA Tasks only if both DMA dummy/data transmit and 
     * DMA data/dummy are completed. */
    if( (DRV_SPI_DMA_COMPLETE == pDrvInstance->rxDMAState) && (DRV_SPI_DMA_COMPLETE == pDrvInstance->txDMAState) )
    {
        /* Find the next job in the queue and process it */
        _DRV_SPI_DMATasks(pDrvInstance, DRV_SPI_BUFFER_EVENT_COMPLETE);
    }
    else if( (DRV_SPI_DMA_ERROR == pDrvInstance->rxDMAState) || (DRV_SPI_DMA_ERROR == pDrvInstance->txDMAState))
    {
        _DRV_SPI_DMATasks(pDrvInstance, DRV_SPI_BUFFER_EVENT_ERROR);
    }
}

int32_t _DRV_SPI_DMATasks(struct DRV_SPI_DRIVER_OBJECT * pDrvInstance,  DRV_SPI_BUFFER_EVENT status)
{
    DRV_SPI_JOB_OBJECT * currentJob = pDrvInstance->currentJob;
    volatile spi_registers_t * spiId = pDrvInstance->spiId;
    
    /* Check to see if the job is done.
     * Master task is done when data is completely received from the 
     * shift register to complete the ring. 
     * So, check RX data and dummy count. */

    if (pDrvInstance->currentJob == NULL)
    {
        if (DRV_SPI_SYS_QUEUE_Dequeue(pDrvInstance->queue, (void *)&(pDrvInstance->currentJob)) != DRV_SPI_SYS_QUEUE_SUCCESS)
        {
            SYS_ASSERT(false, "\r\nSPI Driver: Error in dequeing.");
            return 0;       
        }
        if (pDrvInstance->currentJob == NULL)
        {
            return 0;
        }
    }
    
    /* Check for a current task */
    currentJob = pDrvInstance->currentJob;

    /* Update job status complete*/
    currentJob->status = status;

    /* Call the job complete call back*/
    if (currentJob->completeCB != NULL)
    {
        (*currentJob->completeCB)(currentJob->status, (DRV_SPI_BUFFER_HANDLE)currentJob, currentJob->context);
    }

    /* Call the operation complete call back.  This is different than the
       job complete callback.  This can be used to modify the Slave Select line.
       The job complete callback can be used to free a client that is blocked 
       waiting for complete*/
    DRV_SPI_CLIENT_OBJECT * pClient = (DRV_SPI_CLIENT_OBJECT*)currentJob->pClient;

    if (pClient->operationEnded != NULL)
    {
        (*pClient->operationEnded)(DRV_SPI_BUFFER_EVENT_COMPLETE, (DRV_SPI_BUFFER_HANDLE)currentJob, currentJob->context);
    }

    /* Return the job back to the free queue*/
    if (DRV_SPI_SYS_QUEUE_FreeElement(pDrvInstance->queue, currentJob) != DRV_SPI_SYS_QUEUE_SUCCESS)
    {
        SYS_ASSERT(false, "\r\nSPI Driver: Queue free element error.");
        return 0;
    }

    /* Clean up */
    pDrvInstance->currentJob = NULL;

    if ( DRV_SPI_SYS_QUEUE_IsEmpty(pDrvInstance->queue) )
    { 
        /* Disable the DMA interrupts as SPI queue is empty now. */
        SYS_INT_SourceDisable(pDrvInstance->interruptDMA);
        
        /* Update the DMA state to NONE as DMA is done with its job for now and is ready to take new jobs */
        pDrvInstance->rxDMAState = DRV_SPI_DMA_NONE;
        pDrvInstance->txDMAState = DRV_SPI_DMA_NONE;
    }
    else
    {
        /* Get the next job from the queue */
        if (DRV_SPI_SYS_QUEUE_Dequeue(pDrvInstance->queue, (void *)&(pDrvInstance->currentJob)) != DRV_SPI_SYS_QUEUE_SUCCESS)
        {
            SYS_ASSERT(false, "\r\nSPI Driver: Error in dequeing.");
            return 0;       
        }
        if (pDrvInstance->currentJob == NULL)
        {
            return 0;
        }
        currentJob = pDrvInstance->currentJob;

        /* Call the operation starting function pointer.  
         * This can be used to modify the slave select lines */
        DRV_SPI_CLIENT_OBJECT * pClient = (DRV_SPI_CLIENT_OBJECT*)currentJob->pClient;
        if (pClient->operationStarting != NULL)
        {
            (*pClient->operationStarting)(DRV_SPI_BUFFER_EVENT_PROCESSING, (DRV_SPI_BUFFER_HANDLE)currentJob, currentJob->context);
        }
        /* Check the baud rate.  If its different set the new baud rate*/
        if (pClient->baudRate != pDrvInstance->currentBaudRate)
        {
            spiId->SPI_CSR[0].w = ( (spiId->SPI_CSR[0].w & (~SPI_CSR_SCBR_Msk)) | (SPI_CSR_SCBR( pDrvInstance->spiClkSrc/pClient->baudRate )) );
            pDrvInstance->currentBaudRate = pClient->baudRate;
        }

        /* List the new job as processing*/
        currentJob->status = DRV_SPI_BUFFER_EVENT_PROCESSING;

        /* Check to see if we have any data left to transmit and update the 
         * bytes left. */
        if (currentJob->dataLeftToRx != 0)
        {
            /* Add data read transfer */
            pDrvInstance->rxDMAState = DRV_SPI_DMA_DATA_INPROGRESS;
            SYS_DMA_ChannelTransferAdd(pDrvInstance->dmaChannelHandleRead,
                    (const void*)&pDrvInstance->spiId->SPI_RDR, spiSize, 
                    currentJob->rxBuffer, currentJob->dataLeftToRx, spiSize);
        }
        else if ( (currentJob->dummyLeftToRx != 0))
        {
            /* Add dummy read transfer */
            pDrvInstance->rxDMAState = DRV_SPI_DMA_DUMMY_INPROGRESS;
            SYS_DMA_ChannelTransferAdd(pDrvInstance->dmaChannelHandleRead,
                    (const void*)&pDrvInstance->spiId->SPI_RDR, spiSize, 
                    drvSpiRxDummy, currentJob->dummyLeftToRx, spiSize);
        }

        /* Check to see if we have any data left to transmit and update the 
         * bytes left. */
        if (currentJob->dataLeftToTx != 0)
        {
            /* Add data write transfer */
            pDrvInstance->txDMAState = DRV_SPI_DMA_DATA_INPROGRESS;
            SYS_DMA_ChannelTransferAdd(pDrvInstance->dmaChannelHandleWrite, 
                    currentJob->txBuffer, currentJob->dataLeftToTx, (const void*)&pDrvInstance->spiId->SPI_TDR, spiSize, spiSize);
        }
        else if ( (currentJob->dummyLeftToTx != 0))
        {
            /* Add dummy write transfer */
            pDrvInstance->txDMAState = DRV_SPI_DMA_DUMMY_INPROGRESS;
            SYS_DMA_ChannelTransferAdd(pDrvInstance->dmaChannelHandleWrite, 
                    drvSpiTxDummy, currentJob->dummyLeftToTx, (const void*)&pDrvInstance->spiId->SPI_TDR, spiSize, spiSize);
        }
    }
    
    return 0;
}