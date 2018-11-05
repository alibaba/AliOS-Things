/*******************************************************************************
  USART Driver Dynamic implementation.

  Company:
    Microchip Technology Inc.

  File Name:
    drv_usart_file_io_model_pic32c.c

  Summary:
    Source code for the USART driver dynamic implementation of File IO data
    transfer model.

  Description:
    This file contains the source code for the dynamic implementation of the
    USART driver File IO data transfer model.
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

#include "driver/usart/src/drv_usart_local_pic32c.h"

// *****************************************************************************
/* Function:
    size_t DRV_USART_Read
    (
        DRV_HANDLE hClient, 
        void * destination,
        size_t nBytes
    )

  Summary:
    Dynamic implementation of DRV_USART_Read client interface function.

  Description:
    This is the dynamic implementation of DRV_USART_Read client interface
    function.

  Remarks:
    See drv_usart.h for usage information.
*/

size_t DRV_USART_Read(const DRV_HANDLE hClient, void * destination, size_t nBytes)
{
    DRV_USART_CLIENT_OBJ * clientObj;
    DRV_USART_OBJ * hDriver;
    DRV_USART_BUFFER_OBJ * iterator, * bufferObj;
    size_t count = 0;
    uint8_t * data;

    /* Validate the driver handle */

    clientObj = _DRV_USART_DriverHandleValidate(hClient);
    if(clientObj == NULL)
    {
        SYS_DEBUG(0, "Invalid driver handle");
        return 0;
    }

    hDriver = (DRV_USART_OBJ *)clientObj->hDriver;

    if(!(clientObj->ioIntent & DRV_IO_INTENT_READ))
    {
        /* This client did not open the driver for
           reading */
        SYS_DEBUG(0, "Driver not opened for read");
        return 0;
    }

    if((destination == NULL) || (nBytes == 0))
    {
        /* We have a NULL pointer or don't have
           any data to write. */

        SYS_DEBUG(0, "NULL data pointer or no data to write");
        return 0;
    }

    data = (uint8_t *)destination;

    /* Grab he hardware instance mutex */

    if(OSAL_MUTEX_Lock(&(hDriver->mutexDriverInstance), OSAL_WAIT_FOREVER) == OSAL_RESULT_TRUE)
    {
        if(!(clientObj->ioIntent & DRV_IO_INTENT_NONBLOCKING))
        {
            /* This is a blocking implementation. We populate the client buffer
               object and add it to the queue. We then wait till the buffer is
               completely processed. */

            /* Because we are modifying the queue, we should disable the
               interrupt */
            _DRV_USART_InterruptDisable(hDriver->interruptUSART);

            /* Get the queue head */

            iterator = hDriver->queueRead;

            /* If the queue is not empty, get to the end of the queue */

            if(iterator != NULL)
            {
                while(iterator->next != NULL)
                {
                    iterator = iterator->next;
                }
            }

            /* We should be at the end of the queue now. Populate the client
               buffer object */

            bufferObj = &clientObj->bufferObject;
            bufferObj->buffer = destination;
            bufferObj->nCurrentBytes = 0;
            bufferObj->size = nBytes;
            bufferObj->inUse = true;
            bufferObj->hClient = clientObj;
            bufferObj->flags = (0 | DRV_USART_BUFFER_OBJ_FLAG_READ_WRITE);

            /* Add this object to the queue and enable the RX interrupt */

            bufferObj->previous = iterator;
            bufferObj->next = NULL;

            /* If we are not at the start of the queue, then update the next
               pointer of the last object else set the queue head to point to
               this object */

            if(hDriver->queueRead == NULL)
            {
                hDriver->queueRead = bufferObj;
            }
            else
            {
                iterator->next = bufferObj;
                bufferObj->previous = iterator;
            }

            /* Update the queue size as we have added a buffer to the queue */
            hDriver->queueSizeCurrentRead ++;

            /* Now enable the interrupt and release the mutex so that the system
               can proceed */

            _DRV_USART_RxInterruptSourceEnable(hDriver->moduleId);
            _DRV_USART_InterruptEnable(hDriver->interruptUSART);
            
            OSAL_MUTEX_Unlock(&(hDriver->mutexDriverInstance));

            /* If we are in a bare metal configuration, then wait till the
               buffer is processed. If we are in RTOS configuration then pend on
               the client semaphore. */

            OSAL_SEM_Pend(&(clientObj->semReadDone), OSAL_WAIT_FOREVER);
            /* This is the implementation of the blocking behavior. In a
               RTOS configuration, if the code reaches here, it means then
               that buffer has been processed. */

            while(bufferObj->inUse);
            if(bufferObj->nCurrentBytes != nBytes)
            {
                /* This means this buffer was terminated because of an
                   error. */

                return(DRV_USART_READ_ERROR);
            }

            count = nBytes;
            
        }
        else if(clientObj->ioIntent & DRV_IO_INTENT_NONBLOCKING)
        {
            /* This is a non blocking implementation*/

            if(hDriver->queueRead != NULL)
            {
                /* This means queue is not empty. We cannot read
                   data now. */
                count = 0;
            }
            else
            {

                while((1 == hDriver->moduleId->US_CSR.RXRDY) && (count < nBytes))
                {

                    /* We need to check for errors. Store the error
                       in the client error field. */

                    clientObj->error = (hDriver->moduleId->US_CSR.w & (US_CSR_OVRE_Msk | US_CSR_FRAME_Msk | US_CSR_PARE_Msk));

                    if(clientObj->error != DRV_USART_ERROR_NONE)
                    {
                        /* This means we have an error. Release the mutex and
                           exit */
                           
                        /* Clear error condition before exiting */
                        _DRV_USART_ErrorConditionClear(hDriver);
                        
                        OSAL_MUTEX_Unlock(&(hDriver->mutexDriverInstance));
                        return(DRV_USART_READ_ERROR);
                    }
                    
                    /* This is not a blocking implementation. We read
                       the hardware till the FIFO is empty. */

                    data[count] = hDriver->moduleId->US_RHR.RXCHR;
                    count ++;
                }

                /* Unlock the mutex */
                OSAL_MUTEX_Unlock(&(hDriver->mutexDriverInstance));
            }
        }
    }
    else
    {
        /* Timed out while waiting for read mutex.
         * We simply return 0 */
        count = 0;
    }

    return(count);
}

// *****************************************************************************
/* Function:
    size_t DRIVER DRV_USART_Write
    (
        DRV_HANDLE hClient, 
        void * source, 
        size_t nBytes
    )

  Summary:
    Dynamic implementation of DRV_USART_Write client interface function.

  Description:
    This is the dynamic implementation of DRV_USART_Write client interface function.

  Remarks:
    See drv_usart.h for usage information.
*/

size_t DRV_USART_Write(DRV_HANDLE hClient, void * source, size_t nBytes)
{
    DRV_USART_CLIENT_OBJ * clientObj;
    DRV_USART_OBJ * hDriver;
    DRV_USART_BUFFER_OBJ * iterator, * bufferObj;
    size_t count = 0;
    uint8_t * data;

    /* Validate the driver handle */

    clientObj = _DRV_USART_DriverHandleValidate(hClient);
    if(clientObj == NULL)
    {
        SYS_DEBUG(0, "Invalid driver handle");
        return 0;
    }

    hDriver = (DRV_USART_OBJ *)clientObj->hDriver;

    if(!(clientObj->ioIntent & DRV_IO_INTENT_WRITE))
    {
        /* This client did not open the driver for
           writing */
        SYS_DEBUG(0, "Driver not opened for write");
        return 0;
    }

    if((source == NULL) || (nBytes == 0))
    {
        /* We have a NULL pointer or don't have
           any data to write. */

        SYS_DEBUG(0, "NULL data pointer or no data to write");
        return 0;
    }

    data = (uint8_t *)source;

    /* Grab the write mutex */
    if(OSAL_MUTEX_Lock(&(hDriver->mutexDriverInstance), OSAL_WAIT_FOREVER) == OSAL_RESULT_TRUE)
    {
        if(!(clientObj->ioIntent & DRV_IO_INTENT_NONBLOCKING))
        {
            /* This is a blocking implementation. We populate the client buffer
               object and add it to the queue. We then wait till the buffer is
               completely processed. */

            /* Because we are modifying the queue, we should disable the
               interrupt */
            _DRV_USART_InterruptDisable(hDriver->interruptUSART);

            /* Get the queue head */

            iterator = hDriver->queueWrite;

            /* If the queue is not empty, then get to the
               end of the queue */

            if(iterator != NULL)
            {
                while(iterator->next != NULL)
                {
                    iterator = iterator->next;
                }
            }

            /* We should be at the end of the queue now. Populate the client
               buffer object */

            bufferObj = &clientObj->bufferObject;
            bufferObj->buffer = source;
            bufferObj->nCurrentBytes = 0;
            bufferObj->size = nBytes;
            bufferObj->inUse = true;
            bufferObj->hClient = clientObj;
            bufferObj->flags = (0 | DRV_USART_BUFFER_OBJ_FLAG_READ_WRITE);

            /* Add this object to the queue and enable the RX interrupt */

            bufferObj->previous = iterator;
            bufferObj->next = NULL;

            /* If we are not at the start of the queue, then update the next
               pointer of the last object else set the queue head to point to
               this object */

            if(hDriver->queueWrite == NULL)
            {
                hDriver->queueWrite = bufferObj;
                bufferObj->nCurrentBytes = 1;
                hDriver->queueSizeCurrentWrite ++;
                
                /* Send one byte */
                hDriver->moduleId->US_THR.w |= US_THR_TXCHR( data[0] );
                
                /* Enable module TX interrupt source */
                _DRV_USART_TxInterruptSourceEnable(hDriver->moduleId);
            }
            else
            {
                iterator->next = bufferObj;
                bufferObj->previous = iterator;
                hDriver->queueSizeCurrentWrite ++;
            }

            /* Now enable the interrupt and release the mutex so that the system
               can proceed */

            _DRV_USART_InterruptEnable(hDriver->interruptUSART);
            
            OSAL_MUTEX_Unlock(&(hDriver->mutexDriverInstance));

            /* If we are in a bare metal configuration, then wait till the
               buffer is processed. If we are in RTOS configuration then pend on
               the client semaphore. */

            OSAL_SEM_Pend(&(clientObj->semWriteDone), OSAL_WAIT_FOREVER);

            /* This is the implementation of the blocking behavior. In a
               RTOS configuration, if the code reaches here, it means then
               that buffer has been processed. */

            while(bufferObj->inUse);
            if(bufferObj->nCurrentBytes != nBytes)
            {
                /* This means this buffer was terminated because of an
                   error. */

                return(DRV_USART_WRITE_ERROR);
            }

            count = nBytes;

        }
        else if(clientObj->ioIntent & DRV_IO_INTENT_NONBLOCKING)
        {
            /* This is a non blocking implementation*/

            if(hDriver->queueWrite != NULL)
            {
                /* This means queue is not empty. We cannot send
                   data now. */
                count = 0;
            }
            else
            {
                while((1 == hDriver->moduleId->US_CSR.TXEMPTY) && (count < nBytes))
                {
                    /* This is not a blocking implementation. We write
                       to the hardware till the FIFO is full. */
                    hDriver->moduleId->US_THR.w |= US_THR_TXCHR( data[count] );

                    count ++;

                    /* We need to check for errors. Store the error
                       in the client error field. */

                    clientObj->error = (hDriver->moduleId->US_CSR.w & (US_CSR_OVRE_Msk | US_CSR_FRAME_Msk | US_CSR_PARE_Msk));

                    if(clientObj->error != DRV_USART_ERROR_NONE)
                    {
                        /* This means we have an error. Release the mutex and exit */

                        OSAL_MUTEX_Unlock(&(hDriver->mutexDriverInstance));
                        return(DRV_USART_WRITE_ERROR);
                    }
                }
            }
            /* Release mutex */
            OSAL_MUTEX_Unlock(&(hDriver->mutexDriverInstance));
        }
    }
    else
    {
        /* Write mutex timed out. Set the return count to zero. */
        count = 0;
    }

    return(count);
}


