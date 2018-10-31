/*******************************************************************************
  NVM Peripheral Library Template Implementation

  File Name:
    nvm_EEPROMErrorStatus_Unsupported.h

  Summary:
    NVM PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : EEPROMErrorStatus
    and its Variant : Unsupported
    For following APIs :
        PLIB_NVM_ExistsEEPROMErrorStatus
        PLIB_NVM_EEPROMErrorGet
        PLIB_NVM_EEPROMErrorClear

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

#ifndef _NVM_EEPROMERRORSTATUS_UNSUPPORTED_H
#define _NVM_EEPROMERRORSTATUS_UNSUPPORTED_H

//******************************************************************************
/* Function :  NVM_ExistsEEPROMErrorStatus_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_NVM_ExistsEEPROMErrorStatus

  Description:
    This template implements the Unsupported variant of the PLIB_NVM_ExistsEEPROMErrorStatus function.
*/

PLIB_TEMPLATE bool NVM_ExistsEEPROMErrorStatus_Unsupported( NVM_MODULE_ID index )
{
    return false;
}


//******************************************************************************
/* Function :  NVM_EEPROMErrorGet_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_NVM_EEPROMErrorGet 

  Description:
    This template implements the Unsupported variant of the PLIB_NVM_EEPROMErrorGet function.
*/

PLIB_TEMPLATE EEPROM_ERROR NVM_EEPROMErrorGet_Unsupported( NVM_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_NVM_EEPROMErrorGet");

    return (EEPROM_ERROR) 0;
}


//******************************************************************************
/* Function :  NVM_EEPROMErrorClear_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_NVM_EEPROMErrorClear 

  Description:
    This template implements the Unsupported variant of the PLIB_NVM_EEPROMErrorClear function.
*/

PLIB_TEMPLATE void NVM_EEPROMErrorClear_Unsupported( NVM_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_NVM_EEPROMErrorClear");
}


#endif /*_NVM_EEPROMERRORSTATUS_UNSUPPORTED_H*/

/******************************************************************************
 End of File
*/

