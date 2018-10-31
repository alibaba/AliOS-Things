/*******************************************************************************
  USART Peripheral Library Template Implementation

  File Name:
    usart_ReceiverAddress_Default.h

  Summary:
    USART PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : ReceiverAddress
    and its Variant : Default
    For following APIs :
        PLIB_USART_ExistsReceiverAddress
        PLIB_USART_AddressSet
        PLIB_USART_AddressGet

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

#ifndef _USART_RECEIVERADDRESS_DEFAULT_H
#define _USART_RECEIVERADDRESS_DEFAULT_H

#include "usart_registers.h"

//******************************************************************************
/* Function :  USART_ExistsReceiverAddress_Default

  Summary:
    Implements Default variant of PLIB_USART_ExistsReceiverAddress

  Description:
    This template implements the Default variant of the PLIB_USART_ExistsReceiverAddress function.
*/

#define PLIB_USART_ExistsReceiverAddress PLIB_USART_ExistsReceiverAddress
PLIB_TEMPLATE bool USART_ExistsReceiverAddress_Default( USART_MODULE_ID index )
{
    return true;
}


//******************************************************************************
/* Function :  USART_AddressSet_Default

  Summary:
    Implements Default variant of PLIB_USART_AddressSet 

  Description:
    This template implements the Default variant of the PLIB_USART_AddressSet function.
*/

PLIB_TEMPLATE void USART_AddressSet_Default( USART_MODULE_ID index , uint8_t address )
{
    usart_registers_t volatile * usart = ((usart_registers_t *)(index));
    
    /* Clear and Set, as UxSTA contains status bits and 
       hence need to be accessed atomically.
       Using bit field access may erroneously cause status bits to get cleared */
    usart->UxSTACLR = UxSTA_ADDR_MASK;
    usart->UxSTASET = ( UxSTA_ADDR_MASK & (((uint32_t)address) << UxSTA_ADDR_POSITION) );
}


//******************************************************************************
/* Function :  USART_AddressGet_Default

  Summary:
    Implements Default variant of PLIB_USART_AddressGet 

  Description:
    This template implements the Default variant of the PLIB_USART_AddressGet function.
*/

PLIB_TEMPLATE uint8_t USART_AddressGet_Default( USART_MODULE_ID index )
{
    usart_registers_t volatile * usart = ((usart_registers_t *)(index));
    return (uint8_t)usart->UxSTA.ADDR;
}


#endif /*_USART_RECEIVERADDRESS_DEFAULT_H*/

/******************************************************************************
 End of File
*/

