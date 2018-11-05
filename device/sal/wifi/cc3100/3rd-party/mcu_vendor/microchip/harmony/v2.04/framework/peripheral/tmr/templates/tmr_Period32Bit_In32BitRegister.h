/*******************************************************************************
  TMR Peripheral Library Template Implementation

  File Name:
    tmr_Period32Bit_In32BitRegister.h

  Summary:
    TMR PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : Period32Bit
    and its Variant : In32BitRegister
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

#ifndef _TMR_PERIOD32BIT_IN32BITREGISTER_H
#define _TMR_PERIOD32BIT_IN32BITREGISTER_H

#include "tmr_registers.h"

//******************************************************************************
/* Function :  TMR_Period32BitSet_In32BitRegister

  Summary:
    Implements In32BitRegister variant of PLIB_TMR_Period32BitSet 

  Description:
    This template implements the In32BitRegister variant of the PLIB_TMR_Period32BitSet function.
*/

PLIB_TEMPLATE void TMR_Period32BitSet_In32BitRegister (TMR_MODULE_ID index , uint32_t period )
{
    tmr_registers_t volatile * tmrX = ((tmr_registers_t *)(index));

	if (TMR_ID_1 != index)
	{
        tmrX->PRx = period;
	}
	else
	{
		PLIB_ASSERT(false, "Timer1 instance does not support PLIB_TMR_Period32BitSet");
	}
}


//******************************************************************************
/* Function :  TMR_Period32BitGet_In32BitRegister

  Summary:
    Implements In32BitRegister variant of PLIB_TMR_Period32BitGet 

  Description:
    This template implements the In32BitRegister variant of the PLIB_TMR_Period32BitGet function.
*/

PLIB_TEMPLATE uint32_t TMR_Period32BitGet_In32BitRegister( TMR_MODULE_ID index )
{
    tmr_registers_t volatile * tmrX = ((tmr_registers_t *)(index));
    uint32_t val1 = 0u;

	if (TMR_ID_1 != index)
	{
    	val1 = tmrX->PRx;
	}
	else
	{
		PLIB_ASSERT(false, "Timer1 instance does not support PLIB_TMR_Period32BitGet");
	}	
	return val1;
}


//******************************************************************************
/* Function :  TMR_ExistsPeriod32Bit_In32BitRegister

  Summary:
    Implements In32BitRegister variant of PLIB_TMR_ExistsPeriod32Bit

  Description:
    This template implements the In32BitRegister variant of the PLIB_TMR_ExistsPeriod32Bit function.
*/

#define PLIB_TMR_ExistsPeriod32Bit PLIB_TMR_ExistsPeriod32Bit
PLIB_TEMPLATE bool TMR_ExistsPeriod32Bit_In32BitRegister( TMR_MODULE_ID index )
{
	bool var1;
    if (TMR_ID_1 == index)
    {
        PLIB_ASSERT(false, "Timer1 instance does not support PLIB_TMR_Period32BitSet");
		var1 = false;
    }
    else
    {
        var1 = true;
    }
	return var1;
}


#endif /*_TMR_PERIOD32BIT_IN32BITREGISTER_H*/

/******************************************************************************
 End of File
*/

