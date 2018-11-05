/*******************************************************************************
  NVM Peripheral Library Template Implementation

  File Name:
    nvm_EEPROMEnableOperationControl_Unsupported.h

  Summary:
    NVM PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : EEPROMEnableOperationControl
    and its Variant : Unsupported
    For following APIs :
        PLIB_NVM_ExistsEEPROMEnableOperationControl
        PLIB_NVM_EEPROMWriteEnable
        PLIB_NVM_EEPROMWriteIsEnabled
        PLIB_NVM_EEPROMReadEnable
        PLIB_NVM_EEPROMReadIsEnabled

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

#ifndef _NVM_EEPROMENABLEOPERATIONCONTROL_UNSUPPORTED_H
#define _NVM_EEPROMENABLEOPERATIONCONTROL_UNSUPPORTED_H

//******************************************************************************
/* Function :  NVM_ExistsEEPROMEnableOperationControl_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_NVM_ExistsEEPROMEnableOperationControl

  Description:
    This template implements the Unsupported variant of the PLIB_NVM_ExistsEEPROMEnableOperationControl function.
*/

PLIB_TEMPLATE bool NVM_ExistsEEPROMEnableOperationControl_Unsupported( NVM_MODULE_ID index )
{
    return false;
}


//******************************************************************************
/* Function :  NVM_EEPROMWriteEnable_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_NVM_EEPROMWriteEnable 

  Description:
    This template implements the Unsupported variant of the PLIB_NVM_EEPROMWriteEnable function.
*/

PLIB_TEMPLATE void NVM_EEPROMWriteEnable_Unsupported( NVM_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_NVM_EEPROMWriteEnable");
}


//******************************************************************************
/* Function :  NVM_EEPROMWriteIsEnabled_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_NVM_EEPROMWriteIsEnabled 

  Description:
    This template implements the Unsupported variant of the PLIB_NVM_EEPROMWriteIsEnabled function.
*/

PLIB_TEMPLATE bool NVM_EEPROMWriteIsEnabled_Unsupported( NVM_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_NVM_EEPROMWriteIsEnabled");

    return false;
}


//******************************************************************************
/* Function :  NVM_EEPROMReadEnable_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_NVM_EEPROMReadEnable 

  Description:
    This template implements the Unsupported variant of the PLIB_NVM_EEPROMReadEnable function.
*/

PLIB_TEMPLATE void NVM_EEPROMReadEnable_Unsupported( NVM_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_NVM_EEPROMReadEnable");
}


//******************************************************************************
/* Function :  NVM_EEPROMReadIsEnabled_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_NVM_EEPROMReadIsEnabled 

  Description:
    This template implements the Unsupported variant of the PLIB_NVM_EEPROMReadIsEnabled function.
*/

PLIB_TEMPLATE bool NVM_EEPROMReadIsEnabled_Unsupported( NVM_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_NVM_EEPROMReadIsEnabled");

    return false;
}


#endif /*_NVM_EEPROMENABLEOPERATIONCONTROL_UNSUPPORTED_H*/

/******************************************************************************
 End of File
*/

