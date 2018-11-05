/*******************************************************************************
  SPI Driver Static implementation

  Company:
    Microchip Technology Inc.

  File Name:
    drv_spi_static.c

  Summary:
    Source code for the SPI driver static implementation.

  Description:
    The SPI device driver provides a simple interface to manage the SPI
    modules on Microchip microcontrollers. This file contains static implementation
    for the SPI driver.

  Remarks:
    Static interfaces incorporate the driver instance number within the names
    of the routines, eliminating the need for an object ID or object handle.

    Static single-open interfaces also eliminate the need for the open handle.
*******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2015 released Microchip Technology Inc.  All rights reserved.

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

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <string.h>
#include "system_config.h"
#include "system_definitions.h"
#include "driver/spi/static/src/drv_spi_static_local.h"
// *****************************************************************************
// *****************************************************************************
// Section: Global Data
// *****************************************************************************
// *****************************************************************************
static uint8_t sDrvSPIQueueArea[DRV_SPI_SYS_QUEUE_BUFFER_SIZE(DRV_SPI_INSTANCES_NUMBER, sizeof(DRV_SPI_JOB_OBJECT), DRV_SPI_INSTANCES_NUMBER * DRV_SPI_ELEMENTS_PER_QUEUE)];

/* This is the Queue manager object . */
static DRV_SPI_SYS_QUEUE_MANAGER_SETUP qmInitData = {
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

/* This is the Queue setup object . */
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

/* This is the Queue Manager handle*/
DRV_SPI_SYS_QUEUE_MANAGER_HANDLE  hQueueManager;

<#macro DRV_SPI_STATIC_FUNCTIONS DRV_INSTANCE PLIB_INSTANCE CLOCK_SOURCE CLOCK_MODE INPUT_PHASE BAUD_RATE XMIT_INT RCV_INT ERR_INT USE_SS
QUEUE_SIZE RESERVED_JOB TRANS_PER_SM FRAME_SYNC_PULSE FRAME_PULSE_POLARITY FRAME_PULSE_DIRECTION FRAME_PULSE_EDGE FRAME_PULSE_WIDTH
AUDIO_TRANSMIT_MODE AUDIO_PROTOCOL_MODE USE_DMA TX_DMA_THRESHOLD RX_DMA_THRESHOLD>
<#assign DRV_MODE="Master">
<#assign DRV_ISR="ISR">
<#assign DRV_BUFFER="RM">
<#assign DRV_BIT_WIDTH="8">
<#assign SIDL_MODE="false">
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
<#if .vars["CONFIG_DRV_SPI_ALLOW_IDLE_RUN_IDX${DRV_INSTANCE}"] == true>
    <#assign SIDL_MODE="true">
</#if>
<#if .vars["CONFIG_DRV_SPI_SPI_PROTOCOL_TYPE_IDX${DRV_INSTANCE}"] == "DRV_SPI_PROTOCOL_TYPE_FRAMED">
    <#assign PROTOCOL_TYPE		    = "DRV_SPI_PROTOCOL_TYPE_FRAMED">
<#elseif .vars["CONFIG_DRV_SPI_SPI_PROTOCOL_TYPE_IDX${DRV_INSTANCE}"] == "DRV_SPI_PROTOCOL_TYPE_AUDIO">
    <#assign PROTOCOL_TYPE		    = "DRV_SPI_PROTOCOL_TYPE_AUDIO">
<#else>
    <#assign PROTOCOL_TYPE 		    = "DRV_SPI_PROTOCOL_TYPE_STANDARD">
</#if>

// *****************************************************************************
// *****************************************************************************
// Section: Instance ${DRV_INSTANCE} static driver functions
// *****************************************************************************
// *****************************************************************************
/* This is the driver static object . */
DRV_SPI_OBJ  gDrvSPI${DRV_INSTANCE}Obj ;

SYS_MODULE_OBJ DRV_SPI${DRV_INSTANCE}_Initialize(void)
{
    DRV_SPI_OBJ *dObj = (DRV_SPI_OBJ*)NULL;

    dObj = &gDrvSPI${DRV_INSTANCE}Obj;

    /* Disable the SPI module to configure it*/
    PLIB_SPI_Disable ( ${PLIB_INSTANCE} );

    /* Set up Master or Slave Mode*/
<#if DRV_MODE=="Master">
    PLIB_SPI_MasterEnable ( ${PLIB_INSTANCE} );
    PLIB_SPI_PinDisable(${PLIB_INSTANCE}, SPI_PIN_SLAVE_SELECT);
</#if>
<#if DRV_MODE=="Slave">
    PLIB_SPI_SlaveEnable ( ${PLIB_INSTANCE} );
    <#if USE_SS==true || PROTOCOL_TYPE == "DRV_SPI_PROTOCOL_TYPE_FRAMED">
    PLIB_SPI_PinEnable(${PLIB_INSTANCE}, SPI_PIN_SLAVE_SELECT);
    </#if>
</#if>

    /* Set up if the SPI is allowed to run while the rest of the CPU is in idle mode*/
<#if SIDL_MODE == "true">
    PLIB_SPI_StopInIdleDisable( ${PLIB_INSTANCE} );
<#else>
    PLIB_SPI_StopInIdleEnable( ${PLIB_INSTANCE} );
</#if>

    /* Set up clock Polarity and output data phase*/
<#if CLOCK_MODE == "DRV_SPI_CLOCK_MODE_IDLE_LOW_EDGE_RISE">
    PLIB_SPI_ClockPolaritySelect( ${PLIB_INSTANCE}, SPI_CLOCK_POLARITY_IDLE_LOW );
    PLIB_SPI_OutputDataPhaseSelect( ${PLIB_INSTANCE}, SPI_OUTPUT_DATA_PHASE_ON_IDLE_TO_ACTIVE_CLOCK );
</#if>
<#if CLOCK_MODE == "DRV_SPI_CLOCK_MODE_IDLE_LOW_EDGE_FALL">
    PLIB_SPI_ClockPolaritySelect( ${PLIB_INSTANCE}, SPI_CLOCK_POLARITY_IDLE_LOW );
    PLIB_SPI_OutputDataPhaseSelect( ${PLIB_INSTANCE}, SPI_OUTPUT_DATA_PHASE_ON_ACTIVE_TO_IDLE_CLOCK );
</#if>
<#if CLOCK_MODE == "DRV_SPI_CLOCK_MODE_IDLE_HIGH_EDGE_FALL">
    PLIB_SPI_ClockPolaritySelect( ${PLIB_INSTANCE}, SPI_CLOCK_POLARITY_IDLE_HIGH );
    PLIB_SPI_OutputDataPhaseSelect( ${PLIB_INSTANCE}, SPI_OUTPUT_DATA_PHASE_ON_IDLE_TO_ACTIVE_CLOCK );
</#if>
<#if CLOCK_MODE == "DRV_SPI_CLOCK_MODE_IDLE_HIGH_EDGE_RISE">
    PLIB_SPI_ClockPolaritySelect( ${PLIB_INSTANCE}, SPI_CLOCK_POLARITY_IDLE_HIGH );
    PLIB_SPI_OutputDataPhaseSelect( ${PLIB_INSTANCE}, SPI_OUTPUT_DATA_PHASE_ON_ACTIVE_TO_IDLE_CLOCK );
</#if>

    /* Set up the Input Sample Phase*/
    PLIB_SPI_InputSamplePhaseSelect ( ${PLIB_INSTANCE}, ${INPUT_PHASE});

    /* Communication Width Selection */
    PLIB_SPI_CommunicationWidthSelect ( ${PLIB_INSTANCE}, SPI_COMMUNICATION_WIDTH_${DRV_BIT_WIDTH}BITS );

    /* Baud rate selection */
    PLIB_SPI_BaudRateSet( ${PLIB_INSTANCE} , SYS_CLK_PeripheralFrequencyGet(${CLOCK_SOURCE}), ${BAUD_RATE} );

    /* Protocol selection */
<#if PROTOCOL_TYPE == "DRV_SPI_PROTOCOL_TYPE_STANDARD">
    PLIB_SPI_FramedCommunicationDisable( ${PLIB_INSTANCE}  );
    #if defined (PLIB_SPI_ExistsAudioProtocolControl)
            if (PLIB_SPI_ExistsAudioProtocolControl(${PLIB_INSTANCE}))
            {
                PLIB_SPI_AudioProtocolDisable(${PLIB_INSTANCE});
            }
    #endif
</#if>
<#if PROTOCOL_TYPE == "DRV_SPI_PROTOCOL_TYPE_FRAMED">
    PLIB_SPI_FrameSyncPulseDirectionSelect(${PLIB_INSTANCE}, ${FRAME_PULSE_DIRECTION});
    PLIB_SPI_FrameSyncPulsePolaritySelect(${PLIB_INSTANCE}, ${FRAME_PULSE_POLARITY});
    PLIB_SPI_FrameSyncPulseEdgeSelect(${PLIB_INSTANCE}, ${FRAME_PULSE_EDGE});
    PLIB_SPI_FrameSyncPulseWidthSelect(${PLIB_INSTANCE}, ${FRAME_PULSE_WIDTH});
    PLIB_SPI_FrameSyncPulseCounterSelect(${PLIB_INSTANCE}, ${FRAME_SYNC_PULSE});
    #if defined (PLIB_SPI_ExistsAudioProtocolControl)
        if (PLIB_SPI_ExistsAudioProtocolControl(${PLIB_INSTANCE}))
        {
            PLIB_SPI_AudioProtocolDisable(${PLIB_INSTANCE});
        }
    #endif
    PLIB_SPI_FramedCommunicationEnable(${PLIB_INSTANCE});
</#if>
<#if PROTOCOL_TYPE == "DRV_SPI_PROTOCOL_TYPE_AUDIO">
    PLIB_SPI_FramedCommunicationDisable( ${PLIB_INSTANCE}  );
    #if defined (PLIB_SPI_ExistsAudioProtocolControl)
        if (PLIB_SPI_ExistsAudioProtocolControl(${PLIB_INSTANCE}))
        {
            PLIB_SPI_AudioTransmitModeSelect(${PLIB_INSTANCE}, ${AUDIO_TRANSMIT_MODE});
            PLIB_SPI_AudioProtocolModeSelect(${PLIB_INSTANCE}, ${AUDIO_PROTOCOL_MODE});
            PLIB_SPI_AudioProtocolEnable(${PLIB_INSTANCE});
        }
    #else
        {
            SYS_ASSERT(false, "\r\nInvalid SPI Configuration.");
            return SYS_MODULE_OBJ_INVALID;
        }
    #endif
</#if>

    /* Buffer type selection */
<#if DRV_BUFFER=="RM">
    #if defined (PLIB_SPI_ExistsFIFOControl)
        if (PLIB_SPI_ExistsFIFOControl( ${PLIB_INSTANCE} ))
        {
            PLIB_SPI_FIFODisable( ${PLIB_INSTANCE} );
        }
    #endif
</#if>
<#if DRV_BUFFER=="EBM">
    #if defined (PLIB_SPI_ExistsFIFOControl)
        if (PLIB_SPI_ExistsFIFOControl( ${PLIB_INSTANCE} ))
        {
            PLIB_SPI_FIFOEnable( ${PLIB_INSTANCE} );
            PLIB_SPI_FIFOInterruptModeSelect(${PLIB_INSTANCE}, SPI_FIFO_INTERRUPT_WHEN_TRANSMIT_BUFFER_IS_COMPLETELY_EMPTY);
            PLIB_SPI_FIFOInterruptModeSelect(${PLIB_INSTANCE}, SPI_FIFO_INTERRUPT_WHEN_RECEIVE_BUFFER_IS_NOT_EMPTY);
        }
    #else
        {
            SYS_ASSERT(false, "\r\nInvalid SPI Configuration.");
            return SYS_MODULE_OBJ_INVALID;
        }
    #endif
</#if>

    PLIB_SPI_BufferClear( ${PLIB_INSTANCE} );
    PLIB_SPI_ReceiverOverflowClear ( ${PLIB_INSTANCE} );
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
    qInitData.reserveElements = ${RESERVED_JOB}; //Mininmum number of job queues reserved

    /* Create Queue for this instance of SPI */
    if (DRV_SPI_SYS_QUEUE_CreateQueue(hQueueManager, &qInitData, &dObj->queue) != DRV_SPI_SYS_QUEUE_SUCCESS)
    {
        SYS_ASSERT(false, "\r\nSPI Driver: Could not set up driver instance queue.");
        return SYS_MODULE_OBJ_INVALID;

    }

    /* Update the SPI OBJECT parameters. */
<#if USE_DMA == true>
    dObj->sendDMAHander = DRV_SPI${DRV_INSTANCE}_${DRV_ISR}DMA${DRV_MODE}SendEventHandler${DRV_BIT_WIDTH}bit;
    dObj->receiveDMAHander = DRV_SPI${DRV_INSTANCE}_${DRV_ISR}DMA${DRV_MODE}ReceiveEventHandler${DRV_BIT_WIDTH}bit;
</#if>
<#if CONFIG_DRV_SPI_USE_CLIENT_CONFIG_FUCNTION == true>
    dObj->operationStarting = NULL;
    dObj->operationEnded = NULL;
</#if>
<#if DRV_BUFFER == "RM" && DRV_ISR == "Polled">
    dObj->numTrfsSmPolled = ${TRANS_PER_SM};
</#if>
<#if USE_DMA == true>
    dObj->txDmaThreshold = ${TX_DMA_THRESHOLD};
    dObj->txDmaChannelHandle = SYS_DMA_CHANNEL_HANDLE_INVALID;
    dObj->rxDmaThreshold = ${RX_DMA_THRESHOLD};
    dObj->rxDmaChannelHandle = SYS_DMA_CHANNEL_HANDLE_INVALID;
</#if>
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
<#if DRV_ISR=="ISR">

    SYS_INT_SourceDisable(${XMIT_INT});
    SYS_INT_SourceDisable(${RCV_INT});
    SYS_INT_SourceEnable(${ERR_INT});

    /* Clear all interrupt sources */
    SYS_INT_SourceStatusClear(${XMIT_INT});
    SYS_INT_SourceStatusClear(${RCV_INT});
    SYS_INT_SourceStatusClear(${ERR_INT});
</#if>

    /* Enable the Module */
    PLIB_SPI_Enable(${PLIB_INSTANCE});

    return (SYS_MODULE_OBJ)DRV_SPI_INDEX_${DRV_INSTANCE} ;
}

void DRV_SPI${DRV_INSTANCE}_Deinitialize ( void )
{
<#if CONFIG_USE_3RDPARTY_RTOS ==true>
    DRV_SPI_OBJ *dObj = (DRV_SPI_OBJ*)NULL;

    dObj = &gDrvSPI${DRV_INSTANCE}Obj;

    /* Deallocate all mutexes */
    if(OSAL_MUTEX_Delete(&(dObj->mutexDriverInstance)) != OSAL_RESULT_TRUE)
    {
        SYS_DEBUG(SYS_ERROR_ERROR, "Mutex Delete Failed");
        return;
    }

    /* Delete all Semaphores */
    if (OSAL_SEM_Delete(&(dObj->semaphoreDriverInstance)) != OSAL_RESULT_TRUE) {/*Report Error*/}

</#if>
<#if DRV_ISR=="ISR">
    /* Disable the interrupts */
    SYS_INT_SourceDisable(${XMIT_INT});
    SYS_INT_SourceDisable(${RCV_INT});
    SYS_INT_SourceDisable(${ERR_INT});

</#if>
    /* Disable the SPI Module */
    PLIB_SPI_Disable(${PLIB_INSTANCE});

    return;
}

SYS_STATUS DRV_SPI${DRV_INSTANCE}_Status ( void )
{
    /* Return the current status of driver instance */
    return SYS_STATUS_READY;
}

void DRV_SPI${DRV_INSTANCE}_Tasks ( void )
{
    /* Call the respective task routine */
    DRV_SPI${DRV_INSTANCE}_${DRV_ISR}${DRV_MODE}${DRV_BUFFER}${DRV_BIT_WIDTH}BitTasks(&gDrvSPI${DRV_INSTANCE}Obj);
}

DRV_HANDLE DRV_SPI${DRV_INSTANCE}_Open ( const SYS_MODULE_INDEX index, const DRV_IO_INTENT intent )
{
<#if CONFIG_USE_3RDPARTY_RTOS ==true || USE_DMA == true>

    DRV_SPI_OBJ *dObj = (DRV_SPI_OBJ*)NULL;

    dObj = &gDrvSPI${DRV_INSTANCE}Obj;

</#if>
<#if CONFIG_USE_3RDPARTY_RTOS ==true>

    if (OSAL_SEM_Pend(&(dObj->semaphoreDriverInstance), OSAL_WAIT_FOREVER) != OSAL_RESULT_TRUE)
    {
        /* Report Error*/
    }
</#if>
<#if USE_DMA == true>

        DRV_SPI${DRV_INSTANCE}_SetupDMA(dObj);

</#if>
<#if CONFIG_USE_3RDPARTY_RTOS ==true>

    if (OSAL_SEM_Post(&(dObj->semaphoreDriverInstance)) != OSAL_RESULT_TRUE)
    {
        /* Report Error*/
    }

</#if>
    return (DRV_HANDLE)DRV_SPI_INDEX_${DRV_INSTANCE};
}

void DRV_SPI${DRV_INSTANCE}_Close ( void )
{
<#if CONFIG_USE_3RDPARTY_RTOS ==true>
    DRV_SPI_OBJ *dObj = (DRV_SPI_OBJ*)NULL;

    dObj = &gDrvSPI${DRV_INSTANCE}Obj;

    if (OSAL_SEM_Pend(&dObj->semaphoreDriverInstance, OSAL_WAIT_FOREVER) != OSAL_RESULT_TRUE)
    {
        /* Report Error*/
    }

    if (OSAL_SEM_Post(&dObj->semaphoreDriverInstance) != OSAL_RESULT_TRUE)
    {
        /* Report Error*/
    }

</#if>
    return;
}
<#if CONFIG_DRV_SPI_USE_CLIENT_CONFIG_FUCNTION == true>

int32_t DRV_SPI${DRV_INSTANCE}_ClientConfigure ( const DRV_SPI_CLIENT_DATA * cfgData  )
{
    DRV_SPI_OBJ *dObj = (DRV_SPI_OBJ*)NULL;

    dObj = &gDrvSPI${DRV_INSTANCE}Obj;

    if (cfgData == NULL)
    {
        /* Nothing to do */
        return 0;
    }

    if (cfgData->operationStarting != NULL)
    {
        dObj->operationStarting = cfgData->operationStarting;
    }

    if (cfgData->operationEnded != NULL)
    {
        dObj->operationEnded = cfgData->operationEnded;
    }

    if (cfgData->baudRate != 0)
    {

        PLIB_SPI_BaudRateSet (${PLIB_INSTANCE},
                              SYS_CLK_PeripheralFrequencyGet(${CLOCK_SOURCE}),
                              cfgData->baudRate);
    }

    return 0;
}
</#if>

DRV_SPI_BUFFER_HANDLE DRV_SPI${DRV_INSTANCE}_BufferAddRead2 ( void *rxBuffer, size_t size, DRV_SPI_BUFFER_EVENT_HANDLER completeCB, void * context, DRV_SPI_BUFFER_HANDLE * jobHandle)
{
    DRV_SPI_OBJ *dObj = (DRV_SPI_OBJ*)NULL;

    dObj = &gDrvSPI${DRV_INSTANCE}Obj;
<#if DRV_BIT_WIDTH == "16">

    /* Check if the requested size of data can be processed with the configured Data Width */
    if ((size & 0x1) != 0)
    {
        SYS_ASSERT(false, "\r\nSPI Driver: invalid input size, must be a multiple of 16 bits.");
        return (DRV_SPI_BUFFER_HANDLE)NULL;

    }
</#if>
<#if DRV_BIT_WIDTH == "32">

    /* Check if the requested size of data can be processed with the configured Data Width */
    if ((size & 0x3) != 0)
    {
        SYS_ASSERT(false, "\r\nSPI Driver: invalid input size, must be a multiple of 32 bits.");
        return (DRV_SPI_BUFFER_HANDLE)NULL;
    }
</#if>

    DRV_SPI_JOB_OBJECT * pJob = NULL;

    if (DRV_SPI_SYS_QUEUE_AllocElementLock(dObj->queue, (void **)&pJob) != DRV_SPI_SYS_QUEUE_SUCCESS)
    {
        SYS_ASSERT(false, "\r\nSPI Driver: Error trying to get a free entry.");
        return (DRV_SPI_BUFFER_HANDLE)NULL;
    }

    memset(pJob, 0, sizeof(DRV_SPI_JOB_OBJECT));

    pJob->rxBuffer = rxBuffer;
    pJob->dataLeftToRx = size;
<#if DRV_MODE=="Master">
    pJob->dummyLeftToTx = size;
</#if>
    pJob->completeCB = completeCB;
    pJob->context = context;
    pJob->status = DRV_SPI_BUFFER_EVENT_PENDING;

    if (jobHandle != NULL )
    {
        *jobHandle = (DRV_SPI_BUFFER_HANDLE)pJob;
    }

    if (DRV_SPI_SYS_QUEUE_EnqueueLock(dObj->queue, (void*)pJob) != DRV_SPI_SYS_QUEUE_SUCCESS)
    {
        SYS_ASSERT(false, "\r\nSPI Driver: Error enqueing new job.");
        return (DRV_SPI_BUFFER_HANDLE)NULL;
    }

<#if DRV_ISR=="ISR">
    dObj->txEnabled = true;
    SYS_INT_SourceEnable(${XMIT_INT});

</#if>
    return (DRV_SPI_BUFFER_HANDLE)pJob;
}


DRV_SPI_BUFFER_HANDLE DRV_SPI${DRV_INSTANCE}_BufferAddWrite2 ( void *txBuffer, size_t size, DRV_SPI_BUFFER_EVENT_HANDLER completeCB, void * context, DRV_SPI_BUFFER_HANDLE * jobHandle )
{
    DRV_SPI_OBJ *dObj = (DRV_SPI_OBJ*)NULL;

    dObj = &gDrvSPI${DRV_INSTANCE}Obj;
<#if DRV_BIT_WIDTH == "16">

    /* Check if the requested size of data can be processed with the configured Data Width */
    if ((size & 0x1) != 0)
    {
        SYS_ASSERT(false, "\r\nSPI Driver: invalid input size, must be a multiple of 16 bits.");
        return (DRV_SPI_BUFFER_HANDLE)NULL;

    }
</#if>
<#if DRV_BIT_WIDTH == "32">

    /* Check if the requested size of data can be processed with the configured Data Width */
    if ((size & 0x3) != 0)
    {
        SYS_ASSERT(false, "\r\nSPI Driver: invalid input size, must be a multiple of 32 bits.");
        return (DRV_SPI_BUFFER_HANDLE)NULL;
    }
</#if>

    DRV_SPI_JOB_OBJECT * pJob = NULL;
    if (DRV_SPI_SYS_QUEUE_AllocElementLock(dObj->queue, (void **)&pJob) != DRV_SPI_SYS_QUEUE_SUCCESS)
    {
        SYS_ASSERT(false, "\r\nSPI Driver: Error trying to get a free entry.");
        return (DRV_SPI_BUFFER_HANDLE)NULL;
    }

    memset(pJob, 0, sizeof(DRV_SPI_JOB_OBJECT));
    pJob->txBuffer = txBuffer;
    pJob->dataLeftToTx = size;
    pJob->dummyLeftToRx = size;
    pJob->completeCB = completeCB;
    pJob->context = context;
    pJob->status = DRV_SPI_BUFFER_EVENT_PENDING;

    if (jobHandle != NULL )
    {
        *jobHandle = (DRV_SPI_BUFFER_HANDLE)pJob;
    }

    if (DRV_SPI_SYS_QUEUE_EnqueueLock(dObj->queue, (void*)pJob) != DRV_SPI_SYS_QUEUE_SUCCESS)
    {
        SYS_ASSERT(false, "\r\nSPI Driver: Error enqueing new job.");
        return (DRV_SPI_BUFFER_HANDLE)NULL;
    }

<#if DRV_ISR=="ISR">
    dObj->txEnabled = true;
    SYS_INT_SourceEnable(${XMIT_INT});

</#if>
    return (DRV_SPI_BUFFER_HANDLE)pJob;
}


DRV_SPI_BUFFER_HANDLE DRV_SPI${DRV_INSTANCE}_BufferAddWriteRead2 ( void *txBuffer, size_t txSize, void *rxBuffer, size_t rxSize, DRV_SPI_BUFFER_EVENT_HANDLER completeCB, void * context, DRV_SPI_BUFFER_HANDLE * jobHandle )
{
    DRV_SPI_OBJ *dObj = (DRV_SPI_OBJ*)NULL;

    dObj = &gDrvSPI${DRV_INSTANCE}Obj;
<#if DRV_BIT_WIDTH == "16">

    /* Check if the requested size of data can be processed with the configured Data Width */
    if (((rxSize & 0x1) != 0) || ((txSize & 0x1) != 0))
    {
        SYS_ASSERT(false, "\r\nSPI Driver: invalid input size, must be a multiple of 16 bits.");
        return (DRV_SPI_BUFFER_HANDLE)NULL;

    }
</#if>
<#if DRV_BIT_WIDTH == "32">

    /* Check if the requested size of data can be processed with the configured Data Width */
    if (((rxSize & 0x3) != 0) || ((txSize & 0x3) != 0))
    {
        SYS_ASSERT(false, "\r\nSPI Driver: invalid input size, must be a multiple of 32 bits.");
        return (DRV_SPI_BUFFER_HANDLE)NULL;
    }
</#if>

    DRV_SPI_JOB_OBJECT * pJob = NULL;
    if (DRV_SPI_SYS_QUEUE_AllocElementLock(dObj->queue, (void **)&pJob) != DRV_SPI_SYS_QUEUE_SUCCESS)
    {
        SYS_ASSERT(false, "\r\nSPI Driver: Error trying to get a free entry.");
        return (DRV_SPI_BUFFER_HANDLE)NULL;
    }

    memset(pJob, 0, sizeof(DRV_SPI_JOB_OBJECT));
    pJob->txBuffer = txBuffer;
    pJob->dataLeftToTx = txSize;
    pJob->rxBuffer = rxBuffer;
    pJob->dataLeftToRx = rxSize;
<#if CONFIG_DRV_SPI_USE_BLOCKING>

    (void)jobHandle;
<#else>

    if (jobHandle != NULL )
    {
        *jobHandle = (DRV_SPI_BUFFER_HANDLE)pJob;
    }
</#if>
<#if DRV_MODE=="Master">
    if (rxSize > txSize)
    {
        pJob->dummyLeftToTx = rxSize - txSize;
    }
</#if>
    if (txSize > rxSize)
    {
        pJob->dummyLeftToRx = txSize - rxSize;
    }
    pJob->completeCB = completeCB;
    pJob->context = context;
    pJob->status = DRV_SPI_BUFFER_EVENT_PENDING;

    if (DRV_SPI_SYS_QUEUE_EnqueueLock(dObj->queue, (void*)pJob) != DRV_SPI_SYS_QUEUE_SUCCESS)
    {
        SYS_ASSERT(false, "\r\nSPI Driver: Error enqueing new job.");
        return (DRV_SPI_BUFFER_HANDLE)NULL;
    }

<#if DRV_ISR=="ISR">
    dObj->txEnabled = true;
    SYS_INT_SourceEnable(${XMIT_INT});

</#if>
    return (DRV_SPI_BUFFER_HANDLE)pJob;
}


DRV_SPI_BUFFER_HANDLE DRV_SPI${DRV_INSTANCE}_BufferAddRead ( void *rxBuffer, size_t size, DRV_SPI_BUFFER_EVENT_HANDLER completeCB, void * context)
{
    return DRV_SPI${DRV_INSTANCE}_BufferAddRead2(rxBuffer, size, completeCB, context, NULL);
}

DRV_SPI_BUFFER_HANDLE DRV_SPI${DRV_INSTANCE}_BufferAddWrite ( void *txBuffer, size_t size, DRV_SPI_BUFFER_EVENT_HANDLER completeCB, void * context )
{
    return DRV_SPI${DRV_INSTANCE}_BufferAddWrite2(txBuffer, size, completeCB, context, NULL);
}

DRV_SPI_BUFFER_EVENT DRV_SPI${DRV_INSTANCE}_BufferStatus ( DRV_SPI_BUFFER_HANDLE bufferHandle )
{
    DRV_SPI_JOB_OBJECT * pJob = (DRV_SPI_JOB_OBJECT *)bufferHandle;

    return pJob->status;
}


<#if DRV_ISR=="Polled">
int32_t DRV_SPI${DRV_INSTANCE}_PolledErrorTasks(struct DRV_SPI_OBJ * dObj)
{

    if (dObj->currentJob == NULL)
    {
        return 0;
    }

    register DRV_SPI_JOB_OBJECT * currentJob = dObj->currentJob;

    if (PLIB_SPI_ReceiverHasOverflowed(${PLIB_INSTANCE}))
    {
        currentJob->status = DRV_SPI_BUFFER_EVENT_ERROR;
        if (currentJob->completeCB != NULL)
        {
            (*currentJob->completeCB)(DRV_SPI_BUFFER_EVENT_ERROR, (DRV_SPI_BUFFER_HANDLE)currentJob, currentJob->context);
        }
<#if CONFIG_DRV_SPI_USE_CLIENT_CONFIG_FUCNTION == true>
        if (dObj->operationEnded != NULL)
        {
            (*dObj->operationEnded)(DRV_SPI_BUFFER_EVENT_ERROR, (DRV_SPI_BUFFER_HANDLE)currentJob, currentJob->context);
        }
</#if>
        if (DRV_SPI_SYS_QUEUE_FreeElementLock(dObj->queue, currentJob) != DRV_SPI_SYS_QUEUE_SUCCESS)
        {
            SYS_ASSERT(false, "\r\nSPI Driver: Queue free element error.");
            return 0;
        }
        dObj->currentJob = NULL;
        PLIB_SPI_BufferClear(${PLIB_INSTANCE} );
        PLIB_SPI_ReceiverOverflowClear (${PLIB_INSTANCE} );
    }
    return 0;
}
</#if>

<#if DRV_ISR=="ISR">
int32_t DRV_SPI${DRV_INSTANCE}_ISRErrorTasks(struct DRV_SPI_OBJ * dObj)
{

    if (dObj->currentJob == NULL)
    {
        return 0;
    }

    register DRV_SPI_JOB_OBJECT * currentJob = dObj->currentJob;

    if (PLIB_SPI_ReceiverHasOverflowed(${PLIB_INSTANCE}))
    {
        if (currentJob->completeCB != NULL)
        {
            (*currentJob->completeCB)(DRV_SPI_BUFFER_EVENT_ERROR, (DRV_SPI_BUFFER_HANDLE)currentJob, currentJob->context);
        }
        currentJob->status = DRV_SPI_BUFFER_EVENT_ERROR;
<#if CONFIG_DRV_SPI_USE_CLIENT_CONFIG_FUCNTION == true>
        if (dObj->operationEnded != NULL)
        {
            (*dObj->operationEnded)(DRV_SPI_BUFFER_EVENT_ERROR, (DRV_SPI_BUFFER_HANDLE)currentJob, currentJob->context);
        }
</#if>
        if (DRV_SPI_SYS_QUEUE_FreeElement(dObj->queue, currentJob) != DRV_SPI_SYS_QUEUE_SUCCESS)
        {
            SYS_ASSERT(false, "\r\nSPI Driver: Queue free element error.");
            return 0;
        }
        dObj->currentJob = NULL;
        PLIB_SPI_BufferClear(${PLIB_INSTANCE});
        PLIB_SPI_ReceiverOverflowClear (${PLIB_INSTANCE} );
        SYS_INT_SourceStatusClear(${ERR_INT});

    }
    return 0;
}
</#if>

// *********************************************************************************************
// *********************************************************************************************
// Section: Old static driver compatibility APIs, these will be deprecated.
// *********************************************************************************************
// *********************************************************************************************
bool DRV_SPI${DRV_INSTANCE}_ReceiverBufferIsFull(void)
{
    return (PLIB_SPI_ReceiverBufferIsFull(${PLIB_INSTANCE}));
}

bool DRV_SPI${DRV_INSTANCE}_TransmitterBufferIsFull(void)
{
    return (PLIB_SPI_TransmitBufferIsFull(${PLIB_INSTANCE}));
}

int32_t DRV_SPI${DRV_INSTANCE}_BufferAddWriteRead(const void * txBuffer, void * rxBuffer, uint32_t size)
{
    bool continueLoop;
    int32_t txcounter = 0;
    int32_t rxcounter = 0;
<#if DRV_BUFFER == "EBM">
    uint8_t unitsTxed = 0;
    <#if DRV_BIT_WIDTH == "8">
    const uint8_t maxUnits = 16;
    <#elseif DRV_BIT_WIDTH == "16">
    const uint8_t maxUnits = 8;
    <#else>
    const uint8_t maxUnits = 4;
    </#if>
</#if>
    do {
        continueLoop = false;
<#if DRV_BUFFER == "EBM">
        unitsTxed = 0;
        if (PLIB_SPI_TransmitBufferIsEmpty(${PLIB_INSTANCE}))
        {
    <#if DRV_BIT_WIDTH == "8">
            while (!PLIB_SPI_TransmitBufferIsFull(${PLIB_INSTANCE}) && (txcounter < size) && unitsTxed != maxUnits)
    <#elseif DRV_BIT_WIDTH == "16">
            while (!PLIB_SPI_TransmitBufferIsFull(${PLIB_INSTANCE}) && (txcounter < (size>>1)) && unitsTxed != maxUnits)
    <#else>
            while (!PLIB_SPI_TransmitBufferIsFull(${PLIB_INSTANCE}) && (txcounter < (size>>2)) && unitsTxed != maxUnits)
    </#if>
            {
    <#if DRV_BIT_WIDTH == "8">
                PLIB_SPI_BufferWrite(${PLIB_INSTANCE}, ((uint8_t*)txBuffer)[txcounter]);
    <#elseif DRV_BIT_WIDTH == "16">
                PLIB_SPI_BufferWrite16bit(${PLIB_INSTANCE}, ((uint16_t*)txBuffer)[txcounter]);
    <#else>
                PLIB_SPI_BufferWrite32bit(${PLIB_INSTANCE}, ((uint32_t*)txBuffer)[txcounter]);
    </#if>
                txcounter++;
                continueLoop = true;
                unitsTxed++;
            }
        }
<#else>
    <#if DRV_BIT_WIDTH == "8">
        if(!PLIB_SPI_TransmitBufferIsFull(${PLIB_INSTANCE}) && txcounter < size)
    <#elseif DRV_BIT_WIDTH == "16">
        if(!PLIB_SPI_TransmitBufferIsFull(${PLIB_INSTANCE}) && txcounter < (size>>1))
    <#else>
        if(!PLIB_SPI_TransmitBufferIsFull(${PLIB_INSTANCE}) && txcounter < (size>>2))
    </#if>
        {
    <#if DRV_BIT_WIDTH == "8">
            PLIB_SPI_BufferWrite(${PLIB_INSTANCE}, ((uint8_t*)txBuffer)[txcounter]);
    <#elseif DRV_BIT_WIDTH == "16">
            PLIB_SPI_BufferWrite16bit(${PLIB_INSTANCE}, ((uint16_t*)txBuffer)[txcounter]);
    <#else>
            PLIB_SPI_BufferWrite32bit(${PLIB_INSTANCE}, ((uint32_t*)txBuffer)[txcounter]);
    </#if>
            txcounter++;
            continueLoop = true;
        }
</#if>

        while (txcounter != rxcounter)
        {
<#if DRV_BUFFER == "EBM">
            while (PLIB_SPI_ReceiverFIFOIsEmpty(${PLIB_INSTANCE}));
    <#if DRV_BIT_WIDTH == "8">
            ((uint8_t*)rxBuffer)[rxcounter] = PLIB_SPI_BufferRead(${PLIB_INSTANCE});
    <#elseif DRV_BIT_WIDTH == "16">
            ((uint16_t*)rxBuffer)[rxcounter] = PLIB_SPI_BufferRead16bit(${PLIB_INSTANCE});
    <#else>
            ((uint32_t*)rxBuffer)[rxcounter] = PLIB_SPI_BufferRead32bit(${PLIB_INSTANCE});
    </#if>
            rxcounter++;
            continueLoop = true;
<#else>
            while(!PLIB_SPI_ReceiverBufferIsFull(${PLIB_INSTANCE}));
    <#if DRV_BIT_WIDTH == "8">
            ((uint8_t*)rxBuffer)[rxcounter] = PLIB_SPI_BufferRead(${PLIB_INSTANCE});
    <#elseif DRV_BIT_WIDTH == "16">
            ((uint16_t*)rxBuffer)[rxcounter] = PLIB_SPI_BufferRead16bit(${PLIB_INSTANCE});
    <#else>
            ((uint32_t*)rxBuffer)[rxcounter] = PLIB_SPI_BufferRead32bit(${PLIB_INSTANCE});
    </#if>
            rxcounter++;
            continueLoop = true;
</#if>
        }
        if (txcounter > rxcounter)
        {
            continueLoop = true;
        }
    }while(continueLoop);
<#if DRV_BIT_WIDTH == "8">
    return txcounter;
<#elseif DRV_BIT_WIDTH == "16">
    return txcounter<<1;
<#else>
    return txcounter<<2;
</#if>
}

</#macro>
<#if CONFIG_DRV_SPI_IDX0 == true>
<@DRV_SPI_STATIC_FUNCTIONS DRV_INSTANCE="0"
PLIB_INSTANCE=CONFIG_DRV_SPI_SPI_ID_IDX0
CLOCK_SOURCE=CONFIG_DRV_SPI_SPI_CLOCK_IDX0
CLOCK_MODE=CONFIG_DRV_SPI_CLOCK_MODE_IDX0
INPUT_PHASE=CONFIG_DRV_SPI_INPUT_PHASE_IDX0
BAUD_RATE=CONFIG_DRV_SPI_BAUD_RATE_IDX0
XMIT_INT=CONFIG_DRV_SPI_TX_INT_SOURCE_IDX0
RCV_INT=CONFIG_DRV_SPI_RX_INT_SOURCE_IDX0
ERR_INT=CONFIG_DRV_SPI_ERROR_INT_SOURCE_IDX0
USE_SS=CONFIG_DRV_SPI_SPI_USE_SS_FOR_SLAVE_IDX0
QUEUE_SIZE=CONFIG_DRV_SPI_QUEUE_SIZE_IDX0
RESERVED_JOB=CONFIG_DRV_SPI_RESERVED_JOB_IDX0
TRANS_PER_SM=CONFIG_DRV_SPI_TRANSACTIONS_PER_SM_RUN_IDX0
FRAME_SYNC_PULSE=CONFIG_DRV_SPI_FRAME_SYNC_PULSE_IDX0
FRAME_PULSE_POLARITY=CONFIG_DRV_SPI_FRAME_PULSE_POLARITY_IDX0
FRAME_PULSE_DIRECTION=CONFIG_DRV_SPI_FRAME_PULSE_DIRECTION_IDX0
FRAME_PULSE_EDGE=CONFIG_DRV_SPI_FRAME_PULSE_EDGE_IDX0
FRAME_PULSE_WIDTH=CONFIG_DRV_SPI_FRAME_PULSE_WIDTH_IDX0
AUDIO_TRANSMIT_MODE=CONFIG_DRV_SPI_AUDIO_TRANSMIT_MODE_IDX0
AUDIO_PROTOCOL_MODE=CONFIG_DRV_SPI_AUDIO_PROTOCOL_MODE_IDX0
USE_DMA=CONFIG_DRV_SPI_USE_DMA_IDX0
TX_DMA_THRESHOLD=CONFIG_DRV_SPI_TX_DMA_THRESHOLD_IDX0
RX_DMA_THRESHOLD=CONFIG_DRV_SPI_RX_DMA_THRESHOLD_IDX0
/>
</#if>
<#if CONFIG_DRV_SPI_IDX1 == true>
<@DRV_SPI_STATIC_FUNCTIONS DRV_INSTANCE="1"
PLIB_INSTANCE=CONFIG_DRV_SPI_SPI_ID_IDX1
CLOCK_SOURCE=CONFIG_DRV_SPI_SPI_CLOCK_IDX1
CLOCK_MODE=CONFIG_DRV_SPI_CLOCK_MODE_IDX1
INPUT_PHASE=CONFIG_DRV_SPI_INPUT_PHASE_IDX1
BAUD_RATE=CONFIG_DRV_SPI_BAUD_RATE_IDX1
XMIT_INT=CONFIG_DRV_SPI_TX_INT_SOURCE_IDX1
RCV_INT=CONFIG_DRV_SPI_RX_INT_SOURCE_IDX1
ERR_INT=CONFIG_DRV_SPI_ERROR_INT_SOURCE_IDX1
USE_SS=CONFIG_DRV_SPI_SPI_USE_SS_FOR_SLAVE_IDX1
QUEUE_SIZE=CONFIG_DRV_SPI_QUEUE_SIZE_IDX1
RESERVED_JOB=CONFIG_DRV_SPI_RESERVED_JOB_IDX1
TRANS_PER_SM=CONFIG_DRV_SPI_TRANSACTIONS_PER_SM_RUN_IDX1
FRAME_SYNC_PULSE=CONFIG_DRV_SPI_FRAME_SYNC_PULSE_IDX1
FRAME_PULSE_POLARITY=CONFIG_DRV_SPI_FRAME_PULSE_POLARITY_IDX1
FRAME_PULSE_DIRECTION=CONFIG_DRV_SPI_FRAME_PULSE_DIRECTION_IDX1
FRAME_PULSE_EDGE=CONFIG_DRV_SPI_FRAME_PULSE_EDGE_IDX1
FRAME_PULSE_WIDTH=CONFIG_DRV_SPI_FRAME_PULSE_WIDTH_IDX1
AUDIO_TRANSMIT_MODE=CONFIG_DRV_SPI_AUDIO_TRANSMIT_MODE_IDX1
AUDIO_PROTOCOL_MODE=CONFIG_DRV_SPI_AUDIO_PROTOCOL_MODE_IDX1
USE_DMA=CONFIG_DRV_SPI_USE_DMA_IDX1
TX_DMA_THRESHOLD=CONFIG_DRV_SPI_TX_DMA_THRESHOLD_IDX1
RX_DMA_THRESHOLD=CONFIG_DRV_SPI_RX_DMA_THRESHOLD_IDX1
/>
</#if>
<#if CONFIG_DRV_SPI_IDX2 == true>
<@DRV_SPI_STATIC_FUNCTIONS DRV_INSTANCE="2"
PLIB_INSTANCE=CONFIG_DRV_SPI_SPI_ID_IDX2
CLOCK_SOURCE=CONFIG_DRV_SPI_SPI_CLOCK_IDX2
CLOCK_MODE=CONFIG_DRV_SPI_CLOCK_MODE_IDX2
INPUT_PHASE=CONFIG_DRV_SPI_INPUT_PHASE_IDX2
BAUD_RATE=CONFIG_DRV_SPI_BAUD_RATE_IDX2
XMIT_INT=CONFIG_DRV_SPI_TX_INT_SOURCE_IDX2
RCV_INT=CONFIG_DRV_SPI_RX_INT_SOURCE_IDX2
ERR_INT=CONFIG_DRV_SPI_ERROR_INT_SOURCE_IDX2
USE_SS=CONFIG_DRV_SPI_SPI_USE_SS_FOR_SLAVE_IDX2
QUEUE_SIZE=CONFIG_DRV_SPI_QUEUE_SIZE_IDX2
RESERVED_JOB=CONFIG_DRV_SPI_RESERVED_JOB_IDX2
TRANS_PER_SM=CONFIG_DRV_SPI_TRANSACTIONS_PER_SM_RUN_IDX2
FRAME_SYNC_PULSE=CONFIG_DRV_SPI_FRAME_SYNC_PULSE_IDX2
FRAME_PULSE_POLARITY=CONFIG_DRV_SPI_FRAME_PULSE_POLARITY_IDX2
FRAME_PULSE_DIRECTION=CONFIG_DRV_SPI_FRAME_PULSE_DIRECTION_IDX2
FRAME_PULSE_EDGE=CONFIG_DRV_SPI_FRAME_PULSE_EDGE_IDX2
FRAME_PULSE_WIDTH=CONFIG_DRV_SPI_FRAME_PULSE_WIDTH_IDX2
AUDIO_TRANSMIT_MODE=CONFIG_DRV_SPI_AUDIO_TRANSMIT_MODE_IDX2
AUDIO_PROTOCOL_MODE=CONFIG_DRV_SPI_AUDIO_PROTOCOL_MODE_IDX2
USE_DMA=CONFIG_DRV_SPI_USE_DMA_IDX2
TX_DMA_THRESHOLD=CONFIG_DRV_SPI_TX_DMA_THRESHOLD_IDX2
RX_DMA_THRESHOLD=CONFIG_DRV_SPI_RX_DMA_THRESHOLD_IDX2
/>
</#if>
<#if CONFIG_DRV_SPI_IDX3 == true>
<@DRV_SPI_STATIC_FUNCTIONS DRV_INSTANCE="3"
PLIB_INSTANCE=CONFIG_DRV_SPI_SPI_ID_IDX3
CLOCK_SOURCE=CONFIG_DRV_SPI_SPI_CLOCK_IDX3
CLOCK_MODE=CONFIG_DRV_SPI_CLOCK_MODE_IDX3
INPUT_PHASE=CONFIG_DRV_SPI_INPUT_PHASE_IDX3
BAUD_RATE=CONFIG_DRV_SPI_BAUD_RATE_IDX3
XMIT_INT=CONFIG_DRV_SPI_TX_INT_SOURCE_IDX3
RCV_INT=CONFIG_DRV_SPI_RX_INT_SOURCE_IDX3
ERR_INT=CONFIG_DRV_SPI_ERROR_INT_SOURCE_IDX3
USE_SS=CONFIG_DRV_SPI_SPI_USE_SS_FOR_SLAVE_IDX3
QUEUE_SIZE=CONFIG_DRV_SPI_QUEUE_SIZE_IDX3
RESERVED_JOB=CONFIG_DRV_SPI_RESERVED_JOB_IDX3
TRANS_PER_SM=CONFIG_DRV_SPI_TRANSACTIONS_PER_SM_RUN_IDX3
FRAME_SYNC_PULSE=CONFIG_DRV_SPI_FRAME_SYNC_PULSE_IDX3
FRAME_PULSE_POLARITY=CONFIG_DRV_SPI_FRAME_PULSE_POLARITY_IDX3
FRAME_PULSE_DIRECTION=CONFIG_DRV_SPI_FRAME_PULSE_DIRECTION_IDX3
FRAME_PULSE_EDGE=CONFIG_DRV_SPI_FRAME_PULSE_EDGE_IDX3
FRAME_PULSE_WIDTH=CONFIG_DRV_SPI_FRAME_PULSE_WIDTH_IDX3
AUDIO_TRANSMIT_MODE=CONFIG_DRV_SPI_AUDIO_TRANSMIT_MODE_IDX3
AUDIO_PROTOCOL_MODE=CONFIG_DRV_SPI_AUDIO_PROTOCOL_MODE_IDX3
USE_DMA=CONFIG_DRV_SPI_USE_DMA_IDX3
TX_DMA_THRESHOLD=CONFIG_DRV_SPI_TX_DMA_THRESHOLD_IDX3
RX_DMA_THRESHOLD=CONFIG_DRV_SPI_RX_DMA_THRESHOLD_IDX3
/>
</#if>
<#if CONFIG_DRV_SPI_IDX4 == true>
<@DRV_SPI_STATIC_FUNCTIONS DRV_INSTANCE="4"
PLIB_INSTANCE=CONFIG_DRV_SPI_SPI_ID_IDX4
CLOCK_SOURCE=CONFIG_DRV_SPI_SPI_CLOCK_IDX4
CLOCK_MODE=CONFIG_DRV_SPI_CLOCK_MODE_IDX4
INPUT_PHASE=CONFIG_DRV_SPI_INPUT_PHASE_IDX4
BAUD_RATE=CONFIG_DRV_SPI_BAUD_RATE_IDX4
XMIT_INT=CONFIG_DRV_SPI_TX_INT_SOURCE_IDX4
RCV_INT=CONFIG_DRV_SPI_RX_INT_SOURCE_IDX4
ERR_INT=CONFIG_DRV_SPI_ERROR_INT_SOURCE_IDX4
USE_SS=CONFIG_DRV_SPI_SPI_USE_SS_FOR_SLAVE_IDX4
QUEUE_SIZE=CONFIG_DRV_SPI_QUEUE_SIZE_IDX4
RESERVED_JOB=CONFIG_DRV_SPI_RESERVED_JOB_IDX4
TRANS_PER_SM=CONFIG_DRV_SPI_TRANSACTIONS_PER_SM_RUN_IDX4
FRAME_SYNC_PULSE=CONFIG_DRV_SPI_FRAME_SYNC_PULSE_IDX4
FRAME_PULSE_POLARITY=CONFIG_DRV_SPI_FRAME_PULSE_POLARITY_IDX4
FRAME_PULSE_DIRECTION=CONFIG_DRV_SPI_FRAME_PULSE_DIRECTION_IDX4
FRAME_PULSE_EDGE=CONFIG_DRV_SPI_FRAME_PULSE_EDGE_IDX4
FRAME_PULSE_WIDTH=CONFIG_DRV_SPI_FRAME_PULSE_WIDTH_IDX4
AUDIO_TRANSMIT_MODE=CONFIG_DRV_SPI_AUDIO_TRANSMIT_MODE_IDX4
AUDIO_PROTOCOL_MODE=CONFIG_DRV_SPI_AUDIO_PROTOCOL_MODE_IDX4
USE_DMA=CONFIG_DRV_SPI_USE_DMA_IDX4
TX_DMA_THRESHOLD=CONFIG_DRV_SPI_TX_DMA_THRESHOLD_IDX4
RX_DMA_THRESHOLD=CONFIG_DRV_SPI_RX_DMA_THRESHOLD_IDX4
/>
</#if>
<#if CONFIG_DRV_SPI_IDX5 == true>
<@DRV_SPI_STATIC_FUNCTIONS DRV_INSTANCE="5"
PLIB_INSTANCE=CONFIG_DRV_SPI_SPI_ID_IDX5
CLOCK_SOURCE=CONFIG_DRV_SPI_SPI_CLOCK_IDX5
CLOCK_MODE=CONFIG_DRV_SPI_CLOCK_MODE_IDX5
INPUT_PHASE=CONFIG_DRV_SPI_INPUT_PHASE_IDX5
BAUD_RATE=CONFIG_DRV_SPI_BAUD_RATE_IDX5
XMIT_INT=CONFIG_DRV_SPI_TX_INT_SOURCE_IDX5
RCV_INT=CONFIG_DRV_SPI_RX_INT_SOURCE_IDX5
ERR_INT=CONFIG_DRV_SPI_ERROR_INT_SOURCE_IDX5
USE_SS=CONFIG_DRV_SPI_SPI_USE_SS_FOR_SLAVE_IDX5
QUEUE_SIZE=CONFIG_DRV_SPI_QUEUE_SIZE_IDX5
RESERVED_JOB=CONFIG_DRV_SPI_RESERVED_JOB_IDX5
TRANS_PER_SM=CONFIG_DRV_SPI_TRANSACTIONS_PER_SM_RUN_IDX5
FRAME_SYNC_PULSE=CONFIG_DRV_SPI_FRAME_SYNC_PULSE_IDX5
FRAME_PULSE_POLARITY=CONFIG_DRV_SPI_FRAME_PULSE_POLARITY_IDX5
FRAME_PULSE_DIRECTION=CONFIG_DRV_SPI_FRAME_PULSE_DIRECTION_IDX5
FRAME_PULSE_EDGE=CONFIG_DRV_SPI_FRAME_PULSE_EDGE_IDX5
FRAME_PULSE_WIDTH=CONFIG_DRV_SPI_FRAME_PULSE_WIDTH_IDX5
AUDIO_TRANSMIT_MODE=CONFIG_DRV_SPI_AUDIO_TRANSMIT_MODE_IDX5
AUDIO_PROTOCOL_MODE=CONFIG_DRV_SPI_AUDIO_PROTOCOL_MODE_IDX5
USE_DMA=CONFIG_DRV_SPI_USE_DMA_IDX5
TX_DMA_THRESHOLD=CONFIG_DRV_SPI_TX_DMA_THRESHOLD_IDX5
RX_DMA_THRESHOLD=CONFIG_DRV_SPI_RX_DMA_THRESHOLD_IDX5
/>
</#if>
