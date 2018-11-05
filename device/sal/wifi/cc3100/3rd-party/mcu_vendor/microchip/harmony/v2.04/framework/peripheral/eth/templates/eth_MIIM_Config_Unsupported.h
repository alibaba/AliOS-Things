/*******************************************************************************
  ETH Peripheral Library Template Implementation

  File Name:
    eth_MIIM_Config_Unsupported.h

  Summary:
    ETH PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : MIIM_Config
    and its Variant : Unsupported
    For following APIs :
        PLIB_ETH_MIIMResetEnable
        PLIB_ETH_MIIMResetDisable
        PLIB_ETH_MIIMResetIsEnabled
        PLIB_ETH_MIIMClockGet
        PLIB_ETH_MIIMClockSet
        PLIB_ETH_MIIMNoPreEnable
        PLIB_ETH_MIIMNoPreDisable
        PLIB_ETH_MIIMNoPreIsEnabled
        PLIB_ETH_MIIMScanIncrEnable
        PLIB_ETH_MIIMScanIncrDisable
        PLIB_ETH_MIIMScanIncrIsEnabled
        PLIB_ETH_ExistsMIIM_Config

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

#ifndef _ETH_MIIM_CONFIG_UNSUPPORTED_H
#define _ETH_MIIM_CONFIG_UNSUPPORTED_H

//******************************************************************************
/* Function :  ETH_MIIMResetEnable_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_ETH_MIIMResetEnable 

  Description:
    This template implements the Unsupported variant of the PLIB_ETH_MIIMResetEnable function.
*/

PLIB_TEMPLATE void ETH_MIIMResetEnable_Unsupported( ETH_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_ETH_MIIMResetEnable");
}


//******************************************************************************
/* Function :  ETH_MIIMResetDisable_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_ETH_MIIMResetDisable 

  Description:
    This template implements the Unsupported variant of the PLIB_ETH_MIIMResetDisable function.
*/

PLIB_TEMPLATE void ETH_MIIMResetDisable_Unsupported( ETH_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_ETH_MIIMResetDisable");
}


//******************************************************************************
/* Function :  ETH_MIIMResetIsEnabled_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_ETH_MIIMResetIsEnabled 

  Description:
    This template implements the Unsupported variant of the PLIB_ETH_MIIMResetIsEnabled function.
*/

PLIB_TEMPLATE bool ETH_MIIMResetIsEnabled_Unsupported( ETH_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_ETH_MIIMResetIsEnabled");

    return false;
}


//******************************************************************************
/* Function :  ETH_MIIMClockGet_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_ETH_MIIMClockGet 

  Description:
    This template implements the Unsupported variant of the PLIB_ETH_MIIMClockGet function.
*/

PLIB_TEMPLATE ETH_MIIM_CLK ETH_MIIMClockGet_Unsupported( ETH_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_ETH_MIIMClockGet");

    return 0;
}


//******************************************************************************
/* Function :  ETH_MIIMClockSet_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_ETH_MIIMClockSet 

  Description:
    This template implements the Unsupported variant of the PLIB_ETH_MIIMClockSet function.
*/

PLIB_TEMPLATE void ETH_MIIMClockSet_Unsupported( ETH_MODULE_ID index , ETH_MIIM_CLK MIIMClock )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_ETH_MIIMClockSet");
}


//******************************************************************************
/* Function :  ETH_MIIMNoPreEnable_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_ETH_MIIMNoPreEnable 

  Description:
    This template implements the Unsupported variant of the PLIB_ETH_MIIMNoPreEnable function.
*/

PLIB_TEMPLATE void ETH_MIIMNoPreEnable_Unsupported( ETH_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_ETH_MIIMNoPreEnable");
}


//******************************************************************************
/* Function :  ETH_MIIMNoPreDisable_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_ETH_MIIMNoPreDisable 

  Description:
    This template implements the Unsupported variant of the PLIB_ETH_MIIMNoPreDisable function.
*/

PLIB_TEMPLATE void ETH_MIIMNoPreDisable_Unsupported( ETH_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_ETH_MIIMNoPreDisable");
}


//******************************************************************************
/* Function :  ETH_MIIMNoPreIsEnabled_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_ETH_MIIMNoPreIsEnabled 

  Description:
    This template implements the Unsupported variant of the PLIB_ETH_MIIMNoPreIsEnabled function.
*/

PLIB_TEMPLATE bool ETH_MIIMNoPreIsEnabled_Unsupported( ETH_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_ETH_MIIMNoPreIsEnabled");

    return false;
}


//******************************************************************************
/* Function :  ETH_MIIMScanIncrEnable_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_ETH_MIIMScanIncrEnable 

  Description:
    This template implements the Unsupported variant of the PLIB_ETH_MIIMScanIncrEnable function.
*/

PLIB_TEMPLATE void ETH_MIIMScanIncrEnable_Unsupported( ETH_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_ETH_MIIMScanIncrEnable");
}


//******************************************************************************
/* Function :  ETH_MIIMScanIncrDisable_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_ETH_MIIMScanIncrDisable 

  Description:
    This template implements the Unsupported variant of the PLIB_ETH_MIIMScanIncrDisable function.
*/

PLIB_TEMPLATE void ETH_MIIMScanIncrDisable_Unsupported( ETH_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_ETH_MIIMScanIncrDisable");
}


//******************************************************************************
/* Function :  ETH_MIIMScanIncrIsEnabled_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_ETH_MIIMScanIncrIsEnabled 

  Description:
    This template implements the Unsupported variant of the PLIB_ETH_MIIMScanIncrIsEnabled function.
*/

PLIB_TEMPLATE bool ETH_MIIMScanIncrIsEnabled_Unsupported( ETH_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_ETH_MIIMScanIncrIsEnabled");

    return false;
}


//******************************************************************************
/* Function :  ETH_ExistsMIIM_Config_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_ETH_ExistsMIIM_Config

  Description:
    This template implements the Unsupported variant of the PLIB_ETH_ExistsMIIM_Config function.
*/

PLIB_TEMPLATE bool ETH_ExistsMIIM_Config_Unsupported( ETH_MODULE_ID index )
{
    return false;
}


#endif /*_ETH_MIIM_CONFIG_UNSUPPORTED_H*/

/******************************************************************************
 End of File
*/

