/*******************************************************************************
  OSC Peripheral Library Template Implementation

  File Name:
    osc_OscSwitchInit_Default.h

  Summary:
    OSC PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : OscSwitchInit
    and its Variant : Default
    For following APIs :
        PLIB_OSC_ExistsOscSwitchInit
        PLIB_OSC_ClockSwitchingAbort
        PLIB_OSC_ClockSwitchingIsComplete

*******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2013 released Microchip Technology Inc.  All rights reserved.

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

#ifndef _OSC_OSCSWITCHINIT_DEFAULT_H
#define _OSC_OSCSWITCHINIT_DEFAULT_H

//******************************************************************************
/* Function :  OSC_ExistsOscSwitchInit_Default

  Summary:
    Implements Default variant of PLIB_OSC_ExistsOscSwitchInit

  Description:
    This template implements the Default variant of the PLIB_OSC_ExistsOscSwitchInit function.
*/

#define PLIB_OSC_ExistsOscSwitchInit PLIB_OSC_ExistsOscSwitchInit
PLIB_TEMPLATE bool OSC_ExistsOscSwitchInit_Default( OSC_MODULE_ID index )
{
    return true;
}


//******************************************************************************
/* Function :  OSC_ClockSwitchingAbort_Default

  Summary:
    Implements Default variant of PLIB_OSC_ClockSwitchingAbort 

  Description:
    This template implements the Default variant of the PLIB_OSC_ClockSwitchingAbort function.
*/

PLIB_TEMPLATE void OSC_ClockSwitchingAbort_Default( OSC_MODULE_ID index )
{
    OSCCONCLR = _OSCCON_OSWEN_MASK;
}


//******************************************************************************
/* Function :  OSC_ClockSwitchingIsComplete_Default

  Summary:
    Implements Default variant of PLIB_OSC_ClockSwitchingIsComplete 

  Description:
    This template implements the Default variant of the PLIB_OSC_ClockSwitchingIsComplete function.
*/

PLIB_TEMPLATE bool OSC_ClockSwitchingIsComplete_Default( OSC_MODULE_ID index )
{
    return !( (bool)OSCCONbits.OSWEN );
}


#endif /*_OSC_OSCSWITCHINIT_DEFAULT_H*/

/******************************************************************************
 End of File
*/

