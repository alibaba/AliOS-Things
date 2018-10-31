/*******************************************************************************
  ETH Peripheral Library Template Implementation

  File Name:
    eth_MAC_Testing_Default.h

  Summary:
    ETH PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : MAC_Testing
    and its Variant : Default
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

#ifndef _ETH_MAC_TESTING_DEFAULT_H
#define _ETH_MAC_TESTING_DEFAULT_H
#include "eth_registers.h"
//******************************************************************************
/* Function :  ETH_TestBackPressEnable_Default

  Summary:
    Implements Default variant of PLIB_ETH_TestBackPressEnable
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_TestBackPressEnable function.
*/

PLIB_TEMPLATE void ETH_TestBackPressEnable_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	eth->EMACxTESTSET=_EMACxTEST_TESTBP_MASK;
}


//******************************************************************************
/* Function :  ETH_TestBackPressDisable_Default

  Summary:
    Implements Default variant of PLIB_ETH_TestBackPressDisable
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_TestBackPressDisable function.
*/

PLIB_TEMPLATE void ETH_TestBackPressDisable_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	eth->EMACxTESTCLR=_EMACxTEST_TESTBP_MASK;
}


//******************************************************************************
/* Function :  ETH_TestBackPressIsEnabled_Default

  Summary:
    Implements Default variant of PLIB_ETH_TestBackPressIsEnabled
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_TestBackPressIsEnabled function.
*/

PLIB_TEMPLATE bool ETH_TestBackPressIsEnabled_Default( ETH_MODULE_ID index )
{
	volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	return (bool)(eth->EMACxTEST.TESTBP);    
}


//******************************************************************************
/* Function :  ETH_TestPauseEnable_Default

  Summary:
    Implements Default variant of PLIB_ETH_TestPauseEnable
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_TestPauseEnable function.
*/

PLIB_TEMPLATE void ETH_TestPauseEnable_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	eth->EMACxTESTSET=_EMACxTEST_TESTPAUSE_MASK;
}


//******************************************************************************
/* Function :  ETH_TestPauseDisable_Default

  Summary:
    Implements Default variant of PLIB_ETH_TestPauseDisable
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_TestPauseDisable function.
*/

PLIB_TEMPLATE void ETH_TestPauseDisable_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	eth->EMACxTESTCLR=_EMACxTEST_TESTPAUSE_MASK;
}


//******************************************************************************
/* Function :  ETH_TestPauseIsEnabled_Default

  Summary:
    Implements Default variant of PLIB_ETH_TestPauseIsEnabled
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_TestPauseIsEnabled function.
*/

PLIB_TEMPLATE bool ETH_TestPauseIsEnabled_Default( ETH_MODULE_ID index )
{
	volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	return (bool)(eth->EMACxTEST.TESTPAUSE);    
}


//******************************************************************************
/* Function :  ETH_ShortcutQuantaEnable_Default

  Summary:
    Implements Default variant of PLIB_ETH_ShortcutQuantaEnable
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_ShortcutQuantaEnable function.
*/

PLIB_TEMPLATE void ETH_ShortcutQuantaEnable_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	eth->EMACxTESTSET=_EMACxTEST_SHRTQNTA_MASK;
}


//******************************************************************************
/* Function :  ETH_ShortcutQuantaDisable_Default

  Summary:
    Implements Default variant of PLIB_ETH_ShortcutQuantaDisable
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_ShortcutQuantaDisable function.
*/

PLIB_TEMPLATE void ETH_ShortcutQuantaDisable_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	eth->EMACxTESTCLR=_EMACxTEST_SHRTQNTA_MASK;
}


//******************************************************************************
/* Function :  ETH_ShortcutQuantaIsEnabled_Default

  Summary:
    Implements Default variant of PLIB_ETH_ShortcutQuantaIsEnabled
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_ShortcutQuantaIsEnabled function.
*/

PLIB_TEMPLATE bool ETH_ShortcutQuantaIsEnabled_Default( ETH_MODULE_ID index )
{
	volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	return (bool)(eth->EMACxTEST.SHRTQNTA);
}


//******************************************************************************
/* Function :  ETH_ExistsMAC_Testing_Default

  Summary:
    Implements Default variant of PLIB_ETH_ExistsMAC_Testing

  Description:
    This template implements the Default variant of the PLIB_ETH_ExistsMAC_Testing function.
*/

#define PLIB_ETH_ExistsMAC_Testing PLIB_ETH_ExistsMAC_Testing
PLIB_TEMPLATE bool ETH_ExistsMAC_Testing_Default( ETH_MODULE_ID index )
{
    return true;
}


#endif /*_ETH_MAC_TESTING_DEFAULT_H*/

/******************************************************************************
 End of File
*/

