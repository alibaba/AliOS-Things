/*******************************************************************************
  USART Peripheral Library Template Implementation

  File Name:
    usart_Transmitter9Bits_Unsupported.h

  Summary:
    USART PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : Transmitter9Bits
    and its Variant : Unsupported
    For following APIs :
        PLIB_USART_ExistsTransmitter9BitsSend
        PLIB_USART_Transmitter9BitsSend

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

#ifndef _USART_TRANSMITTER9BITS_UNSUPPORTED_H
#define _USART_TRANSMITTER9BITS_UNSUPPORTED_H


//******************************************************************************
/* Function :  USART_ExistsTransmitter9BitsSend_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_USART_ExistsTransmitter9BitsSend

  Description:
    This template implements the Unsupported variant of the PLIB_USART_ExistsTransmitter9BitsSend function.
*/

PLIB_TEMPLATE bool USART_ExistsTransmitter9BitsSend_Unsupported( USART_MODULE_ID index )
{
    return false;
}


//******************************************************************************
/* Function :  USART_Transmitter9BitsSend_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_USART_Transmitter9BitsSend 

  Description:
    This template implements the Unsupported variant of the PLIB_USART_Transmitter9BitsSend function.
*/

PLIB_TEMPLATE void USART_Transmitter9BitsSend_Unsupported( USART_MODULE_ID index , int8_t data , bool Bit9th )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_USART_Transmitter9BitsSend");
}


#endif /*_USART_TRANSMITTER9BITS_UNSUPPORTED_H*/

/******************************************************************************
 End of File
*/

