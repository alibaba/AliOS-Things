/*******************************************************************************
  PCACHE Peripheral Library Template Implementation

  File Name:
    pcache_PFMAddressWaitStateEnable_Default.h

  Summary:
    PCACHE PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : PFMAddressWaitStateEnable
    and its Variant : Default
    For following APIs :
        PLIB_PCACHE_ExistsPFMAddressWaitStateEnable
        PLIB_PCACHE_PFMAddressWaitStateEnable
        PLIB_PCACHE_PFMAddressWaitStateDisable
		PLIB_PCACHE_PFMAddressWaitStateIsEnabled
				
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

#ifndef _PCACHE_PFMADDRESSWAITSTATEENABLE_DEFAULT_H
#define _PCACHE_PFMADDRESSWAITSTATEENABLE_DEFAULT_H

#include "pcache_Registers.h"

//******************************************************************************
/* Function :  PCACHE_ExistsPFMAddressWaitStateEnable_Default

  Summary:
    Implements Default variant of PLIB_PCACHE_ExistsPFMAddressWaitStateEnable

  Description:
    This template implements the Default variant of the PLIB_PCACHE_ExistsPFMAddressWaitStateEnable function.
*/

#define PLIB_PCACHE_ExistsPFMAddressWaitStateEnable PLIB_PCACHE_ExistsPFMAddressWaitStateEnable
PLIB_TEMPLATE bool PCACHE_ExistsPFMAddressWaitStateEnable_Default( PCACHE_MODULE_ID index )
{
    return true;
}


//******************************************************************************
/* Function :  PCACHE_PFMAddressWaitStateEnable_Default

  Summary:
    Implements Default variant of PLIB_PCACHE_PFMAddressWaitStateEnable

  Description:
    This template implements the Default variant of the PLIB_PCACHE_PFMAddressWaitStateEnable function.
    Operation is atomic.
	Enabling this bit, Adds one more Wait State to flash address setup (suggested for higher system clock frequencies)
*/

PLIB_TEMPLATE void PCACHE_PFMAddressWaitStateEnable_Default( PCACHE_MODULE_ID index )
{
    CHECONSET = _CHECON_PFMAWSEN_MASK;
}

//******************************************************************************
/* Function :  PCACHE_PFMAddressWaitStateDisable_Default

  Summary:
    Implements Default variant of PLIB_PCACHE_PFMAddressWaitStateDisable

  Description:
    This template implements the Default variant of the PLIB_PCACHE_PFMAddressWaitStateDisable function.
    Operation is atomic.
	Disabling this bit, Adds no Wait States to the flash address setup (suggested for lower system clock 
	frequencies to achieve higher performance)
*/

PLIB_TEMPLATE void PCACHE_PFMAddressWaitStateDisable_Default( PCACHE_MODULE_ID index )
{
    CHECONCLR = _CHECON_PFMAWSEN_MASK;
}


//******************************************************************************
/* Function :  PCACHE_PFMAddressWaitStateIsEnabled_Default

  Summary:
    Implements Default variant of PLIB_PCACHE_PFMAddressWaitStateIsEnabled

  Description:
    This template implements the Default variant of the PLIB_PCACHE_PFMAddressWaitStateIsEnabled function.
    Operation is atomic.
*/

PLIB_TEMPLATE bool PCACHE_PFMAddressWaitStateIsEnabled_Default( PCACHE_MODULE_ID index )
{
    return (bool)CHECONbits.PFMAWSEN;
}

#endif /*_PCACHE_PFMADDRESSWAITSTATEENABLE_DEFAULT_H*/

/******************************************************************************
 End of File		
*/

