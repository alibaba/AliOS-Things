/*******************************************************************************
  ETH Peripheral Library Template Implementation

  File Name:
    eth_InterPacketGaps_Default.h

  Summary:
    ETH PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : InterPacketGaps
    and its Variant : Default
    For following APIs :
        PLIB_ETH_BackToBackIPGGet
        PLIB_ETH_BackToBackIPGSet
        PLIB_ETH_NonBackToBackIPG1Get
        PLIB_ETH_NonBackToBackIPG1Set
        PLIB_ETH_NonBackToBackIPG2Get
        PLIB_ETH_NonBackToBackIPG2Set
        PLIB_ETH_ExistsInterPacketGaps

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

#ifndef _ETH_INTERPACKETGAPS_DEFAULT_H
#define _ETH_INTERPACKETGAPS_DEFAULT_H
#include "eth_registers.h"

//******************************************************************************
/* Function :  ETH_BackToBackIPGGet_Default

  Summary:
    Implements Default variant of PLIB_ETH_BackToBackIPGGet
	This function performs atomic register access.

  Description:
    This template implements the Default variant of the PLIB_ETH_BackToBackIPGGet function.
*/

PLIB_TEMPLATE uint8_t ETH_BackToBackIPGGet_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	return (uint8_t) eth->EMACxIPGT.B2BIPKTGP;
}


//******************************************************************************
/* Function :  ETH_BackToBackIPGSet_Default

  Summary:
    Implements Default variant of PLIB_ETH_BackToBackIPGSet
	This function performs atomic register access.

  Description:
    This template implements the Default variant of the PLIB_ETH_BackToBackIPGSet function.
*/

PLIB_TEMPLATE void ETH_BackToBackIPGSet_Default( ETH_MODULE_ID index , uint8_t backToBackIPGValue )
{
	volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	
	eth->EMACxIPGTCLR = _EMAC1IPGT_B2BIPKTGP_MASK;
	eth->EMACxIPGTSET = _EMAC1IPGT_B2BIPKTGP_MASK & ((uint32_t)backToBackIPGValue << _EMAC1IPGT_B2BIPKTGP_POSITION);	 
					 
}


//******************************************************************************
/* Function :  ETH_NonBackToBackIPG1Get_Default

  Summary:
    Implements Default variant of PLIB_ETH_NonBackToBackIPG1Get
	This function performs atomic register access.

  Description:
    This template implements the Default variant of the PLIB_ETH_NonBackToBackIPG1Get function.
*/

PLIB_TEMPLATE uint8_t ETH_NonBackToBackIPG1Get_Default( ETH_MODULE_ID index )
{
	volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	return (uint8_t) eth->EMACxIPGR.NB2BIPKTGP1;
}


//******************************************************************************
/* Function :  ETH_NonBackToBackIPG1Set_Default

  Summary:
    Implements Default variant of PLIB_ETH_NonBackToBackIPG1Set
	This function performs atomic register access.

  Description:
    This template implements the Default variant of the PLIB_ETH_NonBackToBackIPG1Set function.
*/

PLIB_TEMPLATE void ETH_NonBackToBackIPG1Set_Default( ETH_MODULE_ID index , uint8_t nonBackToBackIPGValue )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));	
	eth->EMACxIPGRCLR = _EMACxIPGR_NB2BIPKTGP1_MASK;
	eth->EMACxIPGRSET = _EMACxIPGR_NB2BIPKTGP1_MASK & ((uint32_t)nonBackToBackIPGValue << _EMACxIPGR_NB2BIPKTGP1_POSITION);	
}


//******************************************************************************
/* Function :  ETH_NonBackToBackIPG2Get_Default

  Summary:
    Implements Default variant of PLIB_ETH_NonBackToBackIPG2Get
	This function performs atomic register access.

  Description:
    This template implements the Default variant of the PLIB_ETH_NonBackToBackIPG2Get function.
*/

PLIB_TEMPLATE uint8_t ETH_NonBackToBackIPG2Get_Default( ETH_MODULE_ID index )
{	
	volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	return (uint8_t) eth->EMACxIPGR.NB2BIPKTGP2;
}


//******************************************************************************
/* Function :  ETH_NonBackToBackIPG2Set_Default

  Summary:
    Implements Default variant of PLIB_ETH_NonBackToBackIPG2Set
	This function performs atomic register access.

  Description:
    This template implements the Default variant of the PLIB_ETH_NonBackToBackIPG2Set function.
*/

PLIB_TEMPLATE void ETH_NonBackToBackIPG2Set_Default( ETH_MODULE_ID index , uint8_t nonBackToBackIPGValue )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	eth->EMACxIPGRCLR = _EMACxIPGR_NB2BIPKTGP2_MASK;
	eth->EMACxIPGRSET = _EMACxIPGR_NB2BIPKTGP2_MASK & ((uint32_t)nonBackToBackIPGValue <<_EMACxIPGR_NB2BIPKTGP2_POSITION);	
}


//******************************************************************************
/* Function :  ETH_ExistsInterPacketGaps_Default

  Summary:
    Implements Default variant of PLIB_ETH_ExistsInterPacketGaps

  Description:
    This template implements the Default variant of the PLIB_ETH_ExistsInterPacketGaps function.
*/

#define PLIB_ETH_ExistsInterPacketGaps PLIB_ETH_ExistsInterPacketGaps
PLIB_TEMPLATE bool ETH_ExistsInterPacketGaps_Default( ETH_MODULE_ID index )
{
    return true;
}


#endif /*_ETH_INTERPACKETGAPS_DEFAULT_H*/

/******************************************************************************
 End of File
*/

