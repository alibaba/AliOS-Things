/*******************************************************************************
  RESET Peripheral Library Template Implementation

  File Name:
    reset_ConfigRegReadError_Default.h

  Summary:
    RESET PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : ConfigRegReadError
    and its Variant : Default
    For following APIs :
        PLIB_RESET_ExistsConfigRegReadError
        PLIB_RESET_ConfigRegReadErrorGet

*******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2013 released Microchip Technology Inc.  All rights reserved.

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

#ifndef _RESET_CONFIGREGREADERROR_DEFAULT_H
#define _RESET_CONFIGREGREADERROR_DEFAULT_H

//******************************************************************************
/* Function :  RESET_ExistsConfigRegReadError_Default

  Summary:
    Implements Default variant of PLIB_RESET_ExistsConfigRegReadError

  Description:
    This template implements the Default variant of the PLIB_RESET_ExistsConfigRegReadError function.
*/

#define PLIB_RESET_ExistsConfigRegReadError PLIB_RESET_ExistsConfigRegReadError
PLIB_TEMPLATE bool RESET_ExistsConfigRegReadError_Default( RESET_MODULE_ID index )
{
    return true;
}


//******************************************************************************
/* Function :  RESET_ConfigRegReadErrorGet_Default

  Summary:
    Implements Default variant of PLIB_RESET_ConfigRegReadErrorGet 

  Description:
    This template implements the Default variant of the PLIB_RESET_ConfigRegReadErrorGet function.
*/

PLIB_TEMPLATE RESET_CONFIG_REG_READ_ERROR RESET_ConfigRegReadErrorGet_Default( RESET_MODULE_ID index )
{
    return (RESET_CONFIG_REG_READ_ERROR)(RCON & (_RCON_BCFGERR_MASK | _RCON_BCFGFAIL_MASK));
}


#endif /*_RESET_CONFIGREGREADERROR_DEFAULT_H*/

/******************************************************************************
 End of File
*/

