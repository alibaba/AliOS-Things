/*******************************************************************************
  TMR Peripheral Library Template Implementation

  File Name:
    tmr_ClockSourceSync_Unsupported.h

  Summary:
    TMR PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : ClockSourceSync
    and its Variant : Unsupported
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

#ifndef _TMR_CLOCKSOURCESYNC_UNSUPPORTED_H
#define _TMR_CLOCKSOURCESYNC_UNSUPPORTED_H

//******************************************************************************
/* Function :  TMR_ClockSourceExternalSyncEnable_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_TMR_ClockSourceExternalSyncEnable 

  Description:
    This template implements the Unsupported variant of the PLIB_TMR_ClockSourceExternalSyncEnable function.
*/

PLIB_TEMPLATE void TMR_ClockSourceExternalSyncEnable_Unsupported( TMR_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_TMR_ClockSourceExternalSyncEnable");
}


//******************************************************************************
/* Function :  TMR_ClockSourceExternalSyncDisable_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_TMR_ClockSourceExternalSyncDisable 

  Description:
    This template implements the Unsupported variant of the PLIB_TMR_ClockSourceExternalSyncDisable function.
*/

PLIB_TEMPLATE void TMR_ClockSourceExternalSyncDisable_Unsupported( TMR_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_TMR_ClockSourceExternalSyncDisable");
}


//******************************************************************************
/* Function :  TMR_ExistsClockSourceSync_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_TMR_ExistsClockSourceSync

  Description:
    This template implements the Unsupported variant of the PLIB_TMR_ExistsClockSourceSync function.
*/

PLIB_TEMPLATE bool TMR_ExistsClockSourceSync_Unsupported( TMR_MODULE_ID index )
{
    return false;
}


#endif /*_TMR_CLOCKSOURCESYNC_UNSUPPORTED_H*/

/******************************************************************************
 End of File
*/

