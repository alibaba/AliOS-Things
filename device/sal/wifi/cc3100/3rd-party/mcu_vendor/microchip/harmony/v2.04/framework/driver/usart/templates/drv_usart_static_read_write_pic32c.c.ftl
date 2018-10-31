/*******************************************************************************
  USART driver static implementation of Read Write model.

  Company:
    Microchip Technology Inc.

  File Name:
    drv_usart_static_read_write.c

  Summary:
    Source code for the USART driver static implementation of File IO data
    transfer model.

  Description:
    This file contains the source code for the static implementation of the
    USART driver File IO data transfer model.

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

// *****************************************************************************
// *****************************************************************************
// Section: Global Data
// *****************************************************************************
// *****************************************************************************
<#if CONFIG_DRV_USART_READ_WRITE_BLOCKING>

/* This is the USART Driver Buffer object */
DRV_USART_BUFFER_OBJ gDrvUSARTBuffer;
</#if>

<#macro DRV_USART_STATIC_FUNCTIONS DRV_INSTANCE HARDWARE_INSTANCE USART_INT_SRC>
extern DRV_USART_OBJ  gDrvUSART${DRV_INSTANCE}Obj ;

/* Global variable to access hardware instance */
<#if HARDWARE_INSTANCE == "USART_ID_0">
    <#assign HARDWARE_INSTANCE_REGISTER = "USART0_Reg">
extern usart_registers_t volatile *USART0_Reg;
<#elseif HARDWARE_INSTANCE == "USART_ID_1">
    <#assign HARDWARE_INSTANCE_REGISTER = "USART1_Reg">
extern usart_registers_t volatile *USART1_Reg;
<#elseif HARDWARE_INSTANCE == "USART_ID_2">
    <#assign HARDWARE_INSTANCE_REGISTER = "USART2_Reg">
extern usart_registers_t volatile *USART2_Reg;
</#if>

// *****************************************************************************
// *****************************************************************************
// Section: Instance ${DRV_INSTANCE} static driver functions
// *****************************************************************************
// *****************************************************************************
size_t DRV_USART${DRV_INSTANCE}_Read(void * destination,const size_t nBytes)
{
    DRV_USART_OBJ *dObj = (DRV_USART_OBJ*)NULL;
<#if CONFIG_DRV_USART_READ_WRITE_BLOCKING>
    DRV_USART_BUFFER_OBJ * iterator, * bufferObj;
<#else>
    uint8_t * data;
</#if>
    size_t count = 0;

    dObj = &gDrvUSART${DRV_INSTANCE}Obj;

    if((destination == NULL) || (nBytes == 0))
    {
        /* We have a NULL pointer or don't have
           any data to write. */

        SYS_DEBUG_MESSAGE(SYS_ERROR_DEBUG, "\r\nUSART Driver: NULL data pointer or no data to write");
        return 0;
    }
<#if CONFIG_USE_3RDPARTY_RTOS>

    /* Grab the hardware instance mutex */
    if(OSAL_MUTEX_Lock(&(dObj->mutexDriverInstance), OSAL_WAIT_FOREVER) == OSAL_RESULT_TRUE)
    {
        /* We were able to take the mutex */
    }
    else
    {
        /* Timed out while waiting for read mutex.
         * We simply return 0 */
        return 0;
    }
</#if>
<#if CONFIG_DRV_USART_READ_WRITE_BLOCKING>

    /* This is a blocking implementation. We populate the client buffer
       object and add it to the queue. We then wait till the buffer is
       completely processed. */
<#if CONFIG_DRV_USART_INTERRUPT_MODE == true>

    /* Because we are modifying the queue, we should disable the
       interrupt */
    SYS_INT_SourceDisable(${USART_INT_SRC});
</#if>

    /* Get the queue head */
    iterator = dObj->queueRead;

    /* If the queue is not empty, get to the end of the queue */
    if(iterator != NULL)
    {
        while(iterator->next != NULL)
        {
        iterator = iterator->next;
        }
    }

    /* We should be at the end of the queue now, Use buffer object */
    bufferObj = &gDrvUSARTBuffer;
    bufferObj->buffer = destination;
    bufferObj->nCurrentBytes = 0;
    bufferObj->size = nBytes;
    bufferObj->inUse = true;
    bufferObj->flags = (DRV_USART_BUFFER_OBJ_FLAG_READ_WRITE);

    /* Add this object to the queue and enable the RX interrupt */
    bufferObj->previous = iterator;
    bufferObj->next = NULL;

    /* If we are not at the start of the queue, then update the next
       pointer of the last object else set the queue head to point to
       this object */
    if(dObj->queueRead == NULL)
    {
        dObj->queueRead = bufferObj;
<#if CONFIG_DRV_USART_INTERRUPT_MODE == true>

        /* Now enable the interrupt */
        ${HARDWARE_INSTANCE_REGISTER}->US_IER.w |= US_IER_RXRDY_Msk;
</#if>
    }
    else
    {
        iterator->next = bufferObj;
        bufferObj->previous = iterator;
    }

    /* Update the queue size as we have added a buffer to the queue */
    dObj->queueSizeCurrentRead ++;
<#if CONFIG_DRV_USART_INTERRUPT_MODE == true>

    /* Now enable the interrupt */
    SYS_INT_SourceEnable(${USART_INT_SRC});
</#if>
<#if CONFIG_USE_3RDPARTY_RTOS>

    OSAL_MUTEX_Unlock(&(dObj->mutexDriverInstance));

    /* If we are in RTOS configuration then pend on
       the client semaphore. */

    OSAL_SEM_Pend(&(dObj->semReadDone), OSAL_WAIT_FOREVER);
    /* This is the implementation of the blocking behaviour. In a
       RTOS configuration, if the code reaches here, it means then
       that buffer has been processed. */

</#if>
    while(bufferObj->inUse);
    if(bufferObj->nCurrentBytes != nBytes)
    {
        /* This means this buffer was terminated because of an
           error. */

        return(DRV_USART_READ_ERROR);
    }

    count = nBytes;
<#else>

    /* This is a non blocking implementation*/
    
    data = (uint8_t *)destination;
<#if CONFIG_DRV_USART_BUFFER_QUEUE_SUPPORT ==true>

    if(dObj->queueRead != NULL)
    {
        /* This means queue is not empty. We cannot read
           data now. */
        count = 0;
    }
    else
    {
        while((1 == ${HARDWARE_INSTANCE_REGISTER}->US_CSR.RXRDY) && (count < nBytes))
        {
            /* We need to check for errors. Store the error
               in the client error field. */
            dObj->error = (${HARDWARE_INSTANCE_REGISTER}->US_CSR.w & (US_CSR_OVRE_Msk | US_CSR_FRAME_Msk | US_CSR_PARE_Msk));

            if(dObj->error != DRV_USART_ERROR_NONE)
            {
                /* This means we have an error.*/
<#if CONFIG_USE_3RDPARTY_RTOS>
                /* Release the mutex and exit */
                OSAL_MUTEX_Unlock(&(dObj->mutexDriverInstance));
</#if>
                /* Clear error condition */
                _DRV_USART${DRV_INSTANCE}_ErrorConditionClear();
                        
                return(DRV_USART_READ_ERROR);
            }
            
            /* This is not a blocking implementation. We read
               the hardware till the FIFO is empty. */
            data[count] = ${HARDWARE_INSTANCE_REGISTER}->US_RHR.RXCHR;
            count ++;
        }
    }
<#else>
    while((1 == ${HARDWARE_INSTANCE_REGISTER}->US_CSR.RXRDY) && (count < nBytes))
    {
        /* We need to check for errors. Store the error
           in the client error field. */
        dObj->error = (${HARDWARE_INSTANCE_REGISTER}->US_CSR.w & (US_CSR_OVRE_Msk | US_CSR_FRAME_Msk | US_CSR_PARE_Msk));

        if(dObj->error != DRV_USART_ERROR_NONE)
        {
            /* This means we have an error.*/
<#if CONFIG_USE_3RDPARTY_RTOS>
            /* Release the mutex and exit */
            OSAL_MUTEX_Unlock(&(dObj->mutexDriverInstance));
</#if>
            /* Clear error condition */
            _DRV_USART${DRV_INSTANCE}_ErrorConditionClear();
                    
            return(DRV_USART_READ_ERROR);
        }
        
        /* This is not a blocking implementation. We read
           the hardware till the FIFO is empty. */
        data[count] = ${HARDWARE_INSTANCE_REGISTER}->US_RHR.RXCHR;
        count ++;
    }
</#if>
<#if CONFIG_USE_3RDPARTY_RTOS>

    /* Unlock the mutex */
    OSAL_MUTEX_Unlock(&(dObj->mutexDriverInstance));
</#if>
</#if>
    return(count);
}

size_t DRV_USART${DRV_INSTANCE}_Write(void * source, const size_t nBytes)
{
    DRV_USART_OBJ *dObj = (DRV_USART_OBJ*)NULL;
<#if CONFIG_DRV_USART_READ_WRITE_BLOCKING>
    DRV_USART_BUFFER_OBJ * iterator, * bufferObj;
</#if>
    size_t count = 0;
    uint8_t * data;

    dObj = &gDrvUSART${DRV_INSTANCE}Obj;

    if((source == NULL) || (nBytes == 0))
    {
        /* We have a NULL pointer or don't have
           any data to write. */

        SYS_DEBUG_MESSAGE(SYS_ERROR_DEBUG, "\r\nUSART Driver: NULL data pointer or no data to write");
        return 0;
    }

    data = (uint8_t *)source;
<#if CONFIG_USE_3RDPARTY_RTOS>

    /* Grab the write mutex */
    if(OSAL_MUTEX_Lock(&(dObj->mutexDriverInstance), OSAL_WAIT_FOREVER) == OSAL_RESULT_TRUE)
    {
        /* We were able to take the mutex */
    }
    else
    {
        /* Write mutex timed out. Set the return count to zero. */
        return 0;
    }
</#if>
<#if CONFIG_DRV_USART_READ_WRITE_BLOCKING>

    /* This is a blocking implementation. We populate the client buffer
       object and add it to the queue. We then wait till the buffer is
       completely processed. */
<#if CONFIG_DRV_USART_INTERRUPT_MODE == true>

    /* Because we are modifying the queue, we should disable the
       interrupt */

    SYS_INT_SourceDisable(${USART_INT_SRC});
</#if>

    /* Get the queue head */
    iterator = dObj->queueWrite;

    /* If the queue is not empty, then get to the
       end of the queue */
    if(iterator != NULL)
    {
        while(iterator->next != NULL)
        {
            iterator = iterator->next;
        }
    }

    /* We should be at the end of the queue now, Use buffer object */
    bufferObj = &gDrvUSARTBuffer;
    bufferObj->buffer = source;
    bufferObj->nCurrentBytes = 0;
    bufferObj->size = nBytes;
    bufferObj->inUse = true;
    bufferObj->flags = (DRV_USART_BUFFER_OBJ_FLAG_READ_WRITE);

    /* Add this object to the queue and enable the RX interrupt */
    bufferObj->previous = iterator;
    bufferObj->next = NULL;

    /* If we are not at the start of the queue, then update the next
       pointer of the last object else set the queue head to point to
       this object */
    if(dObj->queueWrite == NULL)
    {
        dObj->queueWrite = bufferObj;
        bufferObj->nCurrentBytes = 1;
        dObj->queueSizeCurrentWrite ++;
        
        /* Send one byte */
        ${HARDWARE_INSTANCE_REGISTER}->US_THR.w |= US_THR_TXCHR( data[0] );
<#if CONFIG_DRV_USART_INTERRUPT_MODE == true>
        
        /* Enable module TX interrupt source */
        ${HARDWARE_INSTANCE_REGISTER}->US_IER.w |= US_IER_TXEMPTY_Msk;
</#if>
    }
    else
    {
        iterator->next = bufferObj;
        bufferObj->previous = iterator;
        dObj->queueSizeCurrentWrite ++;
    }
<#if CONFIG_DRV_USART_INTERRUPT_MODE == true>

    /* Now enable the interrupt so that the system
       can proceed */
    SYS_INT_SourceEnable(${USART_INT_SRC});
</#if>
<#if CONFIG_USE_3RDPARTY_RTOS>

    /*release the mutex */
    OSAL_MUTEX_Unlock(&(dObj->mutexDriverInstance));

    /* If we are in RTOS configuration then pend on
       the client semaphore. */

    OSAL_SEM_Pend(&(dObj->semWriteDone), OSAL_WAIT_FOREVER);

    /* This is the implementation of the blocking behavior. In a
       RTOS configuration, if the code reaches here, it means then
       that buffer has been processed. */
</#if>

    while(bufferObj->inUse);
    if(bufferObj->nCurrentBytes != nBytes)
    {
        /* This means this buffer was terminated because of an
           error. */
        return(DRV_USART_WRITE_ERROR);
    }

    count = nBytes;
<#else>

    /* This is a non blocking implementation*/
<#if CONFIG_DRV_USART_BUFFER_QUEUE_SUPPORT ==true>

    if(dObj->queueWrite != NULL)
    {
        /* This means queue is not empty. We cannot send
           data now. */
        count = 0;
    }
    else
    {
        while((1 == ${HARDWARE_INSTANCE_REGISTER}->US_CSR.TXEMPTY) && (count < nBytes))
        {
            /* This is not a blocking implementation. We write
               to the hardware till the FIFO is full. */
            ${HARDWARE_INSTANCE_REGISTER}->US_THR.w |= US_THR_TXCHR( data[count] );
            count ++;

            /* We need to check for errors. Store the error
               in the client error field. */

            dObj->error = (${HARDWARE_INSTANCE_REGISTER}->US_CSR.w & (US_CSR_OVRE_Msk | US_CSR_FRAME_Msk | US_CSR_PARE_Msk));

            if(dObj->error != DRV_USART_ERROR_NONE)
            {
                /* This means we have an error.*/
<#if CONFIG_USE_3RDPARTY_RTOS>
                /* Release the mutex and exit */
                OSAL_MUTEX_Unlock(&(dObj->mutexDriverInstance));
</#if>
                return(DRV_USART_WRITE_ERROR);
            }
        }
    }
<#else>
    while((1 == ${HARDWARE_INSTANCE_REGISTER}->US_CSR.TXEMPTY) && (count < nBytes))
    {
        /* This is not a blocking implementation. We write
           to the hardware till the FIFO is full. */
        ${HARDWARE_INSTANCE_REGISTER}->US_THR.w |= US_THR_TXCHR( data[count] );
        count ++;

        /* We need to check for errors. Store the error
           in the client error field. */

        dObj->error = (${HARDWARE_INSTANCE_REGISTER}->US_CSR.w & (US_CSR_OVRE_Msk | US_CSR_FRAME_Msk | US_CSR_PARE_Msk));

        if(dObj->error != DRV_USART_ERROR_NONE)
        {
            /* This means we have an error.*/
<#if CONFIG_USE_3RDPARTY_RTOS>
            /* Release the mutex and exit */
            OSAL_MUTEX_Unlock(&(dObj->mutexDriverInstance));
</#if>
            return(DRV_USART_WRITE_ERROR);
        }
    }
</#if>
</#if>
<#if CONFIG_USE_3RDPARTY_RTOS>

    /* Release mutex */
    OSAL_MUTEX_Unlock(&(dObj->mutexDriverInstance));
</#if>

    return(count);
}

</#macro>
<#if CONFIG_DRV_USART_INST_IDX0 == true>
<@DRV_USART_STATIC_FUNCTIONS DRV_INSTANCE="0"
HARDWARE_INSTANCE=CONFIG_DRV_USART_PERIPHERAL_ID_IDX0
USART_INT_SRC=CONFIG_DRV_USART_INT_SRC_IDX0
/>
</#if>
<#if CONFIG_DRV_USART_INST_IDX1 == true>
<@DRV_USART_STATIC_FUNCTIONS DRV_INSTANCE="1"
HARDWARE_INSTANCE=CONFIG_DRV_USART_PERIPHERAL_ID_IDX1
USART_INT_SRC=CONFIG_DRV_USART_INT_SRC_IDX1
/>
</#if>
<#if CONFIG_DRV_USART_INST_IDX2 == true>
<@DRV_USART_STATIC_FUNCTIONS DRV_INSTANCE="2"
HARDWARE_INSTANCE=CONFIG_DRV_USART_PERIPHERAL_ID_IDX2
USART_INT_SRC=CONFIG_DRV_USART_INT_SRC_IDX2
/>
</#if>
/*******************************************************************************
 End of File
*/
