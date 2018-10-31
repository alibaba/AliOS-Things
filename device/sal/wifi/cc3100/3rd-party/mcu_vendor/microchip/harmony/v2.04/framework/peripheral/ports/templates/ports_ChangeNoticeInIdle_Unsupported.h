/*******************************************************************************
  PORTS Peripheral Library Template Implementation

  File Name:
    ports_ChangeNoticeInIdle_Unsupported.h

  Summary:
    PORTS PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : ChangeNoticeInIdle
    and its Variant : Unsupported
    For following APIs :
        PLIB_PORTS_ChangeNoticeInIdleEnable
        PLIB_PORTS_ChangeNoticeInIdleDisable
        PLIB_PORTS_ExistsChangeNoticeInIdle

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

#ifndef _PORTS_CHANGENOTICEINIDLE_UNSUPPORTED_H
#define _PORTS_CHANGENOTICEINIDLE_UNSUPPORTED_H


//******************************************************************************
/* Function :  PORTS_ChangeNoticeInIdleEnable_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_PORTS_ChangeNoticeInIdleEnable 

  Description:
    This template implements the Unsupported variant of the PLIB_PORTS_ChangeNoticeInIdleEnable function.
*/

PLIB_TEMPLATE void PORTS_ChangeNoticeInIdleEnable_Unsupported( PORTS_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_PORTS_ChangeNoticeInIdleEnable");
}


//******************************************************************************
/* Function :  PORTS_ChangeNoticeInIdleDisable_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_PORTS_ChangeNoticeInIdleDisable 

  Description:
    This template implements the Unsupported variant of the PLIB_PORTS_ChangeNoticeInIdleDisable function.
*/

PLIB_TEMPLATE void PORTS_ChangeNoticeInIdleDisable_Unsupported( PORTS_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_PORTS_ChangeNoticeInIdleDisable");
}


//******************************************************************************
/* Function :  PORTS_ExistsChangeNoticeInIdle_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_PORTS_ExistsChangeNoticeInIdle

  Description:
    This template implements the Unsupported variant of the PLIB_PORTS_ExistsChangeNoticeInIdle function.
*/

PLIB_TEMPLATE bool PORTS_ExistsChangeNoticeInIdle_Unsupported( PORTS_MODULE_ID index )
{
    return false;
}


#endif /*_PORTS_CHANGENOTICEINIDLE_UNSUPPORTED_H*/

/******************************************************************************
 End of File
*/

