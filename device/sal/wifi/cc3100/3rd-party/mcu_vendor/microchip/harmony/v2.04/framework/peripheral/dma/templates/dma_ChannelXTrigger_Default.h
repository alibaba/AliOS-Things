/*******************************************************************************
  DMA Peripheral Library Template Implementation

  File Name:
    dma_ChannelXTrigger_Default.h

  Summary:
    DMA PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : ChannelXTrigger
    and its Variant : Default
    For following APIs :
        PLIB_DMA_ExistsChannelXTrigger
        PLIB_DMA_ChannelXTriggerEnable
        PLIB_DMA_ChannelXTriggerIsEnabled
        PLIB_DMA_ChannelXTriggerDisable
        PLIB_DMA_ChannelXTriggerSourceNumberGet

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

SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
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

#ifndef _DMA_CHANNELXTRIGGER_DEFAULT_H
#define _DMA_CHANNELXTRIGGER_DEFAULT_H

#include "dma_Registers.h"

//******************************************************************************
/* Function :  DMA_ExistsChannelXTrigger_Default

  Summary:
    Implements Default variant of PLIB_DMA_ExistsChannelXTrigger

  Description:
    This template implements the Default variant of the PLIB_DMA_ExistsChannelXTrigger function.
    Operation is atomic.
*/

#define PLIB_DMA_ExistsChannelXTrigger PLIB_DMA_ExistsChannelXTrigger
PLIB_TEMPLATE bool DMA_ExistsChannelXTrigger_Default( DMA_MODULE_ID index )
{
    return true;
}


//******************************************************************************
/* Function :  DMA_ChannelXTriggerEnable_Default

  Summary:
    Implements Default variant of PLIB_DMA_ChannelXTriggerEnable

  Description:
    This template implements the Default variant of the PLIB_DMA_ChannelXTriggerEnable function.
    Operation is atomic.
*/

PLIB_TEMPLATE void DMA_ChannelXTriggerEnable_Default( DMA_MODULE_ID index , DMA_CHANNEL channel , DMA_CHANNEL_TRIGGER_TYPE trigger )
{
    volatile dma_ch_register_t *regs = (dma_ch_register_t *)(index + sizeof(dma_register_t) + channel * sizeof(dma_ch_register_t));

    switch (trigger) {
    case DMA_CHANNEL_TRIGGER_TRANSFER_START:
	   regs->DCHxECONSET = _DCH0ECON_SIRQEN_MASK;
	   break;
    case DMA_CHANNEL_TRIGGER_TRANSFER_ABORT:
	   regs->DCHxECONSET = _DCH0ECON_AIRQEN_MASK;
	   break;
    case DMA_CHANNEL_TRIGGER_PATTERN_MATCH_ABORT:
	   regs->DCHxECONSET = _DCH0ECON_PATEN_MASK;
	   break;
    }
}


//******************************************************************************
/* Function :  DMA_ChannelXTriggerIsEnabled_Default

  Summary:
    Implements Default variant of PLIB_DMA_ChannelXTriggerIsEnabled

  Description:
    This template implements the Default variant of the PLIB_DMA_ChannelXTriggerIsEnabled function.
    Operation is atomic.
*/

PLIB_TEMPLATE bool DMA_ChannelXTriggerIsEnabled_Default( DMA_MODULE_ID index , DMA_CHANNEL channel , DMA_CHANNEL_TRIGGER_TYPE trigger )
{
    volatile dma_ch_register_t *regs = (dma_ch_register_t *)(index + sizeof(dma_register_t) + channel * sizeof(dma_ch_register_t));

    switch (trigger) {
    case DMA_CHANNEL_TRIGGER_TRANSFER_START:
	   return (bool)regs->DCHxECON.SIRQEN;
    case DMA_CHANNEL_TRIGGER_TRANSFER_ABORT:
	   return (bool)regs->DCHxECON.AIRQEN;
    case DMA_CHANNEL_TRIGGER_PATTERN_MATCH_ABORT:
	   return (bool)regs->DCHxECON.PATEN;
    }
    return false;
}


//******************************************************************************
/* Function :  DMA_ChannelXTriggerDisable_Default

  Summary:
    Implements Default variant of PLIB_DMA_ChannelXTriggerDisable

  Description:
    This template implements the Default variant of the PLIB_DMA_ChannelXTriggerDisable function.
*/

PLIB_TEMPLATE void DMA_ChannelXTriggerDisable_Default( DMA_MODULE_ID index , DMA_CHANNEL channel , DMA_CHANNEL_TRIGGER_TYPE trigger )
{
    volatile dma_ch_register_t *regs = (dma_ch_register_t *)(index + sizeof(dma_register_t) + channel * sizeof(dma_ch_register_t));

    switch (trigger) {
    case DMA_CHANNEL_TRIGGER_TRANSFER_START:
	   regs->DCHxECONCLR = _DCH0ECON_SIRQEN_MASK;
	   break;
    case DMA_CHANNEL_TRIGGER_TRANSFER_ABORT:
	   regs->DCHxECONCLR = _DCH0ECON_AIRQEN_MASK;
	   break;
    case DMA_CHANNEL_TRIGGER_PATTERN_MATCH_ABORT:
	   regs->DCHxECONCLR = _DCH0ECON_PATEN_MASK;
	   break;
    }
}


//******************************************************************************
/* Function :  DMA_ChannelXTriggerSourceNumberGet_Default

  Summary:
    Implements Default variant of PLIB_DMA_ChannelXTriggerSourceNumberGet

  Description:
    This template implements the Default variant of the PLIB_DMA_ChannelXTriggerSourceNumberGet function.
*/

PLIB_TEMPLATE DMA_CHANNEL_INT_SOURCE DMA_ChannelXTriggerSourceNumberGet_Default( DMA_MODULE_ID index , DMA_CHANNEL channel)
{
	return (DMA_CHANNEL_INT_SOURCE)(DMA_CHANNEL_0_INT_SOURCE + channel);
}


#endif /*_DMA_CHANNELXTRIGGER_DEFAULT_H*/

/******************************************************************************
 End of File
*/

