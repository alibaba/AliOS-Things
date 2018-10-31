/*******************************************************************************
 USART Driver Configuration Template

  Company:
    Microchip Technology Inc.

  File Name:
    drv_usart_config_template.h

  Summary:
    USART Driver Configuration Template.

  Description:
    These file provides the list of all the configurations that can be used with
    the driver. This file should not be included in the driver.

*******************************************************************************/
//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2012-2016 released Microchip Technology Inc.  All rights reserved.

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

#ifndef _DRV_USART_CONFIG_TEMPLATE_H
#define _DRV_USART_CONFIG_TEMPLATE_H

//DOM-IGNORE-BEGIN
#error "This is a configuration template file.  Do not include it directly."
//DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Core Functionality Configuration Options
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* USART driver objects configuration

  Summary
    Sets up the maximum number of hardware instances that can be supported.

  Description
    This macro sets up the maximum number of hardware instances that can be 
    supported.
    It is recommended that this number be set exactly equal to the number of
    USART modules that are needed by the application, as hardware Instance
    support consumes RAM memory space. 
    If this macro is not defined, the driver will be built statically.

  Remarks:
    None
*/

#define DRV_USART_INSTANCES_NUMBER  2

// *****************************************************************************
/* USART Client Count Configuration

  Summary
    Sets up the maximum number of clients that can be connected to any hardware
    instance.

  Description
    This macro sets up the maximum number of clients that can be connected to any 
    hardware instance. This value represents the total number of clients to be 
    supported across all hardware instances. Therefore, if USART1 will be accessed 
    by two clients and USART2 will accessed by three clients, this number should be 5. 
    It is recommended that this value be set exactly equal to the number of expected
    clients, as client support consumes RAM memory space.
    If this macro is not defined and the DRV_USART_INSTANCES_NUMBER macro is not
    defined, the driver will be built for static - single client operation.
    If this macro is defined and the DRV_USART_INSTANCES_NUMBER macro is not
    defined, the driver will be built for static - multi client operation.

  Remarks:
    None.
*/

#define DRV_USART_CLIENTS_NUMBER   4

// *****************************************************************************
/* USART Interrupt Mode Operation Control

  Summary:
    Macro controls interrupt based operation of the driver.

  Description:
    This macro controls the interrupt based operation of the driver. The 
    possible values are:
    - true  - Enables the interrupt mode
    - false - Enables the polling mode
    If the macro value is true, the Interrupt Service Routine (ISR) for the interrupt
    should be defined in the system. The DRV_USART_Tasks routine should
    be called in the ISR. While using the USART driver with DMA, this flag
    should always be true.

  Remarks:
    None.
*/

#define DRV_USART_INTERRUPT_MODE  true

// *****************************************************************************
/* USART Driver Instance combined queue depth.

  Summary:
    Defines the number of entries of all queues in all instances of the driver.
  
  Description:
    This macro defines the number of entries of all queues in all instances of
    the driver.
    
    Each hardware instance supports a buffer queue for transmit and receive
    operations. The size of queue is specified either in driver
    initialization (for dynamic build) or by macros (for static build). The
    hardware instance transmit buffer queue will queue transmit buffers
    submitted by the DRV_USART_BufferAddWrite function.  The hardware instance
    receive buffer queue will queue receive buffers submitted by the
    DRV_USART_BufferAddRead function. 
    
    A buffer queue will contain buffer queue entries, with each related to a
    BufferAdd request. This configuration macro defines the total number of buffer
    entries that will be available for use between all USART driver hardware
    instances. The buffer queue entries are allocated to individual hardware
    instances as requested by hardware instances. Once the request is processed,
    the buffer queue entry is free for use by other hardware instances.

    The total number of buffer entries in the system determines the ability of
    the driver to service non blocking read and write requests. If a free buffer
    entry is not available, the driver will not add the request and will return
    an invalid buffer handle. The greater the number of buffer entries, the greater 
    the ability of the driver to service and add requests to its queue. A hardware 
    instance additionally can queue up as many buffer entries as specified by
    its transmit and receive buffer queue size. 

    For example, consider the case of static single client driver application
    where full duplex non blocking operation is desired without queuing, the
    minimum transmit queue depth and minimum receive queue depth should be 1.
    Therefore, the total number of buffer entries should be 2.
   
    As another example, consider the case of a dynamic driver (i.e., two instances) 
    where instance 1 will queue up to three write requests and up to two read 
    requests, and instance 2 will queue up to two write requests and up to six read requests, the value of this macro should be: 13 (2 + 3 + 2 + 6).

  Remarks:
    None.
*/

#define  DRV_USART_QUEUE_DEPTH_COMBINED   16

// *****************************************************************************
/* USART Driver Buffer Queue Support 

  Summary:
    Specifies if the Buffer Queue support should be enabled. 
  
  Description:
    This macro defines whether or not Buffer Queue support should be enabled. 
    Setting this macro to true will enable buffer queue support and all buffer 
    related driver function. The driver should be built along with the drv_usart_buffer_queue.c file, which contains the functional implementation 
    for buffer queues. If buffer queue operation is enabled, the DRV_USART_BYTE_MODEL_SUPPORT function should
    not be true. If this macro is set to false, the behavior of the USART Driver
    Buffer Queue API is not defined. While using the USART driver with DMA, the
    driver supports Buffer Queue Data transfer model regardless of the value
    of this configuration macro.

  Remarks:
    None.
*/

#define DRV_USART_BUFFER_QUEUE_SUPPORT  true
    
// *****************************************************************************
/* USART Driver Byte Model Support

  Summary:
    Specifies if the Byte Model support should be enabled. 
  
  Description:
    This macro defines whether or Byte Model support should be enabled. Setting this
    macro to true will enable byte model support and all byte operation related
    driver functions. The driver should be built along with the
    drv_usart_byte_model.c file, which contains the functional implementation for
    byte model operation.  If byte model operation is enabled, the driver will
    not support buffer queue and read write models. The behavior of the byte
    mode API when this macro is set to false is not defined.

  Remarks:
    None.
*/

#define DRV_USART_BYTE_MODEL_SUPPORT  false

// *****************************************************************************
/* USART Driver Byte Write Blocking Behavior

  Summary:
    Enables or Disables DRV_USART_ByteWrite function blocking behavior.
  
  Description:
    This USART Driver MHC option controls the blocking behavior of the
    DRV_USART_ByteWrite function and is only applicable when the USART Driver
    Byte Transfer model is selected. Selecting this option will cause the 
    DRV_USART_ByteWrite function to block until the byte has been written to 
    the USART Transmit FIFO. Blocking behavior is enabled by default (to enable 
    backward compatibility with previous versions of the driver).
    This option can be used for simple applications where interoperability with 
    other MPLAB Harmony modules is not a design concern.

    If the application uses several other MPLAB Harmony modules (Middleware, 
    File System, etc.), it is recommended to disable this option and use 
    the non-blocking DRV_USART_ByteWrite function. This requires the application 
    to call the DRV_USART_TransmitBufferIsFull function to check if the byte 
    can be written to the USART, as shown in the following code example.
    <code>
     if(!DRV_USART_TransmitBufferIsFull(usartHandle1))
     {
         byte = '1';
         DRV_USART_WriteByte(usartHandle1,byte);
     }
    </code>
   
    Using the non-blocking implementation results in improved application 
    interoperability with other MPLAB Harmony modules.

  Remarks:
    The DRV_USART_BYTE_MODEL_BLOCKING constant is specified for documentation 
    purposes only. It does not affect the configuration of the driver.
*/

#define DRV_USART_BYTE_MODEL_BLOCKING

// *****************************************************************************
/* USART Driver Byte Model Callback Feature.

  Summary:
    Enables or Disables Callback Feature of the Byte Transfer Model.
  
  Description:
    This USART Driver MHC option controls the Callback feature of the
    Byte Transfer model. Selecting this option allows an application to register 
    Byte Transfer Event Callback functions with the driver. These callback 
    functions are invoked on the occurrence of Byte Transfer events. 
    Callback functions can be registered to Byte Transmit, Byte Receive, and 
    USART Error events, as shown in the following code example.
    <code>
     // This code shows how a callback function is 
     // registered for the Byte Receive event.
     DRV_USART_ByteReceiveCallbackSet(DRV_USART_INDEX_0, APP_USARTReceiveEventHandler);

     // Event Processing Technique. Event is received when
     // a byte is received.

     void APP_USARTReceiveEventHandler(const SYS_MODULE_INDEX index)
     {
         // Byte has been Received. Handle the event.
         // Read byte using DRV_USART_ReadByte.
     }
    </code>

    When operating in Interrupt mode, the callback functions are invoked 
    in an interrupt context.
    If this option is not selected, the application must use the 
    DRV_USART_TransmitBufferIsFull, DRV_USART_ReceiverBufferIsEmpty, and 
    DRV_USART_ErrorGet functions to check the status of Byte transmit or receive.

  Remarks:
    The DRV_USART_BYTE_MODEL_CALLBACK constant is specified for documentation 
    purposes only. It does not affect the configuration of the driver.
*/

#define DRV_USART_BYTE_MODEL_CALLBACK

// *****************************************************************************
/* USART Driver Read Write Model Support

  Summary:
    Specifies if Read/Write Model support should be enabled. 
  
  Description:
    This macro defines whether or not Read Write Model support should be enabled. 
    Setting this macro to true will enable read write model support and all read/write
    related driver functions. The driver should be built along with the
    drv_usart_read_write.c file, which contains the functional implementation for
    byte model operation.  If read/write model operation is enabled, the
    DRV_USART_BYTE_MODEL_SUPPORT macro should not be true. The behavior of the Read
    Write Model API when this macro is set to false is not defined. 

  Remarks:
    None.
*/

#define DRV_USART_READ_WRITE_MODEL_SUPPORT  true

// *****************************************************************************
// *****************************************************************************
// Section: Static Overrides
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Index - Used for static drivers

  Summary:
    USART Static Index selection.

  Description:
    USART Static Index selection for the driver object reference. This macro
    defines the driver index for static and static multi-client builds. 
    For example, if this macro is set to DRV_USART_INDEX_2, the 
    static driver APIs would be DRV_USART2_Initialize, DRV_USART2_Open, etc.
    When building static drivers, this macro should be different for
    each static build of the USART driver that needs to be included in the
    project.

  Remarks:
    This index is required to make a reference to the driver object
*/

#define DRV_USART_INDEX		DRV_USART_INDEX_2

// *****************************************************************************
/* USART Peripheral Library Module ID

  Summary:
    Configures the USART PLIB Module ID. 

  Description:
    This macro configures the PLIB ID if the driver is built statically. This 
    value will override the usartID member of the DRV_USART_INIT initialization
    data structure. In that when the driver is built statically, the 
    usartID member of the DRV_USART_INIT data structure will be ignored by the
    driver initialization routine and this macro will be considered. This should
    be set to the PLIB ID of USART module (USART_ID_1, USART_ID_2, and so on). 

  Remarks:
    None.
*/

#define DRV_USART_PERIPHERAL_ID   USART_ID_2

// *****************************************************************************
/* USART Driver Baud Selection.

  Summary:
    Specifies the USART Baud at which the USART driver is initialized.
  
  Description:
    This configuration constant specifies the baud rate at which the USART 
    Driver is initialized. This is the baud rate at which the USART module will 
    operate when the driver initialization has completed. 
    The driver client can call the DRV_USART_BaudSet function after opening the 
    driver to change the USART baud rate after initialization has completed.

  Remarks:
    This constant is automatically generated by MHC and its value is set 
    to the value specified in USART Driver Baud Selection field.
*/

#define DRV_USART_BAUD_RATE_IDXn

// *****************************************************************************
/* USART Driver Transmit Queue Size Selection.

  Summary:
    Sets the USART Driver Transmit Queue Size while using the Buffer Queue 
    Data Transfer Model.

  Description:
    This constant sets the USART Driver Transmit queue size when using the 
    Buffer Queue Data Transfer Model. It affects the queuing capacity of the
    DRV_USART_BufferAddWrite function, for the selected driver instance. 
    For example, if this option is set to 5 for USART Driver 0, 
    USART Driver 0 can then queue up to a maximum of five driver client 
    transmit buffer requests from any driver clients. 

    Therefore if USART Driver 0 has two clients and if client 1 has queued up
    three buffers for transmit, client 2 can only queue up to two buffers.
    If the client attempts to queue up more buffers, DRV_USART_BufferAddWrite 
    will not accept the request and will generate an invalid buffer handle
    (DRV_USART_BUFFER_HANDLE_INVALID).

  Remarks:
    This constant is automatically generated by MHC and its value is set 
    to the value specified in USART Driver Transmit Queue Size field. 
*/

#define DRV_USART_XMIT_QUEUE_SIZE_IDXn

// *****************************************************************************
/* USART Driver Receive Queue Size Selection.

  Summary:
    Sets the USART Driver Receive Queue Size while using the Buffer Queue 
    Data Transfer Model.

  Description:
    This constant sets the USART Driver Receive queue size when using the 
    Buffer Queue Data Transfer Model. It affects the queuing capacity of the
    DRV_USART_BufferAddRead function for the selected driver instance. 
    For example, if this option is set to 5 for USART Driver 0, 
    USART Driver 0 can then queue up to a maximum of five driver client 
    receive buffer requests from any driver clients. 

    Therefore, if USART Driver 0 has two clients and if client 1 has queued up 
    three buffers for receive, client 2 can only queue up to two buffers.
    If the client attempts to queue up more buffers, DRV_USART_BufferAddRead 
    will not accept the request and will generate an invalid buffer handle
    (DRV_USART_BUFFER_HANDLE_INVALID).

  Remarks:
    This constant is automatically generated by MHC and its value is set 
    to the value specified in USART Driver Receive Queue Size field. 
*/

#define DRV_USART_RCV_QUEUE_SIZE_IDXn

// *****************************************************************************
/* Transmit Interrupt Source

  Summary:
    Defines the Transmit interrupt source for the static driver.

  Description:
    This macro defines the TX interrupt source for the static driver. The
    interrupt source defined by this macro will override the txInterruptSource
    member of the DRV_USB_INIT initialization data structure in the driver
    initialization routine. This value should be set to the USART module
    transmit interrupt enumeration in the Interrupt PLIB for the
    microcontroller.

  Remarks:
    None.
*/

#define DRV_USART_INTERRUPT_SOURCE_TRANSMIT	    INT_SOURCE_USART_2_TRANSMIT

// *****************************************************************************
/* Transmit DMA Channel Interrupt Source

  Summary:
    Defines the Transmit DMA Channel interrupt source for the static driver.

  Description:
    This macro defines the TX DMA Channel interrupt source for the static
    driver. The interrupt source defined by this macro will override the
    dmaInterruptTransmit member of the DRV_USB_INIT initialization
    data structure in the driver initialization routine. This value should be
    set to the DMA channel interrupt enumeration in the Interrupt PLIB for the
    microcontroller.

  Remarks:
    None.
*/

#define DRV_USART_INTERRUPT_SOURCE_TRANSMIT_DMA

// *****************************************************************************
/* Receive DMA Channel Interrupt Source

  Summary:
    Defines the Receive DMA Channel interrupt source for the static driver.

  Description:
    This macro defines the Receive DMA Channel interrupt source for the static
    driver. The interrupt source defined by this macro will override the
    dmaInterruptReceive member of the DRV_USB_INIT initialization
    data structure in the driver initialization routine. This value should be
    set to the DMA channel interrupt enumeration in the Interrupt PLIB for the
    microcontroller.

  Remarks:
    None.
*/

#define DRV_USART_INTERRUPT_SOURCE_RECEIVE_DMA

// *****************************************************************************
/* USART Driver Receive DMA Channel 

  Summary:
    Defines the USART Driver Receive DMA Channel for the static driver.

  Description:
    This macro defines the USART Receive DMA Channel for the static driver. The
    DMA channel defined by this macro will override the dmaReceive member
    of the DRV_USB_INIT initialization data structure in the driver
    initialization routine. This value should be set to the DMA channel in the
    DMA PLIB for the microcontroller.

  Remarks:
    None.
*/

#define DRV_USART_RECEIVE_DMA 

// *****************************************************************************
/* USART Driver Transmit DMA Channel 

  Summary:
    Defines the USART Driver Transmit DMA Channel  in case of static driver.

  Description:
    This macro defines the USART Transmit DMA Channel for the static driver. The
    DMA channel defined by this macro will override the dmaTransmit member
    of the DRV_USB_INIT initialization data structure in the driver
    initialization routine. This value should be set to the DMA channel in the
    DMA PLIB for the microcontroller.

  Remarks:
    None.
*/

#define DRV_USART_TRANSMIT_DMA 

// *****************************************************************************
/* Receive Interrupt Source

  Summary:
    Defines the Receive interrupt source for the static driver.

  Description:
    This macro defines the Receive interrupt source for the static driver. The
    interrupt source defined by this macro will override the rxInterruptSource
    member of the DRV_USB_INIT initialization data structure in the driver
    initialization routine. This value should be set to the USART module
    receive interrupt enumeration in the Interrupt PLIB for the
    microcontroller.

  Remarks:
    None.
*/

#define DRV_USART_INTERRUPT_SOURCE_RECEIVE			INT_SOURCE_USART_2_RECEIVE

// *****************************************************************************
/* Error Interrupt Source

  Summary:
    Defines the error interrupt source for the static driver.

  Description:
    This macro defines the Error interrupt source for the static driver. The
    interrupt source defined by this macro will override the errorInterruptSource
    member of the DRV_USB_INIT initialization data structure in the driver
    initialization routine. This value should be set to the USART module error
    interrupt enumeration in the Interrupt PLIB for the microcontroller.

  Remarks:
    None.
*/

#define DRV_USART_INTERRUPT_SOURCE_ERROR		INT_SOURCE_USART_2_ERROR


#endif // #ifndef _DRV_USART_CONFIG_TEMPLATE_H

/*******************************************************************************
 End of File
*/
