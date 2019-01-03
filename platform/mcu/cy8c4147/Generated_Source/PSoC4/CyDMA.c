/*******************************************************************************
* File Name: CyDMA.c
* Version 1.10
*
* Description:
*  This file contains all of the global DMA API functions.
*
********************************************************************************
* Copyright 2015, Cypress Semiconductor Corporation. All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "CyDMA.h"
#include "CyLib.h"

/* Default callback function. It is invoked if the user callback function is
* undefined.
*/
static void CyDmaDefaultInterruptCallback(void);

/* Array of pointers to user defined callback functions to be called
* from DMA interrupt.
*/
static cydma_callback_t CyDmaInterruptCallback[CYDMA_CH_NR] = {
    &CyDmaDefaultInterruptCallback, &CyDmaDefaultInterruptCallback,
    &CyDmaDefaultInterruptCallback, &CyDmaDefaultInterruptCallback,
    &CyDmaDefaultInterruptCallback, &CyDmaDefaultInterruptCallback,
    &CyDmaDefaultInterruptCallback, &CyDmaDefaultInterruptCallback
};

/* The live status of all DMA channels usage. Bit i is associated to channel i,
* with i = 0..CYDMA_CH_NR-1. '0' - channel is free. '1' - channel is in use.
* All the channels allocated for DMA instances placed on a schematic are in use
* by default and cannot be freed. 
* DMA_CHANNELS_USED__MASK is generated during the build time and is a bit field
* corresponding to the DMA channels placed on a design schematic.
*/
static uint32 CyDmaChannelUsedMask = DMA_CHANNELS_USED__MASK;


/*******************************************************************************
* Function Name: CyDmaEnable
********************************************************************************
*
* Summary:
*  Sets the default ISR to be called by the DMA interrupt and enables the DMA
*  transfer engine.
*
* Parameters:
*  None.
*                   
* Return:
*  None.
*
* Side effect:
*  Does not affect the channel enable status. Overrides the interrupt vector
*  set prior the first time CyDmaEnable() is called.
*
*******************************************************************************/
void CyDmaEnable(void)
{
    static uint8 initIntrVector = 0u;

    if(0u == initIntrVector)
    {
        CyDmaSetInterruptVector(&CyDmaInterrupt);
        CyIntSetPriority(CYDMA_INTR_NUMBER, CYDMA_INTR_PRIO);
        initIntrVector = 1u;
    }
    CYDMA_CTL_REG = CYDMA_ENABLED;
}


/*******************************************************************************
* Function Name: CyDmaDisable
********************************************************************************
*
* Summary:
*  Disables the DMA transfer engine.
*
* Parameters:
*  None.
*                   
* Return:
*  None.
*
* Side effect:
*  Does not affect the channel enable status.
*
*******************************************************************************/
void CyDmaDisable(void)
{
    CYDMA_CTL_REG = (uint32)~CYDMA_ENABLED;
}


/*******************************************************************************
* Function Name: CyDmaGetStatus
********************************************************************************
*
* Summary:
*  Returns the status of the DMA transfer engine.
*
* Parameters:
*  None.
*                   
* Return:
*  The contents of the DMA status register. Refer to the status register bit
*  definitions in CyDMA_P4.h or the API description in the DMA channel
*  datasheet.
*
* Side effect:
*  The series of function calls CyDmaGetActiveSrcAddress(),
*  CyDmaGetActiveDstAddress(), CyDmaGetStatus() are not atomic, the DMA
*  engine may have advanced after one or more of these function calls. Meaning
*  the returns from these three functions may not be related to each other.
*
*******************************************************************************/
uint32 CyDmaGetStatus(void)
{
    return (CYDMA_STATUS_REG);
}


/*******************************************************************************
* Function Name: CyDmaGetActiveChannels
********************************************************************************
*
* Summary:
*  Returns a bit field of all the DMA channels that are either active or
*  pending.
*
* Parameters:
*  None.
*                   
* Return:
*  Bit field of active and pending channels. 
*
*******************************************************************************/
uint32 CyDmaGetActiveChannels(void)
{
    return (CYDMA_STATUS_CH_ACT_REG);
}


/*******************************************************************************
* Function Name: CyDmaGetActiveSrcAddress
********************************************************************************
*
* Summary:
*  Returns the source address currently being used by the DMA transfer engine.
*  This function can be used to debug the DMA, or observe where it is at in a
*  transfer. It will not be used in normal DMA operation.
*
* Parameters:
*  None.
*                   
* Return:
*  Source address currently being used by the DMA transfer engine.
*
* Side effect:
*  The series of function calls CyDmaGetActiveSrcAddress(),
*  CyDmaGetActiveDstAddress(), CyDmaGetStatus() are not atomic, the DMA
*  engine may have advanced after one or more of these function calls. Meaning
*  the returns from these three functions may not be related to each other.
*
*******************************************************************************/
void * CyDmaGetActiveSrcAddress(void)
{
    return ((void *)CYDMA_STATUS_SRC_ADDR_REG);
}


/*******************************************************************************
* Function Name: CyDmaGetActiveDstAddress
********************************************************************************
*
* Summary:
*  Returns the destination address currently being used by the DMA transfer
*  engine. This function can be used to debug the DMA, or observe where it is at
*  in a transfer. It will not be used in normal DMA operation.
*
* Parameters:
*  None.
*                   
* Return:
*  Destination address currently being used by the DMA transfer engine.
*
* Side effect:
*  The series of function calls CyDmaGetActiveSrcAddress(),
*  CyDmaGetActiveDstAddress(), CyDmaGetStatus() are not atomic, the DMA
*  engine may have advanced after one or more of these function calls. Meaning
*  the returns from these three functions may not be related to each other.
*
*******************************************************************************/
void * CyDmaGetActiveDstAddress(void)
{
    return ((void *)CYDMA_STATUS_DST_ADDR_REG);
}


/*******************************************************************************
* Function Name: CyDmaSetInterruptVector
********************************************************************************
*
* Summary:
*  Sets the function that will be called by the DMA interrupt. Note, calling
*  CyDmaEnable() for the first time overrides any effect this API could have.
*  Call CyDmaSetInterruptVector() after calling CyDmaEnable() to change
*  the default ISR.
*
* Parameters:
*  interruptVector: Address of the function that will be called by the DMA
*                   interrupt.
* Return:
*  None.
*
* Side effect:
*  Other components that use DMA may register their callback functions to be
*  called from the default DMA ISR. Therefore, changing the DMA ISR to a user
*  defined ISR using this API may prevent these other components from
*  functioning correctly.
*
*******************************************************************************/
void CyDmaSetInterruptVector(cyisraddress interruptVector)
{
    (void)CyIntSetVector(CYDMA_INTR_NUMBER, interruptVector);
}


/*******************************************************************************
* Function Name: CyDmaGetInterruptSource
********************************************************************************
*
* Summary:
*  Returns the bit field of which channels generated an interrupt request.
*
* Parameters:
*  None.
*                   
* Return:
*  Bit filed of which channels generated an interrupt request.
*
*******************************************************************************/
uint32 CyDmaGetInterruptSource(void)
{
    return (CYDMA_INTR_REG);
}


/*******************************************************************************
* Function Name: CyDmaClearInterruptSource
********************************************************************************
*
* Summary:
*  Clears the pending interrupts.
*
* Parameters:
*  interruptMask: Bit field of interrupts to clear.
*                   
* Return:
*  None.
*
*******************************************************************************/
void CyDmaClearInterruptSource(uint32 interruptMask)
{
    CYDMA_INTR_REG = interruptMask;
}


/*******************************************************************************
* Function Name: CyDmaSetInterruptSourceMask
********************************************************************************
*
* Summary:
*  Sets mask for interrupt source.
*
* Parameters:
*  interruptmask: Mask corresponding to interrupt bit field.
*                   
* Return:
*  None.
*
*******************************************************************************/
void CyDmaSetInterruptSourceMask(uint32 interruptMask)
{
    CYDMA_INTR_MASK_REG = interruptMask;
}


/*******************************************************************************
* Function Name: CyDmaGetInterruptSourceMask
********************************************************************************
*
* Summary:
*  Returns mask for interrupt source.
*
* Parameters:
*  None.
*                   
* Return:
*  Mask corresponding to interrupt bit field.
*
*******************************************************************************/
uint32 CyDmaGetInterruptSourceMask(void)
{
    return (CYDMA_INTR_MASK_REG);
}


/*******************************************************************************
* Function Name: CyDmaGetInterruptSourceMasked
********************************************************************************
*
* Summary:
*  Returns the bitwise AND of the interrupt source and the interrupt mask.
*
* Parameters:
*  None.
*                   
* Return:
*  Bitwise AND of the interrupt source and the interrupt mask.
*
*******************************************************************************/
uint32 CyDmaGetInterruptSourceMasked(void)
{
    return (CYDMA_INTR_MASKED_REG);
}


/*******************************************************************************
* Function Name: CyDmaSetInterruptCallback
********************************************************************************
*
* Summary:
*  Sets a user defined callback function to be called by the DMA interrupt. The
*  function should contain code to process the interrupt request for the 
*  associated DMA channel.
*
* Parameters:
*  channel:  Channel used by this function.
*  callback: Pointer to the user defined callback function.
*
* Return:
*  Pointer to the function previously set for the specified channel. 
*
*******************************************************************************/
cydma_callback_t CyDmaSetInterruptCallback(int32 channel, cydma_callback_t callback)
{
    cydma_callback_t prevCallback;

    CYASSERT((channel >= 0) && (channel < CYDMA_CH_NR));

    prevCallback = CyDmaInterruptCallback[channel];
    CyDmaInterruptCallback[channel] = callback;
    return (prevCallback);
}


/*******************************************************************************
* Function Name: CyDmaGetInterruptCallback
********************************************************************************
*
* Summary:
*  Returns the pointer to the interrupt callback function for the specified DMA
*  channel.
*
* Parameters:
*  channel: Channel used by this function.
*
* Return:
*  Callback function pointer for the specified channel.
*
*******************************************************************************/
cydma_callback_t CyDmaGetInterruptCallback(int32 channel)
{
    CYASSERT((channel >= 0) && (channel < CYDMA_CH_NR));
    
    return (CyDmaInterruptCallback[channel]);
}


/*******************************************************************************
* Function Name: CyDmaInterrupt
********************************************************************************
*
* Summary:
*  The default ISR for DMA interrupts. The handler checks which DMA channel has
*  triggered the interrupt and calls the user defined callback function.
*  The callback function is set using CyDmaSetInteruptCallback() API.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Side Effects:
*  This function clears the pending interrupts.
*
*******************************************************************************/
CY_ISR(CyDmaInterrupt)
{
    uint32 intr;
    uint32 cbIdx = 0u;

    /* Get interrupt source. */
    intr = CYDMA_INTR_MASKED_REG;

    /* Clear pending interrupts. */
    CYDMA_INTR_REG = intr;

    /* Call user defined callback functions for triggered interrupt sources. */
    while(0u != intr)
    {
        if(0u != (intr & 0x1u))
        {
            CyDmaInterruptCallback[cbIdx]();
        }
        intr >>= 1;
        ++cbIdx;
    }
}


/*******************************************************************************
* Function Name: CyDmaDefaultInterruptCallback
********************************************************************************
*
* Summary:
*  This function is invoked when the user defined callback function is undefined
*  for the interrupt source.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
*******************************************************************************/
static void CyDmaDefaultInterruptCallback(void)
{
    /* Halt CPU in debug mode. We get here because there is no user defined
    * callback function for the interrupt source. You should either set a
    * callback function or disable generating an interrupt from this
    * interrupt source.
    * To set a callback, use the CyDmaSetInterruptCallback() API function.
    * To disable interrupt generation, write '0' to the corresponding bit
    * in the interrupt mask by calling CyDmaSetInterruptSourceMask().
    */
    CYASSERT(0 != 0);
}


/*******************************************************************************
* Function Name: CyDmaChAlloc
********************************************************************************
*
* Summary:
*  Allocates a channel number to use with later DMA functions.
*
* Parameters:
*  None.
*                   
* Return:
*  The allocated channel number. Zero is a valid channel number.
*  CYDMA_INVALID_CHANNEL is returned if there are no channels available.
*
*******************************************************************************/
int32 CyDmaChAlloc(void)
{
    int32  channel = CYDMA_CH_NR-1;
    uint32 channelMask = (uint32)(1UL << (CYDMA_CH_NR-1));
    
    while (channel >= 0)
    {
        if(0u == (CyDmaChannelUsedMask & channelMask))
        {
            /* Mark channel as used and exit loop. */ 
            CyDmaChannelUsedMask |= channelMask;
            break;
        }
        channelMask >>= 1;
        --channel;
    }
    return (channel);
}


/*******************************************************************************
* Function Name: CyDmaChFree
********************************************************************************
*
* Summary:
*  Frees a channel handle allocated by CyDmaChAlloc(). Channels allocated for
*  DMA instances placed on a schematic cannot be freed.
*
* Parameters:
*  channel: The handle previously returned by CyDmaChAlloc().
*                   
* Return:
*  CYRET_SUCCESS if successful.
*  CYRET_BAD_PARAM if channel is invalid.
*
*******************************************************************************/
cystatus CyDmaChFree(int32 channel)
{
    uint32   channelMask;
    cystatus result = CYRET_BAD_PARAM;

    CYASSERT((channel >= 0) && (channel < CYDMA_CH_NR));
    
    channelMask = (uint32)(1UL << channel);

    /* Check against attempt to free channel allocated for DMA instance
    * placed on schematic.
    */
    if(0u == (channelMask & DMA_CHANNELS_USED__MASK))
    {
        CyDmaChannelUsedMask &= (uint32)~channelMask;
        result = CYRET_SUCCESS;
    }
    
    return (result);
}


/*******************************************************************************
* Function Name: CyDmaChEnable
********************************************************************************
*
* Summary:
*  Enables the DMA channel.
*
* Parameters:
*  channel: Channel used by this function.
*                   
* Return:
*  None.
*
* Side effect:
*  If this function is called before CyDmaSetConfiguration(), and
*  CyDmaSetSrcAddress() and CyDmaSetDstAddress(), the operation of
*  the DMA is undefined and could result in system data corruption. 
*
*******************************************************************************/
void CyDmaChEnable(int32 channel)
{
    CYASSERT((channel >= 0) && (channel < CYDMA_CH_NR));

    CYDMA_CH_CTL_BASE.ctl[channel] |= CYDMA_ENABLED;
}


/*******************************************************************************
* Function Name: CyDmaChDisable
********************************************************************************
*
* Summary:
*  Disables the DMA channel.
*
* Parameters:
*  channel: Channel used by this function.
*                   
* Return:
*  None.
*
* Side effect:
*  If this function is called during a DMA transfer the transfer is aborted. 
*
*******************************************************************************/
void CyDmaChDisable(int32 channel)
{
    CYASSERT((channel >= 0) && (channel < CYDMA_CH_NR));

    CYDMA_CH_CTL_BASE.ctl[channel] &= (uint32)~CYDMA_ENABLED;
}


/*******************************************************************************
* Function Name: CyDmaTrigger
********************************************************************************
*
* Summary:
*  Triggers the DMA channel, this is useful only for software triggers. This is
*  not needed when hardware triggers are used.
*
* Parameters:
*  channel: Channel used by this function.
*                   
* Return:
*  None. 
*
*******************************************************************************/
void CyDmaTrigger(int32 channel)
{
    /* The LUT is device-dependent, thus it is generated during the build process. */
    static const uint32 trCtlLut[] = {0xC0020000U};

    CYASSERT((channel >= 0) && (channel < CYDMA_CH_NR));

    CYDMA_TR_CTL_REG = trCtlLut[(uint32)channel >> CYDMA_TR_GROUP_SHR] | ((uint32)channel & CYDMA_TR_SEL_MASK);
}


/*******************************************************************************
* Function Name: CyDmaSetPriority
********************************************************************************
*
* Summary:
*  Sets the priority for the channel.
*
* Parameters:
*  channel:  Channel used by this function
*  priority: Priority for channel. Priority can be 0,1,2, or 3.
*            0 is the highest priority.
*                   
* Return:
*  None.
*
* Side effect:
*  This function should not be called while the channel is enabled.
*
*******************************************************************************/
void CyDmaSetPriority(int32 channel, int32 priority)
{
    uint32 regValue;

    CYASSERT((channel  >= 0) && (channel  <  CYDMA_CH_NR));
    CYASSERT((priority >= 0) && (priority <= CY_DMA_MAX_PRIO));

    regValue = CYDMA_CH_CTL_BASE.ctl[channel] & (uint32)~CYDMA_PRIO;
    CYDMA_CH_CTL_BASE.ctl[channel] = regValue | (uint32)((uint32)priority << CYDMA_PRIO_POS);
}


/*******************************************************************************
* Function Name: CyDmaGetPriority
********************************************************************************
*
* Summary:
*  Gets the priority for the channel.
*
* Parameters:
*  channel: Channel used by this function.
*                   
* Return:
*  Priority for channel. Priority can be 0,1,2, or 3. 0 is the highest priority.
*
*******************************************************************************/
int32 CyDmaGetPriority(int32 channel)
{
    uint32 priority;

    CYASSERT((channel >= 0) && (channel < CYDMA_CH_NR));

    priority = (CYDMA_CH_CTL_BASE.ctl[channel] & CYDMA_PRIO) >> CYDMA_PRIO_POS;
    return ((int32)priority);
}


/*******************************************************************************
* Function Name: CyDmaSetNextDescriptor
********************************************************************************
*
* Summary:
*  Sets the descriptor that should be run the next time the channel is
*  triggered.
*
* Parameters:
*  channel:    Channel used by this function.
*  descriptor: Next Descriptor to run (0 or 1).
*
* Return:
*  None.
*                   
* Side effect:
*  This function should not be called while the channel is enabled.
*
*******************************************************************************/
void CyDmaSetNextDescriptor(int32 channel, int32 descriptor)
{
    uint32 regValue;

    CYASSERT((channel    >= 0) && (channel    < CYDMA_CH_NR));
    CYASSERT((descriptor >= 0) && (descriptor < CYDMA_DESCR_NR));

    regValue = CYDMA_CH_CTL_BASE.ctl[channel] & (uint32)~CYDMA_DESCRIPTOR;
    CYDMA_CH_CTL_BASE.ctl[channel] = regValue | (uint32)((uint32)descriptor << CYDMA_DESCRIPTOR_POS);
}


/*******************************************************************************
* Function Name: CyDmaGetNextDescriptor
********************************************************************************
*
* Summary:
*  Returns the next descriptor that should be run, as set by 
*  CyDmaSetNextDescriptor().
*
* Parameters:
*  channel: channel used by this function.
*                   
* Return:
*  Next Descriptor to run (0 or 1).
*
*******************************************************************************/
int32 CyDmaGetNextDescriptor(int32 channel)
{
    uint32 nextDescriptor;

    CYASSERT((channel >= 0) && (channel < CYDMA_CH_NR));

    nextDescriptor = (CYDMA_CH_CTL_BASE.ctl[channel] & CYDMA_DESCRIPTOR) >> CYDMA_DESCRIPTOR_POS;
    return ((int32)nextDescriptor);
}


/*******************************************************************************
* Function Name: CyDmaSetConfiguration
********************************************************************************
*
* Summary:
*  Sets configuration information for the specified descriptor.
*
* Parameters:
*  channel:    DMA channel modified by this function.
*  descriptor: Descriptor (0 or 1) modified by this function.
*  config:     Pointer to a configuration structure. Refer to cydma_init_struct
*              structure definition in CyDMA_P4.h and/or DMA channel datasheet
*              for more information.
*                   
* Return:
*  None.
*
* Side effect:
*  The status register associated with the specified descriptor is reset to
*  zero after this function call. This function also validates the descriptor.
*  This function should not be called while the descriptor is active.
*  This can be checked by calling CyDmaGetStatus().
*
*******************************************************************************/
void CyDmaSetConfiguration(int32 channel, int32 descriptor, const cydma_init_struct * config)
{  
    CYASSERT((channel        >= 0) && (channel         < CYDMA_CH_NR));
    CYASSERT((descriptor     >= 0) && (descriptor      < CYDMA_DESCR_NR));

    CYASSERT(0u == (config->dataElementSize     & (uint32)~CYDMA_DATA_SIZE));   
    CYASSERT((config->numDataElements > 0)     && (config->numDataElements <= CYDMA_MAX_DATA_NR));
    CYASSERT(0u == (config->srcDstTransferWidth & (uint32)~CYDMA_TRANSFER_WIDTH));
    CYASSERT(0u == (config->addressIncrement    & (uint32)~CYDMA_ADDR_INCR));
    CYASSERT(0u == (config->triggerType         & (uint32)~CYDMA_TRIGGER_TYPE));
    CYASSERT(0u == (config->transferMode        & (uint32)~CYDMA_TRANSFER_MODE));
    CYASSERT(0u == (config->preemptable         & (uint32)~CYDMA_PREEMPTABLE));
    CYASSERT(0u == (config->actions             & (uint32)~CYDMA_POST_COMPLETE_ACTIONS));
    
    CYDMA_DESCR_BASE.descriptor[channel][descriptor].ctl = config->dataElementSize     |
                                                  ((uint32)config->numDataElements-1u) |
                                                           config->srcDstTransferWidth |
                                                           config->addressIncrement    |
                                                           config->triggerType         |
                                                           config->transferMode        |
                                                           config->preemptable         |
                                                           config->actions;

    CYDMA_DESCR_BASE.descriptor[channel][descriptor].status = CYDMA_VALID;
}


/*******************************************************************************
* Function Name: CyDmaValidateDescriptor
********************************************************************************
*
* Summary:
*  Validates the specified descriptor after it has been invalidated.
*
* Parameters:
*  channel:    Channel used by this function.
*  descriptor: Descriptor (0 or 1) modified by this function.
*                   
* Return:
*  None. 
*
* Side effect:
*  The status register associated with the specified descriptor is reset to
*  zero after this function call.
*  This function should not be called when the specified descriptor is active
*  in the DMA transfer engine. This can be checked by calling CyDmaGetStatus().
*
*******************************************************************************/
void CyDmaValidateDescriptor(int32 channel, int32 descriptor)
{
    CYASSERT((channel    >= 0) && (channel    < CYDMA_CH_NR));
    CYASSERT((descriptor >= 0) && (descriptor < CYDMA_DESCR_NR));

    CYDMA_DESCR_BASE.descriptor[channel][descriptor].status = CYDMA_VALID;
}


/*******************************************************************************
* Function Name: CyDmaGetDescriptorStatus
********************************************************************************
*
* Summary:
*  Returns the status of the specified descriptor.
*
* Parameters:
*  channel:    Channel used by this function.
*  descriptor: Descriptor (0 or 1) modified by this function.
*                   
* Return:
*  Returns the contents of the specified descriptors status register. Refer to 
*  descriptor status register bit definitions in CyDMA_P4.h or the API
*  description in the DMA channel datasheet for more information.
*
*******************************************************************************/
uint32 CyDmaGetDescriptorStatus(int32 channel, int32 descriptor)
{
    CYASSERT((channel    >= 0) && (channel    < CYDMA_CH_NR));
    CYASSERT((descriptor >= 0) && (descriptor < CYDMA_DESCR_NR));

    return (CYDMA_DESCR_BASE.descriptor[channel][descriptor].status);
}


/*******************************************************************************
* Function Name: CyDmaSetSrcAddress
********************************************************************************
*
* Summary:
*  Configures the source address for the specified descriptor.
*
* Parameters:
*  channel:    Channel used by this function.
*  descriptor: Descriptor (0 or 1) modified by this function.
*  srcAddress: Address of DMA transfer source.
*                   
* Return:
*  None.
*
* Side effect:
*  This function should not be called when the specified descriptor is active
*  in the DMA transfer engine. This can be checked by calling CyDmaGetStatus(). 
*
*******************************************************************************/
void CyDmaSetSrcAddress(int32 channel, int32 descriptor, void * srcAddress)
{
    CYASSERT((channel    >= 0) && (channel    < CYDMA_CH_NR));
    CYASSERT((descriptor >= 0) && (descriptor < CYDMA_DESCR_NR));

    CYDMA_DESCR_BASE.descriptor[channel][descriptor].src = srcAddress;
}


/*******************************************************************************
* Function Name: CyDmaGetSrcAddress
********************************************************************************
*
* Summary:
*  Returns the source address for the specified descriptor, set by 
*  CyDmaSetSrcAddress().
*
* Parameters:
*  channel:    Channel used by this function.
*  descriptor: Specifies descriptor (0 or 1) used by this function.
*                   
* Return:
*  Source address written to specified descriptor.
*
*******************************************************************************/
void * CyDmaGetSrcAddress(int32 channel, int32 descriptor)
{
    CYASSERT((channel    >= 0) && (channel    < CYDMA_CH_NR));
    CYASSERT((descriptor >= 0) && (descriptor < CYDMA_DESCR_NR));

    return (CYDMA_DESCR_BASE.descriptor[channel][descriptor].src);
}


/*******************************************************************************
* Function Name: CyDmaSetDstAddress
********************************************************************************
*
* Summary:
*  Configures the destination address for the specified descriptor.
*
* Parameters:
*  channel:    Channel used by this function.
*  descriptor: Descriptor (0 or 1) modified by this function.
*  dstAddress: Address of DMA transfer destination.
*                   
* Return:
*  None.
*
* Side effect:
*  This function should not be called when the specified descriptor is active
*  in the DMA transfer engine. This can be checked by calling CyDmaGetStatus(). 
*
*******************************************************************************/
void CyDmaSetDstAddress(int32 channel, int32 descriptor, void * dstAddress)
{
    CYASSERT((channel    >= 0) && (channel    < CYDMA_CH_NR));
    CYASSERT((descriptor >= 0) && (descriptor < CYDMA_DESCR_NR));

    CYDMA_DESCR_BASE.descriptor[channel][descriptor].dst = dstAddress;
}


/*******************************************************************************
* Function Name: CyDmaGetDstAddress
********************************************************************************
*
* Summary:
*  Returns the destination address for the specified descriptor, set by 
*  CyDmaSetDstAddress().
*
* Parameters:
*  channel:    Channel used by this function.
*  descriptor: Specifies descriptor (0 or 1) used by this function.
*                   
* Return:
*  Destination address written to specified descriptor.
*
*******************************************************************************/
void * CyDmaGetDstAddress(int32 channel, int32 descriptor)
{
    CYASSERT((channel    >= 0) && (channel    < CYDMA_CH_NR));
    CYASSERT((descriptor >= 0) && (descriptor < CYDMA_DESCR_NR));

    return (CYDMA_DESCR_BASE.descriptor[channel][descriptor].dst);
}

/*******************************************************************************
* Function Name: CyDmaSetDataElementSize
********************************************************************************
*
* Summary:
*  Sets the data element size for the specified descriptor.
*
* Parameters:
*  channel:         Channel used by this function.
*  descriptor:      Descriptor (0 or 1) modified by this function.
*  dataElementSize: Specifies the size of the data element. The DMA transfer
*                   engine transfers one data element at a time. How these
*                   transfers occur is controlled by the transfer mode.
*                   Refer to the API bit field definitions in CyDMA_P4.h
*                   or API description in the DMA channel datasheet for the 
*                   parameter options.
*
* Return:
*  None.
*
* Side effect:
*  This function should not be called when the specified descriptor is active
*  in the DMA transfer engine. This can be checked by calling CyDmaGetStatus(). 
*
*******************************************************************************/
void CyDmaSetDataElementSize(int32 channel, int32 descriptor, uint32 dataElementSize)
{
    uint32 regValue;
    
    CYASSERT((channel    >= 0) && (channel    < CYDMA_CH_NR));
    CYASSERT((descriptor >= 0) && (descriptor < CYDMA_DESCR_NR));
    CYASSERT(0u == (dataElementSize & (uint32)~CYDMA_DATA_SIZE));

    regValue = CYDMA_DESCR_BASE.descriptor[channel][descriptor].ctl & (uint32)~CYDMA_DATA_SIZE;
    CYDMA_DESCR_BASE.descriptor[channel][descriptor].ctl = regValue | dataElementSize;
}


/*******************************************************************************
* Function Name: CyDmaGetDataElementSize
********************************************************************************
*
* Summary:
*  Returns the data element size configured for the specified descriptor, set by
*  CyDmaSetDataElementSize() or CyDmaSetConfiguration.
*
* Parameters:
*  channel:    Channel used by this function.
*  descriptor: Descriptor (0 or 1) modified by this function.
*                   
* Return:
*  The size of the data element. The DMA transfer engine transfers one data
*  element at a time. How these transfers occur is controlled by the transfer
*  mode. Refer to the API bit field definitions in CyDMA_P4.h or the API 
*  description in the DMA channel datasheet for more information.
*
* Note:
*  The size of the source and destination can be configured to use the
*  data element size, or a Word, this is set in CyDmaSetSrcDstTransferWidth().
*
*******************************************************************************/
uint32 CyDmaGetDataElementSize(int32 channel, int32 descriptor)
{
    CYASSERT((channel    >= 0) && (channel    < CYDMA_CH_NR));
    CYASSERT((descriptor >= 0) && (descriptor < CYDMA_DESCR_NR));

    return (CYDMA_DESCR_BASE.descriptor[channel][descriptor].ctl & CYDMA_DATA_SIZE);
}


/*******************************************************************************
* Function Name: CyDmaSetNumDataElements
********************************************************************************
*
* Summary:
*  Sets the number of data elements to transfer for specified descriptor.
*
* Parameters:
*  channel:         Channel used by this function.
*  descriptor:      Descriptor (0 or 1) modified by this function.
*  numDataElements: Total number of data elements this descriptor transfers.
*                   Valid ranges are 1 to 65536.
*                   
* Return:
*  None.
*
* Side effect:
*  This function should not be called when the specified descriptor is active
*  in the DMA transfer engine. This can be checked by calling CyDmaGetStatus(). 
*
*******************************************************************************/
void CyDmaSetNumDataElements(int32 channel, int32 descriptor, int32 numDataElements)
{
    uint32 regValue;
    
    CYASSERT((channel        >= 0) && (channel         <  CYDMA_CH_NR));
    CYASSERT((descriptor     >= 0) && (descriptor      <  CYDMA_DESCR_NR));
    CYASSERT((numDataElements > 0) && (numDataElements <= CYDMA_MAX_DATA_NR));
    
    --numDataElements;
    regValue = CYDMA_DESCR_BASE.descriptor[channel][descriptor].ctl & (uint32)~CYDMA_DATA_NR;
    CYDMA_DESCR_BASE.descriptor[channel][descriptor].ctl = regValue | (uint32)numDataElements;
}


/*******************************************************************************
* Function Name: CyDmaGetNumDataElements
********************************************************************************
*
* Summary:
*  Returns the number of data elements to be transferred. Only reflects the
*  value written by CyDmaSetNumDataElements() or CyDmaSetConfiguration(). This
*  does not reflect how many elements have been transferred. For that
*  information use the CyDmaGetDescriptorStatus() function.
*
* Parameters:
*  channel:    Channel used by this function.
*  descriptor: Descriptor (0 or 1) modified by this function.
*                   
* Return:
*  Number of data elements to transfer.
*
*******************************************************************************/
int32 CyDmaGetNumDataElements(int32 channel, int32 descriptor)
{
    uint32 numElements;

    CYASSERT((channel    >= 0) && (channel    < CYDMA_CH_NR));
    CYASSERT((descriptor >= 0) && (descriptor < CYDMA_DESCR_NR));
    
    numElements = (CYDMA_DESCR_BASE.descriptor[channel][descriptor].ctl & CYDMA_DATA_NR) + 1u;
    return ((int32)numElements);
}


/*******************************************************************************
* Function Name: CyDmaSetSrcDstTransferWidth
********************************************************************************
*
* Summary:
*  Sets the width of the source and destination. The DMA can either read and
*  write data from the source and destination at the size specified by
*  CyDmaSetDataElementSize() or by a word (4bytes). This also determines how
*  many bytes the addresses are incremented if increment source and destination
*  address are turned on.
*
* Parameters:
*  channel:       Channel used by this function.
*  descriptor:    Descriptor (0 or 1) modified by this function.
*  transferWidth: Specifies the source and destination widths. Refer to 
*                 the API bit field definitions in CyDMA_P4.h or the API
*                 description in the DMA channel datasheet for the parameter
*                 options.
*                   
* Return:
*  None.
*
* Side effect:
*  This function should not be called when the specified descriptor is active
*  in the DMA transfer engine. This can be checked by calling CyDmaGetStatus(). 
*
*******************************************************************************/
void CyDmaSetSrcDstTransferWidth(int32 channel, int32 descriptor, uint32 transferWidth)
{
    uint32 regValue;

    CYASSERT((channel    >= 0) && (channel    < CYDMA_CH_NR));
    CYASSERT((descriptor >= 0) && (descriptor < CYDMA_DESCR_NR));
    CYASSERT(0u == (transferWidth & (uint32)~CYDMA_TRANSFER_WIDTH));

    regValue = CYDMA_DESCR_BASE.descriptor[channel][descriptor].ctl & (uint32)~CYDMA_TRANSFER_WIDTH;
    CYDMA_DESCR_BASE.descriptor[channel][descriptor].ctl = regValue | transferWidth;
}


/*******************************************************************************
* Function Name: CyDmaGetSrcDstTransferWidth
********************************************************************************
*
* Summary:
*  Returns the width of the source and destination, as set by 
*  CyDmaSetSrcDstTransferWidth() or CyDmaSetConfiguration().
*
* Parameters:
*  channel:    Channel used by this function.
*  descriptor: Descriptor (0 or 1) modified by this function.
*                   
* Return:
*  Width of source and destination. Refer to the API bit field definitions in
*  CyDMA_P4.h or the API description in the DMA channel datasheet for more
*  information.
*
*******************************************************************************/
uint32 CyDmaGetSrcDstTransferWidth(int32 channel, int32 descriptor)
{
    CYASSERT((channel    >= 0) && (channel    < CYDMA_CH_NR));
    CYASSERT((descriptor >= 0) && (descriptor < CYDMA_DESCR_NR));
    return (CYDMA_DESCR_BASE.descriptor[channel][descriptor].ctl & CYDMA_TRANSFER_WIDTH);
}


/*******************************************************************************
* Function Name: CyDmaSetAddressIncrement
********************************************************************************
*
* Summary:
*  Sets whether the source or destination addresses are incremented after the
*  transfer of each data element. The amount that the source and destination
*  address are incremented is determined by the CyDmaSetSrcDstTransferWidth()
*  function. The addresses will either be incremented by the data element size
*  or by a word (4 bytes).
*
* Parameters:
*  channel:          Channel used by this function.
*  descriptor:       Descriptor (0 or 1) modified by this function.
*  addressIncrement: Address increment settings. Refer to the API bit fields 
*                    definition in CyDMA_P4.h or the API description in the DMA
*                    channel datasheet for the parameter options.
*    
* Return:
*  None.
*
* Side effect:
*  This function should not be called when the specified descriptor is active
*  in the DMA transfer engine. This can be checked by calling CyDmaGetStatus(). 
*
*******************************************************************************/
void CyDmaSetAddressIncrement(int32 channel, int32 descriptor, uint32 addressIncrement)
{
    uint32 regValue;

    CYASSERT((channel    >= 0) && (channel    < CYDMA_CH_NR));
    CYASSERT((descriptor >= 0) && (descriptor < CYDMA_DESCR_NR));
    CYASSERT(0u == (addressIncrement & (uint32)~CYDMA_ADDR_INCR));

    regValue = CYDMA_DESCR_BASE.descriptor[channel][descriptor].ctl & (uint32)~CYDMA_ADDR_INCR;
    CYDMA_DESCR_BASE.descriptor[channel][descriptor].ctl = regValue | addressIncrement;
}


/*******************************************************************************
* Function Name: CyDmaGetAddressIncrement
********************************************************************************
*
* Summary:
*  Returns address increment settings as set by CyDmaSetAddressIncrement() or
*  CyDmaSetConfiguration().
*
* Parameters:
*  channel:    Channel used by this function.
*  descriptor: Descriptor (0 or 1) modified by this function.
*                   
* Return:
*  Address increment settings. Refer to the API bit field definitions in
*  CyDMA_P4.h or the API description in the DMA channel datasheet for more
*  information.
*
*******************************************************************************/
uint32 CyDmaGetAddressIncrement(int32 channel, int32 descriptor)
{
    CYASSERT((channel    >= 0) && (channel    < CYDMA_CH_NR));
    CYASSERT((descriptor >= 0) && (descriptor < CYDMA_DESCR_NR));

    return (CYDMA_DESCR_BASE.descriptor[channel][descriptor].ctl & CYDMA_ADDR_INCR);
}


/*******************************************************************************
* Function Name: CyDmaSetTriggerType
********************************************************************************
*
* Summary:
*  Sets the type of input trigger for the DMA For level sensitive triggers sets
*  how long the DMA waits for the trigger to go low (deactivate) before
*  triggering the channel again.
*
* Parameters:
*  channel:     Channel used by this function.
*  descriptor:  Descriptor (0 or 1) modified by this function.
*  triggerType: Type of DMA trigger. Refer to the API bit field definitions
*               in CyDMA_P4.h or the API description in the DMA channel
*               datasheet for the parameter options.
*                   
* Return:
*  None.
*
* Side effect:
*  This function should not be called when the specified descriptor is active
*  in the DMA transfer engine. This can be checked by calling CyDmaGetStatus(). 
*
*******************************************************************************/
void CyDmaSetTriggerType(int32 channel, int32 descriptor, uint32 triggerType)
{
    uint32 regValue;

    CYASSERT((channel    >= 0) && (channel    < CYDMA_CH_NR));
    CYASSERT((descriptor >= 0) && (descriptor < CYDMA_DESCR_NR));
    CYASSERT(0u == (triggerType & (uint32)~CYDMA_TRIGGER_TYPE));

    regValue = CYDMA_DESCR_BASE.descriptor[channel][descriptor].ctl & (uint32)~CYDMA_TRIGGER_TYPE;
    CYDMA_DESCR_BASE.descriptor[channel][descriptor].ctl = regValue | triggerType;
}


/*******************************************************************************
* Function Name: CyDmaGetTriggerType
********************************************************************************
*
* Summary:
*  Returns the trigger deactivation settings as set by CyDmaSetTriggerType() or
*  CyDmaSetConfiguration().
*
* Parameters:
*  channel:    Channel used by this function.
*  descriptor: Descriptor (0 or 1) modified by this function.
*                   
* Return:
*  Trigger deactivation settings. Refer to the API bit field definitions in
*  CyDMA_P4.h or the API description in the DMA channel datasheet for more
*  information.
*
*******************************************************************************/
uint32 CyDmaGetTriggerType(int32 channel, int32 descriptor)
{
    CYASSERT((channel    >= 0) && (channel    < CYDMA_CH_NR));
    CYASSERT((descriptor >= 0) && (descriptor < CYDMA_DESCR_NR));

    return (CYDMA_DESCR_BASE.descriptor[channel][descriptor].ctl & CYDMA_TRIGGER_TYPE);
}


/*******************************************************************************
* Function Name: CyDmaSetTransferMode
********************************************************************************
*
* Summary:
*  Sets the DMA transfer mode. This determines how the DMA reacts to each trigger
*  event.
*
* Parameters:
*  channel:      Channel used by this function.
*  descriptor:   Descriptor (0 or 1) modified by this function.
*  transferMode: Specifies the DMA transfer mode settings. Refer to the API bit
*                field definitions in CyDMA_P4.h or the API description in the
*                DMA channel datasheet for the parameter options.
*                   
* Return:
*  None.
*
* Side effect:
*  This function should not be called when the specified descriptor is active
*  in the DMA transfer engine. This can be checked by calling CyDmaGetStatus(). 
*
*******************************************************************************/
void CyDmaSetTransferMode(int32 channel, int32 descriptor, uint32 transferMode)
{
    uint32 regValue;

    CYASSERT((channel    >= 0) && (channel    < CYDMA_CH_NR));
    CYASSERT((descriptor >= 0) && (descriptor < CYDMA_DESCR_NR));
    CYASSERT(0u == (transferMode & (uint32)~CYDMA_TRANSFER_MODE));

    regValue = CYDMA_DESCR_BASE.descriptor[channel][descriptor].ctl & (uint32)~CYDMA_TRANSFER_MODE;
    CYDMA_DESCR_BASE.descriptor[channel][descriptor].ctl = regValue | transferMode;
}


/*******************************************************************************
* Function Name: CyDmaGetTransferMode
********************************************************************************
*
* Summary:
*  Returns the transfer mode for the specified descriptor as set by 
*  CyDmaSetTransferMode() or CyDmaSetConfiguration.
*
* Parameters:
*  channel:    Channel used by this function.
*  descriptor: Descriptor (0 or 1) modified by this function.
*                   
* Return:
*  DMA transfer mode settings. Refer to the API bit field definitions in
*  CyDMA_P4.h or the API description in the DMA channel datasheet for more
*  information.
*
*******************************************************************************/
uint32 CyDmaGetTransferMode(int32 channel, int32 descriptor)
{
    CYASSERT((channel    >= 0) && (channel    < CYDMA_CH_NR));
    CYASSERT((descriptor >= 0) && (descriptor < CYDMA_DESCR_NR));

    return (CYDMA_DESCR_BASE.descriptor[channel][descriptor].ctl & CYDMA_TRANSFER_MODE);
}


/*******************************************************************************
* Function Name: CyDmaSetPreemptable
********************************************************************************
*
* Summary:
*  Determines if the descriptor is preemptable.
*
* Parameters:
*  channel:     Channel used by this function.
*  descriptor:  Descriptor (0 or 1) modified by this function.
*  preemptable: Descriptor preemption settings. Refer to the API bit fields
*               definition in CyDMA_P4.h or the API description in the DMA 
*               channel datasheet for the parameter options.
*                   
* Return:
*  None.
*
* Side effect:
*  This function should not be called when the specified descriptor is active
*  in the DMA transfer engine. This can be checked by calling CyDmaGetStatus(). 
*
*******************************************************************************/
void CyDmaSetPreemptable(int32 channel, int32 descriptor, uint32 preemptable)
{
    uint32 regValue;

    CYASSERT((channel    >= 0) && (channel    < CYDMA_CH_NR));
    CYASSERT((descriptor >= 0) && (descriptor < CYDMA_DESCR_NR));
    CYASSERT(0u == (preemptable & (uint32)~CYDMA_PREEMPTABLE));

    regValue = CYDMA_DESCR_BASE.descriptor[channel][descriptor].ctl & (uint32)~CYDMA_PREEMPTABLE;
    CYDMA_DESCR_BASE.descriptor[channel][descriptor].ctl = regValue | preemptable;
}


/*******************************************************************************
* Function Name: CyDmaGetPreemptable
********************************************************************************
*
* Summary:
*  Returns whether or not the descriptor is preemptable.
*
* Parameters:
*  channel:    Channel used by this function.
*  descriptor: Descriptor (0 or 1) modified by this function.
*                   
* Return:
*  Descriptor preemption setting. Refer to the API bit field definitions in
*  CyDMA_P4.h or the API description in the DMA channel datasheet for more
*  information.
*
*******************************************************************************/
uint32 CyDmaGetPreemptable(int32 channel, int32 descriptor)
{
    CYASSERT((channel    >= 0) && (channel    < CYDMA_CH_NR));
    CYASSERT((descriptor >= 0) && (descriptor < CYDMA_DESCR_NR));

    return (CYDMA_DESCR_BASE.descriptor[channel][descriptor].ctl & CYDMA_PREEMPTABLE);
}


/*******************************************************************************
* Function Name: CyDmaSetPostCompletionActions
********************************************************************************
*
* Summary:
*  Determines what occurs after a descriptor completes.
*
* Parameters:
*  channel:    Channel used by this function.
*  descriptor: Descriptor (0 or 1) modified by this function.
*  actions:    Specifies descriptor post completion actions. Refer to the API
*              bit field definitions in CyDMA_P4.h or the API description
*              in the DMA channel datasheet for the parameter options.
*                   
* Return:
*  None.  
*
* Side effect:
*  This function should not be called when the specified descriptor is active
*  in the DMA transfer engine. This can be checked by calling CyDmaGetStatus(). 
*
*******************************************************************************/
void CyDmaSetPostCompletionActions(int32 channel, int32 descriptor, uint32 actions)
{
    uint32 regValue;

    CYASSERT((channel    >= 0) && (channel    < CYDMA_CH_NR));
    CYASSERT((descriptor >= 0) && (descriptor < CYDMA_DESCR_NR));
    CYASSERT(0u == (actions & (uint32)~CYDMA_POST_COMPLETE_ACTIONS));

    regValue = CYDMA_DESCR_BASE.descriptor[channel][descriptor].ctl & (uint32)~CYDMA_POST_COMPLETE_ACTIONS;
    CYDMA_DESCR_BASE.descriptor[channel][descriptor].ctl = regValue | actions;
}


/*******************************************************************************
* Function Name: CyDmaGetPostCompletionActions
********************************************************************************
*
* Summary:
*  Returns the post descriptor action settings as set by 
*  CyDmaSetPostCompletionActions() or CyDmaSetConfiguration().
*
* Parameters:
*  channel:    Channel used by this function.
*  descriptor: Descriptor (0 or 1) modified by this function.
*                   
* Return:
*  Post descriptor actions. Refer to the API bit field definitions in
*  CyDMA_P4.h or the API description in the DMA channel datasheet for more
*  information.
*
*******************************************************************************/
uint32 CyDmaGetPostCompletionActions(int32 channel, int32 descriptor)
{
    CYASSERT((channel    >= 0) && (channel    < CYDMA_CH_NR));
    CYASSERT((descriptor >= 0) && (descriptor < CYDMA_DESCR_NR));

    return (CYDMA_DESCR_BASE.descriptor[channel][descriptor].ctl & CYDMA_POST_COMPLETE_ACTIONS);
}


/* [] END OF FILE */
