/*******************************************************************************
  PCACHE Peripheral Library Template Implementation

  File Name:
    pcache_InvalidateCache_Default.h

  Summary:
    PCACHE PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : InvalidateCache
    and its Variant : Default
    For following APIs :
		PLIB_PCACHE_ExistsInvalidateCache
        PLIB_PCACHE_ForceInvalidateCacheEnable
		PLIB_PCACHE_ForceInvalidateCacheIsEnabled
		PLIB_PCACHE_AutoInvalidateCacheEnable
		PLIB_PCACHE_AutoInvalidateCacheDisable
		PLIB_PCACHE_AutoInvalidateCacheIsEnabled

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

#ifndef _PCACHE_INVALIDATECACHE_DEFAULT_H
#define _PCACHE_INVALIDATECACHE_DEFAULT_H

#include "pcache_Registers.h"

//******************************************************************************
/* Function :  PCACHE_ExistsInvalidateCache_Default

  Summary:
    Implements Default variant of PLIB_PCACHE_ExistsInvalidateCache

  Description:
    This template implements the Default variant of the PLIB_PCACHE_ExistsInvalidateCache function.
*/

#define PLIB_PCACHE_ExistsInvalidateCache PLIB_PCACHE_ExistsInvalidateCache
PLIB_TEMPLATE bool PCACHE_ExistsInvalidateCache_Default( PCACHE_MODULE_ID index )
{
    return true;
}

//******************************************************************************
/* Function :  PCACHE_ForceInvalidateCacheEnable_Default

  Summary:
    Implements Default variant of PLIB_PCACHE_ForceInvalidateCacheEnable 

  Description:
    This template implements the Default variant of the PLIB_PCACHE_ForceInvalidateCacheEnable function.
    Operation is atomic.
*/

PLIB_TEMPLATE void PCACHE_ForceInvalidateCacheEnable_Default( PCACHE_MODULE_ID index, PCACHE_CACHE_TYPE cache_type )
{
    CHECONSET = cache_type << 20;
}

//******************************************************************************
/* Function :  PCACHE_ForceInvalidateCacheIsEnabled_Default

  Summary:
    Implements Default variant of PLIB_PCACHE_ForceInvalidateCacheIsEnabled 

  Description:
    This template implements the Default variant of the PLIB_PCACHE_ForceInvalidateCacheIsEnabled function.
    Operation is atomic.
*/

PLIB_TEMPLATE bool PCACHE_ForceInvalidateCacheIsEnabled_Default( PCACHE_MODULE_ID index, PCACHE_CACHE_TYPE cache_type )
{
    if (cache_type == PLIB_PCACHE_PERIPHERAL_CACHE)
	{
		return (bool)CHECONbits.PERCHEINV;
	}
	else if (cache_type == PLIB_PCACHE_DATA_CACHE)
	{
		return (bool)CHECONbits.DCHEINV;
	}
	else if (cache_type == PLIB_PCACHE_INSTRUCTION_CACHE)
	{
		return (bool)CHECONbits.ICHEINV;
	}
}

//******************************************************************************
/* Function :  PCACHE_AutoInvalidateCacheEnable_Default

  Summary:
    Implements Default variant of PLIB_PCACHE_AutoInvalidateCacheEnable 

  Description:
    This template implements the Default variant of the PLIB_PCACHE_AutoInvalidateCacheEnable function.
    Operation is atomic.
*/

PLIB_TEMPLATE void PCACHE_AutoInvalidateCacheEnable_Default( PCACHE_MODULE_ID index, PCACHE_CACHE_TYPE cache_type )
{
    CHECONSET = cache_type << 16;
}

//******************************************************************************
/* Function :  PCACHE_AutoInvalidateCacheDisable_Default

  Summary:
    Implements Default variant of PLIB_PCACHE_AutoInvalidateCacheDisable 

  Description:
    This template implements the Default variant of the PLIB_PCACHE_AutoInvalidateCacheDisable function.
    Operation is atomic.
*/

PLIB_TEMPLATE void PCACHE_AutoInvalidateCacheDisable_Default( PCACHE_MODULE_ID index, PCACHE_CACHE_TYPE cache_type )
{
    CHECONCLR = cache_type << 16;
}

//******************************************************************************
/* Function :  PCACHE_AutoInvalidateCacheIsEnabled_Default

  Summary:
    Implements Default variant of PLIB_PCACHE_AutoInvalidateCacheIsEnabled 

  Description:
    This template implements the Default variant of the PLIB_PCACHE_AutoInvalidateCacheIsEnabled function.
    Operation is atomic.
*/

PLIB_TEMPLATE bool PCACHE_AutoInvalidateCacheIsEnabled_Default( PCACHE_MODULE_ID index, PCACHE_CACHE_TYPE cache_type )
{
    if (cache_type == PLIB_PCACHE_PERIPHERAL_CACHE)
	{
		return (bool)CHECONbits.PERCHECOH;
	}
	else if (cache_type == PLIB_PCACHE_DATA_CACHE)
	{
		return (bool)CHECONbits.DCHECOH;
	}
	else if (cache_type == PLIB_PCACHE_INSTRUCTION_CACHE)
	{
		return (bool)CHECONbits.ICHECOH;
	}
}

#endif /*_PCACHE_INVALIDATECACHE_DEFAULT_H*/

/******************************************************************************
 End of File
*/

