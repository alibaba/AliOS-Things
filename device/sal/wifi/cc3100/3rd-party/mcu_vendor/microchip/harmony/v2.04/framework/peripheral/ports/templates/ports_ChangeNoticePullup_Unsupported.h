/*******************************************************************************
  PORTS Peripheral Library Template Implementation

  File Name:
    ports_ChangeNoticePullup_Unsupported.h

  Summary:
    PORTS PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : ChangeNoticePullup
    and its Variant : Unsupported
    For following APIs :
        PLIB_PORTS_ChangeNoticePullUpEnable
        PLIB_PORTS_ChangeNoticePullUpDisable
        PLIB_PORTS_CnPinsPullUpEnable
        PLIB_PORTS_CnPinsPullUpDisable
        PLIB_PORTS_ExistsChangeNoticePullUp

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

#ifndef _PORTS_CHANGENOTICEPULLUP_UNSUPPORTED_H
#define _PORTS_CHANGENOTICEPULLUP_UNSUPPORTED_H


//******************************************************************************
/* Function :  PORTS_ChangeNoticePullUpEnable_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_PORTS_ChangeNoticePullUpEnable 

  Description:
    This template implements the Unsupported variant of the PLIB_PORTS_ChangeNoticePullUpEnable function.
*/

PLIB_TEMPLATE void PORTS_ChangeNoticePullUpEnable_Unsupported( PORTS_MODULE_ID         index , PORTS_CHANGE_NOTICE_PIN pinNum )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_PORTS_ChangeNoticePullUpEnable");
}


//******************************************************************************
/* Function :  PORTS_ChangeNoticePullUpDisable_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_PORTS_ChangeNoticePullUpDisable 

  Description:
    This template implements the Unsupported variant of the PLIB_PORTS_ChangeNoticePullUpDisable function.
*/

PLIB_TEMPLATE void PORTS_ChangeNoticePullUpDisable_Unsupported( PORTS_MODULE_ID         index , PORTS_CHANGE_NOTICE_PIN pinNum )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_PORTS_ChangeNoticePullUpDisable");
}


//******************************************************************************
/* Function :  PORTS_CnPinsPullUpEnable_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_PORTS_CnPinsPullUpEnable 

  Description:
    This template implements the Unsupported variant of the PLIB_PORTS_CnPinsPullUpEnable function.
*/

PLIB_TEMPLATE void PORTS_CnPinsPullUpEnable_Unsupported( PORTS_MODULE_ID  index , PORTS_CN_PIN   cnPins )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_PORTS_CnPinsPullUpEnable");
}


//******************************************************************************
/* Function :  PORTS_CnPinsPullUpDisable_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_PORTS_CnPinsPullUpDisable 

  Description:
    This template implements the Unsupported variant of the PLIB_PORTS_CnPinsPullUpDisable function.
*/

PLIB_TEMPLATE void PORTS_CnPinsPullUpDisable_Unsupported( PORTS_MODULE_ID  index , PORTS_CN_PIN   cnPins )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_PORTS_CnPinsPullUpDisable");
}


//******************************************************************************
/* Function :  PORTS_ExistsChangeNoticePullUp_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_PORTS_ExistsChangeNoticePullUp

  Description:
    This template implements the Unsupported variant of the PLIB_PORTS_ExistsChangeNoticePullUp function.
*/

PLIB_TEMPLATE bool PORTS_ExistsChangeNoticePullUp_Unsupported( PORTS_MODULE_ID index )
{
    return false;
}


#endif /*_PORTS_CHANGENOTICEPULLUP_UNSUPPORTED_H*/

/******************************************************************************
 End of File
*/

