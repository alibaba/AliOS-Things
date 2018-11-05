/*******************************************************************************
  PCACHE Peripheral Library Template Implementation

  File Name:
    pcache_CacheLineType_Unsupported.h

  Summary:
    PCACHE PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : CacheLineType
    and its Variant : Unsupported
    For following APIs :
        PLIB_PCACHE_CacheLineData
        PLIB_PCACHE_CacheLineInst
        PLIB_PCACHE_CacheLineIsInst
        PLIB_PCACHE_ExistsCacheLineType

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

#ifndef _PCACHE_CACHELINETYPE_UNSUPPORTED_H
#define _PCACHE_CACHELINETYPE_UNSUPPORTED_H

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
/* Function :  PCACHE_CacheLineData_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_PCACHE_CacheLineData 

  Description:
    This template implements the Unsupported variant of the PLIB_PCACHE_CacheLineData function.
*/

PLIB_TEMPLATE void PCACHE_CacheLineData_Unsupported( PCACHE_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_PCACHE_CacheLineData");
}


//******************************************************************************
/* Function :  PCACHE_CacheLineInst_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_PCACHE_CacheLineInst 

  Description:
    This template implements the Unsupported variant of the PLIB_PCACHE_CacheLineInst function.
*/

PLIB_TEMPLATE void PCACHE_CacheLineInst_Unsupported( PCACHE_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_PCACHE_CacheLineInst");
}


//******************************************************************************
/* Function :  PCACHE_CacheLineIsInst_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_PCACHE_CacheLineIsInst 

  Description:
    This template implements the Unsupported variant of the PLIB_PCACHE_CacheLineIsInst function.
*/

PLIB_TEMPLATE bool PCACHE_CacheLineIsInst_Unsupported( PCACHE_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_PCACHE_CacheLineIsInst");

    return false;
}


//******************************************************************************
/* Function :  PCACHE_ExistsCacheLineType_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_PCACHE_ExistsCacheLineType

  Description:
    This template implements the Unsupported variant of the PLIB_PCACHE_ExistsCacheLineType function.
*/

PLIB_TEMPLATE bool PCACHE_ExistsCacheLineType_Unsupported( PCACHE_MODULE_ID index )
{
    return false;
}


#endif /*_PCACHE_CACHELINETYPE_UNSUPPORTED_H*/

/******************************************************************************
 End of File
*/

