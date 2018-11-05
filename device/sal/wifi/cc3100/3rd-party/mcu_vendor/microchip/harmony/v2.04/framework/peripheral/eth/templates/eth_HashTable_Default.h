/*******************************************************************************
  ETH Peripheral Library Template Implementation

  File Name:
    eth_HashTable_Default.h

  Summary:
    ETH PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : HashTable
    and its Variant : Default
    For following APIs :
        PLIB_ETH_HashTableSet
        PLIB_ETH_HashTableGet
        PLIB_ETH_ExistsHashTable

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

#ifndef _ETH_HASHTABLE_DEFAULT_H
#define _ETH_HASHTABLE_DEFAULT_H
#include "eth_registers.h"

//******************************************************************************
/* Function :  ETH_HashTableSet_Default

  Summary:
    Implements Default variant of PLIB_ETH_HashTableSet
	This function performs atomic register access.

  Description:
    This template implements the Default variant of the PLIB_ETH_HashTableSet function.
*/

PLIB_TEMPLATE void ETH_HashTableSet_Default( ETH_MODULE_ID index , uint64_t hashTableValue )
{
    
	volatile eth_registers_t * eth = ((eth_registers_t *)(index));
	PLIB_ASSERT(((!eth->ETHCON1.RXEN)|| (!eth->ETHRXFC,HTEN)),
                "Receive must be disabled or Hash Table turned off!");
		
	eth->ETHHT0SET = _ETHHT0_HTLOWER_MASK & ((uint32_t)hashTableValue);
	eth->ETHHT1SET = _ETHHT1_HTUPPER_MASK &((uint32_t)(hashTableValue>>32));
	
}

//******************************************************************************
/* Function :  ETH_RxFiltersHTSet_Default

  Summary:
    Implements Default variant of ETH_RxFiltersHTSet

  Description:
    This template implements the Default variant of the ETH_RxFiltersHTSet function.
*/
PLIB_TEMPLATE void ETH_RxFiltersHTSet_Default(ETH_MODULE_ID index,uint64_t htable)
{
    ETH_HashTableSet_Default(index,htable);
}



//******************************************************************************
/* Function :  ETH_HashTableGet_Default

  Summary:
    Implements Default variant of PLIB_ETH_HashTableGet
	This function performs atomic register access.

  Description:
    This template implements the Default variant of the PLIB_ETH_HashTableGet function.
*/

PLIB_TEMPLATE uint32_t ETH_HashTableGet_Default( ETH_MODULE_ID index )
{
   	volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	 return ( ((uint64_t)eth->ETHHT1.HTUPPER<<32)
            + ((uint64_t)eth->ETHHT0.HTLOWER));		
}


//******************************************************************************
/* Function :  ETH_ExistsHashTable_Default

  Summary:
    Implements Default variant of PLIB_ETH_ExistsHashTable

  Description:
    This template implements the Default variant of the PLIB_ETH_ExistsHashTable function.
*/

#define PLIB_ETH_ExistsHashTable PLIB_ETH_ExistsHashTable
PLIB_TEMPLATE bool ETH_ExistsHashTable_Default( ETH_MODULE_ID index )
{
    return true;
}


#endif /*_ETH_HASHTABLE_DEFAULT_H*/

/******************************************************************************
 End of File
*/

