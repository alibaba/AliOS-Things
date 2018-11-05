/*******************************************************************************
  ETH Peripheral Library Template Implementation

  File Name:
    eth_RMII_Support_Default.h

  Summary:
    ETH PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : RMII_Support
    and its Variant : Default
    For following APIs :
        PLIB_ETH_RMIIResetEnable
        PLIB_ETH_RMIIResetDisable
        PLIB_ETH_RMIIResetIsEnabled
        PLIB_ETH_RMIISpeedGet
        PLIB_ETH_RMIISpeedSet
        PLIB_ETH_ExistsRMII_Support

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

#ifndef _ETH_RMII_SUPPORT_DEFAULT_H
#define _ETH_RMII_SUPPORT_DEFAULT_H
#include "eth_registers.h"
//******************************************************************************
/* Function :  ETH_RMIIResetEnable_Default

  Summary:
    Implements Default variant of PLIB_ETH_RMIIResetEnable
	This function performs atomic register access.

  Description:
    This template implements the Default variant of the PLIB_ETH_RMIIResetEnable function.
*/

PLIB_TEMPLATE void ETH_RMIIResetEnable_Default( ETH_MODULE_ID index )
{   	
	volatile eth_registers_t * eth = ((eth_registers_t *)(index));	
	eth->EMACxSUPPSET = _EMACxSUPP_RESETRMII_MASK; 
}


//******************************************************************************
/* Function :  ETH_RMIIResetDisable_Default

  Summary:
    Implements Default variant of PLIB_ETH_RMIIResetDisable
	This function performs atomic register access.

  Description:
    This template implements the Default variant of the PLIB_ETH_RMIIResetDisable function.
*/

PLIB_TEMPLATE void ETH_RMIIResetDisable_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));	
	eth->EMACxSUPP.RESETRMII = 0ul;
}


//******************************************************************************
/* Function :  ETH_RMIIResetIsEnabled_Default

  Summary:
    Implements Default variant of PLIB_ETH_RMIIResetIsEnabled
	This function performs atomic register access.

  Description:
    This template implements the Default variant of the PLIB_ETH_RMIIResetIsEnabled function.
*/

PLIB_TEMPLATE bool ETH_RMIIResetIsEnabled_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));	
	return (bool) eth->EMACxSUPP.RESETRMII; 
}


//******************************************************************************
/* Function :  ETH_RMIISpeedGet_Default

  Summary:
    Implements Default variant of PLIB_ETH_RMIISpeedGet
	This function performs atomic register access.

  Description:
    This template implements the Default variant of the PLIB_ETH_RMIISpeedGet function.
*/

PLIB_TEMPLATE ETH_RMII_SPEED ETH_RMIISpeedGet_Default( ETH_MODULE_ID index )
{
	volatile eth_registers_t * eth = ((eth_registers_t *)(index));	
	return (ETH_RMII_SPEED) eth->EMACxSUPP.SPEEDRMII; 	
}


//******************************************************************************
/* Function :  ETH_RMIISpeedSet_Default

  Summary:
    Implements Default variant of PLIB_ETH_RMIISpeedSet
	This function performs atomic register access.

  Description:
    This template implements the Default variant of the PLIB_ETH_RMIISpeedSet function.
*/

PLIB_TEMPLATE void ETH_RMIISpeedSet_Default( ETH_MODULE_ID index , ETH_RMII_SPEED RMIISpeed )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));	
	eth->EMACxSUPP.SPEEDRMII = RMIISpeed ;	
}


//******************************************************************************
/* Function :  ETH_ExistsRMII_Support_Default

  Summary:
    Implements Default variant of PLIB_ETH_ExistsRMII_Support

  Description:
    This template implements the Default variant of the PLIB_ETH_ExistsRMII_Support function.
*/

#define PLIB_ETH_ExistsRMII_Support PLIB_ETH_ExistsRMII_Support
PLIB_TEMPLATE bool ETH_ExistsRMII_Support_Default( ETH_MODULE_ID index )
{
    return true;
}


#endif /*_ETH_RMII_SUPPORT_DEFAULT_H*/

/******************************************************************************
 End of File
*/

