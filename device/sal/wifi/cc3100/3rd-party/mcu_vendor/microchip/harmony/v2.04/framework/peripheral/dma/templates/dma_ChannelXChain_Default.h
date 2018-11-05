/*******************************************************************************
  DMA Peripheral Library Template Implementation

  File Name:
    dma_ChannelXChain_Default.h

  Summary:
    DMA PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : ChannelXChain
    and its Variant : Default
    For following APIs :
        PLIB_DMA_ExistsChannelXChain
        PLIB_DMA_ChannelXChainToLower
        PLIB_DMA_ChannelXChainToHigher

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

#ifndef _DMA_CHANNELXCHAIN_DEFAULT_H
#define _DMA_CHANNELXCHAIN_DEFAULT_H

#include "dma_Registers.h"

//******************************************************************************
/* Function :  DMA_ExistsChannelXChain_Default

  Summary:
    Implements Default variant of PLIB_DMA_ExistsChannelXChain

  Description:
    This template implements the Default variant of the PLIB_DMA_ExistsChannelXChain function.
*/

#define PLIB_DMA_ExistsChannelXChain PLIB_DMA_ExistsChannelXChain
PLIB_TEMPLATE bool DMA_ExistsChannelXChain_Default( DMA_MODULE_ID index )
{
    return true;
}


//******************************************************************************
/* Function :  DMA_ChannelXChainToLower_Default

  Summary:
    Implements Default variant of PLIB_DMA_ChannelXChainToLower

  Description:
    This template implements the Default variant of the PLIB_DMA_ChannelXChainToLower function.
    Operation is atomic.
*/

PLIB_TEMPLATE void DMA_ChannelXChainToLower_Default( DMA_MODULE_ID index , DMA_CHANNEL channel )
{
    volatile dma_ch_register_t *regs = (dma_ch_register_t *)(index + sizeof(dma_register_t) + channel * sizeof(dma_ch_register_t));

    regs->DCHxCONSET = _DCH0CON_CHCHNS_MASK;
}


//******************************************************************************
/* Function :  DMA_ChannelXChainToHigher_Default

  Summary:
    Implements Default variant of PLIB_DMA_ChannelXChainToHigher

  Description:
    This template implements the Default variant of the PLIB_DMA_ChannelXChainToHigher function.
    Operation is atomic.
*/

PLIB_TEMPLATE void DMA_ChannelXChainToHigher_Default( DMA_MODULE_ID index , DMA_CHANNEL channel )
{
    volatile dma_ch_register_t *regs = (dma_ch_register_t *)(index + sizeof(dma_register_t) + channel * sizeof(dma_ch_register_t));

    regs->DCHxCONCLR = _DCH0CON_CHCHNS_MASK;
}


#endif /*_DMA_CHANNELXCHAIN_DEFAULT_H*/

/******************************************************************************
 End of File
*/

