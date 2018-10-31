/*******************************************************************************
  PCACHE Peripheral Library Template Implementation

  File Name:
    pcache_CacheEnable_Default.h

  Summary:
    PCACHE PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : CacheEnable
    and its Variant : Default
    For following APIs :
        PLIB_PCACHE_ExistsCacheEnable
        PLIB_PCACHE_CacheEnable
        PLIB_PCACHE_CacheDisable
		PLIB_PCACHE_CacheIsEnabled

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

#ifndef _PCACHE_CACHEENABLE_DEFAULT_H
#define _PCACHE_CACHEENABLE_DEFAULT_H

#include "pcache_Registers.h"

//******************************************************************************
/* Function :  PCACHE_ExistsCacheEnable_Default

  Summary:
    Implements Default variant of PLIB_PCACHE_ExistsCacheEnable

  Description:
    This template implements the Default variant of the PLIB_PCACHE_ExistsCacheEnable function.
*/

#define PLIB_PCACHE_ExistsCacheEnable PLIB_PCACHE_ExistsCacheEnable
PLIB_TEMPLATE bool PCACHE_ExistsCacheEnable_Default( PCACHE_MODULE_ID index )
{
    return true;
}


//******************************************************************************
/* Function :  PCACHE_CacheEnable_Default

  Summary:
    Implements Default variant of PLIB_PCACHE_CacheEnable

  Description:
    This template implements the Default variant of the PLIB_PCACHE_CacheEnable function.
    Operation is atomic.
*/

PLIB_TEMPLATE void PCACHE_CacheEnable_Default( PCACHE_MODULE_ID index, PCACHE_CACHE_TYPE cache_type )
{
    CHECONSET = cache_type << 24;
}

//******************************************************************************
/* Function :  PCACHE_CacheDisable_Default

  Summary:
    Implements Default variant of PLIB_PCACHE_CacheDisable

  Description:
    This template implements the Default variant of the PLIB_PCACHE_CacheDisable function.
    Operation is atomic.
*/

PLIB_TEMPLATE void PCACHE_CacheDisable_Default( PCACHE_MODULE_ID index, PCACHE_CACHE_TYPE cache_type )
{
    CHECONCLR = cache_type << 24;
}

//******************************************************************************
/* Function :  PCACHE_CacheIsEnabled_Default

  Summary:
    Implements Default variant of PLIB_PCACHE_CacheIsEnabled

  Description:
    This template implements the Default variant of the PLIB_PCACHE_CacheIsEnabled function.
    Operation is atomic.
*/

PLIB_TEMPLATE bool PCACHE_CacheIsEnabled_Default( PCACHE_MODULE_ID index, PCACHE_CACHE_TYPE cache_type )
{
    if (cache_type == PLIB_PCACHE_PERIPHERAL_CACHE)
	{
		return (bool)CHECONbits.PERCHEEN;
	}
	else if (cache_type == PLIB_PCACHE_DATA_CACHE)
	{
		return (bool)CHECONbits.DCHEEN;
	}
	else if (cache_type == PLIB_PCACHE_INSTRUCTION_CACHE)
	{
		return (bool)CHECONbits.ICHEEN;
	}
}

#endif /*_PCACHE_CACHEENABLE_DEFAULT_H*/

/******************************************************************************
 End of File		
*/

