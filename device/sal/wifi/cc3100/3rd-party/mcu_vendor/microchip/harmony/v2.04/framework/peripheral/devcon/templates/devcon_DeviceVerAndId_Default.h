/*******************************************************************************
  DEVCON Peripheral Library Template Implementation

  File Name:
    devcon_DeviceVerAndId_Default.h

  Summary:
    DEVCON PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : DeviceVerAndId
    and its Variant : Default
    For following APIs :
        PLIB_DEVCON_DeviceVersionGet
        PLIB_DEVCON_DeviceIdGet
        PLIB_DEVCON_ExistsDeviceVerAndId

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

#ifndef _DEVCON_DEVICEVERANDID_DEFAULT_H
#define _DEVCON_DEVICEVERANDID_DEFAULT_H

//******************************************************************************
/* Function :  DEVCON_DeviceVersionGet_Default

  Summary:
    Implements Default variant of PLIB_DEVCON_DeviceVersionGet

  Description:
    This template implements the Default variant of the PLIB_DEVCON_DeviceVersionGet function.
*/

PLIB_TEMPLATE uint8_t DEVCON_DeviceVersionGet_Default( DEVCON_MODULE_ID index )
{
	return (uint8_t) DEVIDbits.VER;
}


//******************************************************************************
/* Function :  DEVCON_DeviceIdGet_Default

  Summary:
    Implements Default variant of PLIB_DEVCON_DeviceIdGet

  Description:
    This template implements the Default variant of the PLIB_DEVCON_DeviceIdGet function.
*/

PLIB_TEMPLATE uint32_t DEVCON_DeviceIdGet_Default( DEVCON_MODULE_ID index )
{
	return (uint32_t)DEVIDbits.DEVID;
}


//******************************************************************************
/* Function :  DEVCON_ExistsDeviceVerAndId_Default

  Summary:
    Implements Default variant of PLIB_DEVCON_ExistsDeviceVerAndId

  Description:
    This template implements the Default variant of the PLIB_DEVCON_ExistsDeviceVerAndId function.
*/

#define PLIB_DEVCON_ExistsDeviceVerAndId PLIB_DEVCON_ExistsDeviceVerAndId
PLIB_TEMPLATE bool DEVCON_ExistsDeviceVerAndId_Default( DEVCON_MODULE_ID index )
{
    return true;
}


#endif /*_DEVCON_DEVICEVERANDID_DEFAULT_H*/

/******************************************************************************
 End of File
*/

