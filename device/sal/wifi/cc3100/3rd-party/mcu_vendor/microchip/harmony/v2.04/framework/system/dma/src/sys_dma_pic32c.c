/*******************************************************************************
  DMA System Service Library

  Company:
    Microchip Technology Inc.

  File Name:
    sys_dma_pic32c.c

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
Copyright (c) 2017 released Microchip Technology Inc.  All rights reserved.

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

#include "system/dma/src/sys_dma_local_pic32c.h"

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
    SYS_MODULE_OBJ SYS_DMA_Initialize ( const SYS_MODULE_INIT * const init )

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
SYS_MODULE_OBJ SYS_DMA_Initialize ( const SYS_MODULE_INIT * const init )
{
    uint8_t chanIndex;
    SYS_DMA_INIT *dmaInit = NULL ;
    SYS_DMA_CHANNEL_OBJECT *chanObj;


    /* Validate the init object */
    if ((SYS_MODULE_INIT *)NULL == init)
    {
        SYS_ASSERT(false, "Invalid Init Object");
        return SYS_MODULE_OBJ_INVALID;
    }

    dmaInit = (SYS_DMA_INIT *)init;

    /* Initialize the available channel objects */
    chanObj = (SYS_DMA_CHANNEL_OBJECT *)&gSysDMAChannelObj[0];

    for (chanIndex = 0; chanIndex < SYS_DMA_CHANNEL_COUNT; chanIndex++)
    {
        chanObj->channelID      =    DMA_CHANNEL_NONE;
        chanObj->channelTrigger =    DMA_TRIGGER_SOURCE_NONE;
        chanObj->inUse          =    false;
        chanObj->pEventCallBack =    NULL;
        chanObj->hClientArg     =    0;
        chanObj->errorInfo      =    SYS_DMA_ERROR_NONE;
        chanObj                 =    chanObj + 1;
    }

    /* Enable system level interrupts */
    SYS_INT_SourceStatusClear(XDMAC_IRQn);
    SYS_INT_SourceEnable(XDMAC_IRQn);

    /* Return the object structure */
    return ((SYS_MODULE_OBJ) dmaInit);
}


// *****************************************************************************
// *****************************************************************************
// Section:  Channel Setup and management routines
// *****************************************************************************
// *****************************************************************************

//******************************************************************************
/* Function:
    SYS_DMA_CHANNEL_HANDLE SYS_DMA_ChannelAllocate ( DMA_CHANNEL channel )

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
SYS_DMA_CHANNEL_HANDLE SYS_DMA_ChannelAllocate ( DMA_CHANNEL channel )
{
    SYS_DMA_CHANNEL_HANDLE              channelHandle;
    SYS_DMA_CHANNEL_OBJECT              *chanObj;
    uint8_t                             chanIndex;
    bool                                chanAlloc;

    channelHandle       =   (SYS_DMA_CHANNEL_HANDLE)NULL;
    chanAlloc           =   false;
    chanObj             =   (SYS_DMA_CHANNEL_OBJECT *)&gSysDMAChannelObj[0];

    for (chanIndex = 0; chanIndex < SYS_DMA_CHANNEL_COUNT; chanIndex++)
    {
        /* Iterate to find a free channel */
        if (false == chanObj->inUse)
        {
            /* If user's choice is any,
             * Allocate the first available free channel */
            if (DMA_CHANNEL_ANY == channel)
            {
                chanObj->channelID      = (DMA_CHANNEL)chanIndex;
                chanObj->inUse          = true;
                chanAlloc               = true;
                break;
            }
            /* Check if the requested channel is free */
            else
            {
                if (chanIndex == channel)
                {
                    chanObj->channelID      = channel;
                    chanObj->inUse          = true;
                    chanAlloc               = true;
                    break;
                }
            }
        }
        /* Check the next channel object */
        chanObj += 1;
    }
    if (true == chanAlloc )
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
    void SYS_DMA_ChannelRelease ( SYS_DMA_CHANNEL_HANDLE handle )

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
 void SYS_DMA_ChannelRelease ( SYS_DMA_CHANNEL_HANDLE handle )
 {
    SYS_DMA_CHANNEL_OBJECT *chanObj;

    chanObj = (SYS_DMA_CHANNEL_OBJECT *) handle;

    /* Disable all interrupts on this channel */
    _XDMAC_REGS->XDMAC_CHID[chanObj->channelID].XDMAC_CID.w = XDMAC_CID_Msk;

    /* Disable the Channel to be on the safer side */
    _XDMAC_REGS->XDMAC_GD.w =(XDMAC_GD_DI0_Msk << chanObj->channelID);

    /* Reset chanel object members */
    chanObj->channelID      = DMA_CHANNEL_NONE;
    chanObj->channelTrigger = DMA_TRIGGER_SOURCE_NONE;
    chanObj->inUse          = false;
    chanObj->pEventCallBack = NULL;
    chanObj->hClientArg     = 0;
    chanObj->errorInfo      = SYS_DMA_ERROR_NONE;
 }


//******************************************************************************
/* Function:
    void SYS_DMA_ChannelSetup ( SYS_DMA_CHANNEL_HANDLE handle,
                                SYS_DMA_CHANNEL_OP_MODE modeEnable,
                                DMA_TRIGGER_SOURCE eventSrc )

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

    Not all features are available on all micro-controllers.
  */

void SYS_DMA_ChannelSetup ( SYS_DMA_CHANNEL_HANDLE handle,
                            SYS_DMA_CHANNEL_OP_MODE modeEnable,
                            DMA_TRIGGER_SOURCE eventSrc )
{
    SYS_DMA_CHANNEL_OBJECT *chanObj;
    DMA_CHANNEL channelIndex;

    chanObj = (SYS_DMA_CHANNEL_OBJECT *) handle;

    /* Get the trigger source into channel onject */
    chanObj->channelTrigger = eventSrc;

    channelIndex = chanObj->channelID;

    /* For basic operation:
     * Clear linked list, data striding and block/microblock length registers */
    _XDMAC_REGS->XDMAC_CHID[channelIndex].XDMAC_CNDC.w    = ~XDMAC_CNDC_Msk;
    _XDMAC_REGS->XDMAC_CHID[channelIndex].XDMAC_CDS_MSP.w = ~XDMAC_CDS_MSP_Msk;
    _XDMAC_REGS->XDMAC_CHID[channelIndex].XDMAC_CSUS.w    = ~XDMAC_CSUS_Msk;
    _XDMAC_REGS->XDMAC_CHID[channelIndex].XDMAC_CDUS.w    = ~XDMAC_CDUS_Msk;

    /* by default, single block support for basic mode */
    _XDMAC_REGS->XDMAC_CHID[channelIndex].XDMAC_CBC.w     = ~XDMAC_CBC_Msk;

    /* Keep both destination and source in increment address mode by default */
    _XDMAC_REGS->XDMAC_CHID[channelIndex].XDMAC_CC.SAM = XDMAC_CC_SAM_INCREMENTED_AM_Val ;
    _XDMAC_REGS->XDMAC_CHID[channelIndex].XDMAC_CC.DAM = XDMAC_CC_DAM_INCREMENTED_AM_Val ;

    /* Setup the DMA Trigger Source */
    if (DMA_TRIGGER_SOURCE_NONE == chanObj->channelTrigger)
    {
        /* This is a memory to memory transfer which requires no
         * hardware/software trigger and transfer starts as soon as
         * channel is enabled */
        _XDMAC_REGS->XDMAC_CHID[channelIndex].XDMAC_CC.TYPE = XDMAC_CC_TYPE_MEM_TRAN_Val ;
    }
    else
    {
        /* This is peripheral synchronized transfer which requires
         * hardware/software trigger and transfer starts as soon as
         * trigger event has occurred */
        _SYS_DMA_PeripheralSyncChannelSetup(chanObj);
    }

    /* By default both source and destination sytem bus set for
     * interface 1 as most of the pripherals are on bus interface 1*/
    /* TODO: System bus interface configuration based on src/dest */
    _XDMAC_REGS->XDMAC_CHID[channelIndex].XDMAC_CC.DIF = XDMAC_CC_DIF_AHB_IF1_Val ;
    _XDMAC_REGS->XDMAC_CHID[channelIndex].XDMAC_CC.SIF = XDMAC_CC_SIF_AHB_IF1_Val ;

    /* Other Channel settings are defaults:
     * XDMAC_CC_MBSIZE_SINGLE,
     * XDMAC_CC_DWIDTH_BYTE,
     * XDMAC_CC_MEMSET_NORMAL_MODE */

    /* Setting the DMA block completion transfer interrupt and
     * other error interrupts. */
    _XDMAC_REGS->XDMAC_CHID[channelIndex].XDMAC_CIE.w = (XDMAC_CIE_BIE_Msk | XDMAC_CIE_RBIE_Msk | XDMAC_CIE_WBIE_Msk | XDMAC_CIE_ROIE_Msk);

    /* Enable module level channel interrupt */
    _XDMAC_REGS->XDMAC_GIE.w = (XDMAC_GIE_IE0_Msk << channelIndex) ;
}


//******************************************************************************
/* Function:
    void SYS_DMA_ChannelEnable ( SYS_DMA_CHANNEL_HANDLE handle )

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

void SYS_DMA_ChannelEnable ( SYS_DMA_CHANNEL_HANDLE handle )
{
    SYS_DMA_CHANNEL_OBJECT *chanObj;

    chanObj = (SYS_DMA_CHANNEL_OBJECT *) handle;

    /* Enable Channel */
    _SYS_DMA_ChannelEnable(chanObj->channelID);
}


//******************************************************************************
/* Function:
    void SYS_DMA_ChannelDisable ( SYS_DMA_CHANNEL_HANDLE handle )

  Summary:
    Disables a channel.

  Description:
    This function disables a channel.

  Remarks:
    None.
*/

void SYS_DMA_ChannelDisable ( SYS_DMA_CHANNEL_HANDLE handle )
{
    SYS_DMA_CHANNEL_OBJECT *chanObj;

    chanObj = (SYS_DMA_CHANNEL_OBJECT *) handle;

    _XDMAC_REGS->XDMAC_GD.w = (XDMAC_GD_DI0_Msk << chanObj->channelID);
}


//******************************************************************************
/* Function:
    void SYS_DMA_ChannelTransferAdd ( SYS_DMA_CHANNEL_HANDLE handle,
                                      const void *srcAddr, size_t srcSize
                                      const void *destAddr, size_t destSize,
                                      size_t cellSize )

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
void SYS_DMA_ChannelTransferAdd( SYS_DMA_CHANNEL_HANDLE handle,
                                 const void *srcAddr, size_t srcSize,
                                 const void *destAddr, size_t destSize,
                                 size_t cellSize )
{
    SYS_DMA_CHANNEL_OBJECT *chanObj;
    DMA_CHANNEL channelIndex;
    volatile uint32_t status;

    chanObj = (SYS_DMA_CHANNEL_OBJECT *) handle;

    channelIndex = chanObj->channelID;

    /* Clear channel level status before adding transfer parameters */
    status = _XDMAC_REGS->XDMAC_CHID[channelIndex].XDMAC_CIS.w;
    (void)status;

    /*Set source address */
    _XDMAC_REGS->XDMAC_CHID[channelIndex].XDMAC_CSA.SA = (uint32_t)srcAddr;

    /* Set destination address */
    _XDMAC_REGS->XDMAC_CHID[channelIndex].XDMAC_CDA.DA = (uint32_t)destAddr;

    /* Setup transfer based on transfer type */
    if (DMA_TRIGGER_SOURCE_NONE == chanObj->channelTrigger)
    {
        /* This is memory to memory transfer */

        /* Source and Destination are in incrementing addressing mode by default */
        /* Both source and destination are in incrementing addressing mode,
         * so use the minimum of these two that is achievable size of data
         * for the transfer */
        _XDMAC_REGS->XDMAC_CHID[channelIndex].XDMAC_CUBC.UBLEN = MIN_SIZE(srcSize, destSize);

    }
    else
    {
        /* This is peripheral<->memory transfer */
        _SYS_DMA_PeripheralSyncTransferSetup(chanObj, srcSize, destSize);
    }

    /* Default cell and burst size will be 1 */
    /* TODO: Configure chunk and burst size */

    /* Enable the channel */
    _SYS_DMA_ChannelEnable(channelIndex);
}


//******************************************************************************
/* Function:
    void SYS_DMA_ChannelDataWidthSet ( SYS_DMA_CHANNEL_HANDLE handle,
                                       SYS_DMA_DATA_WIDTH dataWidth )

  Summary:
    Sets up data width for the specified channel.

  Description:
    This function sets up data width for the specified channel.

  Remarks:
    This is supported by PIC32C devices only.
*/
void SYS_DMA_ChannelDataWidthSet ( SYS_DMA_CHANNEL_HANDLE handle,
                                   SYS_DMA_DATA_WIDTH dataWidth )
{
    SYS_DMA_CHANNEL_OBJECT *chanObj;

    chanObj = (SYS_DMA_CHANNEL_OBJECT *) handle;

    _XDMAC_REGS->XDMAC_CHID[chanObj->channelID].XDMAC_CC.DWIDTH = (dataWidth>>1);
}

//******************************************************************************
/* Function:
    void SYS_DMA_ChannelLinkedListAdd ( SYS_DMA_CHANNEL_HANDLE handle,
                                        SYS_DMA_DESCRIPTOR_SETUP *nextDescriptorControl,
                                        uint32_t nextDescriptorAddr )

  Summary:
    Sets up a multi-block data transfer on a DMA channel using descriptor mode.

  Description:
    This function sets up multi-block data transfer for a DMA channel in
    descriptor model. That is each block of data to be transferred is configured
    as desciptors of a linked list.
    Each desciptor can be defined in a descriptor view format which is defined
    at SYS_DMA_DESCRIPTOR_VIEW.

  Remarks:
    This is supported by PIC32C devices only.
*/
void SYS_DMA_ChannelLinkedListAdd ( SYS_DMA_CHANNEL_HANDLE handle,
                                    SYS_DMA_DESCRIPTOR_CONTROL *firstDescriptorControl,
                                    uint32_t firstDescriptorAddr )
{
    SYS_DMA_CHANNEL_OBJECT *chanObj;
    DMA_CHANNEL channelIndex;
    volatile uint32_t status;

    chanObj = (SYS_DMA_CHANNEL_OBJECT *) handle;

    channelIndex = chanObj->channelID;

    /* Clear channel level status before adding transfer parameters */
    status = _XDMAC_REGS->XDMAC_CHID[channelIndex].XDMAC_CIS.w;
    (void)status;

    /* First descriptor control set */
    _XDMAC_REGS->XDMAC_CHID[channelIndex].XDMAC_CNDC.w = (firstDescriptorControl->mbr_ubc >> SYS_DMA_UBLEN_BIT_WIDTH);

    /* First descriptor address set,
     * and by default it uses system bus interface 1 */
    _XDMAC_REGS->XDMAC_CHID[channelIndex].XDMAC_CNDA.w = (firstDescriptorAddr & XDMAC_CNDA_NDA_Msk) | 1 ;

    /* Enable end of linked list interrupt source */
    _XDMAC_REGS->XDMAC_CHID[channelIndex].XDMAC_CIE.LIE = 1 ;

    /* Enable the channel */

    /* Enable Channel */
    _SYS_DMA_ChannelEnable(channelIndex);
}


/******************************************************************************
  Function:
    void SYS_DMA_ChannelTransferEventHandlerSet ( SYS_DMA_CHANNEL_HANDLE handle,
                    const SYS_DMA_CHANNEL_TRANSFER_EVENT_HANDLER eventHandler,
                    const uintptr_t contextHandle )

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
void SYS_DMA_ChannelTransferEventHandlerSet ( SYS_DMA_CHANNEL_HANDLE handle,
                const SYS_DMA_CHANNEL_TRANSFER_EVENT_HANDLER eventHandler,
                const uintptr_t contextHandle )
{
    SYS_DMA_CHANNEL_OBJECT *chanObj;
    chanObj = (SYS_DMA_CHANNEL_OBJECT *)handle;

    if ( (chanObj == (SYS_DMA_CHANNEL_OBJECT *) NULL) || (eventHandler == (SYS_DMA_CHANNEL_TRANSFER_EVENT_HANDLER) NULL) )
    {
        SYS_ASSERT(false, "Invalid handle");
    }
    else
    {
        /* Set the Event Handler and context */
        chanObj->pEventCallBack = eventHandler;
        chanObj->hClientArg = contextHandle;
    }
}

/******************************************************************************
  Function:
    void SYS_DMA_Tasks ( SYS_MODULE_OBJ object )

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

void SYS_DMA_Tasks ( SYS_MODULE_OBJ object )
{
    SYS_DMA_CHANNEL_OBJECT *chanObj = (SYS_DMA_CHANNEL_OBJECT *)&gSysDMAChannelObj[0];
    uint8_t    chanIndex;

    /* Iterate to find active channel */
    for (chanIndex = 0; chanIndex < SYS_DMA_CHANNEL_COUNT; chanIndex++)
    {
        /* Process events only on active channels */
        if (true == chanObj->inUse)
        {
            /* Channel specific task */
            _SYS_DMA_ChannelTasks( chanObj->channelID );
        }

        /* Point to next channel object */
        chanObj += 1;
    }
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

SYS_DMA_ERROR SYS_DMA_ChannelErrorGet ( SYS_DMA_CHANNEL_HANDLE handle )
{
    SYS_DMA_CHANNEL_OBJECT *chanObj;

    chanObj = (SYS_DMA_CHANNEL_OBJECT *) handle;

    return chanObj->errorInfo;
}


//******************************************************************************
/* Function:
    void SYS_DMA_ChannelForceAbort ( SYS_DMA_CHANNEL_HANDLE handle )

  Summary:
    Force abort of transfer on the selected channel

  Description:
    The function abort a DMA transfer to occur for the selected channel.

  Remarks:
   This function must be used to abort the DMA transfer when the channel has been
   setup(by calling SYS_DMA_ChannelSetup) with the eventSrc as DMA_TRIGGER_SOURCE_NONE.
   and SYS_DMA_ChannelAbortEventSet has not been called.
*/

void SYS_DMA_ChannelForceAbort ( SYS_DMA_CHANNEL_HANDLE handle )
{
    SYS_DMA_CHANNEL_OBJECT *chanObj;

    chanObj = (SYS_DMA_CHANNEL_OBJECT *) handle;

    /* Suspend and disable the channel */
    _XDMAC_REGS->XDMAC_GRWS.w = (XDMAC_GRWS_RWS0_Msk << chanObj->channelID);
    _XDMAC_REGS->XDMAC_GD.w = (XDMAC_GD_DI0_Msk << chanObj->channelID);
}


//******************************************************************************
/* Function:
    bool SYS_DMA_ChannelIsBusy ( SYS_DMA_CHANNEL_HANDLE handle )

  Summary:
    Returns the busy status of the specified DMA channel

  Description:
    This function returns the busy status of the selected DMA channel

  Remarks:
    Not all features are available on all micro-controllers.
*/

bool SYS_DMA_ChannelIsBusy ( SYS_DMA_CHANNEL_HANDLE handle )
{
    bool isBusy = false;
    SYS_DMA_CHANNEL_OBJECT *chanObj;

    chanObj = (SYS_DMA_CHANNEL_OBJECT *) handle;

    /*Check if channel write/read is in progress */
    if ( (1 == _XDMAC_REGS->XDMAC_CHID[chanObj->channelID].XDMAC_CC.WRIP) || (1 == _XDMAC_REGS->XDMAC_CHID[chanObj->channelID].XDMAC_CC.RDIP) )
    {
        isBusy = true;
    }

    return isBusy;
}

//******************************************************************************
/* Function:
    void SYS_DMA_ChannelSuspend ( SYS_DMA_CHANNEL_HANDLE handle )

  Summary:
    Suspend DMA operation on the specified DMA channel.

  Description:
    This function suspends DMA operation on the specified DMA channel.

  Remarks:
    Not all features are available on all micro-controllers.
*/

void SYS_DMA_ChannelSuspend ( SYS_DMA_CHANNEL_HANDLE handle )
{
    SYS_DMA_CHANNEL_OBJECT *chanObj;

    chanObj = (SYS_DMA_CHANNEL_OBJECT *) handle;

    /* Suspend the read and write transactions on channel specified */
    _XDMAC_REGS->XDMAC_GRWS.w = (XDMAC_GRWS_RWS0_Msk << (chanObj->channelID));
}

//******************************************************************************
/* Function:
    void SYS_DMA_ChannelResume ( SYS_DMA_CHANNEL_HANDLE handle )

  Summary:
    Resume DMA operation on the specified DMA channel.

  Description:
    This function resumes DMA operation on the specified DMA channel.

  Remarks:
    Not all features are available on all micro-controllers.
*/

void SYS_DMA_ChannelResume ( SYS_DMA_CHANNEL_HANDLE handle )
{
    SYS_DMA_CHANNEL_OBJECT *chanObj;

    chanObj = (SYS_DMA_CHANNEL_OBJECT *) handle;

    /* Resume the read and write transactions on channel specified */
    _XDMAC_REGS->XDMAC_GRWR.w = (XDMAC_GRWR_RWR0_Msk << (chanObj->channelID));
}


// *****************************************************************************
// *****************************************************************************
// Section: Global DMA Management Routines
// *****************************************************************************
// *****************************************************************************
//******************************************************************************
//******************************************************************************
/* Function:
    void SYS_DMA_Suspend ( void )

  Summary:
    Suspend DMA transfers.

  Description:
    This function suspends DMA transfers to allow CPU uninterrupted access
    to data bus

  Remarks:
    None
*/

void SYS_DMA_Suspend ( void )
{
    /* Suspend transfer on all channels */
    _XDMAC_REGS->XDMAC_GRWS.w = XDMAC_GRWS_Msk;
}

//******************************************************************************
/* Function:
    void SYS_DMA_Resume ( void )

  Summary:
    Resume DMA operations

  Description:
    This function disables DMA suspend. It resumes the DMA operation suspended
    by calling SYS_DMA_Suspend. The DMA operates normally.

  Remarks:
    None
*/
void SYS_DMA_Resume ( void )
{
    /* Resume transfer on all channels */
    _XDMAC_REGS->XDMAC_GRWR.w = XDMAC_GRWR_Msk;
}


//******************************************************************************
/* Function:
    bool SYS_DMA_IsBusy ( void )

  Summary:
    Returns the busy status of the DMA module

  Description:
    This function returns the busy status of the DMA module

  Remarks:
    Not all features are available on all micro-controllers.
*/
bool SYS_DMA_IsBusy ( void )
{
    bool isBusy = false;
    DMA_CHANNEL channels;

    /* Go through each DMA channel and check the status as
     * XDMAC doesn't have global status.
     * Even if single channel is busy then return busy status as true */
    for (channels = DMA_CHANNEL_0; channels < SYS_DMA_CHANNEL_COUNT; channels++)
    {
        /*Check if channel write/read is in progress */
        if ( (1 == _XDMAC_REGS->XDMAC_CHID[channels].XDMAC_CC.WRIP) || (1 == _XDMAC_REGS->XDMAC_CHID[channels].XDMAC_CC.RDIP) )
        {
            isBusy = true;
            break;
        }
    }

    return isBusy;
}

// *****************************************************************************
// *****************************************************************************
// Section: Local functions.
// *****************************************************************************
// *****************************************************************************

void _SYS_DMA_PeripheralSyncChannelSetup( SYS_DMA_CHANNEL_OBJECT *chanObj )
{
    DMA_CHANNEL channelIndex;

    channelIndex = chanObj->channelID;

    /* Peripheral synchronized channel setup */
    _XDMAC_REGS->XDMAC_CHID[channelIndex].XDMAC_CC.TYPE = XDMAC_CC_TYPE_PER_TRAN_Val ;

    /* Select trigger to Hardware connected */
    _XDMAC_REGS->XDMAC_CHID[channelIndex].XDMAC_CC.SWREQ = XDMAC_CC_SWREQ_HWR_CONNECTED_Val ;

    /* Set selected peripheral as a trigger source */
    _XDMAC_REGS->XDMAC_CHID[channelIndex].XDMAC_CC.PERID = (chanObj->channelTrigger & SYS_DMA_PERID_MASK);

    /* Set up the "peripheral to memory"  or "memory to peripheral"
     * transfer based on higher byte of the DMA_TRIGGER_SOURCE */
    if ( (chanObj->channelTrigger) & (~SYS_DMA_PERID_MASK) )
    {
        /* It's a "memory to peripheral", that is Transmit type */

        /* Transfer direction is from memory to peripheral */
        _XDMAC_REGS->XDMAC_CHID[channelIndex].XDMAC_CC.DSYNC = XDMAC_CC_DSYNC_MEM2PER_Val ;

        /* As peripheral is the destination, set fixed addressing
         * mode for the destination */
        _XDMAC_REGS->XDMAC_CHID[channelIndex].XDMAC_CC.DAM = XDMAC_CC_DAM_FIXED_AM_Val ;
    }
    else
    {
        /* It's a "peripheral to memory", that is Receive type */

        /* Transfer direction is from peripheral to memory */
        _XDMAC_REGS->XDMAC_CHID[channelIndex].XDMAC_CC.DSYNC = XDMAC_CC_DSYNC_PER2MEM_Val ;

        /* As peripheral is the source, set fixed addressing
         * mode for the source */
        _XDMAC_REGS->XDMAC_CHID[channelIndex].XDMAC_CC.SAM = XDMAC_CC_SAM_FIXED_AM_Val ;
    }
}

void _SYS_DMA_PeripheralSyncTransferSetup ( SYS_DMA_CHANNEL_OBJECT *chanObj, size_t srcSize, size_t destSize )
{
    DMA_CHANNEL channelIndex;

    channelIndex = chanObj->channelID;

    /* Set up the "peripheral to memory"  or "memory to peripheral"
     * transfer based on higher byte of the DMA_TRIGGER_SOURCE */
    if ( (chanObj->channelTrigger) & (~SYS_DMA_PERID_MASK) )
    {
        /* It's a "memory to peripheral", that is Transmit type */

        /* Data width, 8/16/32 bit selection.
         * By default it's byte, check for other two options */
        if (SYS_DMA_DATA_WIDTH_HALF_WORD == destSize)
        {
            _XDMAC_REGS->XDMAC_CHID[channelIndex].XDMAC_CC.DWIDTH = XDMAC_CC_DWIDTH_HALFWORD_Val;

            /* Make "srcSize" in bytes format to word format of XDMAC.
             * In this case divide size by 2 */
            srcSize = srcSize >> 1;
        }
        else
        {
            if (SYS_DMA_DATA_WIDTH_WORD == destSize)
            {
                _XDMAC_REGS->XDMAC_CHID[channelIndex].XDMAC_CC.DWIDTH = XDMAC_CC_DWIDTH_WORD_Val;

                /* Make "srcSize" in bytes format to word format of XDMAC.
                 * In this case divide size by 4 */
                srcSize = srcSize >> 2;
            }
        }

        /* Transfer size is the size of source, which is memory */
        _XDMAC_REGS->XDMAC_CHID[channelIndex].XDMAC_CUBC.UBLEN = srcSize;
    }
    else
    {
        /* It's a "peripheral to memory", that is Receive type */

        /* Data width, 8/16/32 bit selection.
         * By default it's byte, check for other two options */
        if (SYS_DMA_DATA_WIDTH_HALF_WORD == srcSize)
        {
            _XDMAC_REGS->XDMAC_CHID[channelIndex].XDMAC_CC.DWIDTH = XDMAC_CC_DWIDTH_HALFWORD_Val;

            /* Make "destSize" in bytes format to word format of XDMAC.
             * In this case divide size by 2 */
            destSize = destSize >> 1;
        }
        else
        {
            if(SYS_DMA_DATA_WIDTH_WORD == srcSize)
            {
                _XDMAC_REGS->XDMAC_CHID[channelIndex].XDMAC_CC.DWIDTH = XDMAC_CC_DWIDTH_WORD_Val;

                /* Make "destSize" in bytes format to word format of XDMAC.
                 * In this case divide size by 4 */
                destSize = destSize >> 2;
            }
        }

        /* Transfer size is the size of destination, which is memory */
        _XDMAC_REGS->XDMAC_CHID[channelIndex].XDMAC_CUBC.UBLEN = destSize;
    }
}

void _SYS_DMA_ChannelTasks ( DMA_CHANNEL activeChannel )
{
    SYS_DMA_CHANNEL_OBJECT *chanObj;
    volatile uint32_t chanIntStatus;

    /* Yet to implement logic for DMA_CHANNEL_ANY */
    /* TODO */

    /* Read the interrupt status for the active DMA channel */
    chanIntStatus = _XDMAC_REGS->XDMAC_CHID[activeChannel].XDMAC_CIS.w;

    /* Find out the channel object and give a callback */
    chanObj = (SYS_DMA_CHANNEL_OBJECT *) &gSysDMAChannelObj[activeChannel];

    /* Check if it's a channel error interrupt */
    if (chanIntStatus & ( XDMAC_CIS_RBEIS_Msk | XDMAC_CIS_WBEIS_Msk | XDMAC_CIS_ROIS_Msk))
    {
        if (chanIntStatus & XDMAC_CIS_ROIS_Msk)
        {
            /* It's a request overflow error */
            chanObj->errorInfo = SYS_DMA_ERROR_REQUEST_OVERFLOW_ERROR;
        }
        else
        {
            /* It's a Read/Write bus error */
            chanObj->errorInfo = SYS_DMA_ERROR_BUS_ERROR;
        }

        /* If event handler is registered, then give a callback */
        if (NULL != chanObj->pEventCallBack)
        {
            chanObj->pEventCallBack(SYS_DMA_TRANSFER_EVENT_ERROR, (SYS_DMA_CHANNEL_HANDLE)chanObj, chanObj->hClientArg);
        }

    }

    /* Check if it's a block transfer complete interrupt */
    else
    {
        if (chanIntStatus & XDMAC_CIS_BIS_Msk)
        {
            /* Channel is by default disabled on completion of a block transfer
             * for basic operation mode */

            chanObj->errorInfo = SYS_DMA_ERROR_NONE;

            if (NULL != chanObj->pEventCallBack)
            {
                chanObj->pEventCallBack(SYS_DMA_TRANSFER_EVENT_COMPLETE, (SYS_DMA_CHANNEL_HANDLE)chanObj, chanObj->hClientArg);
            }
        }
        else
        {
            /* Do nothing */
        }
    }
}

void _SYS_DMA_ChannelEnable ( DMA_CHANNEL channelID )
{
    /* Check and invalidate data cache prior to enabling DMA 
     * transfer to avoid coherencey */
    if( (SCB->CCR & SCB_CCR_DC_Msk) == SCB_CCR_DC_Msk)
    {
        SCB_CleanInvalidateDCache();
    }
    
    _XDMAC_REGS->XDMAC_GE.w = (XDMAC_GE_EN0_Msk << channelID);
}

/*******************************************************************************
 End of File
*/
