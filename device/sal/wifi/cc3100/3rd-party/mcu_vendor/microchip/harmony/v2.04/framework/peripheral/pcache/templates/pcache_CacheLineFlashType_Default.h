/*******************************************************************************
  PCACHE Peripheral Library Template Implementation

  File Name:
    pcache_CacheLineFlashType_Default.h

  Summary:
    PCACHE PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : CacheLineFlashType
    and its Variant : Default
    For following APIs :
        PLIB_PCACHE_CacheLineFlashTypeBoot
        PLIB_PCACHE_CacheLineFlashTypeInst
        PLIB_PCACHE_CacheLineFlashTypeIsInst
        PLIB_PCACHE_ExistsCacheLineFlashType

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

#ifndef _PCACHE_CACHELINEFLASHTYPE_DEFAULT_H
#define _PCACHE_CACHELINEFLASHTYPE_DEFAULT_H

#include "pcache_Registers.h"

//******************************************************************************
/* Function :  PCACHE_CacheLineFlashTypeBoot_Default

  Summary:
    Implements Default variant of PLIB_PCACHE_CacheLineFlashTypeBoot 

  Description:
    This template implements the Default variant of the PLIB_PCACHE_CacheLineFlashTypeBoot function.
    Operation is atomic.
*/

PLIB_TEMPLATE void PCACHE_CacheLineFlashTypeBoot_Default( PCACHE_MODULE_ID index )
{
    volatile pcache_register_t *regs = (pcache_register_t *)index;

    regs->CHETAGCLR = _CHETAG_LTAGBOOT_MASK;
}


//******************************************************************************
/* Function :  PCACHE_CacheLineFlashTypeInst_Default

  Summary:
    Implements Default variant of PLIB_PCACHE_CacheLineFlashTypeInst 

  Description:
    This template implements the Default variant of the PLIB_PCACHE_CacheLineFlashTypeInst function.
    Operation is atomic.
*/

PLIB_TEMPLATE void PCACHE_CacheLineFlashTypeInst_Default( PCACHE_MODULE_ID index )
{
    volatile pcache_register_t *regs = (pcache_register_t *)index;

    regs->CHETAGSET = _CHETAG_LTAGBOOT_MASK;
}


//******************************************************************************
/* Function :  PCACHE_CacheLineFlashTypeIsInst_Default

  Summary:
    Implements Default variant of PLIB_PCACHE_CacheLineFlashTypeIsInst 

  Description:
    This template implements the Default variant of the PLIB_PCACHE_CacheLineFlashTypeIsInst function.
    Operation is atomic.
*/

PLIB_TEMPLATE bool PCACHE_CacheLineFlashTypeIsInst_Default( PCACHE_MODULE_ID index )
{
    volatile pcache_register_t *regs = (pcache_register_t *)index;

    return (bool)regs->CHETAG.LTAGBOOT;
}


//******************************************************************************
/* Function :  PCACHE_ExistsCacheLineFlashType_Default

  Summary:
    Implements Default variant of PLIB_PCACHE_ExistsCacheLineFlashType

  Description:
    This template implements the Default variant of the PLIB_PCACHE_ExistsCacheLineFlashType function.
*/

#define PLIB_PCACHE_ExistsCacheLineFlashType PLIB_PCACHE_ExistsCacheLineFlashType
PLIB_TEMPLATE bool PCACHE_ExistsCacheLineFlashType_Default( PCACHE_MODULE_ID index )
{
    return true;
}


#endif /*_PCACHE_CACHELINEFLASHTYPE_DEFAULT_H*/

/******************************************************************************
 End of File
*/

