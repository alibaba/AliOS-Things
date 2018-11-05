/*******************************************************************************
  OSC Peripheral Library Template Implementation

  File Name:
    osc_ReferenceOutputEnable_PIC32MX_XLP.h

  Summary:
    OSC PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : ReferenceOutputEnable
    and its Variant : PIC32MX_XLP
    For following APIs :
        PLIB_OSC_ExistsReferenceOutputEnable
        PLIB_OSC_ReferenceOutputEnable
        PLIB_OSC_ReferenceOutputDisable
        PLIB_OSC_ReferenceOutputIsEnabled

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

#ifndef _OSC_REFERENCEOUTPUTENABLE_PIC32MX_XLP_H
#define _OSC_REFERENCEOUTPUTENABLE_PIC32MX_XLP_H

//******************************************************************************
/* Function :  OSC_ExistsReferenceOutputEnable_PIC32MX_XLP

  Summary:
    Implements PIC32MX_XLP variant of PLIB_OSC_ExistsReferenceOutputEnable

  Description:
    This template implements the PIC32MX_XLP variant of the PLIB_OSC_ExistsReferenceOutputEnable function.
*/

#define PLIB_OSC_ExistsReferenceOutputEnable PLIB_OSC_ExistsReferenceOutputEnable
PLIB_TEMPLATE bool OSC_ExistsReferenceOutputEnable_PIC32MX_XLP( OSC_MODULE_ID index )
{
    return true;
}


//******************************************************************************
/* Function :  OSC_ReferenceOutputEnable_PIC32MX_XLP

  Summary:
    Implements PIC32MX_XLP variant of PLIB_OSC_ReferenceOutputEnable 

  Description:
    This template implements the PIC32MX_XLP variant of the PLIB_OSC_ReferenceOutputEnable function.
*/

PLIB_TEMPLATE void OSC_ReferenceOutputEnable_PIC32MX_XLP( OSC_MODULE_ID index , OSC_REFERENCE referenceOsc )
{
    REFO1CONSET = _REFO1CON_OE_MASK;
}


//******************************************************************************
/* Function :  OSC_ReferenceOutputDisable_PIC32MX_XLP

  Summary:
    Implements PIC32MX_XLP variant of PLIB_OSC_ReferenceOutputDisable 

  Description:
    This template implements the PIC32MX_XLP variant of the PLIB_OSC_ReferenceOutputDisable function.
*/

PLIB_TEMPLATE void OSC_ReferenceOutputDisable_PIC32MX_XLP( OSC_MODULE_ID index , OSC_REFERENCE referenceOsc )
{
    REFO1CONCLR = _REFO1CON_OE_MASK;
}


//******************************************************************************
/* Function :  OSC_ReferenceOutputIsEnabled_PIC32MX_XLP

  Summary:
    Implements PIC32MX_XLP variant of PLIB_OSC_ReferenceOutputIsEnabled 

  Description:
    This template implements the PIC32MX_XLP variant of the PLIB_OSC_ReferenceOutputIsEnabled function.
*/

PLIB_TEMPLATE bool OSC_ReferenceOutputIsEnabled_PIC32MX_XLP( OSC_MODULE_ID index , OSC_REFERENCE referenceOsc )
{
    volatile uint32_t *reg = &REFO1CON + (referenceOsc * 0x08u);

    return ( ((*reg) & _REFO1CON_OE_MASK ) ? true : false );
}


#endif /*_OSC_REFERENCEOUTPUTENABLE_PIC32MX_XLP_H*/

/******************************************************************************
 End of File
*/

