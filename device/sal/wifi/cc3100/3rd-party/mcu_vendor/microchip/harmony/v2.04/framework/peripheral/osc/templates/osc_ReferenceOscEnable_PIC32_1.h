/*******************************************************************************
  OSC Peripheral Library Template Implementation

  File Name:
    osc_ReferenceOscEnable_PIC32_1.h

  Summary:
    OSC PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : ReferenceOscEnable
    and its Variant : PIC32_1
    For following APIs :
        PLIB_OSC_ExistsReferenceOscEnable
        PLIB_OSC_ReferenceOscEnable
        PLIB_OSC_ReferenceOscDisable
        PLIB_OSC_ReferenceOscIsEnabled

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

#ifndef _OSC_REFERENCEOSCENABLE_PIC32_1_H
#define _OSC_REFERENCEOSCENABLE_PIC32_1_H

//******************************************************************************
/* Function :  OSC_ExistsReferenceOscEnable_PIC32_1

  Summary:
    Implements PIC32_1 variant of PLIB_OSC_ExistsReferenceOscEnable

  Description:
    This template implements the PIC32_1 variant of the PLIB_OSC_ExistsReferenceOscEnable function.
*/

#define PLIB_OSC_ExistsReferenceOscEnable PLIB_OSC_ExistsReferenceOscEnable
PLIB_TEMPLATE bool OSC_ExistsReferenceOscEnable_PIC32_1( OSC_MODULE_ID index )
{
    return true;
}


//******************************************************************************
/* Function :  OSC_ReferenceOscEnable_PIC32_1

  Summary:
    Implements PIC32_1 variant of PLIB_OSC_ReferenceOscEnable 

  Description:
    This template implements the PIC32_1 variant of the PLIB_OSC_ReferenceOscEnable function.
*/

PLIB_TEMPLATE void OSC_ReferenceOscEnable_PIC32_1( OSC_MODULE_ID index , OSC_REFERENCE referenceOsc )
{
    REFOCONSET = _REFOCON_ON_MASK;
}


//******************************************************************************
/* Function :  OSC_ReferenceOscDisable_PIC32_1

  Summary:
    Implements PIC32_1 variant of PLIB_OSC_ReferenceOscDisable 

  Description:
    This template implements the PIC32_1 variant of the PLIB_OSC_ReferenceOscDisable function.
*/

PLIB_TEMPLATE void OSC_ReferenceOscDisable_PIC32_1( OSC_MODULE_ID index , OSC_REFERENCE referenceOsc )
{
    REFOCONCLR = _REFOCON_ON_MASK;
}


//******************************************************************************
/* Function :  OSC_ReferenceOscIsEnabled_PIC32_1

  Summary:
    Implements PIC32_1 variant of PLIB_OSC_ReferenceOscIsEnabled 

  Description:
    This template implements the PIC32_1 variant of the PLIB_OSC_ReferenceOscIsEnabled function.
*/

PLIB_TEMPLATE bool OSC_ReferenceOscIsEnabled_PIC32_1( OSC_MODULE_ID index , OSC_REFERENCE referenceOsc )
{
    return (bool)REFOCONbits.ON;
}


#endif /*_OSC_REFERENCEOSCENABLE_PIC32_1_H*/

/******************************************************************************
 End of File
*/

