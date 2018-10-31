/* Created by plibgen $Revision: 1.31 $ */

#ifndef _ETH_P32MZ2048EFM124_H
#define _ETH_P32MZ2048EFM124_H

/* Section 1 - Enumerate instances, define constants, VREGs */

#include <xc.h>
#include <stdbool.h>

#include "peripheral/peripheral_common_32bit.h"

/* Default definition used for all API dispatch functions */
#ifndef PLIB_INLINE_API
    #define PLIB_INLINE_API extern inline
#endif

/* Default definition used for all other functions */
#ifndef PLIB_INLINE
    #define PLIB_INLINE extern inline
#endif

typedef enum {

    ETH_ID_0 = _ETH_BASE_ADDRESS,
    ETH_NUMBER_OF_MODULES = 1

} ETH_MODULE_ID;

/* Section 2 - Feature variant inclusion */

#define PLIB_TEMPLATE PLIB_INLINE
#include "../templates/eth_PauseTimer_Default.h"
#include "../templates/eth_Enable_Default.h"
#include "../templates/eth_StopInIdle_Default.h"
#include "../templates/eth_TransmitRTS_Default.h"
#include "../templates/eth_ReceiveEnable_Default.h"
#include "../templates/eth_AutoFlowControl_Default.h"
#include "../templates/eth_ManualFlowControl_Default.h"
#include "../templates/eth_RxBufferCountDecrement_Default.h"
#include "../templates/eth_ReceiveBufferSize_Default.h"
#include "../templates/eth_TxPacketDescriptorAddress_Default.h"
#include "../templates/eth_RxPacketDescriptorAddress_Default.h"
#include "../templates/eth_HashTable_Default.h"
#include "../templates/eth_PatternMatch_Default.h"
#include "../templates/eth_RxFilterPatternMatch_Default.h"
#include "../templates/eth_ReceiveFilters_Default.h"
#include "../templates/eth_ReceiveWmarks_Default.h"
#include "../templates/eth_Interrupt_Default.h"
#include "../templates/eth_EthernetControllerStatus_Default.h"
#include "../templates/eth_ReceiveOverflowCount_Default.h"
#include "../templates/eth_FramesTransmittedOK_Default.h"
#include "../templates/eth_CollisionCounts_Default.h"
#include "../templates/eth_FramexReceivedOK_Default.h"
#include "../templates/eth_FCSErrorCount_Default.h"
#include "../templates/eth_AlignmentErrorCount_Default.h"
#include "../templates/eth_MAC_Resets_Default.h"
#include "../templates/eth_MAC_Configuration_Default.h"
#include "../templates/eth_InterPacketGaps_Default.h"
#include "../templates/eth_CollisionWindow_Default.h"
#include "../templates/eth_RetransmissionMaximum_Default.h"
#include "../templates/eth_MaxFrameLength_Default.h"
#include "../templates/eth_RMII_Support_Default.h"
#include "../templates/eth_MAC_Testing_Default.h"
#include "../templates/eth_MIIM_Config_Default.h"
#include "../templates/eth_MIIMScanMode_Default.h"
#include "../templates/eth_MIIMReadWrite_Default.h"
#include "../templates/eth_MIIMAddresses_Default.h"
#include "../templates/eth_MIIWriteReadData_Default.h"
#include "../templates/eth_MIIM_Indicators_Default.h"
#include "../templates/eth_StationAddress_Default.h"

/* Section 3 - PLIB dispatch function definitions */

PLIB_INLINE_API void PLIB_ETH_PauseTimerSet(ETH_MODULE_ID index, uint16_t PauseTimerValue)
{
     ETH_PauseTimerSet_Default(index, PauseTimerValue);
}

PLIB_INLINE_API uint16_t PLIB_ETH_PauseTimerGet(ETH_MODULE_ID index)
{
     return ETH_PauseTimerGet_Default(index);
}

PLIB_INLINE_API bool PLIB_ETH_ExistsPauseTimer(ETH_MODULE_ID index)
{
     return ETH_ExistsPauseTimer_Default(index);
}

PLIB_INLINE_API void PLIB_ETH_Enable(ETH_MODULE_ID index)
{
     ETH_Enable_Default(index);
}

PLIB_INLINE_API void PLIB_ETH_Disable(ETH_MODULE_ID index)
{
     ETH_Disable_Default(index);
}

PLIB_INLINE_API bool PLIB_ETH_IsEnabled(ETH_MODULE_ID index)
{
     return ETH_IsEnabled_Default(index);
}

PLIB_INLINE_API bool PLIB_ETH_ExistsEnable(ETH_MODULE_ID index)
{
     return ETH_ExistsEnable_Default(index);
}

PLIB_INLINE_API void PLIB_ETH_StopInIdleEnable(ETH_MODULE_ID index)
{
     ETH_StopInIdleEnable_Default(index);
}

PLIB_INLINE_API void PLIB_ETH_StopInIdleDisable(ETH_MODULE_ID index)
{
     ETH_StopInIdleDisable_Default(index);
}

PLIB_INLINE_API bool PLIB_ETH_StopInIdleIsEnabled(ETH_MODULE_ID index)
{
     return ETH_StopInIdleIsEnabled_Default(index);
}

PLIB_INLINE_API bool PLIB_ETH_ExistsStopInIdle(ETH_MODULE_ID index)
{
     return ETH_ExistsStopInIdle_Default(index);
}

PLIB_INLINE_API void PLIB_ETH_TxRTSEnable(ETH_MODULE_ID index)
{
     ETH_TxRTSEnable_Default(index);
}

PLIB_INLINE_API void PLIB_ETH_TxRTSDisable(ETH_MODULE_ID index)
{
     ETH_TxRTSDisable_Default(index);
}

PLIB_INLINE_API bool PLIB_ETH_TxRTSIsEnabled(ETH_MODULE_ID index)
{
     return ETH_TxRTSIsEnabled_Default(index);
}

PLIB_INLINE_API bool PLIB_ETH_ExistsTransmitRTS(ETH_MODULE_ID index)
{
     return ETH_ExistsTransmitRTS_Default(index);
}

PLIB_INLINE_API void PLIB_ETH_RxEnable(ETH_MODULE_ID index)
{
     ETH_RxEnable_Default(index);
}

PLIB_INLINE_API void PLIB_ETH_RxDisable(ETH_MODULE_ID index)
{
     ETH_RxDisable_Default(index);
}

PLIB_INLINE_API bool PLIB_ETH_RxIsEnabled(ETH_MODULE_ID index)
{
     return ETH_RxIsEnabled_Default(index);
}

PLIB_INLINE_API bool PLIB_ETH_ExistsRxEnable(ETH_MODULE_ID index)
{
     return ETH_ExistsRxEnable_Default(index);
}

PLIB_INLINE_API void PLIB_ETH_AutoFlowControlEnable(ETH_MODULE_ID index)
{
     ETH_AutoFlowControlEnable_Default(index);
}

PLIB_INLINE_API void PLIB_ETH_AutoFlowControlDisable(ETH_MODULE_ID index)
{
     ETH_AutoFlowControlDisable_Default(index);
}

PLIB_INLINE_API bool PLIB_ETH_AutoFlowControlIsEnabled(ETH_MODULE_ID index)
{
     return ETH_AutoFlowControlIsEnabled_Default(index);
}

PLIB_INLINE_API bool PLIB_ETH_ExistsAutoFlowControl(ETH_MODULE_ID index)
{
     return ETH_ExistsAutoFlowControl_Default(index);
}

PLIB_INLINE_API void PLIB_ETH_ManualFlowControlEnable(ETH_MODULE_ID index)
{
     ETH_ManualFlowControlEnable_Default(index);
}

PLIB_INLINE_API void PLIB_ETH_ManualFlowControlDisable(ETH_MODULE_ID index)
{
     ETH_ManualFlowControlDisable_Default(index);
}

PLIB_INLINE_API bool PLIB_ETH_ManualFlowControlIsEnabled(ETH_MODULE_ID index)
{
     return ETH_ManualFlowControlIsEnabled_Default(index);
}

PLIB_INLINE_API bool PLIB_ETH_ExistsManualFlowControl(ETH_MODULE_ID index)
{
     return ETH_ExistsManualFlowControl_Default(index);
}

PLIB_INLINE_API void PLIB_ETH_RxBufferCountDecrement(ETH_MODULE_ID index)
{
     ETH_RxBufferCountDecrement_Default(index);
}

PLIB_INLINE_API bool PLIB_ETH_ExistsRxBufferCountDecrement(ETH_MODULE_ID index)
{
     return ETH_ExistsRxBufferCountDecrement_Default(index);
}

PLIB_INLINE_API uint8_t PLIB_ETH_ReceiveBufferSizeGet(ETH_MODULE_ID index)
{
     return ETH_ReceiveBufferSizeGet_Default(index);
}

PLIB_INLINE_API void PLIB_ETH_ReceiveBufferSizeSet(ETH_MODULE_ID index, uint8_t ReceiveBufferSize)
{
     ETH_ReceiveBufferSizeSet_Default(index, ReceiveBufferSize);
}

PLIB_INLINE_API int PLIB_ETH_RxSetBufferSize(ETH_MODULE_ID index, int rxBuffSize)
{
     return ETH_RxSetBufferSize_Default(index, rxBuffSize);
}

PLIB_INLINE_API bool PLIB_ETH_ExistsReceiveBufferSize(ETH_MODULE_ID index)
{
     return ETH_ExistsReceiveBufferSize_Default(index);
}

PLIB_INLINE_API void PLIB_ETH_TxPacketDescAddrSet(ETH_MODULE_ID index, uint8_t* txPacketDescStartAddr)
{
     ETH_TxPacketDescAddrSet_Default(index, txPacketDescStartAddr);
}

PLIB_INLINE_API uint8_t* PLIB_ETH_TxPacketDescAddrGet(ETH_MODULE_ID index)
{
     return ETH_TxPacketDescAddrGet_Default(index);
}

PLIB_INLINE_API bool PLIB_ETH_ExistsTxPacketDescriptorAddress(ETH_MODULE_ID index)
{
     return ETH_ExistsTxPacketDescriptorAddress_Default(index);
}

PLIB_INLINE_API void PLIB_ETH_RxPacketDescAddrSet(ETH_MODULE_ID index, uint8_t* rxPacketDescStartAddr)
{
     ETH_RxPacketDescAddrSet_Default(index, rxPacketDescStartAddr);
}

PLIB_INLINE_API uint8_t* PLIB_ETH_RxPacketDescAddrGet(ETH_MODULE_ID index)
{
     return ETH_RxPacketDescAddrGet_Default(index);
}

PLIB_INLINE_API bool PLIB_ETH_ExistsRxPacketDescriptorAddress(ETH_MODULE_ID index)
{
     return ETH_ExistsRxPacketDescriptorAddress_Default(index);
}

PLIB_INLINE_API void PLIB_ETH_HashTableSet(ETH_MODULE_ID index, uint64_t hashTableValue)
{
     ETH_HashTableSet_Default(index, hashTableValue);
}

PLIB_INLINE_API uint32_t PLIB_ETH_HashTableGet(ETH_MODULE_ID index)
{
     return ETH_HashTableGet_Default(index);
}

PLIB_INLINE_API void PLIB_ETH_RxFiltersHTSet(ETH_MODULE_ID index, uint64_t htable)
{
     ETH_RxFiltersHTSet_Default(index, htable);
}

PLIB_INLINE_API bool PLIB_ETH_ExistsHashTable(ETH_MODULE_ID index)
{
     return ETH_ExistsHashTable_Default(index);
}

PLIB_INLINE_API void PLIB_ETH_PatternMatchSet(ETH_MODULE_ID index, uint64_t patternMatchMaskValue)
{
     ETH_PatternMatchSet_Default(index, patternMatchMaskValue);
}

PLIB_INLINE_API uint64_t PLIB_ETH_PatternMatchGet(ETH_MODULE_ID index)
{
     return ETH_PatternMatchGet_Default(index);
}

PLIB_INLINE_API void PLIB_ETH_PatternMatchChecksumSet(ETH_MODULE_ID index, uint16_t PatternMatchChecksumValue)
{
     ETH_PatternMatchChecksumSet_Default(index, PatternMatchChecksumValue);
}

PLIB_INLINE_API uint16_t PLIB_ETH_PatternMatchChecksumGet(ETH_MODULE_ID index)
{
     return ETH_PatternMatchChecksumGet_Default(index);
}

PLIB_INLINE_API void PLIB_ETH_PatternMatchOffsetSet(ETH_MODULE_ID index, uint16_t PatternMatchOffsetValue)
{
     ETH_PatternMatchOffsetSet_Default(index, PatternMatchOffsetValue);
}

PLIB_INLINE_API uint16_t PLIB_ETH_PatternMatchOffsetGet(ETH_MODULE_ID index)
{
     return ETH_PatternMatchOffsetGet_Default(index);
}

PLIB_INLINE_API void PLIB_ETH_PatternMatchModeSet(ETH_MODULE_ID index, ETH_PATTERN_MATCH_MODE modeSel)
{
     ETH_PatternMatchModeSet_Default(index, modeSel);
}

PLIB_INLINE_API ETH_PATTERN_MATCH_MODE PLIB_ETH_PatternMatchModeGet(ETH_MODULE_ID index)
{
     return ETH_PatternMatchModeGet_Default(index);
}

PLIB_INLINE_API void PLIB_ETH_RxFiltersPMClr(ETH_MODULE_ID index)
{
     ETH_RxFiltersPMClr_Default(index);
}

PLIB_INLINE_API bool PLIB_ETH_ExistsPatternMatch(ETH_MODULE_ID index)
{
     return ETH_ExistsPatternMatch_Default(index);
}

PLIB_INLINE_API void PLIB_ETH_RxFiltersPMSet(ETH_MODULE_ID index, ETH_PMATCH_MODE mode, uint64_t matchMask, uint32_t matchOffs, uint32_t matchChecksum)
{
     ETH_RxFiltersPMSet_Default(index, mode, matchMask, matchOffs, matchChecksum);
}

PLIB_INLINE_API bool PLIB_ETH_ExistsRxFiltersPMSet(ETH_MODULE_ID index)
{
     return ETH_ExistsRxFiltersPMSet_Default(index);
}

PLIB_INLINE_API void PLIB_ETH_ReceiveFilterEnable(ETH_MODULE_ID index, ETH_RECEIVE_FILTER filter)
{
     ETH_ReceiveFilterEnable_Default(index, filter);
}

PLIB_INLINE_API void PLIB_ETH_ReceiveFilterDisable(ETH_MODULE_ID index, ETH_RECEIVE_FILTER filter)
{
     ETH_ReceiveFilterDisable_Default(index, filter);
}

PLIB_INLINE_API bool PLIB_ETH_ReceiveFilterIsEnable(ETH_MODULE_ID index, ETH_RECEIVE_FILTER filter)
{
     return ETH_ReceiveFilterIsEnable_Default(index, filter);
}

PLIB_INLINE_API void PLIB_ETH_RxFiltersSet(ETH_MODULE_ID index, ETH_RX_FILTERS rxFilters)
{
     ETH_RxFiltersSet_Default(index, rxFilters);
}

PLIB_INLINE_API void PLIB_ETH_RxFiltersClr(ETH_MODULE_ID index, ETH_RX_FILTERS rxFilters)
{
     ETH_RxFiltersClr_Default(index, rxFilters);
}

PLIB_INLINE_API void PLIB_ETH_RxFiltersWrite(ETH_MODULE_ID index, ETH_RX_FILTERS rxFilters)
{
     ETH_RxFiltersWrite_Default(index, rxFilters);
}

PLIB_INLINE_API bool PLIB_ETH_ExistsReceiveFilters(ETH_MODULE_ID index)
{
     return ETH_ExistsReceiveFilters_Default(index);
}

PLIB_INLINE_API void PLIB_ETH_RxFullWmarkSet(ETH_MODULE_ID index, uint8_t watermarkValue)
{
     ETH_RxFullWmarkSet_Default(index, watermarkValue);
}

PLIB_INLINE_API uint8_t PLIB_ETH_RxFullWmarkGet(ETH_MODULE_ID index)
{
     return ETH_RxFullWmarkGet_Default(index);
}

PLIB_INLINE_API void PLIB_ETH_RxEmptyWmarkSet(ETH_MODULE_ID index, uint8_t watermarkValue)
{
     ETH_RxEmptyWmarkSet_Default(index, watermarkValue);
}

PLIB_INLINE_API uint8_t PLIB_ETH_RxEmptyWmarkGet(ETH_MODULE_ID index)
{
     return ETH_RxEmptyWmarkGet_Default(index);
}

PLIB_INLINE_API bool PLIB_ETH_ExistsReceiveWmarks(ETH_MODULE_ID index)
{
     return ETH_ExistsReceiveWmarks_Default(index);
}

PLIB_INLINE_API void PLIB_ETH_InterruptSourceEnable(ETH_MODULE_ID index, ETH_INTERRUPT_SOURCES intmask)
{
     ETH_InterruptSourceEnable_Default(index, intmask);
}

PLIB_INLINE_API void PLIB_ETH_InterruptSourceDisable(ETH_MODULE_ID index, ETH_INTERRUPT_SOURCES intmask)
{
     ETH_InterruptSourceDisable_Default(index, intmask);
}

PLIB_INLINE_API bool PLIB_ETH_InterruptSourceIsEnabled(ETH_MODULE_ID index, ETH_INTERRUPT_SOURCES intmask)
{
     return ETH_InterruptSourceIsEnabled_Default(index, intmask);
}

PLIB_INLINE_API ETH_INTERRUPT_SOURCES PLIB_ETH_InterruptSourcesGet(ETH_MODULE_ID index)
{
     return ETH_InterruptSourcesGet_Default(index);
}

PLIB_INLINE_API void PLIB_ETH_InterruptSet(ETH_MODULE_ID index, ETH_INTERRUPT_SOURCES intmask)
{
     ETH_InterruptSet_Default(index, intmask);
}

PLIB_INLINE_API void PLIB_ETH_InterruptClear(ETH_MODULE_ID index, ETH_INTERRUPT_SOURCES intmask)
{
     ETH_InterruptClear_Default(index, intmask);
}

PLIB_INLINE_API ETH_INTERRUPT_SOURCES PLIB_ETH_InterruptsGet(ETH_MODULE_ID index)
{
     return ETH_InterruptsGet_Default(index);
}

PLIB_INLINE_API bool PLIB_ETH_InterruptStatusGet(ETH_MODULE_ID index, ETH_INTERRUPT_SOURCES intmask)
{
     return ETH_InterruptStatusGet_Default(index, intmask);
}

PLIB_INLINE_API void PLIB_ETH_EventsEnableSet(ETH_MODULE_ID index, PLIB_ETH_EVENTS eEvents)
{
     ETH_EventsEnableSet_Default(index, eEvents);
}

PLIB_INLINE_API void PLIB_ETH_EventsEnableClr(ETH_MODULE_ID index, PLIB_ETH_EVENTS eEvents)
{
     ETH_EventsEnableClr_Default(index, eEvents);
}

PLIB_INLINE_API void PLIB_ETH_EventsEnableWrite(ETH_MODULE_ID index, PLIB_ETH_EVENTS eEvents)
{
     ETH_EventsEnableWrite_Default(index, eEvents);
}

PLIB_INLINE_API PLIB_ETH_EVENTS PLIB_ETH_EventsEnableGet(ETH_MODULE_ID index)
{
     return ETH_EventsEnableGet_Default(index);
}

PLIB_INLINE_API void PLIB_ETH_EventsClr(ETH_MODULE_ID index, PLIB_ETH_EVENTS eEvents)
{
     ETH_EventsClr_Default(index, eEvents);
}

PLIB_INLINE_API PLIB_ETH_EVENTS PLIB_ETH_EventsGet(ETH_MODULE_ID index)
{
     return ETH_EventsGet_Default(index);
}

PLIB_INLINE_API bool PLIB_ETH_ExistsInterrupt(ETH_MODULE_ID index)
{
     return ETH_ExistsInterrupt_Default(index);
}

PLIB_INLINE_API uint8_t PLIB_ETH_RxPacketCountGet(ETH_MODULE_ID index)
{
     return ETH_RxPacketCountGet_Default(index);
}

PLIB_INLINE_API bool PLIB_ETH_EthernetIsBusy(ETH_MODULE_ID index)
{
     return ETH_EthernetIsBusy_Default(index);
}

PLIB_INLINE_API bool PLIB_ETH_TransmitIsBusy(ETH_MODULE_ID index)
{
     return ETH_TransmitIsBusy_Default(index);
}

PLIB_INLINE_API bool PLIB_ETH_ReceiveIsBusy(ETH_MODULE_ID index)
{
     return ETH_ReceiveIsBusy_Default(index);
}

PLIB_INLINE_API bool PLIB_ETH_ExistsEthernetControllerStatus(ETH_MODULE_ID index)
{
     return ETH_ExistsEthernetControllerStatus_Default(index);
}

PLIB_INLINE_API void PLIB_ETH_RxOverflowCountClear(ETH_MODULE_ID index)
{
     ETH_RxOverflowCountClear_Default(index);
}

PLIB_INLINE_API uint16_t PLIB_ETH_RxOverflowCountGet(ETH_MODULE_ID index)
{
     return ETH_RxOverflowCountGet_Default(index);
}

PLIB_INLINE_API bool PLIB_ETH_ExistsReceiveOverflowCount(ETH_MODULE_ID index)
{
     return ETH_ExistsReceiveOverflowCount_Default(index);
}

PLIB_INLINE_API void PLIB_ETH_FramesTxdOkCountClear(ETH_MODULE_ID index)
{
     ETH_FramesTxdOkCountClear_Default(index);
}

PLIB_INLINE_API uint16_t PLIB_ETH_FramesTxdOkCountGet(ETH_MODULE_ID index)
{
     return ETH_FramesTxdOkCountGet_Default(index);
}

PLIB_INLINE_API bool PLIB_ETH_ExistsFramesTransmittedOK(ETH_MODULE_ID index)
{
     return ETH_ExistsFramesTransmittedOK_Default(index);
}

PLIB_INLINE_API void PLIB_ETH_SingleCollisionCountClear(ETH_MODULE_ID index)
{
     ETH_SingleCollisionCountClear_Default(index);
}

PLIB_INLINE_API uint16_t PLIB_ETH_SingleCollisionCountGet(ETH_MODULE_ID index)
{
     return ETH_SingleCollisionCountGet_Default(index);
}

PLIB_INLINE_API void PLIB_ETH_MultipleCollisionCountClear(ETH_MODULE_ID index)
{
     ETH_MultipleCollisionCountClear_Default(index);
}

PLIB_INLINE_API uint16_t PLIB_ETH_MultipleCollisionCountGet(ETH_MODULE_ID index)
{
     return ETH_MultipleCollisionCountGet_Default(index);
}

PLIB_INLINE_API bool PLIB_ETH_ExistsCollisionCounts(ETH_MODULE_ID index)
{
     return ETH_ExistsCollisionCounts_Default(index);
}

PLIB_INLINE_API void PLIB_ETH_FramesRxdOkCountClear(ETH_MODULE_ID index)
{
     ETH_FramesRxdOkCountClear_Default(index);
}

PLIB_INLINE_API uint16_t PLIB_ETH_FramesRxdOkCountGet(ETH_MODULE_ID index)
{
     return ETH_FramesRxdOkCountGet_Default(index);
}

PLIB_INLINE_API bool PLIB_ETH_ExistsFramexReceivedOK(ETH_MODULE_ID index)
{
     return ETH_ExistsFramexReceivedOK_Default(index);
}

PLIB_INLINE_API void PLIB_ETH_FCSErrorCountClear(ETH_MODULE_ID index)
{
     ETH_FCSErrorCountClear_Default(index);
}

PLIB_INLINE_API uint16_t PLIB_ETH_FCSErrorCountGet(ETH_MODULE_ID index)
{
     return ETH_FCSErrorCountGet_Default(index);
}

PLIB_INLINE_API bool PLIB_ETH_ExistsFCSErrorCount(ETH_MODULE_ID index)
{
     return ETH_ExistsFCSErrorCount_Default(index);
}

PLIB_INLINE_API void PLIB_ETH_AlignErrorCountClear(ETH_MODULE_ID index)
{
     ETH_AlignErrorCountClear_Default(index);
}

PLIB_INLINE_API uint16_t PLIB_ETH_AlignErrorCountGet(ETH_MODULE_ID index)
{
     return ETH_AlignErrorCountGet_Default(index);
}

PLIB_INLINE_API bool PLIB_ETH_ExistsAlignmentErrorCount(ETH_MODULE_ID index)
{
     return ETH_ExistsAlignmentErrorCount_Default(index);
}

PLIB_INLINE_API void PLIB_ETH_MIIResetEnable(ETH_MODULE_ID index)
{
     ETH_MIIResetEnable_Default(index);
}

PLIB_INLINE_API void PLIB_ETH_MIIResetDisable(ETH_MODULE_ID index)
{
     ETH_MIIResetDisable_Default(index);
}

PLIB_INLINE_API bool PLIB_ETH_MIIResetIsEnabled(ETH_MODULE_ID index)
{
     return ETH_MIIResetIsEnabled_Default(index);
}

PLIB_INLINE_API void PLIB_ETH_SimResetEnable(ETH_MODULE_ID index)
{
     ETH_SimResetEnable_Default(index);
}

PLIB_INLINE_API void PLIB_ETH_SimResetDisable(ETH_MODULE_ID index)
{
     ETH_SimResetDisable_Default(index);
}

PLIB_INLINE_API bool PLIB_ETH_SimResetIsEnabled(ETH_MODULE_ID index)
{
     return ETH_SimResetIsEnabled_Default(index);
}

PLIB_INLINE_API void PLIB_ETH_MCSRxResetEnable(ETH_MODULE_ID index)
{
     ETH_MCSRxResetEnable_Default(index);
}

PLIB_INLINE_API void PLIB_ETH_MCSRxResetDisable(ETH_MODULE_ID index)
{
     ETH_MCSRxResetDisable_Default(index);
}

PLIB_INLINE_API bool PLIB_ETH_MCSRxResetIsEnabled(ETH_MODULE_ID index)
{
     return ETH_MCSRxResetIsEnabled_Default(index);
}

PLIB_INLINE_API void PLIB_ETH_RxFuncResetEnable(ETH_MODULE_ID index)
{
     ETH_RxFuncResetEnable_Default(index);
}

PLIB_INLINE_API void PLIB_ETH_RxFuncResetDisable(ETH_MODULE_ID index)
{
     ETH_RxFuncResetDisable_Default(index);
}

PLIB_INLINE_API bool PLIB_ETH_RxFuncResetIsEnabled(ETH_MODULE_ID index)
{
     return ETH_RxFuncResetIsEnabled_Default(index);
}

PLIB_INLINE_API void PLIB_ETH_MCSTxResetEnable(ETH_MODULE_ID index)
{
     ETH_MCSTxResetEnable_Default(index);
}

PLIB_INLINE_API void PLIB_ETH_MCSTxResetDisable(ETH_MODULE_ID index)
{
     ETH_MCSTxResetDisable_Default(index);
}

PLIB_INLINE_API bool PLIB_ETH_MCSTxResetIsEnabled(ETH_MODULE_ID index)
{
     return ETH_MCSTxResetIsEnabled_Default(index);
}

PLIB_INLINE_API void PLIB_ETH_TxFuncResetEnable(ETH_MODULE_ID index)
{
     ETH_TxFuncResetEnable_Default(index);
}

PLIB_INLINE_API void PLIB_ETH_TxFuncResetDisable(ETH_MODULE_ID index)
{
     ETH_TxFuncResetDisable_Default(index);
}

PLIB_INLINE_API bool PLIB_ETH_TxFuncResetIsEnabled(ETH_MODULE_ID index)
{
     return ETH_TxFuncResetIsEnabled_Default(index);
}

PLIB_INLINE_API bool PLIB_ETH_ExistsMAC_Resets(ETH_MODULE_ID index)
{
     return ETH_ExistsMAC_Resets_Default(index);
}

PLIB_INLINE_API void PLIB_ETH_LoopbackEnable(ETH_MODULE_ID index)
{
     ETH_LoopbackEnable_Default(index);
}

PLIB_INLINE_API void PLIB_ETH_LoopbackDisable(ETH_MODULE_ID index)
{
     ETH_LoopbackDisable_Default(index);
}

PLIB_INLINE_API bool PLIB_ETH_LoopbackIsEnabled(ETH_MODULE_ID index)
{
     return ETH_LoopbackIsEnabled_Default(index);
}

PLIB_INLINE_API void PLIB_ETH_TxPauseEnable(ETH_MODULE_ID index)
{
     ETH_TxPauseEnable_Default(index);
}

PLIB_INLINE_API void PLIB_ETH_TxPauseDisable(ETH_MODULE_ID index)
{
     ETH_TxPauseDisable_Default(index);
}

PLIB_INLINE_API bool PLIB_ETH_TxPauseIsEnabled(ETH_MODULE_ID index)
{
     return ETH_TxPauseIsEnabled_Default(index);
}

PLIB_INLINE_API void PLIB_ETH_RxPauseEnable(ETH_MODULE_ID index)
{
     ETH_RxPauseEnable_Default(index);
}

PLIB_INLINE_API void PLIB_ETH_RxPauseDisable(ETH_MODULE_ID index)
{
     ETH_RxPauseDisable_Default(index);
}

PLIB_INLINE_API bool PLIB_ETH_RxPauseIsEnabled(ETH_MODULE_ID index)
{
     return ETH_RxPauseIsEnabled_Default(index);
}

PLIB_INLINE_API void PLIB_ETH_PassAllEnable(ETH_MODULE_ID index)
{
     ETH_PassAllEnable_Default(index);
}

PLIB_INLINE_API void PLIB_ETH_PassAllDisable(ETH_MODULE_ID index)
{
     ETH_PassAllDisable_Default(index);
}

PLIB_INLINE_API bool PLIB_ETH_PassAllIsEnabled(ETH_MODULE_ID index)
{
     return ETH_PassAllIsEnabled_Default(index);
}

PLIB_INLINE_API void PLIB_ETH_ReceiveEnable(ETH_MODULE_ID index)
{
     ETH_ReceiveEnable_Default(index);
}

PLIB_INLINE_API void PLIB_ETH_ReceiveDisable(ETH_MODULE_ID index)
{
     ETH_ReceiveDisable_Default(index);
}

PLIB_INLINE_API bool PLIB_ETH_ReceiveIsEnabled(ETH_MODULE_ID index)
{
     return ETH_ReceiveIsEnabled_Default(index);
}

PLIB_INLINE_API void PLIB_ETH_ExcessDeferEnable(ETH_MODULE_ID index)
{
     ETH_ExcessDeferEnable_Default(index);
}

PLIB_INLINE_API void PLIB_ETH_ExcessDeferDisable(ETH_MODULE_ID index)
{
     ETH_ExcessDeferDisable_Default(index);
}

PLIB_INLINE_API bool PLIB_ETH_ExcessDeferIsEnabled(ETH_MODULE_ID index)
{
     return ETH_ExcessDeferIsEnabled_Default(index);
}

PLIB_INLINE_API void PLIB_ETH_BackPresNoBackoffEnable(ETH_MODULE_ID index)
{
     ETH_BackPresNoBackoffEnable_Default(index);
}

PLIB_INLINE_API void PLIB_ETH_BackPresNoBackoffDisable(ETH_MODULE_ID index)
{
     ETH_BackPresNoBackoffDisable_Default(index);
}

PLIB_INLINE_API bool PLIB_ETH_BackPresNoBackoffIsEnabled(ETH_MODULE_ID index)
{
     return ETH_BackPresNoBackoffIsEnabled_Default(index);
}

PLIB_INLINE_API void PLIB_ETH_NoBackoffEnable(ETH_MODULE_ID index)
{
     ETH_NoBackoffEnable_Default(index);
}

PLIB_INLINE_API void PLIB_ETH_NoBackoffDisable(ETH_MODULE_ID index)
{
     ETH_NoBackoffDisable_Default(index);
}

PLIB_INLINE_API bool PLIB_ETH_NoBackoffIsEnabled(ETH_MODULE_ID index)
{
     return ETH_NoBackoffIsEnabled_Default(index);
}

PLIB_INLINE_API void PLIB_ETH_LongPreambleEnable(ETH_MODULE_ID index)
{
     ETH_LongPreambleEnable_Default(index);
}

PLIB_INLINE_API void PLIB_ETH_LongPreambleDisable(ETH_MODULE_ID index)
{
     ETH_LongPreambleDisable_Default(index);
}

PLIB_INLINE_API bool PLIB_ETH_LongPreambleIsEnabled(ETH_MODULE_ID index)
{
     return ETH_LongPreambleIsEnabled_Default(index);
}

PLIB_INLINE_API void PLIB_ETH_PurePreambleEnable(ETH_MODULE_ID index)
{
     ETH_PurePreambleEnable_Default(index);
}

PLIB_INLINE_API void PLIB_ETH_PurePreambleDisable(ETH_MODULE_ID index)
{
     ETH_PurePreambleDisable_Default(index);
}

PLIB_INLINE_API bool PLIB_ETH_PurePreambleIsEnabled(ETH_MODULE_ID index)
{
     return ETH_PurePreambleIsEnabled_Default(index);
}

PLIB_INLINE_API ETH_AUTOPAD_OPTION PLIB_ETH_AutoDetectPadGet(ETH_MODULE_ID index)
{
     return ETH_AutoDetectPadGet_Default(index);
}

PLIB_INLINE_API void PLIB_ETH_AutoDetectPadSet(ETH_MODULE_ID index, ETH_AUTOPAD_OPTION option)
{
     ETH_AutoDetectPadSet_Default(index, option);
}

PLIB_INLINE_API void PLIB_ETH_AutoDetectPadClear(ETH_MODULE_ID index, ETH_AUTOPAD_OPTION option)
{
     ETH_AutoDetectPadClear_Default(index, option);
}

PLIB_INLINE_API void PLIB_ETH_CRCEnable(ETH_MODULE_ID index)
{
     ETH_CRCEnable_Default(index);
}

PLIB_INLINE_API void PLIB_ETH_CRCDisable(ETH_MODULE_ID index)
{
     ETH_CRCDisable_Default(index);
}

PLIB_INLINE_API bool PLIB_ETH_CRCIsEnabled(ETH_MODULE_ID index)
{
     return ETH_CRCIsEnabled_Default(index);
}

PLIB_INLINE_API void PLIB_ETH_DelayedCRCEnable(ETH_MODULE_ID index)
{
     ETH_DelayedCRCEnable_Default(index);
}

PLIB_INLINE_API void PLIB_ETH_DelayedCRCDisable(ETH_MODULE_ID index)
{
     ETH_DelayedCRCDisable_Default(index);
}

PLIB_INLINE_API bool PLIB_ETH_DelayedCRCIsEnabled(ETH_MODULE_ID index)
{
     return ETH_DelayedCRCIsEnabled_Default(index);
}

PLIB_INLINE_API void PLIB_ETH_HugeFrameEnable(ETH_MODULE_ID index)
{
     ETH_HugeFrameEnable_Default(index);
}

PLIB_INLINE_API void PLIB_ETH_HugeFrameDisable(ETH_MODULE_ID index)
{
     ETH_HugeFrameDisable_Default(index);
}

PLIB_INLINE_API bool PLIB_ETH_HugeFrameIsEnabled(ETH_MODULE_ID index)
{
     return ETH_HugeFrameIsEnabled_Default(index);
}

PLIB_INLINE_API void PLIB_ETH_FrameLengthCheckEnable(ETH_MODULE_ID index)
{
     ETH_FrameLengthCheckEnable_Default(index);
}

PLIB_INLINE_API void PLIB_ETH_FrameLengthCheckDisable(ETH_MODULE_ID index)
{
     ETH_FrameLengthCheckDisable_Default(index);
}

PLIB_INLINE_API bool PLIB_ETH_FrameLengthCheckIsEnabled(ETH_MODULE_ID index)
{
     return ETH_FrameLengthCheckIsEnabled_Default(index);
}

PLIB_INLINE_API void PLIB_ETH_FullDuplexEnable(ETH_MODULE_ID index)
{
     ETH_FullDuplexEnable_Default(index);
}

PLIB_INLINE_API void PLIB_ETH_FullDuplexDisable(ETH_MODULE_ID index)
{
     ETH_FullDuplexDisable_Default(index);
}

PLIB_INLINE_API bool PLIB_ETH_FullDuplexIsEnabled(ETH_MODULE_ID index)
{
     return ETH_FullDuplexIsEnabled_Default(index);
}

PLIB_INLINE_API bool PLIB_ETH_ExistsMAC_Configuration(ETH_MODULE_ID index)
{
     return ETH_ExistsMAC_Configuration_Default(index);
}

PLIB_INLINE_API uint8_t PLIB_ETH_BackToBackIPGGet(ETH_MODULE_ID index)
{
     return ETH_BackToBackIPGGet_Default(index);
}

PLIB_INLINE_API void PLIB_ETH_BackToBackIPGSet(ETH_MODULE_ID index, uint8_t backToBackIPGValue)
{
     ETH_BackToBackIPGSet_Default(index, backToBackIPGValue);
}

PLIB_INLINE_API uint8_t PLIB_ETH_NonBackToBackIPG1Get(ETH_MODULE_ID index)
{
     return ETH_NonBackToBackIPG1Get_Default(index);
}

PLIB_INLINE_API void PLIB_ETH_NonBackToBackIPG1Set(ETH_MODULE_ID index, uint8_t nonBackToBackIPGValue)
{
     ETH_NonBackToBackIPG1Set_Default(index, nonBackToBackIPGValue);
}

PLIB_INLINE_API uint8_t PLIB_ETH_NonBackToBackIPG2Get(ETH_MODULE_ID index)
{
     return ETH_NonBackToBackIPG2Get_Default(index);
}

PLIB_INLINE_API void PLIB_ETH_NonBackToBackIPG2Set(ETH_MODULE_ID index, uint8_t nonBackToBackIPGValue)
{
     ETH_NonBackToBackIPG2Set_Default(index, nonBackToBackIPGValue);
}

PLIB_INLINE_API bool PLIB_ETH_ExistsInterPacketGaps(ETH_MODULE_ID index)
{
     return ETH_ExistsInterPacketGaps_Default(index);
}

PLIB_INLINE_API uint8_t PLIB_ETH_CollisionWindowGet(ETH_MODULE_ID index)
{
     return ETH_CollisionWindowGet_Default(index);
}

PLIB_INLINE_API void PLIB_ETH_CollisionWindowSet(ETH_MODULE_ID index, uint8_t collisionWindowValue)
{
     ETH_CollisionWindowSet_Default(index, collisionWindowValue);
}

PLIB_INLINE_API bool PLIB_ETH_ExistsCollisionWindow(ETH_MODULE_ID index)
{
     return ETH_ExistsCollisionWindow_Default(index);
}

PLIB_INLINE_API uint8_t PLIB_ETH_ReTxMaxGet(ETH_MODULE_ID index)
{
     return ETH_ReTxMaxGet_Default(index);
}

PLIB_INLINE_API void PLIB_ETH_ReTxMaxSet(ETH_MODULE_ID index, uint16_t retransmitMax)
{
     ETH_ReTxMaxSet_Default(index, retransmitMax);
}

PLIB_INLINE_API bool PLIB_ETH_ExistsRetransmissionMaximum(ETH_MODULE_ID index)
{
     return ETH_ExistsRetransmissionMaximum_Default(index);
}

PLIB_INLINE_API uint16_t PLIB_ETH_MaxFrameLengthGet(ETH_MODULE_ID index)
{
     return ETH_MaxFrameLengthGet_Default(index);
}

PLIB_INLINE_API void PLIB_ETH_MaxFrameLengthSet(ETH_MODULE_ID index, uint16_t MaxFrameLength)
{
     ETH_MaxFrameLengthSet_Default(index, MaxFrameLength);
}

PLIB_INLINE_API void PLIB_ETH_MACSetMaxFrame(ETH_MODULE_ID index, uint16_t maxFrmSz)
{
     ETH_MACSetMaxFrame_Default(index, maxFrmSz);
}

PLIB_INLINE_API bool PLIB_ETH_ExistsMaxFrameLength(ETH_MODULE_ID index)
{
     return ETH_ExistsMaxFrameLength_Default(index);
}

PLIB_INLINE_API void PLIB_ETH_RMIIResetEnable(ETH_MODULE_ID index)
{
     ETH_RMIIResetEnable_Default(index);
}

PLIB_INLINE_API void PLIB_ETH_RMIIResetDisable(ETH_MODULE_ID index)
{
     ETH_RMIIResetDisable_Default(index);
}

PLIB_INLINE_API bool PLIB_ETH_RMIIResetIsEnabled(ETH_MODULE_ID index)
{
     return ETH_RMIIResetIsEnabled_Default(index);
}

PLIB_INLINE_API ETH_RMII_SPEED PLIB_ETH_RMIISpeedGet(ETH_MODULE_ID index)
{
     return ETH_RMIISpeedGet_Default(index);
}

PLIB_INLINE_API void PLIB_ETH_RMIISpeedSet(ETH_MODULE_ID index, ETH_RMII_SPEED RMIISpeed)
{
     ETH_RMIISpeedSet_Default(index, RMIISpeed);
}

PLIB_INLINE_API bool PLIB_ETH_ExistsRMII_Support(ETH_MODULE_ID index)
{
     return ETH_ExistsRMII_Support_Default(index);
}

PLIB_INLINE_API void PLIB_ETH_TestBackPressEnable(ETH_MODULE_ID index)
{
     ETH_TestBackPressEnable_Default(index);
}

PLIB_INLINE_API void PLIB_ETH_TestBackPressDisable(ETH_MODULE_ID index)
{
     ETH_TestBackPressDisable_Default(index);
}

PLIB_INLINE_API bool PLIB_ETH_TestBackPressIsEnabled(ETH_MODULE_ID index)
{
     return ETH_TestBackPressIsEnabled_Default(index);
}

PLIB_INLINE_API void PLIB_ETH_TestPauseEnable(ETH_MODULE_ID index)
{
     ETH_TestPauseEnable_Default(index);
}

PLIB_INLINE_API void PLIB_ETH_TestPauseDisable(ETH_MODULE_ID index)
{
     ETH_TestPauseDisable_Default(index);
}

PLIB_INLINE_API bool PLIB_ETH_TestPauseIsEnabled(ETH_MODULE_ID index)
{
     return ETH_TestPauseIsEnabled_Default(index);
}

PLIB_INLINE_API void PLIB_ETH_ShortcutQuantaEnable(ETH_MODULE_ID index)
{
     ETH_ShortcutQuantaEnable_Default(index);
}

PLIB_INLINE_API void PLIB_ETH_ShortcutQuantaDisable(ETH_MODULE_ID index)
{
     ETH_ShortcutQuantaDisable_Default(index);
}

PLIB_INLINE_API bool PLIB_ETH_ShortcutQuantaIsEnabled(ETH_MODULE_ID index)
{
     return ETH_ShortcutQuantaIsEnabled_Default(index);
}

PLIB_INLINE_API bool PLIB_ETH_ExistsMAC_Testing(ETH_MODULE_ID index)
{
     return ETH_ExistsMAC_Testing_Default(index);
}

PLIB_INLINE_API void PLIB_ETH_MIIMResetEnable(ETH_MODULE_ID index)
{
     ETH_MIIMResetEnable_Default(index);
}

PLIB_INLINE_API void PLIB_ETH_MIIMResetDisable(ETH_MODULE_ID index)
{
     ETH_MIIMResetDisable_Default(index);
}

PLIB_INLINE_API bool PLIB_ETH_MIIMResetIsEnabled(ETH_MODULE_ID index)
{
     return ETH_MIIMResetIsEnabled_Default(index);
}

PLIB_INLINE_API ETH_MIIM_CLK PLIB_ETH_MIIMClockGet(ETH_MODULE_ID index)
{
     return ETH_MIIMClockGet_Default(index);
}

PLIB_INLINE_API void PLIB_ETH_MIIMClockSet(ETH_MODULE_ID index, ETH_MIIM_CLK MIIMClock)
{
     ETH_MIIMClockSet_Default(index, MIIMClock);
}

PLIB_INLINE_API void PLIB_ETH_MIIMNoPreEnable(ETH_MODULE_ID index)
{
     ETH_MIIMNoPreEnable_Default(index);
}

PLIB_INLINE_API void PLIB_ETH_MIIMNoPreDisable(ETH_MODULE_ID index)
{
     ETH_MIIMNoPreDisable_Default(index);
}

PLIB_INLINE_API bool PLIB_ETH_MIIMNoPreIsEnabled(ETH_MODULE_ID index)
{
     return ETH_MIIMNoPreIsEnabled_Default(index);
}

PLIB_INLINE_API void PLIB_ETH_MIIMScanIncrEnable(ETH_MODULE_ID index)
{
     ETH_MIIMScanIncrEnable_Default(index);
}

PLIB_INLINE_API void PLIB_ETH_MIIMScanIncrDisable(ETH_MODULE_ID index)
{
     ETH_MIIMScanIncrDisable_Default(index);
}

PLIB_INLINE_API bool PLIB_ETH_MIIMScanIncrIsEnabled(ETH_MODULE_ID index)
{
     return ETH_MIIMScanIncrIsEnabled_Default(index);
}

PLIB_INLINE_API bool PLIB_ETH_ExistsMIIM_Config(ETH_MODULE_ID index)
{
     return ETH_ExistsMIIM_Config_Default(index);
}

PLIB_INLINE_API void PLIB_ETH_MIIMScanModeEnable(ETH_MODULE_ID index)
{
     ETH_MIIMScanModeEnable_Default(index);
}

PLIB_INLINE_API void PLIB_ETH_MIIMScanModeDisable(ETH_MODULE_ID index)
{
     ETH_MIIMScanModeDisable_Default(index);
}

PLIB_INLINE_API bool PLIB_ETH_MIIMScanModeIsEnabled(ETH_MODULE_ID index)
{
     return ETH_MIIMScanModeIsEnabled_Default(index);
}

PLIB_INLINE_API bool PLIB_ETH_ExistsMIIMScanMode(ETH_MODULE_ID index)
{
     return ETH_ExistsMIIMScanMode_Default(index);
}

PLIB_INLINE_API void PLIB_ETH_MIIMReadStart(ETH_MODULE_ID index)
{
     ETH_MIIMReadStart_Default(index);
}

PLIB_INLINE_API void PLIB_ETH_MIIMWriteStart(ETH_MODULE_ID index)
{
     ETH_MIIMWriteStart_Default(index);
}

PLIB_INLINE_API bool PLIB_ETH_ExistsMIIMReadWrite(ETH_MODULE_ID index)
{
     return ETH_ExistsMIIMReadWrite_Default(index);
}

PLIB_INLINE_API uint8_t PLIB_ETH_PHYAddressGet(ETH_MODULE_ID index)
{
     return ETH_PHYAddressGet_Default(index);
}

PLIB_INLINE_API void PLIB_ETH_PHYAddressSet(ETH_MODULE_ID index, uint8_t phyAddr)
{
     ETH_PHYAddressSet_Default(index, phyAddr);
}

PLIB_INLINE_API uint8_t PLIB_ETH_RegisterAddressGet(ETH_MODULE_ID index)
{
     return ETH_RegisterAddressGet_Default(index);
}

PLIB_INLINE_API void PLIB_ETH_RegisterAddressSet(ETH_MODULE_ID index, uint8_t regAddr)
{
     ETH_RegisterAddressSet_Default(index, regAddr);
}

PLIB_INLINE_API bool PLIB_ETH_ExistsMIIMAddresses(ETH_MODULE_ID index)
{
     return ETH_ExistsMIIMAddresses_Default(index);
}

PLIB_INLINE_API void PLIB_ETH_MIIMWriteDataSet(ETH_MODULE_ID index, uint16_t writeData)
{
     ETH_MIIMWriteDataSet_Default(index, writeData);
}

PLIB_INLINE_API uint16_t PLIB_ETH_MIIMReadDataGet(ETH_MODULE_ID index)
{
     return ETH_MIIMReadDataGet_Default(index);
}

PLIB_INLINE_API bool PLIB_ETH_ExistsMIIWriteReadData(ETH_MODULE_ID index)
{
     return ETH_ExistsMIIWriteReadData_Default(index);
}

PLIB_INLINE_API bool PLIB_ETH_LinkHasFailed(ETH_MODULE_ID index)
{
     return ETH_LinkHasFailed_Default(index);
}

PLIB_INLINE_API bool PLIB_ETH_DataNotValid(ETH_MODULE_ID index)
{
     return ETH_DataNotValid_Default(index);
}

PLIB_INLINE_API void PLIB_ETH_ClearDataNotValid(ETH_MODULE_ID index)
{
     ETH_ClearDataNotValid_Default(index);
}

PLIB_INLINE_API bool PLIB_ETH_MIIMIsScanning(ETH_MODULE_ID index)
{
     return ETH_MIIMIsScanning_Default(index);
}

PLIB_INLINE_API bool PLIB_ETH_MIIMIsBusy(ETH_MODULE_ID index)
{
     return ETH_MIIMIsBusy_Default(index);
}

PLIB_INLINE_API bool PLIB_ETH_ExistsMIIM_Indicators(ETH_MODULE_ID index)
{
     return ETH_ExistsMIIM_Indicators_Default(index);
}

PLIB_INLINE_API uint8_t PLIB_ETH_StationAddressGet(ETH_MODULE_ID index, uint8_t which)
{
     return ETH_StationAddressGet_Default(index, which);
}

PLIB_INLINE_API void PLIB_ETH_StationAddressSet(ETH_MODULE_ID index, uint8_t which, uint8_t stationAddress)
{
     ETH_StationAddressSet_Default(index, which, stationAddress);
}

PLIB_INLINE_API void PLIB_ETH_MACSetAddress(ETH_MODULE_ID index, uint8_t* bAddress)
{
     ETH_MACSetAddress_Default(index, bAddress);
}

PLIB_INLINE_API void PLIB_ETH_MACGetAddress(ETH_MODULE_ID index, uint8_t* bAddress)
{
     ETH_MACGetAddress_Default(index, bAddress);
}

PLIB_INLINE_API bool PLIB_ETH_ExistsStationAddress(ETH_MODULE_ID index)
{
     return ETH_ExistsStationAddress_Default(index);
}

#endif
