/*******************************************************************************
  USART Driver Interface Declarations for Static Single Instance Driver

  Company:
    Microchip Technology Inc.

  File Name:
    drv_usart_static.h

  Summary:
    USART driver interface declarations for the static single instance driver.

  Description:
    The USART device driver provides a simple interface to manage the USART
    modules on Microchip microcontrollers. This file defines the interface
    Declarations for the USART static driver.

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

#ifndef _DRV_USART_STATIC_H
#define _DRV_USART_STATIC_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
<#if CONFIG_ARCH_ARM>
<#if CONFIG_USE_DRV_USART_DMA_SUPPORT == true>
#include "driver/usart/src/drv_usart_static_local_dma_pic32c.h"
<#else>
#include "driver/usart/src/drv_usart_static_local_pic32c.h"
</#if>
<#else>
<#if CONFIG_USE_DRV_USART_DMA_SUPPORT == true>
#include "driver/usart/src/drv_usart_static_local_dma.h"
<#else>
#include "driver/usart/src/drv_usart_static_local.h"
</#if>
</#if>

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END
<#macro DRV_USART_STATIC_API DRV_INSTANCE>

// *********************************************************************************************
// *********************************************************************************************
// Section: System Interface Headers for the Instance ${DRV_INSTANCE} of USART static driver
// *********************************************************************************************
// *********************************************************************************************

SYS_MODULE_OBJ DRV_USART${DRV_INSTANCE}_Initialize(void);
void  DRV_USART${DRV_INSTANCE}_Deinitialize(void);
SYS_STATUS DRV_USART${DRV_INSTANCE}_Status(void);
void DRV_USART${DRV_INSTANCE}_TasksTransmit(void);
void DRV_USART${DRV_INSTANCE}_TasksReceive(void);
void DRV_USART${DRV_INSTANCE}_TasksError(void);

// *********************************************************************************************
// *********************************************************************************************
// Section: General Client Interface Headers for the Instance ${DRV_INSTANCE} of USART static driver
// *********************************************************************************************
// *********************************************************************************************

DRV_HANDLE DRV_USART${DRV_INSTANCE}_Open(const SYS_MODULE_INDEX index, const DRV_IO_INTENT ioIntent);
void DRV_USART${DRV_INSTANCE}_Close(void);
DRV_USART_CLIENT_STATUS DRV_USART${DRV_INSTANCE}_ClientStatus(void);
DRV_USART_TRANSFER_STATUS DRV_USART${DRV_INSTANCE}_TransferStatus(void);
DRV_USART_ERROR DRV_USART${DRV_INSTANCE}_ErrorGet(void);

<#if CONFIG_DRV_USART_BUFFER_QUEUE_SUPPORT ==true>
// *********************************************************************************************
// *********************************************************************************************
// Section: Buffer Queue Model Client Interface Headers for the Instance ${DRV_INSTANCE} of USART static driver
// *********************************************************************************************
// *********************************************************************************************

void DRV_USART${DRV_INSTANCE}_BufferAddWrite (DRV_USART_BUFFER_HANDLE * bufferHandle, void * buffer, const size_t size);
void DRV_USART${DRV_INSTANCE}_BufferAddRead(DRV_USART_BUFFER_HANDLE * const bufferHandle,void * buffer,const size_t size);
void DRV_USART${DRV_INSTANCE}_BufferEventHandlerSet(const DRV_USART_BUFFER_EVENT_HANDLER eventHandler,const uintptr_t context);
size_t DRV_USART${DRV_INSTANCE}_BufferCompletedBytesGet(DRV_USART_BUFFER_HANDLE bufferHandle );
DRV_USART_BUFFER_RESULT DRV_USART${DRV_INSTANCE}_BufferRemove(DRV_USART_BUFFER_HANDLE bufferHandle);
/* The following function will be deprecated */
size_t DRV_USART${DRV_INSTANCE}_BufferProcessedSizeGet(DRV_USART_BUFFER_HANDLE bufferHandle );

</#if>
<#if CONFIG_DRV_USART_READ_WRITE_MODEL_SUPPORT ==true>
// *********************************************************************************************
// *********************************************************************************************
// Section: Read Write Model Client Interface Headers for the Instance ${DRV_INSTANCE} of USART static driver
// *********************************************************************************************
// *********************************************************************************************

size_t DRV_USART${DRV_INSTANCE}_Read( void * buffer,const size_t numbytes);
size_t DRV_USART${DRV_INSTANCE}_Write( void * buffer, const size_t numbytes);

</#if>
<#if CONFIG_DRV_USART_BYTE_MODEL_SUPPORT ==true>
// *********************************************************************************************
// *********************************************************************************************
// Section: Byte Model Client Interface Headers for the Instance ${DRV_INSTANCE} of USART static driver
// *********************************************************************************************
// *********************************************************************************************

uint8_t DRV_USART${DRV_INSTANCE}_ReadByte( void);
void DRV_USART${DRV_INSTANCE}_WriteByte( const uint8_t byte);
unsigned int DRV_USART${DRV_INSTANCE}_ReceiverBufferSizeGet(void);
unsigned int DRV_USART${DRV_INSTANCE}_TransmitBufferSizeGet(void);
bool DRV_USART${DRV_INSTANCE}_ReceiverBufferIsEmpty( void );
bool DRV_USART${DRV_INSTANCE}_TransmitBufferIsFull(void);
<#if CONFIG_DRV_USART_BYTE_MODEL_CALLBACK ==true>
void DRV_USART${DRV_INSTANCE}_ByteTransmitCallbackSet(const DRV_USART_BYTE_EVENT_HANDLER eventHandler);
void DRV_USART${DRV_INSTANCE}_ByteReceiveCallbackSet(const DRV_USART_BYTE_EVENT_HANDLER eventHandler);
void DRV_USART${DRV_INSTANCE}_ByteErrorCallbackSet(const DRV_USART_BYTE_EVENT_HANDLER eventHandler);
</#if>

</#if>
// *********************************************************************************************
// *********************************************************************************************
// Section: Set up Client Interface Headers for the Instance ${DRV_INSTANCE} of USART static driver
// *********************************************************************************************
// *********************************************************************************************
DRV_USART_BAUD_SET_RESULT DRV_USART${DRV_INSTANCE}_BaudSet(uint32_t baud);
DRV_USART_LINE_CONTROL_SET_RESULT DRV_USART${DRV_INSTANCE}_LineControlSet(DRV_USART_LINE_CONTROL lineControlMode);
</#macro>
<#if CONFIG_DRV_USART_INST_IDX0 == true>
<@DRV_USART_STATIC_API
DRV_INSTANCE="0"
/>
</#if>
<#if CONFIG_DRV_USART_INST_IDX1 == true>
<@DRV_USART_STATIC_API
DRV_INSTANCE="1"
/>
</#if>
<#if CONFIG_DRV_USART_INST_IDX2 == true>
<@DRV_USART_STATIC_API
DRV_INSTANCE="2"
/>
</#if>
<#if CONFIG_DRV_USART_INST_IDX3 == true>
<@DRV_USART_STATIC_API
DRV_INSTANCE="3"
/>
</#if>
<#if CONFIG_DRV_USART_INST_IDX4 == true>
<@DRV_USART_STATIC_API
DRV_INSTANCE="4"
/>
</#if>
<#if CONFIG_DRV_USART_INST_IDX5 == true>
<@DRV_USART_STATIC_API
DRV_INSTANCE="5"
/>
</#if>

// DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
// DOM-IGNORE-END

#endif // #ifndef _DRV_USART_STATIC_H
/*******************************************************************************
 End of File
*/
