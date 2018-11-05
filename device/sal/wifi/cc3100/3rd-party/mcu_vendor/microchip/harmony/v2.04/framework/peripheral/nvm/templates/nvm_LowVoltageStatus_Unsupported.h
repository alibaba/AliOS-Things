/*******************************************************************************
  NVM Peripheral Library Template Implementation

  File Name:
    nvm_LowVoltageStatus_Unsupported.h

  Summary:
    NVM PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : LowVoltageStatus
    and its Variant : Unsupported
    For following APIs :
        PLIB_NVM_ExistsLowVoltageStatus
        PLIB_NVM_LowVoltageEventIsActive

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

#ifndef _NVM_LOWVOLTAGESTATUS_UNSUPPORTED_H
#define _NVM_LOWVOLTAGESTATUS_UNSUPPORTED_H

//******************************************************************************
/* Function :  NVM_ExistsLowVoltageStatus_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_NVM_ExistsLowVoltageStatus

  Description:
    This template implements the Unsupported variant of the PLIB_NVM_ExistsLowVoltageStatus function.
*/

PLIB_TEMPLATE bool NVM_ExistsLowVoltageStatus_Unsupported( NVM_MODULE_ID index )
{
    return false;
}


//******************************************************************************
/* Function :  NVM_LowVoltageEventIsActive_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_NVM_LowVoltageEventIsActive 

  Description:
    This template implements the Unsupported variant of the PLIB_NVM_LowVoltageEventIsActive function.
*/

PLIB_TEMPLATE bool NVM_LowVoltageEventIsActive_Unsupported( NVM_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_NVM_LowVoltageEventIsActive");

    return false;
}


#endif /*_NVM_LOWVOLTAGESTATUS_UNSUPPORTED_H*/

/******************************************************************************
 End of File
*/

