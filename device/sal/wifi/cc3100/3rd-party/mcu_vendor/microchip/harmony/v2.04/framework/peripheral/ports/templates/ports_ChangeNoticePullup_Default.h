/*******************************************************************************
  PORTS Peripheral Library Template Implementation

  File Name:
    ports_ChangeNoticePullup_Default.h

  Summary:
    PORTS PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : ChangeNoticePullup
    and its Variant : Default
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

#ifndef _PORTS_CHANGENOTICEPULLUP_DEFAULT_H
#define _PORTS_CHANGENOTICEPULLUP_DEFAULT_H


//******************************************************************************
/* Function :  PORTS_ChangeNoticePullUpEnable_Default

  Summary:
    Implements Default variant of PLIB_PORTS_ChangeNoticePullUpEnable 

  Description:
    This template implements the Default variant of the PLIB_PORTS_ChangeNoticePullUpEnable function.
*/

PLIB_TEMPLATE void PORTS_ChangeNoticePullUpEnable_Default( PORTS_MODULE_ID index , PORTS_CHANGE_NOTICE_PIN pinNum )
{
    *(&CNPUESET) = 1<<pinNum;
}


//******************************************************************************
/* Function :  PORTS_ChangeNoticePullUpDisable_Default

  Summary:
    Implements Default variant of PLIB_PORTS_ChangeNoticePullUpDisable 

  Description:
    This template implements the Default variant of the PLIB_PORTS_ChangeNoticePullUpDisable function.
*/

PLIB_TEMPLATE void PORTS_ChangeNoticePullUpDisable_Default( PORTS_MODULE_ID index , PORTS_CHANGE_NOTICE_PIN pinNum )
{
    *(&CNPUECLR) = 1<<pinNum;
}

//******************************************************************************
/* Function :  PORTS_CnPinsPullUpEnable_Default

  Summary:
    Implements Default variant of PLIB_PORTS_CnPinsPullUpEnable 

  Description:
    This template implements the Default variant of the PLIB_PORTS_CnPinsPullUpEnable function.
*/

PLIB_TEMPLATE void PORTS_CnPinsPullUpEnable_Default( PORTS_MODULE_ID  index , PORTS_CN_PIN   cnPins )
{
    *(&CNPUESET) = cnPins;
}


//******************************************************************************
/* Function :  PORTS_CnPinsPullUpDisable_Default

  Summary:
    Implements Default variant of PLIB_PORTS_CnPinsPullUpDisable 

  Description:
    This template implements the Default variant of the PLIB_PORTS_CnPinsPullUpDisable function.
*/

PLIB_TEMPLATE void PORTS_CnPinsPullUpDisable_Default( PORTS_MODULE_ID  index , PORTS_CN_PIN   cnPins )
{
    *(&CNPUECLR) = cnPins;
}


//******************************************************************************
/* Function :  PORTS_ExistsChangeNoticePullUp_Default

  Summary:
    Implements Default variant of PLIB_PORTS_ExistsChangeNoticePullUp

  Description:
    This template implements the Default variant of the PLIB_PORTS_ExistsChangeNoticePullUp function.
*/

#define PLIB_PORTS_ExistsChangeNoticePullUp PLIB_PORTS_ExistsChangeNoticePullUp
PLIB_TEMPLATE bool PORTS_ExistsChangeNoticePullUp_Default( PORTS_MODULE_ID index )
{
    return true;
}


#endif /*_PORTS_CHANGENOTICEPULLUP_DEFAULT_H*/

/******************************************************************************
 End of File
*/

