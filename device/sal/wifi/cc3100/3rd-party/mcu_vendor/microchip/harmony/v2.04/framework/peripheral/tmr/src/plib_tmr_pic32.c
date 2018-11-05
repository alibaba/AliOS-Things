/*******************************************************************************
  TMR PLIB PIC32 Specific interfaces

  Company:
    Microchip Technology Inc.

  File Name:
    plib_tmr_pic32.c

  Summary:
    TMR PLIB PIC32 Specific definitions

  Description:
    TMR PLIB PIC32 Specific definitions
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

SOFTWARE AND DOCUMENTATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY KIND,
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

// *****************************************************************************
// *****************************************************************************
// Section: Includes
// *****************************************************************************
// *****************************************************************************


#include "peripheral/tmr/plib_tmr.h"

// *****************************************************************************
// *****************************************************************************
// Section: Symbolic Constant Definitions
// *****************************************************************************
// *****************************************************************************


// *****************************************************************************
// *****************************************************************************
// Section: File Scope Variables
// *****************************************************************************
// *****************************************************************************

typedef struct
{
    TMR_PRESCALE prescale;
    uint16_t prescale_divisor;

} _TMR_DIVISOR_VALUE;

//#warning "This table needs to be updated"
static const _TMR_DIVISOR_VALUE s_prescalerValues[] =
{
    {TMR_PRESCALE_VALUE_1,       1},
    {TMR_PRESCALE_VALUE_8,       8},
    {TMR_PRESCALE_VALUE_64,      64},
    {TMR_PRESCALE_VALUE_256,     256},
    {TMR_PRESCALE_VALUE_1,       1},
    {TMR_PRESCALE_VALUE_2,       2},
    {TMR_PRESCALE_VALUE_4,       4},
    {TMR_PRESCALE_VALUE_8,       8},
    {TMR_PRESCALE_VALUE_16,      16},
    {TMR_PRESCALE_VALUE_32,      32},
    {TMR_PRESCALE_VALUE_64,      64},
    {TMR_PRESCALE_VALUE_256,     256}
};


// *****************************************************************************
// *****************************************************************************
// Section: File Scope Functions
// *****************************************************************************
// *****************************************************************************


// *****************************************************************************
// *****************************************************************************
// Section: Global Scope Function Definitions
// *****************************************************************************
// *****************************************************************************


// *****************************************************************************
/* uint16_t PLIB_TMR_PrescaleDivisorGet(TMR_MODULE_ID index, TMR_PRESCALE prescaler)

   Summary:
    Gets the prescaler divisor information

   Description:
    Gets the prescaler divisor information

   Remarks:
    None
 */

uint16_t PLIB_TMR_PrescaleDivisorGet(TMR_MODULE_ID index, TMR_PRESCALE prescaler)
{
    uint8_t i, arraySize = sizeof(s_prescalerValues)/sizeof(_TMR_DIVISOR_VALUE);

    /* Start from zero if it is type A timer */
    i = 0;

	if (prescaler >= arraySize)
	{
		prescaler %=arraySize;
	}
    /* Re-check this condition. This is considering convensionally Timer 1 is
    type A timer */
    if(index != TMR_ID_1)
    {
            i=4;
    }
    for(; i<arraySize; i++)
    {
        if((prescaler == s_prescalerValues[i].prescale))
        {
            return s_prescalerValues[i].prescale_divisor;
        }
    }

    PLIB_ASSERT(false, "Should never come here");

    return 0;
} /* PLIB_TMR_PrescalerGet */


/*******************************************************************************
End of File
 */

