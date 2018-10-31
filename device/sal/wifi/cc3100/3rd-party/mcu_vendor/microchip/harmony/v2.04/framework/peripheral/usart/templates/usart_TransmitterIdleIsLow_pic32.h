/*******************************************************************************
  USART Peripheral Library Template Implementation

  File Name:
    usart_TransmitterIdleIsLow_pic32.h

  Summary:
    USART PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : TransmitterIdleIsLow
    and its Variant : pic32
    For following APIs :
        PLIB_USART_ExistsTransmitterIdleIsLow
        PLIB_USART_TransmitterIdleIsLowDisable
        PLIB_USART_TransmitterIdleIsLowEnable

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

#ifndef _USART_TRANSMITTERIDLEISLOW_PIC32_H
#define _USART_TRANSMITTERIDLEISLOW_PIC32_H

#include "usart_registers.h"

//******************************************************************************
/* Function :  USART_ExistsTransmitterIdleIsLow_pic32

  Summary:
    Implements pic32 variant of PLIB_USART_ExistsTransmitterIdleIsLow

  Description:
    This template implements the pic32 variant of the PLIB_USART_ExistsTransmitterIdleIsLow function.
*/

#define PLIB_USART_ExistsTransmitterIdleIsLow PLIB_USART_ExistsTransmitterIdleIsLow
PLIB_TEMPLATE bool USART_ExistsTransmitterIdleIsLow_pic32( USART_MODULE_ID index )
{
    return true;
}


//******************************************************************************
/* Function :  USART_TransmitterIdleIsLowDisable_pic32

  Summary:
    Implements pic32 variant of PLIB_USART_TransmitterIdleIsLowDisable

  Description:
    This template implements the pic32 variant of the PLIB_USART_TransmitterIdleIsLowDisable function.
*/

PLIB_TEMPLATE void USART_TransmitterIdleIsLowDisable_pic32( USART_MODULE_ID index )
{
    usart_registers_t volatile * usart = ((usart_registers_t *)(index));

    if (1 == usart->UxMODE.IREN)
    {
        usart->UxSTASET = UxSTA_UTXINV_MASK;
    }
    else
    {
        usart->UxSTACLR = UxSTA_UTXINV_MASK;
    }
}


//******************************************************************************
/* Function :  USART_TransmitterIdleIsLowEnable_pic32

  Summary:
    Implements pic32 variant of PLIB_USART_TransmitterIdleIsLowEnable

  Description:
    This template implements the pic32 variant of the PLIB_USART_TransmitterIdleIsLowEnable function.
*/

PLIB_TEMPLATE void USART_TransmitterIdleIsLowEnable_pic32( USART_MODULE_ID index )
{
    usart_registers_t * usart = ((usart_registers_t *)(index));

    if (1 == usart->UxMODE.IREN)
    {
        usart->UxSTACLR = UxSTA_UTXINV_MASK;
    }
    else
    {
        usart->UxSTASET = UxSTA_UTXINV_MASK;
    }
}


#endif /*_USART_TRANSMITTERIDLEISLOW_PIC32_H*/

/******************************************************************************
 End of File
*/

