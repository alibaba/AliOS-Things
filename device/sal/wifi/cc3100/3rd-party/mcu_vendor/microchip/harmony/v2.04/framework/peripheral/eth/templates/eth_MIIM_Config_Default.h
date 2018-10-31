/*******************************************************************************
  ETH Peripheral Library Template Implementation

  File Name:
    eth_MIIM_Config_Default.h

  Summary:
    ETH PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : MIIM_Config
    and its Variant : Default
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

#ifndef _ETH_MIIM_CONFIG_DEFAULT_H
#define _ETH_MIIM_CONFIG_DEFAULT_H
#include "eth_registers.h"
//******************************************************************************
/* Function :  ETH_MIIMResetEnable_Default

  Summary:
    Implements Default variant of PLIB_ETH_MIIMResetEnable
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_MIIMResetEnable function.
*/

PLIB_TEMPLATE void ETH_MIIMResetEnable_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	eth->EMACxMCFGSET = _EMACxMCFG_RESETMGMT_MASK; 
}


//******************************************************************************
/* Function :  ETH_MIIMResetDisable_Default

  Summary:
    Implements Default variant of PLIB_ETH_MIIMResetDisable
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_MIIMResetDisable function.
*/

PLIB_TEMPLATE void ETH_MIIMResetDisable_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	eth->EMACxMCFGCLR = _EMACxMCFG_RESETMGMT_MASK; 
}


//******************************************************************************
/* Function :  ETH_MIIMResetIsEnabled_Default

  Summary:
    Implements Default variant of PLIB_ETH_MIIMResetIsEnabled
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_MIIMResetIsEnabled function.
*/

PLIB_TEMPLATE bool ETH_MIIMResetIsEnabled_Default( ETH_MODULE_ID index )
{
	volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	return (bool)(eth->EMACxMCFG.RESETMGMT);    
}


//******************************************************************************
/* Function :  ETH_MIIMClockGet_Default

  Summary:
    Implements Default variant of PLIB_ETH_MIIMClockGet
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_MIIMClockGet function.
*/

PLIB_TEMPLATE ETH_MIIM_CLK ETH_MIIMClockGet_Default( ETH_MODULE_ID index )
{
	volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	return (ETH_MIIM_CLK)(eth->EMACxMCFG.CLKSEL); 
}


//******************************************************************************
/* Function :  ETH_MIIMClockSet_Default

  Summary:
    Implements Default variant of PLIB_ETH_MIIMClockSet
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_MIIMClockSet function.
*/

PLIB_TEMPLATE void ETH_MIIMClockSet_Default( ETH_MODULE_ID index , ETH_MIIM_CLK MIIMClock )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));	
	eth->EMACxMCFG.CLKSEL = MIIMClock ;
}


//******************************************************************************
/* Function :  ETH_MIIMNoPreEnable_Default

  Summary:
    Implements Default variant of PLIB_ETH_MIIMNoPreEnable
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_MIIMNoPreEnable function.
*/

PLIB_TEMPLATE void ETH_MIIMNoPreEnable_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	eth->EMACxMCFGSET = _EMACxMCFG_NOPRE_MASK;	
}

//******************************************************************************
/* Function :  ETH_MIIMNoPreDisable_Default

  Summary:
    Implements Default variant of PLIB_ETH_MIIMNoPreDisable
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_MIIMNoPreDisable function.
*/

PLIB_TEMPLATE void ETH_MIIMNoPreDisable_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	eth->EMACxMCFG.NOPRE=0;	
}


//******************************************************************************
/* Function :  ETH_MIIMNoPreIsEnabled_Default

  Summary:
    Implements Default variant of PLIB_ETH_MIIMNoPreIsEnabled
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_MIIMNoPreIsEnabled function.
*/

PLIB_TEMPLATE bool ETH_MIIMNoPreIsEnabled_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	return (bool)(eth->EMACxMCFG.NOPRE);
}


//******************************************************************************
/* Function :  ETH_MIIMScanIncrEnable_Default

  Summary:
    Implements Default variant of PLIB_ETH_MIIMScanIncrEnable
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_MIIMScanIncrEnable function.
*/

PLIB_TEMPLATE void ETH_MIIMScanIncrEnable_Default( ETH_MODULE_ID index )
{    
	volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	eth->EMACxMCFGSET = _EMACxMCFG_SCANINC_MASK;		
}


//******************************************************************************
/* Function :  ETH_MIIMScanIncrDisable_Default

  Summary:
    Implements Default variant of PLIB_ETH_MIIMScanIncrDisable
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_MIIMScanIncrDisable function.
*/

PLIB_TEMPLATE void ETH_MIIMScanIncrDisable_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	eth->EMACxMCFG.SCANINC=0;	
}


//******************************************************************************
/* Function :  ETH_MIIMScanIncrIsEnabled_Default

  Summary:
    Implements Default variant of PLIB_ETH_MIIMScanIncrIsEnabled
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_MIIMScanIncrIsEnabled function.
*/

PLIB_TEMPLATE bool ETH_MIIMScanIncrIsEnabled_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	return (bool)(eth->EMACxMCFG.SCANINC);	
}


//******************************************************************************
/* Function :  ETH_ExistsMIIM_Config_Default

  Summary:
    Implements Default variant of PLIB_ETH_ExistsMIIM_Config

  Description:
    This template implements the Default variant of the PLIB_ETH_ExistsMIIM_Config function.
*/

#define PLIB_ETH_ExistsMIIM_Config PLIB_ETH_ExistsMIIM_Config
PLIB_TEMPLATE bool ETH_ExistsMIIM_Config_Default( ETH_MODULE_ID index )
{
    return true;
}

#endif /*_ETH_MIIM_CONFIG_DEFAULT_H*/

/******************************************************************************
 End of File
*/

