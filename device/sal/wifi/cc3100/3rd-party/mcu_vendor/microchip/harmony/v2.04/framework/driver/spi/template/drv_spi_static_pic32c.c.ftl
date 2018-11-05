/*******************************************************************************
  SPI Driver Interface Implementation

  Company:
    Microchip Technology Inc.

  File Name:
   drv_spi_static_pic32c.c

  Summary:
   SPI Driver static implementation for PIC32C

  Description:
    The SPI Driver provides a interface to access the SPI hardware on the PIC32C
    microcontroller. This file implements the SPI Static Driver. This file
    should be included in the project if SPI driver functionality is needed.
*******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2016 released Microchip Technology Inc.  All rights reserved.

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

#include "driver/spi/static/src/drv_spi_static_local_pic32c.h"

/* Statically Allocated driver variables */
const size_t spiSize = 1;

static uint8_t sDrvSPIQueueArea[DRV_SPI_SYS_QUEUE_BUFFER_SIZE(DRV_SPI_INSTANCES_NUMBER, sizeof(DRV_SPI_JOB_OBJECT), DRV_SPI_QUEUE_DEPTH_COMBINED)];

/* This is the Queue Manager handle*/
DRV_SPI_SYS_QUEUE_MANAGER_HANDLE  hQueueManager;

static DRV_SPI_SYS_QUEUE_MANAGER_SETUP qmInitData = 
{
    sDrvSPIQueueArea,
    sizeof(sDrvSPIQueueArea),
    DRV_SPI_INSTANCES_NUMBER,
    sizeof(DRV_SPI_JOB_OBJECT),
    DRV_SPI_SYS_QUEUE_Fifo,
    <#if CONFIG_USE_3RDPARTY_RTOS ==true>
    false,
    0
    </#if>
};

static DRV_SPI_SYS_QUEUE_SETUP qInitData =
{
    0,
    10,
    0,
    <#if CONFIG_USE_3RDPARTY_RTOS ==true>
    false,
    0
    </#if>
};

<#macro DRV_SPI_STATIC_FUNCTIONS DRV_INSTANCE PLIB_INSTANCE CLOCK_SOURCE CLOCK_MODE BAUD_RATE SPI_INT QUEUE_SIZE TRANS_PER_SM USE_DMA DUMMY_BYTE DMA_DUMMY_BUFFER_SIZE>
<#assign DRV_MODE="Master">
<#assign DRV_ISR="ISR">
<#assign DRV_BIT_WIDTH="8">

<#if .vars["CONFIG_DRV_SPI_SPI_MODE_IDX${DRV_INSTANCE}"] == "SLAVE">
    <#assign DRV_MODE="Slave">
</#if>
<#if .vars["CONFIG_DRV_SPI_INTERRUPT_MODE"] == false>
    <#assign DRV_ISR="Polled">
</#if>
<#if .vars["CONFIG_DRV_SPI_USE_DMA_IDX${DRV_INSTANCE}"] == true>
    <#assign txIndex=.vars["CONFIG_DRV_SPI_TX_DMA_INST_IDX${DRV_INSTANCE}"]/>
    <#assign TX_DMA_CHANNEL=.vars["CONFIG_SYS_DMA_CHANNEL_ID_IDX${txIndex}"]>
    <#assign rxIndex=.vars["CONFIG_DRV_SPI_RX_DMA_INST_IDX${DRV_INSTANCE}"]/>
    <#assign RX_DMA_CHANNEL=.vars["CONFIG_SYS_DMA_CHANNEL_ID_IDX${rxIndex}"]>
</#if>    
<#if .vars["CONFIG_DRV_SPI_SPI_ID_IDX${DRV_INSTANCE}"] == "SPI_ID_0">
    <#assign TX_DMA_CHANNEL_TRIGGER="DMA_TRIGGER_SPI_0_TX"/>
    <#assign RX_DMA_CHANNEL_TRIGGER="DMA_TRIGGER_SPI_0_RX"/>
<#elseif .vars["CONFIG_DRV_SPI_SPI_ID_IDX${DRV_INSTANCE}"] == "SPI_ID_1">
    <#assign TX_DMA_CHANNEL_TRIGGER="DMA_TRIGGER_SPI_1_TX"/>
    <#assign RX_DMA_CHANNEL_TRIGGER="DMA_TRIGGER_SPI_1_RX"/>
</#if>

// *****************************************************************************
// *****************************************************************************
// Section: System functions for Instance ${DRV_INSTANCE} static driver
// *****************************************************************************
// *****************************************************************************

<#if .vars["CONFIG_DRV_SPI_USE_DMA_IDX${DRV_INSTANCE}"] == true>
uint8_t sDrvSpi${DRV_INSTANCE}RxDummy[${DMA_DUMMY_BUFFER_SIZE}];
uint8_t sDrvSpi${DRV_INSTANCE}TxDummy[${DMA_DUMMY_BUFFER_SIZE}];
</#if>

/* This is the driver static object . */
DRV_SPI_OBJ  gDrvSPI${DRV_INSTANCE}Obj ;

SYS_MODULE_OBJ DRV_SPI${DRV_INSTANCE}_Initialize(void)
{  
    DRV_SPI_OBJ *dObj = (DRV_SPI_OBJ*)NULL;

    /* Get a local pointer to driver instance object */ 
    dObj = &gDrvSPI${DRV_INSTANCE}Obj;
    
    /* Clear the object */
    memset(dObj, 0, sizeof(DRV_SPI_OBJ));
    
    /* Initialize SPI hardware */
    if (_DRV_SPI${DRV_INSTANCE}_SetupHardware() != 0)
    {
        return SYS_MODULE_OBJ_INVALID;
    }
    
<#if DRV_INSTANCE == "0">
    /* Initialize Queue only once for all instances of SPI driver*/
    if (DRV_SPI_SYS_QUEUE_Initialize(&qmInitData, &hQueueManager) != DRV_SPI_SYS_QUEUE_SUCCESS)
    {
        SYS_ASSERT(false, "\r\nSPI Driver: Could not create queuing system.");
        return SYS_MODULE_OBJ_INVALID;
    }
</#if>

    /* Update the Queue parameters. */
    qInitData.maxElements = ${QUEUE_SIZE}; //Queue size
    qInitData.reserveElements = ${QUEUE_SIZE}; //Mininmum number of job queues reserved is kept same as queue size

    /* Create Queue for this instance of SPI */
    if (DRV_SPI_SYS_QUEUE_CreateQueue(hQueueManager, &qInitData, &dObj->queue) != DRV_SPI_SYS_QUEUE_SUCCESS)
    {
        SYS_ASSERT(false, "\r\nSPI Driver: Could not set up driver instance queue.");
        return SYS_MODULE_OBJ_INVALID;
    }
    
    <#if CONFIG_USE_3RDPARTY_RTOS ==true>

    /* Create the hardware instance mutex. */
     if(OSAL_MUTEX_Create(&(dObj->mutexDriverInstance)) != OSAL_RESULT_TRUE)
     {
        return SYS_MODULE_OBJ_INVALID;
     }
    /* Create a Semaphore. */
    if (OSAL_SEM_Create(&(dObj->semaphoreDriverInstance), OSAL_SEM_TYPE_BINARY, 1, 1) != OSAL_RESULT_TRUE) {/*Report Error*/}
    qmInitData.semaphoreToUse = dObj->semaphoreDriverInstance;
    </#if>
<#if .vars["CONFIG_DRV_SPI_USE_DMA_IDX${DRV_INSTANCE}"] == true>   
    dObj->dmaChannelHandleWrite = SYS_DMA_CHANNEL_HANDLE_INVALID;
    dObj->dmaChannelHandleRead = SYS_DMA_CHANNEL_HANDLE_INVALID;

    /* DMA mode of operation. Allocate a handle for the specified channel.
     * Setup the channel for transfer */
    dObj->rxDMAState = DRV_SPI_DMA_NONE;
    dObj->txDMAState = DRV_SPI_DMA_NONE;
    memset(sDrvSpi${DRV_INSTANCE}TxDummy, ${DUMMY_BYTE}, sizeof(sDrvSpi${DRV_INSTANCE}TxDummy));
    
    /* TX DMA channel setup */
    if( DMA_CHANNEL_NONE != ${TX_DMA_CHANNEL})
    {
        dObj->dmaChannelHandleWrite = SYS_DMA_ChannelAllocate(${TX_DMA_CHANNEL});
        
        if(SYS_DMA_CHANNEL_HANDLE_INVALID != dObj->dmaChannelHandleWrite)
        {
            /* Setting the channel priority same as the channel number */
            SYS_DMA_ChannelSetup(dObj->dmaChannelHandleWrite,
                SYS_DMA_CHANNEL_OP_MODE_BASIC, ${TX_DMA_CHANNEL_TRIGGER});
                
            /* Setup DMA job event handler */
            SYS_DMA_ChannelTransferEventHandlerSet(dObj->dmaChannelHandleWrite, _DRV_SPI${DRV_INSTANCE}_DMA_EventHandler, (uintptr_t)dObj);
        }
    }

    /* RX DMA channel setup */
    if( DMA_CHANNEL_NONE != ${RX_DMA_CHANNEL} )
    {
        dObj->dmaChannelHandleRead = SYS_DMA_ChannelAllocate(${RX_DMA_CHANNEL});
        if(SYS_DMA_CHANNEL_HANDLE_INVALID != dObj->dmaChannelHandleRead)
        {
            /* Setting the channel priority same as the channel number */
            SYS_DMA_ChannelSetup(dObj->dmaChannelHandleRead,
                SYS_DMA_CHANNEL_OP_MODE_BASIC, ${RX_DMA_CHANNEL_TRIGGER});
                
            /* Setup DMA job event handler */
            SYS_DMA_ChannelTransferEventHandlerSet(dObj->dmaChannelHandleRead, _DRV_SPI${DRV_INSTANCE}_DMA_EventHandler, (uintptr_t)dObj);
        }
    }
</#if>    
    return (SYS_MODULE_OBJ)DRV_SPI_INDEX_${DRV_INSTANCE};
}

void DRV_SPI${DRV_INSTANCE}_Deinitialize (void)
{    
    DRV_SPI_OBJ *dObj = (DRV_SPI_OBJ*)NULL;
    dObj = &gDrvSPI${DRV_INSTANCE}Obj;

    volatile spi_registers_t *spiId = (volatile spi_registers_t *)${PLIB_INSTANCE};
    
<#if CONFIG_USE_3RDPARTY_RTOS ==true>
    /* Deallocate all mutexes */
    if(OSAL_MUTEX_Delete(&(dObj->mutexDriverInstance)) != OSAL_RESULT_TRUE)
    {
        SYS_DEBUG(SYS_ERROR_ERROR, "Mutex Delete Failed");
        return;
    }

    /* Delete all Semaphores */
    if (OSAL_SEM_Delete(&(dObj->semaphoreDriverInstance)) != OSAL_RESULT_TRUE) {/*Report Error*/}

</#if>
    /* Disable the SPI module associated with this instance */
    spiId->SPI_CR.w = SPI_CR_SPIDIS_Msk;;
<#if .vars["CONFIG_DRV_SPI_USE_DMA_IDX${DRV_INSTANCE}"] == true>
    if(SYS_DMA_CHANNEL_HANDLE_INVALID != dObj->dmaChannelHandleWrite)
    {
        SYS_DMA_ChannelRelease(dObj->dmaChannelHandleWrite);
    }
    if(SYS_DMA_CHANNEL_HANDLE_INVALID != dObj->dmaChannelHandleRead)
    {
        SYS_DMA_ChannelRelease(dObj->dmaChannelHandleRead);
    }
</#if>   
    /* Destroy instance queue */
    DRV_SPI_SYS_QUEUE_DestroyQueue(dObj->queue);
    
    /* Reset the instance object */
    memset(dObj, 0, sizeof(DRV_SPI_OBJ));
   
}

SYS_STATUS DRV_SPI${DRV_INSTANCE}_Status ( void )
{
    return SYS_STATUS_READY;
}

// *****************************************************************************
// *****************************************************************************
// Section: Client core functions
// *****************************************************************************
// *****************************************************************************

DRV_HANDLE DRV_SPI${DRV_INSTANCE}_Open ( void )
{  
    volatile spi_registers_t *spiId = (volatile spi_registers_t *)${PLIB_INSTANCE};
    
    /* Setup hardware steps for the client */
<#if DRV_ISR=="ISR">
    /* Enable SPI module level interrupt sources.
     * Only error(Overrun and underrun) interrupt is enabled for now.
     * TX and RX will be enabled while adding buffer requests */
    spiId->SPI_IER.w = SPI_IER_OVRES_Msk;
    spiId->SPI_IER.w = SPI_IER_UNDES_Msk;
    
    /* Enable Interrupt at system level */
    SYS_INT_SourceEnable(${SPI_INT});
</#if>
        
    /* Enable SPI */
    spiId->SPI_CR.w = SPI_CR_SPIEN_Msk;
        
<#if CONFIG_USE_3RDPARTY_RTOS ==true>
    DRV_SPI_OBJ *dObj = (DRV_SPI_OBJ*)NULL;
    dObj = &gDrvSPI${DRV_INSTANCE}Obj; 
    if (OSAL_SEM_Post(&(dObj->semaphoreDriverInstance)) != OSAL_RESULT_TRUE)
    {
        /* Report Error*/
    }
</#if>   
    return (DRV_HANDLE)DRV_SPI_INDEX_${DRV_INSTANCE};
}

void DRV_SPI${DRV_INSTANCE}_Close (void)
{
    DRV_SPI_OBJ *dObj = (DRV_SPI_OBJ*)NULL;
    dObj = &gDrvSPI${DRV_INSTANCE}Obj;

    volatile spi_registers_t *spiId = (volatile spi_registers_t *)${PLIB_INSTANCE};
    
 <#if CONFIG_USE_3RDPARTY_RTOS ==true>    
    if (OSAL_SEM_Pend(&(dObj->semaphoreDriverInstance), OSAL_WAIT_FOREVER) != OSAL_RESULT_TRUE)
    {
        SYS_ASSERT(false, "\r\nSPI Driver: Failed to get semaphore.");
        return;
    }
</#if>
    /* Disable SPI */
    spiId->SPI_CR.w = SPI_CR_SPIDIS_Msk;
    
    /* Reset instance queue */
    while (!DRV_SPI_SYS_QUEUE_IsEmpty(dObj->queue))
    {
        void * queueEntry = NULL;
        if (DRV_SPI_SYS_QUEUE_Dequeue(dObj->queue, &queueEntry) != DRV_SPI_SYS_QUEUE_SUCCESS)
        {
            SYS_ASSERT(false, "\r\nSPI Driver: Dequeue error.");
            <#if CONFIG_USE_3RDPARTY_RTOS ==true>  
            if (OSAL_SEM_Post(&(dObj->semaphoreDriverInstance)) != OSAL_RESULT_TRUE)
            {
                /* Report Error*/
            }
            </#if>
            return;
        }
        if (DRV_SPI_SYS_QUEUE_FreeElement(dObj->queue, queueEntry) != DRV_SPI_SYS_QUEUE_SUCCESS)
        {
            SYS_ASSERT(false, "\r\nSPI Driver: Queue free element error.");
            <#if CONFIG_USE_3RDPARTY_RTOS ==true>  
            if (OSAL_SEM_Post(&(dObj->semaphoreDriverInstance)) != OSAL_RESULT_TRUE)
            {
                /* Report Error*/
            }
            </#if>
            return;
        }
    }
    
    /* Disable interrupts */
 <#if DRV_ISR=="ISR">
    /* Disable all module level interrupt sources */
    spiId->SPI_IDR.w = SPI_IDR_Msk;
    
    /* Disable system level interrupt */
    SYS_INT_SourceDisable(${SPI_INT});
 </#if>
 
<#if CONFIG_USE_3RDPARTY_RTOS ==true>     
    if (OSAL_SEM_Post(&(dObj->semaphoreDriverInstance)) != OSAL_RESULT_TRUE)
    {
        /* Report Error*/
    }
 </#if>   
    return;
}

void DRV_SPI${DRV_INSTANCE}_Tasks ( void )
{
<#if DRV_MODE=="Master">
    _DRV_SPI${DRV_INSTANCE}_MasterTasks();
</#if>
<#if DRV_MODE=="Slave">
   _DRV_SPI${DRV_INSTANCE}_SlaveTasks(); 
</#if>
}

// *****************************************************************************
// *****************************************************************************
// Section: Transfer functions
// *****************************************************************************
// *****************************************************************************

DRV_SPI_BUFFER_HANDLE DRV_SPI${DRV_INSTANCE}_BufferAddRead2 ( void *rxBuffer,
                                              size_t size,
                                              DRV_SPI_BUFFER_EVENT_HANDLER completeCB,
                                              void * context, DRV_SPI_BUFFER_HANDLE * jobHandle)
{
    DRV_SPI_JOB_OBJECT * pJob = NULL;
    
    DRV_SPI_OBJ *dObj = (DRV_SPI_OBJ*)NULL;
    dObj = &gDrvSPI${DRV_INSTANCE}Obj;
    <#if DRV_ISR=="ISR">
    volatile spi_registers_t *spiId = (volatile spi_registers_t *)${PLIB_INSTANCE};
    </#if>
<#if DRV_BIT_WIDTH == "16">
    /* Check if the requested size of data can be processed with the configured Data Width */
    if ((size & 0x1) != 0)
    {
        SYS_ASSERT(false, "\r\nSPI Driver: invalid input size, must be a multiple of 16 bits.");
        return (DRV_SPI_BUFFER_HANDLE)DRV_SPI_BUFFER_HANDLE_INVALID;
    }
</#if>
    <#if DRV_ISR=="ISR">
    /* Disable the interrupt, as we are updating the queue */ 
    SYS_INT_SourceDisable(${SPI_INT});
<#if .vars["CONFIG_DRV_SPI_USE_DMA_IDX${DRV_INSTANCE}"] == true>
    if(SYS_DMA_CHANNEL_HANDLE_INVALID != dObj->dmaChannelHandleRead)
    {
    	SYS_INT_SourceDisable(XDMAC_IRQn);
    }
</#if>
    </#if>
    
    if (DRV_SPI_SYS_QUEUE_AllocElementLock(dObj->queue, (void **)&pJob) != DRV_SPI_SYS_QUEUE_SUCCESS)
    {
        SYS_ASSERT(false, "\r\nSPI Driver: Error trying to get a free entry.");
        return (DRV_SPI_BUFFER_HANDLE)DRV_SPI_BUFFER_HANDLE_INVALID;
    }

    /* Update buffer job parameters */
    memset(pJob, 0, sizeof(DRV_SPI_JOB_OBJECT));
<#if .vars["CONFIG_DRV_SPI_USE_DMA_IDX${DRV_INSTANCE}"] == false>
    pJob->rxBuffer = rxBuffer;
    pJob->dataLeftToRx = size;
    pJob->dummyLeftToTx = size;
</#if>
    pJob->completeCB = completeCB;
    pJob->context = context;
    pJob->status = DRV_SPI_BUFFER_EVENT_PENDING;
    if (jobHandle != NULL )
    {
        *jobHandle = (DRV_SPI_BUFFER_HANDLE)pJob;
    }
    
<#if .vars["CONFIG_DRV_SPI_USE_DMA_IDX${DRV_INSTANCE}"] == true>   
    if(SYS_DMA_CHANNEL_HANDLE_INVALID != dObj->dmaChannelHandleWrite && SYS_DMA_CHANNEL_HANDLE_INVALID != dObj->dmaChannelHandleRead && DRV_SPI_SYS_QUEUE_IsEmpty(dObj->queue) && dObj->rxDMAState == DRV_SPI_DMA_NONE)
    {
        /* Since this is the first buffer in the queue, add it immediately to
         * DMA for processing */
        
        /* Enable SPI error interrupts */
        SYS_INT_SourceEnable(${SPI_INT});
        
        /*Enable DMA system interrupt before adding buffer to transmit */
        SYS_INT_SourceEnable(XDMAC_IRQn);
        
        /* Call the operation starting function pointer.  
         * This can be used to modify the slave select lines */
        if (dObj->operationStarting != NULL)
        {
            (*dObj->operationStarting)(DRV_SPI_BUFFER_EVENT_PROCESSING, (DRV_SPI_BUFFER_HANDLE)pJob, pJob->context);
        }
            
        /* List the new job as processing*/
        pJob->status = DRV_SPI_BUFFER_EVENT_PROCESSING;
            
        /* Add data read transfer */
        dObj->rxDMAState = DRV_SPI_DMA_DATA_INPROGRESS;
        SYS_DMA_ChannelTransferAdd(dObj->dmaChannelHandleRead,
                (const void*)&spiId->SPI_RDR, spiSize, 
                rxBuffer, size, spiSize);
                
        /* Add dummy write transfer */
        dObj->txDMAState = DRV_SPI_DMA_DUMMY_INPROGRESS;
        SYS_DMA_ChannelTransferAdd(dObj->dmaChannelHandleWrite, 
                sDrvSpi${DRV_INSTANCE}TxDummy, size, (const void*)&spiId->SPI_TDR, spiSize, spiSize);
    }
    else
    {
        /* Update job elements */
        pJob->rxBuffer = rxBuffer;
        pJob->dataLeftToRx = size;
        pJob->dummyLeftToTx = size;
    }
</#if>    
    if (DRV_SPI_SYS_QUEUE_EnqueueLock(dObj->queue, (void*)pJob) != DRV_SPI_SYS_QUEUE_SUCCESS)
    {
        SYS_ASSERT(false, "\r\nSPI Driver: Error enqueing new job.");
        return (DRV_SPI_BUFFER_HANDLE)DRV_SPI_BUFFER_HANDLE_INVALID;
    }

    <#if DRV_ISR=="ISR">
    /* Restore/Enable the interrupts */
        /* Enable Receive/Transmit interrupt source for the first buffer only */
<#if .vars["CONFIG_DRV_SPI_USE_DMA_IDX${DRV_INSTANCE}"] == true>
        if((false == dObj->txIntEnabled) && (SYS_DMA_CHANNEL_HANDLE_INVALID == dObj->dmaChannelHandleWrite ))
<#else>
	if(false == dObj->txIntEnabled)
</#if>
        {
            spiId->SPI_IER.w = SPI_IER_TDRE_Msk;
            dObj->txIntEnabled = true;
        }
<#if .vars["CONFIG_DRV_SPI_USE_DMA_IDX${DRV_INSTANCE}"] == true>
        if((false == dObj->rxIntEnabled) && (SYS_DMA_CHANNEL_HANDLE_INVALID == dObj->dmaChannelHandleRead ))
<#else>
	if(false == dObj->rxIntEnabled)
</#if>
        {
            spiId->SPI_IER.w = SPI_IER_RDRF_Msk;
            dObj->rxIntEnabled = true;
        }
        
        /* Restore system interrupt */
    	SYS_INT_SourceEnable(${SPI_INT});
<#if .vars["CONFIG_DRV_SPI_USE_DMA_IDX${DRV_INSTANCE}"] == true>
	if(SYS_DMA_CHANNEL_HANDLE_INVALID != dObj->dmaChannelHandleRead)
        {
            SYS_INT_SourceEnable(XDMAC_IRQn);
        }
</#if>
    </#if>

    return (DRV_SPI_BUFFER_HANDLE)pJob;
}

DRV_SPI_BUFFER_HANDLE DRV_SPI${DRV_INSTANCE}_BufferAddWrite2 (void *txBuffer,
                                              size_t size,
                                              DRV_SPI_BUFFER_EVENT_HANDLER completeCB,
                                              void * context, DRV_SPI_BUFFER_HANDLE * jobHandle )
{
    DRV_SPI_JOB_OBJECT * pJob = NULL;

    DRV_SPI_OBJ *dObj = (DRV_SPI_OBJ*)NULL;
    dObj = &gDrvSPI${DRV_INSTANCE}Obj;
    
    <#if DRV_ISR=="ISR">
    volatile spi_registers_t *spiId = (volatile spi_registers_t *)${PLIB_INSTANCE};
    </#if>
    
<#if DRV_BIT_WIDTH == "16">

    /* Check if the requested size of data can be processed with the configured Data Width */
    if ((size & 0x1) != 0)
    {
        SYS_ASSERT(false, "\r\nSPI Driver: invalid input size, must be a multiple of 16 bits.");
        return (DRV_SPI_BUFFER_HANDLE)DRV_SPI_BUFFER_HANDLE_INVALID;
    }
</#if>
    <#if DRV_ISR=="ISR">
    /* Disable the interrupt, as we are updating the queue */
    SYS_INT_SourceDisable(${SPI_INT});
<#if .vars["CONFIG_DRV_SPI_USE_DMA_IDX${DRV_INSTANCE}"] == true>
    if(SYS_DMA_CHANNEL_HANDLE_INVALID != dObj->dmaChannelHandleWrite)
    {
    	SYS_INT_SourceDisable(XDMAC_IRQn);
    }
</#if>
    </#if>
    
    if (DRV_SPI_SYS_QUEUE_AllocElementLock(dObj->queue, (void **)&pJob) != DRV_SPI_SYS_QUEUE_SUCCESS)
    {
        SYS_ASSERT(false, "\r\nSPI Driver: Error trying to get a free entry.");
        return (DRV_SPI_BUFFER_HANDLE)DRV_SPI_BUFFER_HANDLE_INVALID;
    }

    /* Update buffer job parameters */
    memset(pJob, 0, sizeof(DRV_SPI_JOB_OBJECT));
<#if .vars["CONFIG_DRV_SPI_USE_DMA_IDX${DRV_INSTANCE}"] == false>
    pJob->txBuffer = txBuffer;
    pJob->dataLeftToTx = size;
    pJob->dummyLeftToRx = size;
</#if>
    pJob->completeCB = completeCB;
    pJob->context = context;
    pJob->status = DRV_SPI_BUFFER_EVENT_PENDING;
    if (jobHandle != NULL )
    {
        *jobHandle = (DRV_SPI_BUFFER_HANDLE)pJob;
    }
<#if .vars["CONFIG_DRV_SPI_USE_DMA_IDX${DRV_INSTANCE}"] == true>
    if(SYS_DMA_CHANNEL_HANDLE_INVALID != dObj->dmaChannelHandleWrite && SYS_DMA_CHANNEL_HANDLE_INVALID != dObj->dmaChannelHandleRead && DRV_SPI_SYS_QUEUE_IsEmpty(dObj->queue) && dObj->rxDMAState == DRV_SPI_DMA_NONE)
    {
        /* Since this is the first buffer in the queue, add it immediately to
         * DMA for processing */
        
        /* Enable SPI error interrupts */
        SYS_INT_SourceEnable(${SPI_INT});
        
        /*Enable DMA system interrupt before adding buffer to transmit */
        SYS_INT_SourceEnable(XDMAC_IRQn);
        
        /* Call the operation starting function pointer.  
         * This can be used to modify the slave select lines */
        if (dObj->operationStarting != NULL)
        {
            (*dObj->operationStarting)(DRV_SPI_BUFFER_EVENT_PROCESSING, (DRV_SPI_BUFFER_HANDLE)pJob, pJob->context);
        }
            
        /* List the new job as processing*/
        pJob->status = DRV_SPI_BUFFER_EVENT_PROCESSING;
            
        /* Add data read transfer */
        dObj->rxDMAState = DRV_SPI_DMA_DATA_INPROGRESS;
        SYS_DMA_ChannelTransferAdd(dObj->dmaChannelHandleRead,
                (const void*)&spiId->SPI_RDR, spiSize, 
               sDrvSpi${DRV_INSTANCE}RxDummy , size, spiSize);
                
        /* Add dummy write transfer */
        dObj->txDMAState = DRV_SPI_DMA_DUMMY_INPROGRESS;
        SYS_DMA_ChannelTransferAdd(dObj->dmaChannelHandleWrite, 
                txBuffer, size, (const void*)&spiId->SPI_TDR, spiSize, spiSize);
    }
    else
    {
        /* Update job elements */
        pJob->txBuffer = txBuffer;
        pJob->dataLeftToTx = size;
        pJob->dummyLeftToRx = size;
    }
</#if>   
    if (DRV_SPI_SYS_QUEUE_EnqueueLock(dObj->queue, (void*)pJob) != DRV_SPI_SYS_QUEUE_SUCCESS)
    {
        SYS_ASSERT(false, "\r\nSPI Driver: Error enqueing new job.");
        return (DRV_SPI_BUFFER_HANDLE)DRV_SPI_BUFFER_HANDLE_INVALID;
    }

    
    <#if DRV_ISR=="ISR">
    /* Restore/Enable the interrupts */
        /* Enable Receive/Transmit interrupt source for the first buffer only */
<#if .vars["CONFIG_DRV_SPI_USE_DMA_IDX${DRV_INSTANCE}"] == true>
        if((false == dObj->txIntEnabled) && (SYS_DMA_CHANNEL_HANDLE_INVALID == dObj->dmaChannelHandleWrite ))
<#else>
	if(false == dObj->txIntEnabled)
</#if>
        {
            spiId->SPI_IER.w = SPI_IER_TDRE_Msk;
            dObj->txIntEnabled = true;
        }
<#if .vars["CONFIG_DRV_SPI_USE_DMA_IDX${DRV_INSTANCE}"] == true>
        if((false == dObj->rxIntEnabled) && (SYS_DMA_CHANNEL_HANDLE_INVALID == dObj->dmaChannelHandleRead ))
<#else>
	if(false == dObj->rxIntEnabled)
</#if>
        {
            spiId->SPI_IER.w = SPI_IER_RDRF_Msk;
            dObj->rxIntEnabled = true;
        }
        
        /* Restore system interrupt */
    	SYS_INT_SourceEnable(${SPI_INT});
<#if .vars["CONFIG_DRV_SPI_USE_DMA_IDX${DRV_INSTANCE}"] == true>
	if(SYS_DMA_CHANNEL_HANDLE_INVALID != dObj->dmaChannelHandleRead)
        {
            SYS_INT_SourceEnable(XDMAC_IRQn);
        }
</#if>
    </#if>

    return (DRV_SPI_BUFFER_HANDLE)pJob;
}

DRV_SPI_BUFFER_HANDLE DRV_SPI${DRV_INSTANCE}_BufferAddWriteRead2 ( void *txBuffer, size_t txSize,
                                                   void *rxBuffer, size_t rxSize,
                                                   DRV_SPI_BUFFER_EVENT_HANDLER completeCB,
                                                   void * context, DRV_SPI_BUFFER_HANDLE * jobHandle )
{
    DRV_SPI_JOB_OBJECT * pJob = NULL;

    DRV_SPI_OBJ *dObj = (DRV_SPI_OBJ*)NULL;
    dObj = &gDrvSPI${DRV_INSTANCE}Obj;
    
    <#if DRV_ISR=="ISR">
    volatile spi_registers_t *spiId = (volatile spi_registers_t *)${PLIB_INSTANCE};
    </#if>
    
<#if DRV_BIT_WIDTH == "16">

    /* Check if the requested size of data can be processed with the configured Data Width */
    if ((size & 0x1) != 0)
    {
        SYS_ASSERT(false, "\r\nSPI Driver: invalid input size, must be a multiple of 16 bits.");
        return (DRV_SPI_BUFFER_HANDLE)DRV_SPI_BUFFER_HANDLE_INVALID;
    }
</#if>
    <#if DRV_ISR=="ISR">
    /* Disable the interrupt, as we are updating the queue */
    SYS_INT_SourceDisable(${SPI_INT});
<#if .vars["CONFIG_DRV_SPI_USE_DMA_IDX${DRV_INSTANCE}"] == true>
    if(SYS_DMA_CHANNEL_HANDLE_INVALID != dObj->dmaChannelHandleWrite)
    {
    	SYS_INT_SourceDisable(XDMAC_IRQn);
    }
</#if>
    </#if>
    
    if (DRV_SPI_SYS_QUEUE_AllocElementLock(dObj->queue, (void **)&pJob) != DRV_SPI_SYS_QUEUE_SUCCESS)
    {
        SYS_ASSERT(false, "\r\nSPI Driver: Error trying to get a free entry.");
        return (DRV_SPI_BUFFER_HANDLE)DRV_SPI_BUFFER_HANDLE_INVALID;
    }

    /* Update buffer job parameters */
    memset(pJob, 0, sizeof(DRV_SPI_JOB_OBJECT));
<#if .vars["CONFIG_DRV_SPI_USE_DMA_IDX${DRV_INSTANCE}"] == false>
    pJob->txBuffer = txBuffer;
    pJob->dataLeftToTx = txSize;
    pJob->rxBuffer = rxBuffer;
    pJob->dataLeftToRx = rxSize;
</#if>
    if (jobHandle != NULL )
    {
        *jobHandle = (DRV_SPI_BUFFER_HANDLE)pJob;
    }
    
    /* Calculate dummy bytes needed */
    if (rxSize > txSize)
    {
        pJob->dummyLeftToTx = rxSize - txSize;
    }
    else if (txSize > rxSize)
    {
        pJob->dummyLeftToRx = txSize - rxSize;
    }
    
    pJob->completeCB = completeCB;
    pJob->context = context;
    pJob->status = DRV_SPI_BUFFER_EVENT_PENDING;
<#if .vars["CONFIG_DRV_SPI_USE_DMA_IDX${DRV_INSTANCE}"] == true>
    if(SYS_DMA_CHANNEL_HANDLE_INVALID != dObj->dmaChannelHandleWrite && SYS_DMA_CHANNEL_HANDLE_INVALID != dObj->dmaChannelHandleRead && DRV_SPI_SYS_QUEUE_IsEmpty(dObj->queue) && dObj->rxDMAState == DRV_SPI_DMA_NONE)
    {
    
        /* Since this is the first buffer in the queue, add it immediately to
         * DMA for processing */
        
        /* Enable SPI error interrupts */
        SYS_INT_SourceEnable(${SPI_INT});
        
        /*Enable DMA system interrupt before adding buffer to transmit */
        SYS_INT_SourceEnable(XDMAC_IRQn);
        
        /* Call the operation starting function pointer.  
         * This can be used to modify the slave select lines */
        if (dObj->operationStarting != NULL)
        {
            (*dObj->operationStarting)(DRV_SPI_BUFFER_EVENT_PROCESSING, (DRV_SPI_BUFFER_HANDLE)pJob, pJob->context);
        }
            
        /* List the new job as processing*/
        pJob->status = DRV_SPI_BUFFER_EVENT_PROCESSING;
            
        /* Add data read transfer */
        dObj->rxDMAState = DRV_SPI_DMA_DATA_INPROGRESS;
        SYS_DMA_ChannelTransferAdd(dObj->dmaChannelHandleRead,
                (const void*)&spiId->SPI_RDR, spiSize, 
               rxBuffer , rxSize, spiSize);
                
        /* Add dummy write transfer */
        dObj->txDMAState = DRV_SPI_DMA_DUMMY_INPROGRESS;
        SYS_DMA_ChannelTransferAdd(dObj->dmaChannelHandleWrite, 
                txBuffer, txSize, (const void*)&spiId->SPI_TDR, spiSize, spiSize);
    }
    else
    {
        /* Update job elements */
        pJob->txBuffer = txBuffer;
        pJob->dataLeftToTx = txSize;
        pJob->rxBuffer = rxBuffer;
        pJob->dataLeftToRx = rxSize;
    }
</#if>    
    if (DRV_SPI_SYS_QUEUE_EnqueueLock(dObj->queue, (void*)pJob) != DRV_SPI_SYS_QUEUE_SUCCESS)
    {
        SYS_ASSERT(false, "\r\nSPI Driver: Error enqueing new job.");
        return (DRV_SPI_BUFFER_HANDLE)DRV_SPI_BUFFER_HANDLE_INVALID;
    }

    
    <#if DRV_ISR=="ISR">
    /* Restore/Enable the interrupts */
        /* Enable Receive/Transmit interrupt source for the first buffer only */
<#if .vars["CONFIG_DRV_SPI_USE_DMA_IDX${DRV_INSTANCE}"] == true>
        if((false == dObj->txIntEnabled) && (SYS_DMA_CHANNEL_HANDLE_INVALID == dObj->dmaChannelHandleWrite ))
<#else>
	if(false == dObj->txIntEnabled)
</#if>
        {
            spiId->SPI_IER.w = SPI_IER_TDRE_Msk;
            dObj->txIntEnabled = true;
        }
<#if .vars["CONFIG_DRV_SPI_USE_DMA_IDX${DRV_INSTANCE}"] == true>
        if((false == dObj->rxIntEnabled) && (SYS_DMA_CHANNEL_HANDLE_INVALID == dObj->dmaChannelHandleRead ))
<#else>
	if(false == dObj->rxIntEnabled)
</#if>
        {
            spiId->SPI_IER.w = SPI_IER_RDRF_Msk;
            dObj->rxIntEnabled = true;
        }
        
        /* Restore system interrupt */
    	SYS_INT_SourceEnable(${SPI_INT});
<#if .vars["CONFIG_DRV_SPI_USE_DMA_IDX${DRV_INSTANCE}"] == true>
	if(SYS_DMA_CHANNEL_HANDLE_INVALID != dObj->dmaChannelHandleRead)
        {
            SYS_INT_SourceEnable(XDMAC_IRQn);
        }
</#if>
    </#if>

    return (DRV_SPI_BUFFER_HANDLE)pJob;
}

DRV_SPI_BUFFER_HANDLE DRV_SPI${DRV_INSTANCE}_BufferAddRead ( void *rxBuffer,
                                              size_t size,
                                              DRV_SPI_BUFFER_EVENT_HANDLER completeCB,
                                              void * context)
{
    return DRV_SPI${DRV_INSTANCE}_BufferAddRead2(rxBuffer, size, completeCB, context, NULL);
}

DRV_SPI_BUFFER_HANDLE DRV_SPI${DRV_INSTANCE}_BufferAddWrite ( void *txBuffer,
                                              size_t size,
                                              DRV_SPI_BUFFER_EVENT_HANDLER completeCB,
                                              void * context )
{
    return DRV_SPI${DRV_INSTANCE}_BufferAddWrite2(txBuffer, size, completeCB, context, NULL);
}

DRV_SPI_BUFFER_HANDLE DRV_SPI${DRV_INSTANCE}_BufferAddWriteRead (void *txBuffer, size_t txSize,
                                                   void *rxBuffer, size_t rxSize,
                                                   DRV_SPI_BUFFER_EVENT_HANDLER completeCB,
                                                   void * context )
{
    return DRV_SPI${DRV_INSTANCE}_BufferAddWriteRead2(txBuffer, txSize, rxBuffer, rxSize, completeCB, context, NULL);
}


DRV_SPI_BUFFER_EVENT DRV_SPI${DRV_INSTANCE}_BufferStatus ( DRV_SPI_BUFFER_HANDLE bufferHandle )
{
    DRV_SPI_JOB_OBJECT * pJob = (DRV_SPI_JOB_OBJECT *)bufferHandle;

    return pJob->status;
}


int32_t DRV_SPI${DRV_INSTANCE}_ClientConfigure (const DRV_SPI_CLIENT_DATA * cfgData)
{    
    DRV_SPI_OBJ *dObj = (DRV_SPI_OBJ*)NULL;
    dObj = &gDrvSPI${DRV_INSTANCE}Obj;
    
    uint8_t baud_div = 0;
    volatile spi_registers_t *spiId = (volatile spi_registers_t *)${PLIB_INSTANCE};
    
    if (cfgData->operationStarting != NULL)
    {
        dObj->operationStarting = cfgData->operationStarting;
    }
    if (cfgData->operationEnded != NULL)
    {
        dObj->operationEnded = cfgData->operationEnded;
    }
    
    baud_div = ${CLOCK_SOURCE}/cfgData->baudRate;
    if (baud_div != 0)
    {
        /* Update baud rate */
        spiId->SPI_CSR[0].w = ( (spiId->SPI_CSR[0].w & (~SPI_CSR_SCBR_Msk)) | (SPI_CSR_SCBR( baud_div )) );
    }
    
    return 0;
}

// *****************************************************************************
// *****************************************************************************
// Section: Local functions
// *****************************************************************************
// *****************************************************************************

int32_t _DRV_SPI${DRV_INSTANCE}_SetupHardware(void)
{
    uint32_t dummy = 0;
    volatile spi_registers_t *spiId = (volatile spi_registers_t *)${PLIB_INSTANCE};

    /* Disable and Reset the SPI*/
    spiId->SPI_CR.w = SPI_CR_SPIDIS_Msk;
    spiId->SPI_CR.w = SPI_CR_SWRST_Msk;
    
    <#if DRV_MODE=="Master">
  
        /* Using Chip select line(CSRx) 0 for clock configuration */
	    spiId->SPI_MR.w &= ~SPI_MR_PS_Msk;
        spiId->SPI_MR.w &= ~SPI_MR_PCSDEC_Msk;
	    spiId->SPI_MR.w = ( (spiId->SPI_MR.w & (~SPI_MR_PCS_Msk)) | SPI_MR_PCS(SPI_MR_PCS_PCS0_Val) );
        
        /* Enable Master mode */
        spiId->SPI_MR.w |= SPI_MR_MSTR_Msk;
    
    </#if>
    <#if DRV_MODE=="Slave">
        /* Enable Slave mode */
        spiId->SPI_MR.w &= ~SPI_MR_MSTR_Msk;
    </#if>

     /* Disable Mode Fault Detection. */
    spiId->SPI_MR.w |= SPI_MR_MODFDIS_Msk;
    
    /* Using Chip select line(CSRx) 0 for clock configuration for now */
    /* Set up clock Polarity and data phase */
    spiId->SPI_CSR[0].w |= ( (${CLOCK_MODE} << SPI_CSR_CPOL_Pos) & SPI_CSR_CPOL_Msk );
    spiId->SPI_CSR[0].w |= ( ((${CLOCK_MODE}>> 1) << SPI_CSR_NCPHA_Pos) & SPI_CSR_NCPHA_Msk );
    
    /* Communication Width Selection */
    spiId->SPI_CSR[0].w = ( (spiId->SPI_CSR[0].w & (~SPI_CSR_BITS_Msk)) | (SPI_CSR_BITS( ${DRV_BIT_WIDTH}-8 )) ); 

    <#if DRV_MODE=="Master">
        uint8_t baud_div = 0;
    /* Baudrate selection */
        /* Calculate baud divider value */
        baud_div = (uint8_t)(${CLOCK_SOURCE}/${BAUD_RATE});
        
        /* check if baud_div is invalid */
        if (0 == baud_div)
        {
            /* Error, baudrate divider cant be 0 */
            SYS_ASSERT(false, "\r\nError, Selected Baudrate is higher than Peripheral clock ");
            return -1;
        }   
        
        /* Write valid baudrate divider into SCBR */
        spiId->SPI_CSR[0].w = ( (spiId->SPI_CSR[0].w & (~SPI_CSR_SCBR_Msk)) | (SPI_CSR_SCBR( baud_div )) );
        
    </#if>

    /* Flush the RX buffer */
    dummy = spiId->SPI_RDR.RD;
    (void)dummy;

    /* Note: We do not enable the SPI here, that will be done by 
     * the first client. */
    return 0;
}

int32_t _DRV_SPI${DRV_INSTANCE}_MasterTransmit(void)
{    
    DRV_SPI_OBJ *dObj = (DRV_SPI_OBJ*)NULL;
    dObj = &gDrvSPI${DRV_INSTANCE}Obj;
    
    register DRV_SPI_JOB_OBJECT * currentJob = dObj->currentJob;
    
    volatile spi_registers_t *spiId = (volatile spi_registers_t *)${PLIB_INSTANCE};
    
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
    
    /* Make sure that we don't have something in progress and overrun the 
     * RX buffer */
    if (dObj->symbolsInProgress != 0)
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
        spiId->SPI_TDR.TD = ${DUMMY_BYTE};
        currentJob->dummyLeftToTx--;
    }
    
    /* We now have a symbol in progress*/
    dObj->symbolsInProgress = 1;

    return 0;
}

int32_t _DRV_SPI${DRV_INSTANCE}_MasterReceive(void)

{
    DRV_SPI_OBJ *dObj = (DRV_SPI_OBJ*)NULL;
    dObj = &gDrvSPI${DRV_INSTANCE}Obj;
    
    register DRV_SPI_JOB_OBJECT * currentJob = dObj->currentJob;
    
    volatile spi_registers_t *spiId = (volatile spi_registers_t *)${PLIB_INSTANCE};
    
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
        dObj->symbolsInProgress = 0;
    }

    return 0;
}

int32_t _DRV_SPI${DRV_INSTANCE}_SlaveTransmit(void)
{
    DRV_SPI_OBJ *dObj = (DRV_SPI_OBJ*)NULL;
    dObj = &gDrvSPI${DRV_INSTANCE}Obj;
    
    register DRV_SPI_JOB_OBJECT * currentJob = dObj->currentJob;
    
    volatile spi_registers_t *spiId = (volatile spi_registers_t *)${PLIB_INSTANCE};
    
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
            spiId->SPI_TDR.TD = ${DUMMY_BYTE};
            currentJob->dummyLeftToTx--;
        }
    }

    return 0;
}

int32_t _DRV_SPI${DRV_INSTANCE}_SlaveReceive(void)
{
    DRV_SPI_OBJ *dObj = (DRV_SPI_OBJ*)NULL;
    dObj = &gDrvSPI${DRV_INSTANCE}Obj;
    
    register DRV_SPI_JOB_OBJECT * currentJob = dObj->currentJob;
    
    volatile spi_registers_t *spiId = (volatile spi_registers_t *)${PLIB_INSTANCE};
    
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


int32_t _DRV_SPI${DRV_INSTANCE}_ErrorTasks(void)
{
    DRV_SPI_OBJ *dObj = (DRV_SPI_OBJ*)NULL;
    dObj = &gDrvSPI${DRV_INSTANCE}Obj;
    
    register DRV_SPI_JOB_OBJECT * currentJob = dObj->currentJob;
    
    volatile spi_registers_t *spiId = (volatile spi_registers_t *)${PLIB_INSTANCE};
    
    uint32_t dummy;

    if (dObj->currentJob == NULL)
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
        
        if (dObj->operationEnded != NULL)
        {
            (*dObj->operationEnded)(DRV_SPI_BUFFER_EVENT_ERROR, (DRV_SPI_BUFFER_HANDLE)currentJob, currentJob->context);
        }
        
        if (DRV_SPI_SYS_QUEUE_FreeElement(dObj->queue, currentJob) != DRV_SPI_SYS_QUEUE_SUCCESS)
        {
            SYS_ASSERT(false, "\r\nSPI Driver: Queue free element error.");
            return 0;
        }
        
        dObj->currentJob = NULL;
        
        /* Flush out RX data to clear error */
        dummy = spiId->SPI_RDR.RD;
        (void)dummy;
    }
    
    return 0;
}

int32_t _DRV_SPI${DRV_INSTANCE}_MasterTasks(void)
{
    DRV_SPI_OBJ *dObj = (DRV_SPI_OBJ*)NULL;
    dObj = &gDrvSPI${DRV_INSTANCE}Obj;
    
    register DRV_SPI_JOB_OBJECT * currentJob = dObj->currentJob;
    
    volatile spi_registers_t *spiId = (volatile spi_registers_t *)${PLIB_INSTANCE};
    
    volatile bool continueLoop;
    uint32_t dummy;
    
    <#if DRV_ISR=="ISR">
    SYS_INT_SourceDisable(${SPI_INT});
    </#if>
    do {
        
        /* Check for a new task */
        if (dObj->currentJob == NULL)
        {
            if (DRV_SPI_SYS_QUEUE_Dequeue(dObj->queue, (void *)&(dObj->currentJob)) != DRV_SPI_SYS_QUEUE_SUCCESS)
            {
                SYS_ASSERT(false, "\r\nSPI Driver: Error in dequeing.");
                return 0;       
            }
            if (dObj->currentJob == NULL)
            {
                return 0;
            }
            currentJob = dObj->currentJob;

            dObj->symbolsInProgress = 0;

            /* Call the operation starting function pointer.  
             * This can be used to modify the slave select lines */
            
            if (dObj->operationStarting != NULL)
            {
                (*dObj->operationStarting)(DRV_SPI_BUFFER_EVENT_PROCESSING, (DRV_SPI_BUFFER_HANDLE)currentJob, currentJob->context);
            }
            
            /* List the new job as processing*/
            currentJob->status = DRV_SPI_BUFFER_EVENT_PROCESSING;

            /* Flush out the Receive buffer */
            dummy = spiId->SPI_RDR.RD;
            (void)dummy;
        }
   
        continueLoop = false;
        
        /* Execute the sub tasks */ 
        _DRV_SPI${DRV_INSTANCE}_ErrorTasks();
        
        /* Check to see if we have any data left to receive and update the 
         * bytes left. */
        if(currentJob->dataLeftToTx +currentJob->dummyLeftToTx != 0)
        {
            _DRV_SPI${DRV_INSTANCE}_MasterTransmit();
        }
        
        if(currentJob->dataLeftToRx + currentJob->dummyLeftToRx != 0)
        {
            _DRV_SPI${DRV_INSTANCE}_MasterReceive();
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
            
            if (dObj->operationEnded != NULL)
            {
                (*dObj->operationEnded)(DRV_SPI_BUFFER_EVENT_COMPLETE, (DRV_SPI_BUFFER_HANDLE)currentJob, currentJob->context);
            }
            
            /* Return the job back to the free queue*/
            if (DRV_SPI_SYS_QUEUE_FreeElement(dObj->queue, currentJob) != DRV_SPI_SYS_QUEUE_SUCCESS)
            {
                SYS_ASSERT(false, "\r\nSPI Driver: Queue free element error.");
                return 0;
            }
            
            /* Clean up */
            dObj->currentJob = NULL;
            if (!DRV_SPI_SYS_QUEUE_IsEmpty(dObj->queue))
            {
                continueLoop = true;    
                continue;
            }
            else
            {   
                /* Disable the SPI interrupts as SPI queue is empty now. */
                <#if DRV_ISR=="ISR">
                    if(true == dObj->txIntEnabled)
                    {
                        spiId->SPI_IDR.w = SPI_IDR_TDRE_Msk;    
                        dObj->txIntEnabled = false;
                    }
                    if(true == dObj->rxIntEnabled)
                    {
                        spiId->SPI_IDR.w = SPI_IDR_RDRF_Msk;    
                        dObj->rxIntEnabled = false;
                    }
                    
                    SYS_INT_SourceDisable(${SPI_INT});
                </#if>
                break;
            }
        }
        <#if DRV_ISR=="ISR">
        /* Check to see if the interrupts would fire again if so just go back 
         * into the loop instead of suffering the interrupt latency of exiting 
         * and re-entering*/
        if (dObj->currentJob != NULL)
        {
            /* Interrupts should immediately become active again if they're in 
             * a fired condition */
            if ( (spiId->SPI_SR.RDRF) || (spiId->SPI_SR.TDRE) )
            {
                /* Interrupt would fire again anyway so we should just go back 
                 * to the start*/
                continueLoop = true;
                continue;                            
            }
            /* If we're here then we know that the interrupt should not be 
             * firing again immediately, so re-enable them and exit*/
            SYS_INT_SourceEnable(${SPI_INT});

            return 0;            
        }
        </#if>
    } while(continueLoop);
    
    return 0;
}

int32_t _DRV_SPI${DRV_INSTANCE}_SlaveTasks(void)
{
    DRV_SPI_OBJ *dObj = (DRV_SPI_OBJ*)NULL;
    dObj = &gDrvSPI${DRV_INSTANCE}Obj;
    
    register DRV_SPI_JOB_OBJECT * currentJob = dObj->currentJob;
    
    volatile spi_registers_t *spiId = (volatile spi_registers_t *)${PLIB_INSTANCE};
    
    volatile bool continueLoop;
    uint32_t dummy;

    <#if DRV_ISR=="ISR">
    SYS_INT_SourceDisable(${SPI_INT});
    </#if>
    
    do {
        
        continueLoop = false;
    
        if (dObj->currentJob == NULL)
        {
            if (DRV_SPI_SYS_QUEUE_Dequeue(dObj->queue, (void *)&(dObj->currentJob)) != DRV_SPI_SYS_QUEUE_SUCCESS)
            {
                SYS_ASSERT(false, "\r\nSPI Driver: Error in dequeing.");
                return 0;       
            }
            
            if (dObj->currentJob == NULL)
            {
                return 0;
            }
            
            currentJob = dObj->currentJob;
            dObj->symbolsInProgress = 0;

            /* Call the operation starting function pointer.  
             * This can be used to modify the slave select lines */
            
            if (dObj->operationStarting != NULL)
            {
                (*dObj->operationStarting)(DRV_SPI_BUFFER_EVENT_PROCESSING, (DRV_SPI_BUFFER_HANDLE)currentJob, currentJob->context);
            }
            
            /* List the new job as processing*/
            currentJob->status = DRV_SPI_BUFFER_EVENT_PROCESSING;

            /* Flush out the Receive buffer */
            dummy = spiId->SPI_RDR.RD;
            (void)dummy;
        }
        
        /* Check for errors, if any */
        _DRV_SPI${DRV_INSTANCE}_ErrorTasks();
        
        if(currentJob->dataLeftToTx +currentJob->dummyLeftToTx != 0)
        {
            _DRV_SPI${DRV_INSTANCE}_SlaveTransmit();
        }
        
        if(currentJob->dataLeftToRx + currentJob->dummyLeftToRx != 0)
        {
            _DRV_SPI${DRV_INSTANCE}_SlaveReceive();
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
            
            if (dObj->operationEnded != NULL)
            {
                (*dObj->operationEnded)(DRV_SPI_BUFFER_EVENT_COMPLETE, (DRV_SPI_BUFFER_HANDLE)currentJob, currentJob->context);
            }
            
            /* Return the job back to the free queue*/
            if (DRV_SPI_SYS_QUEUE_FreeElementLock(dObj->queue, currentJob) != DRV_SPI_SYS_QUEUE_SUCCESS)
            {
                SYS_ASSERT(false, "\r\nSPI Driver: Queue free element error.");
                return 0;
            }
            
            /* Clean up */
            dObj->currentJob = NULL;
            
            if (!DRV_SPI_SYS_QUEUE_IsEmpty(dObj->queue))
            {
                continueLoop = true;    
                continue;
            }
            else
            {                
                /* Disable the SPI interrupts as SPI queue is empty now. */
                <#if DRV_ISR=="ISR">
                    if(true == dObj->txIntEnabled)
                    {
                        spiId->SPI_IDR.w = SPI_IDR_TDRE_Msk;   
                        dObj->txIntEnabled = false;
                    }
                    if(true == dObj->rxIntEnabled)
                    {
                        spiId->SPI_IDR.w = SPI_IDR_RDRF_Msk;   
                        dObj->rxIntEnabled = false;
                    }
        
                    SYS_INT_SourceDisable(${SPI_INT});
                </#if>
                break;
            }
        }
        <#if DRV_ISR=="ISR">
        /* Check to see if the interrupts would fire again if so just go back 
         * into the loop instead of suffering the interrupt latency of exiting 
         * and re-entering*/
        if (dObj->currentJob != NULL)
        {
            /* Interrupts should immediately become active again if they're 
             * in a fired condition */
            if ( (spiId->SPI_SR.RDRF) || (spiId->SPI_SR.TDRE) )
            {
                /* Interrupt would fire again anyway so we should just go 
                 * back to the start*/
                continueLoop = true;
                continue;                            
            }
            /* If we're here then we know that the interrupt should not be 
             * firing again immediately, so re-enable them and exit*/
            SYS_INT_SourceEnable(${SPI_INT});

            return 0;            
        }
        </#if>
    } while(continueLoop);
    
    return 0;
}
<#if .vars["CONFIG_DRV_SPI_USE_DMA_IDX${DRV_INSTANCE}"] == true>
// *****************************************************************************
/* Function:
    void _DRV_SPI${DRV_INSTANCE}_DMA_EventHandler
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

void _DRV_SPI${DRV_INSTANCE}_DMA_EventHandler
(
    SYS_DMA_TRANSFER_EVENT event,
    SYS_DMA_CHANNEL_HANDLE handle,
    uintptr_t contextHandle
)
{
    DRV_SPI_OBJ *dObj = (DRV_SPI_OBJ*)NULL;
    dObj = &gDrvSPI${DRV_INSTANCE}Obj;
    DRV_SPI_JOB_OBJECT *currentJob;

    if(SYS_DMA_CHANNEL_HANDLE_INVALID == handle || 0 == handle || 0 == contextHandle )
    {
        /* This means the handle is invalid */
        SYS_ASSERT(false, "Handle is invalid \r\n");
        return;
    }

    dObj = (DRV_SPI_OBJ *)contextHandle;
    currentJob = (DRV_SPI_JOB_OBJECT *)dObj->currentJob;

    switch(event)
    {
        case SYS_DMA_TRANSFER_EVENT_COMPLETE:
        {        
            if(handle == dObj->dmaChannelHandleWrite)
            {
                dObj->txDMAState = DRV_SPI_DMA_COMPLETE;
            }
            else if(handle == dObj->dmaChannelHandleRead)
            {
                dObj->rxDMAState = DRV_SPI_DMA_COMPLETE;
            }
            break;
        }
        
        case SYS_DMA_TRANSFER_EVENT_ABORT:
        case SYS_DMA_TRANSFER_EVENT_ERROR:
        {
            if(handle == dObj->dmaChannelHandleWrite)
            {
                dObj->txDMAState = DRV_SPI_DMA_ERROR;
            }
            else if(handle == dObj->dmaChannelHandleRead)
            {
                dObj->rxDMAState = DRV_SPI_DMA_ERROR;
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
    if( (DRV_SPI_DMA_COMPLETE == dObj->rxDMAState) && (DRV_SPI_DMA_COMPLETE == dObj->txDMAState) )
    {
        /* Find the next job in the queue and process it */
        _DRV_SPI${DRV_INSTANCE}_DMATasks( DRV_SPI_BUFFER_EVENT_COMPLETE);
    }
    else if( (DRV_SPI_DMA_ERROR == dObj->rxDMAState) || (DRV_SPI_DMA_ERROR == dObj->txDMAState))
    {
        _DRV_SPI${DRV_INSTANCE}_DMATasks( DRV_SPI_BUFFER_EVENT_ERROR);
    }
}

int32_t _DRV_SPI${DRV_INSTANCE}_DMATasks( DRV_SPI_BUFFER_EVENT status)
{
    DRV_SPI_OBJ *dObj = (DRV_SPI_OBJ*)NULL;
    dObj = &gDrvSPI${DRV_INSTANCE}Obj;
    
    DRV_SPI_JOB_OBJECT * currentJob = dObj->currentJob;
    volatile spi_registers_t *spiId = (volatile spi_registers_t *)${PLIB_INSTANCE};
    
    /* Check to see if the job is done.
     * Master task is done when data is completely received from the 
     * shift register to complete the ring. 
     * So, check RX data and dummy count. */

    if (dObj->currentJob == NULL)
    {
        if (DRV_SPI_SYS_QUEUE_Dequeue(dObj->queue, (void *)&(dObj->currentJob)) != DRV_SPI_SYS_QUEUE_SUCCESS)
        {
            SYS_ASSERT(false, "\r\nSPI Driver: Error in dequeing.");
            return 0;       
        }
        if (dObj->currentJob == NULL)
        {
            return 0;
        }
    }
    
    /* Check for a current task */
    currentJob = dObj->currentJob;

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

    if (dObj->operationEnded != NULL)
    {
        (*dObj->operationEnded)(DRV_SPI_BUFFER_EVENT_COMPLETE, (DRV_SPI_BUFFER_HANDLE)currentJob, currentJob->context);
    }

    /* Return the job back to the free queue*/
    if (DRV_SPI_SYS_QUEUE_FreeElement(dObj->queue, currentJob) != DRV_SPI_SYS_QUEUE_SUCCESS)
    {
        SYS_ASSERT(false, "\r\nSPI Driver: Queue free element error.");
        return 0;
    }

    /* Clean up */
    dObj->currentJob = NULL;

    if ( DRV_SPI_SYS_QUEUE_IsEmpty(dObj->queue) )
    { 
        /* Disable the DMA interrupts as SPI queue is empty now. */
        SYS_INT_SourceDisable(XDMAC_IRQn);
        
        /* Update the DMA state to NONE as DMA is done with its job for now and is ready to take new jobs */
        dObj->rxDMAState = DRV_SPI_DMA_NONE;
        dObj->txDMAState = DRV_SPI_DMA_NONE;
    }
    else
    {
        /* Get the next job from the queue */
        if (DRV_SPI_SYS_QUEUE_Dequeue(dObj->queue, (void *)&(dObj->currentJob)) != DRV_SPI_SYS_QUEUE_SUCCESS)
        {
            SYS_ASSERT(false, "\r\nSPI Driver: Error in dequeing.");
            return 0;       
        }
        if (dObj->currentJob == NULL)
        {
            return 0;
        }
        currentJob = dObj->currentJob;

        /* Call the operation starting function pointer.  
         * This can be used to modify the slave select lines */
        if (dObj->operationStarting != NULL)
        {
            (*dObj->operationStarting)(DRV_SPI_BUFFER_EVENT_PROCESSING, (DRV_SPI_BUFFER_HANDLE)currentJob, currentJob->context);
        }

        /* List the new job as processing*/
        currentJob->status = DRV_SPI_BUFFER_EVENT_PROCESSING;

        /* Check to see if we have any data left to recieve and update the 
         * bytes left. */
        if (currentJob->dataLeftToRx != 0)
        {
            /* Add data read transfer */
            dObj->rxDMAState = DRV_SPI_DMA_DATA_INPROGRESS;
            SYS_DMA_ChannelTransferAdd(dObj->dmaChannelHandleRead,
                    (const void*)&spiId->SPI_RDR, spiSize, 
                    currentJob->rxBuffer, currentJob->dataLeftToRx, spiSize);
        }
        else if ( (currentJob->dummyLeftToRx != 0))
        {
            /* Add dummy read transfer */
            dObj->rxDMAState = DRV_SPI_DMA_DUMMY_INPROGRESS;
            SYS_DMA_ChannelTransferAdd(dObj->dmaChannelHandleRead,
                    (const void*)&spiId->SPI_RDR, spiSize, 
                    sDrvSpi${DRV_INSTANCE}RxDummy, currentJob->dummyLeftToRx, spiSize);
        }

        /* Check to see if we have any data left to transmit and update the 
         * bytes left. */
        if (currentJob->dataLeftToTx != 0)
        {
            /* Add data write transfer */
            dObj->txDMAState = DRV_SPI_DMA_DATA_INPROGRESS;
            SYS_DMA_ChannelTransferAdd(dObj->dmaChannelHandleWrite, 
                    currentJob->txBuffer, currentJob->dataLeftToTx, (const void*)&spiId->SPI_TDR, spiSize, spiSize);
        }
        else if ( (currentJob->dummyLeftToTx != 0))
        {
            /* Add dummy write transfer */
            dObj->txDMAState = DRV_SPI_DMA_DUMMY_INPROGRESS;
            SYS_DMA_ChannelTransferAdd(dObj->dmaChannelHandleWrite, 
                    sDrvSpi${DRV_INSTANCE}TxDummy, currentJob->dummyLeftToTx, (const void*)&spiId->SPI_TDR, spiSize, spiSize);
        }
    }
    
    return 0;
}
</#if>
</#macro>


<#if (CONFIG_DRV_SPI_IDX0 == true)>
<@DRV_SPI_STATIC_FUNCTIONS DRV_INSTANCE="0"
PLIB_INSTANCE=CONFIG_DRV_SPI_SPI_ID_IDX0
CLOCK_SOURCE=CONFIG_DRV_SPI_CLOCK_SOURCE_IDX0
CLOCK_MODE=CONFIG_DRV_SPI_CLOCK_MODE_IDX0
BAUD_RATE=CONFIG_DRV_SPI_BAUD_RATE_IDX0
SPI_INT=CONFIG_DRV_SPI_INTERRUPT_VECTOR_IDX0
QUEUE_SIZE=CONFIG_DRV_SPI_QUEUE_SIZE_IDX0
TRANS_PER_SM=CONFIG_DRV_SPI_TRANSACTIONS_PER_SM_RUN_IDX0
USE_DMA=CONFIG_DRV_SPI_USE_DMA_IDX0
DUMMY_BYTE=CONFIG_DRV_SPI_TRANSMIT_DUMMY_BYTE_VALUE_IDX0
DMA_DUMMY_BUFFER_SIZE=CONFIG_DRV_SPI_DMA_DUMMY_BUFFER_SIZE_IDX0
/>
</#if>
<#if (CONFIG_DRV_SPI_IDX1 == true)>
<@DRV_SPI_STATIC_FUNCTIONS DRV_INSTANCE="1"
PLIB_INSTANCE=CONFIG_DRV_SPI_SPI_ID_IDX1
CLOCK_SOURCE=CONFIG_DRV_SPI_CLOCK_SOURCE_IDX1
CLOCK_MODE=CONFIG_DRV_SPI_CLOCK_MODE_IDX1
BAUD_RATE=CONFIG_DRV_SPI_BAUD_RATE_IDX1
SPI_INT=CONFIG_DRV_SPI_INTERRUPT_VECTOR_IDX1
QUEUE_SIZE=CONFIG_DRV_SPI_QUEUE_SIZE_IDX1
TRANS_PER_SM=CONFIG_DRV_SPI_TRANSACTIONS_PER_SM_RUN_IDX1
USE_DMA=CONFIG_DRV_SPI_USE_DMA_IDX1
DUMMY_BYTE=CONFIG_DRV_SPI_TRANSMIT_DUMMY_BYTE_VALUE_IDX1
DMA_DUMMY_BUFFER_SIZE=CONFIG_DRV_SPI_DMA_DUMMY_BUFFER_SIZE_IDX1
/>
</#if>