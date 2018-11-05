/*******************************************************************************
  PCACHE Peripheral Library Template Implementation

  File Name:
    pcache_PFMAddressWaitStateEnable_Unsupported.h

  Summary:
    PCACHE PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : PFMAddressWaitStateEnable
    and its Variant : Unsupported
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

#ifndef _PCACHE_PFMADDRESSWAITSTATEENABLE_UNSUPPORTED_H
#define _PCACHE_PFMADDRESSWAITSTATEENABLE_UNSUPPORTED_H

//******************************************************************************
/* Function :  PCACHE_ExistsPFMAddressWaitStateEnable_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_PCACHE_ExistsPFMAddressWaitStateEnable

  Description:
    This template implements the Unsupported variant of the PLIB_PCACHE_ExistsPFMAddressWaitStateEnable function.
*/

PLIB_TEMPLATE bool PCACHE_ExistsPFMAddressWaitStateEnable_Unsupported( PCACHE_MODULE_ID index )
{
    return false;
}

//******************************************************************************
/* Function :  PCACHE_PFMAddressWaitStateEnable_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_PCACHE_PFMAddressWaitStateEnable

  Description:
    This template implements the Unsupported variant of the PLIB_PCACHE_PFMAddressWaitStateEnable function.
*/

PLIB_TEMPLATE void PCACHE_PFMAddressWaitStateEnable_Unsupported( PCACHE_MODULE_ID index)
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_PCACHE_PFMAddressWaitStateEnable");
}

//******************************************************************************
/* Function :  PCACHE_PFMAddressWaitStateDisable_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_PCACHE_PFMAddressWaitStateDisable 

  Description:
    This template implements the Unsupported variant of the PLIB_PCACHE_PFMAddressWaitStateDisable function.
*/

PLIB_TEMPLATE void PCACHE_PFMAddressWaitStateDisable_Unsupported( PCACHE_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_PCACHE_PFMAddressWaitStateDisable");
}

//******************************************************************************
/* Function :  PCACHE_PFMAddressWaitStateIsEnabled_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_PCACHE_PFMAddressWaitStateIsEnabled

  Description:
    This template implements the Unsupported variant of the PLIB_PCACHE_PFMAddressWaitStateIsEnabled function.
*/

PLIB_TEMPLATE bool PCACHE_PFMAddressWaitStateIsEnabled_Unsupported( PCACHE_MODULE_ID index)
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_PCACHE_PFMAddressWaitStateIsEnabled");
	return false;
}

#endif /*_PCACHE_PFMADDRESSWAITSTATEENABLE_UNSUPPORTED_H*/

/******************************************************************************
 End of File
*/

