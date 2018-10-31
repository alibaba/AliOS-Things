/*******************************************************************************
  ETH Peripheral Library Template Implementation

  File Name:
    eth_MAC_Resets_Default.h

  Summary:
    ETH PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : MAC_Resets
    and its Variant : Default
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
        PLIB_ETH_RxFuncResetEnable
        PLIB_ETH_RxFuncResetDisable
        PLIB_ETH_RxFuncResetIsEnabled
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

#ifndef _ETH_MAC_RESETS_DEFAULT_H
#define _ETH_MAC_RESETS_DEFAULT_H
#include "eth_registers.h"
//******************************************************************************
/* Function :  ETH_MIIResetEnable_Default

  Summary:
    Implements Default variant of PLIB_ETH_MIIResetEnable
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_MIIResetEnable function.
*/

PLIB_TEMPLATE void ETH_MIIResetEnable_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	eth->EMACxCFG1SET=_EMACxCFG1_SOFTRESET_MASK;
}


//******************************************************************************
/* Function :  ETH_MIIResetDisable_Default

  Summary:
    Implements Default variant of PLIB_ETH_MIIResetDisable
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_MIIResetDisable function.
*/

PLIB_TEMPLATE void ETH_MIIResetDisable_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	eth->EMACxCFG1.SOFTRESET=0;
}


//******************************************************************************
/* Function :  ETH_MIIResetIsEnabled_Default

  Summary:
    Implements Default variant of PLIB_ETH_MIIResetIsEnabled
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_MIIResetIsEnabled function.
*/

PLIB_TEMPLATE bool ETH_MIIResetIsEnabled_Default( ETH_MODULE_ID index )
{
	volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	return (bool) eth->EMACxCFG1.SOFTRESET;
    
}


//******************************************************************************
/* Function :  ETH_SimResetEnable_Default

  Summary:
    Implements Default variant of PLIB_ETH_SimResetEnable
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_SimResetEnable function.
*/

PLIB_TEMPLATE void ETH_SimResetEnable_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	eth->EMACxCFG1SET=_EMACxCFG1_SIMRESET_MASK;
}


//******************************************************************************
/* Function :  ETH_SimResetDisable_Default

  Summary:
    Implements Default variant of PLIB_ETH_SimResetDisable
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_SimResetDisable function.
*/

PLIB_TEMPLATE void ETH_SimResetDisable_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	eth->EMACxCFG1.SIMRESET=0;
}


//******************************************************************************
/* Function :  ETH_SimResetIsEnabled_Default

  Summary:
    Implements Default variant of PLIB_ETH_SimResetIsEnabled
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_SimResetIsEnabled function.
*/

PLIB_TEMPLATE bool ETH_SimResetIsEnabled_Default( ETH_MODULE_ID index )
{
	volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	return (bool) (eth->EMACxCFG1.SIMRESET);
}


//******************************************************************************
/* Function :  ETH_MCSRxResetEnable_Default

  Summary:
    Implements Default variant of PLIB_ETH_MCSRxResetEnable
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_MCSRxResetEnable function.
*/

PLIB_TEMPLATE void ETH_MCSRxResetEnable_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	eth->EMACxCFG1SET=_EMACxCFG1_RESETRMCS_MASK;
}


//******************************************************************************
/* Function :  ETH_MCSRxResetDisable_Default

  Summary:
    Implements Default variant of PLIB_ETH_MCSRxResetDisable
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_MCSRxResetDisable function.
*/

PLIB_TEMPLATE void ETH_MCSRxResetDisable_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	eth->EMACxCFG1.RESETRMCS=0;
}


//******************************************************************************
/* Function :  ETH_MCSRxResetIsEnabled_Default

  Summary:
    Implements Default variant of PLIB_ETH_MCSRxResetIsEnabled
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_MCSRxResetIsEnabled function.
*/

PLIB_TEMPLATE bool ETH_MCSRxResetIsEnabled_Default( ETH_MODULE_ID index )
{
	volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	return (bool) (eth->EMACxCFG1.RESETRMCS);	
}


//******************************************************************************
/* Function :  ETH_RxFuncResetEnable_Default

  Summary:
    Implements Default variant of PLIB_ETH_RxFuncResetEnable
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_RxFuncResetEnable function.
*/

PLIB_TEMPLATE void ETH_RxFuncResetEnable_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	eth->EMACxCFG1SET =_EMACxCFG1_RESETRFUN_MASK;
}


//******************************************************************************
/* Function :  ETH_RxFuncResetDisable_Default

  Summary:
    Implements Default variant of PLIB_ETH_RxFuncResetDisable
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_RxFuncResetDisable function.
*/

PLIB_TEMPLATE void ETH_RxFuncResetDisable_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	eth->EMACxCFG1.RESETRFUN = 0;

}


//******************************************************************************
/* Function :  ETH_RxFuncResetIsEnabled_Default

  Summary:
    Implements Default variant of PLIB_ETH_RxFuncResetIsEnabled
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_RxFuncResetIsEnabled function.
*/

PLIB_TEMPLATE bool ETH_RxFuncResetIsEnabled_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	return (bool)(eth->EMACxCFG1.RESETRFUN);
}


//******************************************************************************
/* Function :  ETH_MCSTxResetEnable_Default

  Summary:
    Implements Default variant of PLIB_ETH_MCSTxResetEnable
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_MCSTxResetEnable function.
*/

PLIB_TEMPLATE void ETH_MCSTxResetEnable_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	eth->EMACxCFG1SET =_EMACxCFG1_RESETTMCS_MASK;
}


//******************************************************************************
/* Function :  ETH_MCSTxResetDisable_Default

  Summary:
    Implements Default variant of PLIB_ETH_MCSTxResetDisable
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_MCSTxResetDisable function.
*/

PLIB_TEMPLATE void ETH_MCSTxResetDisable_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	eth->EMACxCFG1.RESETTMCS = 0;
}


//******************************************************************************
/* Function :  ETH_MCSTxResetIsEnabled_Default

  Summary:
    Implements Default variant of PLIB_ETH_MCSTxResetIsEnabled
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_MCSTxResetIsEnabled function.
*/

PLIB_TEMPLATE bool ETH_MCSTxResetIsEnabled_Default( ETH_MODULE_ID index )
{
	volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	return (bool) (eth->EMACxCFG1.RESETTMCS);   
}


//******************************************************************************
/* Function :  ETH_TxFuncResetEnable_Default

  Summary:
    Implements Default variant of PLIB_ETH_TxFuncResetEnable
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_TxFuncResetEnable function.
*/

PLIB_TEMPLATE void ETH_TxFuncResetEnable_Default( ETH_MODULE_ID index )
{   	
	volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	eth->EMACxCFG1SET =_EMACxCFG1_RESETTFUN_MASK;
}


//******************************************************************************
/* Function :  ETH_TxFuncResetDisable_Default

  Summary:
    Implements Default variant of PLIB_ETH_TxFuncResetDisable
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_TxFuncResetDisable function.
*/

PLIB_TEMPLATE void ETH_TxFuncResetDisable_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	eth->EMACxCFG1.RESETTFUN = 0;
}


//******************************************************************************
/* Function :  ETH_TxFuncResetIsEnabled_Default

  Summary:
    Implements Default variant of PLIB_ETH_TxFuncResetIsEnabled
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_TxFuncResetIsEnabled function.
*/

PLIB_TEMPLATE bool ETH_TxFuncResetIsEnabled_Default( ETH_MODULE_ID index )
{
	volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	return (bool)(eth->EMACxCFG1.RESETTFUN);    
}


//******************************************************************************
/* Function :  ETH_ExistsMAC_Resets_Default

  Summary:
    Implements Default variant of PLIB_ETH_ExistsMAC_Resets
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_ExistsMAC_Resets function.
*/

#define PLIB_ETH_ExistsMAC_Resets PLIB_ETH_ExistsMAC_Resets
PLIB_TEMPLATE bool ETH_ExistsMAC_Resets_Default( ETH_MODULE_ID index )
{
    return true;
}


#endif /*_ETH_MAC_RESETS_DEFAULT_H*/

/******************************************************************************
 End of File
*/

