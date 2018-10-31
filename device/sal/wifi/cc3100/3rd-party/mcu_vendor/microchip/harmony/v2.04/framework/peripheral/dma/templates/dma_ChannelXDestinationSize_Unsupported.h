/*******************************************************************************
  DMA Peripheral Library Template Implementation

  File Name:
    dma_ChannelXDestinationSize_Unsupported.h

  Summary:
    DMA PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : ChannelXDestinationSize
    and its Variant : Unsupported
    For following APIs :
        PLIB_DMA_ExistsChannelXDestinationSize
        PLIB_DMA_ChannelXDestinationSizeGet
        PLIB_DMA_ChannelXDestinationSizeSet

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

#ifndef _DMA_CHANNELXDESTINATIONSIZE_UNSUPPORTED_H
#define _DMA_CHANNELXDESTINATIONSIZE_UNSUPPORTED_H

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
/* Function :  DMA_ExistsChannelXDestinationSize_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_DMA_ExistsChannelXDestinationSize

  Description:
    This template implements the Unsupported variant of the PLIB_DMA_ExistsChannelXDestinationSize function.
*/

PLIB_TEMPLATE bool DMA_ExistsChannelXDestinationSize_Unsupported( DMA_MODULE_ID index )
{
    return false;
}


//******************************************************************************
/* Function :  DMA_ChannelXDestinationSizeGet_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_DMA_ChannelXDestinationSizeGet 

  Description:
    This template implements the Unsupported variant of the PLIB_DMA_ChannelXDestinationSizeGet function.
*/

PLIB_TEMPLATE uint16_t DMA_ChannelXDestinationSizeGet_Unsupported( DMA_MODULE_ID index , DMA_CHANNEL channel )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_DMA_ChannelXDestinationSizeGet");
    
    return 0;
}


//******************************************************************************
/* Function :  DMA_ChannelXDestinationSizeSet_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_DMA_ChannelXDestinationSizeSet 

  Description:
    This template implements the Unsupported variant of the PLIB_DMA_ChannelXDestinationSizeSet function.
*/

PLIB_TEMPLATE void DMA_ChannelXDestinationSizeSet_Unsupported( DMA_MODULE_ID index , DMA_CHANNEL channel , uint16_t destinationSize )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_DMA_ChannelXDestinationSizeSet");
}


#endif /*_DMA_CHANNELXDESTINATIONSIZE_UNSUPPORTED_H*/

/******************************************************************************
 End of File
*/

