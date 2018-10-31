/*******************************************************************************
  DMA Peripheral Library Template Implementation

  File Name:
    dma_ChannelXDisabled_Default.h

  Summary:
    DMA PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : ChannelXDisabled
    and its Variant : Default
    For following APIs :
        PLIB_DMA_ExistsChannelXDisabled
        PLIB_DMA_ChannelXDisabledEnablesEvents
        PLIB_DMA_ChannelXDisabledDisablesEvents

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

#ifndef _DMA_CHANNELXDISABLED_DEFAULT_H
#define _DMA_CHANNELXDISABLED_DEFAULT_H

#include "dma_Registers.h"

//******************************************************************************
/* Function :  DMA_ExistsChannelXDisabled_Default

  Summary:
    Implements Default variant of PLIB_DMA_ExistsChannelXDisabled

  Description:
    This template implements the Default variant of the PLIB_DMA_ExistsChannelXDisabled function.
*/

#define PLIB_DMA_ExistsChannelXDisabled PLIB_DMA_ExistsChannelXDisabled
PLIB_TEMPLATE bool DMA_ExistsChannelXDisabled_Default( DMA_MODULE_ID index )
{
    return true;
}


//******************************************************************************
/* Function :  DMA_ChannelXDisabledEnablesEvents_Default

  Summary:
    Implements Default variant of PLIB_DMA_ChannelXDisabledEnablesEvents

  Description:
    This template implements the Default variant of the PLIB_DMA_ChannelXDisabledEnablesEvents function.
    Operation is atomic.
*/

PLIB_TEMPLATE void DMA_ChannelXDisabledEnablesEvents_Default( DMA_MODULE_ID index , DMA_CHANNEL channel )
{
    volatile dma_ch_register_t *regs = (dma_ch_register_t *)(index + sizeof(dma_register_t) + channel * sizeof(dma_ch_register_t));

    regs->DCHxCONSET = _DCH0CON_CHAED_MASK;
}


//******************************************************************************
/* Function :  DMA_ChannelXDisabledDisablesEvents_Default

  Summary:
    Implements Default variant of PLIB_DMA_ChannelXDisabledDisablesEvents

  Description:
    This template implements the Default variant of the PLIB_DMA_ChannelXDisabledDisablesEvents function.
    Operation is atomic.
*/

PLIB_TEMPLATE void DMA_ChannelXDisabledDisablesEvents_Default( DMA_MODULE_ID index , DMA_CHANNEL channel )
{
    volatile dma_ch_register_t *regs = (dma_ch_register_t *)(index + sizeof(dma_register_t) + channel * sizeof(dma_ch_register_t));

    regs->DCHxCONCLR = _DCH0CON_CHAED_MASK;
}


#endif /*_DMA_CHANNELXDISABLED_DEFAULT_H*/

/******************************************************************************
 End of File
*/

