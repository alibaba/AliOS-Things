/*******************************************************************************
  SPI Driver Interface

  Company:
    Microchip Technology Inc.

  File Name:
    drv_spi_definitions_pic32c.h

  Summary:
    SPI device driver interface file.

  Description:
    The SPI  driver provides a simple interface to manage the SPI module.
    This file defines the interface definitions and prototypes for the SPI
    driver.
*******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2016 released Microchip Technology Inc.  All rights reserved.

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


#ifndef _DRV_SPI_DEFINITIONS_PIC32C_H
#define _DRV_SPI_DEFINITIONS_PIC32C_H

// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************
// *****************************************************************************

#include "system_config.h"
#include "driver/driver_common.h"
#include "system/system.h"
#include "system/int/sys_int.h"
#if DRV_SPI_DMA
#include "system/dma/sys_dma.h"
#endif

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END

// *****************************************************************************
/* SPI Driver Buffer Handle

  Summary:
    Handle identifying a read or write buffer passed to the driver.

  Description:
    A buffer handle value is returned by a call to the DRV_SPI_BufferAddRead()/
    DRV_SPI_BufferAddWrite or DRV_SPI_BufferAddReadWrite() functions.
    This handle is associated with the buffer passed into the function and
    it allows the application to track the completion of the data from (or into)
    that buffer. The buffer handle value returned from the "buffer add" function
    is returned	back to the client by the "callback" function registered with
    the driver.

    The buffer handle assigned to a client request expires when the client has
    been notified of the completion of the buffer transfer (after event handler
    function that notifies the client returns) or after the buffer has been
    retired by the driver if no event handler callback was set.

  Remarks:
    None.
*/

typedef uintptr_t DRV_SPI_BUFFER_HANDLE;



// *****************************************************************************
/* SPI Driver Invalid Buffer Handle

  Summary:
    Definition of an invalid buffer handle.

  Description:
    This is the definition of an invalid buffer handle. An invalid buffer handle
    is returned by DRV_SPI_BufferAddRead() and DRV_SPI_BufferAddWrite()
    function if the buffer add request was not successful.

  Remarks:
    None.
*/

#define DRV_SPI_BUFFER_HANDLE_INVALID ((DRV_SPI_BUFFER_HANDLE)(-1))


// *****************************************************************************
/* SPI Driver Module Index Numbers

  Summary:
    SPI driver index definitions.

  Description:
    These constants provide the SPI driver index definitions.

  Remarks:
    These constants should be used in place of hard-coded numeric literals.

    These values should be passed into the DRV_SPI_Initialize and
    DRV_SPI_Open functions to identify the driver instance in use.
*/

#define DRV_SPI_INDEX_0         0
#define DRV_SPI_INDEX_1         1

// *****************************************************************************
/* SPI Driver Module Index Count

  Summary:
    Number of valid SPI driver indices.

  Description:
    This constant identifies the number of valid SPI driver indices.

  Remarks:
    This constant should be used in place of hard-coded numeric literals.

    This value is derived from device-specific header files defined as part
    of the peripheral libraries.
*/

#define DRV_SPI_INDEX_COUNT     SPI_NUMBER_OF_MODULES


// *****************************************************************************
/* SPI Clock Mode Selection

  Summary:
    Identifies the various clock modes of the SPI module.

  Description:
    This enumeration identifies the various clock modes of the SPI module.

  Remarks:
    None.
*/

typedef enum
{
    /* SPI Clock Mode 0:
        - Idle State of the clock is Low
        - Serial Output Data (SDO) Changed on Rising Edge of the clock
        - Serial Input Data (SDI) Sampled on Falling Edge of the clock */
    DRV_SPI_CLOCK_MODE_IDLE_LOW_EDGE_RISE
        /*DOM-IGNORE-BEGIN*/  = 0 /*DOM-IGNORE-END*/,

    /* SPI Clock Mode 1:
        - Idle State of the clock is Low
        - Serial Output Data (SDO) Changes on Falling Edge of the clock
        - Serial Input Data (SDI) Sampled on Rising Edge of the clock */
    DRV_SPI_CLOCK_MODE_IDLE_LOW_EDGE_FALL
        /*DOM-IGNORE-BEGIN*/  = 2 /*DOM-IGNORE-END*/,

    /* SPI Clock Mode 2:
        - Idle State of the clock is High
        - Serial Output Data (SDO) Changes on Falling Edge of the clock
        - Serial Input Data (SDI) Sampled on Rising Edge of the clock */
    DRV_SPI_CLOCK_MODE_IDLE_HIGH_EDGE_FALL
        /*DOM-IGNORE-BEGIN*/  = 1 /*DOM-IGNORE-END*/,

    /* SPI Clock Mode 3:
        - Idle State of the clock is High
        - Serial Output Data (SDO) Changes on Rising Edge of the clock
        - Serial Input Data (SDI) Sampled on Falling Edge of the clock */
    DRV_SPI_CLOCK_MODE_IDLE_HIGH_EDGE_RISE
        /*DOM-IGNORE-BEGIN*/  = 3 /*DOM-IGNORE-END*/

} DRV_SPI_CLOCK_MODE;


// *****************************************************************************
/* SPI Buffer Type Selection

  Summary:
    Identifies the various buffer types of the SPI module.

  Description:
    This enumeration identifies the various buffer types of the SPI module.

  Remarks:
    None.
*/

typedef enum
{
    /* SPI Buffer Type selection not supported.
     * By default it's regular buffer type */
    DRV_SPI_BUFFER_TYPE_NONE

} DRV_SPI_BUFFER_TYPE;


// *****************************************************************************
/* SPI Data Width Selection

  Summary:
    Identifies the various data width supported by SPI module.

  Description:
    This enumeration identifies the various data width supported by SPI module.

  Remarks:
    None.
*/

typedef enum {

    /* TODO: Add full range from 8 to 16 */
    SPI_COMMUNICATION_WIDTH_8BITS = 0,
    SPI_COMMUNICATION_WIDTH_16BITS = 8

} SPI_COMMUNICATION_WIDTH;
// *****************************************************************************
/* SPI Protocols Enumeration

  Summary:
    Identifies the various protocols of the SPI module.

  Description:
    This enumeration identifies the various protocols of the SPI module.

  Remarks:
    None.
*/

typedef enum
{
    /* SPI Protocol Type selection not supported.
     * By default it's standard mode */
    DRV_SPI_PROTOCOL_TYPE_NONE

} DRV_SPI_PROTOCOL_TYPE;




// *****************************************************************************
/* SPI Driver Buffer Events

   Summary
    Identifies the possible events that can result from a buffer add request.

   Description
    This enumeration identifies the possible events that can result from a
    buffer add request caused by the client calling either
    DRV_SPI_BufferAddRead2 or DRV_SPI_BufferAddWrite2 or similar functions.

   Remarks:
    One of these values is passed in the "event" parameter of the event
    handling callback function that the client registered during buffer add
    requests.
*/

typedef enum
{
    /* Buffer is pending to get processed */
    DRV_SPI_BUFFER_EVENT_PENDING,

    /* Buffer is being processed */
    DRV_SPI_BUFFER_EVENT_PROCESSING,

    /* All data from or to the buffer was transferred successfully. */
    DRV_SPI_BUFFER_EVENT_COMPLETE,

    /* There was an error while processing the buffer transfer request. */
    DRV_SPI_BUFFER_EVENT_ERROR

}DRV_SPI_BUFFER_EVENT;


// *****************************************************************************
/* SPI Usage Modes Enumeration

  Summary:
    Identifies the various usage modes of the SPI module.

  Description:
    This enumeration identifies the various usage modes of the SPI module.

  Remarks:
    None.
*/

typedef enum
{
    /* SPI Mode Master */
    DRV_SPI_MODE_MASTER
        /*DOM-IGNORE-BEGIN*/  = 0 /*DOM-IGNORE-END*/,

    /* SPI Mode Slave */
    DRV_SPI_MODE_SLAVE
        /*DOM-IGNORE-BEGIN*/  = 1 /*DOM-IGNORE-END*/

} DRV_SPI_MODE;


// *****************************************************************************
/* SPI Task Modes Enumeration

  Summary:
    Identifies the various modes of how the tasks function will be run.

  Description:
    This enumeration identifies the various tasks mode

  Remarks:
    None.
*/


typedef enum
{
    /* Task is configured to run in polled mode */
    DRV_SPI_TASK_MODE_POLLED
        /*DOM-IGNORE-BEGIN*/  = 0 /*DOM-IGNORE-END*/,

    /* Task is configured to run in interrupt mode */
    DRV_SPI_TASK_MODE_ISR
        /*DOM-IGNORE-BEGIN*/  = 1 /*DOM-IGNORE-END*/

} DRV_SPI_TASK_MODE;

// *****************************************************************************
/* SPI Driver Buffer Event Handler Function Pointer

   Summary:
    Pointer to a SPI Driver Buffer Event handler function

   Description:
    This data type defines the required function signature for the SPI driver
    buffer event handling callback function. A client must register a pointer
    to a buffer event handling function who's function signature (parameter
    and return value types) match the types specified by this function pointer
    in order to receive buffer related event calls back from the driver.

    The parameters and return values and return value are described here and
    a partial example implementation is provided.

  Parameters:
    event           - Identifies the type of event

    bufferHandle    - Handle identifying the buffer to which the vent relates

	context         - Value identifying the context of the application that
					  registered the event handling function.

  Returns:
    None.

  Example:
    <code>
    void APP_MyBufferEventHandler( DRV_SPI_BUFFER_EVENT event,
                                   DRV_SPI_BUFFER_HANDLE bufferHandle,
                                   uintptr_t context )
    {
        MY_APP_DATA_STRUCT pAppData = (MY_APP_DATA_STRUCT) context;

        switch(event)
        {
            case DRV_SPI_BUFFER_EVENT_COMPLETE:

                // Handle the completed buffer.
                break;

            case DRV_SPI_BUFFER_EVENT_ERROR:
            default:

                // Handle error.
                break;
        }
    }
    </code>

  Remarks:
    If the event is DRV_SPI_BUFFER_EVENT_COMPLETE, it means that the data was
    transferred successfully.

    If the event is DRV_SPI_BUFFER_EVENT_ERROR, it means that the data was
    not transferred successfully.

    The bufferHandle parameter contains the buffer handle of the buffer that
    failed.

    The context parameter contains the a handle to the client context,
    provided at the time the event handling function registration.
    This context handle value is passed back to the client as the "context"
    parameter.  It can be any value necessary to identify the client context
    or instance (such as a pointer to the client's data) instance of the
    client that made the buffer add request.

    The event handler function executes in an interrupt context when the driver
    is configured for interrupt mode operation. It is recommended of the
    application to not perform process intensive operations with in this
    function.
*/

typedef void ( *DRV_SPI_BUFFER_EVENT_HANDLER )  (DRV_SPI_BUFFER_EVENT event,
        DRV_SPI_BUFFER_HANDLE bufferHandle, void * context );


// *****************************************************************************
/* SPI Driver Initialization Data

  Summary:
    Defines the data required to initialize or reinitialize the SPI driver

  Description:
    This data type defines the data required to initialize or reinitialize the
    SPI driver. If the driver is built statically, the members of this data
    structure are statically over-ridden by static override definitions in the
    system_config.h file.

  Remarks:
    None.
*/

typedef struct _DRV_SPI_INIT
{
    /* System module initialization */
    SYS_MODULE_INIT                             moduleInit;

    /* Identifies peripheral(H/W) instance */
    SPI_MODULE_ID                               spiId;

    /* SPI Task Mode Type, Interrupt/Polled */
    DRV_SPI_TASK_MODE                           taskMode;

    /* SPI Usage Mode Type, Master/Slave */
    DRV_SPI_MODE                                spiMode;

    /* Communication Width */
    SPI_COMMUNICATION_WIDTH                     commWidth;

    /* Clock source */
    uint32_t                                    spiClkSrc;

    /* Baud rate */
    uint32_t                                    baudRate;

    /* SPI Clock mode, Edge and Polarity selection */
    DRV_SPI_CLOCK_MODE                          clockMode;

    /* SPI system interrupt source */
    INT_SOURCE                                  interruptSPI;

    /* Dummy byte value which will be used for dummy transmission */
    uint32_t                                    dummyByteValue;

#if DRV_SPI_DMA
    /* DMA Channel for Write */
    DMA_CHANNEL                                 txDmaChannel;

    /* DMA Channel for Read */
    DMA_CHANNEL                                 rxDmaChannel;

    /* Write DMA channel handle */
    DMA_TRIGGER_SOURCE                          dmaChannelTransmitTrigger;

    /* Read DMA channel handle */
    DMA_TRIGGER_SOURCE                          dmaChannelReceiveTrigger;

    /* DMA system interrupt source */
    INT_SOURCE                                  interruptDMA;
#endif

    /* This is the buffer queue size. This is the maximum
       number of  transfer requests that driver will queue.  */
    uint8_t                                     queueSize;

    /* This callback is fired when an operation is about to start on the
       SPI bus.  This allows the user to set any pins that need to be set.
       This callback may be called from an ISR so should not include OSAL
       calls.  The context parameter is the same one passed into the
       BufferAddRead, BufferAddWrite, BufferAddWriteRead function.
     */
    DRV_SPI_BUFFER_EVENT_HANDLER                operationStarting;

    /* This callback is fired when an operation has just completed on the
       SPI bus.  This allows the user to set any pins that need to be set.
       This callback may be called from an ISR so should not include OSAL
       calls.  The context parameter is the same one passed into the
       BufferAddRead, BufferAddWrite, BufferAddWriteRead function.
     */
    DRV_SPI_BUFFER_EVENT_HANDLER                 operationEnded;


} DRV_SPI_INIT;

// *****************************************************************************
/* SPI Driver Client Specific Configuration

  Summary:
    Defines the data that can be changed per client.

  Description:
    This data type defines the data can be configured per client.  This data can
    be per client, and overrides the configuration data contained inside of
    DRV_SPI_INIT.

  Remarks:
    None.
*/

typedef struct _DRV_SPI_CLIENT_DATA
{
    /* Baud Rate Value */
    uint32_t                        		baudRate;

    /* This callback is fired when an operation is about to start on the
       SPI bus.  This allows the user to set any pins that need to be set.
       This callback may be called from an ISR so should not include OSAL
       calls.  The context parameter is the same one passed into the
       BufferAddRead, BufferAddWrite, BufferAddWriteRead function.
     */
    DRV_SPI_BUFFER_EVENT_HANDLER            operationStarting;

    /* This callback is fired when an operation has just completed on the
       SPI bus.  This allows the user to set any pins that need to be set.
       This callback may be called from an ISR so should not include OSAL
       calls.  The context parameter is the same one passed into the
       BufferAddRead, BufferAddWrite, BufferAddWriteRead function.
     */
    DRV_SPI_BUFFER_EVENT_HANDLER            operationEnded;

}DRV_SPI_CLIENT_DATA;


//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END


#endif // #ifndef _DRV_SPI_DEFINITIONS_PIC32C_H

/*******************************************************************************
 End of File
*/

