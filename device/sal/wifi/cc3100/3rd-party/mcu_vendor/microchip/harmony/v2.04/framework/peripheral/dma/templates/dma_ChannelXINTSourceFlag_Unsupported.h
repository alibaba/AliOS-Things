/*******************************************************************************
  DMA Peripheral Library Template Implementation

  File Name:
    dma_ChannelXINTSourceFlag_Unsupported.h

  Summary:
    DMA PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : ChannelXINTSourceFlag
    and its Variant : Unsupported
    For following APIs :
        PLIB_DMA_ExistsChannelXINTSourceFlag
        PLIB_DMA_ChannelXINTSourceFlagGet
        PLIB_DMA_ChannelXINTSourceFlagSet
        PLIB_DMA_ChannelXINTSourceFlagClear

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

#ifndef _DMA_CHANNELXINTSOURCEFLAG_UNSUPPORTED_H
#define _DMA_CHANNELXINTSOURCEFLAG_UNSUPPORTED_H

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
/* Function :  DMA_ExistsChannelXINTSourceFlag_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_DMA_ExistsChannelXINTSourceFlag

  Description:
    This template implements the Unsupported variant of the PLIB_DMA_ExistsChannelXINTSourceFlag function.
*/

PLIB_TEMPLATE bool DMA_ExistsChannelXINTSourceFlag_Unsupported( DMA_MODULE_ID index )
{
    return false;
}


//******************************************************************************
/* Function :  DMA_ChannelXINTSourceFlagGet_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_DMA_ChannelXINTSourceFlagGet 

  Description:
    This template implements the Unsupported variant of the PLIB_DMA_ChannelXINTSourceFlagGet function.
*/

PLIB_TEMPLATE bool DMA_ChannelXINTSourceFlagGet_Unsupported( DMA_MODULE_ID index , DMA_CHANNEL channel , DMA_INT_TYPE dmaINTSource )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_DMA_ChannelXINTSourceFlagGet");
    
    return false;
}


//******************************************************************************
/* Function :  DMA_ChannelXINTSourceFlagSet_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_DMA_ChannelXINTSourceFlagSet 

  Description:
    This template implements the Unsupported variant of the PLIB_DMA_ChannelXINTSourceFlagSet function.
*/

PLIB_TEMPLATE void DMA_ChannelXINTSourceFlagSet_Unsupported( DMA_MODULE_ID index , DMA_CHANNEL channel , DMA_INT_TYPE dmaINTSource )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_DMA_ChannelXINTSourceFlagSet");
}


//******************************************************************************
/* Function :  DMA_ChannelXINTSourceFlagClear_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_DMA_ChannelXINTSourceFlagClear 

  Description:
    This template implements the Unsupported variant of the PLIB_DMA_ChannelXINTSourceFlagClear function.
*/

PLIB_TEMPLATE void DMA_ChannelXINTSourceFlagClear_Unsupported( DMA_MODULE_ID index , DMA_CHANNEL channel , DMA_INT_TYPE dmaINTSource )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_DMA_ChannelXINTSourceFlagClear");
}


#endif /*_DMA_CHANNELXINTSOURCEFLAG_UNSUPPORTED_H*/

/******************************************************************************
 End of File
*/

