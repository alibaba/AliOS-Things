/*******************************************************************************
  USART Driver Interface Header File

  Company:
    Microchip Technology Inc.

  File Name:
    drv_usart.h

  Summary:
    USART Driver Interface Header File

  Description:
    The USART device driver provides a simple interface to manage the USART or
    UART modules on Microchip microcontrollers.  This file provides the
    interface definition for the USART driver.
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

#ifndef _DRV_USART_H
#define _DRV_USART_H

// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************

#if defined(__PIC32C__)
#include "drv_usart_definitions_pic32c.h"
#else
#include "drv_usart_definitions.h"
#endif

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: USART Driver Module Interface Routines
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
     SYS_MODULE_OBJ DRV_USART_Initialize
     (
        const SYS_MODULE_INDEX index,
        const SYS_MODULE_INIT * const init
     )

  Summary:
    Initializes the USART instance for the specified driver index.
    <p><b>Implementation:</b> Static/Dynamic</p>

  Description:
    This routine initializes the USART driver instance for the specified driver
    index, making it ready for clients to open and use it. The initialization
    data is specified by the init parameter. The initialization may fail if the
    number of driver objects allocated are insufficient or if the specified
    driver instance is already initialized. The driver instance index is
    independent of the USART module ID. For example, driver instance 0 can be
    assigned to USART2.  If the driver is built statically, then some of the
    initialization parameters are overridden by configuration macros. Refer to
    the description of the DRV_USART_INIT data structure for more details on
    which members on this data structure are overridden.

  Precondition:
    None.

  Parameters:
    index  - Identifier for the instance to be initialized

    init   - Pointer to a data structure containing any data necessary to
             initialize the driver.

  Returns:
    If successful, returns a valid handle to a driver instance object.
    Otherwise, returns SYS_MODULE_OBJ_INVALID.

  Example:
    <code>
    // The following code snippet shows an example USART driver initialization.
    // The driver is initialized for normal mode and a baud of 300. The
    // receive queue size is set to 2 and transmit queue size is set to 3.

    DRV_USART_INIT              usartInit;
    SYS_MODULE_OBJ              objectHandle;

    usartInit.baud  = 300;
    usartInit.mode  = DRV_USART_OPERATION_MODE_NORMAL;
    usartInit.flags = DRV_USART_INIT_FLAG_NONE;
    usartInit.usartID   = USART_ID_2;
    usartInit.brgClock  = 80000000;
    usartInit.handshake = DRV_USART_HANDSHAKE_NONE;
    usartInit.lineControl       = DRV_USART_LINE_CONTROL_8NONE1;
    usartInit.interruptError    = INT_SOURCE_USART_2_ERROR;
    usartInit.interruptReceive  = INT_SOURCE_USART_2_RECEIVE;
    usartInit.queueSizeReceive  = 2;
    usartInit.queueSizeTransmit = 3;
    usartInit.interruptTransmit = INT_SOURCE_USART_2_TRANSMIT;
    usartInit.moduleInit.value  = SYS_MODULE_POWER_RUN_FULL;

    objectHandle = DRV_USART_Initialize(DRV_USART_INDEX_1, (SYS_MODULE_INIT*)&usartInitData);
    if (SYS_MODULE_OBJ_INVALID == objectHandle)
    {
        // Handle error
    }
    </code>

  Remarks:
    This routine must be called before any other USART routine is called.

    This routine should only be called once during system initialization
    unless DRV_USART_Deinitialize is called to deinitialize the driver
    instance. This routine will NEVER block for hardware access.
*/

SYS_MODULE_OBJ DRV_USART_Initialize
(
    const SYS_MODULE_INDEX index,
    const SYS_MODULE_INIT * const init
);

// *****************************************************************************
/* Function:
    void DRV_USART_Deinitialize( SYS_MODULE_OBJ object )

  Summary:
    Deinitializes the specified instance of the USART driver module.
    <p><b>Implementation:</b> Static/Dynamic</p>

  Description:
    Deinitializes the specified instance of the USART driver module, disabling
    its operation (and any hardware).  Invalidates all the internal data.

  Precondition:
    Function DRV_USART_Initialize should have been called before calling this
    function.

  Parameters:
    object          - Driver object handle, returned from the
                      DRV_USART_Initialize routine

  Returns:
    None.

  Example:
    <code>
    SYS_MODULE_OBJ      object;     //  Returned from DRV_USART_Initialize
    SYS_STATUS          status;

    DRV_USART_Deinitialize(object);

    status = DRV_USART_Status(object);
    if (SYS_MODULE_DEINITIALIZED != status)
    {
        // Check again later if you need to know
        // when the driver is deinitialized.
    }
    </code>

  Remarks:
    Once the Initialize operation has been called, the Deinitialize operation
    must be called before the Initialize operation can be called again. This
    routine will NEVER block waiting for hardware.
*/

void DRV_USART_Deinitialize( SYS_MODULE_OBJ object);

// *****************************************************************************
/* Function:
    SYS_STATUS DRV_USART_Status( SYS_MODULE_OBJ object )

  Summary:
    Gets the current status of the USART driver module.
    <p><b>Implementation:</b> Static/Dynamic</p>

  Description:
    This routine provides the current status of the USART driver module.

  Precondition:
    Function DRV_USART_Initialize should have been called before calling this
    function.

  Parameters:
    object          - Driver object handle, returned from the
                      DRV_USART_Initialize routine

  Returns:
    SYS_STATUS_READY          - Indicates that the driver is busy with a
                                previous system level operation and cannot start
                                another

    SYS_STATUS_DEINITIALIZED  - Indicates that the driver has been
                                deinitialized

  Example:
    <code>
    SYS_MODULE_OBJ      object;     // Returned from DRV_USART_Initialize
    SYS_STATUS          usartStatus;

    usartStatus = DRV_USART _Status(object);
    if (SYS_STATUS_READY == usartStatus)
    {
        // This means the driver can be opened using the
        // DRV_USART_Open() function.
    }
    </code>

  Remarks:
    A driver can opened only when its status is SYS_STATUS_READY.
*/

SYS_STATUS DRV_USART_Status( SYS_MODULE_OBJ object);

// *****************************************************************************
/* Function:
    void DRV_USART_TasksTransmit (SYS_MODULE_OBJ object );

  Summary:
    Maintains the driver's transmit state machine and implements its ISR.
    <p><b>Implementation:</b> Static/Dynamic</p>

  Description:
    This routine is used to maintain the driver's internal transmit state
    machine and implement its transmit ISR for interrupt-driven implementations.
    In polling mode, this function should be called from the SYS_Tasks
    function. In interrupt mode, this function should be called in the transmit
    interrupt service routine of the USART that is associated with this USART
    driver hardware instance.

  Precondition:
    The DRV_USART_Initialize routine must have been called for the specified
    USART driver instance.

  Parameters:
    object      - Object handle for the specified driver instance (returned from
                  DRV_USART_Initialize)

  Returns:
    None.

  Example:
    <code>
    SYS_MODULE_OBJ      object;     // Returned from DRV_USART_Initialize

    while (true)
    {
        DRV_USART_TasksTransmit (object);

        // Do other tasks
    }
    </code>

  Remarks:
    This routine is normally not called directly by an application.  It is
    called by the system's Tasks routine (SYS_Tasks) or by the appropriate raw
    ISR.

    This routine may execute in an ISR context and will never block or access any
    resources that may cause it to block.
*/

void DRV_USART_TasksTransmit ( SYS_MODULE_OBJ object );

// *****************************************************************************
/* Function:
    void DRV_USART_TasksReceive (SYS_MODULE_OBJ object );

  Summary:
    Maintains the driver's receive state machine and implements its ISR.
    <p><b>Implementation:</b> Static/Dynamic</p>

  Description:
    This routine is used to maintain the driver's internal receive state machine
    and implement its receive ISR for interrupt-driven implementations.  In
    polling mode, this function should be called from the SYS_Tasks function.
    In interrupt mode, this function should be called in the receive interrupt
    service routine of the USART that is associated with this USART driver
    hardware instance.

  Precondition:
    The DRV_USART_Initialize routine must have been called for the specified
    USART driver instance.

  Parameters:
    object      - Object handle for the specified driver instance (returned from
                  DRV_USART_Initialize)

  Returns:
    None.

  Example:
    <code>
    SYS_MODULE_OBJ      object;     // Returned from DRV_USART_Initialize

    while (true)
    {
        DRV_USART_TasksReceive (object);

        // Do other tasks
    }
    </code>

  Remarks:
    This routine is normally not called directly by an application.  It is
    called by the system's Tasks routine (SYS_Tasks) or by the appropriate raw
    ISR.

    This routine may execute in an ISR context and will never block or access any
    resources that may cause it to block.
*/

void DRV_USART_TasksReceive ( SYS_MODULE_OBJ object );

// *****************************************************************************
/* Function:
    void DRV_USART_TasksError (SYS_MODULE_OBJ object );

  Summary:
    Maintains the driver's error state machine and implements its ISR.
    <p><b>Implementation:</b> Static/Dynamic</p>

  Description:
    This routine is used to maintain the driver's internal error state machine
    and implement its error ISR for interrupt-driven implementations.  In
    polling mode, this function should be called from the SYS_Tasks function.
    In interrupt mode, this function should be called in the error interrupt
    service routine of the USART that is associated with this USART driver
    hardware instance.

  Precondition:
    The DRV_USART_Initialize routine must have been called for the specified
    USART driver instance.

  Parameters:
    object      - Object handle for the specified driver instance (returned from
                  DRV_USART_Initialize)

  Returns:
    None.

  Example:
    <code>
    SYS_MODULE_OBJ      object;     // Returned from DRV_USART_Initialize

    while (true)
    {
        DRV_USART_TasksError (object);

        // Do other tasks
    }
    </code>

  Remarks:
    This routine is normally not called directly by an application.  It is
    called by the system's Tasks routine (SYS_Tasks) or by the appropriate raw
    ISR.

    This routine may execute in an ISR context and will never block or access any
    resources that may cause it to block.
*/

void DRV_USART_TasksError ( SYS_MODULE_OBJ object );

// *****************************************************************************
// *****************************************************************************
// Section: USART Driver Client Routines
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    DRV_HANDLE DRV_USART_Open
    (
        const SYS_MODULE_INDEX index,
        const DRV_IO_INTENT ioIntent
    )

  Summary:
    Opens the specified USART driver instance and returns a handle to it.
    <p><b>Implementation:</b> Static/Dynamic</p>

  Description:
    This routine opens the specified USART driver instance and provides a
    handle that must be provided to all other client-level operations to
    identify the caller and the instance of the driver. The ioIntent
    parameter defines how the client interacts with this driver instance.

    The DRV_IO_INTENT_BLOCKING and DRV_IO_INTENT_NONBLOCKING ioIntent options
    additionally affect the behavior of the DRV_USART_Read and
    DRV_USART_Write functions. If the ioIntent is DRV_IO_INTENT_NONBLOCKING,
    then these function will not block even if the required amount of data could
    not be processed. If the ioIntent is DRV_IO_INTENT_BLOCKING, these functions
    will block until the required amount of data is processed.  If the driver is
    configured for polling and bare-metal operation, it will not support
    DRV_IO_INTENT_BLOCKING. The driver will operation will always be
    non-blocking.

    If ioIntent is DRV_IO_INTENT_READ, the client will only be able to read from
    the driver. If ioIntent is DRV_IO_INTENT_WRITE, the client will only be able
    to write to the driver. If the ioIntent is DRV_IO_INTENT_READWRITE, the
    client will be able to do both, read and write.

    Specifying a DRV_IO_INTENT_EXCLUSIVE will cause the driver to provide
    exclusive access to this client. The driver cannot be opened by any
    other client.

  Precondition:
    Function DRV_USART_Initialize must have been called before calling this
    function.

  Parameters:
    index   - Identifier for the object instance to be opened

    intent  - Zero or more of the values from the enumeration
              DRV_IO_INTENT "ORed" together to indicate the intended use
              of the driver. See function description for details.

  Returns:
    If successful, the routine returns a valid open-instance handle (a number
    identifying both the caller and the module instance).

    If an error occurs, the return value is DRV_HANDLE_INVALID. Error can occur
    - if the number of client objects allocated via DRV_USART_CLIENTS_NUMBER is
      insufficient.
    - if the client is trying to open the driver but driver has been opened
      exclusively by another client.
    - if the driver hardware instance being opened is not initialized or is
      invalid.
    - if the client is trying to open the driver exclusively, but has already
      been opened in a non exclusive mode by another client.
    - if the driver is not ready to be opened, typically when the initialize
      routine has not completed execution.

  Example:
    <code>
    DRV_HANDLE handle;

    handle = DRV_USART_Open(DRV_USART_INDEX_0, DRV_IO_INTENT_EXCLUSIVE);
    if (DRV_HANDLE_INVALID == handle)
    {
        // Unable to open the driver
        // May be the driver is not initialized or the initialization
        // is not complete.
    }
    </code>

  Remarks:
    The handle returned is valid until the DRV_USART_Close routine is called.
    This routine will NEVER block waiting for hardware.If the requested intent
    flags are not supported, the routine will return DRV_HANDLE_INVALID.  This
    function is thread safe in a RTOS application.
*/

DRV_HANDLE DRV_USART_Open( const SYS_MODULE_INDEX index, const DRV_IO_INTENT ioIntent);

// *****************************************************************************
/* Function:
    void DRV_USART_Close( DRV_Handle handle )

  Summary:
    Closes an opened-instance of the USART driver.
    <p><b>Implementation:</b> Static/Dynamic</p>

  Description:
    This routine closes an opened-instance of the USART driver, invalidating the
    handle. Any buffers in the driver queue that were submitted by this client
    will be removed.  After calling this routine, the handle passed in "handle"
    must not be used with any of the remaining driver routines (with one
    possible exception described in the "Remarks" section).  A new handle must
    be obtained by calling DRV_USART_Open before the caller may use the driver
    again

  Precondition:
    The DRV_USART_Initialize routine must have been called for the specified
    USART driver instance.

    DRV_USART_Open must have been called to obtain a valid opened device handle.

  Parameters:
    handle       - A valid open-instance handle, returned from the driver's
                   open routine

  Returns:
    None.

  Example:
    <code>
    DRV_HANDLE handle;  // Returned from DRV_USART_Open

    DRV_USART_Close(handle);

    // After this point, the handle cannot be used with any other function
    // except the DRV_USART_ClientStatus function, which can be used to query
    // the success status of the DRV_USART_Close function.

    while(DRV_USART_CLIENT_STATUS_CLOSED != DRV_USART_ClientStatus(handle));

    </code>

  Remarks:
    Usually there is no need for the client to verify that the Close operation
    has completed.  The driver will abort any ongoing operations when this
    routine is called.  However, if it requires additional time to do so in a
    non-blocking environment, it will still return from the Close operation but
    the handle is now a zombie handle.  The client can only call the
    DRV_USART_ClientStatus on a zombie handle to track the completion of the
    Close operation.  The DRV_USART_ClientStatus routine will return
    DRV_CLIENT_STATUS_CLOSED when the close operation has completed.
*/

void DRV_USART_Close( const DRV_HANDLE handle);

// *****************************************************************************
/*
  Function:
    DRV_USART_CLIENT_STATUS DRV_USART_ClientStatus( DRV_HANDLE handle )

  Summary:
    Gets the current client-specific status the USART driver.
    <p><b>Implementation:</b> Static/Dynamic</p>

  Description:
    This function gets the client-specific status of the USART driver associated
    with the given handle. This function can be used to check the status of
    client after the DRV_USART_Close() function has been called.

  Preconditions:
    The DRV_USART_Initialize function must have been called.

    DRV_USART_Open must have been called to obtain a valid opened device
    handle.

  Parameters:
    handle -  Handle returned from the driver's open function.

  Returns:
    A DRV_USART_CLIENT_STATUS value describing the current status of the
    driver.

  Example:
    <code>
    DRV_HANDLE              handle;  // Returned from DRV_USART_Open
    DRV_USART_CLIENT_STATUS   status;

    status = DRV_USART_ClientStatus(handle);
    if( DRV_USART_CLIENT_STATUS_CLOSED != status )
    {
        // The client had not closed.
    }
    </code>

  Remarks:
    This function will not block for hardware access and will immediately return
    the current status. This function is thread safe when called in a RTOS
    application.
*/

DRV_USART_CLIENT_STATUS DRV_USART_ClientStatus ( DRV_HANDLE handle );

// *****************************************************************************
// *****************************************************************************
// Section: USART Driver Buffer Queuing Model Interface Routines
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    void DRV_USART_BufferAddWrite
    (
        const DRV_HANDLE handle,
        DRV_USART_BUFFER_HANDLE * bufferHandle,
        void * buffer,
        size_t size
    );

  Summary:
    Schedule a non-blocking driver write operation.
    <p><b>Implementation:</b> Static/Dynamic</p>

  Description:
    This function schedules a non-blocking write operation. The function returns
    with a valid buffer handle in the bufferHandle argument if the write request
    was scheduled successfully.  The function adds the request to the hardware
    instance transmit queue and returns immediately. While the request is in the
    queue, the application buffer is owned by the driver and should not be
    modified.  On returning, the bufferHandle parameter may be
    DRV_USART_BUFFER_HANDLE_INVALID for the following reasons:
    - if a buffer could not be allocated to the request
    - if the input buffer pointer is NULL
    - if the client opened the driver for read-only
    - if the buffer size is 0
    - if the transmit queue is full or the queue depth is insufficient

    If the requesting client registered an event callback with the driver, the
    driver will issue a DRV_USART_BUFFER_EVENT_COMPLETE event if the buffer was
    processed successfully or a DRV_USART_BUFFER_EVENT_ERROR event if the buffer
    was not processed successfully.

  Precondition:
    The DRV_USART_Initialize routine must have been called for the specified
    USART device instance and the DRV_USART_Status must have returned
    SYS_STATUS_READY.

    DRV_USART_Open must have been called to obtain a valid opened device handle.

    DRV_IO_INTENT_WRITE or DRV_IO_INTENT_READWRITE must have been specified in
    the DRV_USART_Open call.

  Parameters:
    handle       - Handle of the communication channel as return by the
                   DRV_USART_Open function.

    bufferHandle - Pointer to an argument that will contain the return buffer handle.

    buffer       - Data to be transmitted.

    size         - Buffer size in bytes.

  Returns:
    The bufferHandle parameter will contain the return buffer handle. This will be
    DRV_USART_BUFFER_HANDLE_INVALID if the function was not successful.

  Example:
    <code>

    MY_APP_OBJ myAppObj;
    uint8_t mybuffer[MY_BUFFER_SIZE];
    DRV_BUFFER_HANDLE bufferHandle;

    // myUSARTHandle is the handle returned
    // by the DRV_USART_Open function.

    // Client registers an event handler with driver

    DRV_USART_BufferEventHandlerSet(myUSARTHandle,
                    APP_USARTBufferEventHandler, (uintptr_t)&myAppObj);

    DRV_USART_BufferAddWrite(myUSARThandle, &bufferHandle,
                                        myBuffer, MY_BUFFER_SIZE);

    if(DRV_USART_BUFFER_HANDLE_INVALID == bufferHandle)
    {
        // Error handling here
    }

    // Event is received when
    // the buffer is processed.

    void APP_USARTBufferEventHandler(DRV_USART_BUFFER_EVENT event,
            DRV_USART_BUFFER_HANDLE bufferHandle, uintptr_t contextHandle)
    {
        // contextHandle points to myAppObj.

        switch(event)
        {
            case DRV_USART_BUFFER_EVENT_COMPLETE:

                // This means the data was transferred.
                break;

            case DRV_USART_BUFFER_EVENT_ERROR:

                // Error handling here.

                break;

            default:
                break;
        }
    }

    </code>

  Remarks:
    This function is thread safe in a RTOS application. It can be called from
    within the USART Driver Buffer Event Handler that is registered by this
    client. It should not be called in the event handler associated with another
    USART driver instance. It should not otherwise be called directly in an ISR.

*/

void DRV_USART_BufferAddWrite
(
    const DRV_HANDLE handle,
    DRV_USART_BUFFER_HANDLE * bufferHandle,
    void * buffer,
    const size_t size
);

// *****************************************************************************
/* Function:
    void DRV_USART_AddressedBufferAddWrite
    (
        const DRV_HANDLE hClient,
        DRV_USART_BUFFER_HANDLE * bufferHandle,
        uint8_t address,
        void * source,
        size_t nWords
    );

  Summary:
    Schedule a non-blocking addressed driver write operation.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function schedules a non-blocking addressed write operation. The function returns
    with a valid buffer handle in the bufferHandle argument if the addressed write request
    was scheduled successfully.  The function adds the request to the hardware
    instance transmit queue and returns immediately. While the request is in the
    queue, the application buffer is owned by the driver and should not be
    modified.  On returning, the bufferHandle parameter may be
    DRV_USART_BUFFER_HANDLE_INVALID for the following reasons:
    - if a buffer could not be allocated to the request
    - if the input buffer pointer is NULL
    - if the client opened the driver for read-only
    - if the buffer size is 0
    - if the transmit queue is full or the queue depth is insufficient

    If the requesting client registered an event callback with the driver, the
    driver will issue a DRV_USART_BUFFER_EVENT_COMPLETE event if the buffer was
    processed successfully or a DRV_USART_BUFFER_EVENT_ERROR event if the buffer
    was not processed successfully.

  Precondition:
    The DRV_USART_Initialize routine must have been called for the specified
    USART device instance and the DRV_USART_Status must have returned
    SYS_STATUS_READY.

    DRV_USART_Open must have been called to obtain a valid opened device handle.

    DRV_IO_INTENT_WRITE or DRV_IO_INTENT_READWRITE must have been specified in
    the DRV_USART_Open call.

    The operation mode of the driver must be DRV_USART_OPERATION_MODE_ADDRESSED.

  Parameters:
    hClient       - Handle of the communication channel as return by the
                   DRV_USART_Open function.

    bufferHandle - Pointer to an argument that will contain the return buffer handle.

    address      - Address of the receiver client

    source       - Data to be transmitted.

    size         - Buffer size in 16-bit words.

  Returns:
    The bufferHandle parameter will contain the return buffer handle. This will be
    DRV_USART_BUFFER_HANDLE_INVALID if the function was not successful.

  Example:
    <code>

    MY_APP_OBJ myAppObj;
    uint16_t mybuffer[MY_BUFFER_SIZE];
    DRV_BUFFER_HANDLE bufferHandle;
    uint8_t clientAddress;

    // myUSARTHandle is the handle returned
    // by the DRV_USART_Open function.

    // Client registers an event handler with driver

    clientAddress = 0x60;
    DRV_USART_BufferEventHandlerSet(myUSARTHandle,
                    APP_USARTBufferEventHandler, (uintptr_t)&myAppObj);

    DRV_USART_AddressedBufferAddWrite(myUSARThandle, &bufferHandle, clientAddress
                                        myBuffer, MY_BUFFER_SIZE);

    if(DRV_USART_BUFFER_HANDLE_INVALID == bufferHandle)
    {
        // Error handling here
    }

    // Event is received when
    // the buffer is processed.

    void APP_USARTBufferEventHandler(DRV_USART_BUFFER_EVENT event,
            DRV_USART_BUFFER_HANDLE bufferHandle, uintptr_t contextHandle)
    {
        // contextHandle points to myAppObj.

        switch(event)
        {
            case DRV_USART_BUFFER_EVENT_COMPLETE:

                // This means the data was transferred.
                break;

            case DRV_USART_BUFFER_EVENT_ERROR:

                // Error handling here.

                break;

            default:
                break;
        }
    }

    </code>

  Remarks:
    This function is thread safe in a RTOS application. It can be called from
    within the USART Driver Buffer Event Handler that is registered by this
    client. It should not be called in the event handler associated with another
    USART driver instance. It should not otherwise be called directly in an ISR.

    The source buffer should be a 16-bit word aligned buffer.
    The 9th bit of the higher byte 16-bit buffer is used to indicate data/address.
*/
void DRV_USART_AddressedBufferAddWrite
(
    const DRV_HANDLE hClient,
    DRV_USART_BUFFER_HANDLE * bufferHandle,
    uint8_t address,
    void * source,
    size_t nWords
);

// *****************************************************************************
/* Function:
    void DRV_USART_BufferAddRead
    (
        const DRV_HANDLE handle,
        DRV_USART_BUFFER_HANDLE * bufferHandle,
        void * buffer,
        const size_t size
    )

  Summary:
    Schedule a non-blocking driver read operation.
    <p><b>Implementation:</b> Static/Dynamic</p>

  Description:
    This function schedules a non-blocking read operation. The function returns
    with a valid buffer handle in the bufferHandle argument if the read request
    was scheduled successfully. The function adds the request to the hardware
    instance receive queue and returns immediately. While the request is in the
    queue, the application buffer is owned by the driver and should not be
    modified. The function returns DRV_USART_BUFFER_HANDLE_INVALID in the
    bufferHandle argument:
    - if a buffer could not be allocated to the request
    - if the input buffer pointer is NULL
    - if the buffer size is 0
    - if the read queue size is full or queue depth is insufficient.
    - if the driver handle is invalid

    If the requesting client registered an event callback with the driver, the
    driver will issue a DRV_USART_BUFFER_EVENT_COMPLETE event if the buffer was
    processed successfully of DRV_USART_BUFFER_EVENT_ERROR event if the buffer
    was not processed successfully.

  Precondition:
    The DRV_USART_Initialize routine must have been called for the specified
    USART device instance and the DRV_USART_Status must have returned
    SYS_STATUS_READY.

    DRV_USART_Open must have been called to obtain a valid opened device handle.

    DRV_IO_INTENT_READ or DRV_IO_INTENT_READWRITE must have been specified in
    the DRV_USART_Open call.

  Parameters:
    handle       - Handle of the communication channel as returned by the
                   DRV_USART_Open function.

    buffer       - Buffer where the received data will be stored.

    size         - Buffer size in bytes.

  Returns:
     The buffer handle is returned in the bufferHandle argument. This is
     DRV_USART_BUFFER_HANDLE_INVALID if the request was not successful.

  Example:
    <code>

    MY_APP_OBJ myAppObj;
    uint8_t mybuffer[MY_BUFFER_SIZE];
    DRV_USART_BUFFER_HANDLE bufferHandle;

    // myUSARTHandle is the handle returned
    // by the DRV_USART_Open function.

    // Client registers an event handler with driver

    DRV_USART_BufferEventHandlerSet(myUSARTHandle,
                    APP_USARTBufferEventHandler, (uintptr_t)&myAppObj);

    DRV_USART_BufferAddRead(myUSARThandle, &bufferHandle,
                                        myBuffer, MY_BUFFER_SIZE);

    if(DRV_USART_BUFFER_HANDLE_INVALID == bufferHandle)
    {
        // Error handling here
    }

    // Event is received when
    // the buffer is processed.

    void APP_USARTBufferEventHandler(DRV_USART_BUFFER_EVENT event,
            DRV_USART_BUFFER_HANDLE bufferHandle, uintptr_t contextHandle)
    {
        // contextHandle points to myAppObj.

        switch(event)
        {
            case DRV_USART_BUFFER_EVENT_COMPLETE:

                // This means the data was transferred.
                break;

            case DRV_USART_BUFFER_EVENT_ERROR:

                // Error handling here.

                break;

            default:
                break;
        }
    }

    </code>

  Remarks:
    This function is thread safe in a RTOS application. It can be called from
    within the USART Driver Buffer Event Handler that is registered by the
    client. It should not be called in the event handler associated with another
    USART driver instance. It should not be called directly in an ISR.

*/

void DRV_USART_BufferAddRead
(
    const DRV_HANDLE handle,
    DRV_USART_BUFFER_HANDLE * const bufferHandle,
    void * buffer,
    const size_t size
);

// *****************************************************************************
/* Function:
    void DRV_USART_BufferEventHandlerSet
    (
        const DRV_HANDLE handle,
        const DRV_USART_BUFFER_EVENT_HANDLER eventHandler,
        const uintptr_t context
    )

  Summary:
    Allows a client to identify a buffer event handling function for the driver
    to call back when queued buffer transfers have finished.
    <p><b>Implementation:</b> Static/Dynamic</p>

  Description:
    This function allows a client to identify a buffer event handling function
    for the driver to call back when queued buffer transfers have finished.
    When a client calls either the DRV_USART_BufferAddRead or
    DRV_USART_BufferAddWrite function, it is provided with a handle identifying
    the buffer that was added to the driver's buffer queue.  The driver will
    pass this handle back to the client by calling "eventHandler" function when
    the buffer transfer has completed.

    The event handler should be set before the client performs any "buffer add"
    operations that could generate events. The event handler once set, persists
    until the client closes the driver or sets another event handler (which
    could be a "NULL" pointer to indicate no callback).

  Precondition:
    The DRV_USART_Initialize routine must have been called for the specified
    USART driver instance.

    DRV_USART_Open must have been called to obtain a valid opened device handle.

  Parameters:
    handle       - A valid open-instance handle, returned from the driver's
                   open routine

    eventHandler - Pointer to the event handler function.

    context      - The value of parameter will be passed back to the client
                   unchanged, when the eventHandler function is called.  It can
                   be used to identify any client specific data object that
                   identifies the instance of the client module (for example,
                   it may be a pointer to the client module's state structure).

  Returns:
    None.

  Example:
    <code>
    // myAppObj is an application specific state data object.
    MY_APP_OBJ myAppObj;

    uint8_t mybuffer[MY_BUFFER_SIZE];
    DRV_USART_BUFFER_HANDLE bufferHandle;

    // myUSARTHandle is the handle returned
    // by the DRV_USART_Open function.

    // Client registers an event handler with driver. This is done once

    DRV_USART_BufferEventHandlerSet( myUSARTHandle, APP_USARTBufferEventHandle,
                                     (uintptr_t)&myAppObj );

    DRV_USART_BufferAddRead(myUSARThandle, &bufferHandle
                                        myBuffer, MY_BUFFER_SIZE);

    if(DRV_USART_BUFFER_HANDLE_INVALID == bufferHandle)
    {
        // Error handling here
    }

    // Event Processing Technique. Event is received when
    // the buffer is processed.

    void APP_USARTBufferEventHandler(DRV_USART_BUFFER_EVENT event,
            DRV_USART_BUFFER_HANDLE handle, uintptr_t context)
    {
        // The context handle was set to an application specific
        // object. It is now retrievable easily in the event handler.
        MY_APP_OBJ myAppObj = (MY_APP_OBJ *) context;

        switch(event)
        {
            case DRV_USART_BUFFER_EVENT_SUCCESS:

                // This means the data was transferred.
                break;

            case DRV_USART_BUFFER_EVENT_FAILURE:

                // Error handling here.

                break;

            default:
                break;
        }
    }
    </code>

  Remarks:
    If the client does not want to be notified when the queued buffer transfer
    has completed, it does not need to register a callback. This function is
    thread safe when called in a RTOS application.
*/

void DRV_USART_BufferEventHandlerSet
(
    const DRV_HANDLE handle,
    const DRV_USART_BUFFER_EVENT_HANDLER eventHandler,
    const uintptr_t context
);

// *****************************************************************************
/* Function:
    DRV_USART_BUFFER_RESULT DRV_USART_BufferRemove( DRV_USART_BUFFER_HANDLE bufferHandle )

  Summary:
    Removes a requested buffer from the queue.
    <p><b>Implementation:</b> Static/Dynamic</p>

  Description:
    This function removes a specified buffer from the queue.
    The client can use this function to delete
        1. An unwated stalled buffer.
        2. Queued buffers on timeout.
    or in any other use case.

  Precondition:
    The DRV_USART_Initialize routine must have been called for the specified
    USART driver instance.

    DRV_USART_Open must have been called to obtain a valid opened device handle.

    Either the DRV_USART_BufferAddRead or DRV_USART_BufferAddWrite function
    must have been called and a valid buffer handle returned.

  Parameters:
    bufferhandle    - Handle of the buffer to delete.

  Returns:
    DRV_USART_BUFFER_RESULT_HANDLE_INVALID  - Buffer handle is invalid.

    DRV_USART_BUFFER_RESULT_HANDLE_EXPIRED  - Buffer handle is expired.

    DRV_USART_BUFFER_RESULT_REMOVED_SUCCESFULLY - Buffer is removed from the
                queue successfully.

    DRV_USART_BUFFER_RESULT_REMOVAL_FAILED - Failed to remove buffer from
                the queue because of mutex timeout in RTOS environment.

  Example:
    <code>
    // myAppObj is an application specific object.
    MY_APP_OBJ myAppObj;

    uint8_t mybuffer[MY_BUFFER_SIZE];
    DRV_BUFFER_HANDLE bufferHandle;

    // myUSARTHandle is the handle returned
    // by the DRV_USART_Open function.

    // Client registers an event handler with driver. This is done once

    DRV_USART_BufferEventHandlerSet( myUSARTHandle, APP_USARTBufferEventHandle,
                                     (uintptr_t)&myAppObj );

    bufferHandle = DRV_USART_BufferAddRead( myUSARThandle,
                                            myBuffer, MY_BUFFER_SIZE );

    if(DRV_USART_BUFFER_HANDLE_INVALID == bufferHandle)
    {
        // Error handling here
    }

    // Event Processing Technique. Event is received when
    // the buffer is processed.

    void APP_USARTBufferEventHandler( DRV_USART_BUFFER_EVENT event,
            DRV_USART_BUFFER_HANDLE bufferHandle, uintptr_t contextHandle )
    {
        switch(event)
        {
            case DRV_USART_BUFFER_EVENT_SUCCESS:

                // This means the data was transferred.
                break;

            case DRV_USART_BUFFER_EVENT_FAILURE:

                // Error handling here.

                break;

            default:
                break;
        }
    }

    // Timeout function, where remove queued buffer if it still exists.
    void APP_TimeOut(void)
    {
        DRV_USART_BUFFER_RESULT bufferResult;
        bufferResult = DRV_USART_BufferRemove(bufferHandle);

        if(DRV_USART_BUFFER_RESULT_REMOVED_SUCCESFULLY == bufferResult)
        {
            //Buffer removed succesfully from the queue
        }
        else
        {
            //Either buffer is invalid or expired.
            //Or not able to acquire mutex in RTOS mode.
        }
    }
    </code>

  Remarks:
    This function is thread safe when used in a RTOS application.
*/

DRV_USART_BUFFER_RESULT DRV_USART_BufferRemove( DRV_USART_BUFFER_HANDLE bufferHandle );

// *****************************************************************************
/* Function:
    size_t DRV_USART_BufferCompletedBytesGet
    (
        DRV_USART_BUFFER_HANDLE bufferHandle
    );

  Summary:
    Returns the number of bytes that have been processed for the
    specified buffer.
    <p><b>Implementation:</b> Static/Dynamic</p>

  Description:
    This function returns number of bytes that have been processed for the
    specified buffer. The client can use this function, in a case where the
    buffer has terminated due to an error, to obtain the number of bytes that
    have been processed. Or in any other use case.
    This function can be used for non-DMA buffer transfers only. It cannot be 
    used when the USART driver is configured to use DMA.

  Precondition:
    The DRV_USART_Initialize routine must have been called for the specified
    USART driver instance.

    DRV_USART_Open must have been called to obtain a valid opened device handle.

    Either the DRV_USART_BufferAddRead or DRV_USART_BufferAddWrite function
    must have been called and a valid buffer handle returned.

  Parameters:
    bufferhandle    - Handle for the buffer of which the processed number of bytes
                      to be obtained.

  Returns:
    Returns the number of bytes that have been processed for this buffer.

    Returns DRV_USART_BUFFER_HANDLE_INVALID for an invalid or an expired
    buffer handle.

  Example:
    <code>
    // myAppObj is an application specific object.
    MY_APP_OBJ myAppObj;

    uint8_t mybuffer[MY_BUFFER_SIZE];
    DRV_BUFFER_HANDLE bufferHandle;

    // myUSARTHandle is the handle returned
    // by the DRV_USART_Open function.

    // Client registers an event handler with driver. This is done once

    DRV_USART_BufferEventHandlerSet( myUSARTHandle, APP_USARTBufferEventHandle,
                                     (uintptr_t)&myAppObj );

    bufferHandle = DRV_USART_BufferAddRead( myUSARThandle,
                                            myBuffer, MY_BUFFER_SIZE );

    if(DRV_USART_BUFFER_HANDLE_INVALID == bufferHandle)
    {
        // Error handling here
    }

    // Event Processing Technique. Event is received when
    // the buffer is processed.

    void APP_USARTBufferEventHandler( DRV_USART_BUFFER_EVENT event,
            DRV_USART_BUFFER_HANDLE bufferHandle, uintptr_t contextHandle )
    {
        // The context handle was set to an application specific
        // object. It is now retrievable easily in the event handler.
        MY_APP_OBJ myAppObj = (MY_APP_OBJ *) contextHandle;
        size_t processedBytes;

        switch(event)
        {
            case DRV_USART_BUFFER_EVENT_SUCCESS:

                // This means the data was transferred.
                break;

            case DRV_USART_BUFFER_EVENT_FAILURE:

                // Error handling here.
                // We can find out how many bytes were processed in this
                // buffer before the error occurred.

                processedBytes = DRV_USART_BufferCompletedBytesGet(bufferHandle);

                break;

            default:
                break;
        }
    }
    </code>

  Remarks:
    This function is thread safe when used in a RTOS application.
*/

size_t DRV_USART_BufferCompletedBytesGet( DRV_USART_BUFFER_HANDLE bufferHandle );

// *****************************************************************************
/* Function:
    size_t DRV_USART_BufferProcessedSizeGet
    (
        DRV_USART_BUFFER_HANDLE bufferHandle
    );

  Summary:
    This API will be deprecated and not recommended to use.
    Use DRV_USART_BufferCompletedBytesGet to get the number of bytes
    processed for the specified buffer.

  Description:
    None.

  Precondition:
    None.

  Parameters:
    None.

  Returns:
    None.

  Example:
    None.

  Remarks:
    None.
*/

size_t DRV_USART_BufferProcessedSizeGet( DRV_USART_BUFFER_HANDLE bufferHandle );

// *****************************************************************************
// *****************************************************************************
// Section: USART Driver File System Model Routines
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    size_t DRV_USART_Read
    (
        const DRV_HANDLE handle,
        void * buffer,
        const size_t numbytes
    )

  Summary:
    Reads data from the USART.
    <p><b>Implementation:</b> Static/Dynamic</p>

  Description:
    This routine reads data from the USART. This function is blocking if the
    driver was opened by the client for blocking operation.  This function will
    not block if the driver was opened by the client for non blocking operation.
    If the ioIntent parameter at the time of opening the driver was
    DRV_IO_INTENT_BLOCKING, this function will only return when (or will block
    until) numbytes of bytes have been received or if an error occurred. If there
    are buffers queued for receiving data, these buffers will be serviced first. The
    function will not return until the requested number of bytes have been read.

    If the ioIntent parameter at the time of opening the driver was
    DRV_IO_INTENT_NON_BLOCKING, this function will return with the number of
    bytes that were actually read. The function will not wait until numBytes of
    bytes have been read. If there are buffer queued for reading data, then the
    function will not block and will return immediately with 0 bytes read.

  Precondition:
    The DRV_USART_Initialize routine must have been called for the specified
    USART driver instance.

    DRV_USART_Open must have been called to obtain a valid opened device handle.

    DRV_IO_INTENT_READ or DRV_IO_INTENT_READWRITE must have been specified in
    the DRV_USART_Open call.

  Parameters:
    handle       - A valid open-instance handle, returned from the driver's
                   open routine

    buffer       - Buffer into which the data read from the USART instance
                   will be placed.

    numbytes     - Total number of bytes that need to be read from the module
                   instance (must be equal to or less than the size of the
                   buffer)

  Returns:
    Number of bytes actually copied into the caller's buffer. Returns
    DRV_USART_READ_ERROR in case of an error.

  Example:
    <code>
    DRV_HANDLE      myUSARTHandle;    // Returned from DRV_USART_Open
    char            myBuffer[MY_BUFFER_SIZE];
    unsigned int    count;
    unsigned int    total;

    total = 0;
    do
    {
        count  = DRV_USART_Read(myUSARTHandle, &myBuffer[total], MY_BUFFER_SIZE - total);
        if(count == DRV_USART_READ_ERROR)
        {
           // There was an error. The DRV_USART_ErrorGet() function
           // can be called to find the exact error.
        }
        total += count;

        // Do something else...

    } while( total < MY_BUFFER_SIZE );
    </code>

  Remarks:
    This function is thread safe in a RTOS application. If the driver is
    configured for polled operation, this it will not support blocking operation
    in a bare metal (non-RTOS) application.
*/

size_t DRV_USART_Read
(
    const DRV_HANDLE handle,
    void * buffer,
    const size_t numbytes
);

// *****************************************************************************
/* Function:
    size_t DRV_USART_Write
    (
        const DRV_HANDLE handle,
        void * buffer,
        const size_t numbytes
    )

  Summary:
    Writes data to the USART.
    <p><b>Implementation:</b> Static/Dynamic</p>

  Description:
    This routine writes data to the USART. This function is blocking if the
    driver was opened by the client for blocking operation.  This function will
    not block if the driver was opened by the client for non blocking operation.
    If the ioIntent parameter at the time of opening the driver was
    DRV_IO_INTENT_BLOCKING, this function will only return when (or will block
    until) numbytes of bytes have been transmitted or if an error occurred. If
    there are buffers queued for writing, the function will wait until all the
    preceding buffers are completed. Ongoing buffer transmit operations will not
    be affected.

    If the ioIntent parameter at the time of opening the driver was
    DRV_IO_INTENT_NON_BLOCKING, this function will return with the number of
    bytes that were actually accepted for transmission. The function will not
    wait until numBytes of bytes have been transmitted. If there a buffers queued
    for transmit, the function will not wait and will return immediately with 0
    bytes.

  Precondition:
    The DRV_USART_Initialize routine must have been called for the specified
    USART driver instance.

    DRV_USART_Open must have been called to obtain a valid opened device handle.

    DRV_IO_INTENT_WRITE or DRV_IO_INTENT_READWRITE must have been specified in
    the DRV_USART_Open call.

  Parameters:
    handle       - A valid open-instance handle, returned from the driver's
                   open routine

    buffer       - Buffer containing the data to written.

    numbytes     - size of the buffer

  Returns:
    Number of bytes actually written to the driver. Return DRV_USART_WRITE_ERROR
    in case of an error.

  Example:
    <code>
    DRV_HANDLE      myUSARTHandle;    // Returned from DRV_USART_Open
    char            myBuffer[MY_BUFFER_SIZE];
    int    count;
    unsigned int    total;

    total = 0;
    do
    {
        count  = DRV_USART_Write(myUSARTHandle, &myBuffer[total],
                                                MY_BUFFER_SIZE - total);
        total += count;

        // Do something else...

    } while( total < MY_BUFFER_SIZE );
    </code>

  Remarks:
    This function is thread safe in a RTOS application.  This function is thread
    safe in a RTOS application. If the driver is configured for polled
    operation, this it will not support blocking operation in a bare metal (non-RTOS)
    application.
*/

size_t DRV_USART_Write
(
    const DRV_HANDLE handle,
    void * buffer,
    const size_t numbytes
);

// *****************************************************************************
/* Function:
    DRV_USART_TRANSFER_STATUS DRV_USART_TransferStatus( const DRV_HANDLE handle )

  Summary:
    Returns the transmitter and receiver transfer status.
    <p><b>Implementation:</b> Static/Dynamic</p>

  Description:
    This returns the transmitter and receiver transfer status.

  Precondition:
    The DRV_USART_Initialize routine must have been called for the specified
    USART driver instance.

    DRV_USART_Open must have been called to obtain a valid opened device handle.

  Parameters:
    handle       - A valid open-instance handle, returned from the driver's
                   open routine

  Returns:
    A DRV_USART_TRANSFER_STATUS value describing the current status
    of the transfer.

  Example:
  <code>
    DRV_HANDLE      myUSARTHandle;    // Returned from DRV_USART_Open

    if (DRV_USART_TRANSFER_STATUS_RECEIVER_DATA_PRESENT & DRV_USART_TransferStatus(myUSARTHandle))
    {
        // Data has been received that can be read
    }
  </code>

  Remarks:
    The returned status may contain a value with more than one of the bits
    specified in the DRV_USART_TRANSFER_STATUS enumeration set.  The caller
    should perform an "AND" with the bit of interest and verify if the result is
    non-zero (as shown in the example) to verify the desired status bit. This
    function is thread safe when called in a RTOS application.
*/

DRV_USART_TRANSFER_STATUS DRV_USART_TransferStatus( const DRV_HANDLE handle );


// *****************************************************************************
// *****************************************************************************
// Section: USART Driver Byte Model Routines
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    uint8_t DRV_USART_ReadByte( const DRV_HANDLE handle )

  Summary:
    Reads a byte of data from the USART.
    <p><b>Implementation:</b> Static/Dynamic</p>

  Description:
    This routine reads a byte of data from the USART.

  Precondition:
    The DRV_USART_Initialize routine must have been called for the specified
    USART driver instance.

    DRV_USART_Open must have been called to obtain a valid opened device handle.

    The transfer status should be checked to see if the receiver is not empty
    before calling this function.

  Parameters:
    handle       - A valid open-instance handle, returned from the driver's
                   open routine

  Returns:
    A data byte received by the driver.

  Example:
    <code>
    DRV_HANDLE      myUSARTHandle;    // Returned from DRV_USART_Open
    char            myBuffer[MY_BUFFER_SIZE];
    unsigned int    numBytes;

    numBytes = 0;
    do
    {
        if( DRV_USART_TRANSFER_STATUS_RECEIVER_DATA_PRESENT & DRV_USART_TransferStatus(myUSARTHandle) )
        {
            myBuffer[numBytes++] = DRV_USART_ReadByte(myUSARTHandle);
        }

        // Do something else...

    } while( numBytes < MY_BUFFER_SIZE);
    </code>

  Remarks:
    This function is thread safe when called in a RTOS application. Note that
    DRV_USART_WriteByte and DRV_USART_ReadByte function cannot co-exist with
    DRV_USART_BufferAddRead, DRV_USART_BufferAddWrite, DRV_USART_Read and
    DRV_USART_Write functions in a application. Calling the
    DRV_USART_ReadByte and DRV_USART_WriteByte functions will disrupt the
    processing of any queued buffers.
*/

uint8_t DRV_USART_ReadByte( const DRV_HANDLE handle );

// *****************************************************************************
/* Function:
    void DRV_USART_WriteByte( const DRV_HANDLE handle, const uint8_t byte)

  Summary:
    Writes a byte of data to the USART.
    <p><b>Implementation:</b> Static/Dynamic</p>

  Description:
    This routine writes a byte of data to the USART.

  Precondition:
    The DRV_USART_Initialize routine must have been called for the specified
    USART driver instance.

    DRV_USART_Open must have been called to obtain a valid opened device handle.

    The transfer status should be checked to see if transmitter is not full
    before calling this function.

  Parameters:
    handle       - A valid open-instance handle, returned from the driver's
                   open routine

    byte         - Data byte to write to the USART

  Returns:
    None.

  Example:
    <code>
    DRV_HANDLE      myUSARTHandle;    // Returned from DRV_USART_Open
    char            myBuffer[MY_BUFFER_SIZE];
    unsigned int    numBytes;

    // Preinitialize myBuffer with MY_BUFFER_SIZE bytes of valid data.

    numBytes = 0;
    while( numBytes < MY_BUFFER_SIZE );
    {
        if( !(DRV_USART_TRANSFER_STATUS_TRANSMIT_FULL & DRV_USART_TransferStatus(myUSARTHandle)) )
        {
            DRV_USART_WriteByte(myUSARTHandle, myBuffer[numBytes++]);
        }

        // Do something else...
    }
    </code>

  Remarks:
    This function is thread safe when called in a RTOS application. Note that
    DRV_USART_WriteByte and DRV_USART_ReadByte function cannot co-exist with
    DRV_USART_BufferAddRead, DRV_USART_BufferAddWrite, DRV_USART_Read and
    DRV_USART_Write functions in a application. Calling the
    DRV_USART_ReadByte and DRV_USART_WriteByte function will disrupt the
    processing of any queued buffers.
*/

void DRV_USART_WriteByte( const DRV_HANDLE handle, const uint8_t byte);

// *****************************************************************************
/* Function:
     unsigned int DRV_USART_ReceiverBufferSizeGet( const DRV_HANDLE handle )

  Summary:
    Returns the size of the receive buffer.
    <p><b>Implementation:</b> Static/Dynamic</p>

  Description:
    This routine returns the size of the receive buffer.

  Precondition:
    The DRV_USART_Initialize routine must have been called for the specified
    USART driver instance.

    DRV_USART_Open must have been called to obtain a valid opened device handle.

  Parameters:
    handle       - A valid open-instance handle, returned from the driver's
                   open routine

  Returns:
    Size of the driver's receive buffer, in bytes.

  Example:
    <code>
    DRV_HANDLE      myUSARTHandle;    // Returned from DRV_USART_Open
    const uint8_t   readBuffer[5];
    unsigned int    size, numBytes = 0;
    unsigned int    readbufferLen = sizeof(readBuffer);

    size     = DRV_USART_ReceiverBufferSizeGet(myUSARTHandle);

    // Do something based on the receiver buffer size

    </code>

  Remarks:
    Does not account for client queued buffers. This function is thread safe
    when called in a RTOS application.
*/

unsigned int DRV_USART_ReceiverBufferSizeGet( const DRV_HANDLE handle );

// *****************************************************************************
/* Function:
    unsigned int DRV_USART_TransmitBufferSizeGet ( const DRV_HANDLE handle )

  Summary:
    Returns the size of the transmit buffer.
    <p><b>Implementation:</b> Static/Dynamic</p>

  Description:
    This routine returns the size of the transmit buffer and can be used by the
    application to determine the number of bytes to write with the
    DRV_USART_WriteByte function.

  Precondition:
    The DRV_USART_Initialize routine must have been called for the specified
    USART driver instance.

    DRV_USART_Open must have been called to obtain a valid opened device handle.

  Parameters:
    handle       - A valid open-instance handle, returned from the driver's
                   open routine

  Returns:
    Size of the driver's transmit buffer, in bytes.

  Examples:
    <code>

    DRV_HANDLE      myUSARTHandle;    // Returned from DRV_USART_Open
    const uint8_t   writeBuffer[5];
    unsigned int    size, numBytes = 0;
    unsigned int    writeBufferLen = sizeof(writeBuffer);

    size     = DRV_USART_TransmitBufferSizeGet (myUSARTHandle);

    // Do something based on the transmitter buffer size

    </code>

  Remarks:
    Does not account for client queued buffers. This function is thread safe
    when used in a RTOS application.
*/

unsigned int DRV_USART_TransmitBufferSizeGet( const DRV_HANDLE handle );

// *****************************************************************************
/* Function:
    bool DRV_USART_ReceiverBufferIsEmpty( const DRV_HANDLE handle )

  Summary:
    Provides the status of the driver's receive buffer.
    <p><b>Implementation:</b> Static/Dynamic</p>

  Description:
    This routine indicates if the driver's receiver buffer is empty. This
    function can be used in conjunction with the DRV_USART_Read and
    DRV_USART_ReadByte functions.

  Precondition:
    The DRV_USART_Initialize routine must have been called for the specified
    USART driver instance.

    DRV_USART_Open must have been called to obtain a valid opened device handle.

  Parameters:
    handle       - A valid open-instance handle, returned from the driver's
                   open routine

  Returns:
    true    - if the driver's receive buffer is empty

    false   - if the driver's receive buffer is not empty

  Example:
    <code>
    DRV_HANDLE               myUSARTHandle;    // Returned from DRV_USART_Open
    char                     myBuffer[MY_BUFFER_SIZE];
    unsigned int             numBytes;

    numBytes = 0;
    while( numBytes < MY_BUFFER_SIZE );
    {
        if ( !DRV_USART_ReceiverBufferIsEmpty(myUSARTHandle) )
        {
            if( numBytes < MY_BUFFER_SIZE )
            {
                myBuffer[numBytes++] = DRV_USART_ReadByte (myUSARTHandle);
            }
            else
            {
                break;
            }
        }

        // Do something else while more data is received.
    }
    </code>

  Remarks:
    Does not account for client queued buffers. This function is safe thread
    safe when used in a RTOS application.
*/

bool DRV_USART_ReceiverBufferIsEmpty( const DRV_HANDLE handle );

// *****************************************************************************
/* Function:
    bool DRV_USART_TransmitBufferIsFull( const DRV_HANDLE handle )

  Summary:
    Provides the status of the driver's transmit buffer.
    <p><b>Implementation:</b> Static/Dynamic</p>

  Description:
    This routine identifies if the driver's transmit buffer is full or not. This
    function can be used in conjunction with the DRV_USART_Write and
    DRV_USART_WriteByte functions.

  Precondition:
    The DRV_USART_Initialize routine must have been called for the specified
    USART driver instance.

    DRV_USART_Open must have been called to obtain a valid opened device handle.

  Parameters:
    handle       - A valid open-instance handle, returned from the driver's
                   open routine

  Returns:
    true    - if the transmit buffer is full

    false   - if the transmit buffer is not full

  Example:
    <code>
    DRV_HANDLE      myUSARTHandle;    // Returned from DRV_USART_Open
    unsigned int    numBytes;
    int             bytesToWrite;
    const uint8_t   writeBuffer[35] = "1234567890ABCDEFGHIJKLMNOP\n" ;
    int             writebufferLen = strlen((char *)writeBuffer);

    numBytes = 0;
    while( numBytes < writebufferLen )
    {
        if (DRV_USART_TransmitBufferisFull())
        {
            // Do something else until there is some room in the driver's Transmit buffer.
        }
        else
        {
            DRV_USART_WriteByte(myUSARTHandle, writeBuffer[numBytes++]);
        }
    }
    </code>

  Remarks:
    Does not account for client queued buffers. This function is thread safe
    when called in a RTOS application.
*/

bool DRV_USART_TransmitBufferIsFull( const DRV_HANDLE handle );

// *****************************************************************************
/* Function:
    void DRV_USART_ByteTransmitCallbackSet
    (
        const SYS_MODULE_INDEX index,
        const DRV_USART_BYTE_EVENT_HANDLER eventHandler
    )

  Summary:
    Registers a callback function for byte transmit event.

  Description:
    This function allows a transmit callback function to be registered with the
    driver. The callback function is invoked when a byte has been transmitted
    using DRV_USART_WriteByte () function.

    The callback function should be registered with the driver prior to any
    writes to the driver. The callback functionality is available only in the
    interrupt mode of operation. The driver clears the interrupt after invoking
    the callback function.

  Precondition:
    The DRV_USART_Initialize routine must have been called for the specified
    USART driver instance.

  Parameters:
    index        - Identifier for the object instance to be opened

    eventHandler - Pointer to the event handler function.

  Returns:
    None.

  Example:
    <code>
    // myAppObj is an application specific state data object.
    MY_APP_OBJ myAppObj;

    uint8_t mybuffer[MY_BUFFER_SIZE];

    // myUSARTHandle is the handle returned by the DRV_USART_Open function.
    myUSARTHandle = DRV_USART_Open(DRV_USART_INDEX_0, DRV_IO_INTENT_EXCLUSIVE);
                                     (uintptr_t)&myAppObj );

    // Register an event handler with driver. This is done once
    DRV_USART_ByteTransmitCallbackSet (DRV_USART_INDEX_0, APP_USARTTransmitEventHandler);

    DRV_USART_WriteByte (myUSARThandle, myBuffer[0]);

    // Event Processing Technique. Event is received when
    // the byte is transmitted.

    void APP_USARTTransmitEventHandler (const SYS_MODULE_INDEX index)
    {
        // Byte has been transmitted. Handle the event.
    }
    </code>

  Remarks:
    None
*/

void DRV_USART_ByteTransmitCallbackSet
(
    const SYS_MODULE_INDEX index,
    const DRV_USART_BYTE_EVENT_HANDLER eventHandler
);

// *****************************************************************************
/* Function:
    void DRV_USART_ByteReceiveCallbackSet
    (
        const SYS_MODULE_INDEX index,
        const DRV_USART_BYTE_EVENT_HANDLER eventHandler
    )

  Summary:
    Registers receive callback function for byte receive event.

  Description:
    This function allows a receive callback function to be registered with the
    driver. The callback function is invoked when a byte has been received. The
    received byte can then be read using DRV_USART_ReadByte() function.

    The callback function should be registered with the driver as part of the
    initialization. The callback functionality is available only in the
    interrupt mode of operation. The driver clears the interrupt after invoking
    the callback function.

  Precondition:
    The DRV_USART_Initialize routine must have been called for the specified
    USART driver instance.

  Parameters:
    index        - Identifier for the object instance to be opened

    eventHandler - Pointer to the event handler function.

  Returns:
    None.

  Example:
    <code>
    // myAppObj is an application specific state data object.
    MY_APP_OBJ myAppObj;

    uint8_t mybuffer[MY_BUFFER_SIZE];

    // myUSARTHandle is the handle returned by the DRV_USART_Open function.
    myUSARTHandle = DRV_USART_Open(DRV_USART_INDEX_0, DRV_IO_INTENT_EXCLUSIVE);
                                     (uintptr_t)&myAppObj );

    // Register an event handler with driver. This is done once
    DRV_USART_ByteReceiveCallbackSet(DRV_USART_INDEX_0, APP_USARTReceiveEventHandler);

    // Event Processing Technique. Event is received when
    // a byte is received.

    void APP_USARTReceiveEventHandler(const SYS_MODULE_INDEX index)
    {
        // Byte has been Received. Handle the event.
        // Read byte using DRV_USART_ReadByte ()
        // DRV_USART_ReceiverBufferIsEmpty() function can be used to
        // check if the receiver buffer is empty.
    }
    </code>

  Remarks:
    None
*/

void DRV_USART_ByteReceiveCallbackSet
(
    const SYS_MODULE_INDEX index,
    const DRV_USART_BYTE_EVENT_HANDLER eventHandler
);

// *****************************************************************************
/* Function:
    void DRV_USART_ByteErrorCallbackSet
    (
        const SYS_MODULE_INDEX index,
        const DRV_USART_BYTE_EVENT_HANDLER eventHandler
    )

  Summary:
    Registers callback to handle for byte error events.

  Description:
    This function allows a callback function to be registered with the driver
    to handle the error events occurring in the transmit/receive path during
    byte transfers.

    The callback function should be registered as part of the initialization.
    The callback functionality is available only in the interrupt mode of
    operation. The driver clears the interrupt after invoking the callback
    function.

  Precondition:
    The DRV_USART_Initialize routine must have been called for the specified
    USART driver instance.

  Parameters:
    index        - Identifier for the object instance to be opened

    eventHandler - Pointer to the event handler function.

  Returns:
    None.

  Example:
    <code>
    // myAppObj is an application specific state data object.
    MY_APP_OBJ myAppObj;

    uint8_t mybuffer[MY_BUFFER_SIZE];

    // myUSARTHandle is the handle returned by the DRV_USART_Open function.
    myUSARTHandle = DRV_USART_Open(DRV_USART_INDEX_0, DRV_IO_INTENT_EXCLUSIVE);
                                     (uintptr_t)&myAppObj );

    // Register an event handler with driver. This is done once
    DRV_USART_ByteErrorCallbackSet (DRV_USART_INDEX_0, APP_USARTErrorEventHandler);

    // Event Processing Technique.
    void APP_USARTErrorEventHandler(const SYS_MODULE_INDEX index)
    {
        // Error has occurred. Handle the event.
    }
    </code>

  Remarks:
    None
*/

void DRV_USART_ByteErrorCallbackSet
(
    const SYS_MODULE_INDEX index,
    const DRV_USART_BYTE_EVENT_HANDLER eventHandler
);

// *****************************************************************************
// *****************************************************************************
// Section: USART Driver Setup and Status Routines
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    DRV_USART_ERROR DRV_USART_ErrorGet(DRV_HANDLE client);

  Summary:
    This function returns the error(if any) associated with the last client
    request.
    <p><b>Implementation:</b> Static/Dynamic</p>

  Description:
    This function returns the error(if any) associated with the last client
    request. DRV_USART_Read and DRV_USART_Write will update the client
    error status when these functions return DRV_USART_TRANSFER_ERROR.  If the
    driver send a DRV_USART_BUFFER_EVENT_ERROR to the client, the client can
    call this function to know the error cause. The error status will be updated
    on every operation and should be read frequently (ideally immediately after
    the driver operation has completed) to know the relevant error status.

  Precondition:
    The DRV_USART_Initialize routine must have been called for the specified
    USART driver instance.

    DRV_USART_Open must have been called to obtain a valid opened device handle.

  Parameters:
    bufferhandle    - Handle of the buffer of which the processed number of bytes
                      to be obtained.

  Returns:
    A DRV_USART_ERROR type indicating last known error status.

  Example:
    <code>
    // myAppObj is an application specific object.
    MY_APP_OBJ myAppObj;

    uint8_t mybuffer[MY_BUFFER_SIZE];
    DRV_BUFFER_HANDLE bufferHandle;

    // myUSARTHandle is the handle returned
    // by the DRV_USART_Open function.

    // Client registers an event handler with driver. This is done once.

    DRV_USART_BufferEventHandlerSet( myUSARTHandle, APP_USARTBufferEventHandle,
                                     (uintptr_t)&myAppObj );

    bufferHandle = DRV_USART_BufferAddRead( myUSARThandle,
                                            myBuffer, MY_BUFFER_SIZE );

    if(DRV_USART_BUFFER_HANDLE_INVALID == bufferHandle)
    {
        // Error handling here
    }

    // Event Processing Technique. Event is received when
    // the buffer is processed.

    void APP_USARTBufferEventHandler( DRV_USART_BUFFER_EVENT event,
            DRV_USART_BUFFER_HANDLE bufferHandle, uintptr_t contextHandle )
    {
        // The context handle was set to an application specific
        // object. It is now retrievable easily in the event handler.
        MY_APP_OBJ myAppObj = (MY_APP_OBJ *) contextHandle;
        size_t processedBytes;

        switch(event)
        {
            case DRV_USART_BUFFER_EVENT_SUCCESS:

                // This means the data was transferred.
                break;

            case DRV_USART_BUFFER_EVENT_FAILURE:

                // Error handling here.
                // We can find out how many bytes were processed in this
                // buffer before the error occurred. We can also find
                // the error cause.

                processedBytes = DRV_USART_BufferCompletedBytesGet(bufferHandle);
                if(DRV_USART_ERROR_RECEIVE_OVERRUN == DRV_USART_ErrorGet(myUSARTHandle))
                {
                    // There was an receive over flow error.
                    // Do error handling here.
                }

                break;

            default:
                break;
        }
    }
    </code>

  Remarks:
    It is the client's responsibility to make sure that the error status is
    obtained frequently. The driver will update the client error status
    regardless of whether this has been examined by the client. This function
    is thread safe when used in a RTOS application.
*/

DRV_USART_ERROR DRV_USART_ErrorGet(const DRV_HANDLE client);

// *****************************************************************************
/* Function:
    void DRV_USART_BaudSet(DRV_HANDLE client, uint32_t baud);

  Summary:
    This function changes the USART module baud to the specified value.
    <p><b>Implementation:</b> Static/Dynamic</p>

  Description:
    This function changes the USART module baud to the specified value. Any
    queued buffer requests will be processed at the updated baud. The USART
    driver operates at the baud specified in DRV_USART_Initialize function
    unless the DRV_USART_BaudSet function is called to change the baud.

  Precondition:
    The DRV_USART_Initialize routine must have been called for the specified
    USART driver instance.

    DRV_USART_Open must have been called to obtain a valid opened device handle.

  Parameters:
    handle  - client handle returned by DRV_USART_Open function.

    baud    - desired baud.

  Returns:
    None.

  Example:
    <code>

    // myUSARTHandle is the handle returned
    // by the DRV_USART_Open function.

    DRV_USART_BaudSet(myUSARTHandle, 9600);

    </code>

  Remarks:
    The implementation of this function, in this release of the driver, changes
    the baud immediately. This may interrupt on-going data transfer. It is
    recommended that the driver be opened exclusively if this function is to be
    called. This function is thread safe when used in a RTOS application.
*/

DRV_USART_BAUD_SET_RESULT DRV_USART_BaudSet
(
    const DRV_HANDLE client,
    uint32_t baud
);

// *****************************************************************************
/* Function:
    void DRV_USART_LineControlSet
    (
        DRV_HANDLE client,
        DRV_USART_LINE_CONTROL lineControl
    );

  Summary:
    This function changes the USART module line control to the specified value.
    <p><b>Implementation:</b> Static/Dynamic</p>

  Description:
    This function changes the USART module line control parameters to the
    specified value.  Any queued buffer requests will be processed at the
    updated line control parameters. The USART driver operates at the line
    control parameters specified in DRV_USART_Initialize function unless the
    DRV_USART_LineControlSet function is called to change the line control
    parameters.

  Precondition:
    The DRV_USART_Initialize routine must have been called for the specified
    USART driver instance.

    DRV_USART_Open must have been called to obtain a valid opened device handle.

  Parameters:
    handle  - client handle returned by DRV_USART_Open function.

    lineControl - line control parameters.

  Returns:
    DRV_USART_LINE_CONTROL_SET_SUCCESS if the function was successful. Returns
    DRV_HANDLE_INVALID if the client handle is not valid.

  Example:
    <code>

    // myUSARTHandle is the handle returned
    // by the DRV_USART_Open function.

    DRV_USART_LineControlSet(myUSARTHandle, DRV_USART_LINE_CONTROL_8NONE1);

    </code>

  Remarks:
    The implementation of this function, in this release of the driver, changes
    the line control immediately. This may interrupt on-going data transfer. It
    is recommended that the driver be opened exclusively if this function is to
    be called. This function is thread safe when called in a RTOS application.
*/

DRV_USART_LINE_CONTROL_SET_RESULT DRV_USART_LineControlSet
(
    const DRV_HANDLE client,
    const DRV_USART_LINE_CONTROL lineControl
);

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END


#endif // #ifndef _DRV_USART_H
/*******************************************************************************
 End of File
*/

