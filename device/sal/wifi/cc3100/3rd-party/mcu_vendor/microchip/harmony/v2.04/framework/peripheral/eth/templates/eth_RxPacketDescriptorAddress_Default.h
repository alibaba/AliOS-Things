/*******************************************************************************
  ETH Peripheral Library Template Implementation

  File Name:
    eth_RxPacketDescriptorAddress_Default.h

  Summary:
    ETH PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : RxPacketDescriptorAddress
    and its Variant : Default
    For following APIs :
        PLIB_ETH_RxPacketDescAddrSet
        PLIB_ETH_RxPacketDescAddrGet
        PLIB_ETH_ExistsRxPacketDescriptorAddress

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

#ifndef _ETH_RXPACKETDESCRIPTORADDRESS_DEFAULT_H
#define _ETH_RXPACKETDESCRIPTORADDRESS_DEFAULT_H
#include "eth_registers.h"
//******************************************************************************
/* Function :  ETH_RxPacketDescAddrSet_Default

  Summary:
    Implements Default variant of PLIB_ETH_RxPacketDescAddrSet
	This function performs atomic register access.

  Description:
    This template implements the Default variant of the PLIB_ETH_RxPacketDescAddrSet function.
*/

PLIB_TEMPLATE void ETH_RxPacketDescAddrSet_Default( ETH_MODULE_ID index , uint8_t *rxPacketDescStartAddr )
{  
   
   volatile eth_registers_t * eth = ((eth_registers_t *)(index));	
    eth->ETHRXST.RXSTADDR = ((uint32_t)rxPacketDescStartAddr>>2);


}

//******************************************************************************
/* Function :  ETH_RxPacketDescAddrGet_Default

  Summary:
    Implements Default variant of PLIB_ETH_RxPacketDescAddrGet
	This function performs atomic register access.

  Description:
    This template implements the Default variant of the PLIB_ETH_RxPacketDescAddrGet function.
*/

PLIB_TEMPLATE uint8_t *ETH_RxPacketDescAddrGet_Default( ETH_MODULE_ID index )
{
  	volatile eth_registers_t * eth = ((eth_registers_t *)(index));	
    return (uint8_t *)eth->ETHRXST.w;
}


//******************************************************************************
/* Function :  ETH_ExistsRxPacketDescriptorAddress_Default

  Summary:
    Implements Default variant of PLIB_ETH_ExistsRxPacketDescriptorAddress
	

  Description:
    This template implements the Default variant of the PLIB_ETH_ExistsRxPacketDescriptorAddress function.
*/

#define PLIB_ETH_ExistsRxPacketDescriptorAddress PLIB_ETH_ExistsRxPacketDescriptorAddress
PLIB_TEMPLATE bool ETH_ExistsRxPacketDescriptorAddress_Default( ETH_MODULE_ID index )
{
    return true;
}


#endif /*_ETH_RXPACKETDESCRIPTORADDRESS_DEFAULT_H*/

/******************************************************************************
 End of File
*/

