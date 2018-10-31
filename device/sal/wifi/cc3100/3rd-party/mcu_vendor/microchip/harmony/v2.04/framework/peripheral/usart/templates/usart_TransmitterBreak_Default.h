/*******************************************************************************
  USART Peripheral Library Template Implementation

  File Name:
    usart_TransmitterBreak_Default.h

  Summary:
    USART PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : TransmitterBreak
    and its Variant : Default
    For following APIs :
        PLIB_USART_ExistsTransmitterBreak
        PLIB_USART_TransmitterBreakSend
        PLIB_USART_TransmitterBreakSendIsComplete

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

#ifndef _USART_TRANSMITTERBREAK_DEFAULT_H
#define _USART_TRANSMITTERBREAK_DEFAULT_H

#include "usart_registers.h"

//******************************************************************************
/* Function :  USART_ExistsTransmitterBreak_Default

  Summary:
    Implements Default variant of PLIB_USART_ExistsTransmitterBreak

  Description:
    This template implements the Default variant of the PLIB_USART_ExistsTransmitterBreak function.
*/

#define PLIB_USART_ExistsTransmitterBreak PLIB_USART_ExistsTransmitterBreak
PLIB_TEMPLATE bool USART_ExistsTransmitterBreak_Default( USART_MODULE_ID index )
{
    return true;
}


//******************************************************************************
/* Function :  USART_TransmitterBreakSend_Default

  Summary:
    Implements Default variant of PLIB_USART_TransmitterBreakSend 

  Description:
    This template implements the Default variant of the PLIB_USART_TransmitterBreakSend function.
*/

PLIB_TEMPLATE void USART_TransmitterBreakSend_Default( USART_MODULE_ID index )
{
    usart_registers_t volatile * usart = ((usart_registers_t *)(index));
    usart->UxSTASET = UxSTA_UTXBRK_MASK;
}


//******************************************************************************
/* Function :  USART_TransmitterBreakSendIsComplete_Default

  Summary:
    Implements Default variant of PLIB_USART_TransmitterBreakSendIsComplete 

  Description:
    This template implements the Default variant of the PLIB_USART_TransmitterBreakSendIsComplete function.
*/

PLIB_TEMPLATE bool USART_TransmitterBreakSendIsComplete_Default( USART_MODULE_ID index )
{
    usart_registers_t volatile * usart = ((usart_registers_t *)(index));
    return (bool)usart->UxSTA.UTXBRK;
}


#endif /*_USART_TRANSMITTERBREAK_DEFAULT_H*/

/******************************************************************************
 End of File
*/

