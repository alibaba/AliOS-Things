/*******************************************************************************
  USART Driver Static implementation with DMA support

  Company:
    Microchip Technology Inc.

  File Name:
    drv_usart_static_dma.c

  Summary:
    Source code for the USART driver static implementation with DMA support.

  Description:
    The USART device driver provides a simple interface to manage the USART
    modules on Microchip microcontrollers. This file contains static implementation
    for the USART driver supporting DMA mode(Using DMA channels for data transmission
    and reception).

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

#include "system_config.h"
#include "system_definitions.h"

<#macro DRV_USART_STATIC_FUNCTIONS DRV_INSTANCE PLIB_INSTANCE TX_DMA RX_DMA HANDSHAKE_MODE LINE_CONTROL BRG_RATE XMIT_INT RCV_INT ERR_INT>
<#assign MODE_IRDA="false">
<#assign MODE_LOOPBACK="false">
<#assign OPERATION_MODE="USART_ENABLE_TX_RX_USED">
<#assign AUTO_BAUD="false">
<#assign WAKEUP_ONSTART="false">
<#assign SIDL_MODE="false">

<#if .vars["CONFIG_DRV_USART_OPER_MODE_IDX${DRV_INSTANCE}"] == "DRV_USART_OPERATION_MODE_IRDA">
    <#assign MODE_IRDA="true">
    <#assign OPERATION_MODE="USART_ENABLE_TX_RX_BCLK_USED">
</#if>
<#if .vars["CONFIG_DRV_USART_OPER_MODE_IDX${DRV_INSTANCE}"] == "DRV_USART_OPERATION_MODE_LOOPBACK">
    <#assign MODE_LOOPBACK="true">
</#if>
<#if .vars["CONFIG_DRV_USART_HANDSHAKE_MODE_IDX${DRV_INSTANCE}"] != "DRV_USART_HANDSHAKE_NONE">
    <#assign OPERATION_MODE="USART_ENABLE_TX_RX_CTS_RTS_USED">
</#if>
<#if .vars["CONFIG_DRV_USART_INIT_FLAG_AUTO_BAUD_IDX${DRV_INSTANCE}"] == true>
    <#assign AUTO_BAUD="true">
</#if>
<#if .vars["CONFIG_DRV_USART_INIT_FLAG_WAKE_ON_START_IDX${DRV_INSTANCE}"] == true>
    <#assign WAKEUP_ONSTART="true">
</#if>
<#if .vars["CONFIG_DRV_USART_INIT_FLAG_STOP_IN_IDLE_IDX${DRV_INSTANCE}"] == true>
    <#assign SIDL_MODE="true">
</#if>
<#if TX_DMA == true>
<#if .vars["CONFIG_DRV_USART_XMIT_DMA_CH_IDX${DRV_INSTANCE}"] == "0">
    <#assign XMIT_DMA_CHANNEL = CONFIG_SYS_DMA_CHANNEL_ID_IDX0>
    <#assign XMIT_DMA_INT = "INT_SOURCE_DMA_0">
<#elseif .vars["CONFIG_DRV_USART_XMIT_DMA_CH_IDX${DRV_INSTANCE}"] == "1">
    <#assign XMIT_DMA_CHANNEL = CONFIG_SYS_DMA_CHANNEL_ID_IDX1>
    <#assign XMIT_DMA_INT = "INT_SOURCE_DMA_1">
<#elseif .vars["CONFIG_DRV_USART_XMIT_DMA_CH_IDX${DRV_INSTANCE}"] == "2">
    <#assign XMIT_DMA_CHANNEL = CONFIG_SYS_DMA_CHANNEL_ID_IDX2>
    <#assign XMIT_DMA_INT = "INT_SOURCE_DMA_2">
<#elseif .vars["CONFIG_DRV_USART_XMIT_DMA_CH_IDX${DRV_INSTANCE}"] == "3">
    <#assign XMIT_DMA_CHANNEL = CONFIG_SYS_DMA_CHANNEL_ID_IDX3>
    <#assign XMIT_DMA_INT = "INT_SOURCE_DMA_3">
<#elseif .vars["CONFIG_DRV_USART_XMIT_DMA_CH_IDX${DRV_INSTANCE}"] == "4">
    <#assign XMIT_DMA_CHANNEL = CONFIG_SYS_DMA_CHANNEL_ID_IDX4>
    <#assign XMIT_DMA_INT = "INT_SOURCE_DMA_4">
<#elseif .vars["CONFIG_DRV_USART_XMIT_DMA_CH_IDX${DRV_INSTANCE}"] == "5">
    <#assign XMIT_DMA_CHANNEL = CONFIG_SYS_DMA_CHANNEL_ID_IDX5>
    <#assign XMIT_DMA_INT = "INT_SOURCE_DMA_5">
<#elseif .vars["CONFIG_DRV_USART_XMIT_DMA_CH_IDX${DRV_INSTANCE}"] == "6">
    <#assign XMIT_DMA_CHANNEL = CONFIG_SYS_DMA_CHANNEL_ID_IDX6>
    <#assign XMIT_DMA_INT = "INT_SOURCE_DMA_6">
<#elseif .vars["CONFIG_DRV_USART_XMIT_DMA_CH_IDX${DRV_INSTANCE}"] == "7">
    <#assign XMIT_DMA_CHANNEL = CONFIG_SYS_DMA_CHANNEL_ID_IDX7>
    <#assign XMIT_DMA_INT = "INT_SOURCE_DMA_7">
</#if>
<#else>
    <#assign XMIT_DMA_CHANNEL = "DMA_CHANNEL_NONE">
</#if>
<#if RX_DMA == true>
<#if .vars["CONFIG_DRV_USART_RCV_DMA_CH_IDX${DRV_INSTANCE}"] == "0">
    <#assign RCV_DMA_CHANNEL = CONFIG_SYS_DMA_CHANNEL_ID_IDX0>
    <#assign RCV_DMA_INT = "INT_SOURCE_DMA_0">
<#elseif .vars["CONFIG_DRV_USART_RCV_DMA_CH_IDX${DRV_INSTANCE}"] == "1">
    <#assign RCV_DMA_CHANNEL = CONFIG_SYS_DMA_CHANNEL_ID_IDX1>
    <#assign RCV_DMA_INT = "INT_SOURCE_DMA_1">
<#elseif .vars["CONFIG_DRV_USART_RCV_DMA_CH_IDX${DRV_INSTANCE}"] == "2">
    <#assign RCV_DMA_CHANNEL = CONFIG_SYS_DMA_CHANNEL_ID_IDX2>
    <#assign RCV_DMA_INT = "INT_SOURCE_DMA_2">
<#elseif .vars["CONFIG_DRV_USART_RCV_DMA_CH_IDX${DRV_INSTANCE}"] == "3">
    <#assign RCV_DMA_CHANNEL = CONFIG_SYS_DMA_CHANNEL_ID_IDX3>
    <#assign RCV_DMA_INT = "INT_SOURCE_DMA_3">
<#elseif .vars["CONFIG_DRV_USART_RCV_DMA_CH_IDX${DRV_INSTANCE}"] == "4">
    <#assign RCV_DMA_CHANNEL = CONFIG_SYS_DMA_CHANNEL_ID_IDX4>
    <#assign RCV_DMA_INT = "INT_SOURCE_DMA_4">
<#elseif .vars["CONFIG_DRV_USART_RCV_DMA_CH_IDX${DRV_INSTANCE}"] == "5">
    <#assign RCV_DMA_CHANNEL = CONFIG_SYS_DMA_CHANNEL_ID_IDX5>
    <#assign RCV_DMA_INT = "INT_SOURCE_DMA_5">
<#elseif .vars["CONFIG_DRV_USART_RCV_DMA_CH_IDX${DRV_INSTANCE}"] == "6">
    <#assign RCV_DMA_CHANNEL = CONFIG_SYS_DMA_CHANNEL_ID_IDX6>
    <#assign RCV_DMA_INT = "INT_SOURCE_DMA_6">
<#elseif .vars["CONFIG_DRV_USART_RCV_DMA_CH_IDX${DRV_INSTANCE}"] == "7">
    <#assign RCV_DMA_CHANNEL = CONFIG_SYS_DMA_CHANNEL_ID_IDX7>
    <#assign RCV_DMA_INT = "INT_SOURCE_DMA_7">
</#if>
<#else>
    <#assign RCV_DMA_CHANNEL = "DMA_CHANNEL_NONE">
</#if>
// *****************************************************************************
// *****************************************************************************
// Section: Global Data
// *****************************************************************************
// *****************************************************************************

/* This is the driver static object . */
DRV_USART_OBJ  gDrvUSART${DRV_INSTANCE}Obj ;

// *****************************************************************************
// *****************************************************************************
// Section: Instance ${DRV_INSTANCE} static driver functions
// *****************************************************************************
// *****************************************************************************

SYS_MODULE_OBJ DRV_USART${DRV_INSTANCE}_Initialize(void)
{
    DRV_USART_OBJ *dObj = (DRV_USART_OBJ*)NULL;
    uint32_t clockSource;

    dObj = &gDrvUSART${DRV_INSTANCE}Obj;

    /* Disable the USART module to configure it*/
    PLIB_USART_Disable (${PLIB_INSTANCE});

    /* Update the USART STATIC OBJECT parameters. */
    dObj->interruptNestingCount = 0;
    dObj->queueSizeCurrentRead  = 0;
    dObj->queueSizeCurrentWrite = 0;
    dObj->queueRead             = NULL;
    dObj->queueWrite            = NULL;
    dObj->eventHandler          = NULL;
    dObj->context               = (uintptr_t)NULL;
    dObj->error                 = DRV_USART_ERROR_NONE;
<#if TX_DMA == true>
    dObj->dmaChannelHandleWrite = SYS_DMA_CHANNEL_HANDLE_INVALID;
</#if>
<#if RX_DMA == true>
    dObj->dmaChannelHandleRead  = SYS_DMA_CHANNEL_HANDLE_INVALID;
</#if>
<#if CONFIG_USE_3RDPARTY_RTOS>

    /* Create the hardware instance mutex. */
     if(OSAL_MUTEX_Create(&(dObj->mutexDriverInstance)) != OSAL_RESULT_TRUE)
     {
        return SYS_MODULE_OBJ_INVALID;
     }
</#if>

<#if TX_DMA == true || RX_DMA == true>
    /* DMA mode of operation. Allocate a handle for the specified channel. */
<#if TX_DMA == true>
    
    /* TX DMA channel setup */
    dObj->dmaChannelHandleWrite = SYS_DMA_ChannelAllocate(${XMIT_DMA_CHANNEL});
    if(SYS_DMA_CHANNEL_HANDLE_INVALID != dObj->dmaChannelHandleWrite)
    {
        /* Setting the channel priority same as the channel number */
        SYS_DMA_ChannelSetup(dObj->dmaChannelHandleWrite, SYS_DMA_CHANNEL_OP_MODE_BASIC, ${XMIT_INT});
    }
</#if>
<#if RX_DMA == true>

    /* RX DMA channel setup */
    dObj->dmaChannelHandleRead = SYS_DMA_ChannelAllocate(${RCV_DMA_CHANNEL});
    if(SYS_DMA_CHANNEL_HANDLE_INVALID != dObj->dmaChannelHandleRead)
    {
        /* Setting the channel priority same as the channel number */
        SYS_DMA_ChannelSetup(dObj->dmaChannelHandleRead,
            SYS_DMA_CHANNEL_OP_MODE_BASIC, ${RCV_INT});
    }
</#if>
</#if>

<#if HANDSHAKE_MODE != "DRV_USART_HANDSHAKE_NONE">

    /* Set the handshake mode to either simplex or flow control */
    PLIB_USART_HandshakeModeSelect(${PLIB_INSTANCE}, ${HANDSHAKE_MODE});
</#if>

    /* Initialize the USART based on configuration settings */
    PLIB_USART_InitializeModeGeneral(${PLIB_INSTANCE},
            ${AUTO_BAUD},  /*Auto baud*/
            ${MODE_LOOPBACK},  /*LoopBack mode*/
            ${WAKEUP_ONSTART},  /*Auto wakeup on start*/
            ${MODE_IRDA},  /*IRDA mode*/
            ${SIDL_MODE});  /*Stop In Idle mode*/

    /* Set the line control mode */
    PLIB_USART_LineControlModeSelect(${PLIB_INSTANCE}, ${LINE_CONTROL});

    /* We set the receive interrupt mode to receive an interrupt whenever FIFO
       is not empty */
    PLIB_USART_InitializeOperation(${PLIB_INSTANCE},
            USART_RECEIVE_FIFO_ONE_CHAR,
            USART_TRANSMIT_FIFO_IDLE,
            ${OPERATION_MODE});
<#if AUTO_BAUD == "false">

    /* Get the USART clock source value*/
    clockSource = SYS_CLK_PeripheralFrequencyGet ( CLK_BUS_PERIPHERAL_1 );

    /* Set the baud rate and enable the USART */
    PLIB_USART_BaudSetAndEnable(${PLIB_INSTANCE},
            clockSource,
            ${BRG_RATE});  /*Desired Baud rate value*/
<#else>

    /* Enable the USART without setting baud*/
    PLIB_USART_TransmitterEnable(${PLIB_INSTANCE});
    PLIB_USART_ReceiverEnable(${PLIB_INSTANCE});
    PLIB_USART_Enable(${PLIB_INSTANCE});
</#if>

    /* Clear the interrupt status flags to be on the safer side*/
    SYS_INT_SourceStatusClear(${XMIT_INT});
    SYS_INT_SourceStatusClear(${RCV_INT});
    SYS_INT_SourceStatusClear(${ERR_INT});
<#if TX_DMA == true>
    SYS_INT_SourceStatusClear(${XMIT_DMA_INT});
</#if>
<#if RX_DMA == true>
    SYS_INT_SourceStatusClear(${RCV_DMA_INT});
</#if>
<#if CONFIG_DRV_USART_INTERRUPT_MODE == true>

    /* Enable the error interrupt source */
    SYS_INT_SourceEnable(${ERR_INT});

<#if CONFIG_DRV_USART_BYTE_MODEL_SUPPORT ==true>
    /* Enable the Receive interrupt source */
    SYS_INT_SourceEnable(${RCV_INT});
</#if>
</#if>

    /* Return the driver instance value*/
    return (SYS_MODULE_OBJ)DRV_USART_INDEX_${DRV_INSTANCE};
}

void  DRV_USART${DRV_INSTANCE}_Deinitialize(void)
{
<#if CONFIG_DRV_USART_INTERRUPT_MODE == true>
    bool status;
</#if>
<#if TX_DMA == true || RX_DMA == true>
    DRV_USART_OBJ *dObj = (DRV_USART_OBJ*)NULL;

    dObj = &gDrvUSART${DRV_INSTANCE}Obj;
</#if>
<#if CONFIG_DRV_USART_INTERRUPT_MODE == true>

    /* Disable the interrupts */
    status = SYS_INT_SourceDisable(${XMIT_INT}) ;
    status = SYS_INT_SourceDisable(${RCV_INT}) ;
    status = SYS_INT_SourceDisable(${ERR_INT});
<#if TX_DMA == true>
    status = SYS_INT_SourceDisable(${XMIT_DMA_INT});
</#if>
<#if RX_DMA == true>
    status = SYS_INT_SourceDisable(${RCV_DMA_INT});
</#if>
    /* Ignore the warning */
    (void)status;

</#if>
<#if TX_DMA == true || RX_DMA == true>
    /* Deallocate the allocated DMA channel handles  */
<#if TX_DMA == true>
    if(SYS_DMA_CHANNEL_HANDLE_INVALID != dObj->dmaChannelHandleWrite)
    {
        SYS_DMA_ChannelRelease(dObj->dmaChannelHandleWrite);
    }
</#if>
<#if RX_DMA == true>
    if(SYS_DMA_CHANNEL_HANDLE_INVALID != dObj->dmaChannelHandleRead)
    {
        SYS_DMA_ChannelRelease(dObj->dmaChannelHandleRead);
    }
</#if>
</#if>

    /* Disable USART module */
    PLIB_USART_Disable (${PLIB_INSTANCE});
<#if CONFIG_USE_3RDPARTY_RTOS>

    /* Deallocate all mutexes */
    if(OSAL_MUTEX_Delete(&(dObj->mutexDriverInstance)) != OSAL_RESULT_TRUE)
    {
        SYS_DEBUG_MESSAGE(SYS_ERROR_DEBUG, "\r\nUSART Driver: Mutex Delete Failed");
        return;
    }
</#if>

}


SYS_STATUS DRV_USART${DRV_INSTANCE}_Status(void)
{
    /* Return the status as ready always */
    return SYS_STATUS_READY;
}

void DRV_USART${DRV_INSTANCE}_TasksTransmit(void)
{
    /* This is the USART Driver Transmit tasks routine.
       In this function, the driver checks if a transmit
       interrupt is active and if there are any buffers in
       queue. If so the buffer is serviced. A buffer that
       is serviced completely is removed from the queue.
     */

<#if TX_DMA == true>
    if(SYS_INT_SourceStatusGet(${XMIT_INT}) ||
       SYS_INT_SourceStatusGet(${XMIT_DMA_INT}))
<#else>
    if(SYS_INT_SourceStatusGet(${XMIT_INT}))
</#if>
    {
        /* The USART driver is configured to generate an
           interrupt when the FIFO is empty. Additionally
           the queue is not empty. Which means there is
           work to be done in this routine. */

        _DRV_USART${DRV_INSTANCE}_BufferQueueTxTasks();
<#if TX_DMA == false>

        /* Clear up the interrupt flag because DMA is not selected*/
        SYS_INT_SourceStatusClear(${XMIT_INT});
</#if>
    }
}

void DRV_USART${DRV_INSTANCE}_TasksReceive(void)
{
    /* This is the USART Driver Receive tasks routine. If the receive
       interrupt flag is set, the tasks routines are executed.
     */

<#if RX_DMA == true>
    if(SYS_INT_SourceStatusGet(${RCV_INT}) ||
       SYS_INT_SourceStatusGet(${RCV_DMA_INT}))
<#else>
    if(SYS_INT_SourceStatusGet(${RCV_INT}))
</#if>
    {
        _DRV_USART${DRV_INSTANCE}_BufferQueueRxTasks();

<#if RX_DMA == false>
        /* Clear up the interrupt flag because DMA is not selected*/
        SYS_INT_SourceStatusClear(${RCV_INT});
</#if>
    }
}


void DRV_USART${DRV_INSTANCE}_TasksError(void)
{
    /* This is the USART Driver Error tasks routine. In this function, the
     * driver checks if an error interrupt has occurred. If so the error
     * condition is cleared.  */

    if(SYS_INT_SourceStatusGet(${ERR_INT}))
    {
        /* This means an error has occurred */
        _DRV_USART${DRV_INSTANCE}_BufferQueueErrorTasks();
        
        /* Clear up the error interrupt flag */
        SYS_INT_SourceStatusClear(${ERR_INT});
    }
}

DRV_HANDLE DRV_USART${DRV_INSTANCE}_Open(const SYS_MODULE_INDEX index, const DRV_IO_INTENT ioIntent)
{
<#if CONFIG_DRV_USART_READ_WRITE_MODEL_SUPPORT ==true>
<#if CONFIG_USE_3RDPARTY_RTOS>
    DRV_USART_OBJ *dObj = (DRV_USART_OBJ*)NULL;

    dObj = &gDrvUSART${DRV_INSTANCE}Obj;

    /* Create the semaphores for Read Operation of Read-Write Model*/
    if(OSAL_SEM_Create(&(dObj->semReadDone), OSAL_SEM_TYPE_COUNTING, 1, 0) != OSAL_RESULT_TRUE)
    {
        SYS_DEBUG_MESSAGE(SYS_ERROR_DEBUG, "\r\nUSART Driver: Semaphore creation failed");
        return(DRV_HANDLE_INVALID);
    }
    /* Create the semaphores for Write Operation of Read-Write Model*/
    if(OSAL_SEM_Create(&(dObj->semWriteDone), OSAL_SEM_TYPE_COUNTING, 1, 0) != OSAL_RESULT_TRUE)
    {
        SYS_DEBUG_MESSAGE(SYS_ERROR_DEBUG, "\r\nUSART Driver: Semaphore creation failed");
        return(DRV_HANDLE_INVALID);
    }

</#if>
</#if>

    /* Return the driver instance value*/
    return ((DRV_HANDLE)DRV_USART_INDEX_${DRV_INSTANCE} );
}

void DRV_USART${DRV_INSTANCE}_Close(void)
{
<#if CONFIG_DRV_USART_READ_WRITE_MODEL_SUPPORT ==true>
<#if CONFIG_USE_3RDPARTY_RTOS>
    DRV_USART_OBJ *dObj = (DRV_USART_OBJ*)NULL;

    dObj = &gDrvUSART${DRV_INSTANCE}Obj;

    /* Deallocate all semaphores */
    if(OSAL_SEM_Delete(&(dObj->semWriteDone)) != OSAL_RESULT_TRUE)
    {
        SYS_DEBUG_MESSAGE(SYS_ERROR_DEBUG, "\r\nUSART Driver: Unable to delete client write done semaphore");
        return;
    }
    if(OSAL_SEM_Delete(&(dObj->semReadDone)) != OSAL_RESULT_TRUE)
    {
        SYS_DEBUG_MESSAGE(SYS_ERROR_DEBUG, "\r\nUSART Driver: Unable to delete client read done semaphore");
        return;
    }

</#if>
</#if>
    return;
}

DRV_USART_CLIENT_STATUS DRV_USART${DRV_INSTANCE}_ClientStatus(void)
{
    /* Return the status as ready always*/
    return DRV_USART_CLIENT_STATUS_READY;
}

DRV_USART_TRANSFER_STATUS DRV_USART${DRV_INSTANCE}_TransferStatus(void)
{
    DRV_USART_TRANSFER_STATUS result = 0;

    /* Check if RX data available */
    if(PLIB_USART_ReceiverDataIsAvailable(${PLIB_INSTANCE}))
    {
        result|= DRV_USART_TRANSFER_STATUS_RECEIVER_DATA_PRESENT;
    }
    else
    {
        result|= DRV_USART_TRANSFER_STATUS_RECEIVER_EMPTY;
    }

    /* Check if TX Buffer is empty */
    if(PLIB_USART_TransmitterIsEmpty(${PLIB_INSTANCE}))
    {
        result|= DRV_USART_TRANSFER_STATUS_TRANSMIT_EMPTY;
    }

    /* Check if the TX buffer is full */
    if(PLIB_USART_TransmitterBufferIsFull(${PLIB_INSTANCE}))
    {
        result|= DRV_USART_TRANSFER_STATUS_TRANSMIT_FULL;
    }

    return(result);
}

DRV_USART_ERROR DRV_USART${DRV_INSTANCE}_ErrorGet(void)
{
    DRV_USART_ERROR error;
    error = gDrvUSART${DRV_INSTANCE}Obj.error;

    /* Clear the error before returning */
    gDrvUSART${DRV_INSTANCE}Obj.error = DRV_USART_ERROR_NONE;

    /* Return the error*/
    return(error);
}

// *****************************************************************************
// *****************************************************************************
// Section: Local functions
// *****************************************************************************
// *****************************************************************************

void _DRV_USART${DRV_INSTANCE}_BufferQueueRxTasks(void)
{
    DRV_USART_BUFFER_OBJ * bufferObj;
    DRV_USART_OBJ *dObj = (DRV_USART_OBJ*)NULL;
<#if CONFIG_DRV_USART_INTERRUPT_MODE == true || RX_DMA == true>
    bool status;
</#if>

    dObj = &gDrvUSART${DRV_INSTANCE}Obj;
    bufferObj = dObj->queueRead;
<#if CONFIG_USE_3RDPARTY_RTOS && CONFIG_DRV_USART_INTERRUPT_MODE == false>

    /* If this driver is configured for polled mode in an RTOS, the tasks
       routine would be called from another thread. We need to get the driver
       instance mutex before updating the queue. If the driver is configured for
       interrupt mode, then _DRV_USART_TAKE_MUTEX will compile to true */

    if(OSAL_MUTEX_Lock(&(dObj->mutexDriverInstance), OSAL_WAIT_FOREVER) == OSAL_RESULT_TRUE)
    {
        /* We were able to take the mutex */
    }
    else
    {
        /* The mutex acquisition timed out. Return with an
           invalid handle. This code will not execute
           if there is no RTOS. */
        return;
    }
</#if>
<#if RX_DMA == true>

    /* Using DMA */

    if(bufferObj != NULL)
    {
        /* If Data reception is completed or an data transfer
         * Abort has occurred */
        if(0 == bufferObj->size ||
           DRV_USART_DMA_TRANSFER_ABORT == bufferObj->size)
        {
            if((dObj->eventHandler != NULL) &&
                    (bufferObj->flags & DRV_USART_BUFFER_OBJ_FLAG_BUFFER_ADD))
            {
                dObj->interruptNestingCount ++;
                dObj->eventHandler(dObj->bufferEvent,
                        bufferObj->bufferHandle,
                        dObj->context);
                /* Decrement the nesting count */
                dObj->interruptNestingCount -- ;
            }

            dObj->queueRead = bufferObj->next;
            bufferObj->inUse = false;
            dObj->queueSizeCurrentRead --;

            /* Reset the next and the previous pointers */
            bufferObj->next = NULL;
            bufferObj->previous = NULL;

            /* While the first buffer was getting processed, if any
             * buffer requests were Added, Start the added buffer request
             * by adding the request to DMA now */
            if(dObj->queueRead != NULL)
            {
                size_t srcSize, cellSize;
                srcSize = 1;
                cellSize = 1;
                bufferObj = dObj->queueRead;

                /* Reset the current head's previous pointer */
                bufferObj->previous = NULL;

                SYS_DMA_ChannelTransferAdd(dObj->dmaChannelHandleRead,
                        PLIB_USART_ReceiverAddressGet(${PLIB_INSTANCE}),
                        srcSize, bufferObj->buffer, bufferObj->size,cellSize);
            }
        }

    }
    if(dObj->queueRead == NULL)
    {
        /* If the queue is empty, then disable the DMA Receive Interrupt */
        status = SYS_INT_SourceDisable(${RCV_DMA_INT});
        /* Ignore the warning */
        (void)status;
    }
<#if CONFIG_USE_3RDPARTY_RTOS>
    /* Release the mutex */
    _DRV_USART_RELEASE_MUTEX(&(dObj->mutexDriverInstance));
</#if>
<#else>

    /* Not using DMA */

    /* In this function, the driver checks if there are any buffers in queue. If
    so the buffer is serviced. A buffer that is serviced completely is
    removed from the queue. Start by getting the buffer at the head of the
    queue */
    if(bufferObj != NULL)
    {
        /* The USART driver is configured to generate an interrupt when the FIFO
           is not empty. Additionally the queue is not empty. Which means there
           is work to done in this routine. Read data from the FIFO till either
           the FIFO is empty or till we have read the requested number of bytes.
           */

        while((PLIB_USART_ReceiverDataIsAvailable(${PLIB_INSTANCE}))
                && (bufferObj->nCurrentBytes < bufferObj->size ))
        {
            bufferObj->buffer[bufferObj->nCurrentBytes] = PLIB_USART_ReceiverByteReceive(${PLIB_INSTANCE});
            bufferObj->nCurrentBytes ++;
        }

        /* Check if this buffer is done */

        if(bufferObj->nCurrentBytes >= bufferObj->size)
        {
            /* This means the buffer is completed. If there
               is a callback registered with client, then
               call it */

            if((dObj->eventHandler != NULL) && (bufferObj->flags & DRV_USART_BUFFER_OBJ_FLAG_BUFFER_ADD))
            {
                /* Call the event handler. We additionally increment the
                   interrupt nesting count which lets the driver functions
                   that are called from the event handler know that an
                   interrupt context is active.
                   */

                dObj->interruptNestingCount ++;

                dObj->eventHandler(DRV_USART_BUFFER_EVENT_COMPLETE,
                        bufferObj->bufferHandle,
                        dObj->context);

                dObj->interruptNestingCount --;
            }

            /* Get the next buffer in the queue and deallocate
               this buffer */

            dObj->queueRead = bufferObj->next;
            bufferObj->inUse = false;
            dObj->queueSizeCurrentRead --;

            /* Reset the next and the previous pointers */
            bufferObj->next = NULL;
            bufferObj->previous = NULL;

            /* Reset the current head's previous pointer */
            if (dObj->queueRead != NULL)
            {
                dObj->queueRead->previous = NULL;
            }

<#if CONFIG_DRV_USART_READ_WRITE_MODEL_SUPPORT ==true>
<#if CONFIG_USE_3RDPARTY_RTOS>
            if(bufferObj->flags & DRV_USART_BUFFER_OBJ_FLAG_READ_WRITE)
            {
                /* This means we should post the semaphore */
                _DRV_USART_SEM_POST(&(dObj->semReadDone));

            }
</#if>
</#if>
        }
    }
<#if CONFIG_DRV_USART_INTERRUPT_MODE == true>
    if(dObj->queueRead == NULL)
    {
        /* The queue is empty. We can disable the interrupt */
        status = SYS_INT_SourceDisable(${RCV_INT});
        /* Ignore the warning */
        (void)status;
    }
</#if>
<#if CONFIG_USE_3RDPARTY_RTOS>

    /* Release the mutex */
    _DRV_USART_RELEASE_MUTEX(&(dObj->mutexDriverInstance));
</#if>
</#if>
}

void _DRV_USART${DRV_INSTANCE}_BufferQueueTxTasks(void)
{
    DRV_USART_BUFFER_OBJ * bufferObj;
    DRV_USART_OBJ *dObj = (DRV_USART_OBJ*)NULL;
<#if CONFIG_DRV_USART_INTERRUPT_MODE == true || TX_DMA == true>
    bool status;
</#if>

    dObj = &gDrvUSART${DRV_INSTANCE}Obj;
    bufferObj = dObj->queueWrite;
<#if CONFIG_USE_3RDPARTY_RTOS && CONFIG_DRV_USART_INTERRUPT_MODE == false>

    /* If this driver is configured for polled mode in an RTOS, the tasks
       routine would be called from another thread. We need to get the driver
       instance mutex before updating the queue. If the driver is configured for
       interrupt mode, then _DRV_USART_TAKE_MUTEX will compile to true */

    if(DRV_USART_INTERRUPT_MODE == false)
    {
        if(OSAL_MUTEX_Lock(&(dObj->mutexDriverInstance), OSAL_WAIT_FOREVER))
        {
            /* We were able to take the mutex */
        }
        else
        {
            /* The mutex acquisition timed out. Return with an
               invalid handle. This code will not execute
               if there is no RTOS. */
            return;
        }
    }
</#if>
<#if TX_DMA == true>
    /* Uses DMA */

    if(bufferObj != NULL)
    {
        /* If Data transmission is completed or an data transfer
         * Abort has occurred */
        if(0==bufferObj->size ||
           DRV_USART_DMA_TRANSFER_ABORT == bufferObj->size)
        {
            if((dObj->eventHandler != NULL) &&
                    (bufferObj->flags & DRV_USART_BUFFER_OBJ_FLAG_BUFFER_ADD))
            {
                dObj->interruptNestingCount ++;
                dObj->eventHandler(dObj->bufferEvent,
                        bufferObj->bufferHandle,
                        dObj->context);
                /* Decrement the nesting count */
                dObj->interruptNestingCount -- ;
            }

            dObj->queueWrite = bufferObj->next;
            bufferObj->inUse = false;
            dObj->queueSizeCurrentWrite --;

            /* Reset the next and the previous pointers */
            bufferObj->next = NULL;
            bufferObj->previous = NULL;

            /* While the first buffer was getting processed, if any
             * buffer requests were Added, Start the added buffer request
             * by adding the request to DMA now */
            if(dObj->queueWrite!=NULL)
            {
                size_t destSize, cellSize;
                destSize = 1;
                cellSize = 1;

                bufferObj = dObj->queueWrite;
                /* Reset the current head's previous pointer */
                bufferObj->previous = NULL;

                SYS_DMA_ChannelTransferAdd(dObj->dmaChannelHandleWrite,
                        bufferObj->buffer, bufferObj->size,
                        PLIB_USART_TransmitterAddressGet(${PLIB_INSTANCE}),
                        destSize,cellSize);
            }
        }

    }

    if(dObj->queueWrite == NULL)
    {
        /* If the queue is empty, then disable the DMA Transmit Interrupt */
       // status = SYS_INT_SourceDisable(${XMIT_DMA_INT});
       /* Ignore the warning */
       (void)status;
    }
<#if CONFIG_USE_3RDPARTY_RTOS>
    /* Release the mutex */
    _DRV_USART_RELEASE_MUTEX(&(dObj->mutexDriverInstance));
</#if>
<#else>

    /* Not using DMA */
        
    if(bufferObj != NULL)
    {
        /* This means the queue is not empty. Check if this buffer is done */
        if(bufferObj->nCurrentBytes >= bufferObj->size)
        {
            /* This means the buffer is completed. If there
               is a callback registered with client, then
               call it */

            if((dObj->eventHandler != NULL) && (bufferObj->flags & DRV_USART_BUFFER_OBJ_FLAG_BUFFER_ADD))
            {
                /* Before calling the event handler, the interrupt nesting
                   counter is incremented. This will allow driver routine that
                   are called from the event handler to know the interrupt
                   nesting level. Events are only generated for buffers that
                   were submitted using the buffer add routine */

                dObj->interruptNestingCount ++;

                dObj->eventHandler(DRV_USART_BUFFER_EVENT_COMPLETE,
                        bufferObj->bufferHandle,
                        dObj->context);

                /* Decrement the nesting count */
                dObj->interruptNestingCount -- ;
            }

            /* Get the next buffer in the queue and deallocate
             * this buffer */

            dObj->queueWrite = bufferObj->next;
            bufferObj->inUse = false;
            dObj->queueSizeCurrentWrite --;

            /* Reset the next and the previous pointers */
            bufferObj->next = NULL;
            bufferObj->previous = NULL;

            /* Reset the current head's previous pointer */
            if (dObj->queueWrite != NULL)
            {
                dObj->queueWrite->previous = NULL;
            }

<#if CONFIG_DRV_USART_READ_WRITE_MODEL_SUPPORT==true && CONFIG_USE_3RDPARTY_RTOS==true>
            if(bufferObj->flags & DRV_USART_BUFFER_OBJ_FLAG_READ_WRITE)
            {
                /* This means we should post the semaphore */
                _DRV_USART_SEM_POST(&(dObj->semWriteDone));
            }
</#if>
        }
    }

    /* Check if the queue is still not empty and process
       the buffer */

    if(dObj->queueWrite != NULL)
    {
        bufferObj = dObj->queueWrite;

        /* Fill up the FIFO with data till the FIFO is full
           and we have data to send */
        while((!PLIB_USART_TransmitterBufferIsFull(${PLIB_INSTANCE}))
                && (bufferObj->nCurrentBytes < bufferObj->size ))
        {
            PLIB_USART_TransmitterByteSend(${PLIB_INSTANCE}, bufferObj->buffer[bufferObj->nCurrentBytes]);
            bufferObj->nCurrentBytes ++;
        }
    }
<#if CONFIG_DRV_USART_INTERRUPT_MODE == true>
    else
    {
        /* If the queue is empty, then disable the TX interrupt */
        status = SYS_INT_SourceDisable(${XMIT_INT});
        /* Ignore the warning */
        (void)status;
    }
</#if>
<#if CONFIG_USE_3RDPARTY_RTOS>

    /* Release the mutex */
    _DRV_USART_RELEASE_MUTEX(&(dObj->mutexDriverInstance));
</#if>
</#if>

}

void _DRV_USART${DRV_INSTANCE}_BufferQueueErrorTasks(void)
{
    DRV_USART_OBJ *dObj = (DRV_USART_OBJ*)NULL;
    DRV_USART_BUFFER_OBJ * bufferObj;
    bool mutexGrabbed  = true;
<#if RX_DMA == true>
    size_t srcSize = 1, cellSize = 1;
</#if>
<#if CONFIG_DRV_USART_INTERRUPT_MODE == true && RX_DMA == false>
    bool status = false;
</#if>

    dObj = &gDrvUSART${DRV_INSTANCE}Obj;
    
    /* USART driver will take care that TX erros( like overflow etc) are not 
     * going to occur at any time based on checks before write.
     * So, only RX errors are to be handled/reported */
    
    /* Get the RX buffer at the head */
    bufferObj = dObj->queueRead;
<#if CONFIG_USE_3RDPARTY_RTOS>

    /* If this driver is configured for polled mode in an RTOS, the tasks
     * routine would be called from another thread. We need to get the driver
     * instance mutex before updating the queue. If the driver is configured for
     * interrupt mode, then OSAL_MUTEX_Lock will compile to true */

    if(DRV_USART_INTERRUPT_MODE == false)
    {
        if(!OSAL_MUTEX_Lock(&(dObj->mutexDriverInstance), OSAL_WAIT_FOREVER))
        {
            /* The mutex acquisition timed out.
             * This code will not execute if there is no RTOS. */
            mutexGrabbed = false;
        }
    }
</#if>

    /* mutexGrabbed will always be true for non-RTOS case.
     * Will be false when mutex aquisition timed out in RTOS mode */
    if(true == mutexGrabbed)
    {
        if(bufferObj != NULL)
        {
            /* Get the USART errors */
            dObj->error = PLIB_USART_ErrorsGet(${PLIB_INSTANCE});
            
	        /* Clear error condition */
            _DRV_USART${DRV_INSTANCE}_ErrorConditionClear();
<#if RX_DMA == true>
 
            /* Abort the current DMA operation */
            SYS_DMA_ChannelForceAbort(dObj->dmaChannelHandleRead);
</#if>
 
            if(dObj->eventHandler != NULL)
            {
                /* Call the event handler with buffer event error state */
                dObj->eventHandler(DRV_USART_BUFFER_EVENT_ERROR,
                        bufferObj->bufferHandle,
                        dObj->context);
            }

            /* Get the next buffer in the queue and deallocate
             * this buffer */

            dObj->queueRead = bufferObj->next;
            bufferObj->inUse = false;
            dObj->queueSizeCurrentRead --;

            /* Reset the next and previous pointers */
            bufferObj->next = NULL;
            bufferObj->previous = NULL;

            /* If queue head is not null, update buffer pointer parameters*/
            if (dObj->queueRead != NULL)
            {
                /* Reset the updated head's previous pointer */
                dObj->queueRead->previous = NULL;
<#if RX_DMA == true>
 
                /* Add next buffer to the DMA channel*/
                SYS_DMA_ChannelTransferAdd(dObj->dmaChannelHandleRead,
                        PLIB_USART_ReceiverAddressGet(${PLIB_INSTANCE}),
                        srcSize,dObj->queueRead->buffer, dObj->queueRead->size,
                        cellSize);
</#if>
            }
<#if CONFIG_DRV_USART_INTERRUPT_MODE == true && RX_DMA == false>
            else
            {
                /* Queue is empty disable the RX interrupt */
                status = SYS_INT_SourceDisable(${RCV_INT});
                    
                /* Ignore the warning */
                (void)status;
            }
</#if>
        }
        else
        {
            /* There is no buffer in the queue.
             * Flush the RX to clear the error condition */
            _DRV_USART${DRV_INSTANCE}_ErrorConditionClear();
        }
    }
<#if CONFIG_USE_3RDPARTY_RTOS>

    /* Release the mutex in RTOS environment*/
    _DRV_USART_RELEASE_MUTEX(&(dObj->mutexDriverInstance));
</#if>
}

void _DRV_USART${DRV_INSTANCE}_ErrorConditionClear()
{
    uint8_t dummyData = 0u;
    /* RX length = (FIFO level + RX register) */
    uint8_t RXlength = _DRV_USART_RX_DEPTH;
        
    /* If it's a overrun error then clear it to flush FIFO */
    if(USART_ERROR_RECEIVER_OVERRUN & PLIB_USART_ErrorsGet(${PLIB_INSTANCE}))
    {
        PLIB_USART_ReceiverOverrunErrorClear(${PLIB_INSTANCE});
    }

    /* Read existing error bytes from FIFO to clear parity and framing error flags*/
    while( (USART_ERROR_PARITY | USART_ERROR_FRAMING) & PLIB_USART_ErrorsGet(${PLIB_INSTANCE}) )
    {
        dummyData = PLIB_USART_ReceiverByteReceive(${PLIB_INSTANCE});
        RXlength--;
        
        /* Try to flush error bytes for one full FIFO and exit instead of 
         * blocking here if more error bytes are received*/
        if(0u == RXlength)
        {
            break;
        }
    }
    
    /* Ignore the warning */
    (void)dummyData;
    
    /* Clear error interrupt flag */
    SYS_INT_SourceStatusClear(${ERR_INT});
            
    /* Clear up the receive interrupt flag so that RX interrupt is not 
     * triggered for error bytes*/
    SYS_INT_SourceStatusClear(${RCV_INT});
}

DRV_USART_BAUD_SET_RESULT DRV_USART${DRV_INSTANCE}_BaudSet(uint32_t baud)
{
    uint32_t clockSource;
    int32_t brgValueLow=0;
    int32_t brgValueHigh=0;
    DRV_USART_BAUD_SET_RESULT retVal = DRV_USART_BAUD_SET_SUCCESS;
#if defined (PLIB_USART_ExistsModuleBusyStatus)
    bool isEnabled = false;
#endif

<#if CONFIG_USE_3RDPARTY_RTOS>
    /* Making this function thread safe */
    if(OSAL_MUTEX_Lock(&(gDrvUSART${DRV_INSTANCE}Obj.mutexDriverInstance), OSAL_WAIT_FOREVER) == OSAL_RESULT_TRUE)
    {
        /* We were able to take the mutex */
    }
    else
    {
        /* The mutex timed out */
        SYS_DEBUG_MESSAGE(SYS_ERROR_DEBUG, "\r\nUSART Driver: Hardware instance mutex time out in DRV_USART_BaudSet() function");
        return(DRV_USART_BAUD_SET_ERROR);
    }

</#if>
    /* Get the USART clock source value*/
    clockSource = SYS_CLK_PeripheralFrequencyGet ( CLK_BUS_PERIPHERAL_1 );

    /* Calculate low and high baud values */
    brgValueLow  = ( (clockSource/baud) >> 4 ) - 1;
    brgValueHigh = ( (clockSource/baud) >> 2 ) - 1;

#if defined (PLIB_USART_ExistsModuleBusyStatus)
        isEnabled = PLIB_USART_ModuleIsBusy (${PLIB_INSTANCE});
        if (isEnabled)
        {
            PLIB_USART_Disable (${PLIB_INSTANCE});
            while (PLIB_USART_ModuleIsBusy (${PLIB_INSTANCE}));
        }
#endif

    /* Check if the baud value can be set with high baud settings */
    if ((brgValueHigh >= 0) && (brgValueHigh <= UINT16_MAX))
    {
        PLIB_USART_BaudRateHighEnable(${PLIB_INSTANCE});
        PLIB_USART_BaudRateHighSet(${PLIB_INSTANCE},clockSource,baud);
    }
	
    /* Check if the baud value can be set with low baud settings */
    else if ((brgValueLow >= 0) && (brgValueLow <= UINT16_MAX))
    {
        PLIB_USART_BaudRateHighDisable(${PLIB_INSTANCE});
        PLIB_USART_BaudRateSet(${PLIB_INSTANCE}, clockSource, baud);
    }
    else
    {
        retVal = DRV_USART_BAUD_SET_ERROR;
    }
#if defined (PLIB_USART_ExistsModuleBusyStatus)
    if (isEnabled)
    {
        PLIB_USART_Enable (${PLIB_INSTANCE});
    }
#endif
<#if CONFIG_USE_3RDPARTY_RTOS>
    /* Unlock Mutex */
    OSAL_MUTEX_Unlock(&(gDrvUSART${DRV_INSTANCE}Obj.mutexDriverInstance));
</#if>

    return retVal;
}

DRV_USART_LINE_CONTROL_SET_RESULT DRV_USART${DRV_INSTANCE}_LineControlSet(DRV_USART_LINE_CONTROL lineControlMode)
{
#if defined (PLIB_USART_ExistsModuleBusyStatus)
    bool isEnabled = false;
#endif

<#if CONFIG_USE_3RDPARTY_RTOS>
    /* Making this function thread safe */
    if(OSAL_MUTEX_Lock(&(gDrvUSART${DRV_INSTANCE}Obj.mutexDriverInstance), OSAL_WAIT_FOREVER) == OSAL_RESULT_TRUE)
    {
        /* We were able to take the mutex */
    }
    else
    {
        SYS_DEBUG_MESSAGE(SYS_ERROR_DEBUG, "\r\nUSART Driver: Hardware Instance Mutex time out in DRV_USART_LineControlSet() function");
        return DRV_USART_LINE_CONTROL_SET_ERROR;
    }
</#if>
#if defined (PLIB_USART_ExistsModuleBusyStatus)
        isEnabled = PLIB_USART_ModuleIsBusy (${PLIB_INSTANCE});
        if (isEnabled)
        {
            PLIB_USART_Disable (${PLIB_INSTANCE});
            while (PLIB_USART_ModuleIsBusy (${PLIB_INSTANCE}));
        }
#endif

    /* Set the Line Control Mode */
    PLIB_USART_LineControlModeSelect(${PLIB_INSTANCE}, lineControlMode);

#if defined (PLIB_USART_ExistsModuleBusyStatus)
        if (isEnabled)
        {
            PLIB_USART_Enable (${PLIB_INSTANCE});
        }
#endif
<#if CONFIG_USE_3RDPARTY_RTOS>
    OSAL_MUTEX_Unlock(&(gDrvUSART${DRV_INSTANCE}Obj.mutexDriverInstance));

</#if>
    /* Return success */
    return(DRV_USART_LINE_CONTROL_SET_SUCCESS);
}

</#macro>
<#if CONFIG_DRV_USART_INST_IDX0 == true>
<@DRV_USART_STATIC_FUNCTIONS DRV_INSTANCE="0"
PLIB_INSTANCE=CONFIG_DRV_USART_PERIPHERAL_ID_IDX0
TX_DMA=CONFIG_DRV_USART_SUPPORT_TRANSMIT_DMA_IDX0
RX_DMA=CONFIG_DRV_USART_SUPPORT_RECEIVE_DMA_IDX0
HANDSHAKE_MODE=CONFIG_DRV_USART_HANDSHAKE_MODE_IDX0
LINE_CONTROL=CONFIG_DRV_USART_LINE_CNTRL_IDX0
BRG_RATE=CONFIG_DRV_USART_BAUD_RATE_IDX0
XMIT_INT=CONFIG_DRV_USART_XMIT_INT_SRC_IDX0
RCV_INT=CONFIG_DRV_USART_RCV_INT_SRC_IDX0
ERR_INT=CONFIG_DRV_USART_ERR_INT_SRC_IDX0
/>
</#if>
<#if CONFIG_DRV_USART_INST_IDX1 == true>
<@DRV_USART_STATIC_FUNCTIONS DRV_INSTANCE="1"
PLIB_INSTANCE=CONFIG_DRV_USART_PERIPHERAL_ID_IDX1
TX_DMA=CONFIG_DRV_USART_SUPPORT_TRANSMIT_DMA_IDX1
RX_DMA=CONFIG_DRV_USART_SUPPORT_RECEIVE_DMA_IDX1
HANDSHAKE_MODE=CONFIG_DRV_USART_HANDSHAKE_MODE_IDX1
LINE_CONTROL=CONFIG_DRV_USART_LINE_CNTRL_IDX1
BRG_RATE=CONFIG_DRV_USART_BAUD_RATE_IDX1
XMIT_INT=CONFIG_DRV_USART_XMIT_INT_SRC_IDX1
RCV_INT=CONFIG_DRV_USART_RCV_INT_SRC_IDX1
ERR_INT=CONFIG_DRV_USART_ERR_INT_SRC_IDX1
/>
</#if>
<#if CONFIG_DRV_USART_INST_IDX2 == true>
<@DRV_USART_STATIC_FUNCTIONS DRV_INSTANCE="2"
PLIB_INSTANCE=CONFIG_DRV_USART_PERIPHERAL_ID_IDX2
TX_DMA=CONFIG_DRV_USART_SUPPORT_TRANSMIT_DMA_IDX2
RX_DMA=CONFIG_DRV_USART_SUPPORT_RECEIVE_DMA_IDX2
HANDSHAKE_MODE=CONFIG_DRV_USART_HANDSHAKE_MODE_IDX2
LINE_CONTROL=CONFIG_DRV_USART_LINE_CNTRL_IDX2
BRG_RATE=CONFIG_DRV_USART_BAUD_RATE_IDX2
XMIT_INT=CONFIG_DRV_USART_XMIT_INT_SRC_IDX2
RCV_INT=CONFIG_DRV_USART_RCV_INT_SRC_IDX2
ERR_INT=CONFIG_DRV_USART_ERR_INT_SRC_IDX2
/>
</#if>
<#if CONFIG_DRV_USART_INST_IDX3 == true>
<@DRV_USART_STATIC_FUNCTIONS DRV_INSTANCE="3"
PLIB_INSTANCE=CONFIG_DRV_USART_PERIPHERAL_ID_IDX3
TX_DMA=CONFIG_DRV_USART_SUPPORT_TRANSMIT_DMA_IDX3
RX_DMA=CONFIG_DRV_USART_SUPPORT_RECEIVE_DMA_IDX3
HANDSHAKE_MODE=CONFIG_DRV_USART_HANDSHAKE_MODE_IDX3
LINE_CONTROL=CONFIG_DRV_USART_LINE_CNTRL_IDX3
BRG_RATE=CONFIG_DRV_USART_BAUD_RATE_IDX3
XMIT_INT=CONFIG_DRV_USART_XMIT_INT_SRC_IDX3
RCV_INT=CONFIG_DRV_USART_RCV_INT_SRC_IDX3
ERR_INT=CONFIG_DRV_USART_ERR_INT_SRC_IDX3
/>
</#if>
<#if CONFIG_DRV_USART_INST_IDX4 == true>
<@DRV_USART_STATIC_FUNCTIONS DRV_INSTANCE="4"
PLIB_INSTANCE=CONFIG_DRV_USART_PERIPHERAL_ID_IDX4
TX_DMA=CONFIG_DRV_USART_SUPPORT_TRANSMIT_DMA_IDX4
RX_DMA=CONFIG_DRV_USART_SUPPORT_RECEIVE_DMA_IDX4
HANDSHAKE_MODE=CONFIG_DRV_USART_HANDSHAKE_MODE_IDX4
LINE_CONTROL=CONFIG_DRV_USART_LINE_CNTRL_IDX4
BRG_RATE=CONFIG_DRV_USART_BAUD_RATE_IDX4
XMIT_INT=CONFIG_DRV_USART_XMIT_INT_SRC_IDX4
RCV_INT=CONFIG_DRV_USART_RCV_INT_SRC_IDX4
ERR_INT=CONFIG_DRV_USART_ERR_INT_SRC_IDX4
/>
</#if>
<#if CONFIG_DRV_USART_INST_IDX5 == true>
<@DRV_USART_STATIC_FUNCTIONS DRV_INSTANCE="5"
PLIB_INSTANCE=CONFIG_DRV_USART_PERIPHERAL_ID_IDX5
TX_DMA=CONFIG_DRV_USART_SUPPORT_TRANSMIT_DMA_IDX5
RX_DMA=CONFIG_DRV_USART_SUPPORT_RECEIVE_DMA_IDX5
HANDSHAKE_MODE=CONFIG_DRV_USART_HANDSHAKE_MODE_IDX5
LINE_CONTROL=CONFIG_DRV_USART_LINE_CNTRL_IDX5
BRG_RATE=CONFIG_DRV_USART_BAUD_RATE_IDX5
XMIT_INT=CONFIG_DRV_USART_XMIT_INT_SRC_IDX5
RCV_INT=CONFIG_DRV_USART_RCV_INT_SRC_IDX5
ERR_INT=CONFIG_DRV_USART_ERR_INT_SRC_IDX5
/>
</#if>
/*******************************************************************************
 End of File
*/
