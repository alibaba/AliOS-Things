/*******************************************************************************
  ETH Peripheral Library Template Implementation

  File Name:
    eth_ReceiveWmarks_Default.h

  Summary:
    ETH PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : ReceiveWmarks
    and its Variant : Default
    For following APIs :
        PLIB_ETH_RxFullWmarkSet
        PLIB_ETH_RxFullWmarkGet
        PLIB_ETH_RxEmptyWmarkSet
        PLIB_ETH_RxEmptyWmarkGet
        PLIB_ETH_ExistsReceiveWmarks

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

#ifndef _ETH_RECEIVEWMARKS_DEFAULT_H
#define _ETH_RECEIVEWMARKS_DEFAULT_H
#include "eth_registers.h"

//******************************************************************************
/* Function :  ETH_RxFullWmarkSet_Default

  Summary:
    Implements Default variant of PLIB_ETH_RxFullWmarkSet
	This function performs atomic register access.

  Description:
    This template implements the Default variant of the PLIB_ETH_RxFullWmarkSet function.
*/

PLIB_TEMPLATE void ETH_RxFullWmarkSet_Default( ETH_MODULE_ID index , uint8_t watermarkValue )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));					 

	eth->ETHRXWM.RXFWM = watermarkValue;			 
}


//******************************************************************************
/* Function :  ETH_RxFullWmarkGet_Default

  Summary:
    Implements Default variant of PLIB_ETH_RxFullWmarkGet
	This function performs atomic register access.

  Description:
    This template implements the Default variant of the PLIB_ETH_RxFullWmarkGet function.
*/

PLIB_TEMPLATE uint8_t ETH_RxFullWmarkGet_Default( ETH_MODULE_ID index )
{
	volatile eth_registers_t * eth = ((eth_registers_t *)(index));					 
	return (uint8_t) eth->ETHRXWM.RXFWM;
}


//******************************************************************************
/* Function :  ETH_RxEmptyWmarkSet_Default

  Summary:
    Implements Default variant of PLIB_ETH_RxEmptyWmarkSet
	This function performs atomic register access.

  Description:
    This template implements the Default variant of the PLIB_ETH_RxEmptyWmarkSet function.
*/

PLIB_TEMPLATE void ETH_RxEmptyWmarkSet_Default( ETH_MODULE_ID index , uint8_t watermarkValue )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));					 
    eth->ETHRXWM.RXEWM = watermarkValue;		
}


//******************************************************************************
/* Function :  ETH_RxEmptyWmarkGet_Default

  Summary:
    Implements Default variant of PLIB_ETH_RxEmptyWmarkGet
	This function performs atomic register access.

  Description:
    This template implements the Default variant of the PLIB_ETH_RxEmptyWmarkGet function.
*/

PLIB_TEMPLATE uint8_t ETH_RxEmptyWmarkGet_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));					 
	return (uint8_t) eth->ETHRXWM.RXEWM;
}


//******************************************************************************
/* Function :  ETH_ExistsReceiveWmarks_Default

  Summary:
    Implements Default variant of PLIB_ETH_ExistsReceiveWmarks

  Description:
    This template implements the Default variant of the PLIB_ETH_ExistsReceiveWmarks function.
*/

#define PLIB_ETH_ExistsReceiveWmarks PLIB_ETH_ExistsReceiveWmarks
PLIB_TEMPLATE bool ETH_ExistsReceiveWmarks_Default( ETH_MODULE_ID index )
{
    return true;
}


#endif /*_ETH_RECEIVEWMARKS_DEFAULT_H*/

/******************************************************************************
 End of File
*/

