/*******************************************************************************
  ETH Peripheral Library Template Implementation

  File Name:
    eth_MAC_Configuration_Unsupported.h

  Summary:
    ETH PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : MAC_Configuration
    and its Variant : Unsupported
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

#ifndef _ETH_MAC_CONFIGURATION_UNSUPPORTED_H
#define _ETH_MAC_CONFIGURATION_UNSUPPORTED_H


//******************************************************************************
/* Function :  ETH_LoopbackEnable_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_ETH_LoopbackEnable 

  Description:
    This template implements the Unsupported variant of the PLIB_ETH_LoopbackEnable function.
*/

PLIB_TEMPLATE void ETH_LoopbackEnable_Unsupported( ETH_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_ETH_LoopbackEnable");
}


//******************************************************************************
/* Function :  ETH_LoopbackDisable_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_ETH_LoopbackDisable 

  Description:
    This template implements the Unsupported variant of the PLIB_ETH_LoopbackDisable function.
*/

PLIB_TEMPLATE void ETH_LoopbackDisable_Unsupported( ETH_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_ETH_LoopbackDisable");
}


//******************************************************************************
/* Function :  ETH_LoopbackIsEnabled_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_ETH_LoopbackIsEnabled 

  Description:
    This template implements the Unsupported variant of the PLIB_ETH_LoopbackIsEnabled function.
*/

PLIB_TEMPLATE bool ETH_LoopbackIsEnabled_Unsupported( ETH_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_ETH_LoopbackIsEnabled");

    return false;
}


//******************************************************************************
/* Function :  ETH_TxPauseEnable_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_ETH_TxPauseEnable 

  Description:
    This template implements the Unsupported variant of the PLIB_ETH_TxPauseEnable function.
*/

PLIB_TEMPLATE void ETH_TxPauseEnable_Unsupported( ETH_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_ETH_TxPauseEnable");
}


//******************************************************************************
/* Function :  ETH_TxPauseDisable_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_ETH_TxPauseDisable 

  Description:
    This template implements the Unsupported variant of the PLIB_ETH_TxPauseDisable function.
*/

PLIB_TEMPLATE void ETH_TxPauseDisable_Unsupported( ETH_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_ETH_TxPauseDisable");
}


//******************************************************************************
/* Function :  ETH_TxPauseIsEnabled_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_ETH_TxPauseIsEnabled 

  Description:
    This template implements the Unsupported variant of the PLIB_ETH_TxPauseIsEnabled function.
*/

PLIB_TEMPLATE bool ETH_TxPauseIsEnabled_Unsupported( ETH_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_ETH_TxPauseIsEnabled");

    return false;
}


//******************************************************************************
/* Function :  ETH_RxPauseEnable_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_ETH_RxPauseEnable 

  Description:
    This template implements the Unsupported variant of the PLIB_ETH_RxPauseEnable function.
*/

PLIB_TEMPLATE void ETH_RxPauseEnable_Unsupported( ETH_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_ETH_RxPauseEnable");
}


//******************************************************************************
/* Function :  ETH_RxPauseDisable_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_ETH_RxPauseDisable 

  Description:
    This template implements the Unsupported variant of the PLIB_ETH_RxPauseDisable function.
*/

PLIB_TEMPLATE void ETH_RxPauseDisable_Unsupported( ETH_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_ETH_RxPauseDisable");
}


//******************************************************************************
/* Function :  ETH_RxPauseIsEnabled_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_ETH_RxPauseIsEnabled 

  Description:
    This template implements the Unsupported variant of the PLIB_ETH_RxPauseIsEnabled function.
*/

PLIB_TEMPLATE bool ETH_RxPauseIsEnabled_Unsupported( ETH_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_ETH_RxPauseIsEnabled");

    return false;
}


//******************************************************************************
/* Function :  ETH_PassAllEnable_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_ETH_PassAllEnable 

  Description:
    This template implements the Unsupported variant of the PLIB_ETH_PassAllEnable function.
*/

PLIB_TEMPLATE void ETH_PassAllEnable_Unsupported( ETH_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_ETH_PassAllEnable");
}


//******************************************************************************
/* Function :  ETH_PassAllDisable_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_ETH_PassAllDisable 

  Description:
    This template implements the Unsupported variant of the PLIB_ETH_PassAllDisable function.
*/

PLIB_TEMPLATE void ETH_PassAllDisable_Unsupported( ETH_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_ETH_PassAllDisable");
}


//******************************************************************************
/* Function :  ETH_PassAllIsEnabled_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_ETH_PassAllIsEnabled 

  Description:
    This template implements the Unsupported variant of the PLIB_ETH_PassAllIsEnabled function.
*/

PLIB_TEMPLATE bool ETH_PassAllIsEnabled_Unsupported( ETH_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_ETH_PassAllIsEnabled");

    return false;
}


//******************************************************************************
/* Function :  ETH_ReceiveEnable_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_ETH_ReceiveEnable 

  Description:
    This template implements the Unsupported variant of the PLIB_ETH_ReceiveEnable function.
*/

PLIB_TEMPLATE void ETH_ReceiveEnable_Unsupported( ETH_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_ETH_ReceiveEnable");
}


//******************************************************************************
/* Function :  ETH_ReceiveDisable_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_ETH_ReceiveDisable 

  Description:
    This template implements the Unsupported variant of the PLIB_ETH_ReceiveDisable function.
*/

PLIB_TEMPLATE void ETH_ReceiveDisable_Unsupported( ETH_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_ETH_ReceiveDisable");
}


//******************************************************************************
/* Function :  ETH_ReceiveIsEnabled_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_ETH_ReceiveIsEnabled 

  Description:
    This template implements the Unsupported variant of the PLIB_ETH_ReceiveIsEnabled function.
*/

PLIB_TEMPLATE bool ETH_ReceiveIsEnabled_Unsupported( ETH_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_ETH_ReceiveIsEnabled");

    return false;
}


//******************************************************************************
/* Function :  ETH_ExcessDeferEnable_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_ETH_ExcessDeferEnable 

  Description:
    This template implements the Unsupported variant of the PLIB_ETH_ExcessDeferEnable function.
*/

PLIB_TEMPLATE void ETH_ExcessDeferEnable_Unsupported( ETH_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_ETH_ExcessDeferEnable");
}


//******************************************************************************
/* Function :  ETH_ExcessDeferDisable_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_ETH_ExcessDeferDisable 

  Description:
    This template implements the Unsupported variant of the PLIB_ETH_ExcessDeferDisable function.
*/

PLIB_TEMPLATE void ETH_ExcessDeferDisable_Unsupported( ETH_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_ETH_ExcessDeferDisable");
}


//******************************************************************************
/* Function :  ETH_ExcessDeferIsEnabled_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_ETH_ExcessDeferIsEnabled 

  Description:
    This template implements the Unsupported variant of the PLIB_ETH_ExcessDeferIsEnabled function.
*/

PLIB_TEMPLATE bool ETH_ExcessDeferIsEnabled_Unsupported( ETH_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_ETH_ExcessDeferIsEnabled");

    return false;
}


//******************************************************************************
/* Function :  ETH_BackPresNoBackoffEnable_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_ETH_BackPresNoBackoffEnable 

  Description:
    This template implements the Unsupported variant of the PLIB_ETH_BackPresNoBackoffEnable function.
*/

PLIB_TEMPLATE void ETH_BackPresNoBackoffEnable_Unsupported( ETH_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_ETH_BackPresNoBackoffEnable");
}


//******************************************************************************
/* Function :  ETH_BackPresNoBackoffDisable_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_ETH_BackPresNoBackoffDisable 

  Description:
    This template implements the Unsupported variant of the PLIB_ETH_BackPresNoBackoffDisable function.
*/

PLIB_TEMPLATE void ETH_BackPresNoBackoffDisable_Unsupported( ETH_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_ETH_BackPresNoBackoffDisable");
}


//******************************************************************************
/* Function :  ETH_BackPresNoBackoffIsEnabled_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_ETH_BackPresNoBackoffIsEnabled 

  Description:
    This template implements the Unsupported variant of the PLIB_ETH_BackPresNoBackoffIsEnabled function.
*/

PLIB_TEMPLATE bool ETH_BackPresNoBackoffIsEnabled_Unsupported( ETH_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_ETH_BackPresNoBackoffIsEnabled");

    return false;
}


//******************************************************************************
/* Function :  ETH_NoBackoffEnable_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_ETH_NoBackoffEnable 

  Description:
    This template implements the Unsupported variant of the PLIB_ETH_NoBackoffEnable function.
*/

PLIB_TEMPLATE void ETH_NoBackoffEnable_Unsupported( ETH_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_ETH_NoBackoffEnable");
}


//******************************************************************************
/* Function :  ETH_NoBackoffDisable_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_ETH_NoBackoffDisable 

  Description:
    This template implements the Unsupported variant of the PLIB_ETH_NoBackoffDisable function.
*/

PLIB_TEMPLATE void ETH_NoBackoffDisable_Unsupported( ETH_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_ETH_NoBackoffDisable");
}


//******************************************************************************
/* Function :  ETH_NoBackoffIsEnabled_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_ETH_NoBackoffIsEnabled 

  Description:
    This template implements the Unsupported variant of the PLIB_ETH_NoBackoffIsEnabled function.
*/

PLIB_TEMPLATE bool ETH_NoBackoffIsEnabled_Unsupported( ETH_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_ETH_NoBackoffIsEnabled");

    return false;
}


//******************************************************************************
/* Function :  ETH_LongPreambleEnable_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_ETH_LongPreambleEnable 

  Description:
    This template implements the Unsupported variant of the PLIB_ETH_LongPreambleEnable function.
*/

PLIB_TEMPLATE void ETH_LongPreambleEnable_Unsupported( ETH_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_ETH_LongPreambleEnable");
}


//******************************************************************************
/* Function :  ETH_LongPreambleDisable_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_ETH_LongPreambleDisable 

  Description:
    This template implements the Unsupported variant of the PLIB_ETH_LongPreambleDisable function.
*/

PLIB_TEMPLATE void ETH_LongPreambleDisable_Unsupported( ETH_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_ETH_LongPreambleDisable");
}


//******************************************************************************
/* Function :  ETH_LongPreambleIsEnabled_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_ETH_LongPreambleIsEnabled 

  Description:
    This template implements the Unsupported variant of the PLIB_ETH_LongPreambleIsEnabled function.
*/

PLIB_TEMPLATE bool ETH_LongPreambleIsEnabled_Unsupported( ETH_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_ETH_LongPreambleIsEnabled");

    return false;
}


//******************************************************************************
/* Function :  ETH_PurePreambleEnable_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_ETH_PurePreambleEnable 

  Description:
    This template implements the Unsupported variant of the PLIB_ETH_PurePreambleEnable function.
*/

PLIB_TEMPLATE void ETH_PurePreambleEnable_Unsupported( ETH_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_ETH_PurePreambleEnable");
}


//******************************************************************************
/* Function :  ETH_PurePreambleDisable_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_ETH_PurePreambleDisable 

  Description:
    This template implements the Unsupported variant of the PLIB_ETH_PurePreambleDisable function.
*/

PLIB_TEMPLATE void ETH_PurePreambleDisable_Unsupported( ETH_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_ETH_PurePreambleDisable");
}


//******************************************************************************
/* Function :  ETH_PurePreambleIsEnabled_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_ETH_PurePreambleIsEnabled 

  Description:
    This template implements the Unsupported variant of the PLIB_ETH_PurePreambleIsEnabled function.
*/

PLIB_TEMPLATE bool ETH_PurePreambleIsEnabled_Unsupported( ETH_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_ETH_PurePreambleIsEnabled");

    return false;
}


//******************************************************************************
/* Function :  ETH_AutoDetectPadGet_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_ETH_AutoDetectPadGet 

  Description:
    This template implements the Unsupported variant of the PLIB_ETH_AutoDetectPadGet function.
*/

PLIB_TEMPLATE ETH_AUTOPAD_OPTION ETH_AutoDetectPadGet_Unsupported( ETH_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_ETH_AutoDetectPadGet");

    return 0;
}


//******************************************************************************
/* Function :  ETH_AutoDetectPadSet_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_ETH_AutoDetectPadSet 

  Description:
    This template implements the Unsupported variant of the PLIB_ETH_AutoDetectPadSet function.
*/

PLIB_TEMPLATE void ETH_AutoDetectPadSet_Unsupported( ETH_MODULE_ID index , ETH_AUTOPAD_OPTION option )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_ETH_AutoDetectPadSet");
}


//******************************************************************************
/* Function :  ETH_AutoDetectPadClear_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_ETH_AutoDetectPadClear 

  Description:
    This template implements the Unsupported variant of the PLIB_ETH_AutoDetectPadClear function.
*/

PLIB_TEMPLATE void ETH_AutoDetectPadClear_Unsupported( ETH_MODULE_ID index , ETH_AUTOPAD_OPTION option )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_ETH_AutoDetectPadClear");
}


//******************************************************************************
/* Function :  ETH_CRCEnable_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_ETH_CRCEnable 

  Description:
    This template implements the Unsupported variant of the PLIB_ETH_CRCEnable function.
*/

PLIB_TEMPLATE void ETH_CRCEnable_Unsupported( ETH_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_ETH_CRCEnable");
}


//******************************************************************************
/* Function :  ETH_CRCDisable_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_ETH_CRCDisable 

  Description:
    This template implements the Unsupported variant of the PLIB_ETH_CRCDisable function.
*/

PLIB_TEMPLATE void ETH_CRCDisable_Unsupported( ETH_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_ETH_CRCDisable");
}


//******************************************************************************
/* Function :  ETH_CRCIsEnabled_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_ETH_CRCIsEnabled 

  Description:
    This template implements the Unsupported variant of the PLIB_ETH_CRCIsEnabled function.
*/

PLIB_TEMPLATE bool ETH_CRCIsEnabled_Unsupported( ETH_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_ETH_CRCIsEnabled");

    return false;
}


//******************************************************************************
/* Function :  ETH_DelayedCRCEnable_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_ETH_DelayedCRCEnable 

  Description:
    This template implements the Unsupported variant of the PLIB_ETH_DelayedCRCEnable function.
*/

PLIB_TEMPLATE void ETH_DelayedCRCEnable_Unsupported( ETH_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_ETH_DelayedCRCEnable");
}


//******************************************************************************
/* Function :  ETH_DelayedCRCDisable_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_ETH_DelayedCRCDisable 

  Description:
    This template implements the Unsupported variant of the PLIB_ETH_DelayedCRCDisable function.
*/

PLIB_TEMPLATE void ETH_DelayedCRCDisable_Unsupported( ETH_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_ETH_DelayedCRCDisable");
}


//******************************************************************************
/* Function :  ETH_DelayedCRCIsEnabled_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_ETH_DelayedCRCIsEnabled 

  Description:
    This template implements the Unsupported variant of the PLIB_ETH_DelayedCRCIsEnabled function.
*/

PLIB_TEMPLATE bool ETH_DelayedCRCIsEnabled_Unsupported( ETH_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_ETH_DelayedCRCIsEnabled");

    return false;
}


//******************************************************************************
/* Function :  ETH_HugeFrameEnable_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_ETH_HugeFrameEnable 

  Description:
    This template implements the Unsupported variant of the PLIB_ETH_HugeFrameEnable function.
*/

PLIB_TEMPLATE void ETH_HugeFrameEnable_Unsupported( ETH_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_ETH_HugeFrameEnable");
}


//******************************************************************************
/* Function :  ETH_HugeFrameDisable_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_ETH_HugeFrameDisable 

  Description:
    This template implements the Unsupported variant of the PLIB_ETH_HugeFrameDisable function.
*/

PLIB_TEMPLATE void ETH_HugeFrameDisable_Unsupported( ETH_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_ETH_HugeFrameDisable");
}


//******************************************************************************
/* Function :  ETH_HugeFrameIsEnabled_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_ETH_HugeFrameIsEnabled 

  Description:
    This template implements the Unsupported variant of the PLIB_ETH_HugeFrameIsEnabled function.
*/

PLIB_TEMPLATE bool ETH_HugeFrameIsEnabled_Unsupported( ETH_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_ETH_HugeFrameIsEnabled");

    return false;
}


//******************************************************************************
/* Function :  ETH_FrameLengthCheckEnable_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_ETH_FrameLengthCheckEnable 

  Description:
    This template implements the Unsupported variant of the PLIB_ETH_FrameLengthCheckEnable function.
*/

PLIB_TEMPLATE void ETH_FrameLengthCheckEnable_Unsupported( ETH_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_ETH_FrameLengthCheckEnable");
}


//******************************************************************************
/* Function :  ETH_FrameLengthCheckDisable_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_ETH_FrameLengthCheckDisable 

  Description:
    This template implements the Unsupported variant of the PLIB_ETH_FrameLengthCheckDisable function.
*/

PLIB_TEMPLATE void ETH_FrameLengthCheckDisable_Unsupported( ETH_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_ETH_FrameLengthCheckDisable");
}


//******************************************************************************
/* Function :  ETH_FrameLengthCheckIsEnabled_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_ETH_FrameLengthCheckIsEnabled 

  Description:
    This template implements the Unsupported variant of the PLIB_ETH_FrameLengthCheckIsEnabled function.
*/

PLIB_TEMPLATE bool ETH_FrameLengthCheckIsEnabled_Unsupported( ETH_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_ETH_FrameLengthCheckIsEnabled");

    return false;
}


//******************************************************************************
/* Function :  ETH_FullDuplexEnable_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_ETH_FullDuplexEnable 

  Description:
    This template implements the Unsupported variant of the PLIB_ETH_FullDuplexEnable function.
*/

PLIB_TEMPLATE void ETH_FullDuplexEnable_Unsupported( ETH_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_ETH_FullDuplexEnable");
}


//******************************************************************************
/* Function :  ETH_FullDuplexDisable_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_ETH_FullDuplexDisable 

  Description:
    This template implements the Unsupported variant of the PLIB_ETH_FullDuplexDisable function.
*/

PLIB_TEMPLATE void ETH_FullDuplexDisable_Unsupported( ETH_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_ETH_FullDuplexDisable");
}


//******************************************************************************
/* Function :  ETH_FullDuplexIsEnabled_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_ETH_FullDuplexIsEnabled 

  Description:
    This template implements the Unsupported variant of the PLIB_ETH_FullDuplexIsEnabled function.
*/

PLIB_TEMPLATE bool ETH_FullDuplexIsEnabled_Unsupported( ETH_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_ETH_FullDuplexIsEnabled");

    return false;
}


//******************************************************************************
/* Function :  ETH_ExistsMAC_Configuration_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_ETH_ExistsMAC_Configuration

  Description:
    This template implements the Unsupported variant of the PLIB_ETH_ExistsMAC_Configuration function.
*/

PLIB_TEMPLATE bool ETH_ExistsMAC_Configuration_Unsupported( ETH_MODULE_ID index )
{
    return false;
}


#endif /*_ETH_MAC_CONFIGURATION_UNSUPPORTED_H*/

/******************************************************************************
 End of File
*/

