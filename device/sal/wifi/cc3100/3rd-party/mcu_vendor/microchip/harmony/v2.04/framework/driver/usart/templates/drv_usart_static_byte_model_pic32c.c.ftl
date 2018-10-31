/*******************************************************************************
  USART driver static implementation of Byte model.

  Company:
    Microchip Technology Inc.

  File Name:
    drv_usart_static_byte_model.c

  Summary:
    Source code for the USART driver static implementation of Byte model.

  Description:
    This file contains the source code for the static implementation of the
    USART driver Byte model.

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

uint8_t DRV_USART${DRV_INSTANCE}_ReadByte(void)
{
    uint8_t readValue;
	
<#if CONFIG_USE_3RDPARTY_RTOS>
    /* This function needs to be thread safe */
    if(OSAL_MUTEX_Lock(&(gDrvUSART${DRV_INSTANCE}Obj.mutexDriverInstance), OSAL_WAIT_FOREVER) == OSAL_RESULT_TRUE)
    {
        /* We were able to take the mutex */
    }
    else
    {
        SYS_DEBUG_MESSAGE(SYS_ERROR_DEBUG, "\r\nUSART Driver: Hardware Instance Mutex Time out in DRV_USART_ReadByte() function");
        return 0;
    }

</#if>
    /* Receive one byte */
    readValue = ${HARDWARE_INSTANCE_REGISTER}->US_RHR.RXCHR;
<#if CONFIG_USE_3RDPARTY_RTOS>

    OSAL_MUTEX_Unlock(&(gDrvUSART${DRV_INSTANCE}Obj.mutexDriverInstance));

</#if>

    return readValue;
}

void DRV_USART${DRV_INSTANCE}_WriteByte(const uint8_t byte)
{
<#if CONFIG_USE_3RDPARTY_RTOS>
    DRV_USART_OBJ *dObj = &gDrvUSART${DRV_INSTANCE}Obj;

    /* This function needs to be thread safe */
    if(OSAL_MUTEX_Lock(&(dObj->mutexDriverInstance), OSAL_WAIT_FOREVER) == OSAL_RESULT_TRUE)
    {
        /* We were able to take the mutex */
    }
    else
    {
        SYS_DEBUG_MESSAGE(SYS_ERROR_DEBUG, "\r\nUSART Driver: Hardware Instance Mutex Time out in DRV_USART_WriteByte() function");
    }

</#if>
<#if CONFIG_DRV_USART_BYTE_MODEL_BLOCKING == true>
    /* Wait till TX buffer is available as blocking operation is selected */
    while(0 == ${HARDWARE_INSTANCE_REGISTER}->US_CSR.TXEMPTY);
</#if>
    /* Send one byte */    
    ${HARDWARE_INSTANCE_REGISTER}->US_THR.w |= US_THR_TXCHR( byte );
<#if CONFIG_DRV_USART_INTERRUPT_MODE == true>
    /* Enable module TX interrupt source */
    ${HARDWARE_INSTANCE_REGISTER}->US_IER.w |= US_IER_TXEMPTY_Msk;
</#if>
<#if CONFIG_USE_3RDPARTY_RTOS>

    OSAL_MUTEX_Unlock(&(dObj->mutexDriverInstance));
</#if>
}

unsigned int DRV_USART${DRV_INSTANCE}_ReceiverBufferSizeGet(void)
{
    /* Device doesn't support FIFO, hence
     * by default RX buffer size is 1 packet */
    return 1;
}

unsigned int DRV_USART${DRV_INSTANCE}_TransmitBufferSizeGet(void)
{
    /* Device doesn't support FIFO, hence
     * by default TX buffer size is 1 packet */
    return 1;
}

bool DRV_USART${DRV_INSTANCE}_ReceiverBufferIsEmpty( void )
{
    bool rxIsEmpty = false;

    /* If there is data available in the receiver, then the
       buffer is not empty and this function returns false. */
    if(0 == ${HARDWARE_INSTANCE_REGISTER}->US_CSR.RXRDY)
    {
        rxIsEmpty = true;
    }
    
    return rxIsEmpty;
}

bool DRV_USART${DRV_INSTANCE}_TransmitBufferIsFull(void)
{
    bool txIsFull = false;

    /* If there is data still in TX buffer, then this 
     * function returns True. */
    if(0 == ${HARDWARE_INSTANCE_REGISTER}->US_CSR.TXEMPTY)
    {
        txIsFull = true;
    }
    
    return txIsFull;
}
<#if CONFIG_DRV_USART_BYTE_MODEL_CALLBACK ==true>

void DRV_USART${DRV_INSTANCE}_ByteTransmitCallbackSet(const DRV_USART_BYTE_EVENT_HANDLER eventHandler)
{
    gDrvUSART${DRV_INSTANCE}Obj.transmitCallback = eventHandler;
}

void DRV_USART${DRV_INSTANCE}_ByteReceiveCallbackSet(const DRV_USART_BYTE_EVENT_HANDLER eventHandler)
{
    gDrvUSART${DRV_INSTANCE}Obj.receiveCallback = eventHandler;
}

void DRV_USART${DRV_INSTANCE}_ByteErrorCallbackSet(const DRV_USART_BYTE_EVENT_HANDLER eventHandler)
{
    gDrvUSART${DRV_INSTANCE}Obj.errorCallback = eventHandler;
}
</#if>

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
