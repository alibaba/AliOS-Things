/*******************************************************************************
  OSC Peripheral Library Template Implementation

  File Name:
    osc_UPLLMultiplier_PIC32WK.h

  Summary:
    OSC PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : UPLLMultiplier
    and its Variant : PIC32WK
    For following APIs :
        PLIB_OSC_ExistsUPLLMultiplier
        PLIB_OSC_UPLLMultiplierSelect
        PLIB_OSC_UPLLMultiplierGet

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

#ifndef _OSC_UPLLMULTIPLIER_PIC32WK_H
#define _OSC_UPLLMULTIPLIER_PIC32WK_H


// *****************************************************************************
/* Oscillator System PLL Multiplier Value Type

  Summary:
    Type of the oscillator system PLL multiplier value.

  Description:
    This macro defines the type of the oscillator system PLL multiplier value.

  Remarks:
    None
*/

#define OSC_SYSPLL_MULTIPLIER_TYPE						SFR_TYPE

//******************************************************************************
/* Function :  OSC_ExistsUPLLMultiplier_PIC32WK

  Summary:
    Implements PIC32WK variant of PLIB_OSC_ExistsUPLLMultiplier

  Description:
    This template implements the PIC32WK variant of the PLIB_OSC_ExistsUPLLMultiplier function.
*/

#define PLIB_OSC_ExistsUPLLMultiplier PLIB_OSC_ExistsUPLLMultiplier
PLIB_TEMPLATE bool OSC_ExistsUPLLMultiplier_PIC32WK( OSC_MODULE_ID index )
{
    return true;
}


//******************************************************************************
/* Function :  OSC_UPLLMultiplierSelect_PIC32WK

  Summary:
    Implements PIC32WK variant of PLIB_OSC_UPLLMultiplierSelect 

  Description:
    This template implements the PIC32WK variant of the PLIB_OSC_UPLLMultiplierSelect function.
*/

PLIB_TEMPLATE void OSC_UPLLMultiplierSelect_PIC32WK( OSC_MODULE_ID index , OSC_SYSPLL_MULTIPLIER_TYPE pll_multiplier )
{
	if ( pll_multiplier < 256u)
	{
        UPLLCONbits.UPLLMULT = (pll_multiplier - 1);
	}
	else
	{
	    PLIB_ASSERT(false, "Given PLL Multiplier value is not supported for the selected device");
	}
}


//******************************************************************************
/* Function :  OSC_UPLLMultiplierGet_PIC32WK

  Summary:
    Implements PIC32WK variant of PLIB_OSC_UPLLMultiplierGet 

  Description:
    This template implements the PIC32WK variant of the PLIB_OSC_UPLLMultiplierGet function.
*/

PLIB_TEMPLATE OSC_SYSPLL_MULTIPLIER_TYPE OSC_UPLLMultiplierGet_PIC32WK( OSC_MODULE_ID index )
{
    return (OSC_SYSPLL_MULTIPLIER_TYPE)( UPLLCONbits.UPLLMULT + 1);
}


#endif /*_OSC_UPLLMULTIPLIER_PIC32WK_H*/

/******************************************************************************
 End of File
*/

