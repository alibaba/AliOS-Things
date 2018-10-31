/*******************************************************************************
  DMA Peripheral Library Template Implementation

  File Name:
    dma_ChannelXTrigger_Unsupported.h

  Summary:
    DMA PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : ChannelXTrigger
    and its Variant : Unsupported
    For following APIs :
        PLIB_DMA_ExistsChannelXTrigger
        PLIB_DMA_ChannelXTriggerEnable
        PLIB_DMA_ChannelXTriggerIsEnabled
        PLIB_DMA_ChannelXTriggerDisable

*******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2012 released Microchip Technology Inc.  All rights reserved.

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

#ifndef _DMA_CHANNELXTRIGGER_UNSUPPORTED_H
#define _DMA_CHANNELXTRIGGER_UNSUPPORTED_H

//******************************************************************************
/* Routines available for accessing VREGS, MASKS, POS, LEN are 

  VREGs: 
    None.

  MASKs: 
    None.

  POSs: 
    None.

  LENs: 
    None.

*/


//******************************************************************************
/* Function :  DMA_ExistsChannelXTrigger_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_DMA_ExistsChannelXTrigger

  Description:
    This template implements the Unsupported variant of the PLIB_DMA_ExistsChannelXTrigger function.
*/

PLIB_TEMPLATE bool DMA_ExistsChannelXTrigger_Unsupported( DMA_MODULE_ID index )
{
    return false;
}


//******************************************************************************
/* Function :  DMA_ChannelXTriggerEnable_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_DMA_ChannelXTriggerEnable 

  Description:
    This template implements the Unsupported variant of the PLIB_DMA_ChannelXTriggerEnable function.
*/

PLIB_TEMPLATE void DMA_ChannelXTriggerEnable_Unsupported( DMA_MODULE_ID index , DMA_CHANNEL channel , DMA_CHANNEL_TRIGGER_TYPE trigger )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_DMA_ChannelXTriggerEnable");
}


//******************************************************************************
/* Function :  DMA_ChannelXTriggerIsEnabled_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_DMA_ChannelXTriggerIsEnabled 

  Description:
    This template implements the Unsupported variant of the PLIB_DMA_ChannelXTriggerIsEnabled function.
*/

PLIB_TEMPLATE bool DMA_ChannelXTriggerIsEnabled_Unsupported( DMA_MODULE_ID index , DMA_CHANNEL channel , DMA_CHANNEL_TRIGGER_TYPE trigger )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_DMA_ChannelXTriggerIsEnabled");
    
    return false;
}


//******************************************************************************
/* Function :  DMA_ChannelXTriggerDisable_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_DMA_ChannelXTriggerDisable 

  Description:
    This template implements the Unsupported variant of the PLIB_DMA_ChannelXTriggerDisable function.
*/

PLIB_TEMPLATE void DMA_ChannelXTriggerDisable_Unsupported( DMA_MODULE_ID index , DMA_CHANNEL channel , DMA_CHANNEL_TRIGGER_TYPE trigger )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_DMA_ChannelXTriggerDisable");
}


#endif /*_DMA_CHANNELXTRIGGER_UNSUPPORTED_H*/

/******************************************************************************
 End of File
*/

