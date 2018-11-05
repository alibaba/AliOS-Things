/* Created by plibgen $Revision: 1.31 $ */

#ifndef _SPI_P32MZ2048EFM064_H
#define _SPI_P32MZ2048EFM064_H

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

    SPI_ID_1 = _SPI1_BASE_ADDRESS,
    SPI_ID_2 = _SPI2_BASE_ADDRESS,
    SPI_ID_3 = _SPI3_BASE_ADDRESS,
    SPI_ID_4 = _SPI4_BASE_ADDRESS,
    SPI_NUMBER_OF_MODULES = 4

} SPI_MODULE_ID;

typedef enum {

    SPI_INPUT_SAMPLING_PHASE_IN_MIDDLE = 0,
    SPI_INPUT_SAMPLING_PHASE_AT_END = 1

} SPI_INPUT_SAMPLING_PHASE;

typedef enum {

    SPI_OUTPUT_DATA_PHASE_ON_IDLE_TO_ACTIVE_CLOCK = 0,
    SPI_OUTPUT_DATA_PHASE_ON_ACTIVE_TO_IDLE_CLOCK = 1

} SPI_OUTPUT_DATA_PHASE;

typedef enum {

    SPI_COMMUNICATION_WIDTH_8BITS = 0,
    SPI_COMMUNICATION_WIDTH_16BITS = 1,
    SPI_COMMUNICATION_WIDTH_32BITS = 2

} SPI_COMMUNICATION_WIDTH;

typedef enum {

    SPI_CLOCK_POLARITY_IDLE_LOW = 0,
    SPI_CLOCK_POLARITY_IDLE_HIGH = 1

} SPI_CLOCK_POLARITY;

typedef enum {

    SPI_FIFO_INTERRUPT_WHEN_TRANSMIT_BUFFER_IS_NOT_FULL = 0,
    SPI_FIFO_INTERRUPT_WHEN_TRANSMIT_BUFFER_IS_1HALF_EMPTY_OR_MORE = 1,
    SPI_FIFO_INTERRUPT_WHEN_TRANSMIT_BUFFER_IS_COMPLETELY_EMPTY = 2,
    SPI_FIFO_INTERRUPT_WHEN_TRANSMISSION_IS_COMPLETE = 3,
    SPI_FIFO_INTERRUPT_WHEN_RECEIVE_BUFFER_IS_FULL = 4,
    SPI_FIFO_INTERRUPT_WHEN_RECEIVE_BUFFER_IS_1HALF_FULL_OR_MORE = 5,
    SPI_FIFO_INTERRUPT_WHEN_RECEIVE_BUFFER_IS_NOT_EMPTY = 6,
    SPI_FIFO_INTERRUPT_WHEN_BUFFER_IS_EMPTY = 7

} SPI_FIFO_INTERRUPT;

typedef enum {

    SPI_ERROR_INTERRUPT_FRAME_ERROR_OVERFLOW = 0x00001000,
    SPI_ERROR_INTERRUPT_RECEIVE_OVERFLOW = 0x00000800,
    SPI_ERROR_INTERRUPT_TRANSMIT_UNDERRUN = 0x00000400

} SPI_ERROR_INTERRUPT;

typedef enum {

    SPI_PIN_SLAVE_SELECT = 0,
    SPI_PIN_DATA_IN = 1,
    SPI_PIN_DATA_OUT = 2

} SPI_PIN;

typedef enum {

    SPI_FIFO_TYPE_RECEIVE = 0,
    SPI_FIFO_TYPE_TRANSMIT = 1

} SPI_FIFO_TYPE;

typedef enum {

    SPI_BAUD_RATE_PBCLK_CLOCK = 0,
    SPI_BAUD_RATE_MCLK_CLOCK = 1

} SPI_BAUD_RATE_CLOCK;

typedef enum {

    SPI_FRAME_SYNC_PULSE_ON_EVERY_DATA_CHARACTER = 0,
    SPI_FRAME_SYNC_PULSE_ON_EVERY_2_DATA_CHARACTER = 1,
    SPI_FRAME_SYNC_PULSE_ON_EVERY_4_DATA_CHARACTER = 2,
    SPI_FRAME_SYNC_PULSE_ON_EVERY_8_DATA_CHARACTER = 3,
    SPI_FRAME_SYNC_PULSE_ON_EVERY_16_DATA_CHARACTER = 4,
    SPI_FRAME_SYNC_PULSE_ON_EVERY_32_DATA_CHARACTER = 5

} SPI_FRAME_SYNC_PULSE;

typedef enum {

    SPI_FRAME_PULSE_POLARITY_ACTIVE_LOW = 0,
    SPI_FRAME_PULSE_POLARITY_ACTIVE_HIGH = 1

} SPI_FRAME_PULSE_POLARITY;

typedef enum {

    SPI_FRAME_PULSE_DIRECTION_OUTPUT = 0,
    SPI_FRAME_PULSE_DIRECTION_INPUT = 1

} SPI_FRAME_PULSE_DIRECTION;

typedef enum {

    SPI_FRAME_PULSE_EDGE_PRECEDES_FIRST_BIT_CLOCK = 0,
    SPI_FRAME_PULSE_EDGE_COINCIDES_FIRST_BIT_CLOCK = 1

} SPI_FRAME_PULSE_EDGE;

typedef enum {

    SPI_FRAME_PULSE_WIDTH_ONE_CLOCK_WIDE = 0,
    SPI_FRAME_PULSE_WIDTH_ONE_WORD_LENGTH = 1

} SPI_FRAME_PULSE_WIDTH;

typedef enum {

    SPI_AUDIO_PROTOCOL_I2S = 0,
    SPI_AUDIO_PROTOCOL_LEFT_JUSTIFIED = 1,
    SPI_AUDIO_PROTOCOL_RIGHT_JUSTIFIED = 2,
    SPI_AUDIO_PROTOCOL_PCM_DSP = 3

} SPI_AUDIO_PROTOCOL;

typedef enum {

    SPI_AUDIO_TRANSMIT_STEREO = 0,
    SPI_AUDIO_TRANSMIT_MONO = 1

} SPI_AUDIO_TRANSMIT_MODE;

typedef enum {

    SPI_AUDIO_ERROR_RECEIVE_OVERFLOW = 0x00000200,
    SPI_AUDIO_ERROR_TRANSMIT_UNDERRUN = 0x00000100

} SPI_AUDIO_ERROR;

typedef enum {

    SPI_AUDIO_COMMUNICATION_16DATA_16FIFO_16CHANNEL = 0,
    SPI_AUDIO_COMMUNICATION_16DATA_16FIFO_32CHANNEL = 1,
    SPI_AUDIO_COMMUNICATION_32DATA_32FIFO_32CHANNEL = 2,
    SPI_AUDIO_COMMUNICATION_24DATA_32FIFO_32CHANNEL = 3

} SPI_AUDIO_COMMUNICATION_WIDTH;

/* Section 2 - Feature variant inclusion */

#define PLIB_TEMPLATE PLIB_INLINE
#include "../templates/spi_EnableControl_Default.h"
#include "../templates/spi_StopInIdle_Default.h"
#include "../templates/spi_ReceiverOverflow_Default.h"
#include "../templates/spi_TransmitBufferFullStatus_Default.h"
#include "../templates/spi_TransmitBufferEmptyStatus_Default.h"
#include "../templates/spi_ReceiveBufferStatus_Default.h"
#include "../templates/spi_PinControl_PIC32.h"
#include "../templates/spi_CommunicationWidth_PIC32.h"
#include "../templates/spi_AudioCommunicationWidth_PIC32.h"
#include "../templates/spi_InputSamplePhase_Default.h"
#include "../templates/spi_OutputDataPhase_Default.h"
#include "../templates/spi_ClockPolarity_Default.h"
#include "../templates/spi_MasterControl_Default.h"
#include "../templates/spi_BaudRate_Default.h"
#include "../templates/spi_BusStatus_Default.h"
#include "../templates/spi_ReadDataSignStatus_Default.h"
#include "../templates/spi_SlaveSelectControl_Default.h"
#include "../templates/spi_TransmitUnderRunStatus_Default.h"
#include "../templates/spi_FIFOControl_Default.h"
#include "../templates/spi_FIFOCount_PIC32.h"
#include "../templates/spi_ReceiveFIFOStatus_Default.h"
#include "../templates/spi_FIFOShiftRegisterEmptyStatus_Default.h"
#include "../templates/spi_FIFOInterruptMode_PIC32.h"
#include "../templates/spi_FramedCommunication_Default.h"
#include "../templates/spi_FrameSyncPulseDirection_Default.h"
#include "../templates/spi_FrameSyncPulsePolarity_Default.h"
#include "../templates/spi_FrameSyncPulseEdge_Default.h"
#include "../templates/spi_FrameSyncPulseWidth_Default.h"
#include "../templates/spi_FrameSyncPulseCounter_PIC32.h"
#include "../templates/spi_FrameErrorStatus_Default.h"
#include "../templates/spi_Buffer_Default.h"
#include "../templates/spi_BaudRateClock_Default.h"
#include "../templates/spi_ErrorInterruptControl_Default.h"
#include "../templates/spi_AudioErrorControl_Default.h"
#include "../templates/spi_AudioProtocolControl_Default.h"
#include "../templates/spi_AudioTransmitMode_Default.h"
#include "../templates/spi_AudioProtocolMode_Default.h"
#include "../templates/spi_Buffer32bit_Default.h"
#include "../templates/spi_Buffer16bit_Default.h"

/* Section 3 - PLIB dispatch function definitions */

PLIB_INLINE_API void PLIB_SPI_Enable(SPI_MODULE_ID index)
{
     SPI_Enable_Default(index);
}

PLIB_INLINE_API void PLIB_SPI_Disable(SPI_MODULE_ID index)
{
     SPI_Disable_Default(index);
}

PLIB_INLINE_API bool PLIB_SPI_ExistsEnableControl(SPI_MODULE_ID index)
{
     return SPI_ExistsEnableControl_Default(index);
}

PLIB_INLINE_API void PLIB_SPI_StopInIdleEnable(SPI_MODULE_ID index)
{
     SPI_StopInIdleEnable_Default(index);
}

PLIB_INLINE_API void PLIB_SPI_StopInIdleDisable(SPI_MODULE_ID index)
{
     SPI_StopInIdleDisable_Default(index);
}

PLIB_INLINE_API bool PLIB_SPI_ExistsStopInIdleControl(SPI_MODULE_ID index)
{
     return SPI_ExistsStopInIdleControl_Default(index);
}

PLIB_INLINE_API bool PLIB_SPI_ReceiverHasOverflowed(SPI_MODULE_ID index)
{
     return SPI_ReceiverHasOverflowed_Default(index);
}

PLIB_INLINE_API void PLIB_SPI_ReceiverOverflowClear(SPI_MODULE_ID index)
{
     SPI_ReceiverOverflowClear_Default(index);
}

PLIB_INLINE_API bool PLIB_SPI_ExistsReceiverOverflow(SPI_MODULE_ID index)
{
     return SPI_ExistsReceiverOverflow_Default(index);
}

PLIB_INLINE_API bool PLIB_SPI_TransmitBufferIsFull(SPI_MODULE_ID index)
{
     return SPI_TransmitBufferIsFull_Default(index);
}

PLIB_INLINE_API bool PLIB_SPI_ExistsTransmitBufferFullStatus(SPI_MODULE_ID index)
{
     return SPI_ExistsTransmitBufferFullStatus_Default(index);
}

PLIB_INLINE_API bool PLIB_SPI_TransmitBufferIsEmpty(SPI_MODULE_ID index)
{
     return SPI_TransmitBufferIsEmpty_Default(index);
}

PLIB_INLINE_API bool PLIB_SPI_ExistsTransmitBufferEmptyStatus(SPI_MODULE_ID index)
{
     return SPI_ExistsTransmitBufferEmptyStatus_Default(index);
}

PLIB_INLINE_API bool PLIB_SPI_ReceiverBufferIsFull(SPI_MODULE_ID index)
{
     return SPI_ReceiverBufferIsFull_Default(index);
}

PLIB_INLINE_API bool PLIB_SPI_ExistsReceiveBufferStatus(SPI_MODULE_ID index)
{
     return SPI_ExistsReceiveBufferStatus_Default(index);
}

PLIB_INLINE_API void PLIB_SPI_PinEnable(SPI_MODULE_ID index, SPI_PIN pin)
{
     SPI_PinEnable_PIC32(index, pin);
}

PLIB_INLINE_API void PLIB_SPI_PinDisable(SPI_MODULE_ID index, SPI_PIN pin)
{
     SPI_PinDisable_PIC32(index, pin);
}

PLIB_INLINE_API bool PLIB_SPI_ExistsPinControl(SPI_MODULE_ID index)
{
     return SPI_ExistsPinControl_PIC32(index);
}

PLIB_INLINE_API void PLIB_SPI_CommunicationWidthSelect(SPI_MODULE_ID index, SPI_COMMUNICATION_WIDTH width)
{
     SPI_CommunicationWidthSelect_PIC32(index, width);
}

PLIB_INLINE_API bool PLIB_SPI_ExistsCommunicationWidth(SPI_MODULE_ID index)
{
     return SPI_ExistsCommunicationWidth_PIC32(index);
}

PLIB_INLINE_API void PLIB_SPI_AudioCommunicationWidthSelect(SPI_MODULE_ID index, SPI_AUDIO_COMMUNICATION_WIDTH mode)
{
     SPI_AudioCommunicationWidthSelect_PIC32(index, mode);
}

PLIB_INLINE_API bool PLIB_SPI_ExistsAudioCommunicationWidth(SPI_MODULE_ID index)
{
     return SPI_ExistsAudioCommunicationWidth_PIC32(index);
}

PLIB_INLINE_API void PLIB_SPI_InputSamplePhaseSelect(SPI_MODULE_ID index, SPI_INPUT_SAMPLING_PHASE phase)
{
     SPI_InputSamplePhaseSelect_Default(index, phase);
}

PLIB_INLINE_API bool PLIB_SPI_ExistsInputSamplePhase(SPI_MODULE_ID index)
{
     return SPI_ExistsInputSamplePhase_Default(index);
}

PLIB_INLINE_API void PLIB_SPI_OutputDataPhaseSelect(SPI_MODULE_ID index, SPI_OUTPUT_DATA_PHASE phase)
{
     SPI_OutputDataPhaseSelect_Default(index, phase);
}

PLIB_INLINE_API bool PLIB_SPI_ExistsOutputDataPhase(SPI_MODULE_ID index)
{
     return SPI_ExistsOutputDataPhase_Default(index);
}

PLIB_INLINE_API void PLIB_SPI_ClockPolaritySelect(SPI_MODULE_ID index, SPI_CLOCK_POLARITY polarity)
{
     SPI_ClockPolaritySelect_Default(index, polarity);
}

PLIB_INLINE_API bool PLIB_SPI_ExistsClockPolarity(SPI_MODULE_ID index)
{
     return SPI_ExistsClockPolarity_Default(index);
}

PLIB_INLINE_API void PLIB_SPI_MasterEnable(SPI_MODULE_ID index)
{
     SPI_MasterEnable_Default(index);
}

PLIB_INLINE_API void PLIB_SPI_SlaveEnable(SPI_MODULE_ID index)
{
     SPI_SlaveEnable_Default(index);
}

PLIB_INLINE_API bool PLIB_SPI_ExistsMasterControl(SPI_MODULE_ID index)
{
     return SPI_ExistsMasterControl_Default(index);
}

PLIB_INLINE_API void PLIB_SPI_BaudRateSet(SPI_MODULE_ID index, uint32_t clockFrequency, uint32_t baudRate)
{
     SPI_BaudRateSet_Default(index, clockFrequency, baudRate);
}

PLIB_INLINE_API bool PLIB_SPI_ExistsBaudRate(SPI_MODULE_ID index)
{
     return SPI_ExistsBaudRate_Default(index);
}

PLIB_INLINE_API bool PLIB_SPI_IsBusy(SPI_MODULE_ID index)
{
     return SPI_IsBusy_Default(index);
}

PLIB_INLINE_API bool PLIB_SPI_ExistsBusStatus(SPI_MODULE_ID index)
{
     return SPI_ExistsBusStatus_Default(index);
}

PLIB_INLINE_API bool PLIB_SPI_ReadDataIsSignExtended(SPI_MODULE_ID index)
{
     return SPI_ReadDataIsSignExtended_Default(index);
}

PLIB_INLINE_API bool PLIB_SPI_ExistsReadDataSignStatus(SPI_MODULE_ID index)
{
     return SPI_ExistsReadDataSignStatus_Default(index);
}

PLIB_INLINE_API void PLIB_SPI_SlaveSelectEnable(SPI_MODULE_ID index)
{
     SPI_SlaveSelectEnable_Default(index);
}

PLIB_INLINE_API void PLIB_SPI_SlaveSelectDisable(SPI_MODULE_ID index)
{
     SPI_SlaveSelectDisable_Default(index);
}

PLIB_INLINE_API bool PLIB_SPI_ExistsSlaveSelectControl(SPI_MODULE_ID index)
{
     return SPI_ExistsSlaveSelectControl_Default(index);
}

PLIB_INLINE_API bool PLIB_SPI_TransmitUnderRunStatusGet(SPI_MODULE_ID index)
{
     return SPI_TransmitUnderRunStatusGet_Default(index);
}

PLIB_INLINE_API void PLIB_SPI_TransmitUnderRunStatusClear(SPI_MODULE_ID index)
{
     SPI_TransmitUnderRunStatusClear_Default(index);
}

PLIB_INLINE_API bool PLIB_SPI_ExistsTransmitUnderRunStatus(SPI_MODULE_ID index)
{
     return SPI_ExistsTransmitUnderRunStatus_Default(index);
}

PLIB_INLINE_API void PLIB_SPI_FIFOEnable(SPI_MODULE_ID index)
{
     SPI_FIFOEnable_Default(index);
}

PLIB_INLINE_API void PLIB_SPI_FIFODisable(SPI_MODULE_ID index)
{
     SPI_FIFODisable_Default(index);
}

PLIB_INLINE_API bool PLIB_SPI_ExistsFIFOControl(SPI_MODULE_ID index)
{
     return SPI_ExistsFIFOControl_Default(index);
}

PLIB_INLINE_API uint8_t PLIB_SPI_FIFOCountGet(SPI_MODULE_ID index, SPI_FIFO_TYPE type)
{
     return SPI_FIFOCountGet_PIC32(index, type);
}

PLIB_INLINE_API bool PLIB_SPI_ExistsFIFOCount(SPI_MODULE_ID index)
{
     return SPI_ExistsFIFOCount_PIC32(index);
}

PLIB_INLINE_API bool PLIB_SPI_ReceiverFIFOIsEmpty(SPI_MODULE_ID index)
{
     return SPI_ReceiverFIFOIsEmpty_Default(index);
}

PLIB_INLINE_API bool PLIB_SPI_ExistsReceiveFIFOStatus(SPI_MODULE_ID index)
{
     return SPI_ExistsReceiveFIFOStatus_Default(index);
}

PLIB_INLINE_API bool PLIB_SPI_FIFOShiftRegisterIsEmpty(SPI_MODULE_ID index)
{
     return SPI_FIFOShiftRegisterIsEmpty_Default(index);
}

PLIB_INLINE_API bool PLIB_SPI_ExistsFIFOShiftRegisterEmptyStatus(SPI_MODULE_ID index)
{
     return SPI_ExistsFIFOShiftRegisterEmptyStatus_Default(index);
}

PLIB_INLINE_API void PLIB_SPI_FIFOInterruptModeSelect(SPI_MODULE_ID index, SPI_FIFO_INTERRUPT mode)
{
     SPI_FIFOInterruptModeSelect_PIC32(index, mode);
}

PLIB_INLINE_API bool PLIB_SPI_ExistsFIFOInterruptMode(SPI_MODULE_ID index)
{
     return SPI_ExistsFIFOInterruptMode_PIC32(index);
}

PLIB_INLINE_API void PLIB_SPI_FramedCommunicationEnable(SPI_MODULE_ID index)
{
     SPI_FramedCommunicationEnable_Default(index);
}

PLIB_INLINE_API void PLIB_SPI_FramedCommunicationDisable(SPI_MODULE_ID index)
{
     SPI_FramedCommunicationDisable_Default(index);
}

PLIB_INLINE_API bool PLIB_SPI_ExistsFramedCommunication(SPI_MODULE_ID index)
{
     return SPI_ExistsFramedCommunication_Default(index);
}

PLIB_INLINE_API void PLIB_SPI_FrameSyncPulseDirectionSelect(SPI_MODULE_ID index, SPI_FRAME_PULSE_DIRECTION direction)
{
     SPI_FrameSyncPulseDirectionSelect_Default(index, direction);
}

PLIB_INLINE_API bool PLIB_SPI_ExistsFrameSyncPulseDirection(SPI_MODULE_ID index)
{
     return SPI_ExistsFrameSyncPulseDirection_Default(index);
}

PLIB_INLINE_API void PLIB_SPI_FrameSyncPulsePolaritySelect(SPI_MODULE_ID index, SPI_FRAME_PULSE_POLARITY polarity)
{
     SPI_FrameSyncPulsePolaritySelect_Default(index, polarity);
}

PLIB_INLINE_API bool PLIB_SPI_ExistsFrameSyncPulsePolarity(SPI_MODULE_ID index)
{
     return SPI_ExistsFrameSyncPulsePolarity_Default(index);
}

PLIB_INLINE_API void PLIB_SPI_FrameSyncPulseEdgeSelect(SPI_MODULE_ID index, SPI_FRAME_PULSE_EDGE edge)
{
     SPI_FrameSyncPulseEdgeSelect_Default(index, edge);
}

PLIB_INLINE_API bool PLIB_SPI_ExistsFrameSyncPulseEdge(SPI_MODULE_ID index)
{
     return SPI_ExistsFrameSyncPulseEdge_Default(index);
}

PLIB_INLINE_API void PLIB_SPI_FrameSyncPulseWidthSelect(SPI_MODULE_ID index, SPI_FRAME_PULSE_WIDTH width)
{
     SPI_FrameSyncPulseWidthSelect_Default(index, width);
}

PLIB_INLINE_API bool PLIB_SPI_ExistsFrameSyncPulseWidth(SPI_MODULE_ID index)
{
     return SPI_ExistsFrameSyncPulseWidth_Default(index);
}

PLIB_INLINE_API void PLIB_SPI_FrameSyncPulseCounterSelect(SPI_MODULE_ID index, SPI_FRAME_SYNC_PULSE pulse)
{
     SPI_FrameSyncPulseCounterSelect_PIC32(index, pulse);
}

PLIB_INLINE_API bool PLIB_SPI_ExistsFrameSyncPulseCounter(SPI_MODULE_ID index)
{
     return SPI_ExistsFrameSyncPulseCounter_PIC32(index);
}

PLIB_INLINE_API bool PLIB_SPI_FrameErrorStatusGet(SPI_MODULE_ID index)
{
     return SPI_FrameErrorStatusGet_Default(index);
}

PLIB_INLINE_API void PLIB_SPI_FrameErrorStatusClear(SPI_MODULE_ID index)
{
     SPI_FrameErrorStatusClear_Default(index);
}

PLIB_INLINE_API bool PLIB_SPI_ExistsFrameErrorStatus(SPI_MODULE_ID index)
{
     return SPI_ExistsFrameErrorStatus_Default(index);
}

PLIB_INLINE_API void PLIB_SPI_BufferClear(SPI_MODULE_ID index)
{
     SPI_BufferClear_Default(index);
}

PLIB_INLINE_API uint8_t PLIB_SPI_BufferRead(SPI_MODULE_ID index)
{
     return SPI_BufferRead_Default(index);
}

PLIB_INLINE_API void PLIB_SPI_BufferWrite(SPI_MODULE_ID index, uint8_t data)
{
     SPI_BufferWrite_Default(index, data);
}

PLIB_INLINE_API bool PLIB_SPI_ExistsBuffer(SPI_MODULE_ID index)
{
     return SPI_ExistsBuffer_Default(index);
}

PLIB_INLINE_API void* PLIB_SPI_BufferAddressGet(SPI_MODULE_ID index)
{
     return SPI_BufferAddressGet_Default(index);
}

PLIB_INLINE_API void PLIB_SPI_BaudRateClockSelect(SPI_MODULE_ID index, SPI_BAUD_RATE_CLOCK type)
{
     SPI_BaudRateClockSelect_Default(index, type);
}

PLIB_INLINE_API bool PLIB_SPI_ExistsBaudRateClock(SPI_MODULE_ID index)
{
     return SPI_ExistsBaudRateClock_Default(index);
}

PLIB_INLINE_API void PLIB_SPI_ErrorInterruptEnable(SPI_MODULE_ID index, SPI_ERROR_INTERRUPT error)
{
     SPI_ErrorInterruptEnable_Default(index, error);
}

PLIB_INLINE_API void PLIB_SPI_ErrorInterruptDisable(SPI_MODULE_ID index, SPI_ERROR_INTERRUPT error)
{
     SPI_ErrorInterruptDisable_Default(index, error);
}

PLIB_INLINE_API bool PLIB_SPI_ExistsErrorInterruptControl(SPI_MODULE_ID index)
{
     return SPI_ExistsErrorInterruptControl_Default(index);
}

PLIB_INLINE_API void PLIB_SPI_AudioErrorEnable(SPI_MODULE_ID index, SPI_AUDIO_ERROR error)
{
     SPI_AudioErrorEnable_Default(index, error);
}

PLIB_INLINE_API void PLIB_SPI_AudioErrorDisable(SPI_MODULE_ID index, SPI_AUDIO_ERROR error)
{
     SPI_AudioErrorDisable_Default(index, error);
}

PLIB_INLINE_API bool PLIB_SPI_ExistsAudioErrorControl(SPI_MODULE_ID index)
{
     return SPI_ExistsAudioErrorControl_Default(index);
}

PLIB_INLINE_API void PLIB_SPI_AudioProtocolEnable(SPI_MODULE_ID index)
{
     SPI_AudioProtocolEnable_Default(index);
}

PLIB_INLINE_API void PLIB_SPI_AudioProtocolDisable(SPI_MODULE_ID index)
{
     SPI_AudioProtocolDisable_Default(index);
}

PLIB_INLINE_API bool PLIB_SPI_ExistsAudioProtocolControl(SPI_MODULE_ID index)
{
     return SPI_ExistsAudioProtocolControl_Default(index);
}

PLIB_INLINE_API void PLIB_SPI_AudioTransmitModeSelect(SPI_MODULE_ID index, SPI_AUDIO_TRANSMIT_MODE mode)
{
     SPI_AudioTransmitModeSelect_Default(index, mode);
}

PLIB_INLINE_API bool PLIB_SPI_ExistsAudioTransmitMode(SPI_MODULE_ID index)
{
     return SPI_ExistsAudioTransmitMode_Default(index);
}

PLIB_INLINE_API void PLIB_SPI_AudioProtocolModeSelect(SPI_MODULE_ID index, SPI_AUDIO_PROTOCOL mode)
{
     SPI_AudioProtocolModeSelect_Default(index, mode);
}

PLIB_INLINE_API bool PLIB_SPI_ExistsAudioProtocolMode(SPI_MODULE_ID index)
{
     return SPI_ExistsAudioProtocolMode_Default(index);
}

PLIB_INLINE_API uint32_t PLIB_SPI_BufferRead32bit(SPI_MODULE_ID index)
{
     return SPI_BufferRead32bit_Default(index);
}

PLIB_INLINE_API void PLIB_SPI_BufferWrite32bit(SPI_MODULE_ID index, uint32_t data)
{
     SPI_BufferWrite32bit_Default(index, data);
}

PLIB_INLINE_API bool PLIB_SPI_Exists32bitBuffer(SPI_MODULE_ID index)
{
     return SPI_Exists32bitBuffer_Default(index);
}

PLIB_INLINE_API uint16_t PLIB_SPI_BufferRead16bit(SPI_MODULE_ID index)
{
     return SPI_BufferRead16bit_Default(index);
}

PLIB_INLINE_API void PLIB_SPI_BufferWrite16bit(SPI_MODULE_ID index, uint16_t data)
{
     SPI_BufferWrite16bit_Default(index, data);
}

PLIB_INLINE_API bool PLIB_SPI_Exists16bitBuffer(SPI_MODULE_ID index)
{
     return SPI_Exists16bitBuffer_Default(index);
}

#endif
