/*******************************************************************************
  PORTS Peripheral Library Template Implementation

  File Name:
    ports_SlewRateControl_Unsupported.h

  Summary:
    PORTS PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : SlewRateControl
    and its Variant : Unsupported
    For following APIs :
        PLIB_PORTS_ChannelSlewRateSelect
        PLIB_PORTS_PinSlewRateGet
        PLIB_PORTS_ExistsSlewRateControl

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

#ifndef _PORTS_SLEWRATECONTROL_UNSUPPORTED_H
#define _PORTS_SLEWRATECONTROL_UNSUPPORTED_H




//******************************************************************************
/* Function :  PORTS_ChannelSlewRateSelect_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_PORTS_ChannelSlewRateSelect 

  Description:
    This template implements the Unsupported variant of the PLIB_PORTS_ChannelSlewRateSelect function.
*/

PLIB_TEMPLATE void PORTS_ChannelSlewRateSelect_Unsupported( PORTS_MODULE_ID  index , PORTS_CHANNEL channel , PORTS_DATA_MASK   channelMask , PORTS_PIN_SLEW_RATE slewRate )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_PORTS_ChannelSlewRateSelect");
}


//******************************************************************************
/* Function :  PORTS_PinSlewRateGet_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_PORTS_PinSlewRateGet 

  Description:
    This template implements the Unsupported variant of the PLIB_PORTS_PinSlewRateGet function.
*/

PLIB_TEMPLATE PORTS_PIN_SLEW_RATE PORTS_PinSlewRateGet_Unsupported( PORTS_MODULE_ID  index , PORTS_CHANNEL channel , PORTS_BIT_POS bitPos )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_PORTS_PinSlewRateGet");

    return (PORTS_PIN_SLEW_RATE) 0;
}


//******************************************************************************
/* Function :  PORTS_ExistsSlewRateControl_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_PORTS_ExistsSlewRateControl

  Description:
    This template implements the Unsupported variant of the PLIB_PORTS_ExistsSlewRateControl function.
*/

PLIB_TEMPLATE bool PORTS_ExistsSlewRateControl_Unsupported( PORTS_MODULE_ID index )
{
    return false;
}


#endif /*_PORTS_SLEWRATECONTROL_UNSUPPORTED_H*/

/******************************************************************************
 End of File
*/

