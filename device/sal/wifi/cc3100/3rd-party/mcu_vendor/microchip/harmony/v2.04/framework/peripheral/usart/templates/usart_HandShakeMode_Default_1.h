/*******************************************************************************
  USART Peripheral Library Template Implementation

  File Name:
    usart_HandShakeMode_Default_1.h

  Summary:
    USART PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : HandShakeMode
    and its Variant : Default_1
    For following APIs :
        PLIB_USART_ExistsHandshakeMode
        PLIB_USART_HandshakeModeSelect

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

#ifndef _USART_HANDSHAKEMODE_DEFAULT_1_H
#define _USART_HANDSHAKEMODE_DEFAULT_1_H

#include "usart_registers.h"

//******************************************************************************
/* Function :  USART_ExistsHandshakeMode_Default_1

  Summary:
    Implements Default_1 variant of PLIB_USART_ExistsHandshakeMode

  Description:
    This template implements the Default_1 variant of the PLIB_USART_ExistsHandshakeMode function.
*/

PLIB_TEMPLATE bool USART_ExistsHandshakeMode_Default_1( USART_MODULE_ID index )
{
    /* 5XX/6XX/7XX devices have instance specific difference for this feature so mapping accordingly */
    if((USART_ID_1 == index) || (USART_ID_2 == index) || (USART_ID_3 == index))
    {
        return true;
    }
    else
    {
        return false;
    }
}


//******************************************************************************
/* Function :  USART_HandshakeModeSelect_Default_1

  Summary:
    Implements Default_1 variant of PLIB_USART_HandshakeModeSelect 

  Description:
    This template implements the Default_1 variant of the PLIB_USART_HandshakeModeSelect function.
*/

PLIB_TEMPLATE void USART_HandshakeModeSelect_Default_1( USART_MODULE_ID index , USART_HANDSHAKE_MODE handshakeConfig )
{
    /* 5XX/6XX/7XX devices have instance specific difference for this feature so mapping accordingly */
    if((USART_ID_1 == index) || (USART_ID_2 == index) || (USART_ID_3 == index))
    {
        usart_registers_t volatile * usart = ((usart_registers_t *)(index));

        usart->UxMODE.RTSMD = handshakeConfig;
    }
    else
    {
        PLIB_ASSERT(false, "The device selected does not implement PLIB_USART_HandshakeModeSelect");
    }
}


#endif /*_USART_HANDSHAKEMODE_DEFAULT_1_H*/

/******************************************************************************
 End of File
*/

