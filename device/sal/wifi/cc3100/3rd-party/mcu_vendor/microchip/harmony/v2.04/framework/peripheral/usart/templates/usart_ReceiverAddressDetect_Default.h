/*******************************************************************************
  USART Peripheral Library Template Implementation

  File Name:
    usart_ReceiverAddressDetect_Default.h

  Summary:
    USART PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : ReceiverAddressDetect
    and its Variant : Default
    For following APIs :
        PLIB_USART_ExistsReceiverAddressDetect
        PLIB_USART_ReceiverAddressDetectEnable
        PLIB_USART_ReceiverAddressDetectDisable
        PLIB_USART_ReceiverAddressIsReceived

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

#ifndef _USART_RECEIVERADDRESSDETECT_DEFAULT_H
#define _USART_RECEIVERADDRESSDETECT_DEFAULT_H

#include "usart_registers.h"

//******************************************************************************
/* Function :  USART_ExistsReceiverAddressDetect_Default

  Summary:
    Implements Default variant of PLIB_USART_ExistsReceiverAddressDetect

  Description:
    This template implements the Default variant of the PLIB_USART_ExistsReceiverAddressDetect function.
*/

#define PLIB_USART_ExistsReceiverAddressDetect PLIB_USART_ExistsReceiverAddressDetect
PLIB_TEMPLATE bool USART_ExistsReceiverAddressDetect_Default( USART_MODULE_ID index )
{
    return true;
}


//******************************************************************************
/* Function :  USART_ReceiverAddressDetectEnable_Default

  Summary:
    Implements Default variant of PLIB_USART_ReceiverAddressDetectEnable

  Description:
    This template implements the Default variant of the PLIB_USART_ReceiverAddressDetectEnable function.
*/

PLIB_TEMPLATE void USART_ReceiverAddressDetectEnable_Default( USART_MODULE_ID index )
{
    usart_registers_t volatile * usart = ((usart_registers_t *)(index));
    usart->UxSTASET = UxSTA_ADDEN_MASK;
}


//******************************************************************************
/* Function :  USART_ReceiverAddressDetectDisable_Default

  Summary:
    Implements Default variant of PLIB_USART_ReceiverAddressDetectDisable

  Description:
    This template implements the Default variant of the PLIB_USART_ReceiverAddressDetectDisable function.
*/

PLIB_TEMPLATE void USART_ReceiverAddressDetectDisable_Default( USART_MODULE_ID index )
{
    usart_registers_t volatile * usart = ((usart_registers_t *)(index));
    usart->UxSTACLR = UxSTA_ADDEN_MASK;
}


//******************************************************************************
/* Function :  USART_ReceiverAddressIsReceived_Default

  Summary:
    Implements Default variant of PLIB_USART_ReceiverAddressIsReceived

  Description:
    This template implements the Default variant of the PLIB_USART_ReceiverAddressIsReceived function.
*/

PLIB_TEMPLATE bool USART_ReceiverAddressIsReceived_Default( USART_MODULE_ID index )
{
    usart_registers_t volatile * usart = ((usart_registers_t *)(index));
    return (bool)usart->UxSTA.ADDEN;
}


#endif /*_USART_RECEIVERADDRESSDETECT_DEFAULT_H*/

/******************************************************************************
 End of File
*/

