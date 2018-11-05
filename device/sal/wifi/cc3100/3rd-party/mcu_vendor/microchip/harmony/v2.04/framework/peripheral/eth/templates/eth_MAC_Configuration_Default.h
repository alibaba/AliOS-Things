/*******************************************************************************
  ETH Peripheral Library Template Implementation

  File Name:
    eth_MAC_Configuration_Default.h

  Summary:
    ETH PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : MAC_Configuration
    and its Variant : Default
    For following APIs :
        PLIB_ETH_LoopbackEnable
        PLIB_ETH_LoopbackDisable
        PLIB_ETH_LoopbackIsEnabled
        PLIB_ETH_TxPauseEnable
        PLIB_ETH_TxPauseDisable
        PLIB_ETH_TxPauseIsEnabled
        PLIB_ETH_RxPauseEnable
        PLIB_ETH_RxPauseDisable
        PLIB_ETH_RxPauseIsEnabled
        PLIB_ETH_PassAllEnable
        PLIB_ETH_PassAllDisable
        PLIB_ETH_PassAllIsEnabled
        PLIB_ETH_ReceiveEnable
        PLIB_ETH_ReceiveDisable
        PLIB_ETH_ReceiveIsEnabled
        PLIB_ETH_ExcessDeferEnable
        PLIB_ETH_ExcessDeferDisable
        PLIB_ETH_ExcessDeferIsEnabled
        PLIB_ETH_BackPresNoBackoffEnable
        PLIB_ETH_BackPresNoBackoffDisable
        PLIB_ETH_BackPresNoBackoffIsEnabled
        PLIB_ETH_NoBackoffEnable
        PLIB_ETH_NoBackoffDisable
        PLIB_ETH_NoBackoffIsEnabled
        PLIB_ETH_LongPreambleEnable
        PLIB_ETH_LongPreambleDisable
        PLIB_ETH_LongPreambleIsEnabled
        PLIB_ETH_PurePreambleEnable
        PLIB_ETH_PurePreambleDisable
        PLIB_ETH_PurePreambleIsEnabled
        PLIB_ETH_AutoDetectPadGet
        PLIB_ETH_AutoDetectPadSet
        PLIB_ETH_AutoDetectPadClear
        PLIB_ETH_CRCEnable
        PLIB_ETH_CRCDisable
        PLIB_ETH_CRCIsEnabled
        PLIB_ETH_DelayedCRCEnable
        PLIB_ETH_DelayedCRCDisable
        PLIB_ETH_DelayedCRCIsEnabled
        PLIB_ETH_HugeFrameEnable
        PLIB_ETH_HugeFrameDisable
        PLIB_ETH_HugeFrameIsEnabled
        PLIB_ETH_FrameLengthCheckEnable
        PLIB_ETH_FrameLengthCheckDisable
        PLIB_ETH_FrameLengthCheckIsEnabled
        PLIB_ETH_FullDuplexEnable
        PLIB_ETH_FullDuplexDisable
        PLIB_ETH_FullDuplexIsEnabled
        PLIB_ETH_ExistsMAC_Configuration

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

#ifndef _ETH_MAC_CONFIGURATION_DEFAULT_H
#define _ETH_MAC_CONFIGURATION_DEFAULT_H

#include "eth_registers.h"

//******************************************************************************
/* Function :  ETH_LoopbackEnable_Default

  Summary:
    Implements Default variant of PLIB_ETH_LoopbackEnable
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_LoopbackEnable function.
*/

PLIB_TEMPLATE void ETH_LoopbackEnable_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));	
	eth->EMACxCFG1SET = _EMACxCFG1_LOOPBACK_MASK;
}


//******************************************************************************
/* Function :  ETH_LoopbackDisable_Default

  Summary:
    Implements Default variant of PLIB_ETH_LoopbackDisable
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_LoopbackDisable function.
*/

PLIB_TEMPLATE void ETH_LoopbackDisable_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	eth->EMACxCFG1.LOOPBACK=0;
}


//******************************************************************************
/* Function :  ETH_LoopbackIsEnabled_Default

  Summary:
    Implements Default variant of PLIB_ETH_LoopbackIsEnabled
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_LoopbackIsEnabled function.
*/

PLIB_TEMPLATE bool ETH_LoopbackIsEnabled_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	return (bool) (eth->EMACxCFG1.LOOPBACK);
}


//******************************************************************************
/* Function :  ETH_TxPauseEnable_Default

  Summary:
    Implements Default variant of PLIB_ETH_TxPauseEnable
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_TxPauseEnable function.
*/

PLIB_TEMPLATE void ETH_TxPauseEnable_Default( ETH_MODULE_ID index )
{
 	volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	eth->EMACxCFG1SET = _EMACxCFG1_TXPAUSE_MASK;
}


//******************************************************************************
/* Function :  ETH_TxPauseDisable_Default

  Summary:
    Implements Default variant of PLIB_ETH_TxPauseDisable
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_TxPauseDisable function.
*/

PLIB_TEMPLATE void ETH_TxPauseDisable_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	eth->EMACxCFG1.TXPAUSE=0;
}


//******************************************************************************
/* Function :  ETH_TxPauseIsEnabled_Default

  Summary:
    Implements Default variant of PLIB_ETH_TxPauseIsEnabled
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_TxPauseIsEnabled function.
*/

PLIB_TEMPLATE bool ETH_TxPauseIsEnabled_Default( ETH_MODULE_ID index )
{
	volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	return (bool)(eth->EMACxCFG1.TXPAUSE);    
}


//******************************************************************************
/* Function :  ETH_RxPauseEnable_Default

  Summary:
    Implements Default variant of PLIB_ETH_RxPauseEnable
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_RxPauseEnable function.
*/

PLIB_TEMPLATE void ETH_RxPauseEnable_Default( ETH_MODULE_ID index )
{
	volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	eth->EMACxCFG1SET = _EMACxCFG1_RXPAUSE_MASK;
}


//******************************************************************************
/* Function :  ETH_RxPauseDisable_Default

  Summary:
    Implements Default variant of PLIB_ETH_RxPauseDisable
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_RxPauseDisable function.
*/

PLIB_TEMPLATE void ETH_RxPauseDisable_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	eth->EMACxCFG1.RXPAUSE=0;
}


//******************************************************************************
/* Function :  ETH_RxPauseIsEnabled_Default

  Summary:
    Implements Default variant of PLIB_ETH_RxPauseIsEnabled
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_RxPauseIsEnabled function.
*/

PLIB_TEMPLATE bool ETH_RxPauseIsEnabled_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	return (bool)(eth->EMACxCFG1.RXPAUSE);
}


//******************************************************************************
/* Function :  ETH_PassAllEnable_Default

  Summary:
    Implements Default variant of PLIB_ETH_PassAllEnable
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_PassAllEnable function.
*/

PLIB_TEMPLATE void ETH_PassAllEnable_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	eth->EMACxCFG1SET = _EMACxCFG1_PASSALL_MASK;	
}


//******************************************************************************
/* Function :  ETH_PassAllDisable_Default

  Summary:
    Implements Default variant of PLIB_ETH_PassAllDisable
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_PassAllDisable function.
*/

PLIB_TEMPLATE void ETH_PassAllDisable_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	eth->EMACxCFG1.PASSALL=0;	
}


//******************************************************************************
/* Function :  ETH_PassAllIsEnabled_Default

  Summary:
    Implements Default variant of PLIB_ETH_PassAllIsEnabled
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_PassAllIsEnabled function.
*/

PLIB_TEMPLATE bool ETH_PassAllIsEnabled_Default( ETH_MODULE_ID index )
{
	volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	return (bool) eth->EMACxCFG1.PASSALL;
}


//******************************************************************************
/* Function :  ETH_ReceiveEnable_Default

  Summary:
    Implements Default variant of PLIB_ETH_ReceiveEnable
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_ReceiveEnable function.
*/

PLIB_TEMPLATE void ETH_ReceiveEnable_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	eth->EMACxCFG1SET = _EMACxCFG1_RXENABLE_MASK;	
}


//******************************************************************************
/* Function :  ETH_ReceiveDisable_Default

  Summary:
    Implements Default variant of PLIB_ETH_ReceiveDisable
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_ReceiveDisable function.
*/

PLIB_TEMPLATE void ETH_ReceiveDisable_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	eth->EMACxCFG1.RXENABLE = 0;		
}


//******************************************************************************
/* Function :  ETH_ReceiveIsEnabled_Default

  Summary:
    Implements Default variant of PLIB_ETH_ReceiveIsEnabled
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_ReceiveIsEnabled function.
*/

PLIB_TEMPLATE bool ETH_ReceiveIsEnabled_Default( ETH_MODULE_ID index )
{
	volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	return (bool) eth->EMACxCFG1.RXENABLE;    
}


//******************************************************************************
/* Function :  ETH_ExcessDeferEnable_Default

  Summary:
    Implements Default variant of PLIB_ETH_ExcessDeferEnable
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_ExcessDeferEnable function.
*/

PLIB_TEMPLATE void ETH_ExcessDeferEnable_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	eth->EMACxCFG2SET = _EMACxCFG2_EXCESSDFR_MASK;	
}


//******************************************************************************
/* Function :  ETH_ExcessDeferDisable_Default

  Summary:
    Implements Default variant of PLIB_ETH_ExcessDeferDisable
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_ExcessDeferDisable function.
*/

PLIB_TEMPLATE void ETH_ExcessDeferDisable_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	eth->EMACxCFG2.EXCESSDFR=0;	
}


//******************************************************************************
/* Function :  ETH_ExcessDeferIsEnabled_Default

  Summary:
    Implements Default variant of PLIB_ETH_ExcessDeferIsEnabled
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_ExcessDeferIsEnabled function.
*/

PLIB_TEMPLATE bool ETH_ExcessDeferIsEnabled_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	return (bool) eth->EMACxCFG2.EXCESSDFR;
}


//******************************************************************************
/* Function :  ETH_BackPresNoBackoffEnable_Default

  Summary:
    Implements Default variant of PLIB_ETH_BackPresNoBackoffEnable
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_BackPresNoBackoffEnable function.
*/

PLIB_TEMPLATE void ETH_BackPresNoBackoffEnable_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	eth->EMACxCFG2SET = _EMACxCFG2_BPNOBKOFF_MASK;	
}


//******************************************************************************
/* Function :  ETH_BackPresNoBackoffDisable_Default

  Summary:
    Implements Default variant of PLIB_ETH_BackPresNoBackoffDisable
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_BackPresNoBackoffDisable function.
*/

PLIB_TEMPLATE void ETH_BackPresNoBackoffDisable_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	eth->EMACxCFG2.BPNOBKOFF=0;
}


//******************************************************************************
/* Function :  ETH_BackPresNoBackoffIsEnabled_Default

  Summary:
    Implements Default variant of PLIB_ETH_BackPresNoBackoffIsEnabled
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_BackPresNoBackoffIsEnabled function.
*/

PLIB_TEMPLATE bool ETH_BackPresNoBackoffIsEnabled_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	return (bool) eth->EMACxCFG2.BPNOBKOFF;
}


//******************************************************************************
/* Function :  ETH_NoBackoffEnable_Default

  Summary:
    Implements Default variant of PLIB_ETH_NoBackoffEnable
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_NoBackoffEnable function.
*/

PLIB_TEMPLATE void ETH_NoBackoffEnable_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	eth->EMACxCFG2SET=_EMACxCFG2_NOBKOFF_MASK;	
}


//******************************************************************************
/* Function :  ETH_NoBackoffDisable_Default

  Summary:
    Implements Default variant of PLIB_ETH_NoBackoffDisable
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_NoBackoffDisable function.
*/

PLIB_TEMPLATE void ETH_NoBackoffDisable_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	eth->EMACxCFG2.NOBKOFF=0;
}


//******************************************************************************
/* Function :  ETH_NoBackoffIsEnabled_Default

  Summary:
    Implements Default variant of PLIB_ETH_NoBackoffIsEnabled
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_NoBackoffIsEnabled function.
*/

PLIB_TEMPLATE bool ETH_NoBackoffIsEnabled_Default( ETH_MODULE_ID index )
{
	volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	return (bool)(eth->EMACxCFG2.NOBKOFF);
}


//******************************************************************************
/* Function :  ETH_LongPreambleEnable_Default

  Summary:
    Implements Default variant of PLIB_ETH_LongPreambleEnable
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_LongPreambleEnable function.
*/

PLIB_TEMPLATE void ETH_LongPreambleEnable_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	eth->EMACxCFG2SET=_EMACxCFG2_LONGPRE_MASK;
}


//******************************************************************************
/* Function :  ETH_LongPreambleDisable_Default

  Summary:
    Implements Default variant of PLIB_ETH_LongPreambleDisable
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_LongPreambleDisable function.
*/

PLIB_TEMPLATE void ETH_LongPreambleDisable_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	eth->EMACxCFG2.LONGPRE = 0;
}


//******************************************************************************
/* Function :  ETH_LongPreambleIsEnabled_Default

  Summary:
    Implements Default variant of PLIB_ETH_LongPreambleIsEnabled
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_LongPreambleIsEnabled function.
*/

PLIB_TEMPLATE bool ETH_LongPreambleIsEnabled_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	return (bool)(eth->EMACxCFG2.LONGPRE);
}


//******************************************************************************
/* Function :  ETH_PurePreambleEnable_Default

  Summary:
    Implements Default variant of PLIB_ETH_PurePreambleEnable
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_PurePreambleEnable function.
*/

PLIB_TEMPLATE void ETH_PurePreambleEnable_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	eth->EMACxCFG2SET=_EMACxCFG2_PUREPRE_MASK;
}


//******************************************************************************
/* Function :  ETH_PurePreambleDisable_Default

  Summary:
    Implements Default variant of PLIB_ETH_PurePreambleDisable
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_PurePreambleDisable function.
*/

PLIB_TEMPLATE void ETH_PurePreambleDisable_Default( ETH_MODULE_ID index )
{
   volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
   eth->EMACxCFG2.PUREPRE=0;
}


//******************************************************************************
/* Function :  ETH_PurePreambleIsEnabled_Default

  Summary:
    Implements Default variant of PLIB_ETH_PurePreambleIsEnabled
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_PurePreambleIsEnabled function.
*/

PLIB_TEMPLATE bool ETH_PurePreambleIsEnabled_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	return (bool) eth->EMACxCFG2.PUREPRE;
}


//******************************************************************************
/* Function :  ETH_AutoDetectPadGet_Default

  Summary:
    Implements Default variant of PLIB_ETH_AutoDetectPadGet
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_AutoDetectPadGet function.
*/

PLIB_TEMPLATE ETH_AUTOPAD_OPTION ETH_AutoDetectPadGet_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));							   
	return (ETH_AUTOPAD_OPTION) ((eth->EMACxCFG2.w &	
	(_EMACxCFG2_AUTOPAD_MASK | _EMACxCFG2_VLANPAD_MASK | 
	_EMACxCFG2_PADENABLE_MASK)) >> _EMACxCFG2_PADENABLE_POSITION );			
}

//******************************************************************************
/* Function :  ETH_AutoDetectPadSet_Default

  Summary:
    Implements Default variant of PLIB_ETH_AutoDetectPadSet
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_AutoDetectPadSet function.
*/

PLIB_TEMPLATE void ETH_AutoDetectPadSet_Default( ETH_MODULE_ID index , ETH_AUTOPAD_OPTION option )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));							   
	eth->EMACxCFG2SET = ((_EMACxCFG2_AUTOPAD_MASK | _EMACxCFG2_VLANPAD_MASK 
						| _EMACxCFG2_PADENABLE_MASK) & 
						((uint32_t) option << _EMACxCFG2_PADENABLE_POSITION));						
					
}


//******************************************************************************
/* Function :  ETH_AutoDetectPadClear_Default

  Summary:
    Implements Default variant of PLIB_ETH_AutoDetectPadClear
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_AutoDetectPadClear function.
*/

PLIB_TEMPLATE void ETH_AutoDetectPadClear_Default( ETH_MODULE_ID index , ETH_AUTOPAD_OPTION option )
{

	volatile eth_registers_t * eth = ((eth_registers_t *)(index));							   
	eth->EMACxCFG2.AUTOPAD=0;
	eth->EMACxCFG2.VLANPAD=0;
	eth->EMACxCFG2.PADENABLE=0;
}


//******************************************************************************
/* Function :  ETH_CRCEnable_Default

  Summary:
    Implements Default variant of PLIB_ETH_CRCEnable
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_CRCEnable function.
*/

PLIB_TEMPLATE void ETH_CRCEnable_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	eth->EMACxCFG2SET=_EMACxCFG2_CRCENABLE_MASK;
}


//******************************************************************************
/* Function :  ETH_CRCDisable_Default

  Summary:
    Implements Default variant of PLIB_ETH_CRCDisable
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_CRCDisable function.
*/

PLIB_TEMPLATE void ETH_CRCDisable_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	eth->EMACxCFG2.CRCENABLE=0;
}


//******************************************************************************
/* Function :  ETH_CRCIsEnabled_Default

  Summary:
    Implements Default variant of PLIB_ETH_CRCIsEnabled
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_CRCIsEnabled function.
*/

PLIB_TEMPLATE bool ETH_CRCIsEnabled_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	return (bool)(eth->EMACxCFG2.CRCENABLE);
}


//******************************************************************************
/* Function :  ETH_DelayedCRCEnable_Default

  Summary:
    Implements Default variant of PLIB_ETH_DelayedCRCEnable
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_DelayedCRCEnable function.
*/

PLIB_TEMPLATE void ETH_DelayedCRCEnable_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	eth->EMACxCFG2SET=_EMACxCFG2_DELAYCRC_MASK;
}


//******************************************************************************
/* Function :  ETH_DelayedCRCDisable_Default

  Summary:
    Implements Default variant of PLIB_ETH_DelayedCRCDisable
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_DelayedCRCDisable function.
*/

PLIB_TEMPLATE void ETH_DelayedCRCDisable_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	eth->EMACxCFG2.DELAYCRC= 0;
}


//******************************************************************************
/* Function :  ETH_DelayedCRCIsEnabled_Default

  Summary:
    Implements Default variant of PLIB_ETH_DelayedCRCIsEnabled
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_DelayedCRCIsEnabled function.
*/

PLIB_TEMPLATE bool ETH_DelayedCRCIsEnabled_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	return (bool)(eth->EMACxCFG2.DELAYCRC);
}


//******************************************************************************
/* Function :  ETH_HugeFrameEnable_Default

  Summary:
    Implements Default variant of PLIB_ETH_HugeFrameEnable
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_HugeFrameEnable function.
*/

PLIB_TEMPLATE void ETH_HugeFrameEnable_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	eth->EMACxCFG2SET=_EMACxCFG2_HUGEFRM_MASK;
}


//******************************************************************************
/* Function :  ETH_HugeFrameDisable_Default

  Summary:
    Implements Default variant of PLIB_ETH_HugeFrameDisable
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_HugeFrameDisable function.
*/

PLIB_TEMPLATE void ETH_HugeFrameDisable_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	eth->EMACxCFG2.HUGEFRM = 0;
}


//******************************************************************************
/* Function :  ETH_HugeFrameIsEnabled_Default

  Summary:
    Implements Default variant of PLIB_ETH_HugeFrameIsEnabled
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_HugeFrameIsEnabled function.
*/

PLIB_TEMPLATE bool ETH_HugeFrameIsEnabled_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	return (bool)(eth->EMACxCFG2.HUGEFRM);
}


//******************************************************************************
/* Function :  ETH_FrameLengthCheckEnable_Default

  Summary:
    Implements Default variant of PLIB_ETH_FrameLengthCheckEnable
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_FrameLengthCheckEnable function.
*/

PLIB_TEMPLATE void ETH_FrameLengthCheckEnable_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	eth->EMACxCFG2SET=_EMACxCFG2_LENGTHCK_MASK;
}


//******************************************************************************
/* Function :  ETH_FrameLengthCheckDisable_Default

  Summary:
    Implements Default variant of PLIB_ETH_FrameLengthCheckDisable
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_FrameLengthCheckDisable function.
*/

PLIB_TEMPLATE void ETH_FrameLengthCheckDisable_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	eth->EMACxCFG2.LENGTHCK=0;
}


//******************************************************************************
/* Function :  ETH_FrameLengthCheckIsEnabled_Default

  Summary:
    Implements Default variant of PLIB_ETH_FrameLengthCheckIsEnabled
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_FrameLengthCheckIsEnabled function.
*/

PLIB_TEMPLATE bool ETH_FrameLengthCheckIsEnabled_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	return (bool) eth->EMACxCFG2.LENGTHCK;
}


//******************************************************************************
/* Function :  ETH_FullDuplexEnable_Default

  Summary:
    Implements Default variant of PLIB_ETH_FullDuplexEnable
	This function performs atomic register access. 
  Description:
    This template implements the Default variant of the PLIB_ETH_FullDuplexEnable function.
*/

PLIB_TEMPLATE void ETH_FullDuplexEnable_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	eth->EMACxCFG2SET=_EMACxCFG2_FULLDPLX_MASK;
}


//******************************************************************************
/* Function :  ETH_FullDuplexDisable_Default

  Summary:
    Implements Default variant of PLIB_ETH_FullDuplexDisable
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_FullDuplexDisable function.
*/

PLIB_TEMPLATE void ETH_FullDuplexDisable_Default( ETH_MODULE_ID index )
{
    volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	eth->EMACxCFG2.FULLDPLX=0;
}


//******************************************************************************
/* Function :  ETH_FullDuplexIsEnabled_Default

  Summary:
    Implements Default variant of PLIB_ETH_FullDuplexIsEnabled
	This function performs atomic register access. 

  Description:
    This template implements the Default variant of the PLIB_ETH_FullDuplexIsEnabled function.
*/

PLIB_TEMPLATE bool ETH_FullDuplexIsEnabled_Default( ETH_MODULE_ID index )
{
	volatile eth_registers_t * eth = ((eth_registers_t *)(index));		
	return (bool)(eth->EMACxCFG2.FULLDPLX);    
}


//******************************************************************************
/* Function :  ETH_ExistsMAC_Configuration_Default

  Summary:
    Implements Default variant of PLIB_ETH_ExistsMAC_Configuration

  Description:
    This template implements the Default variant of the PLIB_ETH_ExistsMAC_Configuration function.
*/

#define PLIB_ETH_ExistsMAC_Configuration PLIB_ETH_ExistsMAC_Configuration
PLIB_TEMPLATE bool ETH_ExistsMAC_Configuration_Default( ETH_MODULE_ID index )
{
    return true;
}


#endif /*_ETH_MAC_CONFIGURATION_DEFAULT_H*/

/******************************************************************************
 End of File
*/

