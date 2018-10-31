/*******************************************************************************
  DMA System Service Library Interface Definition

  Company:
    Microchip Technology Inc.

  File Name:
    sys_dma.h

  Summary:
    DMA System Service.

  Description:
    This file contains the interface definition for the DMA System
    Service.  It provides a way to interact with the DMA subsystem to
    manage the data transfer between different peripherals and/or memory
    without intervention from the CPU.
*******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2013-2015 released Microchip Technology Inc.  All rights reserved.

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

#ifndef _SYS_DMA_H
#define _SYS_DMA_H


#if defined(__DEVICE_IS_PIC32C)
#include "system/dma/sys_dma_definitions_pic32c.h"
#else
#include "system/dma/sys_dma_definitions.h"
#endif

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: DMA System Service Interface Routines
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* DMA System service Transfer Event Handler Function

   Summary:
    Pointer to a DMA System service Transfer Event handler function.

   Description:
    This data type defines a DMA System service Transfer Event Handler Function.

    A DMA system service client must register an transfer event handler function of
    this type to receive transfer related events from the system service.

    If the event is SYS_DMA_TRANSFER_EVENT_COMPLETE, this means that the data
    was transferred successfully. The channelHandle parameter contains the channel
    handle of the channel on which the transfer was processed.

    If the event is SYS_DMA_TRANSFER_EVENT_ERROR, this means that the data was
    not transferred successfully. TThe channelHandle parameter contains the channel
    handle of the channel on which the transfer had failed.

    The contextHandle parameter contains the context handle that was provided by
    the client at the time of registering the event handler. This context handle
    can be anything that the client consider helpful or necessary to identify
    the client context object associated with the channel of the system
    service that generated the event.

    The event handler function executes in an interrupt context when DMA is setup
    to start operation by an interrupt trigger source. It is recommended of the
    application to not perform process intensive operations with in this
    function.

   Remarks:
    None.

*/
typedef void (*SYS_DMA_CHANNEL_TRANSFER_EVENT_HANDLER) (SYS_DMA_TRANSFER_EVENT event,
        SYS_DMA_CHANNEL_HANDLE handle, uintptr_t contextHandle);


// *****************************************************************************
// *****************************************************************************
// Section: Initialization and Task Functions
// *****************************************************************************
// *****************************************************************************

//******************************************************************************
/* Function:
    SYS_MODULE_OBJ SYS_DMA_Initialize(const SYS_MODULE_INIT * const init)

  Summary:
    Initializes and Enables the DMA Controller.

  Description:
    This function Enables the DMA module. Enable/Disable stop in idle mode
    feature based on the passed parameter value.

    This function initializes the DMA module making it ready for clients to
    open and use it. The initialization data is specified by the init parameter.


  Precondition:
    None.

  Parameters:
    init            - Pointer to the data structure containing any data
                      necessary to initialize the hardware. This pointer may
                      be null if no data is required and default
                      initialization is to be used.

  Returns:
    If successful, returns a valid handle to the DMA module object.
    Otherwise, it returns SYS_MODULE_OBJ_INVALID.

  Example:
  <code>
    SYS_MODULE_OBJ objectHandle;
    SYS_DMA_INIT initParam;

    initParam.sidl = SYS_DMA_SIDL_ENABLE;
    objectHandle = SYS_DMA_Initialize(DRV_I2S_INDEX_1,
                                      (SYS_MODULE_INIT*)initParam);
    if (SYS_MODULE_OBJ_INVALID == objectHandle)
    {
        // Handle error
    }
  </code>

  Remarks:
    This function must be called before any other DMA systems service routines
    are called.

    Not all features are available on all micro-controllers.
*/
SYS_MODULE_OBJ SYS_DMA_Initialize(const SYS_MODULE_INIT * const init);

//*******************************************************************************
/* Function:
    void SYS_DMA_Tasks(SYS_MODULE_OBJ object)

  Summary:
    Maintains the system service's state machine.

  Description:
    This function is used to maintain the DMA system service's internal state machine.
    This function is specifically designed for non DMA interrupt trigger
    implementations(polling mode), and should be used only in polling mode.
    this function should be called from the SYS_Tasks() function.

  Precondition:
    DMA should have been initialized by calling SYS_DMA_Initialize.

  Parameters:
    object      - Object handle for the DMA module (returned from
                  SYS_DMA_Initialize)

  Returns:
    None.

  Example:
    <code>
    // 'object' Returned from SYS_DMA_Initialize

    while (true)
    {
        SYS_DMA_Tasks ((object) );

        // Do other tasks
    }
    </code>

  Remarks:
    This function is normally not called directly by an application.  It is
    called by the system's Tasks routine (SYS_Tasks).

    This function is deprecated. Any calls made to this functions will have no effect.
 */
void SYS_DMA_Tasks(SYS_MODULE_OBJ object);

//*******************************************************************************
/* Function:
    void SYS_DMA_TasksISR(SYS_MODULE_OBJ object, DMA_CHANNEL activeChannel)

  Summary:
    Maintains the system service's state machine and implements its ISR.
  Description:
    This function is used to maintain the DMA system service's internal state machine
    and implement its ISR for DMA interrupt trigger implementations(interrupt mode).
	This function is specifically designed for DMA interrupt trigger
    implementations (interrupt mode).
    In interrupt mode, this function should be called in the interrupt
    service routine of the DMA channel that is associated with this transfer.

  Precondition:
    DMA should have been initialized by calling SYS_DMA_Initialize.

  Parameters:
    object      - Object handle for the DMA module (returned from
                  SYS_DMA_Initialize)
    activeChannel - DMA channel number of the ISR being serviced.

  Returns:
    None.

  Example:
    <code>
    // 'object' Returned from SYS_DMA_Initialize

	void __ISR(_DMA3_VECTOR,ipl5) _InterruptHandler_BT_USART_RX_DMA_CHANNEL(void)
	{
		SYS_DMA_TasksISR(object, DMA_CHANNEL_3);
	}


    </code>

  Remarks:
    It is called by appropriate raw ISR.

    This function may execute in an ISR context and will never block or access any
    resources that may cause it to block.
 */
void SYS_DMA_TasksISR(SYS_MODULE_OBJ object, DMA_CHANNEL activeChannel);

//*****************************************************************************
/* Function:
    void SYS_DMA_TasksError(SYS_MODULE_OBJ object)

  Summary:
    Maintains the system service's error state machine.

  Description:
    This function is used to maintain the DMA system service's error state machine.
	This function is specifically designed for non DMA interrupt trigger
    implementations(polling mode), and should be used only in polling mode.
    this function should be called from the SYS_Tasks() function.

  Precondition:
    DMA should have been initialized by calling SYS_DMA_Initialize.

Parameters:
    object      - Object handle for the DMA module (returned from
                  SYS_DMA_Initialize)

  Returns:
    None.

  Example:
    <code>
    // 'object' Returned from SYS_DMA_Initialize

    while (true)
    {
        SYS_DMA_TasksError (object);
        // Do other tasks
    }
    </code>

  Remarks:
    This function is normally not called directly by an application.  It is
    called by the system's Tasks routine (SYS_Tasks).

 */
void SYS_DMA_TasksError(SYS_MODULE_OBJ object);

//*******************************************************************************
/* Function:
    void SYS_DMA_TasksErrorISR(SYS_MODULE_OBJ object, DMA_CHANNEL activeChannel)

  Summary:
    Maintains the system service's state machine and implements its ISR.

  Description:
    This function is used to maintain the DMA system service's internal error state machine
    and implement its ISR for DMA interrupt trigger implementations(interrupt mode).
	This function is specifically designed for DMA interrupt trigger
    implementations (interrupt mode).
    In interrupt mode, this function should be called in the interrupt
    service routine of the DMA channel that is associated with this transfer.

  Precondition:
    DMA should have been initialized by calling SYS_DMA_Initialize.

  Parameters:
    object      - Object handle for the DMA module (returned from
                  SYS_DMA_Initialize)
    activeChannel - DMA channel number of the ISR being serviced.

  Returns:
    None.

  Example:
    <code>
    // 'object' Returned from SYS_DMA_Initialize

	void __ISR(_DMA3_VECTOR,ipl5) _InterruptHandler_BT_USART_RX_DMA_CHANNEL(void)
	{
		// ....
		SYS_DMA_TasksErrorISR(object, DMA_CHANNEL_3);
	}


    </code>

  Remarks:
    It is called by appropriate raw ISR.

    This function may execute in an ISR context and will never block or access any
    resources that may cause it to block.
 */
void SYS_DMA_TasksErrorISR(SYS_MODULE_OBJ object, DMA_CHANNEL activeChannel);

// *****************************************************************************
// *****************************************************************************
// Section: Channel Setup and management functions
// *****************************************************************************
// *****************************************************************************

//******************************************************************************
/* Function:
    SYS_DMA_CHANNEL_HANDLE SYS_DMA_ChannelAllocate (DMA_CHANNEL channel)

  Summary:
    Allocates the specified DMA channel and returns a handle to it.

  Description:
    This function opens the specified DMA channel and provides a
    handle that must be provided to all other client-level operations to
    identify the caller and the DMA channel.

  Precondition:
    Function SYS_DMA_Initialize must have been called before calling this
    function.

  Parameters:
    channel     - Channel number requested for allocation.
                  When channel number specified is DMA_CHANNEL_ANY
                  a random channel is allocated for DMA transfers.

  Returns:
     The channel handle for the requested channel number.

     If an error occurs, the return value is SYS_DMA_CHANNEL_HANDLE_INVALID.
     Error can occur.
     - if the requested channel number is invalid.
     - if the requested channel number is not free.

  Example:
  <code>
    DMA_CHANNEL channel;
    SYS_DMA_CHANNEL_HANDLE handle

    channel = DMA_CHANNEL_2;
    handle = SYS_DMA_ChannelAllocate(channel);
    if (SYS_DMA_CHANNEL_HANDLE_INVALID == handle)
    {
        // Failed to allocate the channel
    }
    else
    {
        // Proceed with setting up the channel and adding the transfer
    }
  </code>

  Remarks:
    The handle returned is valid until the SYS_DMA_ChannelRelease routine is called.
    This function must be called before any other DMA channel Setup and management
    routines are called
*/
SYS_DMA_CHANNEL_HANDLE SYS_DMA_ChannelAllocate (DMA_CHANNEL channel);



//******************************************************************************
/* Function:
    void SYS_DMA_ChannelRelease(SYS_DMA_CHANNEL_HANDLE handle)

  Summary:
    Deallocates and frees the channel specified by the handle.

  Description:
    This function deallocates an allocated-channel of the DMA module,
    invalidating the handle.

  Precondition:
    DMA should have been initialized by calling SYS_DMA_Initialize.
    DMA channel should have been allocated by calling SYS_DMA_ChannelAllocate

  Parameters:
    handle         -  A valid allocated-channel handle, returned from the service's
                      Allocate routine

  Returns:
    None.

  Example:
  <code>
    DMA_CHANNEL channel;
    SYS_DMA_CHANNEL_HANDLE handle;

    channel = DMA_CHANNEL_2;
    handle = SYS_DMA_ChannelAllocate(channel);
    SYS_DMA_ChannelRelease(handle);
  </code>

  Remarks:
    After calling this routine, the handle passed in "handle" must not be used
    with any of the remaining service's routines.  A new handle must be obtained by
    calling SYS_DMA_ChannelAllocate before the caller may use the service again
*/
 void SYS_DMA_ChannelRelease(SYS_DMA_CHANNEL_HANDLE handle);


//******************************************************************************
/* Function:
    void SYS_DMA_ChannelSetup
    (
        SYS_DMA_CHANNEL_HANDLE handle,
        SYS_DMA_CHANNEL_OP_MODE modeEnable
        DMA_TRIGGER_SOURCE eventSrc
    )

  Summary:
    Setup the DMA channel parameters.

  Description:
    This function sets up the DMA channel parameters.
    It sets the channel priority and enables the mode of operations for the
    current system design.

  Precondition:
    DMA should have been initialized by calling SYS_DMA_Initialize.
    DMA channel should have been allocated by calling SYS_DMA_ChannelAllocate.

  Parameters:
    handle          -   Handle of the DMA channel as returned by the
                        SYS_DMA_ChannelAllocate function.
    priority        -   The priority to be associated to the channel.
    modeEnable      -   The supported operating modes to be enabled.
                        This parameter can be logically ORed to specify
                        multiple options.
    eventSrc        -   The event causing the cell transfer start.

  Returns:
    None.

  Example:
  <code>

     // 'handle' is a valid handle returned
     // by the SYS_DMA_ChannelAllocate function.

    SYS_DMA_CHANNEL_OP_MODE     modeEnable;
    DMA_TRIGGER_SOURCE          eventSrc;

    channel         =   DMA_CHANNEL_2;
    modeEnable      =   (SYS_DMA_CHANNEL_OP_MODE_BASIC | SYS_DMA_CHANNEL_OP_MODE_CRC);
    eventSrc        =   DMA_TRIGGER_USART_1_TRANSMIT;
     // Setup channel number, and enables basic and CRC mode
    SYS_DMA_ChannelSetup(handle, modeEnable,eventSrc);
  </code>

  Remarks:
    If SYS_DMA_CHANNEL_OP_MODE_MATCH_ABORT, SYS_DMA_CHANNEL_OP_MODE_CHAIN or
    SYS_DMA_CHANNEL_OP_MODE_CRC mode of operation is enabled, then corresponding
    mode setup API's needs to be called to set the related parameters.

    If the parameter 'eventSrc' is specified as DMA_TRIGGER_SOURCE_NONE then
    SYS_DMA_ChannelForceStart must be called to start the DMA channel transfer.

    Not all features are available on all microcontrollers.
  */

void SYS_DMA_ChannelSetup
(
    SYS_DMA_CHANNEL_HANDLE handle,
    SYS_DMA_CHANNEL_OP_MODE modeEnable,
    DMA_TRIGGER_SOURCE eventSrc
);


//******************************************************************************
/* Function:
    void SYS_DMA_ChannelSetupMatchAbortMode
    (
        SYS_DMA_CHANNEL_HANDLE handle,
        uint16_t pattern,
        DMA_PATTERN_LENGTH length,
        SYS_DMA_CHANNEL_IGNORE_MATCH ignore,
        uint8_t ignorePattern
    )

  Summary:
    Setup the pattern match abort mode.

  Description:
    This  function sets up the termination of DMA operation when the specified
    pattern is matched. Additionally on supported parts the function also
    sets up the ignoring of part of a pattern(8-bit) from match abort
    pattern(16-bit).

  Precondition:
    DMA should have been initialized by calling SYS_DMA_Initialize.
    DMA channel should have been allocated by calling SYS_DMA_ChannelAllocate.
    The function SYS_DMA_ChannelSetup must be called to enable
    SYS_DMA_CHANNEL_OP_MODE_MATCH_ABORT before setting up pattern
    match mode features.

  Parameters:
    handle          -   Handle of the DMA channel as returned by the
                        SYS_DMA_ChannelAllocate function.
    pattern         -  The pattern that needs to be matched to abort a DMA transfer.
    length          -  Match pattern length can be 1-byte or 2-byte.
    ignore          -  Enable/Disable a byte between a 2-byte pattern match.
    ignorePattern   -  The part of the pattern(8-bit) that needs to be ignored
                       from the match abort pattern(16-bit)

  Returns:
    None.

  Example:
  <code>
    // Example 1
    // The following code is for a device with an 8-bit pattern value and no
    // support for pattern match ignore feature

    // 'handle' is a valid handle returned
    // by the SYS_DMA_ChannelAllocate function.
    uint16_t                        pattern;
    DMA_PATTERN_LENGTH              length;
    SYS_DMA_CHANNEL_IGNORE_MATCH    ignore;
    uint8_t                         ignorePattern;

    pattern         = 0x00; //Stop transfer on detection of a NULL character
    length          = DMA_PATTERN_LENGTH_NONE;
    ignore          = SYS_DMA_CHANNEL_IGNORE_MATCH_DISABLE;
    ignorePattern   = 0;
    SYS_DMA_ChannelSetupMatchAbortMode(handle, pattern, length,
                                       ignoreEnable, ignorePattern);

    // Example 2
    // The following code is for a device with a 16-bit pattern value and
    // support for pattern match ignore feature

    // 'handle' is a valid handle returned
    // by the SYS_DMA_ChannelAllocate function.
    uint16_t                        pattern;
    DMA_PATTERN_LENGTH              length;
    SYS_DMA_CHANNEL_IGNORE_MATCH    ignore;
    uint8_t                         ignorePattern;

    pattern         = 0x0D0A; //Stop transfer on detection of '\r\n'
    length          = DMA_PATTERN_MATCH_LENGTH_2BYTES;
    ignore          = SYS_DMA_CHANNEL_IGNORE_MATCH_ENABLE;
    ignorePattern   = 0x00; \\ Any null character between the termination pattern
                            \\ '\r' and '\n' is ignored.
    SYS_DMA_ChannelSetupMatchAbortMode(handle, pattern, length,
                                            ignore, ignorePattern);
  </code>

  Remarks:
    The parameter 'pattern' (8-bit or 16-bit) is device-specific.
    Not all features are available on all devices.
    Refer to the specific device data sheet for details.
*/

void SYS_DMA_ChannelSetupMatchAbortMode
(
    SYS_DMA_CHANNEL_HANDLE handle,
    uint16_t pattern,
    DMA_PATTERN_LENGTH length,
    SYS_DMA_CHANNEL_IGNORE_MATCH ignore,
    uint8_t ignorePattern
);


//******************************************************************************
/* Function:
    void SYS_DMA_ChannelCRCSet
    (
        SYS_DMA_CHANNEL_HANDLE handle,
        SYS_DMA_CHANNEL_OPERATION_MODE_CRC crc
    )

  Summary:
    Sets up the CRC operation mode.

  Description:
    This function sets up the CRC computation features.

  Precondition:
    DMA should have been initialized by calling SYS_DMA_Initialize.
    DMA channel should have been allocated by calling SYS_DMA_ChannelAllocate.
    The function SYS_DMA_ChannelSetup must be called to enable
    SYS_DMA_CHANNEL_OP_MODE_CRC before setting up the CRC mode.

  Parameters:
    handle          -   Handle of the DMA channel as returned by the
                        SYS_DMA_ChannelAllocate function.
    crc.type        -   CRC will calculate an IP header checksum or an LFSR CRC.
    crc.mode        -   Compute the CRC in Background/Append mode.
    crc.polyLength  -   Denotes the length of the polynomial.
    crc.bitOrder    -   CRC is calculated LSb/MSb first.
    crc.byteOrder   -   Byte selection order input pre-CRC Generator
    crc.writeOrder  -   Write byte order selection post-CRC computation
    crc.data        -   Computed/Seed CRC
    crc.xorBitMask  -   Enable/Disable XOR bit mask on the corresponding bits
                        when mode is LFSR

  Returns:
    None.

  Example:
  <code>
    //Example 1
    // DMA calculation using the CRC background mode

    // 'handle' is a valid handle returned
    // by the SYS_DMA_ChannelAllocate function.
    SYS_DMA_CHANNEL_OPERATION_MODE_CRC  crc;

    crc.type            = DMA_CRC_LFSR;
    crc.mode            = SYS_DMA_CHANNEL_CRC_MODE_BACKGROUND;
    crc.polyLength      = 16;
    crc.bitOrder        = DMA_CRC_BIT_ORDER_LSB;
    crc.byteOrder       = DMA_CRC_BYTEORDER_NO_SWAPPING;
    crc.writeOrder      = SYS_DMA_CRC_WRITE_ORDER_MAINTAIN;
    crc.data            = 0xFFFF;
    crc.xorBitMask      = 0x1021;
    SYS_DMA_ChannelCRCSet(handle, crc);

  </code>

  Remarks:
    This feature may not be available on all devices. Refer to the specific device
	data sheet to determine availability.
*/

void SYS_DMA_ChannelCRCSet
(
    SYS_DMA_CHANNEL_HANDLE handle,
    SYS_DMA_CHANNEL_OPERATION_MODE_CRC crc
);


//******************************************************************************
/* Function:
    uint32_t SYS_DMA_ChannelCRCGet(void)

  Summary:
    Returns the computed CRC.

  Description:
    This  function returns the computed CRC

  Precondition:
    DMA should have been initialized by calling SYS_DMA_Initialize.
    DMA channel should have been allocated by calling SYS_DMA_ChannelAllocate.
    The function SYS_DMA_ChannelSetup must be called to enable
    SYS_DMA_CHANNEL_OP_MODE_CRC before setting up the CRC mode.
    The CRC generator must have been previously setup using
    SYS_DMA_ChannelCRCSet.

  Parameters:
    None

  Returns:
    uint32_t    - The Computed CRC.

  Example:
  <code>
    uint32_t                    computedCRC;

    computedCRC     = SYS_DMA_ChannelCRCGet();
  </code>

  Remarks:
    To get the computed CRC value this function must be called after the block
    transfer completion event (i.e., after getting and processing the callback
    registered with SYS_DMA_ChannelTransferEventHandlerSet).

    This feature may not be available on all devices. Refer to the specific device
	data sheet to determine availability.
*/

uint32_t SYS_DMA_ChannelCRCGet(void);


//******************************************************************************
/* Function:
    void SYS_DMA_ChannelEnable(SYS_DMA_CHANNEL_HANDLE handle)

  Summary:
    Enables a channel.

  Description:
    This function enables a channel.

  Precondition:
    DMA should have been initialized by calling SYS_DMA_Initialize.
    DMA channel should have been allocated by calling SYS_DMA_ChannelAllocate.
    The function SYS_DMA_ChannelSetup must have been called to setup and
    enable the required features.

  Parameters:
    handle          -   Handle of the DMA channel as returned by the
                        SYS_DMA_ChannelAllocate function.

  Returns:
    None.

  Example:
  <code>
    // 'handle' is a valid handle returned
    // by the SYS_DMA_ChannelAllocate function.

    SYS_DMA_ChannelEnable(handle);
  </code>

  Remarks:
        This function may not required to be called when starting DMA setup
        (by SYS_DMA_ChannelSetup) and transfer Add (by SYS_DMA_ChannelTransferAdd).
        But may be needed to be called in the registered callback to enable the
        channel and continue the data transfer with the existing transfer parameters
        previously set with 'SYS_DMA_ChannelTransferAdd'.
        The DMA channel is by default disabled on the completion of block
        transfer(callback generated)
*/

void SYS_DMA_ChannelEnable(SYS_DMA_CHANNEL_HANDLE handle);


//******************************************************************************
/* Function:
    void SYS_DMA_ChannelDisable(SYS_DMA_CHANNEL_HANDLE handle)

  Summary:
    Disables a channel.

  Description:
    This function disables a channel.

  Precondition:
    DMA should have been initialized by calling SYS_DMA_Initialize.
    DMA channel should have been allocated by calling SYS_DMA_ChannelAllocate.
    The function SYS_DMA_ChannelSetup must have been called to setup and
    enable the required features.
    A DMA channel should have been enabled either by calling
    'SYS_DMA_ChannelTransferAdd' or 'SYS_DMA_ChannelEnable'

  Parameters:
    handle          -   Handle of the DMA channel as returned by the
                        SYS_DMA_ChannelAllocate function.
  Returns:
    None.

  Example:
  <code>
    // 'handle' is a valid handle returned
    // by the SYS_DMA_ChannelAllocate function.

    SYS_DMA_ChannelDisable(handle);
  </code>

  Remarks:
    None.
*/

void SYS_DMA_ChannelDisable(SYS_DMA_CHANNEL_HANDLE handle);


//******************************************************************************
/* Function:
    void SYS_DMA_ChannelTransferAdd
    (
        SYS_DMA_CHANNEL_HANDLE handle,
        const void *srcAddr, size_t srcSize
        const void *destAddr, size_t destSize,
        size_t cellSize
    )

  Summary:
    Adds a data transfer to a DMA channel and Enables the channel to start
    data transfer.

  Description:
    This function adds a data transfer characteristics for a DMA channel. The
    The source and the destination addresses, source and destination lengths,
    The number of bytes transferred per cell event are set. It also enables
    the channel to start data transfer.

    If the requesting client registered an event callback with the service,
    the service will issue a SYS_DMA_TRANSFER_EVENT_COMPLETE or
    SYS_DMA_TRANSFER_EVENT_ABORT  event if the transfered was processed
    successfully of SYS_DMA_TRANSFER_EVENT_ERROR event if the transfer was not
    processed successfully.

  Precondition:
    DMA should have been initialized by calling SYS_DMA_Initialize.
    DMA channel should have been allocated by calling SYS_DMA_ChannelAllocate.
    The function SYS_DMA_ChannelSetup must have been called to setup and
    enable the required features.

  Parameters:
     handle          - Handle of the DMA channel as returned by the
                       SYS_DMA_ChannelAllocate function.
     srcAddr         - Source of the DMA transfer
     srcSize         - Size of the source
     destAddr        - Destination of the DMA transfer
     destSize        - Size of the destination
     cellSize        - Size of the cell

  Returns:
    None.

  Example:
    <code>
    // Add 10 bytes of data transfer to UART TX

    // 'handle' is a valid handle returned
    // by the SYS_DMA_ChannelAllocate function.
    MY_APP_OBJ          myAppObj;
    uint8_t             buf[10];
    void                *srcAddr;
    void                *destAddr;
    size_t              srcSize;
    size_t              destSize;
    size_t              cellSize;

    srcAddr     = (uint8_t *) buf;
    srcSize     = 10;
    destAddr    = (uin8_t*) &U2TXREG;
    destSize    = 1;
    cellSize    = 1;

    // User registers an event handler with system service. This is done once

    SYS_DMA_ChannelTransferEventHandlerSet(handle, APP_DMASYSTransferEventHandler,
                                                            (uintptr_t)&myAppObj);

    SYS_DMA_ChannelTransferAdd(handle,srcAddr,srcSize,destAddr,destSize,cellSize);

    if(SYS_DMA_CHANNEL_HANDLE_INVALID == handle)
    {
        // Error handling here
    }

    // Event Processing Technique. Event is received when
    // the transfer is processed.

    void APP_DMASYSTransferEventHandler(SYS_DMA_TRANSFER_EVENT event,
            SYS_DMA_CHANNEL_HANDLE handle, uintptr_t contextHandle)
    {
        switch(event)
        {
            case SYS_DMA_TRANSFER_EVENT_COMPLETE:

                // This means the data was transferred.
                break;

            case SYS_DMA_TRANSFER_EVENT_ERROR:

                // Error handling here.
                break;

            default:
                break;
        }
    }
    </code>

  Remarks:
    For PIC32MZ series of devices, if the source/destination addresses are
    RAM memory addresses, the the source/destination buffers
    should be made coherent to avoid the cache coherency issues.
    For example:
	<code>
    uint8_t buffer[1024];
    // The buffer can be made coherent by adding the 'coherent' attribute, as follows:
    uint8_t __attribute__((coherent)) __attribute__((aligned(16))) buffer[1024];
	</code>
*/

void SYS_DMA_ChannelTransferAdd
(
    SYS_DMA_CHANNEL_HANDLE handle,
    const void *srcAddr, size_t srcSize,
    const void *destAddr, size_t destSize,
    size_t cellSize
);

//******************************************************************************
/* Function:
    void SYS_DMA_ChannelTransferSet
    (
        SYS_DMA_CHANNEL_HANDLE handle,
        const void *srcAddr, size_t srcSize
        const void *destAddr, size_t destSize,
        size_t cellSize
    )

  Summary:
    Sets up a data transfer to a DMA channel.

  Description:
    This function sets up data transfer characteristics for a DMA channel. The
    The source and the destination addresses, source and destination lengths,
    The number of bytes transferred per cell event are set.
    This function does not enables the DMA channel. The channel has to be explicitly
    enabled to start the data transfer.
    The above functions could be used in situations where in the user intends to
    setup transfer parameters but do not intend to enable the channel immediately.
    For example to chain to DMA channels in a cyclic order where the channels remains
    disabled. The channel is enabled automatically by the DMA when the transfer trigger
    condition occurs.

  Precondition:
    DMA should have been initialized by calling SYS_DMA_Initialize.
    DMA channel should have been allocated by calling SYS_DMA_ChannelAllocate.
    The function SYS_DMA_ChannelSetup must have been called to setup and
    enable the required features.

  Parameters:
     handle          - Handle of the DMA channel as returned by the
                       SYS_DMA_ChannelAllocate function.
     srcAddr         - Source of the DMA transfer
     srcSize         - Size of the source
     destAddr        - Destination of the DMA transfer
     destSize        - Size of the destination
     cellSize        - Size of the cell

  Returns:
    None.

  Example:
    <code>
    // Set up 10 bytes of data transfer to UART TX

    // 'handle' is a valid handle returned
    // by the SYS_DMA_ChannelAllocate function.
    MY_APP_OBJ          myAppObj;
    uint8_t             buf[10];
    void                *srcAddr;
    void                *destAddr;
    size_t              srcSize;
    size_t              destSize;
    size_t              cellSize;

    srcAddr     = (uint8_t *) buf;
    srcSize     = 10;
    destAddr    = (uin8_t*) &U2TXREG;
    destSize    = 1;
    cellSize    = 1;

    // User registers an event handler with system service. This is done once

    SYS_DMA_ChannelTransferEventHandlerSet(handle, APP_DMASYSTransferEventHandler,
                                                            (uintptr_t)&myAppObj);

    SYS_DMA_ChannelTransferSet(handle,srcAddr,srcSize,destAddr,destSize,cellSize);
    SYS_DMA_ChannelEnable(handle);

    if(SYS_DMA_CHANNEL_HANDLE_INVALID == handle)
    {
        // Error handling here
    }

    // Event Processing Technique. Event is received when
    // the transfer is processed.

    void APP_DMASYSTransferEventHandler(SYS_DMA_TRANSFER_EVENT event,
            SYS_DMA_CHANNEL_HANDLE handle, uintptr_t contextHandle)
    {
        switch(event)
        {
            case SYS_DMA_TRANSFER_EVENT_COMPLETE:

                // This means the data was transferred.
                break;

            case SYS_DMA_TRANSFER_EVENT_ERROR:

                // Error handling here.
                break;

            default:
                break;
        }
    }
    </code>

  Remarks:
    For PIC32MZ series of devices, if the source/destination addresses are
    RAM memory addresses, the the source/destination buffers
    should be made coherent to avoid the cache coherency issues.
    For example:
	<code>
    uint8_t buffer[1024];
    // The buffer can be made coherent by adding the 'coherent' attribute, as follows:
    uint8_t __attribute__((coherent)) __attribute__((aligned(16))) buffer[1024];
	</code>
*/
void SYS_DMA_ChannelTransferSet
(
    SYS_DMA_CHANNEL_HANDLE handle,
    const void *srcAddr, size_t srcSize,
    const void *destAddr, size_t destSize,
    size_t cellSize
);

//******************************************************************************
/* Function:
    void SYS_DMA_ChannelDataWidthSet
    (
        SYS_DMA_CHANNEL_HANDLE handle,
        SYS_DMA_DATA_WIDTH dataWidth
    )

  Summary:
    Sets up data width for the specified channel.

  Description:
    This function sets up data width for the specified channel during
    linked list operation.

  Precondition:
    DMA should have been initialized by calling SYS_DMA_Initialize.
    DMA channel should have been allocated by calling SYS_DMA_ChannelAllocate.
    The function SYS_DMA_ChannelSetup must have been called to setup and
    enable the required features.

  Parameters:
     handle          - Handle of the DMA channel as returned by the
                       SYS_DMA_ChannelAllocate function.
     dataWidth       - Value from the set SYS_DMA_DATA_WIDTH

  Returns :
     None.

  Example:
    <code>
    SYS_DMA_ChannelDataWidthSetup(appData.channelHandle,
                    SYS_DMA_DATA_WIDTH_BYTE);
    </code>

  Remarks:
    This is supported by PIC32C devices only.
*/
void SYS_DMA_ChannelDataWidthSet
(
    SYS_DMA_CHANNEL_HANDLE handle,
    SYS_DMA_DATA_WIDTH dataWidth
);

//******************************************************************************
/* Function:
    void SYS_DMA_ChannelLinkedListAdd
    (
        SYS_DMA_CHANNEL_HANDLE handle,
        SYS_DMA_DESCRIPTOR_SETUP *firstDescriptorControl,
        uint32_t firstDescriptorAddr
    )

  Summary:
    Sets up a multi-block data transfer on a specified DMA channel using
    linked list feature.

  Description:
    This function sets up multi-block data transfer for a DMA channel by
    linked list operation mode. That is each block of data to be transferred
    is configured as desciptors of a linked list.
    Each desciptor can be defined in any available descriptor view formats and
    the views are avaibale as SYS_DMA_DESCRIPTOR_VIEW_x.

    Specified DMA channel will be enabled by default upon linked list addition.
    Upon each block transfer completion SYS_DMA task will call event handler if
    event handler is registered through SYS_DMA_ChannelTransferEventHandlerSet.

  Precondition:
    DMA should have been initialized by calling SYS_DMA_Initialize.
    DMA channel should have been allocated by calling SYS_DMA_ChannelAllocate.
    The function SYS_DMA_ChannelSetup must have been called to setup and
    enable the required features( Not mandatory for view 2 and 3).
    Data width should have been configured using SYS_DMA_ChannelDataWidthSet.

  Parameters:
    handle                  - Handle of the DMA channel as returned by the
                               SYS_DMA_ChannelAllocate function.
    firstDescriptorControl  - Control settings for the first descriptor.
                               This must be word aligned.
                               And need not to define "blockDataLength" member
                               of SYS_DMA_DESCRIPTOR_SETUP here.
    firstDescriptorAddr     - Address of the first descriptor.
                               All descriptors must be defined in the view
                               formats available as SYS_DMA_DESCRIPTOR_VIEW_x
                               and must be word aligned.

  Returns :
    None.

  Example:
    <code>
    // Add 2 blocks of data transfer to UART TX, each 10 bytes.

    //SYS_DMA channel handle
    SYS_DMA_CHANNEL_HANDLE handle;

    //2 blocks of data
    char src1[11] = {"1234567890"};
    char src2[11] = {"ABCDEFGHIJ"};

    //Word aligned first descriptor control
    __attribute__((__aligned__(4))) SYS_DMA_DESCRIPTOR_CONTROL descptr_crtl =
    {
        .mbr_ubc_type.nextDescriptorDestUpdate = 1,
        .mbr_ubc_type.nextDescriptorFetchEnable = 1,
        .mbr_ubc_type.nextDescriptorSrcUpdate = 1,
        .mbr_ubc_type.nextDescriptorView = 1
    };

    //Word aligned linked list descriptors in a common view( view 1)
    __attribute__((__aligned__(4))) SYS_DMA_DESCRIPTOR_VIEW_1 lld[2] =
    {
        {
            .mbr_nda = (uint32_t)&lld[1],
            .mbr_da = (uint32_t)&USART0->US_THR,
            .mbr_sa = (uint32_t)&src1,
            .mbr_ubc.mbr_ubc_type.blockDataLength = 10,
            .mbr_ubc.mbr_ubc_type.nextDescriptorDestUpdate = 1,
            .mbr_ubc.mbr_ubc_type.nextDescriptorSrcUpdate = 1,
            .mbr_ubc.mbr_ubc_type.nextDescriptorFetchEnable = 1,
            .mbr_ubc.mbr_ubc_type.nextDescriptorView = 1
        },
        {
            .mbr_nda = (uint32_t)NULL,
            .mbr_da = (uint32_t)&USART0->US_THR,
            .mbr_sa = (uint32_t)&src2,
            .mbr_ubc.mbr_ubc_type.blockDataLength = 10,
            .mbr_ubc.mbr_ubc_type.nextDescriptorDestUpdate = 0,
            .mbr_ubc.mbr_ubc_type.nextDescriptorSrcUpdate = 0,
            .mbr_ubc.mbr_ubc_type.nextDescriptorFetchEnable = 0,
            .mbr_ubc.mbr_ubc_type.nextDescriptorView = 0
        }
    };

    // Allocate a DMA channel
    handle = SYS_DMA_ChannelAllocate(DMA_CHANNEL_0);

    if(SYS_DMA_CHANNEL_HANDLE_INVALID == handle)
    {
        // Error handling here
    }

    //If handle is valid continue

    // User registers an event handler with system service.
    SYS_DMA_ChannelTransferEventHandlerSet(handle,
                                APP_SYS_DMA_TransferEventHandler,
                                NULL);

    // Setup the channel
    SYS_DMA_ChannelSetup(appData.channelHandle,
            SYS_DMA_CHANNEL_OP_MODE_BASIC,
            DMA_TRIGGER_USART0_TX);

    // Setup channel width before adding linked list
    SYS_DMA_ChannelDataWidthSetup(appData.channelHandle,
                    SYS_DMA_DATA_WIDTH_BYTE);

    // Add linked list to the SYS_DMA, channel will be enabled by default.
    SYS_DMA_ChannelLinkedListAdd(appData.channelHandle,
                    &descptr_crtl,
                    (uint32_t)&lld[0]);


    // Event Processing Technique. Event is received when
    // the each block is processed.

    void APP_SYS_DMA_TransferEventHandler(SYS_DMA_TRANSFER_EVENT event,
            SYS_DMA_CHANNEL_HANDLE handle, uintptr_t contextHandle)
    {
        switch(event)
        {
            case SYS_DMA_TRANSFER_EVENT_COMPLETE:

                // This means the block was transferred.
                break;

            case SYS_DMA_TRANSFER_EVENT_ERROR:

                // Error handling here.
                break;

            default:
                break;
        }
    }
    </code>

  Remarks:
    This is supported by PIC32C devices only.
*/
void SYS_DMA_ChannelLinkedListAdd
(
    SYS_DMA_CHANNEL_HANDLE handle,
    SYS_DMA_DESCRIPTOR_CONTROL *firstDescriptorControl,
    uint32_t firstDescriptorAddr
);

//******************************************************************************
/* Function:
    size_t SYS_DMA_ChannelSourceTransferredSizeGet(SYS_DMA_CHANNEL_HANDLE handle)

  Summary:
    Returns the number of bytes transferred from source.

  Description:
    When a data transfer request is submitted. At any time while the transmission
    is in progress the size of the amount of data transferred from source
    can be known by calling this function.

  Precondition:
    DMA should have been initialized by calling SYS_DMA_Initialize.
    DMA channel should have been allocated by calling SYS_DMA_ChannelAllocate.
    Data transfer should have been initiated by calling SYS_DMA_ChannelTransferAdd or
    SYS_DMA_ChannelTransferSet.

  Parameters:
    handle       - A valid channel handle, returned from the system service's
                   Allocate routine

  Returns:
    size_t       - Returns the number of bytes transferred from the submitted size.

  Example:
    <code>
    // 'handle' is a valid handle returned
    // by the SYS_DMA_ChannelAllocate function.
    MY_APP_OBJ          myAppObj;
    uint8_t             buf[10];
    void                *srcAddr;
    void                *destAddr;
    size_t              srcSize;
    size_t              destSize;
    size_t              cellSize;
    size_t              transferredSize;

    srcAddr     = (uint8_t *) buf;
    srcSize     = 10;
    destAddr    = (uin8_t*) &U2TXREG;
    destSize    = 1;
    cellSize    = 1;
    channelHandle = SYS_DMA_ChannelAllocate(channel);
    // User registers an event handler with system service. This is done once

    SYS_DMA_ChannelTransferEventHandlerSet(handle, APP_DMASYSTransferEventHandler,
                                                            (uintptr_t)&myAppObj);

    SYS_DMA_ChannelTransferAdd(handle,srcAddr,srcSize,destAddr,destSize,cellSize);

    if(SYS_DMA_CHANNEL_HANDLE_INVALID == handle)
    {
        // Error handling here
    }

    // The data is being transferred after adding the transfer to the DMA channel.
    // The user can get to know dynamically the amount of data
    // transmitted from source by calling SYS_DMA_ChannelSourceTransferredSizeGet
    transferredSize = SYS_DMA_ChannelSourceTransferredSizeGet(channelHandle);
    </code>

  Remarks:
    None.
*/
size_t SYS_DMA_ChannelSourceTransferredSizeGet(SYS_DMA_CHANNEL_HANDLE handle);

//******************************************************************************
/* Function:
     size_t SYS_DMA_ChannelDestinationTransferredSizeGet(SYS_DMA_CHANNEL_HANDLE handle)

  Summary:
    Returns the number of bytes transferred to destination.

  Description:
    When a data transfer request is submitted. At any time while the transmission
    is in progress the size of the amount of data transferred to destination
    can be known by calling this function.

  Precondition:
    DMA should have been initialized by calling SYS_DMA_Initialize.
    DMA channel should have been allocated by calling SYS_DMA_ChannelAllocate.
    Data transfer should have been initiated by calling SYS_DMA_ChannelTransferAdd or
    SYS_DMA_ChannelTransferSet.

  Parameters:
    handle       - A valid channel handle, returned from the system service's
                   Allocate routine

  Returns:
    size_t       - Returns the number of bytes received from the submitted size.

  Example:
    <code>
    // 'handle' is a valid handle returned
    // by the SYS_DMA_ChannelAllocate function.
    MY_APP_OBJ          myAppObj;
    uint8_t             buf[10];
    void                *srcAddr;
    void                *destAddr;
    size_t              srcSize;
    size_t              destSize;
    size_t              cellSize;
    size_t              transferredSize;

    srcAddr     = (uin8_t*) &U2RXREG;
    srcSize     = 1;
    destAddr    = (uint8_t *) buf ;
    destSize    = 10;
    cellSize    = 1;
    channelHandle = SYS_DMA_ChannelAllocate(channel);
    // User registers an event handler with system service. This is done once

    SYS_DMA_ChannelTransferEventHandlerSet(handle, APP_DMASYSTransferEventHandler,
                                                            (uintptr_t)&myAppObj);

    SYS_DMA_ChannelTransferAdd(handle,srcAddr,srcSize,destAddr,destSize,cellSize);

    if(SYS_DMA_CHANNEL_HANDLE_INVALID == handle)
    {
        // Error handling here
    }

    // The data is being transferred after adding the transfer to the DMA channel.
    // The user can get to know dynamically the amount of data
    // transmitted to destination by calling SYS_DMA_ChannelDestinationTransferredSizeGet
    transferredSize = SYS_DMA_ChannelDestinationTransferredSizeGet(channelHandle);
    </code>

  Remarks:
    None.
*/
size_t SYS_DMA_ChannelDestinationTransferredSizeGet(SYS_DMA_CHANNEL_HANDLE handle);

//******************************************************************************
/*
  Function:
    void SYS_DMA_ChannelTransferEventHandlerSet
    (
        SYS_DMA_CHANNEL_HANDLE handle,
        const SYS_DMA_CHANNEL_TRANSFER_EVENT_HANDLER eventHandler,
        const uintptr_t contextHandle
    )

  Summary:
    This function allows a DMA system service client to set an event handler.

  Description:
    This function allows a client to set an event handler. The client may want
    to receive transfer related events in cases when it uses non-blocking read and
    write functions. The event handler should be set before the client
    intends to perform operations that could generate events.

    This function accepts a contextHandle parameter. This parameter could be
    set by the client to contain (or point to) any client specific data object
    that should be associated with this DMA channel.

  Precondition:
    DMA should have been initialized by calling SYS_DMA_Initialize.
    DMA channel should have been allocated by calling SYS_DMA_ChannelAllocate.


  Parameters:
    handle       - A valid channel handle, returned from the system service's
                   Allocate routine

    eventHandler - Pointer to the event handler function.

    contextHandle - Value identifying the context of the application/driver/middleware
    				that registered the event handling function.

  Returns:
    None.

  Example:
    <code>
    // 'handle' is a valid handle returned
    // by the SYS_DMA_ChannelAllocate function.
    MY_APP_OBJ          myAppObj;
    uint8_t             buf[10];
    void                *srcAddr;
    void                *destAddr;
    size_t              srcSize;
    size_t              destSize;
    size_t              cellSize;

    srcAddr     = (uint8_t *) buf;
    srcSize     = 10;
    destAddr    = (uin8_t*) &U2TXREG;
    destSize    = 1;
    cellSize    = 1;
    channelHandle = SYS_DMA_ChannelAllocate(channel);
    // User registers an event handler with system service. This is done once

    SYS_DMA_ChannelTransferEventHandlerSet(handle, APP_DMASYSTransferEventHandler,
                                                            (uintptr_t)&myAppObj);

    SYS_DMA_ChannelTransferAdd(handle,srcAddr,srcSize,destAddr,destSize,cellSize);

    if(SYS_DMA_CHANNEL_HANDLE_INVALID == handle)
    {
        // Error handling here
    }

    // Event Processing Technique. Event is received when
    // the transfer is processed.

    void APP_DMASYSTransferEventHandler(SYS_DMA_TRANSFER_EVENT event,
            SYS_DMA_CHANNEL_HANDLE handle, uintptr_t contextHandle)
    {
        switch(event)
        {
            case SYS_DMA_TRANSFER_EVENT_COMPLETE:

                // This means the data was transferred.
                break;

            case SYS_DMA_TRANSFER_EVENT_ERROR:

                // Error handling here.
                break;

            default:
                break;
        }
    }
    </code>

  Remarks:
    None.
 */
void SYS_DMA_ChannelTransferEventHandlerSet
(
    SYS_DMA_CHANNEL_HANDLE handle,
    const SYS_DMA_CHANNEL_TRANSFER_EVENT_HANDLER eventHandler,
    const uintptr_t contextHandle
);

// *****************************************************************************
/* Function:
    SYS_DMA_ERROR SYS_DMA_ChannelErrorGet(SYS_DMA_CHANNEL_HANDLE handle)

  Summary:
    This function returns the error(if any) associated with the last client
    request.

  Description:
    This function returns the error(if any) associated with the last client
    request. If the service send a SYS_DMA_TRANSFER_EVENT_ERROR to the client,
    the client can call this function to know the error cause.
    The error status will be updated on every operation and should be read
    frequently (ideally immediately after the service operation has completed)
    to know the relevant error status.

  Precondition:
    The SYS_DMA_Initialize routine must have been called for the DMA sub system.
    SYS_DMA_ChannelAllocate must have been called to obtain a valid opened channel
    handle.

  Parameters:
     handle          - Handle of the DMA channel as returned by the
                       SYS_DMA_ChannelAllocate function.

  Returns:
    A SYS_DMA_ERROR type indicating last known error status.

  Example:
    <code>

    // 'handle' is a valid handle returned
    // by the SYS_DMA_ChannelAllocate function.

    // pDmaSrc, pDmaDst is the  source,destination address
    // txferSrcSize, txferDesSize is the source,destination transfer size
    // cellSize is the cell size
    MY_APP_OBJ          myAppObj;
    // Client registers an event handler with service. This is done once

    SYS_DMA_ChannelTransferEventHandlerSet( handle, APP_DMATransferEventHandle,
                                     (uintptr_t)&myAppObj );

    SYS_DMA_ChannelSetup(handle, SYS_DMA_CHANNEL_OP_MODE_BASIC,
                                 DMA_TRIGGER_SOURCE_NONE);

    SYS_DMA_ChannelTransferAdd(handle,pDmaSrc,txferSrcSize,
                                pDmaDst,txferDesSize,cellSize);

    SYS_DMA_ChannelForceStart(handle);

    if(SYS_DMA_CHANNEL_HANDLE_INVALID == handle)
    {
        // Error handling here
    }

    // Event Processing Technique. Event is received when
    // the transfer is processed.

    void APP_DMATransferEventHandle( SYS_DMA_TRANSFER_EVENT event,
            SYS_DMA_CHANNEL_HANDLE handle, uintptr_t contextHandle )
    {

        switch(event)
        {
            case SYS_DMA_TRANSFER_EVENT_COMPLETE:

                // This means the data was transferred.
                break;

            case SYS_DMA_TRANSFER_EVENT_ERROR:

                // Error handling here.

                if(SYS_DMA_ERROR_ADDRESS_ERROR == SYS_DMA_ChannelErrorGet(handle))
                {
                    // There was an address error.
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
    obtained frequently. The service will update the error status
    regardless of whether this has been examined by the client.
*/

SYS_DMA_ERROR SYS_DMA_ChannelErrorGet(SYS_DMA_CHANNEL_HANDLE handle);

//******************************************************************************
/* Function:
    void SYS_DMA_ChannelForceStart(SYS_DMA_CHANNEL_HANDLE handle)

  Summary:
    Force start of transfer on the selected channel.

  Description:
    The function force start a DMA transfer to occur for the selected channel.

  Precondition:
    DMA should have been initialized by calling SYS_DMA_Initialize.
    DMA channel should have been allocated by calling SYS_DMA_ChannelAllocate.
    The function SYS_DMA_ChannelSetup must have been called to setup and
    enable the required features.
    The function 'SYS_DMA_ChannelTransferAdd' to add a transfer.

  Parameters:
     handle          - Handle of the DMA channel as returned by the
                       SYS_DMA_ChannelAllocate function.
  Returns:
    None.

  Example:
  <code>

    // 'handle' is a valid handle returned
    // by the SYS_DMA_ChannelAllocate function.

    // pDmaSrc, pDmaDst is the  source,destination address
    // txferSrcSize, txferDesSize is the source,destination transfer size
    // cellSize is the cell size
    MY_APP_OBJ          myAppObj;
    // Client registers an event handler with service. This is done once

    SYS_DMA_ChannelTransferEventHandlerSet( handle, APP_DMATransferEventHandle,
                                     (uintptr_t)&myAppObj );

    SYS_DMA_ChannelSetup(handle, SYS_DMA_CHANNEL_OP_MODE_BASIC,
                                 DMA_TRIGGER_SOURCE_NONE);

    SYS_DMA_ChannelTransferAdd(handle,pDmaSrc,txferSrcSize,
                                pDmaDst,txferDesSize,cellSize);

    SYS_DMA_ChannelForceStart(handle);

    if(SYS_DMA_CHANNEL_HANDLE_INVALID == handle)
    {
        // Error handling here
    }

    // Event Processing Technique. Event is received when
    // the transfer is processed.

    void APP_DMATransferEventHandle( SYS_DMA_TRANSFER_EVENT event,
            SYS_DMA_CHANNEL_HANDLE handle, uintptr_t contextHandle )
    {

        switch(event)
        {
            case SYS_DMA_TRANSFER_EVENT_COMPLETE:

                // This means the data was transferred.
                break;

            case SYS_DMA_TRANSFER_EVENT_ERROR:

                // Error handling here.

                if(SYS_DMA_ERROR_ADDRESS_ERROR == DRV_I2S_ErrorGet(myI2SHandle))
                {
                    // There was an address error.
                    // Do error handling here.
                }

                break;

            default:
                break;
        }
    }
  </code>

  Remarks:
   This function must be used to start the DMA transfer when the channel has been
   setup(by calling SYS_DMA_ChannelSetup) with the eventSrc as DMA_TRIGGER_SOURCE_NONE.
*/

void SYS_DMA_ChannelForceStart(SYS_DMA_CHANNEL_HANDLE handle);

//******************************************************************************
/* Function:
    void SYS_DMA_ChannelForceAbort(SYS_DMA_CHANNEL_HANDLE handle)

  Summary:
    Force abort of transfer on the selected channel.

  Description:
    The function aborts a DMA transfer to occur for the selected channel.

  Precondition:
    DMA should have been initialized by calling SYS_DMA_Initialize.
    DMA channel should have been allocated by calling SYS_DMA_ChannelAllocate.
    The function SYS_DMA_ChannelSetup must have been called to setup and
    enable the required features.
    The function 'SYS_DMA_ChannelTransferAdd' to add a transfer.

  Parameters:
     handle          - Handle of the DMA channel as returned by the
                       SYS_DMA_ChannelAllocate function.

  Returns:
    None.

  Example:
  <code>

    // 'handle' is a valid handle returned
    // by the SYS_DMA_ChannelAllocate function.

    // pDmaSrc, pDmaDst is the  source,destination address
    // txferSrcSize, txferDesSize is the source,destination transfer size
    // cellSize is the cell size

    MY_APP_OBJ          myAppObj;
    // Client registers an event handler with service. This is done once

    SYS_DMA_ChannelTransferEventHandlerSet( handle, APP_DMATransferEventHandle,
                                     (uintptr_t)&myAppObj );

    SYS_DMA_ChannelSetup(handle, SYS_DMA_CHANNEL_OP_MODE_BASIC,
                                 DMA_TRIGGER_SOURCE_NONE);

    SYS_DMA_ChannelTransferAdd(handle,pDmaSrc,txferSrcSize,
                                pDmaDst,txferDesSize,cellSize);

    SYS_DMA_ChannelForceStart(handle);

    if(SYS_DMA_CHANNEL_HANDLE_INVALID == handle)
    {
        // Error handling here
    }

    ....
    ....
    // Client may need to abort a transfer
    SYS_DMA_ChannelForceAbort(handle);

    // Event Processing Technique. Event is received when
    // the transfer is processed.

    void APP_DMATransferEventHandle( SYS_DMA_TRANSFER_EVENT event,
            SYS_DMA_CHANNEL_HANDLE handle, uintptr_t contextHandle )
    {

        switch(event)
        {
            case SYS_DMA_TRANSFER_EVENT_ABORT:

                // This means the data was transferred.
                break;

            case SYS_DMA_TRANSFER_EVENT_ERROR:

                // Error handling here.

                if(SYS_DMA_ERROR_ADDRESS_ERROR == DRV_I2S_ErrorGet(myI2SHandle))
                {
                    // There was an address error.
                    // Do error handling here.
                }

                break;

            default:
                break;
        }
    }
  </code>

  Remarks:
   This function must be used to abort the DMA transfer when the channel has been
   setup(by calling SYS_DMA_ChannelSetup) with the eventSrc as DMA_TRIGGER_SOURCE_NONE.
   and SYS_DMA_ChannelAbortEventSet has not been called.
*/
void SYS_DMA_ChannelForceAbort(SYS_DMA_CHANNEL_HANDLE handle);


//******************************************************************************
/* Function:
    void SYS_DMA_ChannelAbortEventSet
    (
        SYS_DMA_CHANNEL_HANDLE handle,
        DMA_TRIGGER_SOURCE eventSrc
    )

  Summary:
    Sets an event source and enables cell transfer abort event for the same
    for the selected channel.

  Description:
    This functions enables a cell transfer abort event for the selected source
    event.

  Precondition:
    DMA should have been initialized by calling SYS_DMA_Initialize.
    DMA channel should have been allocated by calling SYS_DMA_ChannelAllocate.
    The function SYS_DMA_ChannelSetup must have been called to setup and
    enable the required features.
    The function 'SYS_DMA_ChannelTransferAdd' to add a transfer.

  Parameters:
     handle          - Handle of the DMA channel as returned by the
                       SYS_DMA_ChannelAllocate function.
     eventSrc         - The event causing the cell transfer abort

  Returns:
    None.

  Example:
  <code>

    // 'handle' is a valid handle returned
    // by the SYS_DMA_ChannelAllocate function.

    // pDmaSrc, pDmaDst is the  source,destination address
    // txferSrcSize, txferDesSize is the source,destination transfer size
    // cellSize is the cell size
    MY_APP_OBJ          myAppObj;
    // Client registers an event handler with service. This is done once

    SYS_DMA_ChannelTransferEventHandlerSet( handle, APP_DMATransferEventHandle,
                                     (uintptr_t)&myAppObj );

    SYS_DMA_ChannelSetup(handle, SYS_DMA_CHANNEL_OP_MODE_BASIC,
                                 DMA_TRIGGER_SOURCE_NONE);

    SYS_DMA_ChannelTransferAdd(handle,pDmaSrc,txferSrcSize,
                                pDmaDst,txferDesSize,cellSize);

    SYS_DMA_ChannelAbortEventSet(handle, DMA_TRIGGER_CTMU);

    SYS_DMA_ChannelForceStart(handle);

    if(SYS_DMA_CHANNEL_HANDLE_INVALID == handle)
    {
        // Error handling here
    }

    // Event Processing Technique. Event is received when
    // the transfer is processed.

    void APP_DMATransferEventHandle( SYS_DMA_TRANSFER_EVENT event,
            SYS_DMA_CHANNEL_HANDLE handle, uintptr_t contextHandle )
    {

        switch(event)
        {
            case SYS_DMA_TRANSFER_EVENT_ABORT:

                // This means the data was transferred.
                break;

            case SYS_DMA_TRANSFER_EVENT_ERROR:

                // Error handling here.

                if(SYS_DMA_ERROR_ADDRESS_ERROR == DRV_I2S_ErrorGet(myI2SHandle))
                {
                    // There was an address error.
                    // Do error handling here.
                }

                break;

            default:
                break;
        }
    }
  </code>

  Remarks:
    If the parameter 'eventSrc' is specified as DMA_TRIGGER_SOURCE_NONE the
    current DMA transfer will be aborted. The behavior is a same as calling
    SYS_DMA_ChannelForceAbort.

*/

void SYS_DMA_ChannelAbortEventSet
(
    SYS_DMA_CHANNEL_HANDLE handle,
    DMA_TRIGGER_SOURCE eventSrc
);

//******************************************************************************
/* Function:
    bool SYS_DMA_ChannelIsBusy(SYS_DMA_CHANNEL_HANDLE handle)

  Summary:
    Returns the busy status of the specified DMA channel.

  Description:
    This function returns the busy status of the selected DMA channel

  Precondition:
    DMA should have been initialized by calling SYS_DMA_Initialize.
    DMA channel should have been allocated by calling SYS_DMA_ChannelAllocate.

  Parameters:
    handle          - Handle of the DMA channel as returned by the
                       SYS_DMA_ChannelAllocate function.

  Returns:
    bool         - true,  if the selected DMA channel is active or enabled
                 - false, if the selected DMA channel is inactive or disabled
  Example:

  <code>
    bool                        busyStat;

    busyStat    = SYS_DMA_ChannelGetBusy(handle);
  </code>

  Remarks:
    This feature may not be available on all devices. Refer to the specific device
	data sheet to determine availability.
*/

bool SYS_DMA_ChannelIsBusy(SYS_DMA_CHANNEL_HANDLE handle);

//******************************************************************************
/* Function:
    void SYS_DMA_ChannelSuspend(SYS_DMA_CHANNEL_HANDLE handle)

  Summary:
    Suspend DMA operation on the specified DMA channel.

  Description:
    This function suspends the DMA operation on the DMA channel
    specified by the channel handle.

  Precondition:
    DMA should have been initialized by calling SYS_DMA_Initialize.
    DMA channel should have been allocated by calling SYS_DMA_ChannelAllocate.

  Parameters:
    handle          - Handle of the DMA channel as returned by the
                       SYS_DMA_ChannelAllocate function.

  Returns:
    None.
  Example:

  <code>
    SYS_DMA_ChannelSuspend(handle);
  </code>

  Remarks:
    This feature may not be available on all devices. Refer to the specific device
	data sheet to determine availability.
*/

void SYS_DMA_ChannelSuspend(SYS_DMA_CHANNEL_HANDLE handle);

//******************************************************************************
/* Function:
    void SYS_DMA_ChannelResume(SYS_DMA_CHANNEL_HANDLE handle)

  Summary:
    Resume DMA operation on the specified DMA channel.

  Description:
    This function resumes the DMA operation on the DMA channel
    specified by the channel handle.

  Precondition:
    DMA should have been initialized by calling SYS_DMA_Initialize.
    DMA channel should have been allocated by calling SYS_DMA_ChannelAllocate.

  Parameters:
    handle          - Handle of the DMA channel as returned by the
                       SYS_DMA_ChannelAllocate function.

  Returns:
    None.
  Example:

  <code>
    SYS_DMA_ChannelResume(handle);
  </code>

  Remarks:
    This feature may not be available on all devices. Refer to the specific device
	data sheet to determine availability.
*/

void SYS_DMA_ChannelResume(SYS_DMA_CHANNEL_HANDLE handle);


// *****************************************************************************
// *****************************************************************************
// Section: Global DMA Management Functions
// *****************************************************************************
// *****************************************************************************

//******************************************************************************
/* Function:
    void SYS_DMA_Suspend(void)

  Summary:
    Suspend DMA transfers.

  Description:
    This function suspends DMA transfers to allow CPU uninterrupted access
    to data bus

  Precondition:
    DMA should have been initialized by calling SYS_DMA_Initialize.

  Parameters:
   None.

  Returns:
    None.

  Example:
  <code>
    SYS_DMA_Suspend();
  </code>

  Remarks:
    None
*/

void SYS_DMA_Suspend(void);

//******************************************************************************
/* Function:
    void SYS_DMA_Resume(void)

  Summary:
    Resume DMA operations.

  Description:
    This function disables DMA suspend. It resumes the DMA operation suspended
    by calling SYS_DMA_Suspend. The DMA operates normally.

  Precondition:
    DMA should have been initialized by calling SYS_DMA_Initialize.

  Parameters:
   None.

  Returns:
    None.

  Example:
  <code>
    SYS_DMA_Resume();
  </code>

  Remarks:
    None
*/
void SYS_DMA_Resume(void);


//******************************************************************************
/* Function:
    bool SYS_DMA_IsBusy(void)

  Summary:
    Returns the busy status of the DMA module.

  Description:
    This function returns the busy status of the DMA module

  Precondition:
    DMA should have been initialized by calling SYS_DMA_Initialize.

  Parameters:
   None.

  Returns:
    Boolean
	- true  - The DMA module is active
    - false - The DMA module is inactive and disabled

  <code>
    bool busyStat;
    busyStat = SYS_DMA_IsBusy();
  </code>

  Remarks:
    This feature may not be available on all devices. Refer to the specific device
	data sheet to determine availability.
*/
bool SYS_DMA_IsBusy(void);


#ifdef __cplusplus
}
#endif

#endif // #ifndef _SYS_DMA_H
/*******************************************************************************
 End of File
*/
