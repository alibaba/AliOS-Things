/*******************************************************************************
  SPI Driver Interface Declarations for Static Single Instance Driver

  Company:
    Microchip Technology Inc.

  File Name:
    drv_spi_static.h

  Summary:
    SPI driver interface declarations for the static single instance driver.

  Description:
    The SPI device driver provides a simple interface to manage the SPI
    modules on Microchip microcontrollers. This file defines the interface
    Declarations for the SPI static driver.

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

#ifndef _DRV_SPI_STATIC_H
#define _DRV_SPI_STATIC_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
#include "driver/spi/drv_spi.h"



// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END

<#macro DRV_SPI_STATIC_API DRV_INSTANCE>
// *********************************************************************************************
// *********************************************************************************************
// Section: System Interface Headers for the Instance ${DRV_INSTANCE} of SPI static driver
// *********************************************************************************************
// *********************************************************************************************

SYS_MODULE_OBJ DRV_SPI${DRV_INSTANCE}_Initialize( void );
void DRV_SPI${DRV_INSTANCE}_Deinitialize ( void );
SYS_STATUS DRV_SPI${DRV_INSTANCE}_Status ( void );
void DRV_SPI${DRV_INSTANCE}_Tasks ( void );

// *********************************************************************************************
// *********************************************************************************************
// Section: General Client Interface Headers for the Instance ${DRV_INSTANCE} of SPI static driver
// *********************************************************************************************
// *********************************************************************************************

DRV_HANDLE DRV_SPI${DRV_INSTANCE}_Open ( const SYS_MODULE_INDEX drvIndex, const DRV_IO_INTENT ioIntent );
void DRV_SPI${DRV_INSTANCE}_Close ( void );
int32_t DRV_SPI${DRV_INSTANCE}_ClientConfigure ( const DRV_SPI_CLIENT_DATA * cfgData );

// *********************************************************************************************
// *********************************************************************************************
// Section: Read & Write Client Interface Headers for the Instance ${DRV_INSTANCE} of SPI static driver
// *********************************************************************************************
// *********************************************************************************************

DRV_SPI_BUFFER_HANDLE DRV_SPI${DRV_INSTANCE}_BufferAddRead ( void *rxBuffer, size_t size, DRV_SPI_BUFFER_EVENT_HANDLER completeCB, void * context);

DRV_SPI_BUFFER_HANDLE DRV_SPI${DRV_INSTANCE}_BufferAddWrite ( void *txBuffer, size_t size, DRV_SPI_BUFFER_EVENT_HANDLER completeCB, void * context );

DRV_SPI_BUFFER_HANDLE DRV_SPI${DRV_INSTANCE}_BufferAddRead2 ( void *rxBuffer, size_t size, DRV_SPI_BUFFER_EVENT_HANDLER completeCB, void * context, DRV_SPI_BUFFER_HANDLE * jobHandle);

DRV_SPI_BUFFER_HANDLE DRV_SPI${DRV_INSTANCE}_BufferAddWrite2 ( void *txBuffer, size_t size, DRV_SPI_BUFFER_EVENT_HANDLER completeCB, void * context, DRV_SPI_BUFFER_HANDLE * jobHandle );

DRV_SPI_BUFFER_HANDLE DRV_SPI${DRV_INSTANCE}_BufferAddWriteRead2 ( void *txBuffer, size_t txSize,void *rxBuffer, size_t rxSize, DRV_SPI_BUFFER_EVENT_HANDLER completeCB, void * context,  DRV_SPI_BUFFER_HANDLE * jobHandle);

DRV_SPI_BUFFER_EVENT DRV_SPI${DRV_INSTANCE}_BufferStatus ( DRV_SPI_BUFFER_HANDLE bufferHandle );

/* This API name was present in the old static driver which is maintained for compatibility.
   So, giving a new name for the new static driver which is taken care of while mapping */
#define  DRV_SPI${DRV_INSTANCE}_BufferAddWriteReadNew(txbuff, txsize, rxbuff, rxsize, cb, ct ) \
    DRV_SPI${DRV_INSTANCE}_BufferAddWriteRead2((txbuff), (txsize), (rxbuff), (rxsize), (cb), (ct), NULL)



// *********************************************************************************************
// *********************************************************************************************
// Section: Old static driver compatibility APIs, these will be deprecated.
// *********************************************************************************************
// *********************************************************************************************
bool DRV_SPI${DRV_INSTANCE}_ReceiverBufferIsFull( void );
bool DRV_SPI${DRV_INSTANCE}_TransmitterBufferIsFull( void );
int32_t DRV_SPI${DRV_INSTANCE}_BufferAddWriteRead(const void * txBuffer, void * rxBuffer, uint32_t size);
// *********************************************************************************************


</#macro>
<#if CONFIG_DRV_SPI_IDX0 == true>
<@DRV_SPI_STATIC_API
DRV_INSTANCE="0"
/>
</#if>
<#if CONFIG_DRV_SPI_IDX1 == true>
<@DRV_SPI_STATIC_API
DRV_INSTANCE="1"
/>
</#if>
<#if CONFIG_DRV_SPI_IDX2 == true>
<@DRV_SPI_STATIC_API
DRV_INSTANCE="2"
/>
</#if>
<#if CONFIG_DRV_SPI_IDX3 == true>
<@DRV_SPI_STATIC_API
DRV_INSTANCE="3"
/>
</#if>
<#if CONFIG_DRV_SPI_IDX4 == true>
<@DRV_SPI_STATIC_API
DRV_INSTANCE="4"
/>
</#if>
<#if CONFIG_DRV_SPI_IDX5 == true>
<@DRV_SPI_STATIC_API
DRV_INSTANCE="5"
/>
</#if>

// DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
// DOM-IGNORE-END

#endif // #ifndef _DRV_SPI_STATIC_H
/*******************************************************************************
 End of File
*/
