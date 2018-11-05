/*******************************************************************************
  SPI Driver Dynamic to Static mapping

  Company:
    Microchip Technology Inc.

  File Name:
    drv_spi_mapping.c

  Summary:
    Source code for the SPI driver dynamic APIs to static API mapping.

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


SYS_MODULE_OBJ DRV_SPI_Initialize(const SYS_MODULE_INDEX index,const SYS_MODULE_INIT * const init)
{
    SYS_MODULE_OBJ returnValue;

    switch(index)
    {
        case DRV_SPI_INDEX_0:
        {
            returnValue = DRV_SPI0_Initialize();
            break;
        }
<#if CONFIG_DRV_SPI_IDX1 == true>
        case DRV_SPI_INDEX_1:
        {
            returnValue = DRV_SPI1_Initialize();
            break;
        }
</#if>
<#if CONFIG_DRV_SPI_IDX2 == true>
        case DRV_SPI_INDEX_2:
        {
            returnValue = DRV_SPI2_Initialize();
            break;
        }
</#if>
<#if CONFIG_DRV_SPI_IDX3 == true>
        case DRV_SPI_INDEX_3:
        {
            returnValue = DRV_SPI3_Initialize();
            break;
        }
</#if>
<#if CONFIG_DRV_SPI_IDX4 == true>
        case DRV_SPI_INDEX_4:
        {
            returnValue = DRV_SPI4_Initialize();
            break;
        }
</#if>
<#if CONFIG_DRV_SPI_IDX5 == true>
        case DRV_SPI_INDEX_5:
        {
            returnValue = DRV_SPI5_Initialize();
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

void DRV_SPI_Deinitialize( SYS_MODULE_OBJ object)
{
    switch(object)
    {
        case DRV_SPI_INDEX_0:
        {
            DRV_SPI0_Deinitialize();
            break;
        }
<#if CONFIG_DRV_SPI_IDX1 == true>
        case DRV_SPI_INDEX_1:
        {
            DRV_SPI1_Deinitialize();
            break;
        }
</#if>
<#if CONFIG_DRV_SPI_IDX2 == true>
        case DRV_SPI_INDEX_2:
        {
            DRV_SPI2_Deinitialize();
            break;
        }
</#if>
<#if CONFIG_DRV_SPI_IDX3 == true>
        case DRV_SPI_INDEX_3:
        {
            DRV_SPI3_Deinitialize();
            break;
        }
</#if>
<#if CONFIG_DRV_SPI_IDX4 == true>
        case DRV_SPI_INDEX_4:
        {
            DRV_SPI4_Deinitialize();
            break;
        }
</#if>
<#if CONFIG_DRV_SPI_IDX5 == true>
        case DRV_SPI_INDEX_5:
        {
            DRV_SPI5_Deinitialize();
            break;
        }
</#if>
        default:
        {
            break;
        }
    }
}

SYS_STATUS DRV_SPI_Status( SYS_MODULE_OBJ object)
{
    SYS_STATUS returnValue;

    switch(object)
    {
        case DRV_SPI_INDEX_0:
        {
            returnValue = DRV_SPI0_Status();
            break;
        }
<#if CONFIG_DRV_SPI_IDX1 == true>
        case DRV_SPI_INDEX_1:
        {
            returnValue = DRV_SPI1_Status();
            break;
        }
</#if>
<#if CONFIG_DRV_SPI_IDX2 == true>
        case DRV_SPI_INDEX_2:
        {
            returnValue = DRV_SPI2_Status();
            break;
        }
</#if>
<#if CONFIG_DRV_SPI_IDX3 == true>
        case DRV_SPI_INDEX_3:
        {
            returnValue = DRV_SPI3_Status();
            break;
        }
</#if>
<#if CONFIG_DRV_SPI_IDX4 == true>
        case DRV_SPI_INDEX_4:
        {
            returnValue = DRV_SPI4_Status();
            break;
        }
</#if>
<#if CONFIG_DRV_SPI_IDX5 == true>
        case DRV_SPI_INDEX_5:
        {
            returnValue = DRV_SPI5_Status();
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

void DRV_SPI_Tasks ( SYS_MODULE_OBJ object )
{
    switch(object)
    {
        case DRV_SPI_INDEX_0:
        {
            DRV_SPI0_Tasks();
            break;
        }
<#if CONFIG_DRV_SPI_IDX1 == true>
        case DRV_SPI_INDEX_1:
        {
            DRV_SPI1_Tasks();
            break;
        }
</#if>
<#if CONFIG_DRV_SPI_IDX2 == true>
        case DRV_SPI_INDEX_2:
        {
            DRV_SPI2_Tasks();
            break;
        }
</#if>
<#if CONFIG_DRV_SPI_IDX3 == true>
        case DRV_SPI_INDEX_3:
        {
            DRV_SPI3_Tasks();
            break;
        }
</#if>
<#if CONFIG_DRV_SPI_IDX4 == true>
        case DRV_SPI_INDEX_4:
        {
            DRV_SPI4_Tasks();
            break;
        }
</#if>
<#if CONFIG_DRV_SPI_IDX5 == true>
        case DRV_SPI_INDEX_5:
        {
            DRV_SPI5_Tasks();
            break;
        }
</#if>
        default:
        {
            break;
        }
    }
}


//client interface
DRV_HANDLE DRV_SPI_Open( const SYS_MODULE_INDEX index, const DRV_IO_INTENT ioIntent)
{
    DRV_HANDLE returnValue;

    switch(index)
    {
        case DRV_SPI_INDEX_0:
        {
            returnValue = DRV_SPI0_Open(index,ioIntent);
            break;
        }
<#if CONFIG_DRV_SPI_IDX1 == true>
        case DRV_SPI_INDEX_1:
        {
            returnValue = DRV_SPI1_Open(index,ioIntent);
            break;
        }
</#if>
<#if CONFIG_DRV_SPI_IDX2 == true>
        case DRV_SPI_INDEX_2:
        {
            returnValue = DRV_SPI2_Open(index,ioIntent);
            break;
        }
</#if>
<#if CONFIG_DRV_SPI_IDX3 == true>
        case DRV_SPI_INDEX_3:
        {
            returnValue = DRV_SPI3_Open(index,ioIntent);
            break;
        }
</#if>
<#if CONFIG_DRV_SPI_IDX4 == true>
        case DRV_SPI_INDEX_4:
        {
            returnValue = DRV_SPI4_Open(index,ioIntent);
            break;
        }
</#if>
<#if CONFIG_DRV_SPI_IDX5 == true>
        case DRV_SPI_INDEX_5:
        {
            returnValue = DRV_SPI5_Open(index,ioIntent);
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

void DRV_SPI_Close( const DRV_HANDLE handle)
{
    uintptr_t instance;

    instance = handle & 0x00FF;
    //As we are handling single client, only multiple instance is taken care.
    switch(instance)
    {
        case DRV_SPI_INDEX_0:
        {
            DRV_SPI0_Close( );
            break;
        }
<#if CONFIG_DRV_SPI_IDX1 == true>
        case DRV_SPI_INDEX_1:
        {
            DRV_SPI1_Close( );
            break;
        }
</#if>
<#if CONFIG_DRV_SPI_IDX2 == true>
        case DRV_SPI_INDEX_2:
        {
            DRV_SPI2_Close( );
            break;
        }
</#if>
<#if CONFIG_DRV_SPI_IDX3 == true>
        case DRV_SPI_INDEX_3:
        {
            DRV_SPI3_Close( );
            break;
        }
</#if>
<#if CONFIG_DRV_SPI_IDX4 == true>
        case DRV_SPI_INDEX_4:
        {
            DRV_SPI4_Close( );
            break;
        }
</#if>
<#if CONFIG_DRV_SPI_IDX5 == true>
        case DRV_SPI_INDEX_5:
        {
            DRV_SPI5_Close( );
            break;
        }
</#if>
        default:
        {
            break;
        }
    }
}

int32_t DRV_SPI_ClientConfigure ( DRV_HANDLE handle, const DRV_SPI_CLIENT_DATA * cfgData  )
{
    uintptr_t instance;
    int32_t returnValue;

    instance = handle & 0x00FF;
    //As we are handling single client, only multiple instance is taken care.
    switch(instance)
    {
        case DRV_SPI_INDEX_0:
        {
            returnValue = DRV_SPI0_ClientConfigure( cfgData );
            break;
        }
<#if CONFIG_DRV_SPI_IDX1 == true>
        case DRV_SPI_INDEX_1:
        {
            returnValue = DRV_SPI1_ClientConfigure( cfgData );
            break;
        }
</#if>
<#if CONFIG_DRV_SPI_IDX2 == true>
        case DRV_SPI_INDEX_2:
        {
            returnValue = DRV_SPI2_ClientConfigure( cfgData );
            break;
        }
</#if>
<#if CONFIG_DRV_SPI_IDX3 == true>
        case DRV_SPI_INDEX_3:
        {
            returnValue = DRV_SPI3_ClientConfigure( cfgData );
            break;
        }
</#if>
<#if CONFIG_DRV_SPI_IDX4 == true>
        case DRV_SPI_INDEX_4:
        {
            returnValue = DRV_SPI4_ClientConfigure( cfgData );
            break;
        }
</#if>
<#if CONFIG_DRV_SPI_IDX5 == true>
        case DRV_SPI_INDEX_5:
        {
            returnValue = DRV_SPI5_ClientConfigure( cfgData );
            break;
        }
</#if>
        default:
        {
            returnValue = (int32_t)DRV_HANDLE_INVALID;
            break;
        }
    }
    return returnValue;
}


//Read & Write Client Interface

DRV_SPI_BUFFER_HANDLE DRV_SPI_BufferAddRead ( DRV_HANDLE handle,void *rxBuffer,size_t size,DRV_SPI_BUFFER_EVENT_HANDLER completeCB,void * context)
{
    uintptr_t instance;
    DRV_SPI_BUFFER_HANDLE returnValue;

    instance = handle & 0x00FF;
    //As we are handling single client, only multiple instance is taken care.
    switch(instance)
    {
        case DRV_SPI_INDEX_0:
        {
            returnValue = DRV_SPI0_BufferAddRead ( rxBuffer, size, completeCB, context);
            break;
        }
<#if CONFIG_DRV_SPI_IDX1 == true>
        case DRV_SPI_INDEX_1:
        {
            returnValue = DRV_SPI1_BufferAddRead ( rxBuffer, size, completeCB, context);
            break;
        }
</#if>
<#if CONFIG_DRV_SPI_IDX2 == true>
        case DRV_SPI_INDEX_2:
        {
            returnValue = DRV_SPI2_BufferAddRead ( rxBuffer, size, completeCB, context);
            break;
        }
</#if>
<#if CONFIG_DRV_SPI_IDX3 == true>
        case DRV_SPI_INDEX_3:
        {
            returnValue = DRV_SPI3_BufferAddRead ( rxBuffer, size, completeCB, context);
            break;
        }
</#if>
<#if CONFIG_DRV_SPI_IDX4 == true>
        case DRV_SPI_INDEX_4:
        {
            returnValue = DRV_SPI4_BufferAddRead ( rxBuffer, size, completeCB, context);
            break;
        }
</#if>
<#if CONFIG_DRV_SPI_IDX5 == true>
        case DRV_SPI_INDEX_5:
        {
            returnValue = DRV_SPI5_BufferAddRead ( rxBuffer, size, completeCB, context);
            break;
        }
</#if>
        default:
        {
            returnValue = DRV_SPI_BUFFER_HANDLE_INVALID;
            break;
        }
    }
    return returnValue;
}

DRV_SPI_BUFFER_HANDLE DRV_SPI_BufferAddWrite ( DRV_HANDLE handle,void *txBuffer,size_t size,DRV_SPI_BUFFER_EVENT_HANDLER completeCB,void * context)
{
    uintptr_t instance;
    DRV_SPI_BUFFER_HANDLE returnValue;

    instance = handle & 0x00FF;
    //As we are handling single client, only multiple instance is taken care.
    switch(instance)
    {
        case DRV_SPI_INDEX_0:
        {
            returnValue = DRV_SPI0_BufferAddWrite ( txBuffer, size, completeCB, context);
            break;
        }
<#if CONFIG_DRV_SPI_IDX1 == true>
        case DRV_SPI_INDEX_1:
        {
            returnValue = DRV_SPI1_BufferAddWrite ( txBuffer, size, completeCB, context);
            break;
        }
</#if>
<#if CONFIG_DRV_SPI_IDX2 == true>
        case DRV_SPI_INDEX_2:
        {
            returnValue = DRV_SPI2_BufferAddWrite ( txBuffer, size, completeCB, context);
            break;
        }
</#if>
<#if CONFIG_DRV_SPI_IDX3 == true>
        case DRV_SPI_INDEX_3:
        {
            returnValue = DRV_SPI3_BufferAddWrite ( txBuffer, size, completeCB, context);
            break;
        }
</#if>
<#if CONFIG_DRV_SPI_IDX4 == true>
        case DRV_SPI_INDEX_4:
        {
            returnValue = DRV_SPI4_BufferAddWrite ( txBuffer, size, completeCB, context);
            break;
        }
</#if>
<#if CONFIG_DRV_SPI_IDX5 == true>
        case DRV_SPI_INDEX_5:
        {
            returnValue = DRV_SPI5_BufferAddWrite ( txBuffer, size, completeCB, context);
            break;
        }
</#if>
        default:
        {
            returnValue = DRV_SPI_BUFFER_HANDLE_INVALID;
            break;
        }
    }
    return returnValue;
}

DRV_SPI_BUFFER_HANDLE DRV_SPI_BufferAddWriteRead ( DRV_HANDLE handle, void *txBuffer, size_t txSize,
    void *rxBuffer, size_t rxSize, DRV_SPI_BUFFER_EVENT_HANDLER completeCB, void * context )
{
    uintptr_t instance;
    DRV_SPI_BUFFER_HANDLE returnValue;

    instance = handle & 0x00FF;
    //As we are handling single client, only multiple instance is taken care.
    //Mapping to a new static driver's API name.
    switch(instance)
    {
        case DRV_SPI_INDEX_0:
        {
<#if CONFIG_DRV_SPI_USE_BLOCKING>
            returnValue = DRV_SPI0_BufferAddWriteRead ( txBuffer, rxBuffer, txSize );
<#else>
            returnValue = DRV_SPI0_BufferAddWriteReadNew ( txBuffer, txSize, rxBuffer, rxSize, completeCB, context );
</#if>
            break;
        }
<#if CONFIG_DRV_SPI_IDX1 == true>
        case DRV_SPI_INDEX_1:
        {
<#if CONFIG_DRV_SPI_USE_BLOCKING>
            returnValue = DRV_SPI1_BufferAddWriteRead ( txBuffer, rxBuffer, txSize );
<#else>
            returnValue = DRV_SPI1_BufferAddWriteReadNew ( txBuffer, txSize, rxBuffer, rxSize, completeCB, context );
</#if>
            break;
        }
</#if>
<#if CONFIG_DRV_SPI_IDX2 == true>
        case DRV_SPI_INDEX_2:
        {
<#if CONFIG_DRV_SPI_USE_BLOCKING>
            returnValue = DRV_SPI2_BufferAddWriteRead ( txBuffer, rxBuffer, txSize );
<#else>
            returnValue = DRV_SPI2_BufferAddWriteReadNew ( txBuffer, txSize, rxBuffer, rxSize, completeCB, context );
</#if>
            break;
        }
</#if>
<#if CONFIG_DRV_SPI_IDX3 == true>
        case DRV_SPI_INDEX_3:
        {
<#if CONFIG_DRV_SPI_USE_BLOCKING>
            returnValue = DRV_SPI3_BufferAddWriteRead ( txBuffer, rxBuffer, txSize );
<#else>
            returnValue = DRV_SPI3_BufferAddWriteReadNew ( txBuffer, txSize, rxBuffer, rxSize, completeCB, context );
</#if>
            break;
        }
</#if>
<#if CONFIG_DRV_SPI_IDX4 == true>
        case DRV_SPI_INDEX_4:
        {
<#if CONFIG_DRV_SPI_USE_BLOCKING>
            returnValue = DRV_SPI4_BufferAddWriteRead ( txBuffer, rxBuffer, txSize );
<#else>
            returnValue = DRV_SPI4_BufferAddWriteReadNew ( txBuffer, txSize, rxBuffer, rxSize, completeCB, context );
</#if>
            break;
        }
</#if>
<#if CONFIG_DRV_SPI_IDX5 == true>
        case DRV_SPI_INDEX_5:
        {
<#if CONFIG_DRV_SPI_USE_BLOCKING>
            returnValue = DRV_SPI5_BufferAddWriteRead ( txBuffer, rxBuffer, txSize );
<#else>
            returnValue = DRV_SPI5_BufferAddWriteReadNew ( txBuffer, txSize, rxBuffer, rxSize, completeCB, context );
</#if>
            break;
        }
</#if>
        default:
        {
            returnValue = DRV_SPI_BUFFER_HANDLE_INVALID;
            break;
        }
    }
    return returnValue;
}

DRV_SPI_BUFFER_HANDLE DRV_SPI_BufferAddRead2 ( DRV_HANDLE handle,void *rxBuffer,size_t size,DRV_SPI_BUFFER_EVENT_HANDLER completeCB,void * context, DRV_SPI_BUFFER_HANDLE * jobHandle)
{
    uintptr_t instance;
    DRV_SPI_BUFFER_HANDLE returnValue;

    instance = handle & 0x00FF;
    //As we are handling single client, only multiple instance is taken care.
    switch(instance)
    {
        case DRV_SPI_INDEX_0:
        {
            returnValue = DRV_SPI0_BufferAddRead2 ( rxBuffer, size, completeCB, context, jobHandle);
            break;
        }
<#if CONFIG_DRV_SPI_IDX1 == true>
        case DRV_SPI_INDEX_1:
        {
            returnValue = DRV_SPI1_BufferAddRead2 ( rxBuffer, size, completeCB, context, jobHandle);
            break;
        }
</#if>
<#if CONFIG_DRV_SPI_IDX2 == true>
        case DRV_SPI_INDEX_2:
        {
            returnValue = DRV_SPI2_BufferAddRead2 ( rxBuffer, size, completeCB, context, jobHandle);
            break;
        }
</#if>
<#if CONFIG_DRV_SPI_IDX3 == true>
        case DRV_SPI_INDEX_3:
        {
            returnValue = DRV_SPI3_BufferAddRead2 ( rxBuffer, size, completeCB, context, jobHandle);
            break;
        }
</#if>
<#if CONFIG_DRV_SPI_IDX4 == true>
        case DRV_SPI_INDEX_4:
        {
            returnValue = DRV_SPI4_BufferAddRead2 ( rxBuffer, size, completeCB, context, jobHandle);
            break;
        }
</#if>
<#if CONFIG_DRV_SPI_IDX5 == true>
        case DRV_SPI_INDEX_5:
        {
            returnValue = DRV_SPI5_BufferAddRead2 ( rxBuffer, size, completeCB, context, jobHandle);
            break;
        }
</#if>
        default:
        {
            returnValue = DRV_SPI_BUFFER_HANDLE_INVALID;
            break;
        }
    }
    return returnValue;
}

DRV_SPI_BUFFER_HANDLE DRV_SPI_BufferAddWrite2 ( DRV_HANDLE handle,void *txBuffer,size_t size,DRV_SPI_BUFFER_EVENT_HANDLER completeCB,void * context, DRV_SPI_BUFFER_HANDLE * jobHandle)
{
    uintptr_t instance;
    DRV_SPI_BUFFER_HANDLE returnValue;

    instance = handle & 0x00FF;
    //As we are handling single client, only multiple instance is taken care.
    switch(instance)
    {
        case DRV_SPI_INDEX_0:
        {
            returnValue = DRV_SPI0_BufferAddWrite2 ( txBuffer, size, completeCB, context, jobHandle);
            break;
        }
<#if CONFIG_DRV_SPI_IDX1 == true>
        case DRV_SPI_INDEX_1:
        {
            returnValue = DRV_SPI1_BufferAddWrite2 ( txBuffer, size, completeCB, context, jobHandle);
            break;
        }
</#if>
<#if CONFIG_DRV_SPI_IDX2 == true>
        case DRV_SPI_INDEX_2:
        {
            returnValue = DRV_SPI2_BufferAddWrite2 ( txBuffer, size, completeCB, context, jobHandle);
            break;
        }
</#if>
<#if CONFIG_DRV_SPI_IDX3 == true>
        case DRV_SPI_INDEX_3:
        {
            returnValue = DRV_SPI3_BufferAddWrite2 ( txBuffer, size, completeCB, context, jobHandle);
            break;
        }
</#if>
<#if CONFIG_DRV_SPI_IDX4 == true>
        case DRV_SPI_INDEX_4:
        {
            returnValue = DRV_SPI4_BufferAddWrite2 ( txBuffer, size, completeCB, context, jobHandle);
            break;
        }
</#if>
<#if CONFIG_DRV_SPI_IDX5 == true>
        case DRV_SPI_INDEX_5:
        {
            returnValue = DRV_SPI5_BufferAddWrite2 ( txBuffer, size, completeCB, context, jobHandle);
            break;
        }
</#if>
        default:
        {
            returnValue = DRV_SPI_BUFFER_HANDLE_INVALID;
            break;
        }
    }
    return returnValue;
}

DRV_SPI_BUFFER_HANDLE DRV_SPI_BufferAddWriteRead2 ( DRV_HANDLE handle, void *txBuffer, size_t txSize,void *rxBuffer, size_t rxSize,DRV_SPI_BUFFER_EVENT_HANDLER completeCB,void * context, DRV_SPI_BUFFER_HANDLE * jobHandle)
{
    uintptr_t instance;
    DRV_SPI_BUFFER_HANDLE returnValue;

    instance = handle & 0x00FF;
    //As we are handling single client, only multiple instance is taken care.
    switch(instance)
    {
        case DRV_SPI_INDEX_0:
        {
            returnValue = DRV_SPI0_BufferAddWriteRead2 ( txBuffer, txSize, rxBuffer, rxSize, completeCB, context, jobHandle);
            break;
        }
<#if CONFIG_DRV_SPI_IDX1 == true>
        case DRV_SPI_INDEX_1:
        {
            returnValue = DRV_SPI1_BufferAddWriteRead2 ( txBuffer, txSize, rxBuffer, rxSize, completeCB, context, jobHandle );
            break;
        }
</#if>
<#if CONFIG_DRV_SPI_IDX2 == true>
        case DRV_SPI_INDEX_2:
        {
            returnValue = DRV_SPI2_BufferAddWriteRead2 ( txBuffer, txSize, rxBuffer, rxSize, completeCB, context, jobHandle );
            break;
        }
</#if>
<#if CONFIG_DRV_SPI_IDX3 == true>
        case DRV_SPI_INDEX_3:
        {
            returnValue = DRV_SPI3_BufferAddWriteRead2 ( txBuffer, txSize, rxBuffer, rxSize, completeCB, context, jobHandle );
            break;
        }
</#if>
<#if CONFIG_DRV_SPI_IDX4 == true>
        case DRV_SPI_INDEX_4:
        {
            returnValue = DRV_SPI4_BufferAddWriteRead2 ( txBuffer, txSize, rxBuffer, rxSize, completeCB, context, jobHandle );
            break;
        }
</#if>
<#if CONFIG_DRV_SPI_IDX5 == true>
        case DRV_SPI_INDEX_5:
        {
            returnValue = DRV_SPI5_BufferAddWriteRead2 ( txBuffer, txSize, rxBuffer, rxSize, completeCB, context, jobHandle );
            break;
        }
</#if>
        default:
        {
            returnValue = DRV_SPI_BUFFER_HANDLE_INVALID;
            break;
        }
    }
    return returnValue;
}

DRV_SPI_BUFFER_EVENT DRV_SPI_BufferStatus ( DRV_SPI_BUFFER_HANDLE bufferHandle )
{
    //This function is independent of instance or client.
    return DRV_SPI0_BufferStatus ( bufferHandle);
}

/*******************************************************************************
 End of File
*/
