/*******************************************************************************
  ETH Peripheral Library Template Implementation

  File Name:
    eth_Interrupt_Default.h

  Summary:
    ETH PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : Interrupt
    and its Variant : Default
    For following APIs :
        PLIB_ETH_InterruptSourceEnable
        PLIB_ETH_InterruptSourceDisable
        PLIB_ETH_InterruptSourceIsEnabled
        PLIB_ETH_InterruptSourcesGet
        PLIB_ETH_InterruptSet
        PLIB_ETH_InterruptClear
        PLIB_ETH_InterruptsGet
        PLIB_ETH_InterruptStatusGet
        PLIB_ETH_ExistsInterrupt

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

#ifndef _ETH_INTERRUPT_DEFAULT_H
#define _ETH_INTERRUPT_DEFAULT_H
#include "eth_registers.h"

//******************************************************************************
/* Function :  ETH_InterruptSourceEnable_Default

  Summary:
    Implements Default variant of PLIB_ETH_InterruptSourceEnable
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_InterruptSourceEnable function.
*/

PLIB_TEMPLATE void ETH_InterruptSourceEnable_Default( ETH_MODULE_ID index , ETH_INTERRUPT_SOURCES intmask )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));
	eth->ETHIENSET = _ETHIEN_w_MASK & ((uint32_t)intmask <<_ETHIEN_w_POSITION);
}
//******************************************************************************
/* Function :  ETH_EventsEnableSet_Default

  Summary:
    Implements Default variant of ETH_EventsEnableSet
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the ETH_EventsEnableSet function.
*/
PLIB_TEMPLATE void ETH_EventsEnableSet_Default(ETH_MODULE_ID index,PLIB_ETH_EVENTS eEvents)
{
    ETH_InterruptSourceEnable_Default(index,eEvents);
}

//******************************************************************************
/* Function :  ETH_InterruptSourceDisable_Default

  Summary:
    Implements Default variant of PLIB_ETH_InterruptSourceDisable
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_InterruptSourceDisable function.
*/

PLIB_TEMPLATE void ETH_InterruptSourceDisable_Default( ETH_MODULE_ID index , ETH_INTERRUPT_SOURCES intmask )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));
	eth->ETHIENCLR = _ETHIEN_w_MASK & ((uint32_t)intmask <<_ETHIEN_w_POSITION);
}

//******************************************************************************
/* Function :  ETH_EventsEnableClr_Default

  Summary:
    Implements Default variant of ETH_EventsEnableClr

  Description:
    This template implements the Default variant of the ETH_EventsEnableClr function.

*/

PLIB_TEMPLATE void ETH_EventsEnableClr_Default(ETH_MODULE_ID index,PLIB_ETH_EVENTS eEvents)
{
   ETH_InterruptSourceDisable_Default(index,eEvents);
}

//******************************************************************************
/* Function :  ETH_EventsEnableWrite_Default

  Summary:
    Implements Default variant of ETH_EventsEnableWrite

  Description:
    This template implements the Default variant of the ETH_EventsEnableWrite function.
*/
PLIB_TEMPLATE void ETH_EventsEnableWrite_Default(ETH_MODULE_ID index,PLIB_ETH_EVENTS eEvents)
{
    ETH_InterruptSourceEnable_Default(index,eEvents);
    ETH_InterruptSourceDisable_Default(index,~eEvents);
}

//******************************************************************************
/* Function :  ETH_InterruptSourceIsEnabled_Default

  Summary:
    Implements Default variant of PLIB_ETH_InterruptSourceIsEnabled
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_InterruptSourceIsEnabled function.
*/

PLIB_TEMPLATE bool ETH_InterruptSourceIsEnabled_Default( ETH_MODULE_ID index , ETH_INTERRUPT_SOURCES intmask )
{
	volatile eth_registers_t * eth = ((eth_registers_t *)(index));
	return ((eth->ETHIEN.w & (uint32_t)intmask ) > 0 ? true : false);	
}


//******************************************************************************
/* Function :  ETH_InterruptSourcesGet_Default

  Summary:
    Implements Default variant of PLIB_ETH_InterruptSourcesGet
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_InterruptSourcesGet function.
*/

PLIB_TEMPLATE ETH_INTERRUPT_SOURCES ETH_InterruptSourcesGet_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));
	return (ETH_INTERRUPT_SOURCES)eth->ETHIEN.w;
}


//******************************************************************************
/* Function :  ETH_EventsEnableGet_Default

  Summary:
    Implements Default variant of ETH_EventsEnableGet

  Description:
    This template implements the Default variant of the ETH_EventsEnableGet function.
*/
PLIB_TEMPLATE PLIB_ETH_EVENTS ETH_EventsEnableGet_Default(ETH_MODULE_ID index)
{
    return ETH_InterruptSourcesGet_Default(index);
}


//******************************************************************************
/* Function :  ETH_InterruptSet_Default

  Summary:
    Implements Default variant of PLIB_ETH_InterruptSet
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_InterruptSet function.
*/

PLIB_TEMPLATE void ETH_InterruptSet_Default( ETH_MODULE_ID index , ETH_INTERRUPT_SOURCES intmask )
{
	volatile eth_registers_t * eth = ((eth_registers_t *)(index));
	eth->ETHIRQSET = _ETHIRQ_w_MASK & ((uint32_t)intmask << _ETHIRQ_w_POSITION);	
}


//******************************************************************************
/* Function :  ETH_InterruptClear_Default

  Summary:
    Implements Default variant of PLIB_ETH_InterruptClear
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_InterruptClear function.
*/

PLIB_TEMPLATE void ETH_InterruptClear_Default( ETH_MODULE_ID index , ETH_INTERRUPT_SOURCES intmask )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));
	eth->ETHIRQCLR = _ETHIRQ_w_MASK & ((uint32_t)intmask << _ETHIRQ_w_POSITION);
}

//******************************************************************************
/* Function :  ETH_EventsClr_Default

  Summary:
    Implements Default variant of ETH_EventsClr

  Description:
    This template implements the Default variant of the ETH_EventsClr function.
*/
PLIB_TEMPLATE void ETH_EventsClr_Default(ETH_MODULE_ID index,PLIB_ETH_EVENTS eEvents)
{
    ETH_InterruptClear_Default(index,eEvents);
}

//******************************************************************************
/* Function :  ETH_InterruptsGet_Default

  Summary:
    Implements Default variant of PLIB_ETH_InterruptsGet.
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_InterruptsGet function.
*/

PLIB_TEMPLATE ETH_INTERRUPT_SOURCES ETH_InterruptsGet_Default( ETH_MODULE_ID index )
{
	volatile eth_registers_t * eth = ((eth_registers_t *)(index));
	return  (ETH_INTERRUPT_SOURCES) eth->ETHIRQ.w;
}

//******************************************************************************
/* Function :  ETH_EventsGet_Default

  Summary:
    Implements Default variant of ETH_EventsGet

  Description:
    This template implements the Default variant of the ETH_EventsGet function.
*/
PLIB_TEMPLATE PLIB_ETH_EVENTS ETH_EventsGet_Default(ETH_MODULE_ID index)
{
    return ETH_InterruptsGet_Default(index);
}

//******************************************************************************
/* Function :  ETH_InterruptStatusGet_Default

  Summary:
    Implements Default variant of PLIB_ETH_InterruptStatusGet
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_InterruptStatusGet function.
*/

PLIB_TEMPLATE bool ETH_InterruptStatusGet_Default( ETH_MODULE_ID index , ETH_INTERRUPT_SOURCES intmask )
{
	volatile eth_registers_t * eth = ((eth_registers_t *)(index));
	return ((eth->ETHIRQ.w & ((uint32_t) intmask )) > 0 ? true : false);	
}


//******************************************************************************
/* Function :  ETH_ExistsInterrupt_Default

  Summary:
    Implements Default variant of PLIB_ETH_ExistsInterrupt

  Description:
    This template implements the Default variant of the PLIB_ETH_ExistsInterrupt function.
*/

#define PLIB_ETH_ExistsInterrupt PLIB_ETH_ExistsInterrupt
PLIB_TEMPLATE bool ETH_ExistsInterrupt_Default( ETH_MODULE_ID index )
{
    return true;
}


#endif /*_ETH_INTERRUPT_DEFAULT_H*/

/******************************************************************************
 End of File
*/

