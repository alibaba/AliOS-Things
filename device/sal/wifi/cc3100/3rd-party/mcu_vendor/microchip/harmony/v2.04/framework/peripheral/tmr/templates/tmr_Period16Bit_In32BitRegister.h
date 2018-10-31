/*******************************************************************************
  TMR Peripheral Library Template Implementation

  File Name:
    tmr_Period16Bit_In32BitRegister.h

  Summary:
    TMR PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : Period16Bit
    and its Variant : In32BitRegister
    For following APIs :
        PLIB_TMR_Period16BitSet
        PLIB_TMR_Period16BitGet
        PLIB_TMR_ExistsPeriod16Bit

*******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2012 released Microchip Technology Inc.  All rights reserved.

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

#ifndef _TMR_PERIOD16BIT_IN_32BIT_REGISTER_H
#define _TMR_PERIOD16BIT_IN_32BIT_REGISTER_H

#include "tmr_registers.h"


//******************************************************************************
/* Function :  TMR_Period16BitSet_In32BitRegister

  Summary:
    Implements In32BitRegister variant of PLIB_TMR_Period16BitSet

  Description:
    This template implements the In32BitRegister variant of the PLIB_TMR_Period16BitSet function.
*/

PLIB_TEMPLATE void TMR_Period16BitSet_In32BitRegister( TMR_MODULE_ID index , uint16_t period )
{
    tmr_registers_t volatile * tmr = ((tmr_registers_t *)(index));
    if (TMR_ID_1 == index)
	{
		tmr->PRx = period;
	}
	else
	{	
		tmr->PRx = ((tmr->PRx & 0xFFFF0000) | (uint32_t)period ) ;
	}
}


//******************************************************************************
/* Function :  TMR_Period16BitGet_In32BitRegister

  Summary:
    Implements In32BitRegister variant of PLIB_TMR_Period16BitGet

  Description:
    This template implements the In32BitRegister variant of the PLIB_TMR_Period16BitGet function.
*/

PLIB_TEMPLATE uint16_t TMR_Period16BitGet_In32BitRegister( TMR_MODULE_ID index )
{
    tmr_registers_t volatile * tmr = ((tmr_registers_t *)(index));
	return (uint16_t)tmr->PRx;
}


//******************************************************************************
/* Function :  TMR_ExistsPeriod16Bit_In32BitRegister

  Summary:
    Implements In32BitRegister variant of PLIB_TMR_ExistsPeriod16Bit

  Description:
    This template implements the In32BitRegister variant of the PLIB_TMR_ExistsPeriod16Bit function.
*/

#define PLIB_TMR_ExistsPeriod16Bit PLIB_TMR_ExistsPeriod16Bit
PLIB_TEMPLATE bool TMR_ExistsPeriod16Bit_In32BitRegister( TMR_MODULE_ID index )
{
    return true;
}


#endif /*_TMR_PERIOD16BIT_IN_32BIT_REGISTER_H*/

/******************************************************************************
 End of File
*/

