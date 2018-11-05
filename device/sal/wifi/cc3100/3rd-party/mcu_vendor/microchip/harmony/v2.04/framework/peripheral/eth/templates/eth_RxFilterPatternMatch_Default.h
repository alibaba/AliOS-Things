/*******************************************************************************
  ETH Peripheral Library Template Implementation

  File Name:
    eth_RxFilterPM_Default.h

  Summary:
    ETH PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : Receive Filter PatternMatch Set
    and its Variant : Default
    For following APIs :
		PLIB_ETH_RxFiltersPMSet
        PLIB_ETH_ExistsRxFilterPMSet

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

#ifndef _ETH_RXFILTERPM_DEFAULT_H
#define _ETH_RXFILTERPM_DEFAULT_H
#include "eth_registers.h"
#include "eth_ReceiveFilters_Default.h"
#include "eth_PatternMatch_Default.h"

//******************************************************************************
/* Function :  ETH_RxFiltersPMSet_Default

  Summary:
    Implements Default variant of ETH_RxFiltersPMSet

  Description:
    This template implements the Default variant of the ETH_RxFiltersPMSet function.
*/
PLIB_TEMPLATE void ETH_RxFiltersPMSet_Default(ETH_MODULE_ID index,
                                   ETH_PMATCH_MODE mode,
                                   uint64_t matchMask,
                                   uint32_t matchOffs,
                                   uint32_t matchChecksum)
{
    ETH_PatternMatchModeSet_Default(index,ETH_PATTERN_MATCH_DISABLED);

    ETH_PatternMatchSet_Default(index,matchMask);
    ETH_PatternMatchOffsetSet_Default(index,matchOffs);
    ETH_PatternMatchChecksumSet_Default(index,matchChecksum);

    if(mode&ETH_FILT_PMATCH_INVERT)
    {
        ETH_ReceiveFilterEnable_Default(index,ETH_PATTERN_MATCH_INVERSION);
    }
    else
    {
        ETH_ReceiveFilterDisable_Default(index,ETH_PATTERN_MATCH_INVERSION);
    }

 // Enable Pattern Match mode
    ETH_PatternMatchModeSet_Default(index,mode&(~ETH_FILT_PMATCH_INVERT));
}

//******************************************************************************
/* Function :  ETH_ExistsRxFilterPMSet_Default

  Summary:
    Implements Default variant of ETH_ExistsRxFilterPMSet

  Description:
    This template implements the Default variant of the ETH_ExistsRxFilterPMSet function.
*/

#define PLIB_ETH_ExistsRxFilterPMSet PLIB_ETH_ExistsRxFilterPMSet
PLIB_TEMPLATE bool ETH_ExistsRxFiltersPMSet_Default( ETH_MODULE_ID index )
{
    return true;
}


#endif /*_ETH_RXFILTERPM_DEFAULT_H*/

/******************************************************************************
 End of File
*/

