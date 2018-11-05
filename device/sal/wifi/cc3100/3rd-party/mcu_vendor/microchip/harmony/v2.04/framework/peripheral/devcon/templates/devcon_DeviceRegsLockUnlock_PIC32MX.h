/*******************************************************************************
  DEVCON Peripheral Library Template Implementation

  File Name:
    devcon_DeviceRegsLockUnlock_PIC32MX.h

  Summary:
    DEVCON PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : DeviceRegsLockUnlock
    and its Variant : PIC32MX
    For following APIs :
        PLIB_DEVCON_DeviceRegistersLock
        PLIB_DEVCON_DeviceRegistersUnlock
        PLIB_DEVCON_ExistsDeviceRegsLockUnlock

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

#ifndef _DEVCON_DEVICEREGSLOCKUNLOCK_PIC32MX_H
#define _DEVCON_DEVICEREGSLOCKUNLOCK_PIC32MX_H

//******************************************************************************
/* Function :  DEVCON_DeviceRegistersLock_PIC32MX

  Summary:
    Implements PIC32MX variant of PLIB_DEVCON_DeviceRegistersLock 

  Description:
    This template implements the PIC32MX variant of the PLIB_DEVCON_DeviceRegistersLock function.
*/

PLIB_TEMPLATE void DEVCON_DeviceRegistersLock_PIC32MX( DEVCON_MODULE_ID index , DEVCON_REGISTER_SET registersToLock )
{
    *((SFR_TYPE *)(&CFGCON)) |= ((_CFGCON_IOLOCK_MASK | _CFGCON_PMDLOCK_MASK) & (registersToLock << (_CFGCON_PMDLOCK_POSITION - 1u)));
}


//******************************************************************************
/* Function :  DEVCON_DeviceRegistersUnlock_PIC32MX

  Summary:
    Implements PIC32MX variant of PLIB_DEVCON_DeviceRegistersUnlock 

  Description:
    This template implements the PIC32MX variant of the PLIB_DEVCON_DeviceRegistersUnlock function.
*/

PLIB_TEMPLATE void DEVCON_DeviceRegistersUnlock_PIC32MX( DEVCON_MODULE_ID index , DEVCON_REGISTER_SET registersToLock )
{
    *((SFR_TYPE *)(&CFGCON)) &= ~((_CFGCON_IOLOCK_MASK | _CFGCON_PMDLOCK_MASK) & (registersToLock << (_CFGCON_PMDLOCK_POSITION - 1u)));
}


//******************************************************************************
/* Function :  DEVCON_ExistsDeviceRegsLockUnlock_PIC32MX

  Summary:
    Implements PIC32MX variant of PLIB_DEVCON_ExistsDeviceRegsLockUnlock

  Description:
    This template implements the PIC32MX variant of the PLIB_DEVCON_ExistsDeviceRegsLockUnlock function.
*/

#define PLIB_DEVCON_ExistsDeviceRegsLockUnlock PLIB_DEVCON_ExistsDeviceRegsLockUnlock
PLIB_TEMPLATE bool DEVCON_ExistsDeviceRegsLockUnlock_PIC32MX( DEVCON_MODULE_ID index )
{
    return true;
}


#endif /*_DEVCON_DEVICEREGSLOCKUNLOCK_PIC32MX_H*/

/******************************************************************************
 End of File
*/

