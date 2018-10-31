/*******************************************************************************
  TMR Peripheral Library Template Implementation

  File Name:
    tmr_Period32Bit_In16BitRegister_pic32.h

  Summary:
    TMR PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : Period32Bit
    and its Variant : In16BitRegister_pic32
    For following APIs :
        PLIB_TMR_Period32BitSet
        PLIB_TMR_Period32BitGet
        PLIB_TMR_ExistsPeriod32Bit

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

#ifndef _TMR_PERIOD32BIT_IN16BITREGISTER_PIC32_H
#define _TMR_PERIOD32BIT_IN16BITREGISTER_PIC32_H

#include "tmr_registers.h"

//******************************************************************************
/* Function :  TMR_Period32BitSet_In16BitRegister_pic32

  Summary:
    Implements In16BitRegister_pic32 variant of PLIB_TMR_Period32BitSet 

  Description:
    This template implements the In16BitRegister_pic32 variant of the PLIB_TMR_Period32BitSet function.
*/

PLIB_TEMPLATE void TMR_Period32BitSet_In16BitRegister_pic32( TMR_MODULE_ID index , uint32_t period )
{
    tmr_registers_t volatile * tmrX = ((tmr_registers_t *)(index));
    tmr_registers_t volatile * tmrY = ((tmr_registers_t *)(index + 0x0200u));

	if( _TMR_MODULE_ID_IS_EVEN(index) )
	{
        /* Lower 16 bits to Even Timer and higher 16 bits to Odd Timer */
        tmrX->PRx = ( period & 0xFFFFu );
        tmrY->PRx = ( (period >> 16u) & 0xFFFFu );
	}
	else
	{
		PLIB_ASSERT(false, "This Timer instance does not support PLIB_TMR_Period32BitSet");
	}
}


//******************************************************************************
/* Function :  TMR_Period32BitGet_In16BitRegister_pic32

  Summary:
    Implements In16BitRegister_pic32 variant of PLIB_TMR_Period32BitGet 

  Description:
    This template implements the In16BitRegister_pic32 variant of the PLIB_TMR_Period32BitGet function.
*/

PLIB_TEMPLATE uint32_t TMR_Period32BitGet_In16BitRegister_pic32( TMR_MODULE_ID index )
{
    tmr_registers_t volatile * tmrX = ((tmr_registers_t *)(index));
    tmr_registers_t volatile * tmrY = ((tmr_registers_t *)(index + 0x0200u));
    uint32_t val1 = 0u, val2 = 0u;

	if( _TMR_MODULE_ID_IS_EVEN(index) )
	{
        val1 = (uint32_t)(tmrX->PRx);
        val2 = (uint32_t)(tmrY->PRx << 16u);
	}
	else
	{
		PLIB_ASSERT(false, "This Timer instance does not support PLIB_TMR_Period32BitGet");
	}	

    return ( val1 | val2 );
}


//******************************************************************************
/* Function :  TMR_ExistsPeriod32Bit_In16BitRegister_pic32

  Summary:
    Implements In16BitRegister_pic32 variant of PLIB_TMR_ExistsPeriod32Bit

  Description:
    This template implements the In16BitRegister_pic32 variant of the PLIB_TMR_ExistsPeriod32Bit function.
*/

#define PLIB_TMR_ExistsPeriod32Bit PLIB_TMR_ExistsPeriod32Bit
PLIB_TEMPLATE bool TMR_ExistsPeriod32Bit_In16BitRegister_pic32( TMR_MODULE_ID index )
{
    if (TMR_ID_1 == index)
    {
        return false;
    }
    else
    {
        return true;
    }
}


#endif /*_TMR_PERIOD32BIT_IN16BITREGISTER_PIC32_H*/

/******************************************************************************
 End of File
*/

