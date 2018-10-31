/*******************************************************************************
  PORTS Peripheral Library Template Implementation

  File Name:
    ports_PinModePerPort_Unsupported.h

  Summary:
    PORTS PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : PinModePerPort
    and its Variant : Unsupported
    For following APIs :
        PLIB_PORTS_PinModePerPortSelect
        PLIB_PORTS_ChannelModeSelect
        PLIB_PORTS_ExistsPinModePerPort

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

#ifndef _PORTS_PINMODEPERPORT_UNSUPPORTED_H
#define _PORTS_PINMODEPERPORT_UNSUPPORTED_H



//******************************************************************************
/* Function :  PORTS_PinModePerPortSelect_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_PORTS_PinModePerPortSelect 

  Description:
    This template implements the Unsupported variant of the PLIB_PORTS_PinModePerPortSelect function.
*/

PLIB_TEMPLATE void PORTS_PinModePerPortSelect_Unsupported( PORTS_MODULE_ID  index , PORTS_CHANNEL channel , PORTS_BIT_POS  bitPos , PORTS_PIN_MODE   mode )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_PORTS_PinModePerPortSelect");
}


//******************************************************************************
/* Function :  PORTS_ChannelModeSelect_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_PORTS_ChannelModeSelect 

  Description:
    This template implements the Unsupported variant of the PLIB_PORTS_ChannelModeSelect function.
*/

PLIB_TEMPLATE void PORTS_ChannelModeSelect_Unsupported( PORTS_MODULE_ID  index , PORTS_CHANNEL channel , PORTS_DATA_MASK   modeMask , PORTS_PIN_MODE mode )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_PORTS_ChannelModeSelect");
}


//******************************************************************************
/* Function :  PORTS_ExistsPinModePerPort_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_PORTS_ExistsPinModePerPort

  Description:
    This template implements the Unsupported variant of the PLIB_PORTS_ExistsPinModePerPort function.
*/

PLIB_TEMPLATE bool PORTS_ExistsPinModePerPort_Unsupported( PORTS_MODULE_ID index )
{
    return false;
}


#endif /*_PORTS_PINMODEPERPORT_UNSUPPORTED_H*/

/******************************************************************************
 End of File
*/

