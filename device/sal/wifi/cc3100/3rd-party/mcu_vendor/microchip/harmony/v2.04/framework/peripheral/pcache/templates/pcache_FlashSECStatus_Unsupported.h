/*******************************************************************************
  PCACHE Peripheral Library Template Implementation

  File Name:
    pcache_FlashSECStatus_Unsupported.h

  Summary:
    PCACHE PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : FlashSECStatus
    and its Variant : Unsupported
    For following APIs :
        PLIB_PCACHE_ExistsFlashSECStatus
        PLIB_PCACHE_FlashSECStatusGet
        PLIB_PCACHE_FlashSECStatusSet
        PLIB_PCACHE_FlashSECStatusClear

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

#ifndef _PCACHE_FLASHSECSTATUS_UNSUPPORTED_H
#define _PCACHE_FLASHSECSTATUS_UNSUPPORTED_H

//******************************************************************************
/* Routines available for accessing VREGS, MASKS, POS, LEN are 

  VREGs: 
    None.

  MASKs: 
    None.

  POSs: 
    None.

  LENs: 
    None.

*/


//******************************************************************************
/* Function :  PCACHE_ExistsFlashSECStatus_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_PCACHE_ExistsFlashSECStatus

  Description:
    This template implements the Unsupported variant of the PLIB_PCACHE_ExistsFlashSECStatus function.
*/

PLIB_TEMPLATE bool PCACHE_ExistsFlashSECStatus_Unsupported( PCACHE_MODULE_ID index )
{
    return false;
}


//******************************************************************************
/* Function :  PCACHE_FlashSECStatusGet_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_PCACHE_FlashSECStatusGet 

  Description:
    This template implements the Unsupported variant of the PLIB_PCACHE_FlashSECStatusGet function.
*/

PLIB_TEMPLATE bool PCACHE_FlashSECStatusGet_Unsupported( PCACHE_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_PCACHE_FlashSECStatusGet");

    return false;
}


//******************************************************************************
/* Function :  PCACHE_FlashSECStatusSet_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_PCACHE_FlashSECStatusSet 

  Description:
    This template implements the Unsupported variant of the PLIB_PCACHE_FlashSECStatusSet function.
*/

PLIB_TEMPLATE void PCACHE_FlashSECStatusSet_Unsupported( PCACHE_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_PCACHE_FlashSECStatusSet");
}


//******************************************************************************
/* Function :  PCACHE_FlashSECStatusClear_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_PCACHE_FlashSECStatusClear 

  Description:
    This template implements the Unsupported variant of the PLIB_PCACHE_FlashSECStatusClear function.
*/

PLIB_TEMPLATE void PCACHE_FlashSECStatusClear_Unsupported( PCACHE_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_PCACHE_FlashSECStatusClear");
}


#endif /*_PCACHE_FLASHSECSTATUS_UNSUPPORTED_H*/

/******************************************************************************
 End of File
*/

