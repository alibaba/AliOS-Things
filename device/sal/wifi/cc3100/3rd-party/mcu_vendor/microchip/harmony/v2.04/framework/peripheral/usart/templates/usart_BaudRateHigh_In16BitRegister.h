/*******************************************************************************
  USART Peripheral Library Template Implementation

  File Name:
    usart_BaudRateHigh_In16BitRegister.h

  Summary:
    USART PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : BaudRateHigh
    and its Variant : In16BitRegister
    For following APIs :
        PLIB_USART_ExistsBaudRateHigh
        PLIB_USART_BaudRateHighSet
        PLIB_USART_BaudRateHighDisable
        PLIB_USART_BaudRateHighEnable

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

#ifndef _USART_BAUDRATEHIGH_IN16BITREGISTER_H
#define _USART_BAUDRATEHIGH_IN16BITREGISTER_H

#include "usart_registers.h"

//******************************************************************************
/* Function :  USART_ExistsBaudRateHigh_In16BitRegister

  Summary:
    Implements In16BitRegister variant of PLIB_USART_ExistsBaudRateHigh

  Description:
    This template implements the In16BitRegister variant of the PLIB_USART_ExistsBaudRateHigh function.
*/

#define PLIB_USART_ExistsBaudRateHigh PLIB_USART_ExistsBaudRateHigh
PLIB_TEMPLATE bool USART_ExistsBaudRateHigh_In16BitRegister( USART_MODULE_ID index )
{
    return true;
}


//******************************************************************************
/* Function :  USART_BaudRateHighSet_In16BitRegister

  Summary:
    Implements In16BitRegister variant of PLIB_USART_BaudRateHighSet 

  Description:
    This template implements the In16BitRegister variant of the PLIB_USART_BaudRateHighSet function.
*/

PLIB_TEMPLATE void USART_BaudRateHighSet_In16BitRegister( USART_MODULE_ID index , uint32_t clockFrequency , uint32_t baudRate )
{
    usart_registers_t volatile * usart = ((usart_registers_t *)(index));
    uint32_t brg = 0;
    
    brg = ( ((clockFrequency >> 2) + (baudRate >> 1)) / baudRate ) - 1; 
    usart->UxBRG = brg;
}


//******************************************************************************
/* Function :  USART_BaudRateHighDisable_In16BitRegister

  Summary:
    Implements In16BitRegister variant of PLIB_USART_BaudRateHighDisable 

  Description:
    This template implements the In16BitRegister variant of the PLIB_USART_BaudRateHighDisable function.
*/

PLIB_TEMPLATE void USART_BaudRateHighDisable_In16BitRegister( USART_MODULE_ID index )
{
    usart_registers_t volatile * usart = ((usart_registers_t *)(index));
    usart->UxMODECLR = UxMODE_BRGH_MASK;
}


//******************************************************************************
/* Function :  USART_BaudRateHighEnable_In16BitRegister

  Summary:
    Implements In16BitRegister variant of PLIB_USART_BaudRateHighEnable 

  Description:
    This template implements the In16BitRegister variant of the PLIB_USART_BaudRateHighEnable function.
*/

PLIB_TEMPLATE void USART_BaudRateHighEnable_In16BitRegister( USART_MODULE_ID index )
{
    usart_registers_t volatile * usart = ((usart_registers_t *)(index));
    usart->UxMODESET = UxMODE_BRGH_MASK;
}


#endif /*_USART_BAUDRATEHIGH_IN16BITREGISTER_H*/

/******************************************************************************
 End of File
*/

