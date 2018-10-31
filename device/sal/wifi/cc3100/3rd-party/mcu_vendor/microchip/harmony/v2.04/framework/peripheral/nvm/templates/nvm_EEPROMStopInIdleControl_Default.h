/*******************************************************************************
  NVM Peripheral Library Template Implementation

  File Name:
    nvm_EEPROMStopInIdleControl_Default.h

  Summary:
    NVM PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : EEPROMStopInIdleControl
    and its Variant : Default
    For following APIs :
        PLIB_NVM_ExistsEEPROMStopInIdleControl
        PLIB_NVM_EEPROMStopInIdleEnable
        PLIB_NVM_EEPROMStopInIdleDisable
        PLIB_NVM_EEPROMStopInIdleIsEnabled

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

#ifndef _NVM_EEPROMSTOPINIDLECONTROL_DEFAULT_H
#define _NVM_EEPROMSTOPINIDLECONTROL_DEFAULT_H

//******************************************************************************
/* Function :  NVM_ExistsEEPROMStopInIdleControl_Default

  Summary:
    Implements Default variant of PLIB_NVM_ExistsEEPROMStopInIdleControl

  Description:
    This template implements the Default variant of the PLIB_NVM_ExistsEEPROMStopInIdleControl function.
*/

#define PLIB_NVM_ExistsEEPROMStopInIdleControl PLIB_NVM_ExistsEEPROMStopInIdleControl
PLIB_TEMPLATE bool NVM_ExistsEEPROMStopInIdleControl_Default( NVM_MODULE_ID index )
{
    return true;
}


//******************************************************************************
/* Function :  NVM_EEPROMStopInIdleEnable_Default

  Summary:
    Implements Default variant of PLIB_NVM_EEPROMStopInIdleEnable 

  Description:
    This template implements the Default variant of the PLIB_NVM_EEPROMStopInIdleEnable function.
*/

PLIB_TEMPLATE void NVM_EEPROMStopInIdleEnable_Default( NVM_MODULE_ID index )
{
    EECONSET = _EECON_SIDL_MASK;
}


//******************************************************************************
/* Function :  NVM_EEPROMStopInIdleDisable_Default

  Summary:
    Implements Default variant of PLIB_NVM_EEPROMStopInIdleDisable 

  Description:
    This template implements the Default variant of the PLIB_NVM_EEPROMStopInIdleDisable function.
*/

PLIB_TEMPLATE void NVM_EEPROMStopInIdleDisable_Default( NVM_MODULE_ID index )
{
    EECONCLR = _EECON_SIDL_MASK;
}


//******************************************************************************
/* Function :  NVM_EEPROMStopInIdleIsEnabled_Default

  Summary:
    Implements Default variant of PLIB_NVM_EEPROMStopInIdleIsEnabled 

  Description:
    This template implements the Default variant of the PLIB_NVM_EEPROMStopInIdleIsEnabled function.
*/

PLIB_TEMPLATE bool NVM_EEPROMStopInIdleIsEnabled_Default( NVM_MODULE_ID index )
{
    return (bool)EECONbits.SIDL;
}


#endif /*_NVM_EEPROMSTOPINIDLECONTROL_DEFAULT_H*/

/******************************************************************************
 End of File
*/

