/*******************************************************************************
  ETH Peripheral Library Template Implementation

  File Name:
    eth_PatternMatch_Default.h

  Summary:
    ETH PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : PatternMatch
    and its Variant : Default
    For following APIs :
        PLIB_ETH_PatternMatchSet
        PLIB_ETH_PatternMatchGet
        PLIB_ETH_PatternMatchChecksumSet
        PLIB_ETH_PatternMatchChecksumGet
        PLIB_ETH_PatternMatchOffsetSet
        PLIB_ETH_PatternMatchOffsetGet
        PLIB_ETH_PatternMatchModeSet
        PLIB_ETH_PatternMatchModeGet
        PLIB_ETH_ExistsPatternMatch

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

#ifndef _ETH_PATTERNMATCH_DEFAULT_H
#define _ETH_PATTERNMATCH_DEFAULT_H
#include "eth_registers.h"
#include "eth_ReceiveFilters_Default.h"

//******************************************************************************
/* Function :  ETH_PatternMatchSet_Default

  Summary:
    Implements Default variant of PLIB_ETH_PatternMatchSet
	This function performs atomic register access.

  Description:
    This template implements the Default variant of the PLIB_ETH_PatternMatchSet function.
*/

PLIB_TEMPLATE void ETH_PatternMatchSet_Default( ETH_MODULE_ID index , uint64_t patternMatchMaskValue )
{
	volatile eth_registers_t * eth = ((eth_registers_t *)(index));	
	 
    PLIB_ASSERT(
        !(bool)(eth->ETHCON1.RXEN) ||(eth->ETHRXFC.PMMODE == 0),
        "Receive must be disabled or pattern match mode disabled!");
				
	eth->ETHPMM0SET = _ETHPMM0_PMMLOWER_MASK&((uint32_t)patternMatchMaskValue);
	
	eth->ETHPMM1SET =	_ETHPMM1_PMMUPPER_MASK&((uint32_t)(patternMatchMaskValue>>32));    
}


//******************************************************************************
/* Function :  ETH_PatternMatchGet_Default

  Summary:
    Implements Default variant of PLIB_ETH_PatternMatchGet

  Description:
    This template implements the Default variant of the PLIB_ETH_PatternMatchGet function.
*/

PLIB_TEMPLATE uint64_t ETH_PatternMatchGet_Default( ETH_MODULE_ID index )
{
	volatile eth_registers_t * eth = ((eth_registers_t *)(index));	
	return ((uint64_t)eth->ETHPMM0.PMMLOWER + ( (uint64_t)eth->ETHPMM1.PMMUPPER<<32));
}


//******************************************************************************
/* Function :  ETH_PatternMatchChecksumSet_Default

  Summary:
    Implements Default variant of PLIB_ETH_PatternMatchChecksumSet
	This function performs atomic register access.

  Description:
    This template implements the Default variant of the PLIB_ETH_PatternMatchChecksumSet function.
*/

PLIB_TEMPLATE void ETH_PatternMatchChecksumSet_Default( ETH_MODULE_ID index , uint16_t PatternMatchChecksumValue )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));	
	 
    PLIB_ASSERT(
        !(bool)(eth->ETHCON1.RXEN) ||(eth->ETHRXFC.PMMODE == 0),
        "Receive must be disabled or pattern match mode disabled!");
	    eth->ETHPMCSSET = _ETHPMCS_PMCS_MASK & ((uint32_t)PatternMatchChecksumValue << _ETHPMCS_PMCS_POSITION);							
}


//******************************************************************************
/* Function :  ETH_PatternMatchChecksumGet_Default

  Summary:
    Implements Default variant of PLIB_ETH_PatternMatchChecksumGet

  Description:
    This template implements the Default variant of the PLIB_ETH_PatternMatchChecksumGet function.
*/

PLIB_TEMPLATE uint16_t ETH_PatternMatchChecksumGet_Default( ETH_MODULE_ID index )
{
	volatile eth_registers_t * eth = ((eth_registers_t *)(index));	
    return (uint16_t)eth->ETHPMCS.PMCS;
}


//******************************************************************************
/* Function :  ETH_PatternMatchOffsetSet_Default

  Summary:
    Implements Default variant of PLIB_ETH_PatternMatchOffsetSet
	This function performs atomic register access.

  Description:
    This template implements the Default variant of the PLIB_ETH_PatternMatchOffsetSet function.
*/

PLIB_TEMPLATE void ETH_PatternMatchOffsetSet_Default( ETH_MODULE_ID index , uint16_t PatternMatchOffsetValue )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));	
	 
    PLIB_ASSERT(!(bool)(eth->ETHCON1.RXEN) ||(eth->ETHRXFC.PMMODE == 0),
        "Receive must be disabled or pattern match mode disabled!");
		
	eth->ETHPMOSET = _ETHPMO_PMO_MASK & ((uint32_t)PatternMatchOffsetValue << _ETHPMO_PMO_POSITION );    
}


//******************************************************************************
/* Function :  ETH_PatternMatchOffsetGet_Default

  Summary:
    Implements Default variant of PLIB_ETH_PatternMatchOffsetGet

  Description:
    This template implements the Default variant of the PLIB_ETH_PatternMatchOffsetGet function.
*/

PLIB_TEMPLATE uint16_t ETH_PatternMatchOffsetGet_Default( ETH_MODULE_ID index )
{
	volatile eth_registers_t * eth = ((eth_registers_t *)(index));	
    return (uint16_t)eth->ETHPMO.PMO;
}


//******************************************************************************
/* Function :  ETH_PatternMatchModeSet_Default

  Summary:
    Implements Default variant of PLIB_ETH_PatternMatchModeSet
	This function performs atomic register access.

  Description:
    This template implements the Default variant of the PLIB_ETH_PatternMatchModeSet function.
*/

PLIB_TEMPLATE void ETH_PatternMatchModeSet_Default( ETH_MODULE_ID index , ETH_PATTERN_MATCH_MODE modeSel )
{
	volatile eth_registers_t * eth = ((eth_registers_t *)(index));	
	
    PLIB_ASSERT(!(bool)(eth->ETHCON1.RXEN),"Receive must be disabled"); 			
	eth->ETHRXFCSET = _ETHRXFC_PMMODE_MASK & ((uint32_t) modeSel << _ETHRXFC_PMMODE_POSITION); 
}

//******************************************************************************
/* Function :  ETH_RxFiltersPMClr_Default

  Summary:
    Implements Default variant of ETH_RxFiltersPMClr

  Description:
    This template implements the Default variant of the ETH_RxFiltersPMClr function.
*/
PLIB_TEMPLATE void ETH_RxFiltersPMClr_Default(ETH_MODULE_ID index)
{
    ETH_PatternMatchModeSet_Default(index,0);
}
//******************************************************************************
/* Function :  ETH_PatternMatchModeGet_Default

  Summary:
    Implements Default variant of PLIB_ETH_PatternMatchModeGet

  Description:
    This template implements the Default variant of the PLIB_ETH_PatternMatchModeGet function.
*/

PLIB_TEMPLATE ETH_PATTERN_MATCH_MODE ETH_PatternMatchModeGet_Default( ETH_MODULE_ID index )
{
	volatile eth_registers_t * eth = ((eth_registers_t *)(index));	
     return (ETH_PATTERN_MATCH_MODE)eth->ETHRXFC.PMMODE;                     
}


//******************************************************************************
/* Function :  ETH_ExistsPatternMatch_Default

  Summary:
    Implements Default variant of PLIB_ETH_ExistsPatternMatch

  Description:
    This template implements the Default variant of the PLIB_ETH_ExistsPatternMatch function.
*/

#define PLIB_ETH_ExistsPatternMatch PLIB_ETH_ExistsPatternMatch
PLIB_TEMPLATE bool ETH_ExistsPatternMatch_Default( ETH_MODULE_ID index )
{
    return true;
}


#endif /*_ETH_PATTERNMATCH_DEFAULT_H*/

/******************************************************************************
 End of File
*/

