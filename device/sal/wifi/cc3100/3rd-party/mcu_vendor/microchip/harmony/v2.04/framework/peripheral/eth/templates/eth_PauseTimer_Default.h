/*******************************************************************************
  ETH Peripheral Library Template Implementation

  File Name:
    eth_PauseTimer_Default.h

  Summary:
    ETH PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : PauseTimer
    and its Variant : Default
    For following APIs :
        PLIB_ETH_PauseTimerSet
        PLIB_ETH_PauseTimerGet
        PLIB_ETH_ExistsPauseTimer

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

#ifndef _ETH_PAUSETIMER_DEFAULT_H
#define _ETH_PAUSETIMER_DEFAULT_H
#include "eth_registers.h"

//******************************************************************************
/* Function :  ETH_PauseTimerSet_Default

  Summary:
    Implements Default variant of PLIB_ETH_PauseTimerSet
	This function performs atomic register access.

  Description:
    This template implements the Default variant of the PLIB_ETH_PauseTimerSet function.
*/

PLIB_TEMPLATE void ETH_PauseTimerSet_Default( ETH_MODULE_ID index , uint16_t PauseTimerValue )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));					 
	eth->ETHCON1SET = _ETHCON1_PTV_MASK & ((uint32_t)PauseTimerValue << _ETHCON1_PTV_POSITION);				 
}


//******************************************************************************
/* Function :  ETH_PauseTimerGet_Default

  Summary:
    Implements Default variant of PLIB_ETH_PauseTimerGet
	This function performs atomic register access.
	
  Description:
    This template implements the Default variant of the PLIB_ETH_PauseTimerGet function.
*/

PLIB_TEMPLATE uint16_t ETH_PauseTimerGet_Default( ETH_MODULE_ID index )
{
	volatile eth_registers_t * eth = ((eth_registers_t *)(index));					 
	return (uint16_t)eth->ETHCON1.PTV;		
}


//******************************************************************************
/* Function :  ETH_ExistsPauseTimer_Default

  Summary:
    Implements Default variant of PLIB_ETH_ExistsPauseTimer

  Description:
    This template implements the Default variant of the PLIB_ETH_ExistsPauseTimer function.
*/

#define PLIB_ETH_ExistsPauseTimer PLIB_ETH_ExistsPauseTimer
PLIB_TEMPLATE bool ETH_ExistsPauseTimer_Default( ETH_MODULE_ID index )
{
    return true;
}


#endif /*_ETH_PAUSETIMER_DEFAULT_H*/

/******************************************************************************
 End of File
*/

