/*******************************************************************************
  PORTS Peripheral Library Template Implementation

  File Name:
    ports_PinChangeNotice_Default.h

  Summary:
    PORTS PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : PinChangeNotice
    and its Variant : Default
    For following APIs :
        PLIB_PORTS_PinChangeNoticeEnable
        PLIB_PORTS_PinChangeNoticeDisable
        PLIB_PORTS_CnPinsEnable
        PLIB_PORTS_CnPinsDisable
        PLIB_PORTS_ExistsPinChangeNotice

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

#ifndef _PORTS_PINCHANGENOTICE_DEFAULT_H
#define _PORTS_PINCHANGENOTICE_DEFAULT_H


//******************************************************************************
/* Function :  PORTS_PinChangeNoticeEnable_Default

  Summary:
    Implements Default variant of PLIB_PORTS_PinChangeNoticeEnable 

  Description:
    This template implements the Default variant of the PLIB_PORTS_PinChangeNoticeEnable function.
*/

PLIB_TEMPLATE void PORTS_PinChangeNoticeEnable_Default( PORTS_MODULE_ID         index , PORTS_CHANGE_NOTICE_PIN pinNum )
{
    *(&CNENSET) = 1<<pinNum;
}


//******************************************************************************
/* Function :  PORTS_PinChangeNoticeDisable_Default

  Summary:
    Implements Default variant of PLIB_PORTS_PinChangeNoticeDisable 

  Description:
    This template implements the Default variant of the PLIB_PORTS_PinChangeNoticeDisable function.
*/

PLIB_TEMPLATE void PORTS_PinChangeNoticeDisable_Default( PORTS_MODULE_ID         index , PORTS_CHANGE_NOTICE_PIN pinNum )
{
    *(&CNENCLR) = 1<<pinNum;
}


//******************************************************************************
/* Function :  PORTS_CnPinsEnable_Default

  Summary:
    Implements Default variant of PLIB_PORTS_CnPinsEnable 

  Description:
    This template implements the Default variant of the PLIB_PORTS_CnPinsEnable function.
*/

PLIB_TEMPLATE void PORTS_CnPinsEnable_Default( PORTS_MODULE_ID  index , PORTS_CN_PIN   cnPins )
{
    *(&CNENSET) = cnPins;
}


//******************************************************************************
/* Function :  PORTS_CnPinsDisable_Default

  Summary:
    Implements Default variant of PLIB_PORTS_CnPinsDisable 

  Description:
    This template implements the Default variant of the PLIB_PORTS_CnPinsDisable function.
*/

PLIB_TEMPLATE void PORTS_CnPinsDisable_Default( PORTS_MODULE_ID  index , PORTS_CN_PIN   cnPins )
{
    *(&CNENCLR) = cnPins;
}


//******************************************************************************
/* Function :  PORTS_ExistsPinChangeNotice_Default

  Summary:
    Implements Default variant of PLIB_PORTS_ExistsPinChangeNotice

  Description:
    This template implements the Default variant of the PLIB_PORTS_ExistsPinChangeNotice function.
*/

#define PLIB_PORTS_ExistsPinChangeNotice PLIB_PORTS_ExistsPinChangeNotice
PLIB_TEMPLATE bool PORTS_ExistsPinChangeNotice_Default( PORTS_MODULE_ID index )
{
    return true;
}


#endif /*_PORTS_PINCHANGENOTICE_DEFAULT_H*/

/******************************************************************************
 End of File
*/

