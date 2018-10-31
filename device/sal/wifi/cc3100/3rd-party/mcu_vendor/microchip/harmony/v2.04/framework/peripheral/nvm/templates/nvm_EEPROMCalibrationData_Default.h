/*******************************************************************************
  NVM Peripheral Library Template Implementation

  File Name:
    nvm_EEPROMCalibrationData_Default.h

  Summary:
    NVM PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : EEPROMCalibrationData
    and its Variant : Default
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

#ifndef _NVM_EEPROMCALIBRATIONDATA_DEFAULT_H
#define _NVM_EEPROMCALIBRATIONDATA_DEFAULT_H

//******************************************************************************
/* Routines available for accessing VREGS, MASKS, POS, LEN are 

  VREGs: 
    _NVM_EEPROM_CALDATA_VREG(index)

  MASKs: 
    _NVM_EEPROM_CALDATA_MASK(index)

  POSs: 
    _NVM_EEPROM_CALDATA_POS(index)

  LENs: 
    _NVM_EEPROM_CALDATA_LEN(index)

*/


//******************************************************************************
/* Function :  NVM_ExistsEEPROMCalibrationData_Default

  Summary:
    Implements Default variant of PLIB_NVM_ExistsEEPROMCalibrationData

  Description:
    This template implements the Default variant of the PLIB_NVM_ExistsEEPROMCalibrationData function.
*/

#define PLIB_NVM_ExistsEEPROMCalibrationData PLIB_NVM_ExistsEEPROMCalibrationData
PLIB_TEMPLATE bool NVM_ExistsEEPROMCalibrationData_Default( NVM_MODULE_ID index )
{
    return true;
}


//******************************************************************************
/* Function :  NVM_EEPROMReadCalibrationData_Default

  Summary:
    Implements Default variant of PLIB_NVM_EEPROMReadCalibrationData 

  Description:
    This template implements the Default variant of the PLIB_NVM_EEPROMReadCalibrationData function.
*/

PLIB_TEMPLATE uint32_t NVM_EEPROMReadCalibrationData_Default( NVM_MODULE_ID index , EEPROM_CALIBRATION_REG regIndex )
{
    uint32_t calData = 0;

    switch (regIndex)
    {
        case CALIBRATION_REG_0:
        default:
            {
                calData = (uint32_t)DEVEE0;
                break;
            }

        case CALIBRATION_REG_1:
            {
                calData = (uint32_t)DEVEE1;
                break;
            }

        case CALIBRATION_REG_2:
            {
                calData = (uint32_t)DEVEE2;
                break;
            }

        case CALIBRATION_REG_3:
            {
                calData = (uint32_t)DEVEE3;
                break;
            }
    }

    return calData;
}


#endif /*_NVM_EEPROMCALIBRATIONDATA_DEFAULT_H*/

/******************************************************************************
 End of File
*/

