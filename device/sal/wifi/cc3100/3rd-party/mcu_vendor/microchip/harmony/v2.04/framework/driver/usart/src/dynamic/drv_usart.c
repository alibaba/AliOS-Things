/********************************************************************************
  USART Driver Dynamic implementation.

  Company:
    Microchip Technology Inc.

  File Name:
    drv_usart.c

  Summary:
    Source code for the USART driver dynamic implementation.

  Description:
    This file contains the source code for the dynamic implementation of the
    USART driver.
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

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
#include "../drv_usart_local.h"



// *****************************************************************************
// *****************************************************************************
// Section: Global Data
// *****************************************************************************
// *****************************************************************************

/* This is the driver instance object array. */
DRV_USART_OBJ gDrvUSARTObj[DRV_USART_INSTANCES_NUMBER] ;

/* This is the client object array. */
DRV_USART_CLIENT_OBJ gDrvUSARTClientObj[DRV_USART_CLIENTS_NUMBER];

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

    /* Disable the USART module */
    PLIB_USART_Disable (usartInit->usartID) ;

    /* Allocate the driver object and set the operation flag to be in use */
    dObj = &gDrvUSARTObj[drvIndex];
    dObj->inUse = true;

    /* Update the USART PLIB Id and other parameters. */

    dObj->nClients              = 0;
    dObj->moduleId              = usartInit->usartID;
    dObj->brgClock              = usartInit->brgClock;
    dObj->isExclusive           = false;
    dObj->queueSizeRead         = usartInit->queueSizeReceive;
    dObj->queueSizeWrite        = usartInit->queueSizeTransmit;
    dObj->dmaChannelRead        = usartInit->dmaChannelReceive;
    dObj->dmaChannelWrite       = usartInit->dmaChannelTransmit;
    dObj->txInterruptSource     = usartInit->interruptTransmit;
    dObj->rxInterruptSource     = usartInit->interruptReceive;
    dObj->errorInterruptSource  = usartInit->interruptError;
    dObj->dmaInterruptTransmit  = usartInit->dmaInterruptTransmit;
    dObj->dmaInterruptReceive   = usartInit->dmaInterruptReceive;
    dObj->interruptNestingCount = 0;
    dObj->queueSizeCurrentRead  = 0;
    dObj->queueSizeCurrentWrite = 0;
    dObj->queueRead             = NULL;
    dObj->queueWrite            = NULL;
    dObj->transmitCallback      = NULL;
    dObj->receiveCallback       = NULL;
    dObj->errorCallback         = NULL;
    dObj->operationMode         = usartInit->mode;

    /* Setup the Hardware */
    _DRV_USART_HardwareSetup(usartInit->usartID, usartInit ) ;

    /* Clear the interrupts */
    SYS_INT_SourceStatusClear(dObj->txInterruptSource);
    SYS_INT_SourceStatusClear(dObj->rxInterruptSource);
    SYS_INT_SourceStatusClear(dObj->errorInterruptSource);

    /* Enable the interrupt source in case of interrupt mode */
    _DRV_USART_InterruptSourceEnable(dObj->errorInterruptSource);

    _DRV_USART_ByteModelInterruptSourceEnable(dObj->rxInterruptSource);

    /* TODO: Enable DMA interrupts if the DMA channel is selected */

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

    /* Enable the USART module */
    PLIB_USART_Enable(usartInit->usartID) ;

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

    bool status;

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

    /* Disable the interrupt */
    status = _DRV_USART_InterruptSourceDisable(dObj->txInterruptSource) ;
    status = _DRV_USART_InterruptSourceDisable(dObj->rxInterruptSource) ;
    status = _DRV_USART_InterruptSourceDisable(dObj->errorInterruptSource);

    /* Ignore the warning */
    (void)status;

    /* Disable USART module */
    PLIB_USART_Disable (dObj->moduleId);

    /* Deallocate all mutexes */
    if(OSAL_MUTEX_Delete(&(dObj->mutexDriverInstance)) != OSAL_RESULT_TRUE)
    {
        SYS_DEBUG(0, "Mutex Delete Failed");
        return;
    }

    /* TODO: Disable all DMA interrupts */

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
    void DRV_USART_ByteTransmitCallbackSet
    (
        const SYS_MODULE_INDEX index,
        const DRV_USART_BYTE_EVENT_HANDLER eventHandler
    )

  Summary:
    Registers transmit callback function.

  Description:
    This function is used to register the callback function to be invoked
    upon transmission of a byte.

  Remarks:
    See drv_usart.h for usage information.
*/

void DRV_USART_ByteTransmitCallbackSet
(
    const SYS_MODULE_INDEX index,
    const DRV_USART_BYTE_EVENT_HANDLER eventHandler
)
{
    if (index >= DRV_USART_INSTANCES_NUMBER)
    {
        /* Invalid driver index */
        SYS_DEBUG(0, "\r\nUSART Driver: Invalid Driver Instance");
    }
    else
    {
        gDrvUSARTObj[index].transmitCallback = eventHandler;
    }
}

// *****************************************************************************
/* Function:
    void DRV_USART_ByteReceiveCallbackSet
    (
        const SYS_MODULE_INDEX index,
        const DRV_USART_BYTE_EVENT_HANDLER eventHandler
    )

  Summary:
    Registers receive callback function.

  Description:
    This function is used to register the callback function to be invoked
    upon reception of a byte.

  Remarks:
    See drv_usart.h for usage information.
*/

void DRV_USART_ByteReceiveCallbackSet
(
    const SYS_MODULE_INDEX index,
    const DRV_USART_BYTE_EVENT_HANDLER eventHandler
)
{
    if (index >= DRV_USART_INSTANCES_NUMBER)
    {
        /* Invalid driver index */
        SYS_DEBUG(0, "\r\nUSART Driver: Invalid Driver Instance");
    }
    else
    {
        gDrvUSARTObj[index].receiveCallback = eventHandler;
    }
}

// *****************************************************************************
/* Function:
    void DRV_USART_ByteErrorCallbackSet
    (
        const SYS_MODULE_INDEX index,
        const DRV_USART_BYTE_EVENT_HANDLER eventHandler
    )

  Summary:
    Registers error notification callback function.

  Description:
    This function is used to register the callback function to be invoked
    to notify error on the tx or the rx path in byte model of operation.

  Remarks:
    See drv_usart.h for usage information.
*/

void DRV_USART_ByteErrorCallbackSet
(
    const SYS_MODULE_INDEX index,
    const DRV_USART_BYTE_EVENT_HANDLER eventHandler
)
{
    if (index >= DRV_USART_INSTANCES_NUMBER)
    {
        /* Invalid driver index */
        SYS_DEBUG(0, "\r\nUSART Driver: Invalid Driver Instance");
    }
    else
    {
        gDrvUSARTObj[index].errorCallback = eventHandler;
    }
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
    USART_MODULE_ID plibID;
    DRV_USART_BAUD_SET_RESULT retVal = DRV_USART_BAUD_SET_SUCCESS;
#if defined (PLIB_USART_ExistsModuleBusyStatus)
    bool isEnabled = false;
#endif

    /* Validate the handle */
    client = _DRV_USART_DriverHandleValidate(handle);

    if(client == NULL)
    {
        /* Driver handle is not valid. Return error */
        SYS_DEBUG(0, "Invalid client handle");
        return DRV_USART_BAUD_SET_ERROR;
    }

    hDriver = (DRV_USART_OBJ*)client->hDriver;
    plibID = hDriver->moduleId;

    if(OSAL_MUTEX_Lock(&(hDriver->mutexDriverInstance), OSAL_WAIT_FOREVER) == OSAL_RESULT_TRUE)
    {
        int32_t brgValueLow=0;
        int32_t brgValueHigh=0;

        brgValueLow  = ( (hDriver->brgClock/baud) >> 4 ) - 1;
        brgValueHigh = ( (hDriver->brgClock/baud) >> 2 ) - 1;

#if defined (PLIB_USART_ExistsModuleBusyStatus)
        isEnabled = PLIB_USART_ModuleIsBusy (plibID);
        if (isEnabled)
        {
            PLIB_USART_Disable (plibID);
            while (PLIB_USART_ModuleIsBusy (plibID));
        }
#endif
        /* Check if the baud value can be set with high baud settings */
        if ((brgValueHigh >= 0) && (brgValueHigh <= UINT16_MAX))
        {
            PLIB_USART_BaudRateHighEnable(plibID);
            PLIB_USART_BaudRateHighSet(plibID, hDriver->brgClock, baud);
        }
        /* Check if the baud value can be set with low baud settings */
        else if ((brgValueLow >= 0) && (brgValueLow <= UINT16_MAX))
        {
            PLIB_USART_BaudRateHighDisable(plibID);
            PLIB_USART_BaudRateSet(plibID, hDriver->brgClock, baud);
        }
        else
        {
            retVal = DRV_USART_BAUD_SET_ERROR;
        }

#if defined (PLIB_USART_ExistsModuleBusyStatus)
        if (isEnabled)
        {
            PLIB_USART_Enable (plibID);
        }
#endif
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
    USART_MODULE_ID plibID;
#if defined (PLIB_USART_ExistsModuleBusyStatus)
    bool isEnabled = false;
#endif

    /* Validate the driver handle */
    client = _DRV_USART_DriverHandleValidate(handle);

    if(client == NULL)
    {
        /* Driver handle is not valid. Return error */

        SYS_DEBUG(0, "Invalid driver handle");
        return DRV_USART_LINE_CONTROL_SET_ERROR;
    }

    hDriver = (DRV_USART_OBJ*)client->hDriver;
    plibID = hDriver->moduleId;

    if(OSAL_MUTEX_Lock(&(hDriver->mutexDriverInstance), OSAL_WAIT_FOREVER) == OSAL_RESULT_TRUE)
    {
#if defined (PLIB_USART_ExistsModuleBusyStatus)
        isEnabled = PLIB_USART_ModuleIsBusy (plibID);
        if (isEnabled)
        {
            PLIB_USART_Disable (plibID);
            while (PLIB_USART_ModuleIsBusy (plibID));
        }
#endif
        /* Set the Line Control Mode */
        PLIB_USART_LineControlModeSelect(plibID, lineControlMode);
#if defined (PLIB_USART_ExistsModuleBusyStatus)
        if (isEnabled)
        {
            PLIB_USART_Enable (plibID);
        }
#endif
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

    if(SYS_INT_SourceStatusGet(hDriver->txInterruptSource))
    {
        /* The USART driver is configured to generate an
           interrupt when the FIFO is empty. Additionally
           the queue is not empty. Which means there is
           work to done in this routine. */

        _DRV_USART_TRANSMIT_BUFFER_QUEUE_TASKS(object);

        /* Clear up the interrupt flag */
        SYS_INT_SourceStatusClear(hDriver->txInterruptSource);
    }
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

    if(SYS_INT_SourceStatusGet(hDriver->errorInterruptSource))
    {
        /* This means an error has occurred */
        _DRV_USART_ERROR_TASKS(object);

        /* Clear up the error interrupt flag */
        SYS_INT_SourceStatusClear(hDriver->errorInterruptSource);
    }
}

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

    if(SYS_INT_SourceStatusGet(hDriver->rxInterruptSource))
    {
        _DRV_USART_RECEIVE_BUFFER_QUEUE_TASKS(object);

        /* Clear up the interrupt flag */
        SYS_INT_SourceStatusClear(hDriver->rxInterruptSource);
    }
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
    result |= (PLIB_USART_ReceiverDataIsAvailable(hDriver->moduleId)) ?
        DRV_USART_TRANSFER_STATUS_RECEIVER_DATA_PRESENT : DRV_USART_TRANSFER_STATUS_RECEIVER_EMPTY;

    /* If the TX interrupt flag is set, then the Tx Buffer is empty */
    result |= (PLIB_USART_TransmitterIsEmpty(hDriver->moduleId)) ? DRV_USART_TRANSFER_STATUS_TRANSMIT_EMPTY : 0;

    /* Check if the TX buffer is full */
    result |= (PLIB_USART_TransmitterBufferIsFull(hDriver->moduleId)) ? DRV_USART_TRANSFER_STATUS_TRANSMIT_FULL : 0;

    return(result);
}

// *****************************************************************************
// *****************************************************************************
// Section: File scope functions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* void _DRV_USART_HardwareSetup
   (
        USART_MODULE_ID  plibID,
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

void _DRV_USART_HardwareSetup(USART_MODULE_ID plibID, DRV_USART_INIT * init)
{
    bool loopbackMode   = false;
    bool irdaMode       = false;
    bool wakeFromSleep  = false;
    bool stopInIdle     = false;
    bool autoBaud       = false;

    /* Default is to use UxTX and UxRX only */
    USART_OPERATION_MODE handshakeLines = USART_ENABLE_TX_RX_USED;

    /* Check the operational mode and update the relevant flags */
    switch(init->mode)
    {
        case DRV_USART_OPERATION_MODE_ADDRESSED:
        {
            PLIB_USART_ReceiverAddressDetectEnable(plibID);
            PLIB_USART_AddressSet(plibID, init->modeData.AddressedModeInit.address);
#if defined (PLIB_USART_ExistsReceiverAddressAutoDetect)
            PLIB_USART_ReceiverAddressAutoDetectEnable(plibID, PLIB_USART_AddressGet(plibID));
#endif
        }
        break;
        case DRV_USART_OPERATION_MODE_IRDA:
            irdaMode = true;
            handshakeLines = USART_ENABLE_TX_RX_BCLK_USED;
            break;
        case DRV_USART_OPERATION_MODE_LOOPBACK:
            loopbackMode = true;
            break;
        default:
            break;
    }

    /* Handshaking */
    if(init->handshake != DRV_USART_HANDSHAKE_NONE)
    {
        /* Note that this mode is exclusive of DRV_USART_OPERATION_MODE_IRDA.
           In that if the driver must be set up to use DRV_USART_OPERATION_MODE_IRDA
           then handshake parameter of the init data structure must be
           DRV_USART_HANDSHAKE_NONE. Setting up the driver to use Simplex or
           flow control will over ride the requirements of the IRDA mode.*/

        handshakeLines = USART_ENABLE_TX_RX_CTS_RTS_USED;

        /* Set the handshake mode to either simplex or flow control */

        PLIB_USART_HandshakeModeSelect(plibID, init->handshake);
    }


    /* Wake up the part from sleep on any activity on the RX Line */
    wakeFromSleep = (init->flags & DRV_USART_INIT_FLAG_WAKE_ON_START);

    /* When the CPU enters IDLE mode, stop the USART */
    stopInIdle = (init->flags & DRV_USART_INIT_FLAG_STOP_IN_IDLE);

    /* When the CPU enters IDLE mode, stop the USART */
    autoBaud = (init->flags & DRV_USART_INIT_FLAG_AUTO_BAUD);

    /* Initialize the USART based on initialization data structure */
    PLIB_USART_InitializeModeGeneral(plibID, autoBaud, loopbackMode,
            wakeFromSleep, irdaMode, stopInIdle);

    /* Set the line control mode */
    PLIB_USART_LineControlModeSelect(plibID, init->lineControl);

    /* We set the receive interrupt mode to receive an interrupt whenever FIFO
       is not empty */
    PLIB_USART_InitializeOperation(plibID, USART_RECEIVE_FIFO_ONE_CHAR,
            USART_TRANSMIT_FIFO_IDLE, handshakeLines);

    /* Set the baud rate and enable the USART */
    PLIB_USART_BaudSetAndEnable(plibID, init->brgClock, init->baud);
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
    bool interruptWasEnabled = false;
    DRV_USART_BUFFER_OBJ * iterator = NULL;

    if(OSAL_MUTEX_Lock(&(dObj->mutexDriverInstance), OSAL_WAIT_FOREVER) == OSAL_RESULT_TRUE)
    {
        /* Disable the transmit interrupt */
        interruptWasEnabled = _DRV_USART_InterruptSourceDisable(dObj->txInterruptSource);
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
        else
        {
            /* Iterate to update the head pointer to point
             * the first valid buffer object in the queue */
            iterator = dObj->queueWrite;
            while(iterator != NULL)
            {
                if(iterator->inUse == true)
                {
                    dObj->queueWrite = iterator;
                    break;
                }
                iterator = iterator->next;
            }
        }


        /* Re-enable the interrupt if it was enabled */
        if(interruptWasEnabled)
        {
	        _DRV_USART_InterruptSourceEnable(dObj->txInterruptSource);
        }

        /* Now check the receive buffer queue. Start by disabling the
           receive interrupt */

        interruptWasEnabled = _DRV_USART_InterruptSourceDisable(dObj->rxInterruptSource);
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
        else
        {
            /* Iterate to update the head pointer to point
             * the first valid buffer object in the queue */
            iterator = dObj->queueRead;
            while(iterator != NULL)
            {
                if(iterator->inUse == true)
                {
                    dObj->queueRead = iterator;
                    break;
                }
                iterator = iterator->next;
            }
        }

        if(interruptWasEnabled)
        {
            _DRV_USART_InterruptSourceEnable(dObj->rxInterruptSource);
        }

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
    USART_MODULE_ID plibID;

    bool status;

    plibID = hDriver->moduleId;

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

    /* In this function, the driver checks if there are any buffers in queue. If
       so the buffer is serviced. A buffer that is serviced completely is
       removed from the queue. Start by getting the buffer at the head of the
       queue */

    bufferObj = hDriver->queueRead;

    if(bufferObj != NULL)
    {
        /* The USART driver is configured to generate an interrupt when the FIFO
           is not empty. Additionally the queue is not empty. Which means there
           is work to done in this routine. Read data from the FIFO until either
           the FIFO is empty or until we have read the requested number of bytes.
        */
        while((PLIB_USART_ReceiverDataIsAvailable(plibID))
            && (bufferObj->nCurrentBytes < bufferObj->size ))
        {
            bufferObj->buffer[bufferObj->nCurrentBytes] = PLIB_USART_ReceiverByteReceive(plibID);
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
        status = _DRV_USART_InterruptSourceDisable(hDriver->rxInterruptSource);
        /* Ignore the warning */
        (void)status;
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
    USART_MODULE_ID plibID;

    bool status;

    bufferObj = hDriver->queueWrite;
    plibID = hDriver->moduleId;

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
        if(hDriver->operationMode == DRV_USART_OPERATION_MODE_ADDRESSED)
        {
			uint16_t *buf_ptr;
            buf_ptr = (uint16_t *) bufferObj->buffer;

            /* Fill up the FIFO with data until the FIFO is full
               and we have data to send */
            while((!PLIB_USART_TransmitterBufferIsFull(plibID))
                    && (bufferObj->nCurrentBytes < bufferObj->size ))
            {
                if((bufferObj->nCurrentBytes == 0) && (false == bufferObj->isAddressSent))
                {
                    /* Send Address */
                    PLIB_USART_Transmitter9BitsSend(plibID, bufferObj->address, true);
                    bufferObj->isAddressSent = true;
                }
                else
                {
                    /* Send data */
                    PLIB_USART_Transmitter9BitsSend(plibID, buf_ptr[bufferObj->nCurrentBytes], false);
                    bufferObj->nCurrentBytes ++; // Here it is incrementing the 16-bit words number.
                }
            }
        }
        else
        {
            /* Fill up the FIFO with data until the FIFO is full
               and we have data to send */
            while((!PLIB_USART_TransmitterBufferIsFull(plibID))
                    && (bufferObj->nCurrentBytes < bufferObj->size ))
            {
                /* Send data */
                PLIB_USART_TransmitterByteSend(plibID, bufferObj->buffer[bufferObj->nCurrentBytes]);
                bufferObj->nCurrentBytes ++;
            }
        }
    }
    else
    {
        /* If the queue is empty, then disable the TX interrupt */
        status = _DRV_USART_InterruptSourceDisable(hDriver->txInterruptSource);
        /* Ignore the warning */
        (void)status;
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
    bool status = false;

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
            clientObj->error = PLIB_USART_ErrorsGet(hDriver->moduleId);

            /* Clear error condition */
            _DRV_USART_ErrorConditionClear(hDriver);

            /* Call event handler in Buffer queue request only.
             * If it's a File i/o request then DRV_USART_Read call itself 
             * will return with error result */
            if((clientObj->eventHandler != NULL) && (bufferObj->flags & DRV_USART_BUFFER_OBJ_FLAG_BUFFER_ADD))
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
            }
            else
            {
                /* Queue is empty disable the RX interrupt */
                status = _DRV_USART_InterruptSourceDisable(hDriver->rxInterruptSource);
                
                /* Ignore the warning */
                (void)status;
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
    /* RX length = (FIFO level + RX register) */
    uint8_t RXlength = _DRV_USART_RX_DEPTH;
        
    /* If it's a overrun error then clear it to flush FIFO */
    if(USART_ERROR_RECEIVER_OVERRUN & PLIB_USART_ErrorsGet(hDriver->moduleId))
    {
        PLIB_USART_ReceiverOverrunErrorClear(hDriver->moduleId);
    }

    /* Read existing error bytes from FIFO to clear parity and framing error flags*/
    while( (USART_ERROR_PARITY | USART_ERROR_FRAMING) & PLIB_USART_ErrorsGet(hDriver->moduleId) )
    {
        dummyData = PLIB_USART_ReceiverByteReceive(hDriver->moduleId);
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
    SYS_INT_SourceStatusClear(hDriver->errorInterruptSource);
            
    /* Clear up the receive interrupt flag so that RX interrupt is not 
     * triggered for error bytes*/
    SYS_INT_SourceStatusClear(hDriver->rxInterruptSource);
}

void _DRV_USART_ByteTransmitTasks (SYS_MODULE_OBJ object)
{
    DRV_USART_OBJ * hDriver = &gDrvUSARTObj[object];

    if (_DRV_USART_InterruptSourceIsEnabled(hDriver->txInterruptSource))
    {
        /* Disable the interrupt, to avoid calling ISR continuously*/
        _DRV_USART_InterruptSourceDisable(hDriver->txInterruptSource);

        if (hDriver->transmitCallback != NULL)
        {
            hDriver->transmitCallback (object);
        }
    }

}

void _DRV_USART_ByteReceiveTasks (SYS_MODULE_OBJ object)
{
    DRV_USART_OBJ * hDriver = &gDrvUSARTObj[object];

    if (_DRV_USART_InterruptSourceIsEnabled(hDriver->rxInterruptSource))
    {
        if (hDriver->receiveCallback != NULL)
        {
            hDriver->receiveCallback (object);
        }
    }
}

void _DRV_USART_ByteErrorTasks (SYS_MODULE_OBJ object)
{
    DRV_USART_OBJ * hDriver = &gDrvUSARTObj[object];

    if (_DRV_USART_InterruptSourceIsEnabled(hDriver->errorInterruptSource))
    {
        /* Clear error condition */
        _DRV_USART_ErrorConditionClear(hDriver);
        
        if (hDriver->errorCallback != NULL)
        {
            hDriver->errorCallback (object);
        }
    }
}

/*******************************************************************************
 End of File
*/

