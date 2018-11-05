/*******************************************************************************
  DMA Peripheral Library Template Implementation

  File Name:
    dma_ChannelXINTSource_Default.h

  Summary:
    DMA PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : ChannelXINTSource
    and its Variant : Default
    For following APIs :
        PLIB_DMA_ExistsChannelXINTSource
        PLIB_DMA_ChannelXINTSourceEnable
        PLIB_DMA_ChannelXINTSourceDisable
        PLIB_DMA_ChannelXINTSourceIsEnabled

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

#ifndef _DMA_CHANNELXINTSOURCE_DEFAULT_H
#define _DMA_CHANNELXINTSOURCE_DEFAULT_H

#include "dma_Registers.h"

//******************************************************************************
/* Function :  DMA_ExistsChannelXINTSource_Default

  Summary:
    Implements Default variant of PLIB_DMA_ExistsChannelXINTSource

  Description:
    This template implements the Default variant of the PLIB_DMA_ExistsChannelXINTSource function.
*/

#define PLIB_DMA_ExistsChannelXINTSource PLIB_DMA_ExistsChannelXINTSource
PLIB_TEMPLATE bool DMA_ExistsChannelXINTSource_Default( DMA_MODULE_ID index )
{
    return true;
}


//******************************************************************************
/* Function :  DMA_ChannelXINTSourceEnable_Default

  Summary:
    Implements Default variant of PLIB_DMA_ChannelXINTSourceEnable

  Description:
    This template implements the Default variant of the PLIB_DMA_ChannelXINTSourceEnable function.
    Operation is atomic.
*/

PLIB_TEMPLATE void DMA_ChannelXINTSourceEnable_Default( DMA_MODULE_ID index , DMA_CHANNEL channel , DMA_INT_TYPE dmaINTSource )
{
    volatile dma_ch_register_t *regs = (dma_ch_register_t *)(index + sizeof(dma_register_t) + channel * sizeof(dma_ch_register_t));

    switch (dmaINTSource) {
    case DMA_INT_ADDRESS_ERROR:
	regs->DCHxINTSET = _DCH0INT_CHERIE_MASK;
	break;

    case DMA_INT_TRANSFER_ABORT:
	regs->DCHxINTSET = _DCH0INT_CHTAIE_MASK;
	break;
    case DMA_INT_CELL_TRANSFER_COMPLETE:
	regs->DCHxINTSET = _DCH0INT_CHCCIE_MASK;
	break;
    case DMA_INT_BLOCK_TRANSFER_COMPLETE:
	regs->DCHxINTSET = _DCH0INT_CHBCIE_MASK;
	break;

    case DMA_INT_DESTINATION_HALF_FULL:
	regs->DCHxINTSET = _DCH0INT_CHDHIE_MASK;
	break;

    case DMA_INT_DESTINATION_DONE:
	regs->DCHxINTSET = _DCH0INT_CHDDIE_MASK;
	break;

    case DMA_INT_SOURCE_HALF_EMPTY:
	regs->DCHxINTSET = _DCH0INT_CHSHIE_MASK;
	break;

    case DMA_INT_SOURCE_DONE:
	regs->DCHxINTSET = _DCH0INT_CHSDIE_MASK;
	break;
    }
}


//******************************************************************************
/* Function :  DMA_ChannelXINTSourceDisable_Default

  Summary:
    Implements Default variant of PLIB_DMA_ChannelXINTSourceDisable

  Description:
    This template implements the Default variant of the PLIB_DMA_ChannelXINTSourceDisable function.
    Operation is atomic.
*/

PLIB_TEMPLATE void DMA_ChannelXINTSourceDisable_Default( DMA_MODULE_ID index , DMA_CHANNEL channel , DMA_INT_TYPE dmaINTSource )
{
    volatile dma_ch_register_t *regs = (dma_ch_register_t *)(index + sizeof(dma_register_t) + channel * sizeof(dma_ch_register_t));

    switch (dmaINTSource) {
    case DMA_INT_ADDRESS_ERROR:
	regs->DCHxINTCLR = _DCH0INT_CHERIE_MASK;
	break;

    case DMA_INT_TRANSFER_ABORT:
	regs->DCHxINTCLR = _DCH0INT_CHTAIE_MASK;
	break;
    case DMA_INT_CELL_TRANSFER_COMPLETE:
	regs->DCHxINTCLR = _DCH0INT_CHCCIE_MASK;
	break;
    case DMA_INT_BLOCK_TRANSFER_COMPLETE:
	regs->DCHxINTCLR = _DCH0INT_CHBCIE_MASK;
	break;

    case DMA_INT_DESTINATION_HALF_FULL:
	regs->DCHxINTCLR = _DCH0INT_CHDHIE_MASK;
	break;

    case DMA_INT_DESTINATION_DONE:
	regs->DCHxINTCLR = _DCH0INT_CHDDIE_MASK;
	break;

    case DMA_INT_SOURCE_HALF_EMPTY:
	regs->DCHxINTCLR = _DCH0INT_CHSHIE_MASK;
	break;

    case DMA_INT_SOURCE_DONE:
	regs->DCHxINTCLR = _DCH0INT_CHSDIE_MASK;
	break;
    }
}


//******************************************************************************
/* Function :  DMA_ChannelXINTSourceIsEnabled_Default

  Summary:
    Implements Default variant of PLIB_DMA_ChannelXINTSourceIsEnabled

  Description:
    This template implements the Default variant of the PLIB_DMA_ChannelXINTSourceIsEnabled function.
    Operation is atomic.
*/

PLIB_TEMPLATE bool DMA_ChannelXINTSourceIsEnabled_Default( DMA_MODULE_ID index , DMA_CHANNEL channel , DMA_INT_TYPE dmaINTSource )
{
    volatile dma_ch_register_t *regs = (dma_ch_register_t *)(index + sizeof(dma_register_t) + channel * sizeof(dma_ch_register_t));

    switch (dmaINTSource) {
    case DMA_INT_ADDRESS_ERROR:
	return (bool)regs->DCHxINT.CHERIE;

    case DMA_INT_TRANSFER_ABORT:
	return (bool)regs->DCHxINT.CHTAIE;

    case DMA_INT_CELL_TRANSFER_COMPLETE:
	return (bool)regs->DCHxINT.CHCCIE;

    case DMA_INT_BLOCK_TRANSFER_COMPLETE:
	return (bool)regs->DCHxINT.CHBCIE;

    case DMA_INT_DESTINATION_HALF_FULL:
	return (bool)regs->DCHxINT.CHDHIE;

    case DMA_INT_DESTINATION_DONE:
	return (bool)regs->DCHxINT.CHDDIE;

    case DMA_INT_SOURCE_HALF_EMPTY:
	return (bool)regs->DCHxINT.CHSHIE;

    case DMA_INT_SOURCE_DONE:
	return (bool)regs->DCHxINT.CHSDIE;
    }

    return (bool)false;
}


#endif /*_DMA_CHANNELXINTSOURCE_DEFAULT_H*/

/******************************************************************************
 End of File
*/

