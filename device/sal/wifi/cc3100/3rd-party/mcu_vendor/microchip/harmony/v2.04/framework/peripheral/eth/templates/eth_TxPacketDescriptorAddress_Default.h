/*******************************************************************************
  ETH Peripheral Library Template Implementation

  File Name:
    eth_TxPacketDescriptorAddress_Default.h

  Summary:
    ETH PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : TxPacketDescriptorAddress
    and its Variant : Default
    For following APIs :
        PLIB_ETH_TxPacketDescAddrSet
        PLIB_ETH_TxPacketDescAddrGet
        PLIB_ETH_ExistsTxPacketDescriptorAddress

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

#ifndef _ETH_TXPACKETDESCRIPTORADDRESS_DEFAULT_H
#define _ETH_TXPACKETDESCRIPTORADDRESS_DEFAULT_H
#include "eth_registers.h"
//******************************************************************************
/* Function :  ETH_TxPacketDescAddrSet_Default

  Summary:
    Implements Default variant of PLIB_ETH_TxPacketDescAddrSet
	This function performs atomic register access.

  Description:
    This template implements the Default variant of the PLIB_ETH_TxPacketDescAddrSet function.
*/

PLIB_TEMPLATE void ETH_TxPacketDescAddrSet_Default( ETH_MODULE_ID index , uint8_t * txPacketDescStartAddr )
{
volatile eth_registers_t * eth = ((eth_registers_t *)(index));	
    eth->ETHTXST.TXSTADDR = ((uint32_t)txPacketDescStartAddr>>2);

	
}


//******************************************************************************
/* Function :  ETH_TxPacketDescAddrGet_Default

  Summary:
    Implements Default variant of PLIB_ETH_TxPacketDescAddrGet
	This function performs atomic register access.

  Description:
    This template implements the Default variant of the PLIB_ETH_TxPacketDescAddrGet function.
*/

PLIB_TEMPLATE uint8_t *  ETH_TxPacketDescAddrGet_Default( ETH_MODULE_ID index )
{
	
	volatile eth_registers_t * eth = ((eth_registers_t *)(index));	
    return (uint8_t *)eth->ETHTXST.w;

}


//******************************************************************************
/* Function :  ETH_ExistsTxPacketDescriptorAddress_Default

  Summary:
    Implements Default variant of PLIB_ETH_ExistsTxPacketDescriptorAddress

  Description:
    This template implements the Default variant of the PLIB_ETH_ExistsTxPacketDescriptorAddress function.
*/

#define PLIB_ETH_ExistsTxPacketDescriptorAddress PLIB_ETH_ExistsTxPacketDescriptorAddress
PLIB_TEMPLATE bool ETH_ExistsTxPacketDescriptorAddress_Default( ETH_MODULE_ID index )
{
    return true;
}


#endif /*_ETH_TXPACKETDESCRIPTORADDRESS_DEFAULT_H*/

/******************************************************************************
 End of File
*/

