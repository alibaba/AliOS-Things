/*******************************************************************************
  DMA Peripheral Library Template Implementation

  File Name:
    dma_ChannelXSourceStartAddress_Default.h

  Summary:
    DMA PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : ChannelXSourceStartAddress
    and its Variant : Default
    For following APIs :
        PLIB_DMA_ExistsChannelXSourceStartAddress
        PLIB_DMA_ChannelXSourceStartAddressGet
        PLIB_DMA_ChannelXSourceStartAddressSet

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

#ifndef _DMA_CHANNELXSOURCESTARTADDRESS_DEFAULT_H
#define _DMA_CHANNELXSOURCESTARTADDRESS_DEFAULT_H

#include <sys/kmem.h>
#include "dma_Registers.h"

#define ConvertToPhysicalAddress(a) ((uint32_t)KVA_TO_PA(a))
#define ConvertToVirtualAddress(a)  PA_TO_KVA1(a)

//******************************************************************************
/* Function :  DMA_ExistsChannelXSourceStartAddress_Default

  Summary:
    Implements Default variant of PLIB_DMA_ExistsChannelXSourceStartAddress

  Description:
    This template implements the Default variant of the PLIB_DMA_ExistsChannelXSourceStartAddress function.
*/

#define PLIB_DMA_ExistsChannelXSourceStartAddress PLIB_DMA_ExistsChannelXSourceStartAddress
PLIB_TEMPLATE bool DMA_ExistsChannelXSourceStartAddress_Default( DMA_MODULE_ID index )
{
    return true;
}


//******************************************************************************
/* Function :  DMA_ChannelXSourceStartAddressGet_Default

  Summary:
    Implements Default variant of PLIB_DMA_ChannelXSourceStartAddressGet

  Description:
    This template implements the Default variant of the PLIB_DMA_ChannelXSourceStartAddressGet function.
    Operation is atomic.
*/

PLIB_TEMPLATE uint32_t DMA_ChannelXSourceStartAddressGet_Default( DMA_MODULE_ID index , DMA_CHANNEL channel )
{
    volatile dma_ch_register_t *regs = (dma_ch_register_t *)(index + sizeof(dma_register_t) + channel * sizeof(dma_ch_register_t));
    uint32_t addr = regs->DCHxSSA;

    /* Check if the address lies in the KSEG2 for MZ devices */
    if ((addr & 0x20000000) == 0x20000000) {
	if((addr >> 28)== 0x2) {
	    // EBI Address translation
	    addr = (addr | 0xc0000000) & 0xcFFFFFFF;
	} else if ((addr >> 28)== 0x3) {
	    //SQI Address translation
	    addr = (addr | 0xD0000000) & 0xDFFFFFFF;
        }
    } else {
	addr = (uint32_t)ConvertToVirtualAddress(addr);
    }

    return addr;
}


//******************************************************************************
/* Function :  DMA_ChannelXSourceStartAddressSet_Default

  Summary:
    Implements Default variant of PLIB_DMA_ChannelXSourceStartAddressSet

  Description:
    This template implements the Default variant of the PLIB_DMA_ChannelXSourceStartAddressSet function.
    Operation is atomic.
*/

PLIB_TEMPLATE void DMA_ChannelXSourceStartAddressSet_Default( DMA_MODULE_ID index , DMA_CHANNEL channel , uint32_t sourceStartAddress )
{
    volatile dma_ch_register_t *regs = (dma_ch_register_t *)(index + sizeof(dma_register_t) + channel * sizeof(dma_ch_register_t));

    /* Check if the address lies in the KSEG2 for MZ devices */
    if ((sourceStartAddress >> 29) == 0x6) {
	if ((sourceStartAddress >> 28)== 0xc) {
	    // EBI Address translation
            sourceStartAddress = ((sourceStartAddress | 0x20000000) & 0x2FFFFFFF);
	} else if((sourceStartAddress >> 28)== 0xD) {
	    //SQI Address translation
            sourceStartAddress = ((sourceStartAddress | 0x30000000) & 0x3FFFFFFF);
        }
    } else if ((sourceStartAddress >> 29) == 0x7) {
	if ((sourceStartAddress >> 28)== 0xE) {
	    // EBI Address translation
            sourceStartAddress = ((sourceStartAddress | 0x20000000) & 0x2FFFFFFF);
	} else if ((sourceStartAddress >> 28)== 0xF) {
	    //SQI Address translation
            sourceStartAddress = ((sourceStartAddress | 0x30000000) & 0x3FFFFFFF);
        }
    } else {
        /*For KSEG0 and KSEG1, The translation is done by KVA_TO_PA */
        sourceStartAddress = ConvertToPhysicalAddress(sourceStartAddress);
    }

    regs->DCHxSSA = sourceStartAddress;
}


#endif /*_DMA_CHANNELXSOURCESTARTADDRESS_DEFAULT_H*/

/******************************************************************************
 End of File
*/

