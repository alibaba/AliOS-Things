/*******************************************************************************
  USART Driver Dynamic to Static mapping

  Company:
    Microchip Technology Inc.

  File Name:
    drv_usart_mapping.c

  Summary:
    Source code for the USART driver dynamic APIs to static API mapping.

  Description:
    This file contains code that maps dynamic APIs to static whenever
    the static mode of the driver is selected..

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


SYS_MODULE_OBJ DRV_USART_Initialize(const SYS_MODULE_INDEX index,const SYS_MODULE_INIT * const init)
{
    SYS_MODULE_OBJ returnValue;

    switch(index)
    {
        case DRV_USART_INDEX_0:
        {
            returnValue = DRV_USART0_Initialize();
            break;
        }
<#if CONFIG_DRV_USART_INST_IDX1 == true>
        case DRV_USART_INDEX_1:
        {
            returnValue = DRV_USART1_Initialize();
            break;
        }
</#if>
<#if CONFIG_DRV_USART_INST_IDX2 == true>
        case DRV_USART_INDEX_2:
        {
            returnValue = DRV_USART2_Initialize();
            break;
        }
</#if>
<#if CONFIG_DRV_USART_INST_IDX3 == true>
        case DRV_USART_INDEX_3:
        {
            returnValue = DRV_USART3_Initialize();
            break;
        }
</#if>
<#if CONFIG_DRV_USART_INST_IDX4 == true>
        case DRV_USART_INDEX_4:
        {
            returnValue = DRV_USART4_Initialize();
            break;
        }
</#if>
<#if CONFIG_DRV_USART_INST_IDX5 == true>
        case DRV_USART_INDEX_5:
        {
            returnValue = DRV_USART5_Initialize();
            break;
        }
</#if>
        default:
        {
            returnValue = SYS_MODULE_OBJ_INVALID;
            break;
        }
    }
    return returnValue;
}

void DRV_USART_Deinitialize( SYS_MODULE_OBJ object)
{
    switch(object)
    {
        case DRV_USART_INDEX_0:
        {
            DRV_USART0_Deinitialize();
            break;
        }
<#if CONFIG_DRV_USART_INST_IDX1 == true>
        case DRV_USART_INDEX_1:
        {
            DRV_USART1_Deinitialize();
            break;
        }
</#if>
<#if CONFIG_DRV_USART_INST_IDX2 == true>
        case DRV_USART_INDEX_2:
        {
            DRV_USART2_Deinitialize();
            break;
        }
</#if>
<#if CONFIG_DRV_USART_INST_IDX3 == true>
        case DRV_USART_INDEX_3:
        {
            DRV_USART3_Deinitialize();
            break;
        }
</#if>
<#if CONFIG_DRV_USART_INST_IDX4 == true>
        case DRV_USART_INDEX_4:
        {
            DRV_USART4_Deinitialize();
            break;
        }
</#if>
<#if CONFIG_DRV_USART_INST_IDX5 == true>
        case DRV_USART_INDEX_5:
        {
            DRV_USART5_Deinitialize();
            break;
        }
</#if>
        default:
        {
            break;
        }
    }
}

SYS_STATUS DRV_USART_Status( SYS_MODULE_OBJ object)
{
    SYS_STATUS returnValue;

    switch(object)
    {
        case DRV_USART_INDEX_0:
        {
            returnValue = DRV_USART0_Status();
            break;
        }
<#if CONFIG_DRV_USART_INST_IDX1 == true>
        case DRV_USART_INDEX_1:
        {
            returnValue = DRV_USART1_Status();
            break;
        }
</#if>
<#if CONFIG_DRV_USART_INST_IDX2 == true>
        case DRV_USART_INDEX_2:
        {
            returnValue = DRV_USART2_Status();
            break;
        }
</#if>
<#if CONFIG_DRV_USART_INST_IDX3 == true>
        case DRV_USART_INDEX_3:
        {
            returnValue = DRV_USART3_Status();
            break;
        }
</#if>
<#if CONFIG_DRV_USART_INST_IDX4 == true>
        case DRV_USART_INDEX_4:
        {
            returnValue = DRV_USART4_Status();
            break;
        }
</#if>
<#if CONFIG_DRV_USART_INST_IDX5 == true>
        case DRV_USART_INDEX_5:
        {
            returnValue = DRV_USART5_Status();
            break;
        }
</#if>
        default:
        {
            returnValue = SYS_STATUS_ERROR;
            break;
        }
    }
    return returnValue;
}

void DRV_USART_TasksTransmit ( SYS_MODULE_OBJ object )
{
<#if !(CONFIG_DRV_USART_BYTE_MODEL_SUPPORT ==true && CONFIG_DRV_USART_BYTE_MODEL_CALLBACK ==false && CONFIG_DRV_USART_INTERRUPT_MODE == false) >
    switch(object)
    {
        case DRV_USART_INDEX_0:
        {
            DRV_USART0_TasksTransmit();
            break;
        }
<#if CONFIG_DRV_USART_INST_IDX1 == true>
        case DRV_USART_INDEX_1:
        {
            DRV_USART1_TasksTransmit();
            break;
        }
</#if>
<#if CONFIG_DRV_USART_INST_IDX2 == true>
        case DRV_USART_INDEX_2:
        {
            DRV_USART2_TasksTransmit();
            break;
        }
</#if>
<#if CONFIG_DRV_USART_INST_IDX3 == true>
        case DRV_USART_INDEX_3:
        {
            DRV_USART3_TasksTransmit();
            break;
        }
</#if>
<#if CONFIG_DRV_USART_INST_IDX4 == true>
        case DRV_USART_INDEX_4:
        {
            DRV_USART4_TasksTransmit();
            break;
        }
</#if>
<#if CONFIG_DRV_USART_INST_IDX5 == true>
        case DRV_USART_INDEX_5:
        {
            DRV_USART5_TasksTransmit();
            break;
        }
</#if>
        default:
        {
            break;
        }
    }
<#else>
    /* Usart is configured for static byte model without callbacks.
       Hence, no task need to be executed */
    return;	
</#if>
}

void DRV_USART_TasksReceive ( SYS_MODULE_OBJ object )
{
<#if !(CONFIG_DRV_USART_BYTE_MODEL_SUPPORT ==true && CONFIG_DRV_USART_BYTE_MODEL_CALLBACK ==false && CONFIG_DRV_USART_INTERRUPT_MODE == false) >
    switch(object)
    {
        case DRV_USART_INDEX_0:
        {
            DRV_USART0_TasksReceive();
            break;
        }
<#if CONFIG_DRV_USART_INST_IDX1 == true>
        case DRV_USART_INDEX_1:
        {
            DRV_USART1_TasksReceive();
            break;
        }
</#if>
<#if CONFIG_DRV_USART_INST_IDX2 == true>
        case DRV_USART_INDEX_2:
        {
            DRV_USART2_TasksReceive();
            break;
        }
</#if>
<#if CONFIG_DRV_USART_INST_IDX3 == true>
        case DRV_USART_INDEX_3:
        {
            DRV_USART3_TasksReceive();
            break;
        }
</#if>
<#if CONFIG_DRV_USART_INST_IDX4 == true>
        case DRV_USART_INDEX_4:
        {
            DRV_USART4_TasksReceive();
            break;
        }
</#if>
<#if CONFIG_DRV_USART_INST_IDX5 == true>
        case DRV_USART_INDEX_5:
        {
            DRV_USART5_TasksReceive();
            break;
        }
</#if>
        default:
        {
            break;
        }
    }
<#else>
    /* Usart is configured for static byte model without callbacks.
       Hence, no task need to be executed */
    return;	
</#if>
}

void DRV_USART_TasksError ( SYS_MODULE_OBJ object )
{
<#if !(CONFIG_DRV_USART_BYTE_MODEL_SUPPORT ==true && CONFIG_DRV_USART_BYTE_MODEL_CALLBACK ==false && CONFIG_DRV_USART_INTERRUPT_MODE == false) >
    switch(object)
    {
        case DRV_USART_INDEX_0:
        {
            DRV_USART0_TasksError();
            break;
        }
<#if CONFIG_DRV_USART_INST_IDX1 == true>
        case DRV_USART_INDEX_1:
        {
            DRV_USART1_TasksError();
            break;
        }
</#if>
<#if CONFIG_DRV_USART_INST_IDX2 == true>
        case DRV_USART_INDEX_2:
        {
            DRV_USART2_TasksError();
            break;
        }
</#if>
<#if CONFIG_DRV_USART_INST_IDX3 == true>
        case DRV_USART_INDEX_3:
        {
            DRV_USART3_TasksError();
            break;
        }
</#if>
<#if CONFIG_DRV_USART_INST_IDX4 == true>
        case DRV_USART_INDEX_4:
        {
            DRV_USART4_TasksError();
            break;
        }
</#if>
<#if CONFIG_DRV_USART_INST_IDX5 == true>
        case DRV_USART_INDEX_5:
        {
            DRV_USART5_TasksError();
            break;
        }
</#if>
        default:
        {
            break;
        }
    }
<#else>
    /* Usart is configured for static byte model without callbacks.
       Hence, no task need to be executed */
    return;	
</#if>
}


//client interface
DRV_HANDLE DRV_USART_Open( const SYS_MODULE_INDEX index, const DRV_IO_INTENT ioIntent)
{
    DRV_HANDLE returnValue;

    switch(index)
    {
        case DRV_USART_INDEX_0:
        {
            returnValue = DRV_USART0_Open(index,ioIntent);
            break;
        }
<#if CONFIG_DRV_USART_INST_IDX1 == true>
        case DRV_USART_INDEX_1:
        {
            returnValue = DRV_USART1_Open(index,ioIntent);
            break;
        }
</#if>
<#if CONFIG_DRV_USART_INST_IDX2 == true>
        case DRV_USART_INDEX_2:
        {
            returnValue = DRV_USART2_Open(index,ioIntent);
            break;
        }
</#if>
<#if CONFIG_DRV_USART_INST_IDX3 == true>
        case DRV_USART_INDEX_3:
        {
            returnValue = DRV_USART3_Open(index,ioIntent);
            break;
        }
</#if>
<#if CONFIG_DRV_USART_INST_IDX4 == true>
        case DRV_USART_INDEX_4:
        {
            returnValue = DRV_USART4_Open(index,ioIntent);
            break;
        }
</#if>
<#if CONFIG_DRV_USART_INST_IDX5 == true>
        case DRV_USART_INDEX_5:
        {
            returnValue = DRV_USART5_Open(index,ioIntent);
            break;
        }
</#if>
        default:
        {
            returnValue = DRV_HANDLE_INVALID;
            break;
        }
    }
    return returnValue;
}

void DRV_USART_Close( const DRV_HANDLE handle)
{
    uintptr_t instance;

    instance = handle & 0x00FF;
    //As we are handling single client, only multiple instance is taken care.
    switch(instance)
    {
        case DRV_USART_INDEX_0:
        {
            DRV_USART0_Close();
            break;
        }
<#if CONFIG_DRV_USART_INST_IDX1 == true>
        case DRV_USART_INDEX_1:
        {
            DRV_USART1_Close();
            break;
        }
</#if>
<#if CONFIG_DRV_USART_INST_IDX2 == true>
        case DRV_USART_INDEX_2:
        {
            DRV_USART2_Close();
            break;
        }
</#if>
<#if CONFIG_DRV_USART_INST_IDX3 == true>
        case DRV_USART_INDEX_3:
        {
            DRV_USART3_Close();
            break;
        }
</#if>
<#if CONFIG_DRV_USART_INST_IDX4 == true>
        case DRV_USART_INDEX_4:
        {
            DRV_USART4_Close();
            break;
        }
</#if>
<#if CONFIG_DRV_USART_INST_IDX5 == true>
        case DRV_USART_INDEX_5:
        {
            DRV_USART5_Close();
            break;
        }
</#if>
        default:
        {
            break;
        }
    }
}

DRV_USART_CLIENT_STATUS DRV_USART_ClientStatus ( DRV_HANDLE handle )
{
    uintptr_t instance;
    DRV_USART_CLIENT_STATUS returnValue;

    instance = handle & 0x00FF;
    //As we are handling single client, only multiple instance is taken care.
    switch(instance)
    {
        case DRV_USART_INDEX_0:
        {
            returnValue = DRV_USART0_ClientStatus();
            break;
        }
<#if CONFIG_DRV_USART_INST_IDX1 == true>
        case DRV_USART_INDEX_1:
        {
            returnValue = DRV_USART1_ClientStatus();
            break;
        }
</#if>
<#if CONFIG_DRV_USART_INST_IDX2 == true>
        case DRV_USART_INDEX_2:
        {
            returnValue = DRV_USART2_ClientStatus();
            break;
        }
</#if>
<#if CONFIG_DRV_USART_INST_IDX3 == true>
        case DRV_USART_INDEX_3:
        {
            returnValue = DRV_USART3_ClientStatus();
            break;
        }
</#if>
<#if CONFIG_DRV_USART_INST_IDX4 == true>
        case DRV_USART_INDEX_4:
        {
            returnValue = DRV_USART4_ClientStatus();
            break;
        }
</#if>
<#if CONFIG_DRV_USART_INST_IDX5 == true>
        case DRV_USART_INDEX_5:
        {
            returnValue = DRV_USART5_ClientStatus();
            break;
        }
</#if>
        default:
        {
            returnValue = DRV_CLIENT_STATUS_ERROR;
            break;
        }
    }
    return returnValue;
}

DRV_USART_TRANSFER_STATUS DRV_USART_TransferStatus( const DRV_HANDLE handle )
{
    uintptr_t instance;
    DRV_USART_TRANSFER_STATUS returnValue;

    instance = handle & 0x00FF;
    //As we are handling single client, only multiple instance is taken care.
    switch(instance)
    {
        case DRV_USART_INDEX_0:
        {
            returnValue = DRV_USART0_TransferStatus();
            break;
        }
<#if CONFIG_DRV_USART_INST_IDX1 == true>
        case DRV_USART_INDEX_1:
        {
            returnValue = DRV_USART1_TransferStatus();
            break;
        }
</#if>
<#if CONFIG_DRV_USART_INST_IDX2 == true>
        case DRV_USART_INDEX_2:
        {
            returnValue = DRV_USART2_TransferStatus();
            break;
        }
</#if>
<#if CONFIG_DRV_USART_INST_IDX3 == true>
        case DRV_USART_INDEX_3:
        {
            returnValue = DRV_USART3_TransferStatus();
            break;
        }
</#if>
<#if CONFIG_DRV_USART_INST_IDX4 == true>
        case DRV_USART_INDEX_4:
        {
            returnValue = DRV_USART4_TransferStatus();
            break;
        }
</#if>
<#if CONFIG_DRV_USART_INST_IDX5 == true>
        case DRV_USART_INDEX_5:
        {
            returnValue = DRV_USART5_TransferStatus();
            break;
        }
</#if>
        default:
        {
            returnValue = (DRV_USART_TRANSFER_STATUS)NULL;
            break;
        }
    }
    return returnValue;
}

DRV_USART_ERROR DRV_USART_ErrorGet(const DRV_HANDLE handle)
{
    uintptr_t instance;
    DRV_USART_ERROR returnValue;

    instance = handle & 0x00FF;
    //As we are handling single client, only multiple instance is taken care.
    switch(instance)
    {
        case DRV_USART_INDEX_0:
        {
            returnValue = DRV_USART0_ErrorGet();
            break;
        }
<#if CONFIG_DRV_USART_INST_IDX1 == true>
        case DRV_USART_INDEX_1:
        {
            returnValue = DRV_USART1_ErrorGet();
            break;
        }
</#if>
<#if CONFIG_DRV_USART_INST_IDX2 == true>
        case DRV_USART_INDEX_2:
        {
            returnValue = DRV_USART2_ErrorGet();
            break;
        }
</#if>
<#if CONFIG_DRV_USART_INST_IDX3 == true>
        case DRV_USART_INDEX_3:
        {
            returnValue = DRV_USART3_ErrorGet();
            break;
        }
</#if>
<#if CONFIG_DRV_USART_INST_IDX4 == true>
        case DRV_USART_INDEX_4:
        {
            returnValue = DRV_USART4_ErrorGet();
            break;
        }
</#if>
<#if CONFIG_DRV_USART_INST_IDX5 == true>
        case DRV_USART_INDEX_5:
        {
            returnValue = DRV_USART5_ErrorGet();
            break;
        }
</#if>
        default:
        {
            returnValue = (DRV_USART_ERROR)NULL;
            break;
        }
    }
    return returnValue;
}

<#if CONFIG_DRV_USART_BUFFER_QUEUE_SUPPORT ==true>
//Buffer Queuing Model Interface
void DRV_USART_BufferAddWrite(const DRV_HANDLE handle,DRV_USART_BUFFER_HANDLE * bufferHandle, void * buffer, const size_t size)
{
    uintptr_t instance;

    instance = handle & 0x00FF;
    //As we are handling single client, only multiple instance is taken care.
    switch(instance)
    {
        case DRV_USART_INDEX_0:
        {
            DRV_USART0_BufferAddWrite (bufferHandle, buffer, size);
            break;
        }
<#if CONFIG_DRV_USART_INST_IDX1 == true>
        case DRV_USART_INDEX_1:
        {
            DRV_USART1_BufferAddWrite (bufferHandle, buffer, size);
            break;
        }
</#if>
<#if CONFIG_DRV_USART_INST_IDX2 == true>
        case DRV_USART_INDEX_2:
        {
            DRV_USART2_BufferAddWrite (bufferHandle, buffer, size);
            break;
        }
</#if>
<#if CONFIG_DRV_USART_INST_IDX3 == true>
        case DRV_USART_INDEX_3:
        {
            DRV_USART3_BufferAddWrite (bufferHandle, buffer, size);
            break;
        }
</#if>
<#if CONFIG_DRV_USART_INST_IDX4 == true>
        case DRV_USART_INDEX_4:
        {
            DRV_USART4_BufferAddWrite (bufferHandle, buffer, size);
            break;
        }
</#if>
<#if CONFIG_DRV_USART_INST_IDX5 == true>
        case DRV_USART_INDEX_5:
        {
            DRV_USART5_BufferAddWrite (bufferHandle, buffer, size);
            break;
        }
</#if>
        default:
        {
            break;
        }
    }
}

void DRV_USART_BufferAddRead(const DRV_HANDLE handle,DRV_USART_BUFFER_HANDLE * const bufferHandle,void * buffer,const size_t size)
{
    uintptr_t instance;

    instance = handle & 0x00FF;
    //As we are handling single client, only multiple instance is taken care.
    switch(instance)
    {
        case DRV_USART_INDEX_0:
        {
            DRV_USART0_BufferAddRead (bufferHandle, buffer, size);
            break;
        }
<#if CONFIG_DRV_USART_INST_IDX1 == true>
        case DRV_USART_INDEX_1:
        {
            DRV_USART1_BufferAddRead (bufferHandle, buffer, size);
            break;
        }
</#if>
<#if CONFIG_DRV_USART_INST_IDX2 == true>
        case DRV_USART_INDEX_2:
        {
            DRV_USART2_BufferAddRead (bufferHandle, buffer, size);
            break;
        }
</#if>
<#if CONFIG_DRV_USART_INST_IDX3 == true>
        case DRV_USART_INDEX_3:
        {
            DRV_USART3_BufferAddRead (bufferHandle, buffer, size);
            break;
        }
</#if>
<#if CONFIG_DRV_USART_INST_IDX4 == true>
        case DRV_USART_INDEX_4:
        {
            DRV_USART4_BufferAddRead (bufferHandle, buffer, size);
            break;
        }
</#if>
<#if CONFIG_DRV_USART_INST_IDX5 == true>
        case DRV_USART_INDEX_5:
        {
            DRV_USART5_BufferAddRead (bufferHandle, buffer, size);
            break;
        }
</#if>
        default:
        {
            break;
        }
    }
}

void DRV_USART_BufferEventHandlerSet (const DRV_HANDLE handle,const DRV_USART_BUFFER_EVENT_HANDLER eventHandler,const uintptr_t context)
{
    uintptr_t instance;

    instance = handle & 0x00FF;
    //As we are handling single client, only multiple instance is taken care.
    switch(instance)
    {
        case DRV_USART_INDEX_0:
        {
            DRV_USART0_BufferEventHandlerSet (eventHandler, context);
            break;
        }
<#if CONFIG_DRV_USART_INST_IDX1 == true>
        case DRV_USART_INDEX_1:
        {
            DRV_USART1_BufferEventHandlerSet (eventHandler, context);
            break;
        }
</#if>
<#if CONFIG_DRV_USART_INST_IDX2 == true>
        case DRV_USART_INDEX_2:
        {
            DRV_USART2_BufferEventHandlerSet (eventHandler, context);
            break;
        }
</#if>
<#if CONFIG_DRV_USART_INST_IDX3 == true>
        case DRV_USART_INDEX_3:
        {
            DRV_USART3_BufferEventHandlerSet (eventHandler, context);
            break;
        }
</#if>
<#if CONFIG_DRV_USART_INST_IDX4 == true>
        case DRV_USART_INDEX_4:
        {
            DRV_USART4_BufferEventHandlerSet (eventHandler, context);
            break;
        }
</#if>
<#if CONFIG_DRV_USART_INST_IDX5 == true>
        case DRV_USART_INDEX_5:
        {
            DRV_USART5_BufferEventHandlerSet (eventHandler, context);
            break;
        }
</#if>
        default:
        {
            break;
        }
    }
}

extern DRV_USART_BUFFER_OBJ gDrvUSARTBufferObj[DRV_USART_QUEUE_DEPTH_COMBINED];

DRV_USART_BUFFER_RESULT DRV_USART_BufferRemove( DRV_USART_BUFFER_HANDLE bufferHandle )
{
    uintptr_t instance;
    DRV_USART_BUFFER_OBJ * bufferObj;
    
    /* Get the driver instance to which this buffer handle belongs to */
    bufferObj = &gDrvUSARTBufferObj[bufferHandle & 0xFFFF];
    instance = bufferObj->drvInstance;

    switch(instance)
    {
        case DRV_USART_INDEX_0:
        {
            return DRV_USART0_BufferRemove (bufferHandle);
            break;
        }
<#if CONFIG_DRV_USART_INST_IDX1 == true>
        case DRV_USART_INDEX_1:
        {
            return DRV_USART1_BufferRemove (bufferHandle);
            break;
        }
</#if>
<#if CONFIG_DRV_USART_INST_IDX2 == true>
        case DRV_USART_INDEX_2:
        {
            return DRV_USART2_BufferRemove (bufferHandle);
            break;
        }
</#if>
<#if CONFIG_DRV_USART_INST_IDX3 == true>
        case DRV_USART_INDEX_3:
        {
            return DRV_USART3_BufferRemove (bufferHandle);
            break;
        }
</#if>
<#if CONFIG_DRV_USART_INST_IDX4 == true>
        case DRV_USART_INDEX_4:
        {
            return DRV_USART4_BufferRemove (bufferHandle);
            break;
        }
</#if>
<#if CONFIG_DRV_USART_INST_IDX5 == true>
        case DRV_USART_INDEX_5:
        {
            return DRV_USART5_BufferRemove (bufferHandle);
            break;
        }
</#if>
        default:
        {
            return DRV_USART_BUFFER_RESULT_HANDLE_INVALID;
        }
    }
}

size_t DRV_USART_BufferCompletedBytesGet( DRV_USART_BUFFER_HANDLE bufferHandle )
{
    /* This function is independent of instance or client */
    return DRV_USART0_BufferCompletedBytesGet (bufferHandle);
}

size_t DRV_USART_BufferProcessedSizeGet( DRV_USART_BUFFER_HANDLE bufferHandle )
{
    /* This function will be deprecated */
    return DRV_USART0_BufferProcessedSizeGet(bufferHandle);
}
</#if>

<#if CONFIG_DRV_USART_READ_WRITE_MODEL_SUPPORT ==true>
//File System Model
size_t DRV_USART_Read(const DRV_HANDLE handle,void * buffer, const size_t numbytes)
{
    uintptr_t instance;
    size_t returnValue;

    instance = handle & 0x00FF;
    //As we are handling single client, only multiple instance is taken care.
    switch(instance)
    {
        case DRV_USART_INDEX_0:
        {
            returnValue = DRV_USART0_Read(buffer, numbytes);
            break;
        }
<#if CONFIG_DRV_USART_INST_IDX1 == true>
        case DRV_USART_INDEX_1:
        {
            returnValue = DRV_USART1_Read(buffer, numbytes);
            break;
        }
</#if>
<#if CONFIG_DRV_USART_INST_IDX2 == true>
        case DRV_USART_INDEX_2:
        {
            returnValue = DRV_USART2_Read(buffer, numbytes);
            break;
        }
</#if>
<#if CONFIG_DRV_USART_INST_IDX3 == true>
        case DRV_USART_INDEX_3:
        {
            returnValue = DRV_USART3_Read(buffer, numbytes);
            break;
        }
</#if>
<#if CONFIG_DRV_USART_INST_IDX4 == true>
        case DRV_USART_INDEX_4:
        {
            returnValue = DRV_USART4_Read(buffer, numbytes);
            break;
        }
</#if>
<#if CONFIG_DRV_USART_INST_IDX5 == true>
        case DRV_USART_INDEX_5:
        {
            returnValue = DRV_USART5_Read(buffer, numbytes);
            break;
        }
</#if>
        default:
        {
            returnValue = DRV_USART_READ_ERROR;
            break;
        }
    }
    return returnValue;
}

size_t DRV_USART_Write(const DRV_HANDLE handle,void * buffer,const size_t numbytes)
{
    uintptr_t instance;
    size_t returnValue;

    instance = handle & 0x00FF;
    //As we are handling single client, only multiple instance is taken care.
    switch(instance)
    {
        case DRV_USART_INDEX_0:
        {
            returnValue = DRV_USART0_Write(buffer, numbytes);
            break;
        }
<#if CONFIG_DRV_USART_INST_IDX1 == true>
        case DRV_USART_INDEX_1:
        {
            returnValue = DRV_USART1_Write(buffer, numbytes);
            break;
        }
</#if>
<#if CONFIG_DRV_USART_INST_IDX2 == true>
        case DRV_USART_INDEX_2:
        {
            returnValue = DRV_USART2_Write(buffer, numbytes);
            break;
        }
</#if>
<#if CONFIG_DRV_USART_INST_IDX3 == true>
        case DRV_USART_INDEX_3:
        {
            returnValue = DRV_USART3_Write(buffer, numbytes);
            break;
        }
</#if>
<#if CONFIG_DRV_USART_INST_IDX4 == true>
        case DRV_USART_INDEX_4:
        {
            returnValue = DRV_USART4_Write(buffer, numbytes);
            break;
        }
</#if>
<#if CONFIG_DRV_USART_INST_IDX5 == true>
        case DRV_USART_INDEX_5:
        {
            returnValue = DRV_USART5_Write(buffer, numbytes);
            break;
        }
</#if>
        default:
        {
            returnValue = DRV_USART_WRITE_ERROR;
            break;
        }
    }
    return returnValue;
}

</#if>

<#if CONFIG_DRV_USART_BYTE_MODEL_SUPPORT ==true>
//Byte Model
uint8_t DRV_USART_ReadByte( const DRV_HANDLE handle )
{
    uintptr_t instance;
    uint8_t returnValue;

    instance = handle & 0x00FF;
    //As we are handling single client, only multiple instance is taken care.
    switch(instance)
    {
        case DRV_USART_INDEX_0:
        {
            returnValue = DRV_USART0_ReadByte();
            break;
        }
<#if CONFIG_DRV_USART_INST_IDX1 == true>
        case DRV_USART_INDEX_1:
        {
            returnValue = DRV_USART1_ReadByte();
            break;
        }
</#if>
<#if CONFIG_DRV_USART_INST_IDX2 == true>
        case DRV_USART_INDEX_2:
        {
            returnValue = DRV_USART2_ReadByte();
            break;
        }
</#if>
<#if CONFIG_DRV_USART_INST_IDX3 == true>
        case DRV_USART_INDEX_3:
        {
            returnValue = DRV_USART3_ReadByte();
            break;
        }
</#if>
<#if CONFIG_DRV_USART_INST_IDX4 == true>
        case DRV_USART_INDEX_4:
        {
            returnValue = DRV_USART4_ReadByte();
            break;
        }
</#if>
<#if CONFIG_DRV_USART_INST_IDX5 == true>
        case DRV_USART_INDEX_5:
        {
            returnValue = DRV_USART5_ReadByte();
            break;
        }
</#if>
        default:
        {
            SYS_ASSERT(false, "Incorrect Driver Handle");
            returnValue = 0;
            break;
        }
    }
    return returnValue;
}

void DRV_USART_WriteByte( const DRV_HANDLE handle, const uint8_t byte)
{
    uintptr_t instance;

    instance = handle & 0x00FF;
    //As we are handling single client, only multiple instance is taken care.
    switch(instance)
    {
        case DRV_USART_INDEX_0:
        {
            DRV_USART0_WriteByte(byte);
            break;
        }
<#if CONFIG_DRV_USART_INST_IDX1 == true>
        case DRV_USART_INDEX_1:
        {
            DRV_USART1_WriteByte(byte);
            break;
        }
</#if>
<#if CONFIG_DRV_USART_INST_IDX2 == true>
        case DRV_USART_INDEX_2:
        {
            DRV_USART2_WriteByte(byte);
            break;
        }
</#if>
<#if CONFIG_DRV_USART_INST_IDX3 == true>
        case DRV_USART_INDEX_3:
        {
            DRV_USART3_WriteByte(byte);
            break;
        }
</#if>
<#if CONFIG_DRV_USART_INST_IDX4 == true>
        case DRV_USART_INDEX_4:
        {
            DRV_USART4_WriteByte(byte);
            break;
        }
</#if>
<#if CONFIG_DRV_USART_INST_IDX5 == true>
        case DRV_USART_INDEX_5:
        {
            DRV_USART5_WriteByte(byte);
            break;
        }
</#if>
        default:
        {
            break;
        }
    }
}

unsigned int DRV_USART_ReceiverBufferSizeGet( const DRV_HANDLE handle )
{
    uintptr_t instance;
    unsigned int returnValue;

    instance = handle & 0x00FF;
    //As we are handling single client, only multiple instance is taken care.
    switch(instance)
    {
        case DRV_USART_INDEX_0:
        {
            returnValue = DRV_USART0_ReceiverBufferSizeGet();
            break;
        }
<#if CONFIG_DRV_USART_INST_IDX1 == true>
        case DRV_USART_INDEX_1:
        {
            returnValue = DRV_USART1_ReceiverBufferSizeGet();
            break;
        }
</#if>
<#if CONFIG_DRV_USART_INST_IDX2 == true>
        case DRV_USART_INDEX_2:
        {
            returnValue = DRV_USART2_ReceiverBufferSizeGet();
            break;
        }
</#if>
<#if CONFIG_DRV_USART_INST_IDX3 == true>
        case DRV_USART_INDEX_3:
        {
            returnValue = DRV_USART3_ReceiverBufferSizeGet();
            break;
        }
</#if>
<#if CONFIG_DRV_USART_INST_IDX4 == true>
        case DRV_USART_INDEX_4:
        {
            returnValue = DRV_USART4_ReceiverBufferSizeGet();
            break;
        }
</#if>
<#if CONFIG_DRV_USART_INST_IDX5 == true>
        case DRV_USART_INDEX_5:
        {
            returnValue = DRV_USART5_ReceiverBufferSizeGet();
            break;
        }
</#if>
        default:
        {
            returnValue = (unsigned int)NULL;
            break;
        }
    }
    return returnValue;
}

unsigned int DRV_USART_TransmitBufferSizeGet( const DRV_HANDLE handle )
{
    uintptr_t instance;
    unsigned int returnValue;

    instance = handle & 0x00FF;
    //As we are handling single client, only multiple instance is taken care.
    switch(instance)
    {
        case DRV_USART_INDEX_0:
        {
            returnValue = DRV_USART0_TransmitBufferSizeGet();
            break;
        }
<#if CONFIG_DRV_USART_INST_IDX1 == true>
        case DRV_USART_INDEX_1:
        {
            returnValue = DRV_USART1_TransmitBufferSizeGet();
            break;
        }
</#if>
<#if CONFIG_DRV_USART_INST_IDX2 == true>
        case DRV_USART_INDEX_2:
        {
            returnValue = DRV_USART2_TransmitBufferSizeGet();
            break;
        }
</#if>
<#if CONFIG_DRV_USART_INST_IDX3 == true>
        case DRV_USART_INDEX_3:
        {
            returnValue = DRV_USART3_TransmitBufferSizeGet();
            break;
        }
</#if>
<#if CONFIG_DRV_USART_INST_IDX4 == true>
        case DRV_USART_INDEX_4:
        {
            returnValue = DRV_USART4_TransmitBufferSizeGet();
            break;
        }
</#if>
<#if CONFIG_DRV_USART_INST_IDX5 == true>
        case DRV_USART_INDEX_5:
        {
            returnValue = DRV_USART5_TransmitBufferSizeGet();
            break;
        }
</#if>
        default:
        {
            returnValue = (unsigned int)NULL;
            break;
        }
    }
    return returnValue;
}

bool DRV_USART_ReceiverBufferIsEmpty( const DRV_HANDLE handle )
{
    uintptr_t instance;
    bool returnValue;

    instance = handle & 0x00FF;
    //As we are handling single client, only multiple instance is taken care.
    switch(instance)
    {
        case DRV_USART_INDEX_0:
        {
            returnValue = DRV_USART0_ReceiverBufferIsEmpty();
            break;
        }
<#if CONFIG_DRV_USART_INST_IDX1 == true>
        case DRV_USART_INDEX_1:
        {
            returnValue = DRV_USART1_ReceiverBufferIsEmpty();
            break;
        }
</#if>
<#if CONFIG_DRV_USART_INST_IDX2 == true>
        case DRV_USART_INDEX_2:
        {
            returnValue = DRV_USART2_ReceiverBufferIsEmpty();
            break;
        }
</#if>
<#if CONFIG_DRV_USART_INST_IDX3 == true>
        case DRV_USART_INDEX_3:
        {
            returnValue = DRV_USART3_ReceiverBufferIsEmpty();
            break;
        }
</#if>
<#if CONFIG_DRV_USART_INST_IDX4 == true>
        case DRV_USART_INDEX_4:
        {
            returnValue = DRV_USART4_ReceiverBufferIsEmpty();
            break;
        }
</#if>
<#if CONFIG_DRV_USART_INST_IDX5 == true>
        case DRV_USART_INDEX_5:
        {
            returnValue = DRV_USART5_ReceiverBufferIsEmpty();
            break;
        }
</#if>
        default:
        {
            returnValue = false;
            break;
        }
    }
    return returnValue;
}

bool DRV_USART_TransmitBufferIsFull( const DRV_HANDLE handle )
{
    uintptr_t instance;
    bool returnValue;

    instance = handle & 0x00FF;
    //As we are handling single client, only multiple instance is taken care.
    switch(instance)
    {
        case DRV_USART_INDEX_0:
        {
            returnValue = DRV_USART0_TransmitBufferIsFull();
            break;
        }
<#if CONFIG_DRV_USART_INST_IDX1 == true>
        case DRV_USART_INDEX_1:
        {
            returnValue = DRV_USART1_TransmitBufferIsFull();
            break;
        }
</#if>
<#if CONFIG_DRV_USART_INST_IDX2 == true>
        case DRV_USART_INDEX_2:
        {
            returnValue = DRV_USART2_TransmitBufferIsFull();
            break;
        }
</#if>
<#if CONFIG_DRV_USART_INST_IDX3 == true>
        case DRV_USART_INDEX_3:
        {
            returnValue = DRV_USART3_TransmitBufferIsFull();
            break;
        }
</#if>
<#if CONFIG_DRV_USART_INST_IDX4 == true>
        case DRV_USART_INDEX_4:
        {
            returnValue = DRV_USART4_TransmitBufferIsFull();
            break;
        }
</#if>
<#if CONFIG_DRV_USART_INST_IDX5 == true>
        case DRV_USART_INDEX_5:
        {
            returnValue = DRV_USART5_TransmitBufferIsFull();
            break;
        }
</#if>
        default:
        {
            returnValue = false;
            break;
        }
    }
    return returnValue;
}
<#if CONFIG_DRV_USART_BYTE_MODEL_CALLBACK ==true>

void DRV_USART_ByteTransmitCallbackSet ( const SYS_MODULE_INDEX index, const DRV_USART_BYTE_EVENT_HANDLER eventHandler )
{
    switch(index)
    {
        case DRV_USART_INDEX_0:
        {
            DRV_USART0_ByteTransmitCallbackSet(eventHandler);
            break;
        }
<#if CONFIG_DRV_USART_INST_IDX1 == true>
        case DRV_USART_INDEX_1:
        {
            DRV_USART1_ByteTransmitCallbackSet(eventHandler);
            break;
        }
</#if>
<#if CONFIG_DRV_USART_INST_IDX2 == true>
        case DRV_USART_INDEX_2:
        {
            DRV_USART2_ByteTransmitCallbackSet(eventHandler);
            break;
        }
</#if>
<#if CONFIG_DRV_USART_INST_IDX3 == true>
        case DRV_USART_INDEX_3:
        {
            DRV_USART3_ByteTransmitCallbackSet(eventHandler);
            break;
        }
</#if>
<#if CONFIG_DRV_USART_INST_IDX4 == true>
        case DRV_USART_INDEX_4:
        {
            DRV_USART4_ByteTransmitCallbackSet(eventHandler);
            break;
        }
</#if>
<#if CONFIG_DRV_USART_INST_IDX5 == true>
        case DRV_USART_INDEX_5:
        {
            DRV_USART5_ByteTransmitCallbackSet(eventHandler);
            break;
        }
</#if>
        default:
        {
            break;
        }
    }
}

void DRV_USART_ByteReceiveCallbackSet ( const SYS_MODULE_INDEX index, const DRV_USART_BYTE_EVENT_HANDLER eventHandler )
{
    switch(index)
    {
        case DRV_USART_INDEX_0:
        {
            DRV_USART0_ByteReceiveCallbackSet(eventHandler);
            break;
        }
<#if CONFIG_DRV_USART_INST_IDX1 == true>
        case DRV_USART_INDEX_1:
        {
            DRV_USART1_ByteReceiveCallbackSet(eventHandler);
            break;
        }
</#if>
<#if CONFIG_DRV_USART_INST_IDX2 == true>
        case DRV_USART_INDEX_2:
        {
            DRV_USART2_ByteReceiveCallbackSet(eventHandler);
            break;
        }
</#if>
<#if CONFIG_DRV_USART_INST_IDX3 == true>
        case DRV_USART_INDEX_3:
        {
            DRV_USART3_ByteReceiveCallbackSet(eventHandler);
            break;
        }
</#if>
<#if CONFIG_DRV_USART_INST_IDX4 == true>
        case DRV_USART_INDEX_4:
        {
            DRV_USART4_ByteReceiveCallbackSet(eventHandler);
            break;
        }
</#if>
<#if CONFIG_DRV_USART_INST_IDX5 == true>
        case DRV_USART_INDEX_5:
        {
            DRV_USART5_ByteReceiveCallbackSet(eventHandler);
            break;
        }
</#if>
        default:
        {
            break;
        }
    }
}

void DRV_USART_ByteErrorCallbackSet ( const SYS_MODULE_INDEX index, const DRV_USART_BYTE_EVENT_HANDLER eventHandler )
{
    switch(index)
    {
        case DRV_USART_INDEX_0:
        {
            DRV_USART0_ByteErrorCallbackSet(eventHandler);
            break;
        }
<#if CONFIG_DRV_USART_INST_IDX1 == true>
        case DRV_USART_INDEX_1:
        {
            DRV_USART1_ByteErrorCallbackSet(eventHandler);
            break;
        }
</#if>
<#if CONFIG_DRV_USART_INST_IDX2 == true>
        case DRV_USART_INDEX_2:
        {
            DRV_USART2_ByteErrorCallbackSet(eventHandler);
            break;
        }
</#if>
<#if CONFIG_DRV_USART_INST_IDX3 == true>
        case DRV_USART_INDEX_3:
        {
            DRV_USART3_ByteErrorCallbackSet(eventHandler);
            break;
        }
</#if>
<#if CONFIG_DRV_USART_INST_IDX4 == true>
        case DRV_USART_INDEX_4:
        {
            DRV_USART4_ByteErrorCallbackSet(eventHandler);
            break;
        }
</#if>
<#if CONFIG_DRV_USART_INST_IDX5 == true>
        case DRV_USART_INDEX_5:
        {
            DRV_USART5_ByteErrorCallbackSet(eventHandler);
            break;
        }
</#if>
        default:
        {
            break;
        }
    }
}
</#if>
</#if>

DRV_USART_BAUD_SET_RESULT DRV_USART_BaudSet(const DRV_HANDLE handle, uint32_t baud)
{
    uintptr_t instance;
    DRV_USART_BAUD_SET_RESULT returnValue;

    instance = handle & 0x00FF;
    //As we are handling single client, only multiple instance is taken care.
    switch(instance)
    {
        case DRV_USART_INDEX_0:
        {
            returnValue = DRV_USART0_BaudSet(baud);
            break;
        }
<#if CONFIG_DRV_USART_INST_IDX1 == true>
        case DRV_USART_INDEX_1:
        {
            returnValue = DRV_USART1_BaudSet(baud);
            break;
        }
</#if>
<#if CONFIG_DRV_USART_INST_IDX2 == true>
        case DRV_USART_INDEX_2:
        {
            returnValue = DRV_USART2_BaudSet(baud);
            break;
        }
</#if>
<#if CONFIG_DRV_USART_INST_IDX3 == true>
        case DRV_USART_INDEX_3:
        {
            returnValue = DRV_USART3_BaudSet(baud);
            break;
        }
</#if>
<#if CONFIG_DRV_USART_INST_IDX4 == true>
        case DRV_USART_INDEX_4:
        {
            returnValue = DRV_USART4_BaudSet(baud);
            break;
        }
</#if>
<#if CONFIG_DRV_USART_INST_IDX5 == true>
        case DRV_USART_INDEX_5:
        {
            returnValue = DRV_USART5_BaudSet(baud);
            break;
        }
</#if>
        default:
        {
            returnValue = DRV_USART_BAUD_SET_ERROR;
            break;
        }
    }
    return returnValue;
}

DRV_USART_LINE_CONTROL_SET_RESULT DRV_USART_LineControlSet(const DRV_HANDLE handle,const DRV_USART_LINE_CONTROL lineControl)
{
    uintptr_t instance;
    DRV_USART_LINE_CONTROL_SET_RESULT returnValue;

    instance = handle & 0x00FF;
    //As we are handling single client, only multiple instance is taken care.
    switch(instance)
    {
        case DRV_USART_INDEX_0:
        {
            returnValue = DRV_USART0_LineControlSet(lineControl);
            break;
        }
<#if CONFIG_DRV_USART_INST_IDX1 == true>
        case DRV_USART_INDEX_1:
        {
            returnValue = DRV_USART1_LineControlSet(lineControl);
            break;
        }
</#if>
<#if CONFIG_DRV_USART_INST_IDX2 == true>
        case DRV_USART_INDEX_2:
        {
            returnValue = DRV_USART2_LineControlSet(lineControl);
            break;
        }
</#if>
<#if CONFIG_DRV_USART_INST_IDX3 == true>
        case DRV_USART_INDEX_3:
        {
            returnValue = DRV_USART3_LineControlSet(lineControl);
            break;
        }
</#if>
<#if CONFIG_DRV_USART_INST_IDX4 == true>
        case DRV_USART_INDEX_4:
        {
            returnValue = DRV_USART4_LineControlSet(lineControl);
            break;
        }
</#if>
<#if CONFIG_DRV_USART_INST_IDX5 == true>
        case DRV_USART_INDEX_5:
        {
            returnValue = DRV_USART5_LineControlSet(lineControl);
            break;
        }
</#if>
        default:
        {
            returnValue = DRV_USART_LINE_CONTROL_SET_ERROR;
            break;
        }
    }
    return returnValue;
}


/*******************************************************************************
 End of File
*/
