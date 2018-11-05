/*******************************************************************************
  DMA System Service Library

  Company:
    Microchip Technology Inc.

  File Name:
    sys_dma.c

  Summary:
    DMA System Service.

  Description:
    This file implements the core functionality of the DMA System Service.
    It provides a way to interact with the DMA subsystem to
    manage the data transfer between different peripherals and/or memory
    without intervention from the CPU.
*******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2013 released Microchip Technology Inc.  All rights reserved.

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
// Section: File includes
// *****************************************************************************
// *****************************************************************************

#include "system_config.h"
#include "system/int/sys_int.h"
#include "system/dma/sys_dma.h"
#include "system/dma/src/sys_dma_local.h"



// *****************************************************************************
/* DMA Channel instance objects.

  Summary:
    Defines the DMA channel objects

  Description:
    This data type defines the DMA channel objects. It holds the information
    about the usage/availability of a channel for DMA data transfers

  Remarks:
    Not all modes are available on all micro-controllers.
 */

SYS_DMA_CHANNEL_OBJECT  gSysDMAChannelObj[SYS_DMA_CHANNEL_COUNT];


// *****************************************************************************
// *****************************************************************************
// Section: DMA System Service Interface Routines
// *****************************************************************************
// *****************************************************************************


// *****************************************************************************
// *****************************************************************************
// Section: Initialization
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

    This routine initializes the DMA module making it ready for clients to
    open and use it. The initialization data is specified by the init parameter.

  Remarks:
    This routine must be called before any other DMA systems service routines
    are called.

    Not all features are available on all micro-controllers.
*/
SYS_MODULE_OBJ SYS_DMA_Initialize(const SYS_MODULE_INIT * const init)
{
    SYS_DMA_INIT *initp;
    uint8_t chanIndex;
    SYS_DMA_CHANNEL_OBJECT *chanObj;


    /* Validate the init object */
    if ((SYS_MODULE_INIT *)NULL == init)
    {
        SYS_ASSERT(false, "Invalid Init Object");
        return SYS_MODULE_OBJ_INVALID;
    }

    initp = (SYS_DMA_INIT *)init;

    /* Enable/disable the stop in idle mode feature. */
#if defined (PLIB_DMA_ExistsStopInIdle)
    if(true == PLIB_DMA_ExistsStopInIdle(DMA_ID_0))
    {
        if(SYS_DMA_SIDL_DISABLE == initp->sidl)
        {
            PLIB_DMA_StopInIdleDisable(DMA_ID_0);
        }
        else if(SYS_DMA_SIDL_ENABLE == initp->sidl)
        {
            PLIB_DMA_StopInIdleEnable(DMA_ID_0);
        }
    }
#endif
    /* Enable the DMA module */
    PLIB_DMA_Enable(DMA_ID_0);

    /* Initialize the available channel objects */
    chanObj             =   (SYS_DMA_CHANNEL_OBJECT *)&gSysDMAChannelObj[0];
    for(chanIndex = 0; chanIndex < SYS_DMA_CHANNEL_COUNT; chanIndex++)
    {
        chanObj->inUse          =    false;
        chanObj->pEventCallBack =    NULL;
        chanObj->hClientArg     =    0;
        chanObj->errorInfo      =    SYS_DMA_ERROR_NONE;
        chanObj                 =    chanObj + 1;
        /* Initializing all channel priorities as 0 */
        PLIB_DMA_ChannelXPrioritySelect(DMA_ID_0, chanIndex, chanIndex);
    }

    /* Return the object structure */
    return ((SYS_MODULE_OBJ) initp);
}


// *****************************************************************************
// *****************************************************************************
// Section:  Channel Setup and management routines
// *****************************************************************************
// *****************************************************************************

//******************************************************************************
/* Function:
    SYS_DMA_CHANNEL_HANDLE SYS_DMA_ChannelAllocate(DMA_CHANNEL channel)

  Summary:
    Allocates the specified DMA channel and returns a handle to it

  Description:
    This routine opens the specified DMA channel and provides a
    handle that must be provided to all other client-level operations to
    identify the caller and the DMA channel.

  Remarks:
    The handle returned is valid until the SYS_DMA_ChannelRelease routine is called.
    This routine must be called before any other DMA channel Setup and management
    routines are called
*/
SYS_DMA_CHANNEL_HANDLE SYS_DMA_ChannelAllocate(DMA_CHANNEL channel)
{
    SYS_DMA_CHANNEL_HANDLE              channelHandle;
    SYS_DMA_CHANNEL_OBJECT              *chanObj;
    uint8_t                             chanIndex;
    bool                                chanAlloc;

    channelHandle       =   (SYS_DMA_CHANNEL_HANDLE)NULL;
    chanAlloc           =   false;
    chanObj             =   (SYS_DMA_CHANNEL_OBJECT *)&gSysDMAChannelObj[0];

    for(chanIndex = 0; chanIndex < SYS_DMA_CHANNEL_COUNT; chanIndex++)
    {
        /* Iterate to find a free channel */
        if(false == chanObj->inUse)
        {
            /* If user's choice is any,
             * Allocate the first available free channel */
            if(DMA_CHANNEL_ANY == channel)
            {
                chanObj->inUse          = true;
                chanAlloc               = true;
                break;
            }
            /* Check if the requested channel is free */
            else if(chanIndex == channel)
            {
                chanObj->inUse          = true;
                chanAlloc               = true;
                break;
            }

        }
        /* Check the next channel object */
        chanObj += 1;
    }
    if(true == chanAlloc )
    {
        /* Return the valid found handle */
        channelHandle  = (SYS_DMA_CHANNEL_HANDLE)chanObj;
    }
    else
    {
        /* Return an Invalid handle */
        channelHandle = SYS_DMA_CHANNEL_HANDLE_INVALID;
    }
    return channelHandle;
}


//******************************************************************************
/* Function:
    void SYS_DMA_ChannelRelease(SYS_DMA_CHANNEL_HANDLE handle)

  Summary:
    Deallocates and frees the channel specified by the handle.

  Description:
    This routine deallocates an allocated-channel of the DMA module,
    invalidating the handle.

  Remarks:
    After calling this routine, the handle passed in "handle" must not be used
    with any of the remaining service's routines.  A new handle must be obtained by
    calling SYS_DMA_ChannelAllocate before the caller may use the service again
*/
 void SYS_DMA_ChannelRelease (SYS_DMA_CHANNEL_HANDLE handle)
 {
    uint8_t index;
    SYS_DMA_CHANNEL_OBJECT *chanObj;
    DMA_CHANNEL channelNumber;

    chanObj = (SYS_DMA_CHANNEL_OBJECT *) handle;
    channelNumber = (chanObj  - (SYS_DMA_CHANNEL_OBJECT *) &gSysDMAChannelObj[0]);
    chanObj->inUse = false;
    /* Disable all interrupts on this channel */
    for(index = 0; index < 8 ;index++)
    {
        PLIB_DMA_ChannelXINTSourceDisable( DMA_ID_0, channelNumber, (1<<index));
    }
    return;
 }


//******************************************************************************
/* Function:
    void SYS_DMA_ChannelSetup
    (
        SYS_DMA_CHANNEL_HANDLE handle,
        SYS_DMA_CHANNEL_OP_MODE modeEnable,
        DMA_TRIGGER_SOURCE eventSrc
    )

  Summary:
    Setup the DMA channel parameters.

  Description:
    This function sets up the DMA channel parameters.
    It sets the channel priority and enables the mode of operations for the
    current system design.

  Remarks:
    If SYS_DMA_CHANNEL_OP_MODE_MATCH_ABORT or
    SYS_DMA_CHANNEL_OP_MODE_CRC mode of operation is enabled, then corresponding
    mode setup API's needs to be called to set the related parameters.

    If the parameter 'eventSrc' is specified as DMA_TRIGGER_SOURCE_NONE then
    SYS_DMA_ChannelForceStart must be called to start the DMA channel transfer.

    Not all features are available on all micro-controllers.
  */

void SYS_DMA_ChannelSetup
(
    SYS_DMA_CHANNEL_HANDLE handle,
    SYS_DMA_CHANNEL_OP_MODE modeEnable,
    DMA_TRIGGER_SOURCE eventSrc
)
{

    SYS_DMA_CHANNEL_OBJECT *chanObj;
    DMA_CHANNEL channelNumber;

    chanObj = (SYS_DMA_CHANNEL_OBJECT *) handle;
    channelNumber = (chanObj  - (SYS_DMA_CHANNEL_OBJECT *) &gSysDMAChannelObj[0]);

    /* Setup the channel modes */
    /* Basic mode, nothing specifically needs to be set */
    if(SYS_DMA_CHANNEL_OP_MODE_BASIC & modeEnable)
    {
        /* Do nothing */
        ;
    }

    /* Pattern match Abort */
    if(SYS_DMA_CHANNEL_OP_MODE_MATCH_ABORT & modeEnable)
    {
        PLIB_DMA_ChannelXTriggerEnable( DMA_ID_0, channelNumber,
                                        DMA_CHANNEL_TRIGGER_PATTERN_MATCH_ABORT);
    }

    /* Channel chaining mode */
    if(SYS_DMA_CHANNEL_OP_MODE_CHAIN_HIGH & modeEnable)
    {
        PLIB_DMA_ChannelXChainEnable( DMA_ID_0, channelNumber);
        PLIB_DMA_ChannelXChainToHigher(DMA_ID_0, channelNumber);
        PLIB_DMA_ChannelXDisabledEnablesEvents(DMA_ID_0, channelNumber);
    }
    else if(SYS_DMA_CHANNEL_OP_MODE_CHAIN_LOW & modeEnable)
    {
        PLIB_DMA_ChannelXChainEnable( DMA_ID_0, channelNumber);
        PLIB_DMA_ChannelXChainToLower(DMA_ID_0, channelNumber);
        PLIB_DMA_ChannelXDisabledEnablesEvents(DMA_ID_0, channelNumber);
    }
    else
    {
        ;
    }

    /* Auto Enable mode */
    if(SYS_DMA_CHANNEL_OP_MODE_AUTO & modeEnable)
    {
        PLIB_DMA_ChannelXAutoEnable( DMA_ID_0, channelNumber);
    }

    /* CRC Enable */
    if(SYS_DMA_CHANNEL_OP_MODE_CRC & modeEnable)
    {
            PLIB_DMA_CRCEnable( DMA_ID_0);
    }


    /* Setup the DMA Trigger Source and Enable it */
    if(DMA_TRIGGER_SOURCE_NONE == eventSrc)
    {
        /* This is polling mode of Implementation */
        PLIB_DMA_ChannelXINTSourceEnable(DMA_ID_0, channelNumber,
                                            DMA_INT_BLOCK_TRANSFER_COMPLETE);
        PLIB_DMA_ChannelXINTSourceEnable(DMA_ID_0, channelNumber,
                                            DMA_INT_ADDRESS_ERROR);
    }
    else
    {
        PLIB_DMA_ChannelXStartIRQSet( DMA_ID_0, channelNumber, eventSrc);
        PLIB_DMA_ChannelXTriggerEnable( DMA_ID_0, channelNumber, DMA_CHANNEL_TRIGGER_TRANSFER_START);

        /* We need to notify the user on the completion of a transfer request.
         * Setting the DMA block completion transfer interrupt. */
        PLIB_DMA_ChannelXINTSourceEnable(DMA_ID_0, channelNumber,
                                                DMA_INT_BLOCK_TRANSFER_COMPLETE);
        PLIB_DMA_ChannelXINTSourceEnable(DMA_ID_0, channelNumber,
                                            DMA_INT_ADDRESS_ERROR);
    }
    return;
}


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


  Remarks:
    The parameter 'pattern' (8-bit or 16-bit) is part specific.
    (Refer the particular datasheet for details)

    Not all features are available on all micro-controllers.
    Refer the particular datasheet for details)
*/

void SYS_DMA_ChannelSetupMatchAbortMode
(
    SYS_DMA_CHANNEL_HANDLE handle,
    uint16_t pattern,
    DMA_PATTERN_LENGTH length,
    SYS_DMA_CHANNEL_IGNORE_MATCH ignore,
    uint8_t ignorePattern
)
{
    SYS_DMA_CHANNEL_OBJECT *chanObj;
    DMA_CHANNEL channelNumber;

    chanObj = (SYS_DMA_CHANNEL_OBJECT *) handle;
    channelNumber = (chanObj  - (SYS_DMA_CHANNEL_OBJECT *) &gSysDMAChannelObj[0]);

#if defined(PLIB_DMA_ExistsChannelXPatternLength)
    if(true == PLIB_DMA_ExistsChannelXPatternLength(DMA_ID_0))
    {
        PLIB_DMA_ChannelXPatternLengthSet( DMA_ID_0, channelNumber, length );
    }
#endif
    PLIB_DMA_ChannelXPatternDataSet(DMA_ID_0, channelNumber, pattern);

if(SYS_DMA_CHANNEL_IGNORE_MATCH_ENABLE == ignore)
{    
    #if defined(PLIB_DMA_ExistsChannelXPatternIgnoreByte)    
        if(true == PLIB_DMA_ExistsChannelXPatternIgnoreByte(DMA_ID_0))
        {
            PLIB_DMA_ChannelXPatternIgnoreByteEnable(DMA_ID_0, channelNumber);
        }
    #endif    

    #if defined(PLIB_DMA_ExistsChannelXPatternIgnore)    
        if(true == PLIB_DMA_ExistsChannelXPatternIgnore(DMA_ID_0))
        {
            PLIB_DMA_ChannelXPatternIgnoreSet( DMA_ID_0, channelNumber, ignorePattern );
        }
    #endif
}
    
    return;
}


//******************************************************************************
/* Function:
    void SYS_DMA_ChannelCRCSet
    (
        SYS_DMA_CHANNEL_HANDLE handle,
        SYS_DMA_CHANNEL_OPERATION_MODE_CRC crc
    )

  Summary:
    Sets up the CRC operation mode

  Description:
    This function sets up the CRC computation features.

  Remarks:
    Not all features are available on all micro-controllers.
*/

void SYS_DMA_ChannelCRCSet
(
    SYS_DMA_CHANNEL_HANDLE handle,
    SYS_DMA_CHANNEL_OPERATION_MODE_CRC crc
)
{
    SYS_DMA_CHANNEL_OBJECT *chanObj;
    DMA_CHANNEL channelNumber;

    chanObj = (SYS_DMA_CHANNEL_OBJECT *) handle;
    channelNumber = (chanObj  - (SYS_DMA_CHANNEL_OBJECT *) &gSysDMAChannelObj[0]);

#if defined (PLIB_DMA_ExistsCRCChannel)
    if(true == PLIB_DMA_ExistsCRCChannel(DMA_ID_0))
    {
        PLIB_DMA_CRCChannelSelect(DMA_ID_0, channelNumber);
    }
#endif

#if defined (PLIB_DMA_ExistsCRCType)
    if(true == PLIB_DMA_ExistsCRCType(DMA_ID_0))
    {
        PLIB_DMA_CRCTypeSet(DMA_ID_0, crc.type);
    }
#endif

    PLIB_DMA_CRCPolynomialLengthSet(DMA_ID_0, (crc.polyLength-1));
    
#if defined (PLIB_DMA_ExistsCRCBitOrder)
    if(true == PLIB_DMA_ExistsCRCBitOrder(DMA_ID_0))
    {
        PLIB_DMA_CRCBitOrderSelect(DMA_ID_0, crc.bitOrder);
    }
#endif
    
#if defined (PLIB_DMA_ExistsCRCByteOrder)
    if(true == PLIB_DMA_ExistsCRCByteOrder(DMA_ID_0))
    {
        PLIB_DMA_CRCByteOrderSelect(DMA_ID_0, crc.byteOrder);
    }
#endif

#if defined (PLIB_DMA_ExistsCRCWriteByteOrder)
    if(true == PLIB_DMA_ExistsCRCWriteByteOrder(DMA_ID_0))
    {
        if(SYS_DMA_CRC_WRITE_ORDER_MAINTAIN == crc.writeOrder)
        {
            PLIB_DMA_CRCWriteByteOrderMaintain(DMA_ID_0);
        }
        else if(SYS_DMA_CRC_WRITE_ORDER_CHANGE == crc.writeOrder)
        {
            PLIB_DMA_CRCWriteByteOrderAlter(DMA_ID_0);
        }
        else
        {
        }
    }
#endif

#if defined (PLIB_DMA_ExistsCRCData)
    if(true == PLIB_DMA_ExistsCRCData(DMA_ID_0))
    {
        PLIB_DMA_CRCDataWrite(DMA_ID_0, crc.data);
    }
#endif

#if defined (PLIB_DMA_ExistsCRCXOREnable)
    if(true == PLIB_DMA_ExistsCRCXOREnable(DMA_ID_0))
    {
        PLIB_DMA_CRCXOREnableSet(DMA_ID_0, crc.xorBitMask);
    }
#endif

    if(SYS_DMA_CHANNEL_CRC_MODE_APPEND == crc.mode)
    {
        PLIB_DMA_CRCAppendModeEnable(DMA_ID_0);
    }
        return;
}


//******************************************************************************
/* Function:
    uint32_t SYS_DMA_ChannelCRCGet(void)

  Summary:
    Returns the computed CRC

  Description:
    This  function returns the computed CRC

  Remarks:
    To get the computed CRC value this function must be called after the block
    transfer completion event. ie After getting and processing the callback
    registered with SYS_DMA_ChannelTransferEventHandlerSet.

    Not all features are available on all micro-controllers.
*/

uint32_t SYS_DMA_ChannelCRCGet(void)
{
    uint32_t crcData = 0;

#if defined (PLIB_DMA_ExistsCRCData)
    if(true == PLIB_DMA_ExistsCRCData(DMA_ID_0))
    {
        crcData = PLIB_DMA_CRCDataRead(DMA_ID_0);
    }
#endif
    return crcData;
}


//******************************************************************************
/* Function:
    void SYS_DMA_ChannelEnable(SYS_DMA_CHANNEL_HANDLE handle)

  Summary:
    Enables a channel.

  Description:
    This function enables a channel.

  Remarks:
        This function may not required to be called when starting DMA setup
        (by SYS_DMA_ChannelSetup) and transfer Add (by SYS_DMA_ChannelTransferAdd).
        But may be needed to be called in the registered callback to enable the
        channel and continue the data transfer with the existing transfer parameters
        previously set with 'SYS_DMA_ChannelTransferAdd'.
        The DMA channel is by default disabled on the completion of block
        transfer(callback generated)
*/

void SYS_DMA_ChannelEnable(SYS_DMA_CHANNEL_HANDLE handle)
{
    SYS_DMA_CHANNEL_OBJECT *chanObj;
    DMA_CHANNEL channelNumber;

    chanObj = (SYS_DMA_CHANNEL_OBJECT *) handle;
    channelNumber = (chanObj  - (SYS_DMA_CHANNEL_OBJECT *) &gSysDMAChannelObj[0]);

    PLIB_DMA_ChannelXEnable(DMA_ID_0, channelNumber);

    return;
}


//******************************************************************************
/* Function:
    void SYS_DMA_ChannelDisable(SYS_DMA_CHANNEL_HANDLE handle)

  Summary:
    Disables a channel.

  Description:
    This function disables a channel.

  Remarks:
    None.
*/

void SYS_DMA_ChannelDisable(SYS_DMA_CHANNEL_HANDLE handle)
{
    SYS_DMA_CHANNEL_OBJECT *chanObj;
    DMA_CHANNEL channelNumber;

    chanObj = (SYS_DMA_CHANNEL_OBJECT *) handle;
    channelNumber = (chanObj  - (SYS_DMA_CHANNEL_OBJECT *) &gSysDMAChannelObj[0]);

    PLIB_DMA_ChannelXDisable(DMA_ID_0, channelNumber);

    return;
}


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

  Remarks:
    None.
*/
void SYS_DMA_ChannelTransferAdd
(
    SYS_DMA_CHANNEL_HANDLE handle,
    const void *srcAddr, size_t srcSize,
    const void *destAddr, size_t destSize,
    size_t cellSize
)
{
    SYS_DMA_CHANNEL_OBJECT *chanObj;
    DMA_CHANNEL channelNumber;

    chanObj = (SYS_DMA_CHANNEL_OBJECT *) handle;
    channelNumber = (chanObj  - (SYS_DMA_CHANNEL_OBJECT *) &gSysDMAChannelObj[0]);

    /* Set the transfer paramteters */
    PLIB_DMA_ChannelXSourceStartAddressSet(DMA_ID_0, channelNumber, (uint32_t) srcAddr);
    PLIB_DMA_ChannelXSourceSizeSet(DMA_ID_0, channelNumber, (uint16_t) srcSize);
    PLIB_DMA_ChannelXDestinationStartAddressSet(DMA_ID_0, channelNumber,(uint32_t) destAddr);
    PLIB_DMA_ChannelXDestinationSizeSet(DMA_ID_0, channelNumber, (uint16_t) destSize);
    PLIB_DMA_ChannelXCellSizeSet(DMA_ID_0, channelNumber,(uint16_t) cellSize);

    /* Enable the channel */
    PLIB_DMA_ChannelXEnable(DMA_ID_0, channelNumber);
    return;
}

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

  Remarks:
    None.
*/
void SYS_DMA_ChannelTransferSet
(
    SYS_DMA_CHANNEL_HANDLE handle,
    const void *srcAddr, size_t srcSize,
    const void *destAddr, size_t destSize,
    size_t cellSize
)
{
    SYS_DMA_CHANNEL_OBJECT *chanObj;
    DMA_CHANNEL channelNumber;

    chanObj = (SYS_DMA_CHANNEL_OBJECT *) handle;
    channelNumber = (chanObj  - (SYS_DMA_CHANNEL_OBJECT *) &gSysDMAChannelObj[0]);

    /* Set the transfer parameters */
    PLIB_DMA_ChannelXSourceStartAddressSet(DMA_ID_0, channelNumber, (uint32_t) srcAddr);
    PLIB_DMA_ChannelXSourceSizeSet(DMA_ID_0, channelNumber, (uint16_t) srcSize);
    PLIB_DMA_ChannelXDestinationStartAddressSet(DMA_ID_0, channelNumber,(uint32_t) destAddr);
    PLIB_DMA_ChannelXDestinationSizeSet(DMA_ID_0, channelNumber, (uint16_t) destSize);
    PLIB_DMA_ChannelXCellSizeSet(DMA_ID_0, channelNumber,(uint16_t) cellSize);

    return;
}

//******************************************************************************
/* Function:
    size_t SYS_DMA_ChannelSourceTransferredSizeGet(SYS_DMA_CHANNEL_HANDLE handle)

  Summary:
    Returns the number of bytes transferred from source.

  Description:
    When a data transfer request is submitted. At any time while the transmission
    is in progress the size of the amount of data transferred from source
    can be known by calling this function.

  Remarks:
    None.
*/
size_t SYS_DMA_ChannelSourceTransferredSizeGet(SYS_DMA_CHANNEL_HANDLE handle)
{
    SYS_DMA_CHANNEL_OBJECT *chanObj;
    DMA_CHANNEL channelNumber;

    chanObj = (SYS_DMA_CHANNEL_OBJECT *) handle;
    channelNumber = (chanObj  - (SYS_DMA_CHANNEL_OBJECT *) &gSysDMAChannelObj[0]);
    return (size_t) PLIB_DMA_ChannelXSourcePointerGet(DMA_ID_0,channelNumber);
}

//******************************************************************************
/* Function:
    size_t SYS_DMA_ChannelDestinationTransferredSizeGet(SYS_DMA_CHANNEL_HANDLE handle)

  Summary:
    Returns the number of bytes transferred to destination.

  Description:
    When a data transfer request is submitted. At any time while the transmission
    is in progress the size of the amount of data transferred to destination
    can be known by calling this function.

  Remarks:
    None.
*/
size_t SYS_DMA_ChannelDestinationTransferredSizeGet(SYS_DMA_CHANNEL_HANDLE handle)
{
    SYS_DMA_CHANNEL_OBJECT *chanObj;
    DMA_CHANNEL channelNumber;

    chanObj = (SYS_DMA_CHANNEL_OBJECT *) handle;
    channelNumber = (chanObj  - (SYS_DMA_CHANNEL_OBJECT *) &gSysDMAChannelObj[0]);
    return (size_t) PLIB_DMA_ChannelXDestinationPointerGet(DMA_ID_0,channelNumber);
}

/******************************************************************************
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

  Remarks:
    None.
 */
void SYS_DMA_ChannelTransferEventHandlerSet
(
    SYS_DMA_CHANNEL_HANDLE handle,
    const SYS_DMA_CHANNEL_TRANSFER_EVENT_HANDLER eventHandler,
    const uintptr_t contextHandle
)
{
    SYS_DMA_CHANNEL_OBJECT *chanObj;
    chanObj = (SYS_DMA_CHANNEL_OBJECT *)handle;

    if (chanObj == (SYS_DMA_CHANNEL_OBJECT *) NULL ||
        eventHandler == (SYS_DMA_CHANNEL_TRANSFER_EVENT_HANDLER) NULL)
    {
        SYS_ASSERT(false, "Invalid handle");
        return;
    }
    else
    {
        /* Set the Event Handler and context */
        chanObj->pEventCallBack = eventHandler;
        chanObj->hClientArg = contextHandle;
    }
    return;
}


/*******************************************************************************
  Function:
    void SYS_DMA_Tasks(SYS_MODULE_OBJ object );

  Summary:
    Maintains the system service's state machine.

  Description:
    This routine is used to maintain the DMA system service's internal state machine.
    This function is specifically designed for non DMA interrupt trigger
    implementations(polling mode), and should be used only in polling mode.
    this function should be called from the SYS_Tasks() function.

  Remarks:
    This routine is normally not called directly by an application.  It is
    called by the system's Tasks routine (SYS_Tasks).
 
    This function is deprecated. Any calls made to this functions will have no effect.
 */

void SYS_DMA_Tasks(SYS_MODULE_OBJ object)
{
    return;
}


/*******************************************************************************
  Function:
    void SYS_DMA_TasksISR(SYS_MODULE_OBJ object, DMA_CHANNEL activeChannel)

  Summary:
    Maintains the system service's state machine and implements its ISR

  Description:
    This routine is used to maintain the DMA system service's internal state machine
    and implement its ISR for DMA interrupt trigger implementations(interrupt mode).
    This function is specifically designed for DMA interrupt trigger
    implementations (interrupt mode).
    In interrupt mode, this function should be called in the interrupt
    service routine of the DMA channel that is associated with this transfer.

  Remarks:
    It is called by appropriate raw ISR.

    This routine may execute in an ISR context and will never block or access any
    resources that may cause it to block.
 */
void SYS_DMA_TasksISR(SYS_MODULE_OBJ object, DMA_CHANNEL activeChannel)
{
    SYS_DMA_CHANNEL_OBJECT *chanObj;
    DMA_CHANNEL_INT_SOURCE chanIntSrc;

    /* Find out the interrupt source number for the active DMA channel */
     chanIntSrc = PLIB_DMA_ChannelXTriggerSourceNumberGet(DMA_ID_0, activeChannel);

    /* Check whether the active DMA channel interrupt has occurred */
   // if(true == SYS_INT_SourceStatusGet(chanIntSrc))
   // {
        if(true == PLIB_DMA_ChannelXINTSourceFlagGet(DMA_ID_0,activeChannel,
                        DMA_INT_BLOCK_TRANSFER_COMPLETE))
        {
            /* Channel is by default disabled on completion of a block transfer */

            /* Clear the Block transfer complete flag */
            PLIB_DMA_ChannelXINTSourceFlagClear(DMA_ID_0,activeChannel,
                     DMA_INT_BLOCK_TRANSFER_COMPLETE);

            SYS_INT_SourceStatusClear(chanIntSrc);

            /* Find out the channel object and give a callback */
            chanObj = (SYS_DMA_CHANNEL_OBJECT *) &gSysDMAChannelObj[activeChannel];
            chanObj->errorInfo = SYS_DMA_ERROR_NONE;
			if(NULL != chanObj->pEventCallBack)
			{
	            chanObj->pEventCallBack(SYS_DMA_TRANSFER_EVENT_COMPLETE,
                         (SYS_DMA_CHANNEL_HANDLE)chanObj,chanObj->hClientArg);
			}

        }
        else if(true == PLIB_DMA_ChannelXINTSourceFlagGet(DMA_ID_0,activeChannel,
                        DMA_INT_TRANSFER_ABORT))
        {
            /* Channel is by default disabled on Transfer Abortion */

            /* Clear the Abort transfer complete flag */
            PLIB_DMA_ChannelXINTSourceFlagClear(DMA_ID_0,activeChannel,
                        DMA_INT_TRANSFER_ABORT);

            SYS_INT_SourceStatusClear(chanIntSrc);

            /* Find out the channel object and give a callback */
            chanObj = (SYS_DMA_CHANNEL_OBJECT *) &gSysDMAChannelObj[activeChannel];
            chanObj->errorInfo = SYS_DMA_ERROR_NONE;
			if(NULL != chanObj->pEventCallBack)
			{
	            chanObj->pEventCallBack(SYS_DMA_TRANSFER_EVENT_ABORT,
                        (SYS_DMA_CHANNEL_HANDLE)chanObj,chanObj->hClientArg);
			}
        }
        else
        {
            ;
        }
  //  }
    return;
}

/*****************************************************************************
  Function:
    void SYS_DMA_TasksError(SYS_MODULE_OBJ object );

  Summary:
    Maintains the system service's error state machine.

  Description:
    This routine is used to maintain the DMA system service's error state machine.
	This function is specifically designed for non DMA interrupt trigger
    implementations(polling mode), and should be used only in polling mode.
    this function should be called from the SYS_Tasks() function.

  Remarks:
    This routine is normally not called directly by an application.  It is
    called by the system's Tasks routine (SYS_Tasks).

 */

void SYS_DMA_TasksError(SYS_MODULE_OBJ object)
{
    SYS_DMA_CHANNEL_OBJECT *chanObj;
    DMA_CHANNEL channels;
    DMA_CHANNEL_INT_SOURCE chanIntSrc;

    /* Go through each DMA channel and service it */
    for (channels = DMA_CHANNEL_0; channels < DMA_NUMBER_OF_CHANNELS; channels++)
    {
		/* Find out the interrupt source number for the last active DMA channel */
		chanIntSrc = PLIB_DMA_ChannelXTriggerSourceNumberGet(DMA_ID_0, channels);

		/* Check whether the active DMA channel interrupt has occured */
		if(true == SYS_INT_SourceStatusGet(chanIntSrc))
		{
			if(true == PLIB_DMA_ChannelXINTSourceFlagGet(DMA_ID_0,channels,
						DMA_INT_ADDRESS_ERROR))
			{
				/* Channel is by default disabled on completion of a block transfer */

				/* Clear the Block transfer complete flag */
				PLIB_DMA_ChannelXINTSourceFlagClear(DMA_ID_0,channels,
						DMA_INT_ADDRESS_ERROR);

				SYS_INT_SourceStatusClear(chanIntSrc);

				/* Find out the channel object and give a callback */
				chanObj = (SYS_DMA_CHANNEL_OBJECT *) &gSysDMAChannelObj[channels];
				chanObj->errorInfo = SYS_DMA_ERROR_ADDRESS_ERROR;
                if(NULL != chanObj->pEventCallBack)
                {                
                    chanObj->pEventCallBack(SYS_DMA_TRANSFER_EVENT_ERROR,
						(SYS_DMA_CHANNEL_HANDLE)chanObj,chanObj->hClientArg);
                }
			}
		}
	}
    return;
}


/*******************************************************************************
  Function:
    void SYS_DMA_TasksErrorISR(SYS_MODULE_OBJ object, DMA_CHANNEL activeChannel)

  Summary:
    Maintains the system service's state machine and implements its ISR

  Description:
    This routine is used to maintain the DMA system service's internal error state machine
    and implement its ISR for DMA interrupt trigger implementations(interrupt mode).
    This function is specifically designed for DMA interrupt trigger
    implementations (interrupt mode).
    In interrupt mode, this function should be called in the interrupt
    service routine of the DMA channel that is associated with this transfer.

  Remarks:
    It is called by appropriate raw ISR.

    This routine may execute in an ISR context and will never block or access any
    resources that may cause it to block.
 */
void SYS_DMA_TasksErrorISR(SYS_MODULE_OBJ object, DMA_CHANNEL activeChannel)
{
    SYS_DMA_CHANNEL_OBJECT *chanObj;
    DMA_CHANNEL_INT_SOURCE chanIntSrc;

    /* Find out the interrupt source number for the last active DMA channel */
    chanIntSrc = PLIB_DMA_ChannelXTriggerSourceNumberGet(DMA_ID_0, activeChannel);

    /* Check whether the active DMA channel interrupt has occured */
    if(true == SYS_INT_SourceStatusGet(chanIntSrc))
    {
        if(true == PLIB_DMA_ChannelXINTSourceFlagGet(DMA_ID_0,activeChannel,
                    DMA_INT_ADDRESS_ERROR))
        {
            /* Channel is by default disabled on completion of a block transfer */

            /* Clear the Block transfer complete flag */
            PLIB_DMA_ChannelXINTSourceFlagClear(DMA_ID_0,activeChannel,
                    DMA_INT_ADDRESS_ERROR);

            SYS_INT_SourceStatusClear(chanIntSrc);

            /* Find out the channel object and give a callback */
            chanObj = (SYS_DMA_CHANNEL_OBJECT *) &gSysDMAChannelObj[activeChannel];
            chanObj->errorInfo = SYS_DMA_ERROR_ADDRESS_ERROR;
			if(NULL != chanObj->pEventCallBack)
			{            
                chanObj->pEventCallBack(SYS_DMA_TRANSFER_EVENT_ERROR,
                        (SYS_DMA_CHANNEL_HANDLE)chanObj,chanObj->hClientArg);
            }
        }
    }
    return;
}

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

  Remarks:
    It is the client's reponsibility to make sure that the error status is
    obtained frequently. The service will update the error status
    irrespective of whether this has been examined by the client.
*/

SYS_DMA_ERROR SYS_DMA_ChannelErrorGet(SYS_DMA_CHANNEL_HANDLE handle)
{
    SYS_DMA_CHANNEL_OBJECT *chanObj;

    chanObj = (SYS_DMA_CHANNEL_OBJECT *) handle;
    return chanObj->errorInfo;
}


//******************************************************************************
/* Function:
    void SYS_DMA_ChannelForceStart(SYS_DMA_CHANNEL_HANDLE handle)

  Summary:
    Force start of transfer on the selected channel

  Description:
    The function force start a DMA transfer to occur for the selected channel.

  Remarks:
   This function must be used to start the DMA transfer when the channel has been
   setup(by calling SYS_DMA_ChannelSetup) with the eventSrc as DMA_TRIGGER_SOURCE_NONE.
*/

void SYS_DMA_ChannelForceStart(SYS_DMA_CHANNEL_HANDLE handle)
{
    SYS_DMA_CHANNEL_OBJECT *chanObj;
    DMA_CHANNEL channelNumber;

    chanObj = (SYS_DMA_CHANNEL_OBJECT *) handle;
    channelNumber = (chanObj  - (SYS_DMA_CHANNEL_OBJECT *) &gSysDMAChannelObj[0]);

    PLIB_DMA_StartTransferSet(DMA_ID_0, channelNumber);

    return;
}

//******************************************************************************
/* Function:
    void SYS_DMA_ChannelForceAbort(SYS_DMA_CHANNEL_HANDLE handle)

  Summary:
    Force abort of transfer on the selected channel

  Description:
    The function abort a DMA transfer to occur for the selected channel.

  Remarks:
   This function must be used to abort the DMA transfer when the channel has been
   setup(by calling SYS_DMA_ChannelSetup) with the eventSrc as DMA_TRIGGER_SOURCE_NONE.
   and SYS_DMA_ChannelAbortEventSet has not been called.
*/

void SYS_DMA_ChannelForceAbort(SYS_DMA_CHANNEL_HANDLE handle)
{
    SYS_DMA_CHANNEL_OBJECT *chanObj;
    DMA_CHANNEL channelNumber;

    chanObj = (SYS_DMA_CHANNEL_OBJECT *) handle;
    channelNumber = (chanObj  - (SYS_DMA_CHANNEL_OBJECT *) &gSysDMAChannelObj[0]);

    PLIB_DMA_AbortTransferSet(DMA_ID_0, channelNumber);

    return;
}


//******************************************************************************
/* Function:
    void SYS_DMA_ChannelAbortEventSet(SYS_DMA_CHANNEL_HANDLE handle,
                                        DMA_TRIGGER_SOURCE eventSrc)

  Summary:
    Sets an event source and enables cell transfer abort event for the same
    for the selected channel.

  Description:
    This functions enables a cell transfer abort event for the selected source
    event.

  Remarks:
    If the parameter 'eventSrc' is specified as DMA_TRIGGER_SOURCE_NONE the
    current DMA transfer will be aborted. The behavior is a same as calling
    SYS_DMA_ChannelForceAbort.

*/

void SYS_DMA_ChannelAbortEventSet(SYS_DMA_CHANNEL_HANDLE handle,
                                    DMA_TRIGGER_SOURCE eventSrc)
{
    SYS_DMA_CHANNEL_OBJECT *chanObj;
    DMA_CHANNEL channelNumber;

    chanObj = (SYS_DMA_CHANNEL_OBJECT *) handle;
    channelNumber = (chanObj  - (SYS_DMA_CHANNEL_OBJECT *) &gSysDMAChannelObj[0]);

    /* Setup the DMA Trigger Abort Source and Enable it */
    if(DMA_TRIGGER_SOURCE_NONE == eventSrc)
    {
        PLIB_DMA_AbortTransferSet(DMA_ID_0, channelNumber);
    }
    else
    {
        PLIB_DMA_ChannelXAbortIRQSet( DMA_ID_0, channelNumber, eventSrc);
        PLIB_DMA_ChannelXTriggerEnable( DMA_ID_0, channelNumber, DMA_CHANNEL_TRIGGER_TRANSFER_ABORT);
    }
    return;
}


//******************************************************************************
/* Function:
    bool SYS_DMA_ChannelIsBusy(SYS_DMA_CHANNEL_HANDLE handle)

  Summary:
    Returns the busy status of the specified DMA channel

  Description:
    This function returns the busy status of the selected DMA channel

  Remarks:
    Not all features are available on all micro-controllers.
*/

bool SYS_DMA_ChannelIsBusy(SYS_DMA_CHANNEL_HANDLE handle)
{
    bool isBusy;
    SYS_DMA_CHANNEL_OBJECT *chanObj;
    DMA_CHANNEL channelNumber;

    chanObj = (SYS_DMA_CHANNEL_OBJECT *) handle;
    channelNumber = (chanObj  - (SYS_DMA_CHANNEL_OBJECT *) &gSysDMAChannelObj[0]);
    isBusy = false;

#if defined(PLIB_DMA_ExistsChannelXBusy)
    if(true == PLIB_DMA_ExistsChannelXBusy(DMA_ID_0))
    {
        if(true == PLIB_DMA_ChannelXBusyIsBusy( DMA_ID_0, channelNumber))
        {
            isBusy = true;
        }
    }
#endif
    return isBusy;
}


// *****************************************************************************
// *****************************************************************************
// Section: Global DMA Management Routines
// *****************************************************************************
// *****************************************************************************
//******************************************************************************
//******************************************************************************
/* Function:
    void SYS_DMA_Suspend(void)

  Summary:
    Suspend DMA transfers.

  Description:
    This function suspends DMA transfers to allow CPU uninterrupted access
    to data bus

  Remarks:
    None
*/

void SYS_DMA_Suspend(void)
{
    PLIB_DMA_SuspendEnable(DMA_ID_0);
    return;
}

//******************************************************************************
/* Function:
    void SYS_DMA_Resume(void)

  Summary:
    Resume DMA operations

  Description:
    This function disables DMA suspend. It resumes the DMA operation suspended
    by calling SYS_DMA_Suspend. The DMA operates normally.

  Remarks:
    None
*/
void SYS_DMA_Resume(void)
{
    PLIB_DMA_SuspendDisable(DMA_ID_0);
    return;
}


//******************************************************************************
/* Function:
    bool SYS_DMA_IsBusy(void)

  Summary:
    Returns the busy status of the DMA module

  Description:
    This function returns the busy status of the DMA module

  Remarks:
    Not all features are available on all micro-controllers.
*/
bool SYS_DMA_IsBusy(void)
{
#if defined(PLIB_DMA_ExistsBusy)
    if(true == PLIB_DMA_ExistsBusy(DMA_ID_0))
    {
        return PLIB_DMA_IsBusy(DMA_ID_0);
    }
#endif
    return false;  // added this line to avoid compiler warning
}

/*******************************************************************************
 End of File
*/



