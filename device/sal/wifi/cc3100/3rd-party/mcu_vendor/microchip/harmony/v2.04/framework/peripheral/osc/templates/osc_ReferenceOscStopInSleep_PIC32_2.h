/*******************************************************************************
  OSC Peripheral Library Template Implementation

  File Name:
    osc_ReferenceOscStopInSleep_PIC32_2.h

  Summary:
    OSC PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : ReferenceOscStopInSleep
    and its Variant : PIC32_2
    For following APIs :
        PLIB_OSC_ExistsReferenceOscStopInSleep
        PLIB_OSC_ReferenceOscStopInSleepEnable
        PLIB_OSC_ReferenceOscStopInSleepDisable
        PLIB_OSC_ReferenceOscStopInSleepIsEnabled

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

#ifndef _OSC_REFERENCEOSCSTOPINSLEEP_PIC32_2_H
#define _OSC_REFERENCEOSCSTOPINSLEEP_PIC32_2_H

//******************************************************************************
/* Function :  OSC_ExistsReferenceOscStopInSleep_PIC32_2

  Summary:
    Implements PIC32_2 variant of PLIB_OSC_ExistsReferenceOscStopInSleep

  Description:
    This template implements the PIC32_2 variant of the PLIB_OSC_ExistsReferenceOscStopInSleep function.
*/

#define PLIB_OSC_ExistsReferenceOscStopInSleep PLIB_OSC_ExistsReferenceOscStopInSleep
PLIB_TEMPLATE bool OSC_ExistsReferenceOscStopInSleep_PIC32_2( OSC_MODULE_ID index )
{
    return true;
}


//******************************************************************************
/* Function :  OSC_ReferenceOscStopInSleepEnable_PIC32_2

  Summary:
    Implements PIC32_2 variant of PLIB_OSC_ReferenceOscStopInSleepEnable 

  Description:
    This template implements the PIC32_2 variant of the PLIB_OSC_ReferenceOscStopInSleepEnable function.
*/

PLIB_TEMPLATE void OSC_ReferenceOscStopInSleepEnable_PIC32_2( OSC_MODULE_ID index , OSC_REFERENCE referenceOsc )
{
    volatile uint32_t *reg = &REFO1CON + (referenceOsc * 0x08u) + 1u;

    *reg = _REFO1CON_RSLP_MASK;
}


//******************************************************************************
/* Function :  OSC_ReferenceOscStopInSleepDisable_PIC32_2

  Summary:
    Implements PIC32_2 variant of PLIB_OSC_ReferenceOscStopInSleepDisable 

  Description:
    This template implements the PIC32_2 variant of the PLIB_OSC_ReferenceOscStopInSleepDisable function.
*/

PLIB_TEMPLATE void OSC_ReferenceOscStopInSleepDisable_PIC32_2( OSC_MODULE_ID index , OSC_REFERENCE referenceOsc )
{
    volatile uint32_t *reg = &REFO1CON + (referenceOsc * 0x08u) + 2u;

    *reg = _REFO1CON_RSLP_MASK;
}


//******************************************************************************
/* Function :  OSC_ReferenceOscStopInSleepIsEnabled_PIC32_2

  Summary:
    Implements PIC32_2 variant of PLIB_OSC_ReferenceOscStopInSleepIsEnabled 

  Description:
    This template implements the PIC32_2 variant of the PLIB_OSC_ReferenceOscStopInSleepIsEnabled function.
*/

PLIB_TEMPLATE bool OSC_ReferenceOscStopInSleepIsEnabled_PIC32_2( OSC_MODULE_ID index , OSC_REFERENCE referenceOsc )
{
    volatile uint32_t *reg = &REFO1CON + (referenceOsc * 0x08u);

    return ( ((*reg) & _REFO1CON_RSLP_MASK ) ? false : true );
}


#endif /*_OSC_REFERENCEOSCSTOPINSLEEP_PIC32_2_H*/

/******************************************************************************
 End of File
*/

