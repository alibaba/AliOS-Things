/*******************************************************************************
  RESET Peripheral Library Template Implementation

  File Name:
    reset_ResetReasonStatus_MX_1.h

  Summary:
    RESET PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : ResetReasonStatus
    and its Variant : MX_1
    For following APIs :
        PLIB_RESET_ExistsResetReasonStatus
        PLIB_RESET_ReasonGet
        PLIB_RESET_ReasonClear

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

#ifndef _RESET_RESETREASONSTATUS_MX_1_H
#define _RESET_RESETREASONSTATUS_MX_1_H

//******************************************************************************
/* Function :  RESET_ExistsResetReasonStatus_MX_1

  Summary:
    Implements MX_1 variant of PLIB_RESET_ExistsResetReasonStatus

  Description:
    This template implements the MX_1 variant of the PLIB_RESET_ExistsResetReasonStatus function.
*/

#define PLIB_RESET_ExistsResetReasonStatus PLIB_RESET_ExistsResetReasonStatus
PLIB_TEMPLATE bool RESET_ExistsResetReasonStatus_MX_1( RESET_MODULE_ID index )
{
    return true;
}


//******************************************************************************
/* Function :  RESET_ReasonGet_MX_1

  Summary:
    Implements MX_1 variant of PLIB_RESET_ReasonGet 

  Description:
    This template implements the MX_1 variant of the PLIB_RESET_ReasonGet function.
*/

PLIB_TEMPLATE RESET_REASON RESET_ReasonGet_MX_1( RESET_MODULE_ID index )
{
    return(RESET_REASON)(RCON & (_RCON_HVDR_MASK | _RCON_CMR_MASK |
				 _RCON_EXTR_MASK |_RCON_SWR_MASK | _RCON_WDTO_MASK|
				 _RCON_BOR_MASK | _RCON_POR_MASK));
}


//******************************************************************************
/* Function :  RESET_ReasonClear_MX_1

  Summary:
    Implements MX_1 variant of PLIB_RESET_ReasonClear 

  Description:
    This template implements the MX_1 variant of the PLIB_RESET_ReasonClear function.
*/

PLIB_TEMPLATE void RESET_ReasonClear_MX_1( RESET_MODULE_ID index , RESET_REASON reason )
{
   RCONCLR = reason;
}


#endif /*_RESET_RESETREASONSTATUS_MX_1_H*/

/******************************************************************************
 End of File
*/

