/*******************************************************************************
  ETH Peripheral Library Template Implementation

  File Name:
    eth_PatternMatch_Unsupported.h

  Summary:
    ETH PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : PatternMatch
    and its Variant : Unsupported
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

#ifndef _ETH_PATTERNMATCH_UNSUPPORTED_H
#define _ETH_PATTERNMATCH_UNSUPPORTED_H


//******************************************************************************
/* Function :  ETH_PatternMatchSet_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_ETH_PatternMatchSet 

  Description:
    This template implements the Unsupported variant of the PLIB_ETH_PatternMatchSet function.
*/

PLIB_TEMPLATE void ETH_PatternMatchSet_Unsupported( ETH_MODULE_ID index , uint64_t patternMatchMaskValue )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_ETH_PatternMatchSet");
}


//******************************************************************************
/* Function :  ETH_PatternMatchGet_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_ETH_PatternMatchGet 

  Description:
    This template implements the Unsupported variant of the PLIB_ETH_PatternMatchGet function.
*/

PLIB_TEMPLATE uint64_t ETH_PatternMatchGet_Unsupported( ETH_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_ETH_PatternMatchGet");

    return 0;
}


//******************************************************************************
/* Function :  ETH_PatternMatchChecksumSet_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_ETH_PatternMatchChecksumSet 

  Description:
    This template implements the Unsupported variant of the PLIB_ETH_PatternMatchChecksumSet function.
*/

PLIB_TEMPLATE void ETH_PatternMatchChecksumSet_Unsupported( ETH_MODULE_ID index , uint16_t PatternMatchChecksumValue )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_ETH_PatternMatchChecksumSet");
}


//******************************************************************************
/* Function :  ETH_PatternMatchChecksumGet_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_ETH_PatternMatchChecksumGet 

  Description:
    This template implements the Unsupported variant of the PLIB_ETH_PatternMatchChecksumGet function.
*/

PLIB_TEMPLATE uint16_t ETH_PatternMatchChecksumGet_Unsupported( ETH_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_ETH_PatternMatchChecksumGet");

    return 0;
}


//******************************************************************************
/* Function :  ETH_PatternMatchOffsetSet_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_ETH_PatternMatchOffsetSet 

  Description:
    This template implements the Unsupported variant of the PLIB_ETH_PatternMatchOffsetSet function.
*/

PLIB_TEMPLATE void ETH_PatternMatchOffsetSet_Unsupported( ETH_MODULE_ID index , uint16_t PatternMatchOffsetValue )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_ETH_PatternMatchOffsetSet");
}


//******************************************************************************
/* Function :  ETH_PatternMatchOffsetGet_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_ETH_PatternMatchOffsetGet 

  Description:
    This template implements the Unsupported variant of the PLIB_ETH_PatternMatchOffsetGet function.
*/

PLIB_TEMPLATE uint16_t ETH_PatternMatchOffsetGet_Unsupported( ETH_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_ETH_PatternMatchOffsetGet");

    return 0;
}


//******************************************************************************
/* Function :  ETH_PatternMatchModeSet_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_ETH_PatternMatchModeSet 

  Description:
    This template implements the Unsupported variant of the PLIB_ETH_PatternMatchModeSet function.
*/

PLIB_TEMPLATE void ETH_PatternMatchModeSet_Unsupported( ETH_MODULE_ID index , ETH_PATTERN_MATCH_MODE modeSel )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_ETH_PatternMatchModeSet");
}


//******************************************************************************
/* Function :  ETH_PatternMatchModeGet_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_ETH_PatternMatchModeGet 

  Description:
    This template implements the Unsupported variant of the PLIB_ETH_PatternMatchModeGet function.
*/

PLIB_TEMPLATE ETH_PATTERN_MATCH_MODE ETH_PatternMatchModeGet_Unsupported( ETH_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_ETH_PatternMatchModeGet");

    return 0;
}


//******************************************************************************
/* Function :  ETH_ExistsPatternMatch_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_ETH_ExistsPatternMatch

  Description:
    This template implements the Unsupported variant of the PLIB_ETH_ExistsPatternMatch function.
*/

PLIB_TEMPLATE bool ETH_ExistsPatternMatch_Unsupported( ETH_MODULE_ID index )
{
    return false;
}


#endif /*_ETH_PATTERNMATCH_UNSUPPORTED_H*/

/******************************************************************************
 End of File
*/

