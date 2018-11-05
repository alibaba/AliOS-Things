/*******************************************************************************
  OSC Peripheral Library Template Implementation

  File Name:
    osc_DreamModeControl_Unsupported.h

  Summary:
    OSC PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : DreamModeControl
    and its Variant : Unsupported
    For following APIs :
        PLIB_OSC_ExistsDreamModeControl
        PLIB_OSC_DreamModeEnable
        PLIB_OSC_DreamModeDisable
        PLIB_OSC_DreamModeStatus

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

#ifndef _OSC_DREAMMODECONTROL_UNSUPPORTED_H
#define _OSC_DREAMMODECONTROL_UNSUPPORTED_H

//******************************************************************************
/* Function :  OSC_ExistsDreamModeControl_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_OSC_ExistsDreamModeControl

  Description:
    This template implements the Unsupported variant of the PLIB_OSC_ExistsDreamModeControl function.
*/

PLIB_TEMPLATE bool OSC_ExistsDreamModeControl_Unsupported( OSC_MODULE_ID index )
{
    return false;
}


//******************************************************************************
/* Function :  OSC_DreamModeEnable_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_OSC_DreamModeEnable 

  Description:
    This template implements the Unsupported variant of the PLIB_OSC_DreamModeEnable function.
*/

PLIB_TEMPLATE void OSC_DreamModeEnable_Unsupported( OSC_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_OSC_DreamModeEnable");
}


//******************************************************************************
/* Function :  OSC_DreamModeDisable_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_OSC_DreamModeDisable 

  Description:
    This template implements the Unsupported variant of the PLIB_OSC_DreamModeDisable function.
*/

PLIB_TEMPLATE void OSC_DreamModeDisable_Unsupported( OSC_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_OSC_DreamModeDisable");
}


//******************************************************************************
/* Function :  OSC_DreamModeStatus_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_OSC_DreamModeStatus 

  Description:
    This template implements the Unsupported variant of the PLIB_OSC_DreamModeStatus function.
*/

PLIB_TEMPLATE bool OSC_DreamModeStatus_Unsupported( OSC_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_OSC_DreamModeStatus");

    return false;
}


#endif /*_OSC_DREAMMODECONTROL_UNSUPPORTED_H*/

/******************************************************************************
 End of File
*/

