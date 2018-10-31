/*******************************************************************************
  ETH Peripheral Library Template Implementation

  File Name:
    eth_MAC_Resets_Unsupported.h

  Summary:
    ETH PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : MAC_Resets
    and its Variant : Unsupported
    For following APIs :
        PLIB_ETH_MIIResetEnable
        PLIB_ETH_MIIResetDisable
        PLIB_ETH_MIIResetIsEnabled
        PLIB_ETH_SimResetEnable
        PLIB_ETH_SimResetDisable
        PLIB_ETH_SimResetIsEnabled
        PLIB_ETH_MCSRxResetEnable
        PLIB_ETH_MCSRxResetDisable
        PLIB_ETH_MCSRxResetIsEnabled
        PLIB_ETH_RXFuncResetEnable
        PLIB_ETH_RXFuncResetDisable
        PLIB_ETH_RXFuncResetIsEnabled
        PLIB_ETH_MCSTxResetEnable
        PLIB_ETH_MCSTxResetDisable
        PLIB_ETH_MCSTxResetIsEnabled
        PLIB_ETH_TxFuncResetEnable
        PLIB_ETH_TxFuncResetDisable
        PLIB_ETH_TxFuncResetIsEnabled
        PLIB_ETH_ExistsMAC_Resets

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

#ifndef _ETH_MAC_RESETS_UNSUPPORTED_H
#define _ETH_MAC_RESETS_UNSUPPORTED_H


//******************************************************************************
/* Function :  ETH_MIIResetEnable_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_ETH_MIIResetEnable 

  Description:
    This template implements the Unsupported variant of the PLIB_ETH_MIIResetEnable function.
*/

PLIB_TEMPLATE void ETH_MIIResetEnable_Unsupported( ETH_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_ETH_MIIResetEnable");
}


//******************************************************************************
/* Function :  ETH_MIIResetDisable_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_ETH_MIIResetDisable 

  Description:
    This template implements the Unsupported variant of the PLIB_ETH_MIIResetDisable function.
*/

PLIB_TEMPLATE void ETH_MIIResetDisable_Unsupported( ETH_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_ETH_MIIResetDisable");
}


//******************************************************************************
/* Function :  ETH_MIIResetIsEnabled_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_ETH_MIIResetIsEnabled 

  Description:
    This template implements the Unsupported variant of the PLIB_ETH_MIIResetIsEnabled function.
*/

PLIB_TEMPLATE bool ETH_MIIResetIsEnabled_Unsupported( ETH_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_ETH_MIIResetIsEnabled");

    return false;
}


//******************************************************************************
/* Function :  ETH_SimResetEnable_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_ETH_SimResetEnable 

  Description:
    This template implements the Unsupported variant of the PLIB_ETH_SimResetEnable function.
*/

PLIB_TEMPLATE void ETH_SimResetEnable_Unsupported( ETH_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_ETH_SimResetEnable");
}


//******************************************************************************
/* Function :  ETH_SimResetDisable_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_ETH_SimResetDisable 

  Description:
    This template implements the Unsupported variant of the PLIB_ETH_SimResetDisable function.
*/

PLIB_TEMPLATE void ETH_SimResetDisable_Unsupported( ETH_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_ETH_SimResetDisable");
}


//******************************************************************************
/* Function :  ETH_SimResetIsEnabled_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_ETH_SimResetIsEnabled 

  Description:
    This template implements the Unsupported variant of the PLIB_ETH_SimResetIsEnabled function.
*/

PLIB_TEMPLATE bool ETH_SimResetIsEnabled_Unsupported( ETH_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_ETH_SimResetIsEnabled");

    return false;
}


//******************************************************************************
/* Function :  ETH_MCSRxResetEnable_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_ETH_MCSRxResetEnable 

  Description:
    This template implements the Unsupported variant of the PLIB_ETH_MCSRxResetEnable function.
*/

PLIB_TEMPLATE void ETH_MCSRxResetEnable_Unsupported( ETH_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_ETH_MCSRxResetEnable");
}


//******************************************************************************
/* Function :  ETH_MCSRxResetDisable_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_ETH_MCSRxResetDisable 

  Description:
    This template implements the Unsupported variant of the PLIB_ETH_MCSRxResetDisable function.
*/

PLIB_TEMPLATE void ETH_MCSRxResetDisable_Unsupported( ETH_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_ETH_MCSRxResetDisable");
}


//******************************************************************************
/* Function :  ETH_MCSRxResetIsEnabled_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_ETH_MCSRxResetIsEnabled 

  Description:
    This template implements the Unsupported variant of the PLIB_ETH_MCSRxResetIsEnabled function.
*/

PLIB_TEMPLATE bool ETH_MCSRxResetIsEnabled_Unsupported( ETH_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_ETH_MCSRxResetIsEnabled");

    return false;
}


//******************************************************************************
/* Function :  ETH_RXFuncResetEnable_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_ETH_RXFuncResetEnable 

  Description:
    This template implements the Unsupported variant of the PLIB_ETH_RXFuncResetEnable function.
*/

PLIB_TEMPLATE void ETH_RXFuncResetEnable_Unsupported( ETH_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_ETH_RXFuncResetEnable");
}


//******************************************************************************
/* Function :  ETH_RXFuncResetDisable_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_ETH_RXFuncResetDisable 

  Description:
    This template implements the Unsupported variant of the PLIB_ETH_RXFuncResetDisable function.
*/

PLIB_TEMPLATE void ETH_RXFuncResetDisable_Unsupported( ETH_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_ETH_RXFuncResetDisable");
}


//******************************************************************************
/* Function :  ETH_RXFuncResetIsEnabled_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_ETH_RXFuncResetIsEnabled 

  Description:
    This template implements the Unsupported variant of the PLIB_ETH_RXFuncResetIsEnabled function.
*/

PLIB_TEMPLATE bool ETH_RXFuncResetIsEnabled_Unsupported( ETH_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_ETH_RXFuncResetIsEnabled");

    return false;
}


//******************************************************************************
/* Function :  ETH_MCSTxResetEnable_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_ETH_MCSTxResetEnable 

  Description:
    This template implements the Unsupported variant of the PLIB_ETH_MCSTxResetEnable function.
*/

PLIB_TEMPLATE void ETH_MCSTxResetEnable_Unsupported( ETH_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_ETH_MCSTxResetEnable");
}


//******************************************************************************
/* Function :  ETH_MCSTxResetDisable_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_ETH_MCSTxResetDisable 

  Description:
    This template implements the Unsupported variant of the PLIB_ETH_MCSTxResetDisable function.
*/

PLIB_TEMPLATE void ETH_MCSTxResetDisable_Unsupported( ETH_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_ETH_MCSTxResetDisable");
}


//******************************************************************************
/* Function :  ETH_MCSTxResetIsEnabled_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_ETH_MCSTxResetIsEnabled 

  Description:
    This template implements the Unsupported variant of the PLIB_ETH_MCSTxResetIsEnabled function.
*/

PLIB_TEMPLATE bool ETH_MCSTxResetIsEnabled_Unsupported( ETH_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_ETH_MCSTxResetIsEnabled");

    return false;
}


//******************************************************************************
/* Function :  ETH_TxFuncResetEnable_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_ETH_TxFuncResetEnable 

  Description:
    This template implements the Unsupported variant of the PLIB_ETH_TxFuncResetEnable function.
*/

PLIB_TEMPLATE void ETH_TxFuncResetEnable_Unsupported( ETH_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_ETH_TxFuncResetEnable");
}


//******************************************************************************
/* Function :  ETH_TxFuncResetDisable_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_ETH_TxFuncResetDisable 

  Description:
    This template implements the Unsupported variant of the PLIB_ETH_TxFuncResetDisable function.
*/

PLIB_TEMPLATE void ETH_TxFuncResetDisable_Unsupported( ETH_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_ETH_TxFuncResetDisable");
}


//******************************************************************************
/* Function :  ETH_TxFuncResetIsEnabled_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_ETH_TxFuncResetIsEnabled 

  Description:
    This template implements the Unsupported variant of the PLIB_ETH_TxFuncResetIsEnabled function.
*/

PLIB_TEMPLATE bool ETH_TxFuncResetIsEnabled_Unsupported( ETH_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_ETH_TxFuncResetIsEnabled");

    return false;
}


//******************************************************************************
/* Function :  ETH_ExistsMAC_Resets_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_ETH_ExistsMAC_Resets

  Description:
    This template implements the Unsupported variant of the PLIB_ETH_ExistsMAC_Resets function.
*/

PLIB_TEMPLATE bool ETH_ExistsMAC_Resets_Unsupported( ETH_MODULE_ID index )
{
    return false;
}


#endif /*_ETH_MAC_RESETS_UNSUPPORTED_H*/

/******************************************************************************
 End of File
*/

