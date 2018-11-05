/*******************************************************************************
  PCACHE Peripheral Library Template Implementation

  File Name:
    pcache_LeastRecentlyUsedState_Default.h

  Summary:
    PCACHE PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : LeastRecentlyUsedState
    and its Variant : Default
    For following APIs :
        PLIB_PCACHE_LeastRecentlyUsedStateRead
        PLIB_PCACHE_ExistsLeastRecentlyUsedState

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

#ifndef _PCACHE_LEASTRECENTLYUSEDSTATE_DEFAULT_H
#define _PCACHE_LEASTRECENTLYUSEDSTATE_DEFAULT_H

#include "pcache_Registers.h"

//******************************************************************************
/* Function :  PCACHE_LeastRecentlyUsedStateRead_Default

  Summary:
    Implements Default variant of PLIB_PCACHE_LeastRecentlyUsedStateRead 

  Description:
    This template implements the Default variant of the PLIB_PCACHE_LeastRecentlyUsedStateRead function.
    Operation is atomic.
*/

PLIB_TEMPLATE uint32_t PCACHE_LeastRecentlyUsedStateRead_Default( PCACHE_MODULE_ID index )
{
    volatile pcache_register_t *regs = (pcache_register_t *)index;

    return regs->CHELRU;
}


//******************************************************************************
/* Function :  PCACHE_ExistsLeastRecentlyUsedState_Default

  Summary:
    Implements Default variant of PLIB_PCACHE_ExistsLeastRecentlyUsedState

  Description:
    This template implements the Default variant of the PLIB_PCACHE_ExistsLeastRecentlyUsedState function.
*/

#define PLIB_PCACHE_ExistsLeastRecentlyUsedState PLIB_PCACHE_ExistsLeastRecentlyUsedState
PLIB_TEMPLATE bool PCACHE_ExistsLeastRecentlyUsedState_Default( PCACHE_MODULE_ID index )
{
    return true;
}


#endif /*_PCACHE_LEASTRECENTLYUSEDSTATE_DEFAULT_H*/

/******************************************************************************
 End of File
*/

