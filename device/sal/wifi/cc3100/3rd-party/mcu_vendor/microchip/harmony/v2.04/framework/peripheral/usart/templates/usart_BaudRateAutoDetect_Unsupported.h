/*******************************************************************************
  USART Peripheral Library Template Implementation

  File Name:
    usart_BaudRateAutoDetect_Unsupported.h

  Summary:
    USART PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : BaudRateAutoDetect
    and its Variant : Unsupported
    For following APIs :
        PLIB_USART_ExistsBaudRateAutoDetect
        PLIB_USART_BaudRateAutoDetectEnable
        PLIB_USART_BaudRateAutoDetectIsComplete

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

#ifndef _USART_BAUDRATEAUTODETECT_UNSUPPORTED_H
#define _USART_BAUDRATEAUTODETECT_UNSUPPORTED_H


//******************************************************************************
/* Function :  USART_ExistsBaudRateAutoDetect_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_USART_ExistsBaudRateAutoDetect

  Description:
    This template implements the Unsupported variant of the PLIB_USART_ExistsBaudRateAutoDetect function.
*/

PLIB_TEMPLATE bool USART_ExistsBaudRateAutoDetect_Unsupported( USART_MODULE_ID index )
{
    return false;
}


//******************************************************************************
/* Function :  USART_BaudRateAutoDetectEnable_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_USART_BaudRateAutoDetectEnable 

  Description:
    This template implements the Unsupported variant of the PLIB_USART_BaudRateAutoDetectEnable function.
*/

PLIB_TEMPLATE void USART_BaudRateAutoDetectEnable_Unsupported( USART_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_USART_BaudRateAutoDetectEnable");
}


//******************************************************************************
/* Function :  USART_BaudRateAutoDetectIsComplete_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_USART_BaudRateAutoDetectIsComplete 

  Description:
    This template implements the Unsupported variant of the PLIB_USART_BaudRateAutoDetectIsComplete function.
*/

PLIB_TEMPLATE bool USART_BaudRateAutoDetectIsComplete_Unsupported( USART_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_USART_BaudRateAutoDetectIsComplete");

    return false;
}


#endif /*_USART_BAUDRATEAUTODETECT_UNSUPPORTED_H*/

/******************************************************************************
 End of File
*/

