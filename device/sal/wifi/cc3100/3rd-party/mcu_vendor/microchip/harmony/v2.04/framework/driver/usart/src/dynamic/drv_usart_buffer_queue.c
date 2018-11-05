/*******************************************************************************
  USART Driver Dynamic implementation.

  Company:
    Microchip Technology Inc.

  File Name:
    drv_usart.c

  Summary:
    USART Driver Dynamic Implementation.

  Description:
    This file contains the Dynamic mode implementation of the USART driver
    buffer queue routines.
*******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2013 released Microchip Technology Inc.  All rights reserved.

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

/*************************************************************
 * Include files.
 ************************************************************/
#include "../drv_usart_local.h"

/**************************************************************
 * This is the hardware instance object array.
 **************************************************************/
extern DRV_USART_OBJ gDrvUSARTObj[DRV_USART_INSTANCES_NUMBER] ;

/**************************************************************
 * This is the client object array.
 **************************************************************/
extern DRV_USART_CLIENT_OBJ gDrvUSARTClientObj[DRV_USART_CLIENTS_NUMBER];

/**************************************************************
 * This is the array of USART Driver Buffet object.
 **************************************************************/
DRV_USART_BUFFER_OBJ gDrvUSARTBufferObj[DRV_USART_QUEUE_DEPTH_COMBINED];

/**************************************************************
 * This a global token counter used to generate unique
 * buffer handles
 *************************************************************/
static uint16_t gDrvUSARTTokenCount = 0;

// *****************************************************************************
// *****************************************************************************
// Section: USART Driver Buffer Queue Interface Implementations
// *****************************************************************************
// *****************************************************************************
// *****************************************************************************
/* Function:
    void DRV_USART_AddressedBufferAddWrite
    (
        const DRV_HANDLE hClient,
        DRV_USART_BUFFER_HANDLE * bufferHandle,
        uint8_t address,
        void * source,
        size_t nWords
    );

  Summary:
    Schedule a non-blocking addressed driver write operation.

  Description:
    This function schedules a non-blocking addressed write operation. The function returns
    with a valid buffer handle in the bufferHandle argument if the addressed write request
    was scheduled successfully.  The function adds the request to the hardware
    instance transmit queue and returns immediately. While the request is in the
    queue, the application buffer is owned by the driver and should not be
    modified.

  Remarks:
    The source buffer should be a 16-bit word aligned buffer.
    The 9th bit of the higher byte 16-bit buffer is used to indicate data/address.
 */

void DRV_USART_AddressedBufferAddWrite
(
    const DRV_HANDLE hClient,
    DRV_USART_BUFFER_HANDLE * bufferHandle,
    uint8_t address,
    void * source,
    size_t nWords
)
{
    DRV_USART_CLIENT_OBJ * clientObj;
    DRV_USART_OBJ * hDriver;
    bool interruptWasEnabled = false;
    DRV_USART_BUFFER_OBJ * bufferObj = NULL;
    DRV_USART_BUFFER_OBJ * iterator;
    uint32_t i;

    /* This function adds a buffer to the write queue */

    /* We first check the arguments and initialize the
       buffer handle */

    if (bufferHandle == NULL)
    {
        return;
    }

    *bufferHandle = DRV_USART_BUFFER_HANDLE_INVALID;

    /* Validate the driver handle */
    clientObj = _DRV_USART_DriverHandleValidate(hClient);
    if(clientObj == NULL)
    {
        SYS_ASSERT(false, "Invalid Driver Handle");
        return;
    }

    if((nWords == 0) || (NULL == source))
    {
        /* We either got an invalid source pointer or 0 bytes to transfer */
        SYS_ASSERT(false, "Invalid parameters");
        return;
    }

    hDriver = clientObj->hDriver;

    if(hDriver->queueSizeCurrentWrite >= hDriver->queueSizeWrite)
    {
        /* This means the queue is full. We cannot add
           this request */

        SYS_ASSERT(false, "Transmit Queue is full");
        return;
    }

    /* We will allow buffers to be added in the interrupt
       context of this USART driver. But we must make
       sure that if we are in interrupt, then we should
       not modify mutexes. */

    if(hDriver->interruptNestingCount == 0)
    {
        /* Grab a mutex. This is okay because we are not in an
           interrupt context */

        if(OSAL_MUTEX_Lock(&(hDriver->mutexDriverInstance), OSAL_WAIT_FOREVER) == OSAL_RESULT_TRUE)
        {
            /* We will disable interrupts so that the queue
               status does not get updated asynchronously.
               This code will always execute. */

            interruptWasEnabled = _DRV_USART_InterruptSourceDisable(hDriver->txInterruptSource);
        }
        else
        {
            /* The mutex acquisition timed out. Return with an
               invalid handle. This code will not execute
               if there is no RTOS. */
            return;
        }
    }

    /* Search the buffer pool for a free buffer object */
    for(i = 0 ; i < DRV_USART_QUEUE_DEPTH_COMBINED; i ++)
    {
        if(!gDrvUSARTBufferObj[i].inUse)
        {
            /* This means this object is free.
             * Configure the object and then
             * break */
            bufferObj = &gDrvUSARTBufferObj[i];
            bufferObj->size     = nWords;
            bufferObj->inUse    = true;
            bufferObj->address  = address;
            bufferObj->isAddressSent = false;
            bufferObj->buffer   = (uint8_t*)source;
            bufferObj->hClient  = clientObj;
            bufferObj->nCurrentBytes = 0;
            bufferObj->next = NULL;
            bufferObj->previous = NULL;
            bufferObj->currentState    = DRV_USART_BUFFER_IS_IN_WRITE_QUEUE;
            bufferObj->flags = (0 | DRV_USART_BUFFER_OBJ_FLAG_BUFFER_ADD);

            /* Assign a handle to this buffer. The buffer handle must be unique.
             * To do this, we construct the buffer handle out of the
             * gDrvUSARTTokenCount and allocated buffer index. Note that
             * gDrvUSARTTokenCount is incremented and wrapped around when the
             * value reaches OxFFFF. We do avoid a case where the token value
             * becomes 0xFFFF and the buffer index also becomes 0xFFFF */

            bufferObj->bufferHandle = _DRV_USART_MAKE_HANDLE(gDrvUSARTTokenCount, i);
            *bufferHandle = bufferObj->bufferHandle;

            /* Update the token number. */
            _DRV_USART_UPDATE_BUFFER_TOKEN(gDrvUSARTTokenCount);
            break;
        }
    }

    if(i == DRV_USART_QUEUE_DEPTH_COMBINED)
    {
        /* This means we could not find a buffer. This
           will happen if the the DRV_USART_QUEUE_DEPTH_COMBINED
           parameter is configured to be less */

        SYS_ASSERT(false, "Insufficient Combined Queue Depth");

        /* Enable the interrupt if it was enabled */
        if(interruptWasEnabled)
        {
            _DRV_USART_InterruptSourceEnable(hDriver->txInterruptSource);
        }
        if(hDriver->interruptNestingCount == 0)
        {
            /* Release mutex */
            OSAL_MUTEX_Unlock(&(hDriver->mutexDriverInstance));
        }

        return;
    }

    /* Increment the current queue size*/
    hDriver->queueSizeCurrentWrite ++;

    /* Check if the queue is empty */
    if(hDriver->queueWrite == NULL)
    {
        /* This is the first buffer in the
           queue */

        hDriver->queueWrite = bufferObj;

        /* Because this is the first buffer in the queue, we need to send the
         first byte (In this case, the address of the client to communicate,
         so that we trigger the transmit interrupt generation. The
         rest of the buffer then gets processed in the task routine, which
         may or may not be called from the interrupt service routine. */

        if(false == bufferObj->isAddressSent)
        {
            PLIB_USART_Transmitter9BitsSend(hDriver->moduleId, bufferObj->address, true);
            bufferObj->isAddressSent = true;
        }

        /* If the driver is configured for interrupt mode, then the following
         statement should enable the interrupt. */

        _DRV_USART_InterruptSourceEnable(hDriver->txInterruptSource);
    }
    else
    {
        /* This means the write queue is not empty. We must add
         * the buffer object to the end of the queue */

        iterator = hDriver->queueWrite;
        while(iterator->next != NULL)
        {
            /* Get the next buffer object */
            iterator = iterator->next;
        }

        /* At this point, iterator will point to the
           last object in the queue. We add the buffer
           object to the linked list. Note that we
           need to set up the previous pointer as well
           because buffer should be deleted when the
           client closes the driver */

        iterator->next = bufferObj;
        bufferObj->previous = iterator;

        /* We are done. Restore the interrupt enable status
           and return. */

        if(interruptWasEnabled)
        {
            _DRV_USART_InterruptSourceEnable(hDriver->txInterruptSource);
        }

    }
    if(hDriver->interruptNestingCount == 0)
    {
        /* Release mutex */
        OSAL_MUTEX_Unlock(&(hDriver->mutexDriverInstance));
    }
    return;
}



// *****************************************************************************
/* Function:
    void DRV_USART_BufferAddWrite
    (
        DRV_HANDLE hClient,
        DRV_USART_BUFFER_HANDLE * bufferHandle,
        void * destination,
        size_t nBytes
    )

  Summary:
    Dynamic implementation of DRV_USART_BufferAddWrite client interface
    function.

  Description:
    This is the dynamic implementation of DRV_USART_BufferAddWrite
    client interface function.

  Remarks:
    See drv_usart.h for usage information.
*/

void DRV_USART_BufferAddWrite
(
    DRV_HANDLE hClient,
    DRV_USART_BUFFER_HANDLE * bufferHandle,
    void * source,
    size_t nBytes
)
{
    DRV_USART_CLIENT_OBJ * clientObj;
    DRV_USART_OBJ * hDriver;
    bool interruptWasEnabled = false;
    DRV_USART_BUFFER_OBJ * bufferObj = NULL;
    DRV_USART_BUFFER_OBJ * iterator;
    unsigned int i;

    /* This function adds a buffer to the write queue */

    /* We first check the arguments and initialize the
       buffer handle */

    if (bufferHandle == NULL)
    {
        return;
    }

    *bufferHandle = DRV_USART_BUFFER_HANDLE_INVALID;

    /* Validate the driver handle */
    clientObj = _DRV_USART_DriverHandleValidate(hClient);
    if(clientObj == NULL)
    {
        SYS_ASSERT(false, "Invalid Driver Handle");
        return;
    }

    if((nBytes == 0) || (NULL == source))
    {
        /* We either got an invalid client handle,
           invalid source pointer or 0 bytes to
           transfer */

        SYS_ASSERT(false, "Invalid parameters");
        return;
    }

    hDriver = clientObj->hDriver;

    if(hDriver->queueSizeCurrentWrite >= hDriver->queueSizeWrite)
    {
        /* This means the queue is full. We cannot add
           this request */

        SYS_ASSERT(false, "Transmit Queue is full");
        return;
    }

    /* We will allow buffers to be added in the interrupt
       context of this USART driver. But we must make
       sure that if we are in interrupt, then we should
       not modify mutexes. */

    if(hDriver->interruptNestingCount == 0)
    {
        /* Grab a mutex. This is okay because we are not in an
           interrupt context */

        if(OSAL_MUTEX_Lock(&(hDriver->mutexDriverInstance), OSAL_WAIT_FOREVER) == OSAL_RESULT_TRUE)
        {
            /* We will disable interrupts so that the queue
               status does not get updated asynchronously.
               This code will always execute. */

            interruptWasEnabled = _DRV_USART_InterruptSourceDisable(hDriver->txInterruptSource);
        }
        else
        {
            /* The mutex acquisition timed out. Return with an
               invalid handle. This code will not execute
               if there is no RTOS. */
            return;
        }
    }

    /* Search the buffer pool for a free buffer object */
    for(i = 0 ; i < DRV_USART_QUEUE_DEPTH_COMBINED; i ++)
    {
        if(!gDrvUSARTBufferObj[i].inUse)
        {
            /* This means this object is free.
             * Configure the object and then
             * break */
            bufferObj = &gDrvUSARTBufferObj[i];
            bufferObj->size     = nBytes;
            bufferObj->inUse    = true;
            bufferObj->buffer   = (uint8_t*)source;
            bufferObj->hClient  = clientObj;
            bufferObj->nCurrentBytes = 0;
            bufferObj->next = NULL;
            bufferObj->previous = NULL;
            bufferObj->currentState    = DRV_USART_BUFFER_IS_IN_WRITE_QUEUE;
            bufferObj->flags = (0 | DRV_USART_BUFFER_OBJ_FLAG_BUFFER_ADD);

            /* Assign a handle to this buffer. The buffer handle must be unique.
             * To do this, we construct the buffer handle out of the
             * gDrvUSARTTokenCount and allocated buffer index. Note that
             * gDrvUSARTTokenCount is incremented and wrapped around when the
             * value reaches OxFFFF. We do avoid a case where the token value
             * becomes 0xFFFF and the buffer index also becomes 0xFFFF */

            bufferObj->bufferHandle = _DRV_USART_MAKE_HANDLE(gDrvUSARTTokenCount, i);
            *bufferHandle = bufferObj->bufferHandle;

            /* Update the token number. */
            _DRV_USART_UPDATE_BUFFER_TOKEN(gDrvUSARTTokenCount);
            break;
        }
    }

    if(i == DRV_USART_QUEUE_DEPTH_COMBINED)
    {
        /* This means we could not find a buffer. This
           will happen if the the DRV_USART_QUEUE_DEPTH_COMBINED
           parameter is configured to be less */

        SYS_ASSERT(false, "Insufficient Combined Queue Depth");

        /* Enable the interrupt if it was enabled */
        if(interruptWasEnabled)
        {
            _DRV_USART_InterruptSourceEnable(hDriver->txInterruptSource);
        }
        if(hDriver->interruptNestingCount == 0)
        {
            /* Release mutex */
            OSAL_MUTEX_Unlock(&(hDriver->mutexDriverInstance));
        }

        return;
    }

    /* Increment the current queue size*/
    hDriver->queueSizeCurrentWrite ++;

    /* Check if the queue is empty */
    if(hDriver->queueWrite == NULL)
    {
        /* This is the first buffer in the
           queue */

        hDriver->queueWrite = bufferObj;

        /* Because this is the first buffer in the queue, we need to send the
         first byte so that we trigger the transmit interrupt generation. The
         rest of the buffer then gets processed in the task routine, which
         may or may not be called from the interrupt service routine. */

        /* Check if TX FIFO is full before sending a byte even though it is a
         first buffer in the queue, because FIFO may be full when using 
         Read/Write model along with the Buffer queue model */

        while(PLIB_USART_TransmitterBufferIsFull(hDriver->moduleId));
        bufferObj->nCurrentBytes ++;
        PLIB_USART_TransmitterByteSend(hDriver->moduleId, bufferObj->buffer[0]);

        /* If the driver is configured for interrupt mode, then the following
         statement should enable the interrupt. */

        _DRV_USART_InterruptSourceEnable(hDriver->txInterruptSource);
    }
    else
    {
        /* This means the write queue is not empty. We must add
         * the buffer object to the end of the queue */

        iterator = hDriver->queueWrite;
        while(iterator->next != NULL)
        {
            /* Get the next buffer object */
            iterator = iterator->next;
        }

        /* At this point, iterator will point to the
           last object in the queue. We add the buffer
           object to the linked list. Note that we
           need to set up the previous pointer as well
           because buffer should be deleted when the
           client closes the driver */

        iterator->next = bufferObj;
        bufferObj->previous = iterator;

        /* We are done. Restore the interrupt enable status
           and return. */

        if(interruptWasEnabled)
        {
            _DRV_USART_InterruptSourceEnable(hDriver->txInterruptSource);
        }

    }
    if(hDriver->interruptNestingCount == 0)
    {
        /* Release mutex */
        OSAL_MUTEX_Unlock(&(hDriver->mutexDriverInstance));
    }
    return;
}

// *****************************************************************************
/* Function:
    void DRV_USART_BufferAddRead
    (
        DRV_HANDLE hClient,
        DRV_USART_BUFFER_HANDLE * bufferHandle,
        void * destination,
        size_t nBytes
    )

  Summary:
    Dynamic implementation of DRV_USART_BufferAddRead client interface
    function.

  Description:
    This is the dynamic implementation of DRV_USART_BufferAddRead
    client interface function.

  Remarks:
    See drv_usart.h for usage information.
*/

void DRV_USART_BufferAddRead
(
    DRV_HANDLE hClient,
    DRV_USART_BUFFER_HANDLE * bufferHandle,
    void * destination,
    size_t nBytes
)
{
    DRV_USART_CLIENT_OBJ * clientObj;
    DRV_USART_OBJ * hDriver;
    bool interruptWasEnabled = false;
    DRV_USART_BUFFER_OBJ * bufferObj = NULL;
    DRV_USART_BUFFER_OBJ * iterator;
    unsigned int i;

    /* This function adds a buffer to the read queue */

    /* We first check the arguments and initialize the
       buffer handle */

    if(bufferHandle == NULL)
    {
        return;
    }

    *bufferHandle = DRV_USART_BUFFER_HANDLE_INVALID;

    clientObj = _DRV_USART_DriverHandleValidate(hClient);

    if(clientObj == NULL)
    {
        SYS_ASSERT(false, "Invalid driver handle");
        return;
    }

    if((nBytes == 0) || (NULL == destination))
    {
        /* We either got an invalid client handle,
           invalid destination pointer or 0 bytes to
           transfer */

        SYS_ASSERT(false, "Invalid parameters");
        return;
    }

    hDriver = clientObj->hDriver;

    if(hDriver->queueSizeCurrentRead >= hDriver->queueSizeRead)
    {
        /* This means the queue is full. We cannot add
         * this request */

        //SYS_ASSERT(false, "Receive Queue is full");
        return;
    }

    /* We will allow buffers to be added in the interrupt
     * context of this USART driver. But we must make
     * sure that if we are in interrupt, then we should
     * not modify mutexes. */

    if(hDriver->interruptNestingCount == 0)
    {
        /* Grab a mutex. This is okay because we are not in an
         * interrupt context */

        if(OSAL_MUTEX_Lock(&(hDriver->mutexDriverInstance), OSAL_WAIT_FOREVER) == OSAL_RESULT_TRUE)
        {
            /* We will disable interrupts so that the queue
             * status does not get updated asynchronously.
             * This code will always execute. */

            interruptWasEnabled = _DRV_USART_InterruptSourceDisable(hDriver->rxInterruptSource);
            
            /* Error interrupt is always enabled irrespective of the elements 
             * in the queue. So no need to check the enable status, but has to 
             * be disabled to make sure that DRV_USART_TasksError doesn't update 
             * the queue asynchronously */
            _DRV_USART_InterruptSourceDisable(hDriver->errorInterruptSource);
        }
        else
        {
            /* The mutex acquisition timed out. Return with an
             * invalid handle. This code will not execute
             * if there is no RTOS. */
            return;
        }
    }

    /* Search the buffer pool for a free buffer object */
    for(i = 0 ; i < DRV_USART_QUEUE_DEPTH_COMBINED; i ++)
    {
        if(!gDrvUSARTBufferObj[i].inUse)
        {
            /* This means this object is free.
             * Configure the object and then
             * break */
            bufferObj = &gDrvUSARTBufferObj[i];
            bufferObj->size     = nBytes;
            bufferObj->inUse    = true;
            bufferObj->buffer   = (uint8_t*)destination;
            bufferObj->hClient  = clientObj;
            bufferObj->next     = NULL;
            bufferObj->previous = NULL;
            bufferObj->nCurrentBytes = 0;
            bufferObj->currentState    = DRV_USART_BUFFER_IS_IN_READ_QUEUE;
            bufferObj->flags = (0 | DRV_USART_BUFFER_OBJ_FLAG_BUFFER_ADD);

            /* Assign a handle to this buffer. The buffer handle must be unique.
             * To do this, we construct the buffer handle out of the
             * gDrvUSARTTokenCount and allocated buffer index. Note that
             * gDrvUSARTTokenCount is incremented and wrapped around when the
             * value reaches OxFFFF. We do avoid a case where the token value
             * becomes 0xFFFF and the buffer index also becomes 0xFFFF */

            bufferObj->bufferHandle = _DRV_USART_MAKE_HANDLE(gDrvUSARTTokenCount, i);
            *bufferHandle = bufferObj->bufferHandle;

            /* Update the token number. */
            _DRV_USART_UPDATE_BUFFER_TOKEN(gDrvUSARTTokenCount);
            break;
        }
    }

    if(i == DRV_USART_QUEUE_DEPTH_COMBINED)
    {
        /* This means we could not find a buffer. This
           will happen if the the DRV_USART_QUEUE_DEPTH_COMBINED
           parameter is configured to be less */

        SYS_ASSERT(false, "Insufficient Combined Queue Depth");

        /* Enable the interrupt if it was enabled */
        if(interruptWasEnabled)
        {
            _DRV_USART_InterruptSourceEnable(hDriver->rxInterruptSource);
        }
        
        /* Restore Error interrupt always in the interrupt mode */
        _DRV_USART_InterruptSourceEnable(hDriver->errorInterruptSource);

        if(hDriver->interruptNestingCount == 0)
        {
            /* Release mutex */
            OSAL_MUTEX_Unlock(&(hDriver->mutexDriverInstance));
        }

        return;
    }

    /* Increment the current queue size*/
    hDriver->queueSizeCurrentRead ++;

    /* Check if the queue is empty */
    if(hDriver->queueRead == NULL)
    {
        /* This is the first buffer in the
           queue */

        hDriver->queueRead = bufferObj;

        /* This is the first item in the queue. Enable
           RX interrupt. */

        _DRV_USART_InterruptSourceEnable(hDriver->rxInterruptSource);

    }
    else
    {
        /* This means the read queue is not empty. We must add
           the buffer object to the end of the queue */

        iterator = hDriver->queueRead;
        while(iterator->next != NULL)
        {
            /* Get the next buffer object */
            iterator = iterator->next;
        }

        /* At this point, iterator will point to the
           last object in the queue. We add the buffer
           object to the linked list. Note that we
           need to set up the previous pointer as well
           because buffer should be deleted when the
           client closes the driver */

        iterator->next = bufferObj;
        bufferObj->previous = iterator;

        /* We are done. Restore the interrupt enable status
           and return. */

        if(interruptWasEnabled)
        {
            _DRV_USART_InterruptSourceEnable(hDriver->rxInterruptSource);
        }
        
        /* Restore Error interrupt always in the interrupt mode */
        _DRV_USART_InterruptSourceEnable(hDriver->errorInterruptSource);

    }
    if(hDriver->interruptNestingCount == 0)
    {
        /* Release mutex */
        OSAL_MUTEX_Unlock(&(hDriver->mutexDriverInstance));
    }
    return;
}

// *****************************************************************************
/* Function:
    void DRV_USART_BufferEventHandlerSet
    (
        const DRV_HANDLE hClient,
        const DRV_USART_BUFFER_EVENT_HANDLER eventHandler,
        const uintptr_t context
    )

  Summary:
    Dynamic implementation of DRV_USART_BufferEventHandlerSet client interface
    function.

  Description:
    This is the dynamic implementation of DRV_USART_BufferEventHandlerSet
    client interface function.

  Remarks:
    See drv_usart.h for usage information.
*/

void DRV_USART_BufferEventHandlerSet
(
    const DRV_HANDLE hClient,
    const DRV_USART_BUFFER_EVENT_HANDLER eventHandler,
    const uintptr_t context
)
{
    DRV_USART_CLIENT_OBJ * clientObj;

    /* Validate the driver handle */
    if((DRV_HANDLE_INVALID == hClient) || (0 == hClient))
    {
        /* This means the handle is invalid */
        SYS_ASSERT(false, "Driver Handle is invalid");
        return;
    }

    clientObj = (DRV_USART_CLIENT_OBJ *)hClient;

    if(!clientObj->inUse)
    {
        SYS_ASSERT(false, "Invalid driver handle");
        return;
    }

    /* Register the event handler with the client */
    clientObj->eventHandler = eventHandler;
    clientObj->context = context;
}

// *****************************************************************************
/* Function:
    size_t DRV_USART_BufferProcessedSizeGet(DRV_USART_BUFFER_HANDLE bufferHandle)

  Summary:
    This API will be deprecated.

  Description:
    None.

  Remarks:
    None.
*/

size_t DRV_USART_BufferProcessedSizeGet(DRV_USART_BUFFER_HANDLE bufferHandle)
{
    DRV_USART_BUFFER_OBJ * bufferObj;

    /* Validate the handle */
    if(DRV_USART_BUFFER_HANDLE_INVALID == bufferHandle)
    {
        return 0;
    }

    /* The buffer index is the contained in the lower 16 bits of the buffer
     * handle */
    bufferObj = &gDrvUSARTBufferObj[bufferHandle & 0xFFFF];
    if(!bufferObj->inUse)
    {
        return 0;
    }

    /* Return the processed number of bytes */
    return(bufferObj->nCurrentBytes);
}

// *****************************************************************************
/* Function:
    size_t DRV_USART_BufferCompletedBytesGet(DRV_USART_BUFFER_HANDLE bufferHandle)

  Summary:
    Dynamic implementation of DRV_USART_BufferCompletedBytesGet client interface
    function.

  Description:
    This is the dynamic implementation of DRV_USART_BufferCompletedBytesGet
    client interface function.

  Remarks:
    See drv_usart.h for usage information.
*/

size_t DRV_USART_BufferCompletedBytesGet(DRV_USART_BUFFER_HANDLE bufferHandle)
{
    DRV_USART_BUFFER_OBJ * bufferObj;
    size_t processedBytes = DRV_USART_BUFFER_HANDLE_INVALID;

    /* Validate the handle */
    if(DRV_USART_BUFFER_HANDLE_INVALID != bufferHandle)
    {
        /* The buffer index is the contained in the lower 16 bits 
         * of the buffer handle */
        bufferObj = &gDrvUSARTBufferObj[bufferHandle & 0xFFFF];
        
        /* Check if the buffer object is being used by any client */
        if(bufferObj->inUse)
        {
            /* Get the number of bytes processed. */
            processedBytes = bufferObj->nCurrentBytes;
        }
    }

    /* Return the processed number of bytes..
     * If the buffer handle is invalid or bufferObj is expired
     * then return DRV_USART_BUFFER_HANDLE_INVALID */
    return processedBytes;
}

// *****************************************************************************
/* Function:
    DRV_USART_BUFFER_RESULT DRV_USART_BufferRemove(DRV_USART_BUFFER_HANDLE bufferHandle)

  Summary:
    Dynamic implementation of DRV_USART_BufferRemove client interface
    function.

  Description:
    This is the dynamic implementation of DRV_USART_BufferRemove
    client interface function.

  Remarks:
    See drv_usart.h for usage information.
*/

DRV_USART_BUFFER_RESULT DRV_USART_BufferRemove(DRV_USART_BUFFER_HANDLE bufferHandle)
{
    DRV_USART_OBJ *hDriver;
    DRV_USART_BUFFER_OBJ * bufferObj;
    DRV_USART_CLIENT_OBJ * client;
    bool interruptWasEnabled = false;
    bool mutexGrabbed = true;
    DRV_USART_BUFFER_RESULT bufferResult = DRV_USART_BUFFER_RESULT_REMOVAL_FAILED;

    /* Validate the handle */
    if(DRV_USART_BUFFER_HANDLE_INVALID == bufferHandle)
    {
        bufferResult = DRV_USART_BUFFER_RESULT_HANDLE_INVALID;
    }
    else
    {
        /* The buffer index is the contained in the lower 16 bits of the buffer
         * handle */
        bufferObj = &gDrvUSARTBufferObj[bufferHandle & 0xFFFF];

        /* Get the driver object */
        client = (DRV_USART_CLIENT_OBJ *)bufferObj->hClient;
        hDriver = (DRV_USART_OBJ *)client->hDriver;

        /* We will allow modifications to buffer queue in the interrupt
         * context of this USART driver. But we must make
         * sure that if we are in interrupt, then we should
         * not modify mutexes. */

        if(hDriver->interruptNestingCount == 0)
        {
            /* Grab a mutex. This is okay because we are not in an
             * interrupt context */

            if(OSAL_MUTEX_Lock(&(hDriver->mutexDriverInstance), OSAL_WAIT_FOREVER) == OSAL_RESULT_TRUE)
            {
                /* We will disable interrupts so that the queue
                 * status does not get updated asynchronously.
                 * This code will always execute. */
                if( DRV_USART_BUFFER_IS_IN_WRITE_QUEUE == bufferObj->currentState)
                {
                    interruptWasEnabled = _DRV_USART_InterruptSourceDisable(hDriver->txInterruptSource);
                }
                else if( DRV_USART_BUFFER_IS_IN_READ_QUEUE == bufferObj->currentState)
                {
                    interruptWasEnabled = _DRV_USART_InterruptSourceDisable(hDriver->rxInterruptSource);
                }
                
                /* Error interrupt is always enabled irrespective of the 
                 * elements in the queue. So no need to check the enable status, 
                 * but has to be disabled to make sure that DRV_USART_TasksError 
                 * doesn't update the queue asynchronously */
                _DRV_USART_InterruptSourceDisable(hDriver->errorInterruptSource);
            }
            else
            {
                /* The mutex acquisition timed out.
                   This code will not execute if there is no RTOS. */
                mutexGrabbed = false;
            }
        }

        /* mutexGrabbed will always be true for non-RTOS case.
         * Will be false when mutex aquisition timed out in RTOS mode */
        if(true == mutexGrabbed)
        {
            /* Check if the buffer object is being used by any client */
            if(!bufferObj->inUse)
            {
                /* Buffer is not present in the queue */
                bufferResult = DRV_USART_BUFFER_RESULT_HANDLE_EXPIRED;
            }
            else
            {
                if( DRV_USART_BUFFER_IS_IN_WRITE_QUEUE == bufferObj->currentState)
                {

                    if(hDriver->queueWrite == bufferObj)
                    {
                        /* This is the first buffer in the
                           queue */

                        /* Get the next buffer in the write queue */
                         hDriver->queueWrite = bufferObj->next;

                        if (hDriver->queueWrite != NULL)
                        {
                            /* Reset the new head's previous pointer */
                            hDriver->queueWrite->previous = NULL;
                        }
                        else
                        {
                            /* The queue is empty. We can disable the interrupt */
                            _DRV_USART_InterruptSourceDisable(hDriver->txInterruptSource);
                        }
                    }
                    else
                    {
                        /* Buffer queue link updates */

                        /* Update previous buffer objects next pointer
                         * to the next buffer object of current buffer */
                        bufferObj->previous->next = bufferObj->next;

                        /* This may be the last buffer in the queue, if not
                         * then update the previous pointer of the next one */
                        if(NULL != bufferObj->next)
                        {
                            bufferObj->next->previous = bufferObj->previous;
                        }
                    }

                    /* Update the write queue size */
                    hDriver->queueSizeCurrentWrite --;
                }

                else if( DRV_USART_BUFFER_IS_IN_READ_QUEUE == bufferObj->currentState)
                {
                    if(hDriver->queueRead == bufferObj)
                    {
                        /* This is the first buffer in the
                           queue */

                        /* Get the next buffer in the read queue */
                         hDriver->queueRead = bufferObj->next;

                        if (hDriver->queueRead != NULL)
                        {
                            /* Reset the new head's previous pointer */
                            hDriver->queueRead->previous = NULL;
                        }
                    }
                    else
                    {
                        /* Buffer queue link updates */

                        /* Update previous buffer objects next pointer
                         * to the next buffer object of current buffer */
                        bufferObj->previous->next = bufferObj->next;

                        /* This may be the last buffer in the queue, if not
                         * then update the previous pointer of the next one */
                        if(NULL != bufferObj->next)
                        {
                            bufferObj->next->previous = bufferObj->previous;
                        }
                    }

                    /* Update the read queue size */
                    hDriver->queueSizeCurrentRead --;
                }

                /* Reset the current buffers flags and pointers */
                bufferObj->inUse = false;
                bufferObj->next = NULL;
                bufferObj->previous = NULL;
                bufferObj->currentState = DRV_USART_BUFFER_IS_FREE;

                if(interruptWasEnabled)
                {
                    if( DRV_USART_BUFFER_IS_IN_WRITE_QUEUE == bufferObj->currentState)
                    {
                        _DRV_USART_InterruptSourceEnable(hDriver->txInterruptSource);
                    }
                    else if( DRV_USART_BUFFER_IS_IN_READ_QUEUE == bufferObj->currentState)
                    {
                        _DRV_USART_InterruptSourceEnable(hDriver->rxInterruptSource);
                    }
                }
                
                /* Restore Error interrupt always in the interrupt mode */
                _DRV_USART_InterruptSourceEnable(hDriver->errorInterruptSource);

                /* Update the buffer processing result */
                bufferResult = DRV_USART_BUFFER_RESULT_REMOVED_SUCCESFULLY;

                if(hDriver->interruptNestingCount == 0)
                {
                    /* Release mutex */
                    OSAL_MUTEX_Unlock(&(hDriver->mutexDriverInstance));
                }

            }
        }
    }

    /* Return the buffer result */
    return bufferResult;
}

