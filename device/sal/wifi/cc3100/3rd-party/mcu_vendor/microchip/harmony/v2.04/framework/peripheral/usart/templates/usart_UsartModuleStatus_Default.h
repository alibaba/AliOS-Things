/*******************************************************************************
  USART Peripheral Library Template Implementation

  File Name:
    usart_UsartModuleStatus_Default.h

  Summary:
    USART PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : UsartModuleStatus
    and its Variant : Default
    For following APIs :
        PLIB_USART_ExistsModuleBusyStatus
        PLIB_USART_ModuleIsBusy

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

#ifndef _USART_USARTMODULESTATUS_DEFAULT_H
#define _USART_USARTMODULESTATUS_DEFAULT_H

#include "usart_registers.h"

//******************************************************************************
/* Function :  USART_ExistsModuleBusyStatus_Default

  Summary:
    Implements Default variant of PLIB_USART_ExistsModuleBusyStatus

  Description:
    This template implements the Default variant of the PLIB_USART_ExistsModuleBusyStatus function.
*/

#define PLIB_USART_ExistsModuleBusyStatus PLIB_USART_ExistsModuleBusyStatus
PLIB_TEMPLATE bool USART_ExistsModuleBusyStatus_Default( USART_MODULE_ID index )
{
    return true;
}


//******************************************************************************
/* Function :  USART_ModuleIsBusy_Default

  Summary:
    Implements Default variant of PLIB_USART_ModuleIsBusy 

  Description:
    This template implements the Default variant of the PLIB_USART_ModuleIsBusy function.
*/

PLIB_TEMPLATE bool USART_ModuleIsBusy_Default( USART_MODULE_ID index )
{
    usart_registers_t volatile * usart = ((usart_registers_t *)(index));
    return (bool)usart->UxMODE.ACTIVE ;
}


#endif /*_USART_USARTMODULESTATUS_DEFAULT_H*/

/******************************************************************************
 End of File
*/

