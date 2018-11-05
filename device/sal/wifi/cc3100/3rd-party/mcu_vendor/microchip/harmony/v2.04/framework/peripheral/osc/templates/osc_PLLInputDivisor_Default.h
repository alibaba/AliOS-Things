/*******************************************************************************
  OSC Peripheral Library Template Implementation

  File Name:
    osc_PLLInputDivisor_Default.h

  Summary:
    OSC PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : PLLInputDivisor
    and its Variant : Default
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

#ifndef _OSC_PLLINPUTDIVISOR_DEFAULT_H
#define _OSC_PLLINPUTDIVISOR_DEFAULT_H

// *****************************************************************************
/* System PLL maximum input divisor value.

  Summary:
    Defines System PLL maximum input divisor value.

  Description:
    This macro defines System PLL maximum input divisor value.

  Remarks:
    None
*/

#define OSC_SYSPLL_INPUT_DIV_MAX						8


//******************************************************************************
/* Function :  OSC_ExistsSysPLLInputDivisor_Default

  Summary:
    Implements Default variant of PLIB_OSC_ExistsSysPLLInputDivisor

  Description:
    This template implements the Default variant of the PLIB_OSC_ExistsSysPLLInputDivisor function.
*/

#define PLIB_OSC_ExistsSysPLLInputDivisor PLIB_OSC_ExistsSysPLLInputDivisor
PLIB_TEMPLATE bool OSC_ExistsSysPLLInputDivisor_Default( OSC_MODULE_ID index )
{
    return true;
}


//******************************************************************************
/* Function :  OSC_SysPLLInputDivisorSet_Default

  Summary:
    Implements Default variant of PLIB_OSC_SysPLLInputDivisorSet

  Description:
    This template implements the Default variant of the PLIB_OSC_SysPLLInputDivisorSet function.
*/

PLIB_TEMPLATE void OSC_SysPLLInputDivisorSet_Default( OSC_MODULE_ID index , uint16_t PLLInDiv )
{
    SPLLCONbits.PLLIDIV = PLLInDiv;

    Nop();
    Nop();
}


//******************************************************************************
/* Function :  OSC_SysPLLInputDivisorGet_Default

  Summary:
    Implements Default variant of PLIB_OSC_SysPLLInputDivisorGet

  Description:
    This template implements the Default variant of the PLIB_OSC_SysPLLInputDivisorGet function.
*/

PLIB_TEMPLATE uint16_t OSC_SysPLLInputDivisorGet_Default( OSC_MODULE_ID index )
{
    return (uint16_t)SPLLCONbits.PLLIDIV;
}


#endif /*_OSC_PLLINPUTDIVISOR_DEFAULT_H*/

/******************************************************************************
 End of File
*/

