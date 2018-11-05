/*******************************************************************************
  USART Peripheral Library Template Implementation

  File Name:
    usart_ReceiverInterruptMode_Default.h

  Summary:
    USART PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : ReceiverInterruptMode
    and its Variant : Default
    For following APIs :
        PLIB_USART_ExistsReceiverInterruptMode
        PLIB_USART_ReceiverInterruptModeSelect
        PLIB_USART_InitializeOperation

*******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2013-2014 released Microchip Technology Inc.  All rights reserved.

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

#ifndef _USART_RECEIVERINTERRUPTMODE_DEFAULT_H
#define _USART_RECEIVERINTERRUPTMODE_DEFAULT_H

#include "usart_registers.h"

//******************************************************************************
/* Function :  USART_ExistsReceiverInterruptMode_Default

  Summary:
    Implements Default variant of PLIB_USART_ExistsReceiverInterruptMode

  Description:
    This template implements the Default variant of the PLIB_USART_ExistsReceiverInterruptMode function.
*/

#define PLIB_USART_ExistsReceiverInterruptMode PLIB_USART_ExistsReceiverInterruptMode
PLIB_TEMPLATE bool USART_ExistsReceiverInterruptMode_Default( USART_MODULE_ID index )
{
    return true;
}


//******************************************************************************
/* Function :  USART_ReceiverInterruptModeSelect_Default

  Summary:
    Implements Default variant of PLIB_USART_ReceiverInterruptModeSelect 

  Description:
    This template implements the Default variant of the PLIB_USART_ReceiverInterruptModeSelect function.
*/

PLIB_TEMPLATE void USART_ReceiverInterruptModeSelect_Default( USART_MODULE_ID index , USART_RECEIVE_INTR_MODE interruptMode )
{
    usart_registers_t volatile * usart = ((usart_registers_t *)(index));

    /* Clear and Set, as UxSTA contains status bits and 
       hence need to be accessed atomically.
       Using bit field access may erroneously cause status bits to get cleared */
    usart->UxSTACLR = UxSTA_URXISEL_MASK;
    usart->UxSTASET = ( UxSTA_URXISEL_MASK & ((uint32_t)interruptMode << UxSTA_URXISEL_POSITION) );
}


//******************************************************************************
/* Function :  USART_InitializeOperation_Default

  Summary:
    Implements Default variant of PLIB_USART_InitializeOperation 

  Description:
    This template implements the Default variant of the PLIB_USART_InitializeOperation function.
*/

PLIB_TEMPLATE void USART_InitializeOperation_Default( USART_MODULE_ID index , USART_RECEIVE_INTR_MODE receiveInterruptMode ,
        USART_TRANSMIT_INTR_MODE transmitInterruptMode, USART_OPERATION_MODE operationMode)
{
    usart_registers_t volatile * usart = ((usart_registers_t *)(index));

    /* Clear and Set, as UxSTA contains status bits and 
       hence need to be accessed atomically.
       Using bit field access may erroneously cause status bits to get cleared */
    usart->UxSTACLR = UxSTA_URXISEL_MASK;
    usart->UxSTASET = ( UxSTA_URXISEL_MASK & ((uint32_t)receiveInterruptMode << UxSTA_URXISEL_POSITION) );

    /* Clear and Set, as UxSTA contains status bits and 
       hence need to be accessed atomically.
       Using bit field access may erroneously cause status bits to get cleared */
    usart->UxSTACLR = UxSTA_UTXISEL_MASK;
    usart->UxSTASET = ( UxSTA_UTXISEL_MASK & ((uint32_t)transmitInterruptMode << UxSTA_UTXISEL_POSITION) );

    /* Select the operation mode */
    usart->UxMODE.UEN = operationMode;

}


#endif /*_USART_RECEIVERINTERRUPTMODE_DEFAULT_H*/

/******************************************************************************
 End of File
*/

