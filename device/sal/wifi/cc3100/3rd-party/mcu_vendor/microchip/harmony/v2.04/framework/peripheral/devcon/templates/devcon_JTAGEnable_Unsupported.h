/*******************************************************************************
  DEVCON Peripheral Library Template Implementation

  File Name:
    devcon_JTAGEnable_Unsupported.h

  Summary:
    DEVCON PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : JTAGEnable
    and its Variant : Unsupported
    For following APIs :
        PLIB_DEVCON_JTAGPortEnable
        PLIB_DEVCON_JTAGPortDisable
        PLIB_DEVCON_ExistsJTAGEnable

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

#ifndef _DEVCON_JTAGENABLE_UNSUPPORTED_H
#define _DEVCON_JTAGENABLE_UNSUPPORTED_H

//******************************************************************************
/* Function :  DEVCON_JTAGPortEnable_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_DEVCON_JTAGPortEnable 

  Description:
    This template implements the Unsupported variant of the PLIB_DEVCON_JTAGPortEnable function.
*/

PLIB_TEMPLATE void DEVCON_JTAGPortEnable_Unsupported( DEVCON_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_DEVCON_JTAGPortEnable");
}


//******************************************************************************
/* Function :  DEVCON_JTAGPortDisable_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_DEVCON_JTAGPortDisable 

  Description:
    This template implements the Unsupported variant of the PLIB_DEVCON_JTAGPortDisable function.
*/

PLIB_TEMPLATE void DEVCON_JTAGPortDisable_Unsupported( DEVCON_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_DEVCON_JTAGPortDisable");
}


//******************************************************************************
/* Function :  DEVCON_ExistsJTAGEnable_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_DEVCON_ExistsJTAGEnable

  Description:
    This template implements the Unsupported variant of the PLIB_DEVCON_ExistsJTAGEnable function.
*/

PLIB_TEMPLATE bool DEVCON_ExistsJTAGEnable_Unsupported( DEVCON_MODULE_ID index )
{
    return false;
}


#endif /*_DEVCON_JTAGENABLE_UNSUPPORTED_H*/

/******************************************************************************
 End of File
*/

