/*******************************************************************************
  OSC Peripheral Library Template Implementation

  File Name:
    osc_SlewDivisorStepControl_Default.h

  Summary:
    OSC PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : SlewDivisorStepControl
    and its Variant : Default
    For following APIs :
        PLIB_OSC_ExistsSlewDivisorStepControl
        PLIB_OSC_SlewDivisorStepSelect
        PLIB_OSC_SlewDivisorStepGet

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

#ifndef _OSC_SLEWDIVISORSTEPCONTROL_DEFAULT_H
#define _OSC_SLEWDIVISORSTEPCONTROL_DEFAULT_H

//******************************************************************************
/* Function :  OSC_ExistsSlewDivisorStepControl_Default

  Summary:
    Implements Default variant of PLIB_OSC_ExistsSlewDivisorStepControl

  Description:
    This template implements the Default variant of the PLIB_OSC_ExistsSlewDivisorStepControl function.
*/

#define PLIB_OSC_ExistsSlewDivisorStepControl PLIB_OSC_ExistsSlewDivisorStepControl
PLIB_TEMPLATE bool OSC_ExistsSlewDivisorStepControl_Default( OSC_MODULE_ID index )
{
    return true;
}


//******************************************************************************
/* Function :  OSC_SlewDivisorStepSelect_Default

  Summary:
    Implements Default variant of PLIB_OSC_SlewDivisorStepSelect 

  Description:
    This template implements the Default variant of the PLIB_OSC_SlewDivisorStepSelect function.
*/

PLIB_TEMPLATE void OSC_SlewDivisorStepSelect_Default( OSC_MODULE_ID index , uint32_t slewSteps )
{
    SLEWCONbits.SLWDIV = slewSteps;
}


//******************************************************************************
/* Function :  OSC_SlewDivisorStepGet_Default

  Summary:
    Implements Default variant of PLIB_OSC_SlewDivisorStepGet 

  Description:
    This template implements the Default variant of the PLIB_OSC_SlewDivisorStepGet function.
*/

PLIB_TEMPLATE uint32_t OSC_SlewDivisorStepGet_Default( OSC_MODULE_ID index )
{
    return (uint32_t)SLEWCONbits.SLWDIV;
}


#endif /*_OSC_SLEWDIVISORSTEPCONTROL_DEFAULT_H*/

/******************************************************************************
 End of File
*/

