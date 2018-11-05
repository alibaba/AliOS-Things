/*******************************************************************************
  NVM Peripheral Library Template Implementation

  File Name:
    nvm_LowVoltageError_Default.h

  Summary:
    NVM PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : LowVoltageError
    and its Variant : Default
    For following APIs :
        PLIB_NVM_ExistsLowVoltageError
        PLIB_NVM_LowVoltageIsDetected

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

#ifndef _NVM_LOWVOLTAGEERROR_DEFAULT_H
#define _NVM_LOWVOLTAGEERROR_DEFAULT_H

//******************************************************************************
/* Function :  NVM_ExistsLowVoltageError_Default

  Summary:
    Implements Default variant of PLIB_NVM_ExistsLowVoltageError

  Description:
    This template implements the Default variant of the PLIB_NVM_ExistsLowVoltageError function.
*/

#define PLIB_NVM_ExistsLowVoltageError PLIB_NVM_ExistsLowVoltageError
PLIB_TEMPLATE bool NVM_ExistsLowVoltageError_Default( NVM_MODULE_ID index )
{
    return true;
}


//******************************************************************************
/* Function :  NVM_LowVoltageIsDetected_Default

  Summary:
    Implements Default variant of PLIB_NVM_LowVoltageIsDetected 

  Description:
    This template implements the Default variant of the PLIB_NVM_LowVoltageIsDetected function.
*/

PLIB_TEMPLATE bool NVM_LowVoltageIsDetected_Default( NVM_MODULE_ID index )
{
    return (bool)NVMCONbits.LVDERR;
}


#endif /*_NVM_LOWVOLTAGEERROR_DEFAULT_H*/

/******************************************************************************
 End of File
*/

