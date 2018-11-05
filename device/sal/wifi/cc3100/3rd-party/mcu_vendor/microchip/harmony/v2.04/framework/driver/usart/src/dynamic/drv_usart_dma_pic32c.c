/********************************************************************************
  USART Driver Dynamic implementation with DMA support.

  Company:
    Microchip Technology Inc.

  File Name:
    drv_usart_dma_pic32c.c

  Summary:
    Source code for the USART Driver Dynamic implementation with DMA support.

  Description:
    This file contains the source code for the dynamic implementation of the
    USART driver supporting DMA mode(Using DMA channels for data transmission
    and reception).
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

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
#include "driver/usart/src/drv_usart_local_dma_pic32c.h"

// *****************************************************************************
// *****************************************************************************
// Section: Global Data
// *****************************************************************************
// *****************************************************************************

/* This is the driver instance object array. */
DRV_USART_OBJ gDrvUSARTObj[DRV_USART_INSTANCES_NUMBER] ;

/* This is the client object array. */
DRV_USART_CLIENT_OBJ gDrvUSARTClientObj[DRV_USART_CLIENTS_NUMBER];

/* This is the array of USART Driver Buffer objects. */
DRV_USART_BUFFER_OBJ gDrvUSARTBufferObj[DRV_USART_QUEUE_DEPTH_COMBINED];

/* This object maintains data that is required by all USART
   driver instances. */
DRV_USART_COMMON_DATA_OBJ gDrvUSARTCommonDataObj;

// *****************************************************************************
// *****************************************************************************
// Section: USART Driver Interface Implementations
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    SYS_MODULE_OBJ DRV_USART_Initialize
    (
        const SYS_MODULE_INDEX drvIndex,
        const SYS_MODULE_INIT * const init
    )

  Summary:
    Dynamic implementation of DRV_USART_Initialize system interface function.

  Description:
    This is the dynamic implementation of DRV_USART_Initialize system interface
    function.

  Remarks:
    See drv_usart.h for usage information.
*/

SYS_MODULE_OBJ DRV_USART_Initialize
(
    const SYS_MODULE_INDEX drvIndex,
    const SYS_MODULE_INIT * const init
)
{
    DRV_USART_OBJ *dObj = (DRV_USART_OBJ*)NULL;
    DRV_USART_INIT *usartInit = NULL ;

    /* Check if the specified driver index is in valid range */
    if(drvIndex >= DRV_USART_INSTANCES_NUMBER)
    {
        SYS_DEBUG(0, "Invalid driver index");
        return SYS_MODULE_OBJ_INVALID;
    }

    /* Check if this hardware instance was already initialized */
    if(gDrvUSARTObj[drvIndex].inUse != false)
    {
        SYS_DEBUG(0, "Instance already in use");
        return SYS_MODULE_OBJ_INVALID;
    }

    /* Assign to the local pointer the init data passed */
    usartInit = ( DRV_USART_INIT * ) init ;

    /* Allocate the driver object and set the operation flag to be in use */
    dObj = &gDrvUSARTObj[drvIndex];
    dObj->inUse = true;

    /* Update the USART Id and other parameters. */

    dObj->nClients              = 0;
    dObj->moduleId              = (usart_registers_t *)usartInit->usartID;
    dObj->brgClock              = usartInit->brgClock;
    dObj->isExclusive           = false;
    dObj->queueSizeRead         = usartInit->queueSizeReceive;
    dObj->queueSizeWrite        = usartInit->queueSizeTransmit;
    dObj->interruptUSART        = usartInit->interruptUSART;
    dObj->dmaChannelHandleWrite = SYS_DMA_CHANNEL_HANDLE_INVALID;
    dObj->dmaChannelHandleRead  = SYS_DMA_CHANNEL_HANDLE_INVALID;
    dObj->interruptDMA          = usartInit->interruptDMA;
    dObj->interruptNestingCount = 0;
    dObj->queueSizeCurrentRead  = 0;
    dObj->queueSizeCurrentWrite = 0;
    dObj->queueRead             = NULL;
    dObj->queueWrite            = NULL;
    dObj->operationMode         = usartInit->mode;
    
    /* TX DMA channel setup */
    if( DMA_CHANNEL_NONE != usartInit->dmaChannelTransmit )
    {
        dObj->dmaChannelHandleWrite = SYS_DMA_ChannelAllocate(usartInit->dmaChannelTransmit);
        if(SYS_DMA_CHANNEL_HANDLE_INVALID != dObj->dmaChannelHandleWrite)
        {
            /* Setting the channel priority same as the channel number */
            SYS_DMA_ChannelSetup(dObj->dmaChannelHandleWrite,
                SYS_DMA_CHANNEL_OP_MODE_BASIC, usartInit->dmaChannelTransmitTrigger);
        }
    }

    /* RX DMA channel setup */
    if( DMA_CHANNEL_NONE != usartInit->dmaChannelReceive )
    {
        dObj->dmaChannelHandleRead = SYS_DMA_ChannelAllocate(usartInit->dmaChannelReceive);
        if(SYS_DMA_CHANNEL_HANDLE_INVALID != dObj->dmaChannelHandleRead)
        {
            /* Setting the channel priority same as the channel number */
            SYS_DMA_ChannelSetup(dObj->dmaChannelHandleRead,
                SYS_DMA_CHANNEL_OP_MODE_BASIC, usartInit->dmaChannelReceiveTrigger);
        }
    }

    /* Setup the Hardware */
    _DRV_USART_HardwareSetup(dObj->moduleId, usartInit ) ;

    /* Create the hardware instance mutex. */
     if(OSAL_MUTEX_Create(&(dObj->mutexDriverInstance)) != OSAL_RESULT_TRUE)
     {
         return SYS_MODULE_OBJ_INVALID;
     }

    /* Check if the global mutexes have been created. If not
       then create these. */

     if(!gDrvUSARTCommonDataObj.membersAreInitialized)
     {
         /* This means that mutexes where not created. Create them. */
        if(OSAL_MUTEX_Create(&(gDrvUSARTCommonDataObj.mutexClientObjects)) != OSAL_RESULT_TRUE)
        {
             return SYS_MODULE_OBJ_INVALID;
        }
        if(OSAL_MUTEX_Create(&(gDrvUSARTCommonDataObj.mutexBufferQueueObjects)) != OSAL_RESULT_TRUE)
        {
             return SYS_MODULE_OBJ_INVALID;
        }
         /* Set this flag so that global mutexes get allocated only once */
         gDrvUSARTCommonDataObj.membersAreInitialized = true;
     }

    /* Enable the system interrupt flag */
    _DRV_USART_InterruptEnable(dObj->interruptUSART);
     
    /* Enable the RX and TX*/
    dObj->moduleId->US_CR.w |= (US_CR_RXEN_Msk | US_CR_TXEN_Msk);

    /* Update the status */
    dObj->status = SYS_STATUS_READY;

    /* Return the object structure */
    return ( (SYS_MODULE_OBJ)drvIndex );
}

// *****************************************************************************
/* Function:
    void DRV_USART_Deinitialize( SYS_MODULE_OBJ object )

  Summary:
    Dynamic implementation of DRV_USART_Deinitialize system interface function.

  Description:
    This is the dynamic implementation of DRV_USART_Deinitialize system interface
    function.

  Remarks:
    See drv_usart.h for usage information.
*/

void  DRV_USART_Deinitialize(SYS_MODULE_OBJ object)
{
    DRV_USART_OBJ * dObj;
    DRV_USART_BUFFER_OBJ * iterator;

    /* Check that the object is valid */

    if(object == SYS_MODULE_OBJ_INVALID)
    {
        SYS_DEBUG(0, "Invalid system object handle" );
        return;
    }

    if(object >= DRV_USART_INSTANCES_NUMBER)
    {
        SYS_DEBUG(0, "Invalid system object handle" );
        return;
    }

    dObj = (DRV_USART_OBJ*) &gDrvUSARTObj[object];

    if(!dObj->inUse)
    {
        SYS_DEBUG(0, "Invalid system object handle");
        return;
    }

    /* The driver will not have clients when it is
       being deinitialized. So the order in which
       we do the following steps is not that important */

    /* Indicate that this object is not is use */
    dObj->inUse = false;

    /* Deinitialize the USART status */
    dObj->status =  SYS_STATUS_UNINITIALIZED ;

    /* Disable the system interrupt */
    _DRV_USART_InterruptDisable(dObj->interruptUSART) ;
    
    /* Disable all module level sources for interrupt */
    _DRV_USART_InterruptDisable(dObj->interruptUSART) ;

    /* Disable USART RX and TX */
    dObj->moduleId->US_CR.w |= (US_CR_RXDIS_Msk | US_CR_TXDIS_Msk);

    /* Deallocate the allocated channel handles  */
    if(SYS_DMA_CHANNEL_HANDLE_INVALID != dObj->dmaChannelHandleWrite)
    {
        SYS_DMA_ChannelRelease(dObj->dmaChannelHandleWrite);
    }
    if(SYS_DMA_CHANNEL_HANDLE_INVALID != dObj->dmaChannelHandleRead)
    {
        SYS_DMA_ChannelRelease(dObj->dmaChannelHandleRead);
    }

    /* Deallocate all mutexes */
    if(OSAL_MUTEX_Delete(&(dObj->mutexDriverInstance)) != OSAL_RESULT_TRUE)
    {
        SYS_DEBUG(0, "Mutex Delete Failed");
        return;
    }
    /* Remove all objects from the read and write queue */

    iterator = dObj->queueWrite;
    while(iterator != NULL)
    {
        /* Return the buffer object to the pool */
        iterator->inUse = false;
        iterator = iterator->next;
    }

    iterator = dObj->queueRead;
    while(iterator != NULL)
    {
        /* Return the buffer object to the pool */
        iterator->inUse = false;
        iterator = iterator->next;
    }
}

// *****************************************************************************
/* Function:
    SYS_STATUS DRV_USART_Status( SYS_MODULE_OBJ object )

  Summary:
    Dynamic implementation of DRV_USART_Status system interface function.

  Description:
    This is the dynamic implementation of DRV_USART_Status system interface
    function.

  Remarks:
    See drv_usart.h for usage information.
*/

SYS_STATUS DRV_USART_Status( SYS_MODULE_OBJ object)
{
    /* Check if we have a valid object */
    if(object == SYS_MODULE_OBJ_INVALID)
    {
        SYS_DEBUG(0, "Invalid system object handle");
        return(SYS_STATUS_UNINITIALIZED);
    }

    if(object > DRV_USART_INSTANCES_NUMBER)
    {
        SYS_DEBUG(0, "Invalid system object handle");
        return(SYS_STATUS_UNINITIALIZED);
    }

    /* Return the system status of the hardware instance object */
    return (gDrvUSARTObj[object].status);
}

// *****************************************************************************
/* Function:
    DRV_HANDLE DRV_USART_Open( const SYS_MODULE_INDEX index,
                               const DRV_IO_INTENT    ioIntent )

  Summary:
    Dynamic implementation of DRV_USART_Open client interface function.

  Description:
    This is the dynamic implementation of DRV_USART_Open client interface
    function.

  Remarks:
    See drv_usart.h for usage information.
*/

DRV_HANDLE DRV_USART_Open
(
    const SYS_MODULE_INDEX drvIndex,
    const DRV_IO_INTENT ioIntent
)
{
    DRV_USART_CLIENT_OBJ *clientObj;
    DRV_USART_OBJ *dObj;
    unsigned int iClient;

    if (drvIndex >= DRV_USART_INSTANCES_NUMBER)
    {
        /* Invalid driver index */
        SYS_DEBUG(0, "Invalid Driver Instance");
        return (DRV_HANDLE_INVALID);
    }

    dObj = &gDrvUSARTObj[drvIndex];

    if((dObj->status != SYS_STATUS_READY) || (dObj->inUse == false))
    {
        /* The USART module should be ready */

        SYS_DEBUG(0, "Was the driver initialized?");
        return DRV_HANDLE_INVALID;
    }

    if(dObj->isExclusive)
    {
        /* This means the another client has opened the driver in exclusive
           mode. The driver cannot be opened again */

        SYS_DEBUG(0, "Driver already opened exclusively");
        return ( DRV_HANDLE_INVALID ) ;
    }

    if((dObj->nClients > 0) && (ioIntent & DRV_IO_INTENT_EXCLUSIVE))
    {
        /* This means the driver was already opened and another driver was
           trying to open it exclusively.  We cannot give exclusive access in
           this case */

        SYS_DEBUG(0, "Driver already opened. Cannot be opened exclusively");
        return(DRV_HANDLE_INVALID);
    }

    /* Grab client object mutex here */

    if(OSAL_MUTEX_Lock(&(gDrvUSARTCommonDataObj.mutexClientObjects), OSAL_WAIT_FOREVER) == OSAL_RESULT_TRUE)
    {
        /* Enter here only if the lock was obtained (applicable in
           RTOS only). If the mutex lock fails due to time out then
           this code does not get executed */

        for(iClient = 0; iClient != DRV_USART_CLIENTS_NUMBER; iClient ++)
        {
            if(!gDrvUSARTClientObj[iClient].inUse)
            {
                /* This means we have a free client object to use */
                clientObj = &gDrvUSARTClientObj[iClient];
                clientObj->inUse        = true;

                /* We have found a client object. Release the mutex */

                OSAL_MUTEX_Unlock(&(gDrvUSARTCommonDataObj.mutexClientObjects));

                clientObj->hDriver      = dObj;

                /* In a case where the driver is configured for polled
                   and bare metal operation, it will not support blocking operation */

                clientObj->ioIntent     = (ioIntent | _DRV_USART_ALWAYS_NON_BLOCKING);
                clientObj->eventHandler = NULL;
                clientObj->context      = (uintptr_t)NULL;
                clientObj->error        = DRV_USART_ERROR_NONE;

                if(ioIntent & DRV_IO_INTENT_EXCLUSIVE)
                {
                    /* Set the driver exclusive flag */
                    dObj->isExclusive = true;
                }

                dObj->nClients ++;

                /* Create the semaphores */
                if(OSAL_SEM_Create(&(clientObj->semReadDone), OSAL_SEM_TYPE_COUNTING, 1, 0) != OSAL_RESULT_TRUE)
                {
                        SYS_DEBUG(0, "Semaphore creation failed");
                        return(DRV_HANDLE_INVALID);
                }
                if(OSAL_SEM_Create(&(clientObj->semWriteDone), OSAL_SEM_TYPE_COUNTING, 1, 0) != OSAL_RESULT_TRUE)
                {
                        SYS_DEBUG(0, "Semaphore creation failed");
                        return(DRV_HANDLE_INVALID);
                }
                /* Update the client status */
                clientObj->status = DRV_USART_CLIENT_STATUS_READY;
                return ((DRV_HANDLE) clientObj );
            }
        }

        /* Could not find a client object. Release the mutex and
           return with an invalid handle. */
        OSAL_MUTEX_Unlock(&(gDrvUSARTCommonDataObj.mutexClientObjects));
    }

    /* If we have reached here, it means either we could not find a spare
       client object or the mutex timed out in a RTOS environment. */

    return DRV_HANDLE_INVALID;
}

// *****************************************************************************
/* Function:
    DRV_CLIENT_STATUS DRV_USART_Close ( DRV_HANDLE handle)

  Summary:
    Dynamic implementation of DRV_USART_Close client interface function.

  Description:
    This is the dynamic implementation of DRV_USART_Close client interface
    function.

  Remarks:
    See drv_usart.h for usage information.
*/

// *****************************************************************************
/* Function:
    DRV_USART_CLIENT_STATUS DRV_USART_ClientStatus (DRV_HANDLE handle )

  Summary:
    Dynamic implementation of DRV_USART_ClientStatus client interface function.

  Description:
    This is the dynamic implementation of DRV_USART_ClientStatus client interface
    function.

  Remarks:
    See drv_usart.h for usage information.
*/

DRV_USART_CLIENT_STATUS DRV_USART_ClientStatus(DRV_HANDLE handle)
{
    DRV_USART_CLIENT_OBJ * client;

    /* Validate the driver handle */
    client = _DRV_USART_DriverHandleValidate(handle);

    if(client == NULL)
    {
        /* Driver handle is not valid */

        SYS_DEBUG(0, "Invalid driver handle");
        return DRV_USART_CLIENT_STATUS_CLOSED;
    }

    /* Return the client status */
    return(client->status);
}

void DRV_USART_Close ( DRV_HANDLE handle)
{
    /* This function closes the client, The client
       object is deallocated and returned to the
       pool. */

    DRV_USART_CLIENT_OBJ * clientObj;
    DRV_USART_OBJ * dObj;

    /* Validate the handle */
    clientObj = _DRV_USART_DriverHandleValidate(handle);

    if(clientObj == NULL)
    {
        /* Driver handle is not valid */
        SYS_DEBUG(0, "Invalid Driver Handle");
        return;
    }

    dObj = (DRV_USART_OBJ *)clientObj->hDriver;

    /* Remove all buffers that this client owns from the driver queue. This
       function will map to _DRV_USART_ClientBufferQueueObjectsRemove() if the
       driver was built for buffer queue support. Else this condition always
       maps to true. */

    if(!_DRV_USART_CLIENT_BUFFER_QUEUE_OBJECTS_REMOVE(clientObj))
    {
        /* The function could fail if the mutex time out occurred */
        SYS_DEBUG(0, "Could not remove client buffer objects");
        clientObj->status = DRV_USART_CLIENT_STATUS_ERROR;
        return;
    }

    /* Deallocate all semaphores */
    if(OSAL_SEM_Delete(&(clientObj->semWriteDone)) != OSAL_RESULT_TRUE)
    {
        SYS_DEBUG(0, "Unable to delete client write done semaphore");
        clientObj->status = DRV_USART_CLIENT_STATUS_ERROR;
        return;
    }
    if(OSAL_SEM_Delete(&(clientObj->semReadDone)) != OSAL_RESULT_TRUE)
    {
        SYS_DEBUG(0, "Unable to delete client read done semaphore");
        clientObj->status = DRV_USART_CLIENT_STATUS_ERROR;
        return;
    }
    /* Reduce the number of clients */
    dObj->nClients --;

    /* Reset the exclusive flag */
    dObj->isExclusive = false;

    /* De-allocate the object */
    clientObj->status = DRV_USART_CLIENT_STATUS_CLOSED;
    clientObj->inUse = false;

    return;
}



// *****************************************************************************
/* Function:
    void DRV_USART_TasksTransmit (SYS_MODULE_OBJ object )

  Summary:
    Dynamic implementation of DRV_USART_TasksTransmit system interface function.

  Description:
    This is the dynamic implementation of DRV_USART_TasksTransmit system interface
    function.

  Remarks:
    See drv_usart.h for usage information.
*/

void DRV_USART_TasksTransmit(SYS_MODULE_OBJ object)
{
    /* This is the USART Driver Transmit tasks routine.
       In this function, the driver checks if a transmit
       interrupt is active and if there are any buffers in
       queue. If so the buffer is serviced. A buffer that
       is serviced completely is removed from the queue.
     */

    DRV_USART_OBJ * hDriver = &gDrvUSARTObj[object];

    if((!hDriver->inUse) || (hDriver->status != SYS_STATUS_READY))
    {
        /* This instance of the driver is not initialized. Don't
         * do anything */
        return;
    }

    /* Execute transmit task */
    _DRV_USART_TRANSMIT_BUFFER_QUEUE_TASKS(object);
}


// *****************************************************************************
/* Function:
    void DRV_USART_TasksError (SYS_MODULE_OBJ object );

  Summary:
    Maintains the driver's error state machine and implements its ISR

  Description:
    This routine is used to maintain the driver's internal error state machine
    and implement its error ISR for interrupt-driven implementations.  In
    polling mode, this function should be called from the SYS_Tasks function.
    In interrupt mode, this function should be called in the error interrupt
    service routine of the USART that is associated with this USART driver
    hardware instance.

  Remarks:
    Refer to drv_usart.h for usage information.
*/

void DRV_USART_TasksError(SYS_MODULE_OBJ object)
{
    /* This is the USART Driver Error tasks routine. In this function, the
     * driver checks if an error interrupt has occurred. If so the error
     * condition is cleared.  */

    DRV_USART_OBJ * hDriver = &gDrvUSARTObj[object];

    if((!hDriver->inUse) || (hDriver->status != SYS_STATUS_READY))
    {
        /* This instance of the driver is not initialized. Don't
         * do anything */
        return;
    }

    /* Process the overflow/framing/parity errors if any */
    if ( (1 == hDriver->moduleId->US_CSR.OVRE) || (1 == hDriver->moduleId->US_CSR.FRAME) || (1 == hDriver->moduleId->US_CSR.PARE) )
    {
        _DRV_USART_ERROR_TASKS(object);
    }
}

// *****************************************************************************
/* Function:
    void DRV_USART_TasksReceive (SYS_MODULE_OBJ object )

  Summary:
    Dynamic implementation of DRV_USART_Initialize system interface function.

  Description:
    This is the dynamic implementation of DRV_USART_Initialize system interface
    function.

  Remarks:
    See drv_usart.h for usage information.
*/

void DRV_USART_TasksReceive(SYS_MODULE_OBJ object)
{
    /* This is the USART Driver Receive tasks routine. If the receive
       interrupt flag is set, the tasks routines are executed.
     */

    DRV_USART_OBJ * hDriver = &gDrvUSARTObj[object];

    if((!hDriver->inUse) || (hDriver->status != SYS_STATUS_READY))
    {
        /* This instance of the driver is not initialized. Dont
           do anything */

        return;
    }

    /* Process the RX data if received */
    _DRV_USART_RECEIVE_BUFFER_QUEUE_TASKS(object);
}

// *****************************************************************************
/* Function:
    DRV_USART_TRANSFER_STATUS DRV_USART_TransferStatus(const DRV_HANDLE handle)

  Summary:
    Dynamic implementation of DRV_USART_TransferStatus client interface function.

  Description:
    This is the dynamic implementation of DRV_USART_TransferStatus client
    interface function.

  Remarks:
    See drv_usart.h for usage information.
*/

DRV_USART_TRANSFER_STATUS DRV_USART_TransferStatus(const DRV_HANDLE handle)
{
    DRV_USART_CLIENT_OBJ * client;
    DRV_USART_OBJ * hDriver;
    DRV_USART_TRANSFER_STATUS result = 0;

    client = _DRV_USART_DriverHandleValidate(handle);

    /* Validate the handle */
    if(client == NULL)
    {
        SYS_DEBUG(0, "Invalid Driver handle");
        return 0;
    }

    hDriver = client->hDriver;

    /* If the Rx interrupt flag is set then there is data available
       else there isn't */
    if(1 == hDriver->moduleId->US_CSR.RXRDY)
    {
        result |= DRV_USART_TRANSFER_STATUS_RECEIVER_DATA_PRESENT;
    }
    else
    {
        result |= DRV_USART_TRANSFER_STATUS_RECEIVER_EMPTY;
    }
    
    /* If the TX interrupt flag is set, then the Tx Buffer is empty */
    if(1 == hDriver->moduleId->US_CSR.TXEMPTY)
    {
        result |= DRV_USART_TRANSFER_STATUS_TRANSMIT_EMPTY;    
    }
    else
    {
        result |= DRV_USART_TRANSFER_STATUS_TRANSMIT_FULL; 
    }

    return(result);
}

// *****************************************************************************
/* Function:
    DRV_USART_ERROR DRV_USART_ErrorGet( DRV_HANDLE hClient )

  Summary:
    Dynamic implementation of DRV_USART_ErrorGet client interface function.

  Description:
    This is the dynamic implementation of DRV_USART_ErrorGet client interface
    function.

  Remarks:
    See drv_usart.h for usage information.
*/

DRV_USART_ERROR DRV_USART_ErrorGet(DRV_HANDLE hClient)
{
    DRV_USART_CLIENT_OBJ * clientObj;
    DRV_USART_ERROR error;

    /* Validate the handle */
    clientObj = _DRV_USART_DriverHandleValidate(hClient);

    if(clientObj == NULL)
    {
        /* Driver handle is not valid. Return error */
        SYS_DEBUG(0, "Invalid client handle");
        return DRV_USART_BAUD_SET_ERROR;
    }

    /* Return the error. Clear the error before
       returning. */

    error = clientObj->error;
    clientObj->error = DRV_USART_ERROR_NONE;
    return(error);
}

// *****************************************************************************
/* Function:
    DRV_USART_BAUD_SET_RESULT DRV_USART_BaudSet(DRV_HANDLE handle, uint32_t baud)

  Summary:
    Dynamic implementation of DRV_USART_BaudSet client interface function.

  Description:
    This is the dynamic implementation of DRV_USART_BaudSet client interface
    function.

  Remarks:
    See drv_usart.h for usage information.
*/

DRV_USART_BAUD_SET_RESULT DRV_USART_BaudSet(DRV_HANDLE handle, uint32_t baud)
{
    DRV_USART_OBJ * hDriver;
    DRV_USART_CLIENT_OBJ * client;
    DRV_USART_BAUD_SET_RESULT retVal = DRV_USART_BAUD_SET_SUCCESS;
    uint32_t cd;

    /* Validate the handle */
    client = _DRV_USART_DriverHandleValidate(handle);

    if(client == NULL)
    {
        /* Driver handle is not valid. Return error */
        SYS_DEBUG(0, "Invalid client handle");
        return DRV_USART_BAUD_SET_ERROR;
    }

    hDriver = (DRV_USART_OBJ*)client->hDriver;

    if(OSAL_MUTEX_Lock(&(hDriver->mutexDriverInstance), OSAL_WAIT_FOREVER) == OSAL_RESULT_TRUE)
    {
	    /* Calculate and set baud rate, 
	     * choose oversampling for RX pin based on requirement */
	    if (hDriver->brgClock >= (_DRV_USART_HIGH_FRQ_SAMPLE_DIV * baud) ) 
	    {
	        /* Disable over sampling*/
	        hDriver->moduleId->US_MR.w &= ~US_MR_OVER_Msk; 
	        cd = (hDriver->brgClock)/(_DRV_USART_HIGH_FRQ_SAMPLE_DIV * baud);
	    } 
	    else 
	    {
	        /* Enable over sampling*/
	        hDriver->moduleId->US_MR.w |= US_MR_OVER_Msk;
	        cd = (hDriver->brgClock)/(_DRV_USART_LOW_FRQ_SAMPLE_DIV * baud);
	    }

	    /* Configure the baudrate configure register. */
        hDriver->moduleId->US_BRGR.w = ( (hDriver->moduleId->US_BRGR.w & (~US_BRGR_CD_Msk)) | US_BRGR_CD(cd) );

        OSAL_MUTEX_Unlock(&(hDriver->mutexDriverInstance));
    }
    else
    {
        /* The mutex timed out */
        SYS_DEBUG(0, "Hardware instance mutex time out in DRV_USART_BaudSet() function");
        retVal = DRV_USART_BAUD_SET_ERROR;
    }

    return retVal;
}

// *****************************************************************************
/* Function:
    DRV_USART_LINE_CONTROL_SET_RESULT DRV_USART_LineControlSet
    (
        DRV_HANDLE handle,
        DRV_USART_LINE_CONTROL lineControlMode
    )

  Summary:
    Dynamic implementation of DRV_USART_LineControlSet client interface function.

  Description:
    This is the dynamic implementation of DRV_USART_LineControlSet client
    interface function.

  Remarks:
    See drv_usart.h for usage information.
*/

DRV_USART_LINE_CONTROL_SET_RESULT DRV_USART_LineControlSet
(
    DRV_HANDLE handle,
    DRV_USART_LINE_CONTROL lineControlMode
)
{
    DRV_USART_OBJ * hDriver;
    DRV_USART_CLIENT_OBJ * client;

    /* Validate the driver handle */
    client = _DRV_USART_DriverHandleValidate(handle);

    if(client == NULL)
    {
        /* Driver handle is not valid. Return error */

        SYS_DEBUG(0, "Invalid driver handle");
        return DRV_USART_LINE_CONTROL_SET_ERROR;
    }

    hDriver = (DRV_USART_OBJ*)client->hDriver;

    if(OSAL_MUTEX_Lock(&(hDriver->mutexDriverInstance), OSAL_WAIT_FOREVER) == OSAL_RESULT_TRUE)
    {
	    /* Set parity, stop bits and default 8 bit character mode */
	    hDriver->moduleId->US_MR.w = ( (hDriver->moduleId->US_MR.w & ~(US_MR_CHRL_Msk|US_MR_PAR_Msk|US_MR_NBSTOP_Msk)) | lineControlMode );
    
	    /* Enable 9 bit character length if 9 bit mode is selected */
	    if( !(lineControlMode & US_MR_CHRL_8_BIT) )
	    {
	        /* Enable 9 bit mode, otherwise by default 8 bit mode */
	        hDriver->moduleId->US_MR.w |= US_MR_MODE9_Msk;
	    }
        OSAL_MUTEX_Unlock(&(hDriver->mutexDriverInstance));
    }
    else
    {
        SYS_DEBUG(0, "Hardware Instance Mutex time out in DRV_USART_LineControlSet() function");
        return DRV_USART_LINE_CONTROL_SET_ERROR;
    }

    /* Return success */
    return(DRV_USART_LINE_CONTROL_SET_SUCCESS);
}

// *****************************************************************************
// *****************************************************************************
// Section: File scope functions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* void _DRV_USART_HardwareSetup
   (
        void *moduleId,
        DRV_USART_INIT * init
   )

  Summary:
    Initializes the USART module based on the init data structure.

  Description:
    This function initializes the USART module based on the init data structure.

  Remarks:
    This is a private function and should not be called directly by the
    application.
*/

void _DRV_USART_HardwareSetup(usart_registers_t *moduleId, DRV_USART_INIT * init)
{
    uint32_t cd;
    
    /* Reset USART mode register */
    moduleId->US_MR.w = 0;
    
    /* Disable the USART module to configure it */
    moduleId->US_CR.w |= (US_CR_RXDIS_Msk | US_CR_TXDIS_Msk);
    
    /* Reset USART status flags register */
    moduleId->US_CR.w |= US_CR_RSTSTA_Msk;

    /* By default it's normal mode */
    moduleId->US_MR.w |= US_MR_USART_MODE( US_MR_USART_MODE_NORMAL_Val );
    
    /* Check the operational mode and update the relevant fields */
    switch(init->mode)
    {
        case DRV_USART_OPERATION_MODE_ADDRESSED:
        {
            moduleId->US_MR.w |= US_MR_USART_MODE( US_MR_USART_MODE_RS485_Val);
            break;
        }
        case DRV_USART_OPERATION_MODE_HANDSHAKE:
        {
            moduleId->US_MR.w |= US_MR_USART_MODE( US_MR_USART_MODE_HW_HANDSHAKING_Val);
            break;
        }
        case DRV_USART_OPERATION_MODE_IRDA:
        {
            /* Have to set IRDA filter value here */
            moduleId->US_MR.w |= US_MR_USART_MODE( US_MR_USART_MODE_IRDA_Val);
            break;
        }
        case DRV_USART_OPERATION_MODE_LOOPBACK:
        {
            /* By default loopback  by shorting RX and TX pins internally.
             * Loopback in normal operation mode */
            moduleId->US_MR.w |= US_MR_CHMODE( US_MR_CHMODE_LOCAL_LOOPBACK_Val);
            break;
        }
        default:
            break;
    }

    /* Set the line control mode */
    
    /* Set parity, stop bits and default 8 bit character mode */
    moduleId->US_MR.w = ( (moduleId->US_MR.w & ~(US_MR_CHRL_Msk|US_MR_PAR_Msk|US_MR_NBSTOP_Msk)) | (init->lineControl) );
    
    /* Enable 9 bit character length if 9 bit mode is selected */
    if( !((init->lineControl) & US_MR_CHRL_8_BIT) )
    {
        /* Enable 9 bit mode, otherwise by default 8 bit mode */
        moduleId->US_MR.w |= US_MR_MODE9_Pos;
    }
    
    /* Select clock source.
     * By default it's peripheral clock(MCK) for now. */
    /* TODO: Implement clock source selection */
    moduleId->US_MR.w |= US_MR_USCLKS( US_MR_USCLKS_MCK_Val);
    
    /* Calculate and set baud rate, 
     * choose oversampling for RX pin based on requirement */
    if (init->brgClock >= (_DRV_USART_HIGH_FRQ_SAMPLE_DIV * (init->baud)) ) 
    {
        /* Disable over sampling*/
        moduleId->US_MR.w &= ~US_MR_OVER_Msk;
        cd = (init->brgClock)/(_DRV_USART_HIGH_FRQ_SAMPLE_DIV * init->baud);
    } 
    else 
    {
        /* Enable over sampling*/
        moduleId->US_MR.w |= US_MR_OVER_Msk;
        cd = (init->brgClock)/(_DRV_USART_LOW_FRQ_SAMPLE_DIV * init->baud);
    }

    /* Configure the baudrate configure register. */
    moduleId->US_BRGR.w = ( (moduleId->US_BRGR.w & (~US_BRGR_CD_Msk)) | US_BRGR_CD(cd) );
    
    /* Enable only error interrupt source as both Transmit and Receive
     * is handled by SYS_DMA */
    _DRV_USART_ErrorInterruptSourceEnable(moduleId);
}

// *****************************************************************************
/* Function:
    DRV_USART_CLIENT_OBJ * _DRV_USART_DriverHandleValidate(DRV_HANDLE handle)

  Summary:
    Dynamic implementation of the _DRV_USART_DriverHandleValidate() function.

  Description:
    Dynamic implementation of the _DRV_USART_DriverHandleValidate() function.
    This function return NULL if the handle is invalid else it return a pointer
    to the USART Driver Client Object associated with this handle.

  Remarks:
    This is a private function and should not be called directly by an
    application.
*/

DRV_USART_CLIENT_OBJ * _DRV_USART_DriverHandleValidate(DRV_HANDLE handle)
{
    /* This function returns the pointer to the client object that is
       associated with this handle if the handle is valid. Returns NULL
       otherwise. */

    DRV_USART_CLIENT_OBJ * client;

    if((DRV_HANDLE_INVALID == handle) ||
            (0 == handle))
    {
        return(NULL);
    }

    client = (DRV_USART_CLIENT_OBJ *)handle;

    if(!client->inUse)
    {
        return(NULL);
    }

    return(client);
}

// *****************************************************************************
// *****************************************************************************
// Section: File scope functions
// *****************************************************************************
// *****************************************************************************

bool _DRV_USART_ClientBufferQueueObjectsRemove(DRV_USART_CLIENT_OBJ * clientObj)
{
    DRV_USART_OBJ * dObj = clientObj->hDriver;
    DRV_USART_BUFFER_OBJ * iterator = NULL;

    if(OSAL_MUTEX_Lock(&(dObj->mutexDriverInstance), OSAL_WAIT_FOREVER) == OSAL_RESULT_TRUE)
    {
        /* Disable the USART interrupt */
        _DRV_USART_InterruptDisable(dObj->interruptUSART);
        
        iterator = dObj->queueWrite;
        while(iterator != NULL)
        {
            if(clientObj == (DRV_USART_CLIENT_OBJ *)iterator->hClient)
            {
                /* That means this buffer object is owned
                   by this client. This buffer object should
                   be removed. The following code removes
                   the object from a doubly linked list queue. */

                iterator->inUse = false;
                if(iterator->previous != NULL)
                {
                    iterator->previous->next = iterator->next;
                }
                if(iterator->next != NULL)
                {
                    iterator->next->previous = iterator->previous;
                }
                /* Decrementing Current queue size */
                dObj->queueSizeCurrentWrite --;

            }
            iterator = iterator->next;
        }
        /* If there are no buffers in the write queue.
         * Make the head pointer point to NULL */
        if(dObj->queueSizeCurrentWrite == 0)
        {
            dObj->queueWrite = NULL;
        }
        
        iterator = dObj->queueRead;
        while(iterator != NULL)
        {
            if(clientObj == (DRV_USART_CLIENT_OBJ *)iterator->hClient)
            {
                /* That means this buffer object is owned
                   by this client. This buffer object should
                   be removed. The following code removed
                   the object from a doubly linked list queue. */

                iterator->inUse = false;
                if(iterator->previous != NULL)
                {
                    iterator->previous->next = iterator->next;
                }
                if(iterator->next != NULL)
                {
                    iterator->next->previous = iterator->previous;
                }
                /* Decrementing Current queue size */
                dObj->queueSizeCurrentRead --;
            }
            iterator = iterator->next;
        }
        /* If there are no buffers in the read queue.
         * Make the head pointer point to NULL */
        if(dObj->queueSizeCurrentRead == 0)
        {
            dObj->queueRead = NULL;
        }

        /* Restore the interrupt */
        _DRV_USART_InterruptEnable(dObj->interruptUSART);

        /* Unlock the mutex */

        OSAL_MUTEX_Unlock(&(dObj->mutexDriverInstance));
    }
    else
    {
        /* The case where the mutex lock timed out and the
           client buffer objects could not be removed from
           the driver queue, the close function should fail. */

        return false;
    }

    return true;
}

void _DRV_USART_BufferQueueRxTasks(SYS_MODULE_OBJ object)
{
    DRV_USART_OBJ * hDriver = &gDrvUSARTObj[object];
    DRV_USART_BUFFER_OBJ * bufferObj;
    DRV_USART_CLIENT_OBJ * client;

    bufferObj = hDriver->queueRead;

    /* If this driver is configured for polled mode in an RTOS, the tasks
       routine would be called from another thread. We need to get the driver
       instance mutex before updating the queue. If the driver is configured for
       interrupt mode, then _DRV_USART_TAKE_MUTEX will compile to true */

    if(DRV_USART_INTERRUPT_MODE == false)
    {
        if(OSAL_MUTEX_Lock(&(hDriver->mutexDriverInstance), OSAL_WAIT_FOREVER) == OSAL_RESULT_TRUE)
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

    /* Checking whether the transfer uses DMA channels */
    if(SYS_DMA_CHANNEL_HANDLE_INVALID != hDriver->dmaChannelHandleRead)
    {
        if(bufferObj != NULL)
        {
            client = (DRV_USART_CLIENT_OBJ *)bufferObj->hClient;
            /* If Data reception is completed or an data transfer
             * Abort has occurred */
            if(0 == bufferObj->size || DRV_USART_DMA_TRANSFER_ABORT == bufferObj->size)
            {
                if((client->eventHandler != NULL) && (bufferObj->flags & DRV_USART_BUFFER_OBJ_FLAG_BUFFER_ADD))
                {
                    hDriver->interruptNestingCount ++;
                    client->eventHandler(client->bufferEvent,
                            bufferObj->bufferHandle,
                            client->context);
                    /* Decrement the nesting count */
                    hDriver->interruptNestingCount -- ;
                }

                hDriver->queueRead = bufferObj->next;
                bufferObj->inUse = false;
                hDriver->queueSizeCurrentRead --;

                /* Reset the next and the previous pointers */
                bufferObj->next = NULL;
                bufferObj->previous = NULL;

                /* While the first buffer was getting processed, if any
                 * buffer requests were Added, Start the added buffer request
                 * by adding the request to DMA now */
                if(hDriver->queueRead != NULL)
                {
                    size_t srcSize, cellSize;

                    srcSize = 1;
                    cellSize = 1;
                    bufferObj = hDriver->queueRead;

                    /* Reset the current head's previous pointer */
                    bufferObj->previous = NULL;

                    SYS_DMA_ChannelTransferAdd(hDriver->dmaChannelHandleRead,
                            (const void*)&hDriver->moduleId->US_RHR,
                            srcSize, bufferObj->buffer, bufferObj->size,cellSize);
                }
            }

        }
    }
    /* This instance doesn't use XDMAC for the transfer.
     * Check receive USART flag to process */
    else if( 1 == hDriver->moduleId->US_CSR.RXRDY )
    {
        /* In this function, the driver checks if there are any buffers in queue. If
           so the buffer is serviced. A buffer that is serviced completely is
           removed from the queue. Start by getting the buffer at the head of the
           queue */

        if(bufferObj != NULL)
        {
            /* The USART driver is configured to generate an interrupt when the FIFO
               is not empty. Additionally the queue is not empty. Which means there
               is work to done in this routine. Read data from the FIFO until either
               the FIFO is empty or until we have read the requested number of bytes.
            */
            while((1 == hDriver->moduleId->US_CSR.RXRDY) && (bufferObj->nCurrentBytes < bufferObj->size ))
            {
                bufferObj->buffer[bufferObj->nCurrentBytes] = hDriver->moduleId->US_RHR.RXCHR;
                bufferObj->nCurrentBytes ++;
            }

            /* Check if this buffer is done */
            if(bufferObj->nCurrentBytes >= bufferObj->size)
            {
                /* This means the buffer is completed. If there
                   is a callback registered with client, then
                   call it */

                client = (DRV_USART_CLIENT_OBJ *)bufferObj->hClient;
                if((client->eventHandler != NULL) && (bufferObj->flags & DRV_USART_BUFFER_OBJ_FLAG_BUFFER_ADD))
                {
                    /* Call the event handler. We additionally increment the
                       interrupt nesting count which lets the driver functions
                       that are called from the event handler know that an
                       interrupt context is active.
                       */

                    hDriver->interruptNestingCount ++;

                    client->eventHandler(DRV_USART_BUFFER_EVENT_COMPLETE,
                            bufferObj->bufferHandle,
                            client->context);

                    hDriver->interruptNestingCount --;
                }

                /* Get the next buffer in the queue and deallocate
                   this buffer */

                hDriver->queueRead = bufferObj->next;
                bufferObj->inUse = false;
                hDriver->queueSizeCurrentRead --;

                /* Reset the next and previous pointers */
                bufferObj->next = NULL;
                bufferObj->previous = NULL;

                /* Reset the current head's previous pointer */
                if (hDriver->queueRead != NULL)
                {
                    hDriver->queueRead->previous = NULL;
                }

                if(bufferObj->flags & DRV_USART_BUFFER_OBJ_FLAG_READ_WRITE)
                {
                    /* This means we should post the semaphore */
                    _DRV_USART_SEM_POST(&(client->semReadDone));
                }
            }
        }
        
        if(hDriver->queueRead == NULL)
        {
            /* The queue is empty. We can disable the interrupt */
            _DRV_USART_RxInterruptSourceDisable(hDriver->moduleId);
        }
    }
    
    /* Release the mutex */
   _DRV_USART_RELEASE_MUTEX(&(hDriver->mutexDriverInstance));
}

void _DRV_USART_BufferQueueTxTasks(SYS_MODULE_OBJ object)
{
    /* Start by getting the buffer at the head of queue. */

    DRV_USART_OBJ *hDriver = &gDrvUSARTObj[object];
    DRV_USART_BUFFER_OBJ * bufferObj;
    DRV_USART_CLIENT_OBJ * client;

    bufferObj = hDriver->queueWrite;

    /* If this driver is configured for polled mode in an RTOS, the tasks
       routine would be called from another thread. We need to get the driver
       instance mutex before updating the queue. If the driver is configured for
       interrupt mode, then _DRV_USART_TAKE_MUTEX will compile to true */

    if(DRV_USART_INTERRUPT_MODE == false)
    {
        if(OSAL_MUTEX_Lock(&(hDriver->mutexDriverInstance), OSAL_WAIT_FOREVER))
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

    /* Checking whether the transfer uses DMA channels */
    if(SYS_DMA_CHANNEL_HANDLE_INVALID != hDriver->dmaChannelHandleWrite)
    {
        if(bufferObj != NULL)
        {
            client = (DRV_USART_CLIENT_OBJ *)bufferObj->hClient;
            /* If Data transmission is completed or an data transfer
             * Abort has occurred */
            if((0==bufferObj->size) || (DRV_USART_DMA_TRANSFER_ABORT == bufferObj->size))
            {
                if((client->eventHandler != NULL) &&
                        (bufferObj->flags & DRV_USART_BUFFER_OBJ_FLAG_BUFFER_ADD))
                {
                    hDriver->interruptNestingCount ++;
                    client->eventHandler(client->bufferEvent,
                            bufferObj->bufferHandle,
                            client->context);
                    /* Decrement the nesting count */
                    hDriver->interruptNestingCount -- ;
                }

                hDriver->queueWrite = bufferObj->next;
                bufferObj->inUse = false;
                hDriver->queueSizeCurrentWrite --;

                /* Reset the next and the previous pointers */
                bufferObj->next = NULL;
                bufferObj->previous = NULL;

                /* While the first buffer was getting processed, if any
                 * buffer requests were Added, Start the added buffer request
                 * by adding the request to DMA now */
                if(hDriver->queueWrite!=NULL)
                {
                    size_t destSize, cellSize;

                    bufferObj = hDriver->queueWrite;
                    if(hDriver->operationMode == DRV_USART_OPERATION_MODE_ADDRESSED)
                    {
                        destSize = 2;
                        cellSize = 2;
                        if(false == bufferObj->isAddressSent)
                        {
                            hDriver->moduleId->US_THR.w |= US_THR_TXCHR((bufferObj->address | _DRV_USART_ADDRESSED_MODE_MSB));
                            bufferObj->isAddressSent = true;
                        }
                        /* Reset the current head's previous pointer */
                        bufferObj->previous = NULL;

                        SYS_DMA_ChannelTransferAdd(hDriver->dmaChannelHandleWrite,
                                bufferObj->buffer, (bufferObj->size*2),
                                (const void*)&hDriver->moduleId->US_THR,
                                destSize,cellSize);
                    }
                    else
                    {
                        destSize = 1;
                        cellSize = 1;

                        /* Reset the current head's previous pointer */
                        bufferObj->previous = NULL;

                        SYS_DMA_ChannelTransferAdd(hDriver->dmaChannelHandleWrite,
                                bufferObj->buffer, bufferObj->size,
                                (const void*)&hDriver->moduleId->US_THR,
                                destSize,cellSize);
                    }
                }
            }

        }
    }
    /* Channel doesn't use DMA, so check if USART TX is 
     * empty to process TX data */
    else if( 1 == hDriver->moduleId->US_CSR.TXEMPTY )
    {
        if(bufferObj != NULL)
        {
            /* This means the queue is not empty. Check if this buffer is done */
            if(bufferObj->nCurrentBytes >= bufferObj->size)
            {
                /* This means the buffer is completed. If there
                   is a callback registered with client, then
                   call it */

                client = (DRV_USART_CLIENT_OBJ *)bufferObj->hClient;
                if((client->eventHandler != NULL) && (bufferObj->flags & DRV_USART_BUFFER_OBJ_FLAG_BUFFER_ADD))
                {
                    /* Before calling the event handler, the interrupt nesting
                       counter is incremented. This will allow driver routine that
                       are called from the event handler to know the interrupt
                       nesting level. Events are only generated for buffers that
                       were submitted using the buffer add routine */

                    hDriver->interruptNestingCount ++;

                    client->eventHandler(DRV_USART_BUFFER_EVENT_COMPLETE,
                            bufferObj->bufferHandle,
                            client->context);

                    /* Decrement the nesting count */
                    hDriver->interruptNestingCount -- ;
                }

                /* Get the next buffer in the queue and deallocate
                 * this buffer */

                hDriver->queueWrite = bufferObj->next;
                bufferObj->inUse = false;
                hDriver->queueSizeCurrentWrite --;

                /* Reset the next and previous pointers */
                bufferObj->next = NULL;
                bufferObj->previous = NULL;

                /* Reset the current head's previous pointer */
                if (hDriver->queueWrite != NULL)
                {
                    hDriver->queueWrite->previous = NULL;
                }

                if(bufferObj->flags & DRV_USART_BUFFER_OBJ_FLAG_READ_WRITE)
                {
                    /* This means we should post the semaphore */
                    _DRV_USART_SEM_POST(&(client->semWriteDone));

                }
            }
        }

        /* Check if the queue is still not empty and process
           the buffer */

        if(hDriver->queueWrite != NULL)
        {
            bufferObj = hDriver->queueWrite;
     
            /* Submit data to send if TX is empty*/
            while((1 == hDriver->moduleId->US_CSR.TXEMPTY) && (bufferObj->nCurrentBytes < bufferObj->size ))
            {
                /* Send data */
                hDriver->moduleId->US_THR.w |= US_THR_TXCHR(bufferObj->buffer[bufferObj->nCurrentBytes]);

                bufferObj->nCurrentBytes ++;
            }
        }
        else
        {
            /* If the queue is empty, then disable the TX 
             * interrupt source */
            _DRV_USART_TxInterruptSourceDisable(hDriver->moduleId);
        }
    }
    
    /* Release the mutex */
    _DRV_USART_RELEASE_MUTEX(&(hDriver->mutexDriverInstance));
}

void _DRV_USART_BufferQueueErrorTasks(SYS_MODULE_OBJ object)
{
    DRV_USART_OBJ *hDriver = &gDrvUSARTObj[object];
    DRV_USART_BUFFER_OBJ * bufferObj;
    DRV_USART_CLIENT_OBJ * clientObj;
    bool mutexGrabbed  = true;
    size_t srcSize = 1, cellSize = 1;

    /* If transfer uses DMA then abort the current DMA operation */
    if( SYS_DMA_CHANNEL_HANDLE_INVALID != hDriver->dmaChannelHandleRead )
    {
        /* Abort the current DMA operation */
        SYS_DMA_ChannelForceAbort(hDriver->dmaChannelHandleRead);
    }


    /* USART driver will take care that TX erros( like overflow etc) are not 
     * going to occur at any time based on checks before write.
     * So, only RX errors are to be handled/reported */
    
    /* Get the RX buffer at the head */
    bufferObj = hDriver->queueRead;

    /* If this driver is configured for polled mode in an RTOS, the tasks
     * routine would be called from another thread. We need to get the driver
     * instance mutex before updating the queue. If the driver is configured for
     * interrupt mode, then OSAL_MUTEX_Lock will compile to true */

    if(DRV_USART_INTERRUPT_MODE == false)
    {
        if(!OSAL_MUTEX_Lock(&(hDriver->mutexDriverInstance), OSAL_WAIT_FOREVER))
        {
            /* The mutex acquisition timed out.
             * This code will not execute if there is no RTOS. */
            mutexGrabbed = false;
        }
    }

    /* mutexGrabbed will always be true for non-RTOS case.
     * Will be false when mutex aquisition timed out in RTOS mode */
    if(true == mutexGrabbed)
    {
            
        if(bufferObj != NULL)
        {
            /* Get the client objact */
            clientObj = (DRV_USART_CLIENT_OBJ *)bufferObj->hClient;
            
            /* Update the USART client error flag */
            clientObj->error = (hDriver->moduleId->US_CSR.w & (US_CSR_OVRE_Msk | US_CSR_FRAME_Msk | US_CSR_PARE_Msk));

            /* Clear error condition */
            _DRV_USART_ErrorConditionClear(hDriver);

            if(clientObj->eventHandler != NULL)
            {
                /* Call the event handler with buffer event error state */
                clientObj->eventHandler(DRV_USART_BUFFER_EVENT_ERROR,
                        bufferObj->bufferHandle,
                        clientObj->context);
            }
            

            /* Get the next buffer in the queue and deallocate
             * this buffer */
            hDriver->queueRead = bufferObj->next;
            bufferObj->inUse = false;
            hDriver->queueSizeCurrentRead --;

            /* Reset the next and previous pointers */
            bufferObj->next = NULL;
            bufferObj->previous = NULL;

            /* If queue head is not null, update buffer pointer parameters*/
            if (hDriver->queueRead != NULL)
            {
                /* Reset the updated head's previous pointer */
                hDriver->queueRead->previous = NULL;

                /* Add next buffer to the DMA channel*/
                if( SYS_DMA_CHANNEL_HANDLE_INVALID != hDriver->dmaChannelHandleRead )
                {
                    SYS_DMA_ChannelTransferAdd(hDriver->dmaChannelHandleRead,
                            (const void*)&hDriver->moduleId->US_RHR,
                            srcSize,hDriver->queueRead->buffer, hDriver->queueRead->size,
                            cellSize);
                }
            }
        }
        else
        {
            /* There is no buffer in the queue.
             * Flush the RX to clear the error condition */
            _DRV_USART_ErrorConditionClear(hDriver);
        }
    }

    /* Release the mutex in RTOS environment*/
    _DRV_USART_RELEASE_MUTEX(&(hDriver->mutexDriverInstance));
}

void _DRV_USART_ErrorConditionClear(DRV_USART_OBJ *hDriver)
{
    uint8_t dummyData = 0u;
        
     /* Clear all error flags */
    hDriver->moduleId->US_CR.w |= US_CR_RSTSTA_Msk;

    /* Read existing error bytes from RX */
    if( 1 == hDriver->moduleId->US_CSR.RXRDY )
    {
        dummyData = hDriver->moduleId->US_RHR.RXCHR;
    }
    
    /* Ignore the warning */
    (void)dummyData;
}

/*******************************************************************************
 End of File
*/

