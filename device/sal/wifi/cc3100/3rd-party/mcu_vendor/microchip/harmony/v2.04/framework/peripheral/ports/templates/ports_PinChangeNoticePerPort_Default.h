/*******************************************************************************
  PORTS Peripheral Library Template Implementation

  File Name:
    ports_PinChangeNoticePerPort_Default.h

  Summary:
    PORTS PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : PinChangeNoticePerPort
    and its Variant : Default
    For following APIs :
        PLIB_PORTS_PinChangeNoticePerPortEnable
        PLIB_PORTS_PinChangeNoticePerPortDisable
        PLIB_PORTS_ChannelChangeNoticeEnable
        PLIB_PORTS_ChannelChangeNoticeDisable
        PLIB_PORTS_ExistsPinChangeNoticePerPort

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

#ifndef _PORTS_PINCHANGENOTICEPERPORT_DEFAULT_H
#define _PORTS_PINCHANGENOTICEPERPORT_DEFAULT_H



//******************************************************************************
/* Function :  PORTS_PinChangeNoticePerPortEnable_Default

  Summary:
    Implements Default variant of PLIB_PORTS_PinChangeNoticePerPortEnable 

  Description:
    This template implements the Default variant of the PLIB_PORTS_PinChangeNoticePerPortEnable function.
*/

PLIB_TEMPLATE void PORTS_PinChangeNoticePerPortEnable_Default( PORTS_MODULE_ID  index , PORTS_CHANNEL channel , PORTS_BIT_POS  bitPos )
{
    *(&CNENBSET + (channel - 1) * 0x40) = 1<<bitPos;
}


//******************************************************************************
/* Function :  PORTS_PinChangeNoticePerPortDisable_Default

  Summary:
    Implements Default variant of PLIB_PORTS_PinChangeNoticePerPortDisable 

  Description:
    This template implements the Default variant of the PLIB_PORTS_PinChangeNoticePerPortDisable function.
*/

PLIB_TEMPLATE void PORTS_PinChangeNoticePerPortDisable_Default( PORTS_MODULE_ID  index , PORTS_CHANNEL channel , PORTS_BIT_POS  bitPos )
{
    *(&CNENBCLR + (channel - 1) * 0x40) = 1<<bitPos;
}


//******************************************************************************
/* Function :  PORTS_ChannelChangeNoticeEnable_Default

  Summary:
    Implements Default variant of PLIB_PORTS_ChannelChangeNoticeEnable 

  Description:
    This template implements the Default variant of the PLIB_PORTS_ChannelChangeNoticeEnable function.
*/

PLIB_TEMPLATE void PORTS_ChannelChangeNoticeEnable_Default( PORTS_MODULE_ID  index , PORTS_CHANNEL channel , PORTS_DATA_MASK  mask )
{
    *(&CNENBSET + (channel - 1) * 0x40) = mask;
}


//******************************************************************************
/* Function :  PORTS_ChannelChangeNoticeDisable_Default

  Summary:
    Implements Default variant of PLIB_PORTS_ChannelChangeNoticeDisable 

  Description:
    This template implements the Default variant of the PLIB_PORTS_ChannelChangeNoticeDisable function.
*/

PLIB_TEMPLATE void PORTS_ChannelChangeNoticeDisable_Default( PORTS_MODULE_ID  index , PORTS_CHANNEL channel , PORTS_DATA_MASK  mask )
{
    *(&CNENBCLR + (channel - 1) * 0x40) = mask;
}


//******************************************************************************
/* Function :  PORTS_ExistsPinChangeNoticePerPort_Default

  Summary:
    Implements Default variant of PLIB_PORTS_ExistsPinChangeNoticePerPort

  Description:
    This template implements the Default variant of the PLIB_PORTS_ExistsPinChangeNoticePerPort function.
*/

#define PLIB_PORTS_ExistsPinChangeNoticePerPort PLIB_PORTS_ExistsPinChangeNoticePerPort
PLIB_TEMPLATE bool PORTS_ExistsPinChangeNoticePerPort_Default( PORTS_MODULE_ID index )
{
    return true;
}


#endif /*_PORTS_PINCHANGENOTICEPERPORT_DEFAULT_H*/

/******************************************************************************
 End of File
*/

