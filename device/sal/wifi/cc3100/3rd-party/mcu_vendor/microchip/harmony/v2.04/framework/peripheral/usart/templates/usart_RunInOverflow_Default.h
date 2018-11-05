/*******************************************************************************
  USART Peripheral Library Template Implementation

  File Name:
    usart_RunInOverflow_Default.h

  Summary:
    USART PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : RunInOverflow
    and its Variant : Default
    For following APIs :
        PLIB_USART_ExistsRunInOverflow
        PLIB_USART_RunInOverflowEnable
        PLIB_USART_RunInOverflowDisable
        PLIB_USART_RunInOverflowIsEnabled

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

#ifndef _USART_RUNINOVERFLOW_DEFAULT_H
#define _USART_RUNINOVERFLOW_DEFAULT_H

#include "usart_registers.h"

//******************************************************************************
/* Function :  USART_ExistsRunInOverflow_Default

  Summary:
    Implements Default variant of PLIB_USART_ExistsRunInOverflow

  Description:
    This template implements the Default variant of the PLIB_USART_ExistsRunInOverflow function.
*/

#define PLIB_USART_ExistsRunInOverflow PLIB_USART_ExistsRunInOverflow
PLIB_TEMPLATE bool USART_ExistsRunInOverflow_Default( USART_MODULE_ID index )
{
    return true;
}


//******************************************************************************
/* Function :  USART_RunInOverflowEnable_Default

  Summary:
    Implements Default variant of PLIB_USART_RunInOverflowEnable 

  Description:
    This template implements the Default variant of the PLIB_USART_RunInOverflowEnable function.
*/

PLIB_TEMPLATE void USART_RunInOverflowEnable_Default( USART_MODULE_ID index )
{
    usart_registers_t volatile * usart = ((usart_registers_t *)(index));
    usart->UxMODESET = UxMODE_RUNOVF_MASK;
}


//******************************************************************************
/* Function :  USART_RunInOverflowDisable_Default

  Summary:
    Implements Default variant of PLIB_USART_RunInOverflowDisable 

  Description:
    This template implements the Default variant of the PLIB_USART_RunInOverflowDisable function.
*/

PLIB_TEMPLATE void USART_RunInOverflowDisable_Default( USART_MODULE_ID index )
{
    usart_registers_t volatile * usart = ((usart_registers_t *)(index));
    usart->UxMODECLR = UxMODE_RUNOVF_MASK;
}


//******************************************************************************
/* Function :  USART_RunInOverflowIsEnabled_Default

  Summary:
    Implements Default variant of PLIB_USART_RunInOverflowIsEnabled 

  Description:
    This template implements the Default variant of the PLIB_USART_RunInOverflowIsEnabled function.
*/

PLIB_TEMPLATE bool USART_RunInOverflowIsEnabled_Default( USART_MODULE_ID index )
{
    usart_registers_t volatile * usart = ((usart_registers_t *)(index));
    return (bool)usart->UxMODE.RUNOVF;
}


#endif /*_USART_RUNINOVERFLOW_DEFAULT_H*/

/******************************************************************************
 End of File
*/

