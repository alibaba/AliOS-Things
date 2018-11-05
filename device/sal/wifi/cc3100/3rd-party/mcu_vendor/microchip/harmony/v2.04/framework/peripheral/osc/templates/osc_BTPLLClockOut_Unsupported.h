/*******************************************************************************
  OSC Peripheral Library Template Implementation

  File Name:
    osc_BTPLLClockOut_Unsupported.h

  Summary:
    OSC PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : BTPLLClockOut
    and its Variant : Unsupported
    For following APIs :
        PLIB_OSC_ExistsBTPLLClockOut
        PLIB_OSC_BTPLLClockOutEnable
        PLIB_OSC_BTPLLClockOutDisable
        PLIB_OSC_BTPLLClockOutStatus

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

#ifndef _OSC_BTPLLCLOCKOUT_UNSUPPORTED_H
#define _OSC_BTPLLCLOCKOUT_UNSUPPORTED_H

//******************************************************************************
/* Function :  OSC_ExistsBTPLLClockOut_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_OSC_ExistsBTPLLClockOut

  Description:
    This template implements the Unsupported variant of the PLIB_OSC_ExistsBTPLLClockOut function.
*/

PLIB_TEMPLATE bool OSC_ExistsBTPLLClockOut_Unsupported( OSC_MODULE_ID index )
{
    return false;
}


//******************************************************************************
/* Function :  OSC_BTPLLClockOutEnable_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_OSC_BTPLLClockOutEnable 

  Description:
    This template implements the Unsupported variant of the PLIB_OSC_BTPLLClockOutEnable function.
*/

PLIB_TEMPLATE void OSC_BTPLLClockOutEnable_Unsupported( OSC_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_OSC_BTPLLClockOutEnable");
}


//******************************************************************************
/* Function :  OSC_BTPLLClockOutDisable_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_OSC_BTPLLClockOutDisable 

  Description:
    This template implements the Unsupported variant of the PLIB_OSC_BTPLLClockOutDisable function.
*/

PLIB_TEMPLATE void OSC_BTPLLClockOutDisable_Unsupported( OSC_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_OSC_BTPLLClockOutDisable");
}


//******************************************************************************
/* Function :  OSC_BTPLLClockOutStatus_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_OSC_BTPLLClockOutStatus 

  Description:
    This template implements the Unsupported variant of the PLIB_OSC_BTPLLClockOutStatus function.
*/

PLIB_TEMPLATE bool OSC_BTPLLClockOutStatus_Unsupported( OSC_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_OSC_BTPLLClockOutStatus");

    return false;
}


#endif /*_OSC_BTPLLCLOCKOUT_UNSUPPORTED_H*/

/******************************************************************************
 End of File
*/

