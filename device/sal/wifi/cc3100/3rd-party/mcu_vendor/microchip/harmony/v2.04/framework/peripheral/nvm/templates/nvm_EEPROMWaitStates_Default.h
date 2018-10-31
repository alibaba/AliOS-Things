/*******************************************************************************
  NVM Peripheral Library Template Implementation

  File Name:
    nvm_EEPROMWaitStates_Default.h

  Summary:
    NVM PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : EEPROMWaitStates
    and its Variant : Default
    For following APIs :
        PLIB_NVM_ExistsEEPROMWaitStates
        PLIB_NVM_EEPROMWaitStatesGet
        PLIB_NVM_EEPROMWaitStatesSet

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

#ifndef _NVM_EEPROMWAITSTATES_DEFAULT_H
#define _NVM_EEPROMWAITSTATES_DEFAULT_H

//******************************************************************************
/* Routines available for accessing VREGS, MASKS, POS, LEN are 

  VREGs: 
    _NVM_EEPROM_WAITSTATES_VREG(index)

  MASKs: 
    _NVM_EEPROM_WAITSTATES_MASK(index)

  POSs: 
    _NVM_EEPROM_WAITSTATES_POS(index)

  LENs: 
    _NVM_EEPROM_WAITSTATES_LEN(index)

*/


//******************************************************************************
/* Function :  NVM_ExistsEEPROMWaitStates_Default

  Summary:
    Implements Default variant of PLIB_NVM_ExistsEEPROMWaitStates

  Description:
    This template implements the Default variant of the PLIB_NVM_ExistsEEPROMWaitStates function.
*/

#define PLIB_NVM_ExistsEEPROMWaitStates PLIB_NVM_ExistsEEPROMWaitStates
PLIB_TEMPLATE bool NVM_ExistsEEPROMWaitStates_Default( NVM_MODULE_ID index )
{
    return true;
}


//******************************************************************************
/* Function :  NVM_EEPROMWaitStatesGet_Default

  Summary:
    Implements Default variant of PLIB_NVM_EEPROMWaitStatesGet 

  Description:
    This template implements the Default variant of the PLIB_NVM_EEPROMWaitStatesGet function.
*/

PLIB_TEMPLATE uint8_t NVM_EEPROMWaitStatesGet_Default( NVM_MODULE_ID index )
{
    return (uint8_t)CFGCON2bits.EEWS;
}


//******************************************************************************
/* Function :  NVM_EEPROMWaitStatesSet_Default

  Summary:
    Implements Default variant of PLIB_NVM_EEPROMWaitStatesSet 

  Description:
    This template implements the Default variant of the PLIB_NVM_EEPROMWaitStatesSet function.
*/

PLIB_TEMPLATE void NVM_EEPROMWaitStatesSet_Default( NVM_MODULE_ID index , uint8_t eews )
{
    CFGCON2bits.EEWS = eews;
}


#endif /*_NVM_EEPROMWAITSTATES_DEFAULT_H*/

/******************************************************************************
 End of File
*/

