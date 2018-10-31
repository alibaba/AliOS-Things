/* Created by plibgen $Revision: 1.31 $ */

#ifndef _USART_P32MZ2048EFM124_H
#define _USART_P32MZ2048EFM124_H

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

    USART_ID_1 = _UART1_BASE_ADDRESS,
    USART_ID_2 = _UART2_BASE_ADDRESS,
    USART_ID_3 = _UART3_BASE_ADDRESS,
    USART_ID_4 = _UART4_BASE_ADDRESS,
    USART_ID_5 = _UART5_BASE_ADDRESS,
    USART_ID_6 = _UART6_BASE_ADDRESS,
    USART_NUMBER_OF_MODULES = 6

} USART_MODULE_ID;

typedef enum {

    USART_HANDSHAKE_MODE_FLOW_CONTROL = 0x00,
    USART_HANDSHAKE_MODE_SIMPLEX = 0x01

} USART_HANDSHAKE_MODE;

typedef enum {

    USART_ENABLE_TX_RX_BCLK_USED = 0x03,
    USART_ENABLE_TX_RX_CTS_RTS_USED = 0x02,
    USART_ENABLE_TX_RX_RTS_USED = 0x01,
    USART_ENABLE_TX_RX_USED = 0x00

} USART_OPERATION_MODE;

typedef enum {

    USART_8N1 = 0x00,
    USART_8E1 = 0x01,
    USART_8O1 = 0x02,
    USART_9N1 = 0x03,
    USART_8N2 = 0x04,
    USART_8E2 = 0x05,
    USART_8O2 = 0x06,
    USART_9N2 = 0x07

} USART_LINECONTROL_MODE;

typedef enum {

    USART_BRG_CLOCK_SOURCE_NONE

} USART_BRG_CLOCK_SOURCE;

typedef enum {

    USART_ERROR_NONE = 0x00,
    USART_ERROR_RECEIVER_OVERRUN = 0x01,
    USART_ERROR_FRAMING = 0x02,
    USART_ERROR_PARITY = 0x04

} USART_ERROR;

typedef enum {

    USART_TRANSMIT_FIFO_NOT_FULL = 0x00,
    USART_TRANSMIT_FIFO_IDLE = 0x01,
    USART_TRANSMIT_FIFO_EMPTY = 0x02

} USART_TRANSMIT_INTR_MODE;

typedef enum {

    USART_RECEIVE_FIFO_3B4FULL = 0x02,
    USART_RECEIVE_FIFO_HALF_FULL = 0x01,
    USART_RECEIVE_FIFO_ONE_CHAR = 0x00

} USART_RECEIVE_INTR_MODE;

/* Section 2 - Feature variant inclusion */

#define PLIB_TEMPLATE PLIB_INLINE
#include "../templates/usart_EnableControl_Default.h"
#include "../templates/usart_HandShakeMode_Default.h"
#include "../templates/usart_IrDAControl_Default.h"
#include "../templates/usart_LineControlMode_RXandTXCombined.h"
#include "../templates/usart_Loopback_Default.h"
#include "../templates/usart_OperationMode_Default.h"
#include "../templates/usart_StopInIdle_Default.h"
#include "../templates/usart_RunInOverflow_Unsupported.h"
#include "../templates/usart_BRGClockSourceSelect_Unsupported.h"
#include "../templates/usart_UsartModuleStatus_Unsupported.h"
#include "../templates/usart_RunInSleepMode_Unsupported.h"
#include "../templates/usart_WakeOnStart_Default.h"
#include "../templates/usart_BaudRate_In16BitRegister.h"
#include "../templates/usart_BaudRateAutoDetect_Default.h"
#include "../templates/usart_BaudRateHigh_In16BitRegister.h"
#include "../templates/usart_Receiver_Default.h"
#include "../templates/usart_Receiver9Bits_Default.h"
#include "../templates/usart_ReceiverAddressAutoDetect_Default.h"
#include "../templates/usart_ReceiverAddressDetect_Default.h"
#include "../templates/usart_ReceiverAddress_Default.h"
#include "../templates/usart_ReceiverAddressMask_Unsupported.h"
#include "../templates/usart_ReceiverDataAvailable_Default.h"
#include "../templates/usart_ReceiverEnableControl_Default.h"
#include "../templates/usart_ReceiverIdle_Default.h"
#include "../templates/usart_ReceiverFramingError_Default.h"
#include "../templates/usart_ReceiverInterruptMode_Default.h"
#include "../templates/usart_ReceiverPolarityInvert_Default.h"
#include "../templates/usart_ReceiverParityError_Default.h"
#include "../templates/usart_ReceiverOverrunError_Default.h"
#include "../templates/usart_Transmitter_Default.h"
#include "../templates/usart_Transmitter9Bits_InDataOnly.h"
#include "../templates/usart_TransmitterBreak_Default.h"
#include "../templates/usart_TransmitterBufferFull_Default.h"
#include "../templates/usart_TransmitterEmpty_Default.h"
#include "../templates/usart_TransmitterEnableControl_Default.h"
#include "../templates/usart_TransmitterInterruptMode_Default.h"
#include "../templates/usart_TransmitterIdleIsLow_pic32.h"

/* Section 3 - PLIB dispatch function definitions */

PLIB_INLINE_API bool PLIB_USART_ExistsEnable(USART_MODULE_ID index)
{
     return USART_ExistsEnable_Default(index);
}

PLIB_INLINE_API void PLIB_USART_Disable(USART_MODULE_ID index)
{
     USART_Disable_Default(index);
}

PLIB_INLINE_API void PLIB_USART_Enable(USART_MODULE_ID index)
{
     USART_Enable_Default(index);
}

PLIB_INLINE_API void PLIB_USART_BaudSetAndEnable(USART_MODULE_ID index, uint32_t systemClock, uint32_t baud)
{
     USART_BaudSetAndEnable_Default(index, systemClock, baud);
}

PLIB_INLINE_API bool PLIB_USART_ExistsHandshakeMode(USART_MODULE_ID index)
{
     return USART_ExistsHandshakeMode_Default(index);
}

PLIB_INLINE_API void PLIB_USART_HandshakeModeSelect(USART_MODULE_ID index, USART_HANDSHAKE_MODE handshakeConfig)
{
     USART_HandshakeModeSelect_Default(index, handshakeConfig);
}

PLIB_INLINE_API bool PLIB_USART_ExistsIrDA(USART_MODULE_ID index)
{
     return USART_ExistsIrDA_Default(index);
}

PLIB_INLINE_API void PLIB_USART_IrDADisable(USART_MODULE_ID index)
{
     USART_IrDADisable_Default(index);
}

PLIB_INLINE_API void PLIB_USART_IrDAEnable(USART_MODULE_ID index)
{
     USART_IrDAEnable_Default(index);
}

PLIB_INLINE_API bool PLIB_USART_ExistsLineControlMode(USART_MODULE_ID index)
{
     return USART_ExistsLineControlMode_RXandTXCombined(index);
}

PLIB_INLINE_API void PLIB_USART_LineControlModeSelect(USART_MODULE_ID index, USART_LINECONTROL_MODE dataFlowConfig)
{
     USART_LineControlModeSelect_RXandTXCombined(index, dataFlowConfig);
}

PLIB_INLINE_API bool PLIB_USART_ExistsLoopback(USART_MODULE_ID index)
{
     return USART_ExistsLoopback_Default(index);
}

PLIB_INLINE_API void PLIB_USART_LoopbackEnable(USART_MODULE_ID index)
{
     USART_LoopbackEnable_Default(index);
}

PLIB_INLINE_API void PLIB_USART_LoopbackDisable(USART_MODULE_ID index)
{
     USART_LoopbackDisable_Default(index);
}

PLIB_INLINE_API bool PLIB_USART_ExistsOperationMode(USART_MODULE_ID index)
{
     return USART_ExistsOperationMode_Default(index);
}

PLIB_INLINE_API void PLIB_USART_OperationModeSelect(USART_MODULE_ID index, USART_OPERATION_MODE operationmode)
{
     USART_OperationModeSelect_Default(index, operationmode);
}

PLIB_INLINE_API bool PLIB_USART_ExistsStopInIdle(USART_MODULE_ID index)
{
     return USART_ExistsStopInIdle_Default(index);
}

PLIB_INLINE_API void PLIB_USART_StopInIdleEnable(USART_MODULE_ID index)
{
     USART_StopInIdleEnable_Default(index);
}

PLIB_INLINE_API void PLIB_USART_StopInIdleDisable(USART_MODULE_ID index)
{
     USART_StopInIdleDisable_Default(index);
}

PLIB_INLINE_API bool PLIB_USART_ExistsRunInOverflow(USART_MODULE_ID index)
{
     return USART_ExistsRunInOverflow_Unsupported(index);
}

PLIB_INLINE_API void _PLIB_UNSUPPORTED PLIB_USART_RunInOverflowEnable(USART_MODULE_ID index)
{
     USART_RunInOverflowEnable_Unsupported(index);
}

PLIB_INLINE_API void _PLIB_UNSUPPORTED PLIB_USART_RunInOverflowDisable(USART_MODULE_ID index)
{
     USART_RunInOverflowDisable_Unsupported(index);
}

PLIB_INLINE_API bool _PLIB_UNSUPPORTED PLIB_USART_RunInOverflowIsEnabled(USART_MODULE_ID index)
{
     return USART_RunInOverflowIsEnabled_Unsupported(index);
}

PLIB_INLINE_API bool PLIB_USART_ExistsBRGClockSourceSelect(USART_MODULE_ID index)
{
     return USART_ExistsBRGClockSourceSelect_Unsupported(index);
}

PLIB_INLINE_API void _PLIB_UNSUPPORTED PLIB_USART_BRGClockSourceSelect(USART_MODULE_ID index, USART_BRG_CLOCK_SOURCE brgClockSource)
{
     USART_BRGClockSourceSelect_Unsupported(index, brgClockSource);
}

PLIB_INLINE_API USART_BRG_CLOCK_SOURCE _PLIB_UNSUPPORTED PLIB_USART_BRGClockSourceGet(USART_MODULE_ID index)
{
     return USART_BRGClockSourceGet_Unsupported(index);
}

PLIB_INLINE_API bool PLIB_USART_ExistsModuleBusyStatus(USART_MODULE_ID index)
{
     return USART_ExistsModuleBusyStatus_Unsupported(index);
}

PLIB_INLINE_API bool _PLIB_UNSUPPORTED PLIB_USART_ModuleIsBusy(USART_MODULE_ID index)
{
     return USART_ModuleIsBusy_Unsupported(index);
}

PLIB_INLINE_API bool PLIB_USART_ExistsRunInSleepMode(USART_MODULE_ID index)
{
     return USART_ExistsRunInSleepMode_Unsupported(index);
}

PLIB_INLINE_API void _PLIB_UNSUPPORTED PLIB_USART_RunInSleepModeEnable(USART_MODULE_ID index)
{
     USART_RunInSleepModeEnable_Unsupported(index);
}

PLIB_INLINE_API void _PLIB_UNSUPPORTED PLIB_USART_RunInSleepModeDisable(USART_MODULE_ID index)
{
     USART_RunInSleepModeDisable_Unsupported(index);
}

PLIB_INLINE_API bool _PLIB_UNSUPPORTED PLIB_USART_RunInSleepModeIsEnabled(USART_MODULE_ID index)
{
     return USART_RunInSleepModeIsEnabled_Unsupported(index);
}

PLIB_INLINE_API bool PLIB_USART_ExistsWakeOnStart(USART_MODULE_ID index)
{
     return USART_ExistsWakeOnStart_Default(index);
}

PLIB_INLINE_API void PLIB_USART_WakeOnStartEnable(USART_MODULE_ID index)
{
     USART_WakeOnStartEnable_Default(index);
}

PLIB_INLINE_API void PLIB_USART_WakeOnStartDisable(USART_MODULE_ID index)
{
     USART_WakeOnStartDisable_Default(index);
}

PLIB_INLINE_API bool PLIB_USART_WakeOnStartIsEnabled(USART_MODULE_ID index)
{
     return USART_WakeOnStartIsEnabled_Default(index);
}

PLIB_INLINE_API void PLIB_USART_InitializeModeGeneral(USART_MODULE_ID index, bool autobaud, bool loopBackMode, bool wakeFromSleep, bool irdaMode, bool stopInIdle)
{
     USART_InitializeModeGeneral_Default(index, autobaud, loopBackMode, wakeFromSleep, irdaMode, stopInIdle);
}

PLIB_INLINE_API USART_ERROR PLIB_USART_ErrorsGet(USART_MODULE_ID index)
{
     return USART_ErrorsGet_Default(index);
}

PLIB_INLINE_API bool PLIB_USART_ExistsBaudRate(USART_MODULE_ID index)
{
     return USART_ExistsBaudRate_In16BitRegister(index);
}

PLIB_INLINE_API void PLIB_USART_BaudRateSet(USART_MODULE_ID index, uint32_t clockFrequency, uint32_t baudRate)
{
     USART_BaudRateSet_In16BitRegister(index, clockFrequency, baudRate);
}

PLIB_INLINE_API uint32_t PLIB_USART_BaudRateGet(USART_MODULE_ID index, int32_t clockFrequency)
{
     return USART_BaudRateGet_In16BitRegister(index, clockFrequency);
}

PLIB_INLINE_API bool PLIB_USART_ExistsBaudRateAutoDetect(USART_MODULE_ID index)
{
     return USART_ExistsBaudRateAutoDetect_Default(index);
}

PLIB_INLINE_API void PLIB_USART_BaudRateAutoDetectEnable(USART_MODULE_ID index)
{
     USART_BaudRateAutoDetectEnable_Default(index);
}

PLIB_INLINE_API bool PLIB_USART_BaudRateAutoDetectIsComplete(USART_MODULE_ID index)
{
     return USART_BaudRateAutoDetectIsComplete_Default(index);
}

PLIB_INLINE_API bool PLIB_USART_ExistsBaudRateHigh(USART_MODULE_ID index)
{
     return USART_ExistsBaudRateHigh_In16BitRegister(index);
}

PLIB_INLINE_API void PLIB_USART_BaudRateHighSet(USART_MODULE_ID index, uint32_t clockFrequency, uint32_t baudRate)
{
     USART_BaudRateHighSet_In16BitRegister(index, clockFrequency, baudRate);
}

PLIB_INLINE_API void PLIB_USART_BaudRateHighDisable(USART_MODULE_ID index)
{
     USART_BaudRateHighDisable_In16BitRegister(index);
}

PLIB_INLINE_API void PLIB_USART_BaudRateHighEnable(USART_MODULE_ID index)
{
     USART_BaudRateHighEnable_In16BitRegister(index);
}

PLIB_INLINE_API bool PLIB_USART_ExistsReceiver(USART_MODULE_ID index)
{
     return USART_ExistsReceiver_Default(index);
}

PLIB_INLINE_API int8_t PLIB_USART_ReceiverByteReceive(USART_MODULE_ID index)
{
     return USART_ReceiverByteReceive_Default(index);
}

PLIB_INLINE_API void* PLIB_USART_ReceiverAddressGet(USART_MODULE_ID index)
{
     return USART_ReceiverAddressGet_Default(index);
}

PLIB_INLINE_API bool PLIB_USART_ExistsReceiver9Bits(USART_MODULE_ID index)
{
     return USART_ExistsReceiver9Bits_Default(index);
}

PLIB_INLINE_API int16_t PLIB_USART_Receiver9BitsReceive(USART_MODULE_ID index)
{
     return USART_Receiver9BitsReceive_Default(index);
}

PLIB_INLINE_API bool PLIB_USART_ExistsReceiverAddressAutoDetect(USART_MODULE_ID index)
{
     return USART_ExistsReceiverAddressAutoDetect_Default(index);
}

PLIB_INLINE_API void PLIB_USART_ReceiverAddressAutoDetectEnable(USART_MODULE_ID index, int8_t Mask)
{
     USART_ReceiverAddressAutoDetectEnable_Default(index, Mask);
}

PLIB_INLINE_API void PLIB_USART_ReceiverAddressAutoDetectDisable(USART_MODULE_ID index)
{
     USART_ReceiverAddressAutoDetectDisable_Default(index);
}

PLIB_INLINE_API bool PLIB_USART_ExistsReceiverAddressDetect(USART_MODULE_ID index)
{
     return USART_ExistsReceiverAddressDetect_Default(index);
}

PLIB_INLINE_API void PLIB_USART_ReceiverAddressDetectEnable(USART_MODULE_ID index)
{
     USART_ReceiverAddressDetectEnable_Default(index);
}

PLIB_INLINE_API void PLIB_USART_ReceiverAddressDetectDisable(USART_MODULE_ID index)
{
     USART_ReceiverAddressDetectDisable_Default(index);
}

PLIB_INLINE_API bool PLIB_USART_ReceiverAddressIsReceived(USART_MODULE_ID index)
{
     return USART_ReceiverAddressIsReceived_Default(index);
}

PLIB_INLINE_API bool PLIB_USART_ExistsReceiverAddress(USART_MODULE_ID index)
{
     return USART_ExistsReceiverAddress_Default(index);
}

PLIB_INLINE_API void PLIB_USART_AddressSet(USART_MODULE_ID index, uint8_t address)
{
     USART_AddressSet_Default(index, address);
}

PLIB_INLINE_API uint8_t PLIB_USART_AddressGet(USART_MODULE_ID index)
{
     return USART_AddressGet_Default(index);
}

PLIB_INLINE_API bool PLIB_USART_ExistsReceiverAddressMask(USART_MODULE_ID index)
{
     return USART_ExistsReceiverAddressMask_Unsupported(index);
}

PLIB_INLINE_API void _PLIB_UNSUPPORTED PLIB_USART_AddressMaskSet(USART_MODULE_ID index, uint8_t mask)
{
     USART_AddressMaskSet_Unsupported(index, mask);
}

PLIB_INLINE_API uint8_t _PLIB_UNSUPPORTED PLIB_USART_AddressMaskGet(USART_MODULE_ID index)
{
     return USART_AddressMaskGet_Unsupported(index);
}

PLIB_INLINE_API bool PLIB_USART_ExistsReceiverDataAvailableStatus(USART_MODULE_ID index)
{
     return USART_ExistsReceiverDataAvailableStatus_Default(index);
}

PLIB_INLINE_API bool PLIB_USART_ReceiverDataIsAvailable(USART_MODULE_ID index)
{
     return USART_ReceiverDataIsAvailable_Default(index);
}

PLIB_INLINE_API bool PLIB_USART_ExistsReceiverEnable(USART_MODULE_ID index)
{
     return USART_ExistsReceiverEnable_Default(index);
}

PLIB_INLINE_API void PLIB_USART_ReceiverEnable(USART_MODULE_ID index)
{
     USART_ReceiverEnable_Default(index);
}

PLIB_INLINE_API void PLIB_USART_ReceiverDisable(USART_MODULE_ID index)
{
     USART_ReceiverDisable_Default(index);
}

PLIB_INLINE_API bool PLIB_USART_ExistsReceiverIdleStatus(USART_MODULE_ID index)
{
     return USART_ExistsReceiverIdleStatus_Default(index);
}

PLIB_INLINE_API bool PLIB_USART_ReceiverIsIdle(USART_MODULE_ID index)
{
     return USART_ReceiverIsIdle_Default(index);
}

PLIB_INLINE_API bool PLIB_USART_ExistsReceiverFramingErrorStatus(USART_MODULE_ID index)
{
     return USART_ExistsReceiverFramingErrorStatus_Default(index);
}

PLIB_INLINE_API bool PLIB_USART_ReceiverFramingErrorHasOccurred(USART_MODULE_ID index)
{
     return USART_ReceiverFramingErrorHasOccurred_Default(index);
}

PLIB_INLINE_API bool PLIB_USART_ExistsReceiverInterruptMode(USART_MODULE_ID index)
{
     return USART_ExistsReceiverInterruptMode_Default(index);
}

PLIB_INLINE_API void PLIB_USART_ReceiverInterruptModeSelect(USART_MODULE_ID index, USART_RECEIVE_INTR_MODE interruptMode)
{
     USART_ReceiverInterruptModeSelect_Default(index, interruptMode);
}

PLIB_INLINE_API void PLIB_USART_InitializeOperation(USART_MODULE_ID index, USART_RECEIVE_INTR_MODE receiveInterruptMode, USART_TRANSMIT_INTR_MODE transmitInterruptMode, USART_OPERATION_MODE operationMode)
{
     USART_InitializeOperation_Default(index, receiveInterruptMode, transmitInterruptMode, operationMode);
}

PLIB_INLINE_API bool PLIB_USART_ExistsReceiverIdleStateLowEnable(USART_MODULE_ID index)
{
     return USART_ExistsReceiverIdleStateLowEnable_Default(index);
}

PLIB_INLINE_API void PLIB_USART_ReceiverIdleStateLowEnable(USART_MODULE_ID index)
{
     USART_ReceiverIdleStateLowEnable_Default(index);
}

PLIB_INLINE_API void PLIB_USART_ReceiverIdleStateLowDisable(USART_MODULE_ID index)
{
     USART_ReceiverIdleStateLowDisable_Default(index);
}

PLIB_INLINE_API bool PLIB_USART_ExistsReceiverParityErrorStatus(USART_MODULE_ID index)
{
     return USART_ExistsReceiverParityErrorStatus_Default(index);
}

PLIB_INLINE_API bool PLIB_USART_ReceiverParityErrorHasOccurred(USART_MODULE_ID index)
{
     return USART_ReceiverParityErrorHasOccurred_Default(index);
}

PLIB_INLINE_API bool PLIB_USART_ExistsReceiverOverrunStatus(USART_MODULE_ID index)
{
     return USART_ExistsReceiverOverrunStatus_Default(index);
}

PLIB_INLINE_API void PLIB_USART_ReceiverOverrunErrorClear(USART_MODULE_ID index)
{
     USART_ReceiverOverrunErrorClear_Default(index);
}

PLIB_INLINE_API bool PLIB_USART_ReceiverOverrunHasOccurred(USART_MODULE_ID index)
{
     return USART_ReceiverOverrunHasOccurred_Default(index);
}

PLIB_INLINE_API bool PLIB_USART_ExistsTransmitter(USART_MODULE_ID index)
{
     return USART_ExistsTransmitter_Default(index);
}

PLIB_INLINE_API void PLIB_USART_TransmitterByteSend(USART_MODULE_ID index, int8_t data)
{
     USART_TransmitterByteSend_Default(index, data);
}

PLIB_INLINE_API void* PLIB_USART_TransmitterAddressGet(USART_MODULE_ID index)
{
     return USART_TransmitterAddressGet_Default(index);
}

PLIB_INLINE_API bool PLIB_USART_ExistsTransmitter9BitsSend(USART_MODULE_ID index)
{
     return USART_ExistsTransmitter9BitsSend_InDataOnly(index);
}

PLIB_INLINE_API void PLIB_USART_Transmitter9BitsSend(USART_MODULE_ID index, int8_t data, bool Bit9th)
{
     USART_Transmitter9BitsSend_InDataOnly(index, data, Bit9th);
}

PLIB_INLINE_API bool PLIB_USART_ExistsTransmitterBreak(USART_MODULE_ID index)
{
     return USART_ExistsTransmitterBreak_Default(index);
}

PLIB_INLINE_API void PLIB_USART_TransmitterBreakSend(USART_MODULE_ID index)
{
     USART_TransmitterBreakSend_Default(index);
}

PLIB_INLINE_API bool PLIB_USART_TransmitterBreakSendIsComplete(USART_MODULE_ID index)
{
     return USART_TransmitterBreakSendIsComplete_Default(index);
}

PLIB_INLINE_API bool PLIB_USART_ExistsTransmitterBufferFullStatus(USART_MODULE_ID index)
{
     return USART_ExistsTransmitterBufferFullStatus_Default(index);
}

PLIB_INLINE_API bool PLIB_USART_TransmitterBufferIsFull(USART_MODULE_ID index)
{
     return USART_TransmitterBufferIsFull_Default(index);
}

PLIB_INLINE_API bool PLIB_USART_TransmitterIsEmpty(USART_MODULE_ID index)
{
     return USART_TransmitterIsEmpty_Default(index);
}

PLIB_INLINE_API bool PLIB_USART_ExistsTransmitterEmptyStatus(USART_MODULE_ID index)
{
     return USART_ExistsTransmitterEmptyStatus_Default(index);
}

PLIB_INLINE_API bool PLIB_USART_ExistsTransmitterEnable(USART_MODULE_ID index)
{
     return USART_ExistsTransmitterEnable_Default(index);
}

PLIB_INLINE_API void PLIB_USART_TransmitterEnable(USART_MODULE_ID index)
{
     USART_TransmitterEnable_Default(index);
}

PLIB_INLINE_API void PLIB_USART_TransmitterDisable(USART_MODULE_ID index)
{
     USART_TransmitterDisable_Default(index);
}

PLIB_INLINE_API void PLIB_USART_TransmitterInterruptModeSelect(USART_MODULE_ID index, USART_TRANSMIT_INTR_MODE fifolevel)
{
     USART_TransmitterInterruptModeSelect_Default(index, fifolevel);
}

PLIB_INLINE_API bool PLIB_USART_ExistsTransmitterInterruptMode(USART_MODULE_ID index)
{
     return USART_ExistsTransmitterInterruptMode_Default(index);
}

PLIB_INLINE_API bool PLIB_USART_ExistsTransmitterIdleIsLow(USART_MODULE_ID index)
{
     return USART_ExistsTransmitterIdleIsLow_pic32(index);
}

PLIB_INLINE_API void PLIB_USART_TransmitterIdleIsLowDisable(USART_MODULE_ID index)
{
     USART_TransmitterIdleIsLowDisable_pic32(index);
}

PLIB_INLINE_API void PLIB_USART_TransmitterIdleIsLowEnable(USART_MODULE_ID index)
{
     USART_TransmitterIdleIsLowEnable_pic32(index);
}

#endif
