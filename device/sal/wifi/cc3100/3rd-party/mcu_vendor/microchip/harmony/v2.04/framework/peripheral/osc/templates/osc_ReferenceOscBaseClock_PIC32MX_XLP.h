/*******************************************************************************
  OSC Peripheral Library Template Implementation

  File Name:
    osc_ReferenceOscBaseClock_PIC32MX_XLP.h

  Summary:
    OSC PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : ReferenceOscBaseClock
    and its Variant : PIC32MX_XLP
    For following APIs :
        PLIB_OSC_ExistsReferenceOscBaseClock
        PLIB_OSC_ReferenceOscBaseClockSelect

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

#ifndef _OSC_REFERENCEOSCBASECLOCK_PIC32MX_XLP_H
#define _OSC_REFERENCEOSCBASECLOCK_PIC32MX_XLP_H

//******************************************************************************
/* Function :  OSC_ExistsReferenceOscBaseClock_PIC32MX_XLP

  Summary:
    Implements PIC32MX_XLP variant of PLIB_OSC_ExistsReferenceOscBaseClock

  Description:
    This template implements the PIC32MX_XLP variant of the PLIB_OSC_ExistsReferenceOscBaseClock function.
*/

#define PLIB_OSC_ExistsReferenceOscBaseClock PLIB_OSC_ExistsReferenceOscBaseClock
PLIB_TEMPLATE bool OSC_ExistsReferenceOscBaseClock_PIC32MX_XLP( OSC_MODULE_ID index )
{
    return true;
}


//******************************************************************************
/* Function :  OSC_ReferenceOscBaseClockSelect_PIC32MX_XLP

  Summary:
    Implements PIC32MX_XLP variant of PLIB_OSC_ReferenceOscBaseClockSelect 

  Description:
    This template implements the PIC32MX_XLP variant of the PLIB_OSC_ReferenceOscBaseClockSelect function.
*/

PLIB_TEMPLATE void OSC_ReferenceOscBaseClockSelect_PIC32MX_XLP( OSC_MODULE_ID index , OSC_REFERENCE referenceOsc , OSC_REF_BASECLOCK refOscBaseClock )
{
    REFO1CONbits.ROSEL = refOscBaseClock;
    REFO1CONSET = _REFO1CON_DIVSWEN_MASK;
}


#endif /*_OSC_REFERENCEOSCBASECLOCK_PIC32MX_XLP_H*/

/******************************************************************************
 End of File
*/

