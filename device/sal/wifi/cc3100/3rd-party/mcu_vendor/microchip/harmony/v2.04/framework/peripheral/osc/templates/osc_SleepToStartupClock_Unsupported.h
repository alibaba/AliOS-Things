/*******************************************************************************
  OSC Peripheral Library Template Implementation

  File Name:
    osc_SleepToStartupClock_Unsupported.h

  Summary:
    OSC PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : SleepToStartupClock
    and its Variant : Unsupported
    For following APIs :
        PLIB_OSC_ExistsSleepToStartupClock
        PLIB_OSC_SleepToStartupClockSelect
        PLIB_OSC_SleepToStartupClockGet

*******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2013-2014 released Microchip Technology Inc.  All rights reserved.

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

#ifndef _OSC_SLEEPTOSTARTUPCLOCK_UNSUPPORTED_H
#define _OSC_SLEEPTOSTARTUPCLOCK_UNSUPPORTED_H

//******************************************************************************
/* Function :  OSC_ExistsSleepToStartupClock_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_OSC_ExistsSleepToStartupClock

  Description:
    This template implements the Unsupported variant of the PLIB_OSC_ExistsSleepToStartupClock function.
*/

PLIB_TEMPLATE bool OSC_ExistsSleepToStartupClock_Unsupported( OSC_MODULE_ID index )
{
    return false;
}


//******************************************************************************
/* Function :  OSC_SleepToStartupClockSelect_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_OSC_SleepToStartupClockSelect 

  Description:
    This template implements the Unsupported variant of the PLIB_OSC_SleepToStartupClockSelect function.
*/

PLIB_TEMPLATE void OSC_SleepToStartupClockSelect_Unsupported( OSC_MODULE_ID index , OSC_SLEEP_TO_STARTUP_CLK_TYPE startupOsc )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_OSC_SleepToStartupClockSelect");
}


//******************************************************************************
/* Function :  OSC_SleepToStartupClockGet_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_OSC_SleepToStartupClockGet 

  Description:
    This template implements the Unsupported variant of the PLIB_OSC_SleepToStartupClockGet function.
*/

PLIB_TEMPLATE OSC_SLEEP_TO_STARTUP_CLK_TYPE OSC_SleepToStartupClockGet_Unsupported( OSC_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_OSC_SleepToStartupClockGet");

    return (OSC_SLEEP_TO_STARTUP_CLK_TYPE) 0;
}


#endif /*_OSC_SLEEPTOSTARTUPCLOCK_UNSUPPORTED_H*/

/******************************************************************************
 End of File
*/

