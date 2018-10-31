/*******************************************************************************
  USART Peripheral Library Template Implementation

  File Name:
    usart_ReceiverInterruptMode_Unsupported.h

  Summary:
    USART PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : ReceiverInterruptMode
    and its Variant : Unsupported
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

#ifndef _USART_RECEIVERINTERRUPTMODE_UNSUPPORTED_H
#define _USART_RECEIVERINTERRUPTMODE_UNSUPPORTED_H


//******************************************************************************
/* Function :  USART_ExistsReceiverInterruptMode_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_USART_ExistsReceiverInterruptMode

  Description:
    This template implements the Unsupported variant of the PLIB_USART_ExistsReceiverInterruptMode function.
*/

PLIB_TEMPLATE bool USART_ExistsReceiverInterruptMode_Unsupported( USART_MODULE_ID index )
{
    return false;
}


//******************************************************************************
/* Function :  USART_ReceiverInterruptModeSelect_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_USART_ReceiverInterruptModeSelect 

  Description:
    This template implements the Unsupported variant of the PLIB_USART_ReceiverInterruptModeSelect function.
*/

PLIB_TEMPLATE void USART_ReceiverInterruptModeSelect_Unsupported( USART_MODULE_ID index , USART_RECEIVE_INTR_MODE interruptMode )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_USART_ReceiverInterruptModeSelect");
}


//******************************************************************************
/* Function :  USART_InitializeOperation_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_USART_InitializeOperation 

  Description:
    This template implements the Unsupported variant of the PLIB_USART_InitializeOperation function.
*/

PLIB_TEMPLATE void USART_InitializeOperation_Unsupported( USART_MODULE_ID index , USART_RECEIVE_INTR_MODE receiveInterruptMode ,
        USART_TRANSMIT_INTR_MODE transmitInterruptMode, USART_OPERATION_MODE operationMode)
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_USART_InitializeOperation");
}


#endif /*_USART_RECEIVERINTERRUPTMODE_UNSUPPORTED_H*/

/******************************************************************************
 End of File
*/

