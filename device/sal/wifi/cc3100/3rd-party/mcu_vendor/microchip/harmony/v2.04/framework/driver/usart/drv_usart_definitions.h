/*******************************************************************************
  USART Driver Definitions Header File

  Company:
    Microchip Technology Inc.

  File Name:
    drv_usart_definitions.h

  Summary:
    USART Driver Definitions Header File

  Description:
    This file will provide enumerations and other dependencies needed by
    USART driver to manage the USART/UART module on PIC32 microcontrollers.
*******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2012-2014 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

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

#ifndef _DRV_USART_DEFINITIONS_H
#define _DRV_USART_DEFINITIONS_H

// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************

#include "system_config.h"
#include "driver/driver_common.h"
#include "peripheral/usart/plib_usart.h"
#include "system/system.h"
#include "system/int/sys_int.h"
#include "system/dma/sys_dma.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Driver USART Module Index

  Summary:
    USART driver index definitions

  Description:
    These constants provide USART driver index definitions.

  Remarks:
    These constants should be used in place of hard-coded numeric literals.
    These values should be passed into the DRV_USART_Initialize and
    DRV_USART_Open routines to identify the driver instance in use.
*/

#define DRV_USART_INDEX_0             0
#define DRV_USART_INDEX_1             1
#define DRV_USART_INDEX_2             2
#define DRV_USART_INDEX_3             3
#define DRV_USART_INDEX_4             4
#define DRV_USART_INDEX_5             5

// *****************************************************************************
/* USART Driver Module Count

  Summary:
    Number of valid USART drivers

  Description:
    This constant identifies the maximum number of USART Driver instances that
    should be defined in the system. Defining more instances than this
    constant will waste RAM memory space.

    This constant can also be used by the system and application to identify the
    number of USART instances on this microcontroller.

  Remarks:
    This value is part-specific.
*/

#define DRV_USART_COUNT  /*DOM-IGNORE-BEGIN*/ USART_NUMBER_OF_MODULES/*DOM-IGNORE-END*/

// *****************************************************************************
/* USART Driver Write Error

  Summary:
    USART Driver Write Error.

  Description:
    This constant is returned by DRV_USART_Write() function when an error
    occurs.

  Remarks:
    None.
*/

#define DRV_USART_WRITE_ERROR   /*DOM-IGNORE-BEGIN*/((uint32_t)(-1))/*DOM-IGNORE-END*/

// *****************************************************************************
/* USART Driver Read Error

  Summary:
    USART Driver Read Error.

  Description:
    This constant is returned by DRV_USART_Read() function when an error
    occurs.

  Remarks:
    None.
*/

#define DRV_USART_READ_ERROR   /*DOM-IGNORE-BEGIN*/((uint32_t)(-1))/*DOM-IGNORE-END*/

// *****************************************************************************
/* USART Driver Buffer Handle

  Summary:
    Handle identifying a read or write buffer passed to the driver.

  Description:
    A buffer handle value is returned by a call to the DRV_USART_BufferAddRead
    or DRV_USART_BufferAddWrite functions. This handle is associated with the
    buffer passed into the function and it allows the application to track the
    completion of the data from (or into) that buffer.  The buffer handle value
    returned from the "buffer add" function is returned back to the client
    by the "event handler callback" function registered with the driver.

    The buffer handle assigned to a client request expires when the client has
    been notified of the completion of the buffer transfer (after event handler
    function that notifies the client returns) or after the buffer has been
    retired by the driver if no event handler callback was set.

  Remarks:
    None
*/

typedef uintptr_t DRV_USART_BUFFER_HANDLE;

// *****************************************************************************
/* USART Driver Invalid Buffer Handle

  Summary:
    Definition of an invalid buffer handle.

  Description:
    This is the definition of an invalid buffer handle. An invalid buffer handle
    is returned by DRV_USART_BufferAddRead and DRV_USART_BufferAddWrite
    functions if the buffer add request was not successful.

  Remarks:
    None
*/

#define DRV_USART_BUFFER_HANDLE_INVALID /*DOM-IGNORE-BEGIN*/((DRV_USART_BUFFER_HANDLE)(-1))/*DOM-IGNORE-END*/

// *****************************************************************************
/* USART Modes of Operation

  Summary:
    Identifies the modes of the operation of the USART module

  Description:
    This data type identifies the modes of the operation of the USART module.

  Remarks:
    Not all modes are available on all devices.  Refer to the specific device data
    sheet to determine availability.
*/

typedef enum
{
    /* USART works in IRDA mode */
    DRV_USART_OPERATION_MODE_IRDA,

    /* This is the normal point to point communication mode where the USART
    communicates directly with another USART by connecting it's Transmit signal
    to the external USART's Receiver signal and vice versa. An external
    transceiver may be connected to obtain RS-232 signal levels. This type of
    connection is typically full duplex.  */
    DRV_USART_OPERATION_MODE_NORMAL,

    /* This is a multi-point bus mode where the USART can communicate with
    many other USARTS on a bus using an address-based protocol such as RS-485.
    This mode is typically half duplex and the physical layer may require a
    transceiver. In this mode every USART on the bus is assigned an address and
    the number of data bits is 9 bits */
    DRV_USART_OPERATION_MODE_ADDRESSED,

    /* Loopback mode internally connects the Transmit signal to the Receiver
     signal, looping data transmission back into this USART's own input. It is
     useful primarily as a test mode. */
    DRV_USART_OPERATION_MODE_LOOPBACK

} DRV_USART_OPERATION_MODE;


// *****************************************************************************
/* USART Driver Buffer Events

   Summary
    Identifies the possible events that can result from a buffer add request.

   Description
    This enumeration identifies the possible events that can result from a
    buffer add request caused by the client calling either the
    DRV_USART_BufferAddRead or DRV_USART_BufferAddWrite functions.

   Remarks:
    One of these values is passed in the "event" parameter of the event
    handling callback function that the client registered with the driver by
    calling the DRV_USART_BufferEventHandlerSet function when a buffer
    transfer request is completed.

*/

typedef enum
{
    /* All data from or to the buffer was transferred successfully. */
    DRV_USART_BUFFER_EVENT_COMPLETE,

    /* There was an error while processing the buffer transfer request. */
    DRV_USART_BUFFER_EVENT_ERROR,

    /* Data transfer aborted (Applicable in DMA mode) */
    DRV_USART_BUFFER_EVENT_ABORT

} DRV_USART_BUFFER_EVENT;


// *****************************************************************************
/* USART Driver Buffer Result

   Summary
    Identifies the possible result of the buffer processing.

   Description
    This enumeration identifies the possible result of the buffer processing.
    
   Remarks:
    DRV_USART_BUFFER_RESULT_HANDLE_EXPIRED is the state of the buffer which 
    is in the free buffer pool.

*/

typedef enum
{
    /* Buffer handle is not valid*/
    DRV_USART_BUFFER_RESULT_HANDLE_INVALID,

    /* Buffer handle has expired. */
    DRV_USART_BUFFER_RESULT_HANDLE_EXPIRED,

    /* Buffer is removed from the queue succesfully*/
    DRV_USART_BUFFER_RESULT_REMOVED_SUCCESFULLY,

    /* Buffer removal failed because of unable to acquire the mutex
     * This is applicable in RTOS mode only */
    DRV_USART_BUFFER_RESULT_REMOVAL_FAILED

} DRV_USART_BUFFER_RESULT;


// *****************************************************************************
/* USART Driver Buffer Event Handler Function Pointer

   Summary
    Pointer to a USART Driver Buffer Event handler function

   Description
    This data type defines the required function signature for the USART driver
    buffer event handling callback function. A client must register a pointer
    to a buffer event handling function whose function signature (parameter
    and return value types) match the types specified by this function pointer
    in order to receive buffer related event calls back from the driver.

    The parameters and return values and are described here and
    a partial example implementation is provided.

  Parameters:
    event           - Identifies the type of event

    bufferHandle    - Handle identifying the buffer to which the vent relates

    context         - Value identifying the context of the application that registered
                      the event handling function.

  Returns:
    None.

  Example:
    <code>
    void APP_MyBufferEventHandler( DRV_USART_BUFFER_EVENT event,
                                   DRV_USART_BUFFER_HANDLE bufferHandle,
                                   uintptr_t context )
    {
        MY_APP_DATA_STRUCT pAppData = (MY_APP_DATA_STRUCT) context;

        switch(event)
        {
            case DRV_USART_BUFFER_EVENT_COMPLETE:

                // Handle the completed buffer.
                break;

            case DRV_USART_BUFFER_EVENT_ERROR:
            default:

                // Handle error.
                break;
        }
    }
    </code>

  Remarks:
    If the event is DRV_USART_BUFFER_EVENT_COMPLETE, it means that the data was
    transferred successfully.

    If the event is DRV_USART_BUFFER_EVENT_ERROR, it means that the data was not
    transferred successfully. The DRV_USART_ErrorGet function can be called to
    know the error. The DRV_USART_BufferCompletedBytesGet function can be
    called to find out how many bytes were processed.

    The bufferHandle parameter contains the buffer handle of the buffer that
    associated with the event.

    The context parameter contains the a handle to the client context,
    provided at the time the event handling function was registered using the
    DRV_USART_BufferEventHandlerSet function.  This context handle value is
    passed back to the client as the "context" parameter.  It can be any value
    necessary to identify the client context or instance (such as a pointer to
    the client's data) instance of the client that made the buffer add request.

    The event handler function executes in the driver peripheral's interrupt
    context when the driver is configured for interrupt mode operation. It is
    recommended of the application to not perform process intensive or blocking
    operations with in this function.

    The DRV_USART_BufferAddRead and DRV_USART_BufferAddWrite functions can
    be called in the event handler to add a buffer to the driver queue. These
    functions can only be called to add buffers to the driver whose event
    handler is running. For example, buffers cannot be added USART2 driver in
    USART1 driver event handler.
*/

typedef void ( *DRV_USART_BUFFER_EVENT_HANDLER )
(
    DRV_USART_BUFFER_EVENT event,
    DRV_USART_BUFFER_HANDLE bufferHandle,
    uintptr_t context
);

// *****************************************************************************
/* USART Driver Byte Event Handler Function Pointer

   Summary
    Pointer to a USART Driver Byte Event handler function

   Description
   This data type defines the required function signature for the USART driver
   byte event handling callback function. A client must register a pointer to a
   byte event handling function whose function signature (parameter  and return
   value types) match the types specified by this function pointer in order to
   receive byte related event calls back from the driver.

  Parameters:
    index  - Identifier for the instance

  Returns:
    None.

  Example:
    <code>
    void APP_MyUsartTxEventHandler(void)
    {
        // Handle the transmit byte event
    }
    </code>

  Remarks:
    The event handler function executes in the driver peripheral's interrupt
    context when the driver is configured for interrupt mode operation. It is
    recommended that the application not perform process intensive or blocking
    operations with in this function.
*/

typedef void (* DRV_USART_BYTE_EVENT_HANDLER)
(
    const SYS_MODULE_INDEX index
);

// *****************************************************************************
/* USART Handshake Modes

  Summary:
    Identifies the handshaking modes supported by the USART driver.

  Description:
    This data type identifies the handshaking modes supported by the USART
    driver.

  Remarks:
    Not all modes are available on all devices.  Refer to the specific device data
    sheet to determine availability.
*/

typedef enum
{
    /* Handshaking occurs in Flow Control Mode */
    DRV_USART_HANDSHAKE_FLOWCONTROL = /*DOM-IGNORE-BEGIN*/ USART_HANDSHAKE_MODE_FLOW_CONTROL /*DOM-IGNORE-END*/,

    /* Handshaking occurs in Simplex Mode */
    DRV_USART_HANDSHAKE_SIMPLEX = /*DOM-IGNORE-BEGIN*/USART_HANDSHAKE_MODE_SIMPLEX/*DOM-IGNORE-END*/,

    /* No Handshaking */
    DRV_USART_HANDSHAKE_NONE = /*DOM-IGNORE-BEGIN*/ 2 /*DOM-IGNORE-END*/

} DRV_USART_HANDSHAKE;


// *****************************************************************************
/* USART Baud Set Result

  Summary:
    Identifies the results of the baud set function.

  Description:
    This data type identifies the results of the DRV_USART_BaudSet function.

  Remarks:
    None.
*/

typedef enum
{
    /* The driver was not able to change the baud */
    DRV_USART_BAUD_SET_ERROR = /*DOM-IGNORE-BEGIN*/ -1 /*DOM-IGNORE-END*/,

    /* The driver was able to change the baud successfully */
    DRV_USART_BAUD_SET_SUCCESS

} DRV_USART_BAUD_SET_RESULT;


// *****************************************************************************
/* USART Line Control Set Result

  Summary:
    Identifies the results of the baud set function.

  Description:
    This data type identifies the results of the DRV_USART_LineControlSet
    function.

  Remarks:
    None.
*/

typedef enum
{
    /* The driver was not able to change the Line Control */
    DRV_USART_LINE_CONTROL_SET_ERROR = /*DOM-IGNORE-BEGIN*/ -1 /*DOM-IGNORE-END*/,

    /* The driver was able to change the Line Control successfully */
    DRV_USART_LINE_CONTROL_SET_SUCCESS

} DRV_USART_LINE_CONTROL_SET_RESULT;


// *****************************************************************************
/* USART Line Control Modes

  Summary:
    Identifies the line control modes supported by the USART driver.

  Description:
    This data type identifies the line control modes supported by the USART
    driver. Line control modes define the number of data bits, parity mode, and
    the number of stop bits in a USART transmit and receive frames.

  Remarks:
    The abbreviations used in the labels for the values of this enumeration
    follow the format <data><parity><stop>, where:

    <data>      is the number of data bits

    <parity>    is either "NONE" (for no parity), "EVEN" for 1 parity bit
                added to obtain an even number of bits, or "ODD" for one bit
                added to obtain an odd number of bits.

    <stop>      is the number of Stop bits
*/

typedef enum
{
    /* 8 data bits, no parity bit, 1 stop bit */
    DRV_USART_LINE_CONTROL_8NONE1 = /*DOM-IGNORE-BEGIN*/ USART_8N1  /* DOM-IGNORE-END*/,

    /* 9 data bits, no parity bit, 1 stop bit */
    DRV_USART_LINE_CONTROL_9NONE1 = /*DOM-IGNORE-BEGIN*/ USART_9N1 /* DOM-IGNORE-END*/,

    /* 8 data bits, 1 bit for even parity, 1 stop bit */
    DRV_USART_LINE_CONTROL_8EVEN1 = /*DOM-IGNORE-BEGIN*/ USART_8E1 /* DOM-IGNORE-END*/,

    /* 8 data bits, 1 bit for even parity, 2 stop bits */
    DRV_USART_LINE_CONTROL_8EVEN2 = /*DOM-IGNORE-BEGIN*/ USART_8E2 /* DOM-IGNORE-END*/,

    /* 8 data bits, 1 bit for odd parity, 1 stop bit */
    DRV_USART_LINE_CONTROL_8ODD1 =  /*DOM-IGNORE-BEGIN*/ USART_8O1 /* DOM-IGNORE-END*/,

    /* 8 data bits, 1 bit for odd parity, 2 stop bits */
    DRV_USART_LINE_CONTROL_8ODD2 =  /*DOM-IGNORE-BEGIN*/ USART_8O2 /* DOM-IGNORE-END*/,

    /* 8 data bits, no parity bit, 2 stop bit */
    DRV_USART_LINE_CONTROL_8NONE2 = /*DOM-IGNORE-BEGIN*/ USART_8N2 /* DOM-IGNORE-END*/,

    /* 9 data bits, no parity bit, 2 stop bit */
    DRV_USART_LINE_CONTROL_9NONE2 = /*DOM-IGNORE-BEGIN*/ USART_9N2 /* DOM-IGNORE-END*/

} DRV_USART_LINE_CONTROL;


// *****************************************************************************
/* USART Initialization flags

  Summary:
    Flags identifying features that can be enabled when the driver is
    initialized.

  Description:
    This enumeration defines flags identifying features that can be enabled
    when the driver is initialized.

  Remarks:
    These flags can be logically ORed together.  They are passed into the
    DRV_USART_Initialize function through the "flags" member of the
    DRV_USART_INIT structure.
*/

typedef enum
{
    /* Use this if no flags need to be set */
    DRV_USART_INIT_FLAG_NONE = /* DOM-IGNORE-BEGIN */0 /*DOM-IGNORE-END*/,

    /* Flag to enable "wake on start" operation.  If supported and enabled,
       this feature will allow the USART to wake-up the device when a
       Start bit is received. This option should be selected only when the
       device is to placed in Sleep mode. Note that enabling this bit will
       also cause the first received character to be lost. Refer to the specific
       device data sheet for more information. */
    DRV_USART_INIT_FLAG_WAKE_ON_START    /*DOM-IGNORE-BEGIN*/ = (1 << 0)  /*DOM-IGNORE-END*/,

    /* Flag to enable auto baud detection. If supported and enabled, this
       feature will allow the USART to automatically detect the baud rate in
       use.  */
    DRV_USART_INIT_FLAG_AUTO_BAUD       /*DOM-IGNORE-BEGIN*/ = (1 << 1)  /*DOM-IGNORE-END*/,

    /* Flag to enable stop in idle. If supported and enabled , this
       feature will allow the USART to stop when the CPU enters Idle
       mode */
    DRV_USART_INIT_FLAG_STOP_IN_IDLE         /* DOM-IGNORE-BEGIN*/ = (1 << 2) /*DOM-IGNORE-END*/


} DRV_USART_INIT_FLAGS;


// *****************************************************************************
/* Operation Mode Initialization Data

  Summary:
    Defines the initialization data required for different operation modes of
    USART.

  Description:
    This data type defines the initialization data required for different
    operation modes of the USART.

  Remarks:
    None
*/

typedef union
{
    /* Initialization for Addressed mode */
    struct
    {
        /* Address of the device. */
        uint8_t address;

    }AddressedModeInit;

} DRV_USART_OPERATION_MODE_DATA;


// *****************************************************************************
/* USART Driver Errors.

  Summary:
    Defines the possible errors that can occur during driver operation.

  Description:
    This data type defines the possible errors that can occur when occur during
    USART driver operation. These values are returned by DRV_USART_ErrorGet
    function.

  Remarks:
    None
*/

typedef enum
{
    /* There was no error */
    DRV_USART_ERROR_NONE
            /*DOM-IGNORE-BEGIN*/ = USART_ERROR_NONE /*DOM-IGNORE-END*/,

    /* This indicates that a parity error has occurred */
    DRV_USART_ERROR_PARITY
            /*DOM-IGNORE-BEGIN*/ = USART_ERROR_PARITY /*DOM-IGNORE-END*/,

    /* This indicates that a framing error has occurred */
    DRV_USART_ERROR_FRAMING
            /*DOM-IGNORE-BEGIN*/ = USART_ERROR_FRAMING /*DOM-IGNORE-END*/,

    /* This indicates a receiver overflow has occurred */
    DRV_USART_ERROR_RECEIVE_OVERRUN
            /*DOM-IGNORE-BEGIN*/ = USART_ERROR_RECEIVER_OVERRUN /*DOM-IGNORE-END*/,

   /* Channel address error (Applicable in DMA mode) */
    DRV_USART_ERROR_ADDRESS /*DOM-IGNORE-BEGIN*/ = (1 << 4) /* DOM-IGNORE-END*/

} DRV_USART_ERROR;


// *****************************************************************************
/* USART Client-Specific Driver Status

  Summary:
    Defines the client-specific status of the USART driver.

  Description:
    This enumeration defines the client-specific status codes of the USART
    driver.

  Remarks:
    Returned by the DRV_USART_ClientStatus function.
*/

typedef enum
{
    /* An error has occurred.*/
    DRV_USART_CLIENT_STATUS_ERROR    = DRV_CLIENT_STATUS_ERROR,

    /* The driver is closed, no operations for this client are ongoing,
    and/or the given handle is invalid. */
    DRV_USART_CLIENT_STATUS_CLOSED   = DRV_CLIENT_STATUS_CLOSED,

    /* The driver is currently busy and cannot start additional operations. */
    DRV_USART_CLIENT_STATUS_BUSY     = DRV_CLIENT_STATUS_BUSY,

    /* The module is running and ready for additional operations */
    DRV_USART_CLIENT_STATUS_READY    = DRV_CLIENT_STATUS_READY

} DRV_USART_CLIENT_STATUS;


// *****************************************************************************
/* USART Driver Transfer Flags

  Summary
    Specifies the status of the receive or transmit

  Description
    This type specifies the status of the receive or transmit operation.

  Remarks:
    More than one of these values may be OR'd together to create a complete
    status value.  To test a value of this type, the bit of interest must be
    ANDed with the value and checked to see if the result is non-zero.
*/

typedef enum
{

    /* Indicates that at least one byte of Data has been received */
    DRV_USART_TRANSFER_STATUS_RECEIVER_DATA_PRESENT
        /*DOM-IGNORE-BEGIN*/  = (1 << 0) /*DOM-IGNORE-END*/,

    /* Indicates that the core driver receiver buffer is empty */
    DRV_USART_TRANSFER_STATUS_RECEIVER_EMPTY
        /*DOM-IGNORE-BEGIN*/  = (1 << 1) /*DOM-IGNORE-END*/,

    /* Indicates that the core driver transmitter buffer is full */
    DRV_USART_TRANSFER_STATUS_TRANSMIT_FULL
        /*DOM-IGNORE-BEGIN*/  = (1 << 2) /*DOM-IGNORE-END*/,

    /* Indicates that the core driver transmitter buffer is empty */
    DRV_USART_TRANSFER_STATUS_TRANSMIT_EMPTY
        /*DOM-IGNORE-BEGIN*/  = (1 << 3) /*DOM-IGNORE-END*/

} DRV_USART_TRANSFER_STATUS;


// *****************************************************************************
/* USART Driver Initialization Data

  Summary:
    Defines the data required to initialize or reinitialize the USART driver

  Description:
    This data type defines the data required to initialize or reinitialize the
    USART driver. If the driver is built statically, the members of this data
    structure are statically over-ridden by static override definitions in the
    system_config.h file.

  Remarks:
    None.
*/

typedef struct
{
    /* System module initialization data */
    SYS_MODULE_INIT moduleInit;

    /* Identifies USART hardware module (PLIB-level) ID. For a static build of
       the driver, this is overridden by DRV_USART_MODULE_ID macro in the
       system_config.h header file. */
    USART_MODULE_ID usartID;

    /* Identifies the Operation mode of the USART driver. For a static build of
       the driver, this is overridden by DRV_USART_MODE_SELECT macro in the
       system_config.h header file. */
    DRV_USART_OPERATION_MODE mode;

    /* Data required by the operation mode of the driver. For a static build of
       the driver, this is overridden by DRV_USART_MODE_DATA macro in the
       system_config.h header file. */
    DRV_USART_OPERATION_MODE_DATA modeData;

    /* Flags to enable specific features. Refer to the
       description of DRV_USART_INIT_FLAGS for more details. For a static build
       of the driver, this is overridden by DRV_USART_FLAGS macro in the
       system_config.h header file. */
    DRV_USART_INIT_FLAGS flags;

    /* USART module Baud Rate Generator Clock. This typically
       the peripheral bus clock frequency. For a static build of the driver,
       this is overridden by DRV_USART_BRG_CLOCK macro in the system_config.h
       header file. */
    uint32_t brgClock;

    /* The initial USART line control settings. For a static build of the driver
       this is overridden by the DRV_USART_LINE_CONTROL_SET macro in the
       system_config.h header file. */
    DRV_USART_LINE_CONTROL lineControl;

    /* Baud Rate value to be used, if not using auto baud. For a static build of
       the driver, this is overridden by the DRV_USART_BAUD macro in the
       system_config.h header file. */
    uint32_t baud;

    /* Handshaking mode. For a static build of the driver, this is overridden by
       the DRV_USART_HANDSHAKE_SET macro in the system_config.h header file. */
    DRV_USART_HANDSHAKE handshake;

    /* Interrupt source ID for the transmitter interrupt. For a static build of
       the driver, this is overridden by the DRV_USART_TRANSMIT_INTERRUPT_SOURCE
       macro in the system_config.h header. */
    INT_SOURCE interruptTransmit;

    /* Interrupt source ID for the receiver interrupt. For a static build of
       the driver, this is overridden by the DRV_USART_RECEIVE_INTERRUPT_SOURCE
       macro in the system_config.h header. */
    INT_SOURCE interruptReceive;

    /* Interrupt source ID for the error Interrupt. For a static build of the
       driver, this is overridden by the DRV_USART_ERROR_INTERRUPT_SOURCE macro
       in the system_config.h header. */
    INT_SOURCE interruptError;

    /* This is the receive buffer queue size. This is the maximum
       number of read requests that driver will queue. For a
       static build of the driver, this is overridden by the
       DRV_USART_RECEIVE_QUEUE_SIZE macro in system_config.h */
    unsigned int queueSizeReceive;

    /* This is the transmit buffer queue size. This is the maximum
       number of write requests that driver will queue. For a
       static build of the driver, this is overridden by the
       DRV_USART_TRANSMIT_QUEUE_SIZE macro in system_config.h */
    unsigned int queueSizeTransmit;

    /* This is the USART transmit DMA channel.
       For a static build of the driver, this is overridden by the
       DRV_USART_XMIT_DMA_CH_IDXn macro in system_config.h */
    DMA_CHANNEL dmaChannelTransmit;

    /* This is the USART receive DMA channel.
       For a static build of the driver, this is overridden by the
       DRV_USART_RCV_DMA_CH_IDXn macro in system_config.h */
    DMA_CHANNEL dmaChannelReceive;

    /* This is the USART transmit DMA channel interrupt. 
       For a static build of the driver, this is overridden by the 
       DRV_USART_XMIT_DMA_INT_SRC_IDXn macro in system_config.h */
    INT_SOURCE dmaInterruptTransmit;

    /* This is the USART receive DMA channel interrupt. 
       For a static build of the driver, this is overridden by the 
       DRV_USART_RCV_DMA_INT_SRC_IDXn macro in system_config.h */
    INT_SOURCE dmaInterruptReceive;

} DRV_USART_INIT;

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END


#endif // #ifndef _DRV_USART_DEFINITIONS_H
/*******************************************************************************
 End of File
*/

