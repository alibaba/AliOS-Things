/*******************************************************************************
  ETH Peripheral Library Template Implementation

  File Name:
    eth_AutoFlowControl_Unsupported.h

  Summary:
    ETH PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : AutoFlowControl
    and its Variant : Unsupported
    For following APIs :
        PLIB_ETH_AutoFlowControlEnable
        PLIB_ETH_AutoFlowControlDisable
        PLIB_ETH_AutoFlowControlIsEnabled
        PLIB_ETH_ExistsAutoFlowControl

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

#ifndef _ETH_AUTOFLOWCONTROL_UNSUPPORTED_H
#define _ETH_AUTOFLOWCONTROL_UNSUPPORTED_H

//******************************************************************************
/* Function :  ETH_AutoFlowControlEnable_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_ETH_AutoFlowControlEnable 

  Description:
    This template implements the Unsupported variant of the PLIB_ETH_AutoFlowControlEnable function.
*/

PLIB_TEMPLATE void ETH_AutoFlowControlEnable_Unsupported( ETH_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_ETH_AutoFlowControlEnable");
}


//******************************************************************************
/* Function :  ETH_AutoFlowControlDisable_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_ETH_AutoFlowControlDisable 

  Description:
    This template implements the Unsupported variant of the PLIB_ETH_AutoFlowControlDisable function.
*/

PLIB_TEMPLATE void ETH_AutoFlowControlDisable_Unsupported( ETH_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_ETH_AutoFlowControlDisable");
}


//******************************************************************************
/* Function :  ETH_AutoFlowControlIsEnabled_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_ETH_AutoFlowControlIsEnabled 

  Description:
    This template implements the Unsupported variant of the PLIB_ETH_AutoFlowControlIsEnabled function.
*/

PLIB_TEMPLATE bool ETH_AutoFlowControlIsEnabled_Unsupported( ETH_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_ETH_AutoFlowControlIsEnabled");

    return false;
}


//******************************************************************************
/* Function :  ETH_ExistsAutoFlowControl_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_ETH_ExistsAutoFlowControl

  Description:
    This template implements the Unsupported variant of the PLIB_ETH_ExistsAutoFlowControl function.
*/

PLIB_TEMPLATE bool ETH_ExistsAutoFlowControl_Unsupported( ETH_MODULE_ID index )
{
    return false;
}


#endif /*_ETH_AUTOFLOWCONTROL_UNSUPPORTED_H*/

/******************************************************************************
 End of File
*/

