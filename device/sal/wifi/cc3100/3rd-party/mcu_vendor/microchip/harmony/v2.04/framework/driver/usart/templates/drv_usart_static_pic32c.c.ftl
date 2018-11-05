/*******************************************************************************
  USART Driver Static implementation

  Company:
    Microchip Technology Inc.

  File Name:
    drv_usart_static.c

  Summary:
    Source code for the USART driver static implementation.

  Description:
    The USART device driver provides a simple interface to manage the USART
    modules on Microchip microcontrollers. This file contains static implementation
    for the USART driver.

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

<#macro DRV_USART_STATIC_FUNCTIONS DRV_INSTANCE HARDWARE_INSTANCE OPER_MODE LINE_CONTROL BRG_CLK BRG_RATE USART_INT_SRC>
// *****************************************************************************
// *****************************************************************************
// Section: Global Data
// *****************************************************************************
// *****************************************************************************

/* This is the driver static object . */
DRV_USART_OBJ  gDrvUSART${DRV_INSTANCE}Obj ;

/* Global variable to access hardware instance */
<#if HARDWARE_INSTANCE == "USART_ID_0">
    <#assign HARDWARE_INSTANCE_REGISTER = "USART0_Reg">
usart_registers_t volatile *USART0_Reg = (usart_registers_t *)USART_ID_0;
<#elseif HARDWARE_INSTANCE == "USART_ID_1">
    <#assign HARDWARE_INSTANCE_REGISTER = "USART1_Reg">
usart_registers_t volatile *USART1_Reg = (usart_registers_t *)USART_ID_1;
<#elseif HARDWARE_INSTANCE == "USART_ID_2">
    <#assign HARDWARE_INSTANCE_REGISTER = "USART2_Reg">
usart_registers_t volatile *USART2_Reg = (usart_registers_t *)USART_ID_2;
</#if>

// *****************************************************************************
// *****************************************************************************
// Section: USART Driver Interface Implementations
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    SYS_MODULE_OBJ DRV_USART${DRV_INSTANCE}_Initialize(void)

  Summary:
    Dynamic implementation of DRV_USART${DRV_INSTANCE}_Initialize system interface function.

  Description:
    This is the dynamic implementation of DRV_USART${DRV_INSTANCE}_Initialize system interface
    function.

  Remarks:
    See drv_usart.h for usage information.
*/

SYS_MODULE_OBJ DRV_USART${DRV_INSTANCE}_Initialize(void)
{
    DRV_USART_OBJ *dObj = &gDrvUSART${DRV_INSTANCE}Obj;

    /* Initialize the instance object parameters. */
<#if CONFIG_DRV_USART_BUFFER_QUEUE_SUPPORT ==true || (CONFIG_DRV_USART_READ_WRITE_MODEL_SUPPORT ==true && CONFIG_DRV_USART_READ_WRITE_BLOCKING==true)>

    /* Update the USART OBJECT parameters. */
    dObj->interruptNestingCount = 0;
    dObj->queueSizeCurrentRead  = 0;
    dObj->queueSizeCurrentWrite = 0;
    dObj->queueRead             = NULL;
    dObj->queueWrite            = NULL;
    dObj->eventHandler          = NULL;
    dObj->context               = (uintptr_t)NULL;
</#if>
    dObj->error                 = DRV_USART_ERROR_NONE;
<#if CONFIG_DRV_USART_BYTE_MODEL_SUPPORT ==true && CONFIG_DRV_USART_BYTE_MODEL_CALLBACK ==true>
    dObj->transmitCallback      = NULL;
    dObj->receiveCallback       = NULL;
    dObj->errorCallback         = NULL;
</#if>
    
    /* Setup the Hardware */
    _DRV_USART${DRV_INSTANCE}_HardwareSetup();

<#if CONFIG_USE_3RDPARTY_RTOS>
    /* Create the hardware instance mutex. */
    if(OSAL_MUTEX_Create(&(dObj->mutexDriverInstance)) != OSAL_RESULT_TRUE)
    {
        return SYS_MODULE_OBJ_INVALID;
    }

</#if>
<#if CONFIG_DRV_USART_INTERRUPT_MODE == true>
    /* Enable the system interrupt flag */
    SYS_INT_SourceEnable(${USART_INT_SRC});
    
</#if>     
    /* Enable the RX and TX*/
    ${HARDWARE_INSTANCE_REGISTER}->US_CR.w |= (US_CR_RXEN_Msk | US_CR_TXEN_Msk);

    /* Return the object structure */
    return (SYS_MODULE_OBJ)DRV_USART_INDEX_${DRV_INSTANCE};
}

// *****************************************************************************
/* Function:
    void DRV_USART${DRV_INSTANCE}_Deinitialize( void )

  Summary:
    Dynamic implementation of DRV_USART${DRV_INSTANCE}_Deinitialize system interface function.

  Description:
    This is the dynamic implementation of DRV_USART${DRV_INSTANCE}_Deinitialize system interface
    function.

  Remarks:
    See drv_usart.h for usage information.
*/

void  DRV_USART${DRV_INSTANCE}_Deinitialize(void)
{
<#if CONFIG_USE_3RDPARTY_RTOS>
    DRV_USART_OBJ * dObj = &gDrvUSART${DRV_INSTANCE}Obj;

    /* Deallocate all mutexes */
    if(OSAL_MUTEX_Delete(&(dObj->mutexDriverInstance)) != OSAL_RESULT_TRUE)
    {
        SYS_DEBUG(0, "Mutex Delete Failed");
        return;
    }
    
</#if>    
<#if CONFIG_DRV_USART_INTERRUPT_MODE == true>
    /* Disable all module level sources for interrupt */
    ${HARDWARE_INSTANCE_REGISTER}->US_IDR.w |= ((US_IDR_OVRE_Msk) | (US_IDR_FRAME_Msk) | (US_IDR_PARE_Msk) | (US_IDR_TXEMPTY_Msk) | (US_IDR_RXRDY_Msk));
    
    /* Disable the system interrupt */
    SYS_INT_SourceDisable(${USART_INT_SRC}) ;

</#if>
    /* Disable USART RX and TX */
    ${HARDWARE_INSTANCE_REGISTER}->US_CR.w |= (US_CR_RXDIS_Msk | US_CR_TXDIS_Msk);

    return;
}

// *****************************************************************************
/* Function:
    SYS_STATUS DRV_USART${DRV_INSTANCE}_Status( void )

  Summary:
    Dynamic implementation of DRV_USART${DRV_INSTANCE}_Status system interface function.

  Description:
    This is the dynamic implementation of DRV_USART${DRV_INSTANCE}_Status system interface
    function.

  Remarks:
    See drv_usart.h for usage information.
*/

SYS_STATUS DRV_USART${DRV_INSTANCE}_Status( void )
{
    /* Return the status as ready always */
    return SYS_STATUS_READY;
}

// *****************************************************************************
/* Function:
    DRV_HANDLE DRV_USART${DRV_INSTANCE}_Open( const SYS_MODULE_INDEX index,
                               const DRV_IO_INTENT    ioIntent )

  Summary:
    Dynamic implementation of DRV_USART${DRV_INSTANCE}_Open client interface function.

  Description:
    This is the dynamic implementation of DRV_USART${DRV_INSTANCE}_Open client interface
    function.

  Remarks:
    See drv_usart.h for usage information.
*/

DRV_HANDLE DRV_USART${DRV_INSTANCE}_Open
(
    const SYS_MODULE_INDEX drvIndex,
    const DRV_IO_INTENT ioIntent
)
{
<#if CONFIG_DRV_USART_READ_WRITE_MODEL_SUPPORT ==true && CONFIG_USE_3RDPARTY_RTOS ==true>
    DRV_USART_OBJ *dObj = &gDrvUSART${DRV_INSTANCE}Obj;

    /* Create the semaphores for Read Operation of Read-Write Model*/
    if(OSAL_SEM_Create(&(dObj->semReadDone), OSAL_SEM_TYPE_COUNTING, 1, 0) != OSAL_RESULT_TRUE)
    {
        SYS_DEBUG_MESSAGE(SYS_ERROR_DEBUG, "\r\nUSART Driver: Semaphore creation failed");
        return (DRV_HANDLE_INVALID);
    }
    /* Create the semaphores for Write Operation of Read-Write Model*/
    if(OSAL_SEM_Create(&(dObj->semWriteDone), OSAL_SEM_TYPE_COUNTING, 1, 0) != OSAL_RESULT_TRUE)
    {
        SYS_DEBUG_MESSAGE(SYS_ERROR_DEBUG, "\r\nUSART Driver: Semaphore creation failed");
        return (DRV_HANDLE_INVALID);
    }

</#if>

    /* Return the driver instance value*/
    return ((DRV_HANDLE)DRV_USART_INDEX_${DRV_INSTANCE} );
}

// *****************************************************************************
/* Function:
    DRV_CLIENT_STATUS DRV_USART${DRV_INSTANCE}_Close ( void )

  Summary:
    Dynamic implementation of DRV_USART${DRV_INSTANCE}_Close client interface function.

  Description:
    This is the dynamic implementation of DRV_USART${DRV_INSTANCE}_Close client interface
    function.

  Remarks:
    See drv_usart.h for usage information.
*/

void DRV_USART${DRV_INSTANCE}_Close( void )
{
<#if CONFIG_DRV_USART_READ_WRITE_MODEL_SUPPORT ==true && CONFIG_USE_3RDPARTY_RTOS==true>
    DRV_USART_OBJ *dObj = &gDrvUSART${DRV_INSTANCE}Obj;

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
    return;
}

// *****************************************************************************
/* Function:
    DRV_USART_CLIENT_STATUS DRV_USART${DRV_INSTANCE}_ClientStatus ( void )

  Summary:
    Dynamic implementation of DRV_USART${DRV_INSTANCE}_ClientStatus client interface function.

  Description:
    This is the dynamic implementation of DRV_USART${DRV_INSTANCE}_ClientStatus client interface
    function.

  Remarks:
    See drv_usart.h for usage information.
*/

DRV_USART_CLIENT_STATUS DRV_USART${DRV_INSTANCE}_ClientStatus( void )
{
    /* Return the status as ready always*/
    return DRV_USART_CLIENT_STATUS_READY;
}

// *****************************************************************************
/* Function:
    void DRV_USART${DRV_INSTANCE}_TasksTransmit ( void )

  Summary:
    Dynamic implementation of DRV_USART${DRV_INSTANCE}_TasksTransmit system interface function.

  Description:
    This is the dynamic implementation of DRV_USART${DRV_INSTANCE}_TasksTransmit system interface
    function.

  Remarks:
    See drv_usart.h for usage information.
*/

void DRV_USART${DRV_INSTANCE}_TasksTransmit( void )
{
    /* This is the USART Driver Transmit tasks routine.
       In this function, the driver checks if a transmit
       flag is active and performs respective action*/

    /* Reading the transmit interrupt flag */
    if( 1 == ${HARDWARE_INSTANCE_REGISTER}->US_CSR.TXEMPTY )
    {
<#if CONFIG_DRV_USART_BUFFER_QUEUE_SUPPORT ==true || (CONFIG_DRV_USART_READ_WRITE_MODEL_SUPPORT ==true && CONFIG_DRV_USART_READ_WRITE_BLOCKING==true)>
        /* The USART driver is configured to generate an
           interrupt when the FIFO is empty. Additionally
           the queue is not empty. Which means there is
           work to be done in this routine. */
        _DRV_USART${DRV_INSTANCE}_BufferQueueTxTasks();
<#elseif CONFIG_DRV_USART_BYTE_MODEL_SUPPORT ==true && CONFIG_DRV_USART_BYTE_MODEL_CALLBACK ==true>
        /* Call the Transmit callback function*/
        _DRV_USART${DRV_INSTANCE}_ByteTransmitTasks ();
<#elseif CONFIG_DRV_USART_BYTE_MODEL_SUPPORT ==true && CONFIG_DRV_USART_BYTE_MODEL_BLOCKING ==true>
        /* Disable the TX interrupt source, 
         * to avoid calling ISR continuously.
         * Interrupt will be enabled when
         * DRV_USART${DRV_INSTANCE}_WriteByte is called again. */
        ${HARDWARE_INSTANCE_REGISTER}->US_IDR.w |= US_IDR_TXEMPTY_Msk;
<#elseif CONFIG_DRV_USART_READ_WRITE_MODEL_SUPPORT ==true && CONFIG_DRV_USART_READ_WRITE_BLOCKING==false>
        /* Disable the TX interrupt source, 
         * to avoid calling ISR continuously. */
        ${HARDWARE_INSTANCE_REGISTER}->US_IDR.w |= US_IDR_TXEMPTY_Msk;
</#if>
    }
}


// *****************************************************************************
/* Function:
    void DRV_USART${DRV_INSTANCE}_TasksError ( void );

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

void DRV_USART${DRV_INSTANCE}_TasksError( void )
{
<#if CONFIG_DRV_USART_BYTE_MODEL_SUPPORT ==true && CONFIG_DRV_USART_BYTE_MODEL_BLOCKING ==true>
    DRV_USART_OBJ * hDriver = &gDrvUSART${DRV_INSTANCE}Obj;
    
</#if>
    /* This is the USART Driver Error tasks routine. In this function, the
     * driver checks if an error interrupt has occurred. If so the error
     * condition is cleared.  */

    /* Process the overflow/framing/parity errors if any */
    if ( (1 == ${HARDWARE_INSTANCE_REGISTER}->US_CSR.OVRE) || (1 == ${HARDWARE_INSTANCE_REGISTER}->US_CSR.FRAME) || (1 == ${HARDWARE_INSTANCE_REGISTER}->US_CSR.PARE) )
    {
        /* This means an error has occurred */
<#if CONFIG_DRV_USART_BUFFER_QUEUE_SUPPORT ==true || (CONFIG_DRV_USART_READ_WRITE_MODEL_SUPPORT ==true && CONFIG_DRV_USART_READ_WRITE_BLOCKING==true)>
        _DRV_USART${DRV_INSTANCE}_BufferQueueErrorTasks();
<#elseif CONFIG_DRV_USART_BYTE_MODEL_SUPPORT ==true && CONFIG_DRV_USART_BYTE_MODEL_CALLBACK ==true>
        /* Call the Error callback function*/
        _DRV_USART${DRV_INSTANCE}_ByteErrorTasks ();
<#elseif CONFIG_DRV_USART_BYTE_MODEL_SUPPORT ==true && CONFIG_DRV_USART_BYTE_MODEL_BLOCKING ==true>
         /* Update the USART client error flag */
         hDriver->error = (${HARDWARE_INSTANCE_REGISTER}->US_CSR.w & (US_CSR_OVRE_Msk | US_CSR_FRAME_Msk | US_CSR_PARE_Msk));
         
        /* It's a byte model blocking operation.
         * So, it's applications responsibility to read the 
         * data within ISR if interrupt mode is selected.
         * If data is not read then it may cause overrun error.
         * For now driver clears error condition */
        _DRV_USART${DRV_INSTANCE}_ErrorConditionClear();
<#elseif CONFIG_DRV_USART_READ_WRITE_MODEL_SUPPORT ==true && CONFIG_DRV_USART_READ_WRITE_BLOCKING==false>
        /* Clear error condition. */
        _DRV_USART${DRV_INSTANCE}_ErrorConditionClear();
</#if>
    }
}

// *****************************************************************************
/* Function:
    void DRV_USART${DRV_INSTANCE}_TasksReceive ( void )

  Summary:
    Dynamic implementation of DRV_USART${DRV_INSTANCE}_TasksReceive system interface function.

  Description:
    This is the dynamic implementation of DRV_USART${DRV_INSTANCE}_TasksReceive system interface
    function.

  Remarks:
    See drv_usart.h for usage information.
*/

void DRV_USART${DRV_INSTANCE}_TasksReceive( void )
{
    /* This is the USART Driver Receive tasks routine. If the receive
       interrupt flag is set, the tasks routines are executed.
     */
    if( 1 == ${HARDWARE_INSTANCE_REGISTER}->US_CSR.RXRDY )
    {
<#if CONFIG_DRV_USART_BUFFER_QUEUE_SUPPORT ==true || (CONFIG_DRV_USART_READ_WRITE_MODEL_SUPPORT ==true && CONFIG_DRV_USART_READ_WRITE_BLOCKING==true)>
        _DRV_USART${DRV_INSTANCE}_BufferQueueRxTasks();
<#elseif CONFIG_DRV_USART_BYTE_MODEL_SUPPORT ==true && CONFIG_DRV_USART_BYTE_MODEL_CALLBACK ==true>
        /* Call the Receive callback function*/
        _DRV_USART${DRV_INSTANCE}_ByteReceiveTasks ();
<#elseif CONFIG_DRV_USART_BYTE_MODEL_SUPPORT ==true && CONFIG_DRV_USART_BYTE_MODEL_BLOCKING ==true>
        /* It's a byte model blocking operation.
         * So, it's applications responsibility to read the 
         * data within ISR if interrupt mode is selected.
         * If data is not read then it may cause overrun error */
        return;
<#elseif CONFIG_DRV_USART_READ_WRITE_MODEL_SUPPORT ==true && CONFIG_DRV_USART_READ_WRITE_BLOCKING==false>
        /* Do nothing. */
        return;
</#if>
    }
}

// *****************************************************************************
/* Function:
    DRV_USART_TRANSFER_STATUS DRV_USART${DRV_INSTANCE}_TransferStatus( void )

  Summary:
    Dynamic implementation of DRV_USART${DRV_INSTANCE}_TransferStatus client interface function.

  Description:
    This is the dynamic implementation of DRV_USART${DRV_INSTANCE}_TransferStatus client
    interface function.

  Remarks:
    See drv_usart.h for usage information.
*/

DRV_USART_TRANSFER_STATUS DRV_USART${DRV_INSTANCE}_TransferStatus(void)
{
    DRV_USART_TRANSFER_STATUS result = 0;
    
    /* If the Rx interrupt flag is set then there is data available
       else there isn't */
    if(1 == ${HARDWARE_INSTANCE_REGISTER}->US_CSR.RXRDY)
    {
        result |= DRV_USART_TRANSFER_STATUS_RECEIVER_DATA_PRESENT;
    }
    else
    {
        result |= DRV_USART_TRANSFER_STATUS_RECEIVER_EMPTY;
    }
    
    /* If the TX interrupt flag is set, then the Tx Buffer is empty */
    if(1 == ${HARDWARE_INSTANCE_REGISTER}->US_CSR.TXEMPTY)
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
    DRV_USART_ERROR DRV_USART${DRV_INSTANCE}_ErrorGet( void )

  Summary:
    Dynamic implementation of DRV_USART${DRV_INSTANCE}_ErrorGet client interface function.

  Description:
    This is the dynamic implementation of DRV_USART${DRV_INSTANCE}_ErrorGet client interface
    function.

  Remarks:
    See drv_usart.h for usage information.
*/

DRV_USART_ERROR DRV_USART${DRV_INSTANCE}_ErrorGet( void )
{
    DRV_USART_ERROR error;
    error = gDrvUSART${DRV_INSTANCE}Obj.error;

    /* Clear the error before returning */
    gDrvUSART${DRV_INSTANCE}Obj.error = DRV_USART_ERROR_NONE;

    /* Return the error*/
    return(error);
}

// *****************************************************************************
/* Function:
    DRV_USART_BAUD_SET_RESULT DRV_USART${DRV_INSTANCE}_BaudSet(uint32_t baud)

  Summary:
    Dynamic implementation of DRV_USART${DRV_INSTANCE}_BaudSet client interface function.

  Description:
    This is the dynamic implementation of DRV_USART${DRV_INSTANCE}_BaudSet client interface
    function.

  Remarks:
    See drv_usart.h for usage information.
*/

DRV_USART_BAUD_SET_RESULT DRV_USART${DRV_INSTANCE}_BaudSet(uint32_t baud)
{
    uint32_t cd;

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
    /* Calculate and set baud rate, 
     * choose oversampling for RX pin based on requirement. */
    if (${BRG_CLK} >= (_DRV_USART_HIGH_FRQ_SAMPLE_DIV * baud) ) 
    {
        /* Disable over sampling*/
        ${HARDWARE_INSTANCE_REGISTER}->US_MR.w &= ~US_MR_OVER_Msk;
        cd = ${BRG_CLK}/(_DRV_USART_HIGH_FRQ_SAMPLE_DIV * baud);
    } 
    else 
    {
        /* Enable over sampling*/
        ${HARDWARE_INSTANCE_REGISTER}->US_MR.w |= US_MR_OVER_Msk;
        cd = ${BRG_CLK}/(_DRV_USART_LOW_FRQ_SAMPLE_DIV * baud);
    }

    /* Configure the baudrate configure register. */
    ${HARDWARE_INSTANCE_REGISTER}->US_BRGR.w = ( (${HARDWARE_INSTANCE_REGISTER}->US_BRGR.w & (~US_BRGR_CD_Msk)) | US_BRGR_CD(cd) );

<#if CONFIG_USE_3RDPARTY_RTOS>
    /* Unlock Mutex */
    OSAL_MUTEX_Unlock(&(gDrvUSART${DRV_INSTANCE}Obj.mutexDriverInstance));

</#if>
    return DRV_USART_BAUD_SET_SUCCESS;
}

// *****************************************************************************
/* Function:
    DRV_USART_LINE_CONTROL_SET_RESULT DRV_USART${DRV_INSTANCE}_LineControlSet
    (
        DRV_USART_LINE_CONTROL lineControlMode
    )

  Summary:
    Dynamic implementation of DRV_USART${DRV_INSTANCE}_LineControlSet client interface function.

  Description:
    This is the dynamic implementation of DRV_USART${DRV_INSTANCE}_LineControlSet client
    interface function.

  Remarks:
    See drv_usart.h for usage information.
*/

DRV_USART_LINE_CONTROL_SET_RESULT DRV_USART${DRV_INSTANCE}_LineControlSet(DRV_USART_LINE_CONTROL lineControlMode)
{
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
        return DRV_USART_LINE_CONTROL_SET_ERROR;
    }
    
</#if>
    /* Set the line control mode */
    
    /* Set parity, stop bits and default 8 bit character mode */
    ${HARDWARE_INSTANCE_REGISTER}->US_MR.w = ( (${HARDWARE_INSTANCE_REGISTER}->US_MR.w & ~(US_MR_CHRL_Msk|US_MR_PAR_Msk|US_MR_NBSTOP_Msk)) | lineControlMode );
    
    /* Enable 9 bit character length if 9 bit mode is selected */
    if( !(lineControlMode & US_MR_CHRL_8_BIT) )
    {
        /* Enable 9 bit mode, otherwise by default 8 bit mode */
        ${HARDWARE_INSTANCE_REGISTER}->US_MR.w |= US_MR_MODE9_Msk;
    }

<#if CONFIG_USE_3RDPARTY_RTOS>
    /* Unlock Mutex */
    OSAL_MUTEX_Unlock(&(gDrvUSART${DRV_INSTANCE}Obj.mutexDriverInstance));

</#if>

    /* Return success */
    return DRV_USART_LINE_CONTROL_SET_SUCCESS;
}

// *****************************************************************************
// *****************************************************************************
// Section: File scope functions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* void _DRV_USART${DRV_INSTANCE}_HardwareSetup(void)

  Summary:
    Initializes the USART module based on the init data structure.

  Description:
    This function initializes the USART module based on the init data structure.

  Remarks:
    This is a private function and should not be called directly by the
    application.
*/

void _DRV_USART${DRV_INSTANCE}_HardwareSetup(void)
{
    uint32_t cd;
    
    /* Reset USART mode register */
    ${HARDWARE_INSTANCE_REGISTER}->US_MR.w = 0;
    
    /* Disable the USART module to configure it */
    ${HARDWARE_INSTANCE_REGISTER}->US_CR.w |= (US_CR_RXDIS_Msk | US_CR_TXDIS_Msk);
    
    /* Reset USART status flags register */
    ${HARDWARE_INSTANCE_REGISTER}->US_CR.w |= US_CR_RSTSTA_Msk;

    /* Set selected operation mode */
<#if OPER_MODE == "DRV_USART_OPERATION_MODE_NORMAL">
    ${HARDWARE_INSTANCE_REGISTER}->US_MR.w |= US_MR_USART_MODE(US_MR_USART_MODE_NORMAL_Val);
<#elseif OPER_MODE == "DRV_USART_OPERATION_MODE_ADDRESSED">
    ${HARDWARE_INSTANCE_REGISTER}->US_MR.w |= US_MR_USART_MODE(US_MR_USART_MODE_RS485_Val);
<#elseif OPER_MODE == "DRV_USART_OPERATION_MODE_HANDSHAKE">
    ${HARDWARE_INSTANCE_REGISTER}->US_MR.w |= US_MR_USART_MODE(US_MR_USART_MODE_HW_HANDSHAKING_Val);
<#elseif OPER_MODE == "DRV_USART_OPERATION_MODE_IRDA">
    ${HARDWARE_INSTANCE_REGISTER}->US_MR.w |= US_MR_USART_MODE(US_MR_USART_MODE_IRDA_Val);
<#elseif OPER_MODE == "DRV_USART_OPERATION_MODE_LOOPBACK">
    ${HARDWARE_INSTANCE_REGISTER}->US_MR.w |= US_MR_USART_MODE(US_MR_USART_MODE_NORMAL_Val);
    ${HARDWARE_INSTANCE_REGISTER}->US_MR.w |= US_MR_CHMODE(US_MR_CHMODE_LOCAL_LOOPBACK_Val);
</#if>

    /* Set the line control mode */
    
    /* Set parity, stop bits and default 8 bit character mode */
    ${HARDWARE_INSTANCE_REGISTER}->US_MR.w = ( (${HARDWARE_INSTANCE_REGISTER}->US_MR.w & ~(US_MR_CHRL_Msk|US_MR_PAR_Msk|US_MR_NBSTOP_Msk)) | ${LINE_CONTROL} );
    
<#if LINE_CONTROL == "DRV_USART_LINE_CONTROL_9NONE1" || LINE_CONTROL == "DRV_USART_LINE_CONTROL_9NONE2">
    /* Enable 9 bit mode, otherwise by default 8 bit mode */
    ${HARDWARE_INSTANCE_REGISTER}->US_MR.w |= US_MR_MODE9_Pos;
</#if>
    
    /* Select clock source.
     * By default it's peripheral clock(MCK) for now. */
    /* TODO: Implement clock source selection */
    ${HARDWARE_INSTANCE_REGISTER}->US_MR.w |= US_MR_USCLKS( US_MR_USCLKS_MCK_Val);
    
    /* Calculate and set baud rate, 
     * choose oversampling for RX pin based on requirement. */
    if (${BRG_CLK} >= (_DRV_USART_HIGH_FRQ_SAMPLE_DIV * ${BRG_RATE}) ) 
    {
        /* Disable over sampling*/
        ${HARDWARE_INSTANCE_REGISTER}->US_MR.w &= ~US_MR_OVER_Msk; 
        cd = ${BRG_CLK}/(_DRV_USART_HIGH_FRQ_SAMPLE_DIV * ${BRG_RATE});
    } 
    else 
    {
        /* Enable over sampling*/
        ${HARDWARE_INSTANCE_REGISTER}->US_MR.w |= US_MR_OVER_Msk;
        cd = ${BRG_CLK}/(_DRV_USART_LOW_FRQ_SAMPLE_DIV * ${BRG_RATE});
    }

    /* Configure the baudrate configure register. */
    ${HARDWARE_INSTANCE_REGISTER}->US_BRGR.w = ( (${HARDWARE_INSTANCE_REGISTER}->US_BRGR.w & (~US_BRGR_CD_Msk)) | US_BRGR_CD(cd) );
<#if CONFIG_DRV_USART_INTERRUPT_MODE == true>
<#if CONFIG_DRV_USART_READ_WRITE_MODEL_SUPPORT ==false && CONFIG_DRV_USART_READ_WRITE_BLOCKING==false>
    /* Enable module level sources for interrupt. 
     * viz. RX ready, overflow error , framing error, parity error.
     * TX interrupt source will be enabled while initiating transmit.
     * RX interrupt source will be enabled by default for byte model only,
     * for other models it is taken care while adding read requests */
<#if CONFIG_DRV_USART_BYTE_MODEL_SUPPORT == true>
    ${HARDWARE_INSTANCE_REGISTER}->US_IER.w |= (US_IER_OVRE_Msk | US_IER_FRAME_Msk | US_IER_PARE_Msk | US_IER_RXRDY_Msk);
<#else>
    ${HARDWARE_INSTANCE_REGISTER}->US_IER.w |= (US_IER_OVRE_Msk | US_IER_FRAME_Msk | US_IER_PARE_Msk);
</#if>
</#if>
</#if>
}

// *****************************************************************************
// *****************************************************************************
// Section: File scope functions
// *****************************************************************************
// *****************************************************************************
<#if CONFIG_DRV_USART_BUFFER_QUEUE_SUPPORT ==true || (CONFIG_DRV_USART_READ_WRITE_MODEL_SUPPORT ==true && CONFIG_DRV_USART_READ_WRITE_BLOCKING==true)>

void _DRV_USART${DRV_INSTANCE}_BufferQueueRxTasks(void)
{
    DRV_USART_OBJ * hDriver = &gDrvUSART${DRV_INSTANCE}Obj;
    DRV_USART_BUFFER_OBJ * bufferObj;

<#if CONFIG_USE_3RDPARTY_RTOS>
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
</#if>

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
        while((1 == ${HARDWARE_INSTANCE_REGISTER}->US_CSR.RXRDY) && (bufferObj->nCurrentBytes < bufferObj->size ))
        {
            bufferObj->buffer[bufferObj->nCurrentBytes] = ${HARDWARE_INSTANCE_REGISTER}->US_RHR.RXCHR;
            bufferObj->nCurrentBytes ++;
        }

        /* Check if this buffer is done */
        if(bufferObj->nCurrentBytes >= bufferObj->size)
        {
            /* This means the buffer is completed. If there
               is a callback registered with client, then
               call it */

            if((hDriver->eventHandler != NULL) && (bufferObj->flags & DRV_USART_BUFFER_OBJ_FLAG_BUFFER_ADD))
            {
                /* Call the event handler. We additionally increment the
                   interrupt nesting count which lets the driver functions
                   that are called from the event handler know that an
                   interrupt context is active.
                   */

                hDriver->interruptNestingCount ++;

                hDriver->eventHandler(DRV_USART_BUFFER_EVENT_COMPLETE,
                        bufferObj->bufferHandle,
                        hDriver->context);

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

<#if CONFIG_DRV_USART_READ_WRITE_MODEL_SUPPORT ==true>
<#if CONFIG_USE_3RDPARTY_RTOS>
            /* Check if read write mode and post semaphore */
            if(bufferObj->flags & DRV_USART_BUFFER_OBJ_FLAG_READ_WRITE)
            {
                /* This means we should post the semaphore */
                _DRV_USART_SEM_POST(&(hDriver->semReadDone));
            }
</#if>
</#if>
        }
    }
<#if CONFIG_DRV_USART_INTERRUPT_MODE == true>

    if(hDriver->queueRead == NULL)
    {
        /* The queue is empty. We can disable the interrupt */
        ${HARDWARE_INSTANCE_REGISTER}->US_IDR.w |= US_IDR_RXRDY_Msk;
    }
</#if>
<#if CONFIG_USE_3RDPARTY_RTOS>

    /* Release the mutex */
   _DRV_USART_RELEASE_MUTEX(&(hDriver->mutexDriverInstance));
</#if>
}

void _DRV_USART${DRV_INSTANCE}_BufferQueueTxTasks(void)
{
    DRV_USART_OBJ *hDriver = &gDrvUSART${DRV_INSTANCE}Obj;
    DRV_USART_BUFFER_OBJ * bufferObj;

    /* Start by getting the buffer at the head of queue. */
    bufferObj = hDriver->queueWrite;
<#if CONFIG_USE_3RDPARTY_RTOS>

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
</#if>

    if(bufferObj != NULL)
    {
        /* This means the queue is not empty. Check if this buffer is done */
        if(bufferObj->nCurrentBytes >= bufferObj->size)
        {
            /* This means the buffer is completed. If there
               is a callback registered with client, then
               call it */

            if((hDriver->eventHandler != NULL) && (bufferObj->flags & DRV_USART_BUFFER_OBJ_FLAG_BUFFER_ADD))
            {
                /* Before calling the event handler, the interrupt nesting
                   counter is incremented. This will allow driver routine that
                   are called from the event handler to know the interrupt
                   nesting level. Events are only generated for buffers that
                   were submitted using the buffer add routine */

                hDriver->interruptNestingCount ++;

                hDriver->eventHandler(DRV_USART_BUFFER_EVENT_COMPLETE,
                        bufferObj->bufferHandle,
                        hDriver->context);

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

<#if CONFIG_DRV_USART_READ_WRITE_MODEL_SUPPORT ==true>
<#if CONFIG_USE_3RDPARTY_RTOS>
            /* Check if read write mode and post semaphore */
            if(bufferObj->flags & DRV_USART_BUFFER_OBJ_FLAG_READ_WRITE)
            {
                /* This means we should post the semaphore */
                _DRV_USART_SEM_POST(&(hDriver->semWriteDone));

            }
</#if>
</#if>
        }
    }

    /* Check if the queue is still not empty and process
       the buffer */

    if(hDriver->queueWrite != NULL)
    {
        bufferObj = hDriver->queueWrite;
 
        /* Submit data to send if TX is empty*/
        while((1 == ${HARDWARE_INSTANCE_REGISTER}->US_CSR.TXEMPTY) && (bufferObj->nCurrentBytes < bufferObj->size ))
        {
            /* Send data */
            ${HARDWARE_INSTANCE_REGISTER}->US_THR.w |= US_THR_TXCHR( bufferObj->buffer[bufferObj->nCurrentBytes]);

            bufferObj->nCurrentBytes ++;
        }
    }
<#if CONFIG_DRV_USART_INTERRUPT_MODE == true>
    else
    {
        /* If the queue is empty, then disable the TX interrupt */
        ${HARDWARE_INSTANCE_REGISTER}->US_IDR.w |= US_IDR_TXEMPTY_Msk;
    }
</#if>

<#if CONFIG_USE_3RDPARTY_RTOS>

    /* Release the mutex */
    _DRV_USART_RELEASE_MUTEX(&(hDriver->mutexDriverInstance));
</#if>
}

void _DRV_USART${DRV_INSTANCE}_BufferQueueErrorTasks(void)
{
    DRV_USART_OBJ *hDriver = &gDrvUSART${DRV_INSTANCE}Obj;
    DRV_USART_BUFFER_OBJ * bufferObj;
    bool mutexGrabbed  = true;

    /* USART driver will take care that TX erros( like overflow etc) are not 
     * going to occur at any time based on checks before write.
     * So, only RX errors are to be handled/reported */
    
    /* Get the RX buffer at the head */
    bufferObj = hDriver->queueRead;
<#if CONFIG_USE_3RDPARTY_RTOS>

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
</#if>

    /* mutexGrabbed will always be true for non-RTOS case.
     * Will be false when mutex aquisition timed out in RTOS mode */
    if(true == mutexGrabbed)
    {
            
        if(bufferObj != NULL)
        {
            
            /* Update the USART client error flag */
            hDriver->error = (${HARDWARE_INSTANCE_REGISTER}->US_CSR.w & (US_CSR_OVRE_Msk | US_CSR_FRAME_Msk | US_CSR_PARE_Msk));

            /* Clear error condition */
            _DRV_USART${DRV_INSTANCE}_ErrorConditionClear();

            /* Call event handler in Buffer queue request only.
             * If it's a File i/o request then DRV_USART_Read call itself 
             * will return with error result */
            if((hDriver->eventHandler != NULL) && (bufferObj->flags & DRV_USART_BUFFER_OBJ_FLAG_BUFFER_ADD))
            {
                /* Call the event handler with buffer event error state */
                hDriver->eventHandler(DRV_USART_BUFFER_EVENT_ERROR,
                        bufferObj->bufferHandle,
                        hDriver->context);
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
    _DRV_USART_RELEASE_MUTEX(&(hDriver->mutexDriverInstance));
</#if>
}
</#if>

void _DRV_USART${DRV_INSTANCE}_ErrorConditionClear(void)
{
    uint8_t dummyData = 0u;
        
     /* Clear all error flags */
    ${HARDWARE_INSTANCE_REGISTER}->US_CR.w |= US_CR_RSTSTA_Msk;

    /* Read existing error bytes from RX */
    if( 1 == ${HARDWARE_INSTANCE_REGISTER}->US_CSR.RXRDY )
    {
        dummyData = ${HARDWARE_INSTANCE_REGISTER}->US_RHR.RXCHR;
    }
    
    /* Ignore the warning */
    (void)dummyData;
}

<#if CONFIG_DRV_USART_BYTE_MODEL_SUPPORT ==true && CONFIG_DRV_USART_BYTE_MODEL_CALLBACK ==true>
void _DRV_USART${DRV_INSTANCE}_ByteTransmitTasks (void)
{
    DRV_USART_OBJ * hDriver = &gDrvUSART${DRV_INSTANCE}Obj;

    /* Call event handler only if interrupt mode is selected for TX */
    if (1 == ${HARDWARE_INSTANCE_REGISTER}->US_IMR.TXEMPTY)
    {
<#if CONFIG_DRV_USART_INTERRUPT_MODE == true>
        /* Disable the TX interrupt and TX functionality, 
         * to avoid calling ISR continuously */
        ${HARDWARE_INSTANCE_REGISTER}->US_IDR.w |= US_IDR_TXEMPTY_Msk;

</#if>

        if (hDriver->transmitCallback != NULL)
        {
            hDriver->transmitCallback (DRV_USART_INDEX_${DRV_INSTANCE});
        }
    }

}

void _DRV_USART${DRV_INSTANCE}_ByteReceiveTasks (void)
{
    DRV_USART_OBJ * hDriver = &gDrvUSART${DRV_INSTANCE}Obj;

    /* Call event handler only if interrupt mode is selected for RX */
    if ( 1 == ${HARDWARE_INSTANCE_REGISTER}->US_IMR.RXRDY )
    {
        if (hDriver->receiveCallback != NULL)
        {
            hDriver->receiveCallback (DRV_USART_INDEX_${DRV_INSTANCE});
        }
    }
}

void _DRV_USART${DRV_INSTANCE}_ByteErrorTasks (void)
{
    DRV_USART_OBJ * hDriver = &gDrvUSART${DRV_INSTANCE}Obj;

    /* Call event handler only if error interrupt mode is selected */
    if ( (1 == ${HARDWARE_INSTANCE_REGISTER}->US_IMR.OVRE) || (1 == ${HARDWARE_INSTANCE_REGISTER}->US_IMR.FRAME) ||(1 == ${HARDWARE_INSTANCE_REGISTER}->US_IMR.PARE) )
    {
         /* Update the USART client error flag */
         hDriver->error = (${HARDWARE_INSTANCE_REGISTER}->US_CSR.w & (US_CSR_OVRE_Msk | US_CSR_FRAME_Msk | US_CSR_PARE_Msk));
         
        /* Clear error condition */
        _DRV_USART${DRV_INSTANCE}_ErrorConditionClear();
        
        if (hDriver->errorCallback != NULL)
        {
            hDriver->errorCallback (DRV_USART_INDEX_${DRV_INSTANCE});
        }
    }
}
</#if>

</#macro>
<#if CONFIG_DRV_USART_INST_IDX0 == true>
<@DRV_USART_STATIC_FUNCTIONS DRV_INSTANCE="0"
HARDWARE_INSTANCE=CONFIG_DRV_USART_PERIPHERAL_ID_IDX0
OPER_MODE=CONFIG_DRV_USART_OPER_MODE_IDX0
LINE_CONTROL=CONFIG_DRV_USART_LINE_CNTRL_IDX0
BRG_CLK=CONFIG_DRV_USART_BRG_CLOCK_IDX0
BRG_RATE=CONFIG_DRV_USART_BAUD_RATE_IDX0
USART_INT_SRC=CONFIG_DRV_USART_INT_SRC_IDX0
/>
</#if>
<#if CONFIG_DRV_USART_INST_IDX1 == true>
<@DRV_USART_STATIC_FUNCTIONS DRV_INSTANCE="1"
HARDWARE_INSTANCE=CONFIG_DRV_USART_PERIPHERAL_ID_IDX1
OPER_MODE=CONFIG_DRV_USART_OPER_MODE_IDX1
LINE_CONTROL=CONFIG_DRV_USART_LINE_CNTRL_IDX1
BRG_CLK=CONFIG_DRV_USART_BRG_CLOCK_IDX1
BRG_RATE=CONFIG_DRV_USART_BAUD_RATE_IDX1
USART_INT_SRC=CONFIG_DRV_USART_INT_SRC_IDX1
/>
</#if>
<#if CONFIG_DRV_USART_INST_IDX2 == true>
<@DRV_USART_STATIC_FUNCTIONS DRV_INSTANCE="2"
HARDWARE_INSTANCE=CONFIG_DRV_USART_PERIPHERAL_ID_IDX2
OPER_MODE=CONFIG_DRV_USART_OPER_MODE_IDX2
LINE_CONTROL=CONFIG_DRV_USART_LINE_CNTRL_IDX2
BRG_CLK=CONFIG_DRV_USART_BRG_CLOCK_IDX2
BRG_RATE=CONFIG_DRV_USART_BAUD_RATE_IDX2
USART_INT_SRC=CONFIG_DRV_USART_INT_SRC_IDX2
/>
</#if>
/*******************************************************************************
 End of File
*/
