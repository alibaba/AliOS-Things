/*******************************************************************************
  TMR Peripheral Library Template Implementation

  File Name:
    tmr_ClockSourceSync_NonInverted.h

  Summary:
    TMR PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : ClockSourceSync
    and its Variant : NonInverted
    For following APIs :
        PLIB_TMR_ClockSourceExternalSyncEnable
        PLIB_TMR_ClockSourceExternalSyncDisable
        PLIB_TMR_ExistsClockSourceSync

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

#ifndef _TMR_CLOCKSOURCESYNC_NONINVERTED_H
#define _TMR_CLOCKSOURCESYNC_NONINVERTED_H

#include "tmr_registers.h"

//******************************************************************************
/* Function :  TMR_ClockSourceExternalSyncEnable_NonInverted

  Summary:
    Implements NonInverted variant of PLIB_TMR_ClockSourceExternalSyncEnable 

  Description:
    This template implements the NonInverted variant of the PLIB_TMR_ClockSourceExternalSyncEnable function.
*/

PLIB_TEMPLATE void TMR_ClockSourceExternalSyncEnable_NonInverted( TMR_MODULE_ID index )
{
    PLIB_ASSERT(false, "This API is not used by any device");
}


//******************************************************************************
/* Function :  TMR_ClockSourceExternalSyncDisable_NonInverted

  Summary:
    Implements NonInverted variant of PLIB_TMR_ClockSourceExternalSyncDisable 

  Description:
    This template implements the NonInverted variant of the PLIB_TMR_ClockSourceExternalSyncDisable function.
*/

PLIB_TEMPLATE void TMR_ClockSourceExternalSyncDisable_NonInverted( TMR_MODULE_ID index )
{
    PLIB_ASSERT(false, "This API is not used by any device");
}


//******************************************************************************
/* Function :  TMR_ExistsClockSourceSync_NonInverted

  Summary:
    Implements NonInverted variant of PLIB_TMR_ExistsClockSourceSync

  Description:
    This template implements the NonInverted variant of the PLIB_TMR_ExistsClockSourceSync function.
*/

#define PLIB_TMR_ExistsClockSourceSync PLIB_TMR_ExistsClockSourceSync
PLIB_TEMPLATE bool TMR_ExistsClockSourceSync_NonInverted( TMR_MODULE_ID index )
{
    PLIB_ASSERT(false, "This API is not used by any device");
    return false;
}


#endif /*_TMR_CLOCKSOURCESYNC_NONINVERTED_H*/

/******************************************************************************
 End of File
*/

