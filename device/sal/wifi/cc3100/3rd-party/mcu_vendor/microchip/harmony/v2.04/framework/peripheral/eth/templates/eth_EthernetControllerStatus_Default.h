/*******************************************************************************
  ETH Peripheral Library Template Implementation

  File Name:
    eth_EthernetControllerStatus_Default.h

  Summary:
    ETH PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : EthernetControllerStatus
    and its Variant : Default
    For following APIs :
        PLIB_ETH_RxPacketCountGet
        PLIB_ETH_EthernetIsBusy
        PLIB_ETH_TransmitIsBusy
        PLIB_ETH_ReceiveIsBusy
        PLIB_ETH_ExistsEthernetControllerStatus

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

#ifndef _ETH_ETHERNETCONTROLLERSTATUS_DEFAULT_H
#define _ETH_ETHERNETCONTROLLERSTATUS_DEFAULT_H
#include "eth_registers.h"

//******************************************************************************
/* Function :  ETH_RxPacketCountGet_Default

  Summary:
    Implements Default variant of PLIB_ETH_RxPacketCountGet
	This function performs atomic register access.

  Description:
    This template implements the Default variant of the PLIB_ETH_RxPacketCountGet function.
*/

PLIB_TEMPLATE uint8_t ETH_RxPacketCountGet_Default( ETH_MODULE_ID index )
{
	volatile eth_registers_t * eth = ((eth_registers_t *)(index));
    return (uint8_t)(eth->ETHSTAT.BUFCNT); 
}


//******************************************************************************
/* Function :  ETH_EthernetIsBusy_Default

  Summary:
    Implements Default variant of PLIB_ETH_EthernetIsBusy
	This function performs atomic register access.

  Description:
    This template implements the Default variant of the PLIB_ETH_EthernetIsBusy function.
*/

PLIB_TEMPLATE bool ETH_EthernetIsBusy_Default( ETH_MODULE_ID index )
{
 	volatile eth_registers_t * eth = ((eth_registers_t *)(index));
    return (bool)(eth->ETHSTAT.BUSY); 						   
}


//******************************************************************************
/* Function :  ETH_TransmitIsBusy_Default

  Summary:
    Implements Default variant of PLIB_ETH_TransmitIsBusy
	This function performs atomic register access.

  Description:
    This template implements the Default variant of the PLIB_ETH_TransmitIsBusy function.
*/

PLIB_TEMPLATE bool ETH_TransmitIsBusy_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));
    return (bool)(eth->ETHSTAT.TXBUSY); 
}


//******************************************************************************
/* Function :  ETH_ReceiveIsBusy_Default

  Summary:
    Implements Default variant of PLIB_ETH_ReceiveIsBusy
	This function performs atomic register access.

  Description:
    This template implements the Default variant of the PLIB_ETH_ReceiveIsBusy function.
*/

PLIB_TEMPLATE bool ETH_ReceiveIsBusy_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));
    return (bool)(eth->ETHSTAT.RXBUSY); 
}


//******************************************************************************
/* Function :  ETH_ExistsEthernetControllerStatus_Default

  Summary:
    Implements Default variant of PLIB_ETH_ExistsEthernetControllerStatus

  Description:
    This template implements the Default variant of the PLIB_ETH_ExistsEthernetControllerStatus function.
*/

#define PLIB_ETH_ExistsEthernetControllerStatus PLIB_ETH_ExistsEthernetControllerStatus
PLIB_TEMPLATE bool ETH_ExistsEthernetControllerStatus_Default( ETH_MODULE_ID index )
{
    return true;
}


#endif /*_ETH_ETHERNETCONTROLLERSTATUS_DEFAULT_H*/

/******************************************************************************
 End of File
*/

