/*******************************************************************************
  NVM Peripheral Library Template Implementation

  File Name:
    nvm_EEPROMCalibrationData_Unsupported.h

  Summary:
    NVM PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : EEPROMCalibrationData
    and its Variant : Unsupported
    For following APIs :
        PLIB_NVM_ExistsEEPROMCalibrationData
        PLIB_NVM_EEPROMReadCalibrationData

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

#ifndef _NVM_EEPROMCALIBRATIONDATA_UNSUPPORTED_H
#define _NVM_EEPROMCALIBRATIONDATA_UNSUPPORTED_H

//******************************************************************************
/* Routines available for accessing VREGS, MASKS, POS, LEN are 

  VREGs: 
    None.

  MASKs: 
    None.

  POSs: 
    None.

  LENs: 
    None.

*/


//******************************************************************************
/* Function :  NVM_ExistsEEPROMCalibrationData_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_NVM_ExistsEEPROMCalibrationData

  Description:
    This template implements the Unsupported variant of the PLIB_NVM_ExistsEEPROMCalibrationData function.
*/

PLIB_TEMPLATE bool NVM_ExistsEEPROMCalibrationData_Unsupported( NVM_MODULE_ID index )
{
    return false;
}


//******************************************************************************
/* Function :  NVM_EEPROMReadCalibrationData_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_NVM_EEPROMReadCalibrationData 

  Description:
    This template implements the Unsupported variant of the PLIB_NVM_EEPROMReadCalibrationData function.
*/

PLIB_TEMPLATE uint32_t NVM_EEPROMReadCalibrationData_Unsupported( NVM_MODULE_ID index , EEPROM_CALIBRATION_REG regIndex )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_NVM_EEPROMReadCalibrationData");

    return (uint32_t) 0;
}


#endif /*_NVM_EEPROMCALIBRATIONDATA_UNSUPPORTED_H*/

/******************************************************************************
 End of File
*/

