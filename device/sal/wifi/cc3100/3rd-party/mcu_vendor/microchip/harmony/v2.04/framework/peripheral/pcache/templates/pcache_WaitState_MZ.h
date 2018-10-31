/*******************************************************************************
  PCACHE Peripheral Library Template Implementation

  File Name:
    pcache_WaitState_MZ.h

  Summary:
    PCACHE PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : WaitState
    and its Variant : MZ
    For following APIs :
        PLIB_PCACHE_ExistsWaitState
        PLIB_PCACHE_WaitStateSet
        PLIB_PCACHE_WaitStateGet

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

#ifndef _PCACHE_WAITSTATE_MZ_H
#define _PCACHE_WAITSTATE_MZ_H

#include "pcache_Registers.h"

//******************************************************************************
/* Function :  PCACHE_ExistsWaitState_MZ

  Summary:
    Implements MZ variant of PLIB_PCACHE_ExistsWaitState

  Description:
    This template implements the MZ variant of the PLIB_PCACHE_ExistsWaitState function.
*/

#define PLIB_PCACHE_ExistsWaitState PLIB_PCACHE_ExistsWaitState
#define PLIB_PCACHE_ExistsWaitState PLIB_PCACHE_ExistsWaitState
PLIB_TEMPLATE bool PCACHE_ExistsWaitState_MZ( PCACHE_MODULE_ID index )
{
    return true;
}


//******************************************************************************
/* Function :  PCACHE_WaitStateSet_MZ

  Summary:
    Implements MZ variant of PLIB_PCACHE_WaitStateSet 

  Description:
    This template implements the MZ variant of the PLIB_PCACHE_WaitStateSet function.
    Operation is not atomic.
*/

PLIB_TEMPLATE void PCACHE_WaitStateSet_MZ( PCACHE_MODULE_ID index , uint32_t clocks )
{
    volatile pfm_register_t *regs = (pfm_register_t *)index;
    regs->PRECON.PFMWS = clocks;
}


//******************************************************************************
/* Function :  PCACHE_WaitStateGet_MZ

  Summary:
    Implements MZ variant of PLIB_PCACHE_WaitStateGet 

  Description:
    This template implements the MZ variant of the PLIB_PCACHE_WaitStateGet function.
    Operation is atomic.
*/

PLIB_TEMPLATE uint32_t PCACHE_WaitStateGet_MZ( PCACHE_MODULE_ID index )
{
    volatile pfm_register_t *regs = (pfm_register_t *)index;
    return (uint32_t)(regs->PRECON.PFMWS);
}


#endif /*_PCACHE_WAITSTATE_MZ_H*/

/******************************************************************************
 End of File
*/

