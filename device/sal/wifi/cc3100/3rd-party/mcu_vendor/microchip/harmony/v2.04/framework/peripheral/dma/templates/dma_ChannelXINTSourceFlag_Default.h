/*******************************************************************************
  DMA Peripheral Library Template Implementation

  File Name:
    dma_ChannelXINTSourceFlag_Default.h

  Summary:
    DMA PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : ChannelXINTSourceFlag
    and its Variant : Default
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

#ifndef _DMA_CHANNELXINTSOURCEFLAG_DEFAULT_H
#define _DMA_CHANNELXINTSOURCEFLAG_DEFAULT_H

#include "dma_Registers.h"

//******************************************************************************
/* Function :  DMA_ExistsChannelXINTSourceFlag_Default

  Summary:
    Implements Default variant of PLIB_DMA_ExistsChannelXINTSourceFlag

  Description:
    This template implements the Default variant of the PLIB_DMA_ExistsChannelXINTSourceFlag function.
*/

#define PLIB_DMA_ExistsChannelXINTSourceFlag PLIB_DMA_ExistsChannelXINTSourceFlag
PLIB_TEMPLATE bool DMA_ExistsChannelXINTSourceFlag_Default( DMA_MODULE_ID index )
{
    return true;
}


//******************************************************************************
/* Function :  DMA_ChannelXINTSourceFlagGet_Default

  Summary:
    Implements Default variant of PLIB_DMA_ChannelXINTSourceFlagGet

  Description:
    This template implements the Default variant of the PLIB_DMA_ChannelXINTSourceFlagGet function.
    Operation is atomic.
*/

PLIB_TEMPLATE bool DMA_ChannelXINTSourceFlagGet_Default( DMA_MODULE_ID index , DMA_CHANNEL channel , DMA_INT_TYPE dmaINTSource )
{
    volatile dma_ch_register_t *regs = (dma_ch_register_t *)(index + sizeof(dma_register_t) + channel * sizeof(dma_ch_register_t));
    bool intSrcFlag = false;

    switch (dmaINTSource) {
    case DMA_INT_ADDRESS_ERROR:
	intSrcFlag = regs->DCHxINT.CHERIF;
	break;

    case DMA_INT_TRANSFER_ABORT:
	intSrcFlag = regs->DCHxINT.CHTAIF;
	break;

    case DMA_INT_CELL_TRANSFER_COMPLETE:
	intSrcFlag = regs->DCHxINT.CHCCIF;
	break;

    case DMA_INT_BLOCK_TRANSFER_COMPLETE:
	intSrcFlag = regs->DCHxINT.CHBCIF;
	break;

    case DMA_INT_DESTINATION_HALF_FULL:
	intSrcFlag = regs->DCHxINT.CHDHIF;
	break;

    case DMA_INT_DESTINATION_DONE:
	intSrcFlag = regs->DCHxINT.CHDDIF;
	break;

    case DMA_INT_SOURCE_HALF_EMPTY:
	intSrcFlag = regs->DCHxINT.CHSHIF;
	break;

    case DMA_INT_SOURCE_DONE:
	intSrcFlag = regs->DCHxINT.CHSDIF;
	break;
    }

    return intSrcFlag;
}


//******************************************************************************
/* Function :  DMA_ChannelXINTSourceFlagSet_Default

  Summary:
    Implements Default variant of PLIB_DMA_ChannelXINTSourceFlagSet

  Description:
    This template implements the Default variant of the PLIB_DMA_ChannelXINTSourceFlagSet function.
    Operation is atomic.
*/

PLIB_TEMPLATE void DMA_ChannelXINTSourceFlagSet_Default( DMA_MODULE_ID index , DMA_CHANNEL channel , DMA_INT_TYPE dmaINTSource )
{
    volatile dma_ch_register_t *regs = (dma_ch_register_t *)(index + sizeof(dma_register_t) + channel * sizeof(dma_ch_register_t));

    switch (dmaINTSource) {
    case DMA_INT_ADDRESS_ERROR:
	regs->DCHxINTSET = _DCH0INT_CHERIF_MASK;
	break;

    case DMA_INT_TRANSFER_ABORT:
	regs->DCHxINTSET = _DCH0INT_CHTAIF_MASK;
	break;

    case DMA_INT_CELL_TRANSFER_COMPLETE:
	regs->DCHxINTSET = _DCH0INT_CHCCIF_MASK;
	break;

    case DMA_INT_BLOCK_TRANSFER_COMPLETE:
	regs->DCHxINTSET = _DCH0INT_CHBCIF_MASK;
	break;

    case DMA_INT_DESTINATION_HALF_FULL:
	regs->DCHxINTSET = _DCH0INT_CHDHIF_MASK;
	break;

    case DMA_INT_DESTINATION_DONE:
	regs->DCHxINTSET = _DCH0INT_CHDDIF_MASK;
	break;

    case DMA_INT_SOURCE_HALF_EMPTY:
	regs->DCHxINTSET = _DCH0INT_CHSHIF_MASK;
	break;

    case DMA_INT_SOURCE_DONE:
	regs->DCHxINTSET = _DCH0INT_CHSDIF_MASK;
	break;
    }
}

//******************************************************************************
/* Function :  DMA_ChannelXINTSourceFlagClear_Default

  Summary:
    Implements Default variant of PLIB_DMA_ChannelXINTSourceFlagClear

  Description:
    This template implements the Default variant of the PLIB_DMA_ChannelXINTSourceFlagClear function.
    Operation is atomic.
*/

PLIB_TEMPLATE void DMA_ChannelXINTSourceFlagClear_Default( DMA_MODULE_ID index , DMA_CHANNEL channel , DMA_INT_TYPE dmaINTSource )
{
    volatile dma_ch_register_t *regs = (dma_ch_register_t *)(index + sizeof(dma_register_t) + channel * sizeof(dma_ch_register_t));

    switch (dmaINTSource) {
    case DMA_INT_ADDRESS_ERROR:
	regs->DCHxINTCLR = _DCH0INT_CHERIF_MASK;
	break;

    case DMA_INT_TRANSFER_ABORT:
	regs->DCHxINTCLR = _DCH0INT_CHTAIF_MASK;
	break;

    case DMA_INT_CELL_TRANSFER_COMPLETE:
	regs->DCHxINTCLR = _DCH0INT_CHCCIF_MASK;
	break;

    case DMA_INT_BLOCK_TRANSFER_COMPLETE:
	regs->DCHxINTCLR = _DCH0INT_CHBCIF_MASK;
	break;

    case DMA_INT_DESTINATION_HALF_FULL:
	regs->DCHxINTCLR = _DCH0INT_CHDHIF_MASK;
	break;

    case DMA_INT_DESTINATION_DONE:
	regs->DCHxINTCLR = _DCH0INT_CHDDIF_MASK;
	break;

    case DMA_INT_SOURCE_HALF_EMPTY:
	regs->DCHxINTCLR = _DCH0INT_CHSHIF_MASK;
	break;

    case DMA_INT_SOURCE_DONE:
	regs->DCHxINTCLR = _DCH0INT_CHSDIF_MASK;
	break;
    }
}

#endif /*_DMA_CHANNELXINTSOURCEFLAG_DEFAULT_H*/

/******************************************************************************
 End of File
*/

