/*******************************************************************************
  TMR Peripheral Library Template Implementation

  File Name:
    tmr_Prescale_Default.h

  Summary:
    TMR PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : Prescale
    and its Variant : Default
    For following APIs :
        PLIB_TMR_PrescaleSelect
        PLIB_TMR_PrescaleGet
        PLIB_TMR_ExistsPrescale

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

#ifndef _TMR_PRESCALE_DEFAULT_H
#define _TMR_PRESCALE_DEFAULT_H

#include "tmr_registers.h"

//******************************************************************************
/* Function :  TMR_PrescaleSelect_Default

  Summary:
    Implements Default variant of PLIB_TMR_PrescaleSelect

  Description:
    This template implements the Default variant of the PLIB_TMR_PrescaleSelect function.
*/

PLIB_TEMPLATE void TMR_PrescaleSelect_Default( TMR_MODULE_ID index , TMR_PRESCALE prescale )
{
    tmr_registers_t volatile * tmr = ((tmr_registers_t *)(index));
    
    if(TMR_ID_1 == index)
    {
        switch(prescale)
        {
            case TMR_PRESCALE_VALUE_1:  tmr->TxCON.TCKPS = 0;
                                        break;
            case TMR_PRESCALE_VALUE_8:  tmr->TxCON.TCKPS = 1;
                                        break;
            case TMR_PRESCALE_VALUE_64: tmr->TxCON.TCKPS = 2;
                                        break;
            case TMR_PRESCALE_VALUE_256: tmr->TxCON.TCKPS = 3;
                                        break;
            default: PLIB_ASSERT(false, " Given Prescale value is not supported for Timer1");
        }
    }
    else
    {
        tmr->TxCON.TCKPS = prescale;
    }
}


//******************************************************************************
/* Function :  TMR_PrescaleGet_Default

  Summary:
    Implements Default variant of PLIB_TMR_PrescaleGet

  Description:
    This template implements the Default variant of the PLIB_TMR_PrescaleGet function.
*/

PLIB_TEMPLATE uint16_t TMR_PrescaleGet_Default( TMR_MODULE_ID index )
{
    tmr_registers_t volatile * tmr = ((tmr_registers_t *)(index));
    uint8_t temp_prescaler;
    uint16_t preScaler;

    temp_prescaler = tmr->TxCON.TCKPS;
                          
    if(TMR_ID_1 == index)
    {
        /* 1, 8, 64, 256 */
        switch(temp_prescaler)
        {
            case 0x00u: preScaler = 1u;
                        break;
            case 0x01u: preScaler = 8u;
                        break;
            case 0x02u: preScaler = 64u;
                        break;
            case 0x03u: preScaler = 256u;
                        break;
            default:    preScaler = 0u;
        }
    }
    else
    {
        /* 1, 2, 4, 8, 16, 32, 64, 256 */
        if ( temp_prescaler == 7u )
        {
            temp_prescaler++;
        }
        preScaler = (uint16_t)(0x01u << temp_prescaler);
    }
    return preScaler;

}


//******************************************************************************
/* Function :  TMR_ExistsPrescale_Default

  Summary:
    Implements Default variant of PLIB_TMR_ExistsPrescale

  Description:
    This template implements the Default variant of the PLIB_TMR_ExistsPrescale function.
*/

#define PLIB_TMR_ExistsPrescale PLIB_TMR_ExistsPrescale
PLIB_TEMPLATE bool TMR_ExistsPrescale_Default( TMR_MODULE_ID index )
{
    return true;
}


#endif /*_TMR_PRESCALE_DEFAULT_H*/

/******************************************************************************
 End of File
*/

