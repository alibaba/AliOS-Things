/*******************************************************************************
  DEVCON Peripheral Library Template Implementation

  File Name:
    devcon_OTPConfigLockUnlock_Unsupported.h

  Summary:
    DEVCON PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : OTPConfigLockUnlock
    and its Variant : Unsupported
    For following APIs :
        PLIB_DEVCON_OTPConfigLock
        PLIB_DEVCON_OTPConfigUnlock
        PLIB_DEVCON_ExistsOTPConfigLockUnlock

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

#ifndef _DEVCON_OTPCONFIGLOCKUNLOCK_UNSUPPORTED_H
#define _DEVCON_OTPCONFIGLOCKUNLOCK_UNSUPPORTED_H

//******************************************************************************
/* Function :  DEVCON_OTPConfigLock_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_DEVCON_OTPConfigLock 

  Description:
    This template implements the Unsupported variant of the PLIB_DEVCON_OTPConfigLock function.
*/

PLIB_TEMPLATE void DEVCON_OTPConfigLock_Unsupported( DEVCON_MODULE_ID index , DEVCON_CFGLOCK lockType )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_DEVCON_OTPConfigLock");
}


//******************************************************************************
/* Function :  DEVCON_OTPConfigUnlock_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_DEVCON_OTPConfigUnlock 

  Description:
    This template implements the Unsupported variant of the PLIB_DEVCON_OTPConfigUnlock function.
*/

PLIB_TEMPLATE void DEVCON_OTPConfigUnlock_Unsupported( DEVCON_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_DEVCON_OTPConfigUnlock");
}


//******************************************************************************
/* Function :  DEVCON_ExistsOTPConfigLockUnlock_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_DEVCON_ExistsOTPConfigLockUnlock

  Description:
    This template implements the Unsupported variant of the PLIB_DEVCON_ExistsOTPConfigLockUnlock function.
*/

PLIB_TEMPLATE bool DEVCON_ExistsOTPConfigLockUnlock_Unsupported( DEVCON_MODULE_ID index )
{
    return false;
}


#endif /*_DEVCON_OTPCONFIGLOCKUNLOCK_UNSUPPORTED_H*/

/******************************************************************************
 End of File
*/

