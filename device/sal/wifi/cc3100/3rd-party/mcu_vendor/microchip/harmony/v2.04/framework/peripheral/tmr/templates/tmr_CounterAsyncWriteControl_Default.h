/*******************************************************************************
  TMR Peripheral Library Template Implementation

  File Name:
    tmr_CounterAsyncWriteControl_Default.h

  Summary:
    TMR PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : CounterAsyncWriteControl
    and its Variant : Default
    For following APIs :
        PLIB_TMR_CounterAsyncWriteEnable
        PLIB_TMR_CounterAsyncWriteDisable
        PLIB_TMR_ExistsCounterAsyncWriteControl

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

#ifndef _TMR_COUNTERASYNCWRITECONTROL_DEFAULT_H
#define _TMR_COUNTERASYNCWRITECONTROL_DEFAULT_H

#include "tmr_registers.h"

//******************************************************************************
/* Function :  TMR_CounterAsyncWriteEnable_Default

  Summary:
    Implements Default variant of PLIB_TMR_CounterAsyncWriteEnable

  Description:
    This template implements the Default variant of the PLIB_TMR_CounterAsyncWriteEnable function.
*/

PLIB_TEMPLATE void TMR_CounterAsyncWriteEnable_Default( TMR_MODULE_ID index )
{
    tmr1_registers_t volatile * tmr = ((tmr1_registers_t *)(index));
    
    if(TMR_ID_1 == index)
    {
        tmr->TxCONCLR = TxCON_TWDIS_MASK; 
    }
    else
    {
        PLIB_ASSERT(false, "This Timer instance does not support PLIB_TMR_CounterAsyncWriteEnable");
    }
}


//******************************************************************************
/* Function :  TMR_CounterAsyncWriteDisable_Default

  Summary:
    Implements Default variant of PLIB_TMR_CounterAsyncWriteDisable

  Description:
    This template implements the Default variant of the PLIB_TMR_CounterAsyncWriteDisable function.
*/

PLIB_TEMPLATE void TMR_CounterAsyncWriteDisable_Default( TMR_MODULE_ID index )
{
    tmr1_registers_t volatile * tmr = ((tmr1_registers_t *)(index));
    
    if(TMR_ID_1 == index)
    {
        tmr->TxCONSET = TxCON_TWDIS_MASK;
    }
    else
    {
        PLIB_ASSERT(false, "This Timer instance does not support PLIB_TMR_CounterAsyncWriteDisable");
    }
}


//******************************************************************************
/* Function :  TMR_ExistsCounterAsyncWriteControl_Default

  Summary:
    Implements Default variant of PLIB_TMR_ExistsCounterAsyncWriteControl

  Description:
    This template implements the Default variant of the PLIB_TMR_ExistsCounterAsyncWriteControl function.
*/

#define PLIB_TMR_ExistsCounterAsyncWriteControl PLIB_TMR_ExistsCounterAsyncWriteControl
PLIB_TEMPLATE bool TMR_ExistsCounterAsyncWriteControl_Default( TMR_MODULE_ID index )
{
    if(TMR_ID_1 == index)
    {
        return true;
    }
    else
    {
        PLIB_ASSERT(false, "This Timer instance does not support CounterAsyncWrite feature");
        return false;
    }
}


#endif /*_TMR_COUNTERASYNCWRITECONTROL_DEFAULT_H*/

/******************************************************************************
 End of File
*/

