/*******************************************************************************
  ETH Peripheral Library Template Implementation

  File Name:
    eth_ReceiveFilters_Default.h

  Summary:
    ETH PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : ReceiveFilters
    and its Variant : Default
    For following APIs :
        PLIB_ETH_ReceiveFilterEnable
        PLIB_ETH_ReceiveFilterDisable
        PLIB_ETH_ReceiveFilterIsEnable
        PLIB_ETH_ExistsReceiveFilters

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

#ifndef _ETH_RECEIVEFILTERS_DEFAULT_H
#define _ETH_RECEIVEFILTERS_DEFAULT_H
#include "eth_registers.h"
//******************************************************************************
/* Function :  ETH_ReceiveFilterEnable_Default

  Summary:
    Implements Default variant of PLIB_ETH_ReceiveFilterEnable
	This function performs atomic register access.

  Description:
    This template implements the Default variant of the PLIB_ETH_ReceiveFilterEnable function.
*/

PLIB_TEMPLATE void ETH_ReceiveFilterEnable_Default( ETH_MODULE_ID index , ETH_RECEIVE_FILTER filter )
{
   	volatile eth_registers_t * eth = ((eth_registers_t *)(index));			
	
	eth->ETHRXFCSET = ((uint32_t)filter  & ETH_ENABLE_ALL_FILTER);	
}


//******************************************************************************
/* Function :  ETH_RxFiltersSet_Default

  Summary:
    Implements Default variant of ETH_RxFiltersSet

  Description:
    This template implements the Default variant of the ETH_RxFiltersSet function.
*/
PLIB_TEMPLATE void ETH_RxFiltersSet_Default(ETH_MODULE_ID index,ETH_RX_FILTERS rxFilters)
{
    ETH_ReceiveFilterEnable_Default(index,rxFilters);
}


//******************************************************************************
/* Function :  ETH_ReceiveFilterDisable_Default

  Summary:
    Implements Default variant of PLIB_ETH_ReceiveFilterDisable
	This function performs atomic register access.

  Description:
    This template implements the Default variant of the PLIB_ETH_ReceiveFilterDisable function.
*/

PLIB_TEMPLATE void ETH_ReceiveFilterDisable_Default( ETH_MODULE_ID index , ETH_RECEIVE_FILTER filter )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));	
	eth->ETHRXFCCLR = (uint32_t)filter & ETH_ENABLE_ALL_FILTER;	
}

//******************************************************************************
/* Function :  ETH_RxFiltersClr_Default

  Summary:
    Implements Default variant of ETH_RxFiltersClr

  Description:
    This template implements the Default variant of the ETH_RxFiltersClr function.
*/
PLIB_TEMPLATE void ETH_RxFiltersClr_Default(ETH_MODULE_ID index,ETH_RX_FILTERS rxFilters)
{
    ETH_ReceiveFilterDisable_Default(index,rxFilters);
}

//******************************************************************************
/* Function :  ETH_RxFiltersWrite_Default

  Summary:
    Implements Default variant of ETH_RxFiltersWrite

  Description:
    This template implements the Default variant of the ETH_RxFiltersWrite function.
*/

PLIB_TEMPLATE void ETH_RxFiltersWrite_Default(ETH_MODULE_ID index,ETH_RX_FILTERS rxFilters)
{
    ETH_ReceiveFilterEnable_Default(index,rxFilters);
    ETH_ReceiveFilterDisable_Default(index,~rxFilters);
}

//******************************************************************************
/* Function :  ETH_ReceiveFilterIsEnable_Default

  Summary:
    Implements Default variant of PLIB_ETH_ReceiveFilterIsEnable
	This function performs atomic register access.

  Description:
    This template implements the Default variant of the PLIB_ETH_ReceiveFilterIsEnable function.
*/

PLIB_TEMPLATE bool ETH_ReceiveFilterIsEnable_Default( ETH_MODULE_ID index , ETH_RECEIVE_FILTER filter )
{
	volatile eth_registers_t * eth = ((eth_registers_t *)(index));	
		
    return ((eth->ETHRXFC.w &(uint32_t)filter) > 0 ? true : false );
}


//******************************************************************************
/* Function :  ETH_ExistsReceiveFilters_Default

  Summary:
    Implements Default variant of PLIB_ETH_ExistsReceiveFilters

  Description:
    This template implements the Default variant of the PLIB_ETH_ExistsReceiveFilters function.
*/

#define PLIB_ETH_ExistsReceiveFilters PLIB_ETH_ExistsReceiveFilters
PLIB_TEMPLATE bool ETH_ExistsReceiveFilters_Default( ETH_MODULE_ID index )
{
    return true;
}


#endif /*_ETH_RECEIVEFILTERS_DEFAULT_H*/

/******************************************************************************
 End of File
*/

