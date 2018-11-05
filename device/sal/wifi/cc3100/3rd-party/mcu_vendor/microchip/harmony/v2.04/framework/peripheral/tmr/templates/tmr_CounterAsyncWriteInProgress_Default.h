/*******************************************************************************
  TMR Peripheral Library Template Implementation

  File Name:
    tmr_CounterAsyncWriteInProgress_Default.h

  Summary:
    TMR PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : CounterAsyncWriteInProgress
    and its Variant : Default
    For following APIs :
        PLIB_TMR_CounterAsyncWriteInProgress
        PLIB_TMR_ExistsCounterAsyncWriteInProgress

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

#ifndef _TMR_COUNTERASYNCWRITEINPROGRESS_DEFAULT_H
#define _TMR_COUNTERASYNCWRITEINPROGRESS_DEFAULT_H

#include "tmr_registers.h"

//******************************************************************************
/* Function :  TMR_CounterAsyncWriteInProgress_Default

  Summary:
    Implements Default variant of PLIB_TMR_CounterAsyncWriteInProgress

  Description:
    This template implements the Default variant of the PLIB_TMR_CounterAsyncWriteInProgress function.
*/

PLIB_TEMPLATE bool TMR_CounterAsyncWriteInProgress_Default( TMR_MODULE_ID index )
{
    tmr1_registers_t volatile * tmr = ((tmr1_registers_t *)(index));
    
    if(TMR_ID_1 == index)
    {
        return (bool)(tmr->TxCON.TWIP); 
    }
    else
    {
        PLIB_ASSERT(false, "This Timer instance does not support PLIB_TMR_CounterAsyncWriteInProgress");
        return false;
    }
}


//******************************************************************************
/* Function :  TMR_ExistsCounterAsyncWriteInProgress_Default

  Summary:
    Implements Default variant of PLIB_TMR_ExistsCounterAsyncWriteInProgress

  Description:
    This template implements the Default variant of the PLIB_TMR_ExistsCounterAsyncWriteInProgress function.
*/

#define PLIB_TMR_ExistsCounterAsyncWriteInProgress PLIB_TMR_ExistsCounterAsyncWriteInProgress
PLIB_TEMPLATE bool TMR_ExistsCounterAsyncWriteInProgress_Default( TMR_MODULE_ID index )
{
    if(TMR_ID_1 == index)
    {
        return true; 
    }
    else
    {
        PLIB_ASSERT(false, "This Timer instance does not support CounterAsyncWriteInProgress feature");
        return false;
    }   
}


#endif /*_TMR_COUNTERASYNCWRITEINPROGRESS_DEFAULT_H*/

/******************************************************************************
 End of File
*/

