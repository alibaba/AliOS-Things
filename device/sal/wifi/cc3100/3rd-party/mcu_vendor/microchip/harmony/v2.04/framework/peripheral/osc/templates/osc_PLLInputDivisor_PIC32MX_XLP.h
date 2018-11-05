/*******************************************************************************
  OSC Peripheral Library Template Implementation

  File Name:
    osc_PLLInputDivisor_PIC32MX_XLP.h

  Summary:
    OSC PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : PLLInputDivisor
    and its Variant : PIC32MX_XLP
    For following APIs :
        PLIB_OSC_ExistsSysPLLInputDivisor
        PLIB_OSC_SysPLLInputDivisorSet
        PLIB_OSC_SysPLLInputDivisorGet

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

#ifndef _OSC_PLLINPUTDIVISOR_PIC32MX_XLP_H
#define _OSC_PLLINPUTDIVISOR_PIC32MX_XLP_H

// *****************************************************************************
/* System PLL maximum input divisor value.

  Summary:
    Defines System PLL maximum input divisor value.

  Description:
    This macro defines System PLL maximum input divisor value.

  Remarks:
    None
*/

#define OSC_SYSPLL_INPUT_DIV_MAX						12


//******************************************************************************
/* Function :  OSC_ExistsSysPLLInputDivisor_PIC32MX_XLP

  Summary:
    Implements PIC32MX_XLP variant of PLIB_OSC_ExistsSysPLLInputDivisor

  Description:
    This template implements the PIC32MX_XLP variant of the PLIB_OSC_ExistsSysPLLInputDivisor function.
*/

#define PLIB_OSC_ExistsSysPLLInputDivisor PLIB_OSC_ExistsSysPLLInputDivisor
PLIB_TEMPLATE bool OSC_ExistsSysPLLInputDivisor_PIC32MX_XLP( OSC_MODULE_ID index )
{
    return true;
}


//******************************************************************************
/* Function :  OSC_SysPLLInputDivisorSet_PIC32MX_XLP

  Summary:
    Implements PIC32MX_XLP variant of PLIB_OSC_SysPLLInputDivisorSet

  Description:
    This template implements the PIC32MX_XLP variant of the PLIB_OSC_SysPLLInputDivisorSet function.
*/

PLIB_TEMPLATE void OSC_SysPLLInputDivisorSet_PIC32MX_XLP( OSC_MODULE_ID index , uint16_t PLLInDiv )
{
	if ( PLLInDiv < 13u )
	{
		SPLLCONbits.PLLIDIV = PLLInDiv;
	}	
	else
	{	
		PLIB_ASSERT(false, "Given PLL Input Divider value is not supported for the selected device");
	}
	
    Nop();
    Nop();
}


//******************************************************************************
/* Function :  OSC_SysPLLInputDivisorGet_PIC32MX_XLP

  Summary:
    Implements PIC32MX_XLP variant of PLIB_OSC_SysPLLInputDivisorGet

  Description:
    This template implements the PIC32MX_XLP variant of the PLIB_OSC_SysPLLInputDivisorGet function.
*/

PLIB_TEMPLATE uint16_t OSC_SysPLLInputDivisorGet_PIC32MX_XLP( OSC_MODULE_ID index )
{
	uint16_t inputDivider, returnValue = 0;
	
	inputDivider = SPLLCONbits.PLLIDIV;
	
	if (inputDivider == 7u )
	{
		returnValue = (inputDivider + 5u);
	}	
	else if (inputDivider == 6u)
	{
		returnValue = (inputDivider + 4u);
	}
	else
	{
		returnValue = (inputDivider + 1u);
	}
	
	return returnValue;
}


#endif /*_OSC_PLLINPUTDIVISOR_PIC32MX_XLP_H*/

/******************************************************************************
 End of File
*/

