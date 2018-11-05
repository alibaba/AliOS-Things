/*******************************************************************************
  ETH Peripheral Library Template Implementation

  File Name:
    eth_MAC_Testing_Unsupported.h

  Summary:
    ETH PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : MAC_Testing
    and its Variant : Unsupported
    For following APIs :
        PLIB_ETH_TestBackPressEnable
        PLIB_ETH_TestBackPressDisable
        PLIB_ETH_TestBackPressIsEnabled
        PLIB_ETH_TestPauseEnable
        PLIB_ETH_TestPauseDisable
        PLIB_ETH_TestPauseIsEnabled
        PLIB_ETH_ShortcutQuantaEnable
        PLIB_ETH_ShortcutQuantaDisable
        PLIB_ETH_ShortcutQuantaIsEnabled
        PLIB_ETH_ExistsMAC_Testing

*******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2012 released Microchip Technology Inc.  All rights reserved.

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

#ifndef _ETH_MAC_TESTING_UNSUPPORTED_H
#define _ETH_MAC_TESTING_UNSUPPORTED_H

//******************************************************************************
/* Function :  ETH_TestBackPressEnable_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_ETH_TestBackPressEnable 

  Description:
    This template implements the Unsupported variant of the PLIB_ETH_TestBackPressEnable function.
*/

PLIB_TEMPLATE void ETH_TestBackPressEnable_Unsupported( ETH_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_ETH_TestBackPressEnable");
}


//******************************************************************************
/* Function :  ETH_TestBackPressDisable_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_ETH_TestBackPressDisable 

  Description:
    This template implements the Unsupported variant of the PLIB_ETH_TestBackPressDisable function.
*/

PLIB_TEMPLATE void ETH_TestBackPressDisable_Unsupported( ETH_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_ETH_TestBackPressDisable");
}


//******************************************************************************
/* Function :  ETH_TestBackPressIsEnabled_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_ETH_TestBackPressIsEnabled 

  Description:
    This template implements the Unsupported variant of the PLIB_ETH_TestBackPressIsEnabled function.
*/

PLIB_TEMPLATE bool ETH_TestBackPressIsEnabled_Unsupported( ETH_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_ETH_TestBackPressIsEnabled");

    return false;
}


//******************************************************************************
/* Function :  ETH_TestPauseEnable_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_ETH_TestPauseEnable 

  Description:
    This template implements the Unsupported variant of the PLIB_ETH_TestPauseEnable function.
*/

PLIB_TEMPLATE void ETH_TestPauseEnable_Unsupported( ETH_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_ETH_TestPauseEnable");
}


//******************************************************************************
/* Function :  ETH_TestPauseDisable_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_ETH_TestPauseDisable 

  Description:
    This template implements the Unsupported variant of the PLIB_ETH_TestPauseDisable function.
*/

PLIB_TEMPLATE void ETH_TestPauseDisable_Unsupported( ETH_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_ETH_TestPauseDisable");
}


//******************************************************************************
/* Function :  ETH_TestPauseIsEnabled_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_ETH_TestPauseIsEnabled 

  Description:
    This template implements the Unsupported variant of the PLIB_ETH_TestPauseIsEnabled function.
*/

PLIB_TEMPLATE bool ETH_TestPauseIsEnabled_Unsupported( ETH_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_ETH_TestPauseIsEnabled");

    return false;
}


//******************************************************************************
/* Function :  ETH_ShortcutQuantaEnable_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_ETH_ShortcutQuantaEnable 

  Description:
    This template implements the Unsupported variant of the PLIB_ETH_ShortcutQuantaEnable function.
*/

PLIB_TEMPLATE void ETH_ShortcutQuantaEnable_Unsupported( ETH_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_ETH_ShortcutQuantaEnable");
}


//******************************************************************************
/* Function :  ETH_ShortcutQuantaDisable_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_ETH_ShortcutQuantaDisable 

  Description:
    This template implements the Unsupported variant of the PLIB_ETH_ShortcutQuantaDisable function.
*/

PLIB_TEMPLATE void ETH_ShortcutQuantaDisable_Unsupported( ETH_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_ETH_ShortcutQuantaDisable");
}


//******************************************************************************
/* Function :  ETH_ShortcutQuantaIsEnabled_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_ETH_ShortcutQuantaIsEnabled 

  Description:
    This template implements the Unsupported variant of the PLIB_ETH_ShortcutQuantaIsEnabled function.
*/

PLIB_TEMPLATE bool ETH_ShortcutQuantaIsEnabled_Unsupported( ETH_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_ETH_ShortcutQuantaIsEnabled");

    return false;
}


//******************************************************************************
/* Function :  ETH_ExistsMAC_Testing_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_ETH_ExistsMAC_Testing

  Description:
    This template implements the Unsupported variant of the PLIB_ETH_ExistsMAC_Testing function.
*/

PLIB_TEMPLATE bool ETH_ExistsMAC_Testing_Unsupported( ETH_MODULE_ID index )
{
    return false;
}


#endif /*_ETH_MAC_TESTING_UNSUPPORTED_H*/

/******************************************************************************
 End of File
*/

