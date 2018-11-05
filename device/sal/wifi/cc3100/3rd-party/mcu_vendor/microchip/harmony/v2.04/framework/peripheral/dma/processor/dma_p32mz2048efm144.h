/* Created by plibgen $Revision: 1.31 $ */

#ifndef _DMA_P32MZ2048EFM144_H
#define _DMA_P32MZ2048EFM144_H

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

    DMA_ID_0 = _DMAC_BASE_ADDRESS,
    DMA_NUMBER_OF_MODULES = 1

} DMA_MODULE_ID;

typedef enum {

    DMA_CHANNEL_0 = 0x0,
    DMA_CHANNEL_1 = 0x1,
    DMA_CHANNEL_2 = 0x2,
    DMA_CHANNEL_3 = 0x3,
    DMA_CHANNEL_4 = 0x4,
    DMA_CHANNEL_5 = 0x5,
    DMA_CHANNEL_6 = 0x6,
    DMA_CHANNEL_7 = 0x7,
    DMA_NUMBER_OF_CHANNELS = 0x8

} DMA_CHANNEL;

typedef enum {

    DMA_CHANNEL_0_INT_SOURCE = 134,
    DMA_CHANNEL_1_INT_SOURCE = 135,
    DMA_CHANNEL_2_INT_SOURCE = 136,
    DMA_CHANNEL_3_INT_SOURCE = 137,
    DMA_CHANNEL_4_INT_SOURCE = 138,
    DMA_CHANNEL_5_INT_SOURCE = 139,
    DMA_CHANNEL_6_INT_SOURCE = 140,
    DMA_CHANNEL_7_INT_SOURCE = 141

} DMA_CHANNEL_INT_SOURCE;

typedef enum {

    DMA_CHANNEL_PRIORITY_0 = 0x0,
    DMA_CHANNEL_PRIORITY_1 = 0x1,
    DMA_CHANNEL_PRIORITY_2 = 0x2,
    DMA_CHANNEL_PRIORITY_3 = 0x3

} DMA_CHANNEL_PRIORITY;

typedef enum {

    DMA_CHANNEL_TRIGGER_TRANSFER_START = 0x0,
    DMA_CHANNEL_TRIGGER_TRANSFER_ABORT = 0x1,
    DMA_CHANNEL_TRIGGER_PATTERN_MATCH_ABORT = 0x2

} DMA_CHANNEL_TRIGGER_TYPE;

typedef enum {

    DMA_TRIGGER_SOURCE_NONE = -1,
    DMA_TRIGGER_TIMER_CORE = 0,
    DMA_TRIGGER_SOFTWARE_0 = 1,
    DMA_TRIGGER_SOFTWARE_1 = 2,
    DMA_TRIGGER_EXTERNAL_0 = 3,
    DMA_TRIGGER_EXTERNAL_1 = 8,
    DMA_TRIGGER_EXTERNAL_2 = 13,
    DMA_TRIGGER_EXTERNAL_3 = 18,
    DMA_TRIGGER_EXTERNAL_4 = 23,
    DMA_TRIGGER_TIMER_1 = 4,
    DMA_TRIGGER_TIMER_2 = 9,
    DMA_TRIGGER_TIMER_3 = 14,
    DMA_TRIGGER_TIMER_4 = 19,
    DMA_TRIGGER_TIMER_5 = 24,
    DMA_TRIGGER_TIMER_6 = 28,
    DMA_TRIGGER_TIMER_7 = 32,
    DMA_TRIGGER_TIMER_8 = 36,
    DMA_TRIGGER_TIMER_9 = 40,
    DMA_TRIGGER_INPUT_CAPTURE_1 = 6,
    DMA_TRIGGER_INPUT_CAPTURE_2 = 11,
    DMA_TRIGGER_INPUT_CAPTURE_3 = 16,
    DMA_TRIGGER_INPUT_CAPTURE_4 = 21,
    DMA_TRIGGER_INPUT_CAPTURE_5 = 26,
    DMA_TRIGGER_INPUT_CAPTURE_6 = 30,
    DMA_TRIGGER_INPUT_CAPTURE_7 = 34,
    DMA_TRIGGER_INPUT_CAPTURE_8 = 38,
    DMA_TRIGGER_INPUT_CAPTURE_9 = 42,
    DMA_TRIGGER_INPUT_CAPTURE_1_ERROR = 5,
    DMA_TRIGGER_INPUT_CAPTURE_2_ERROR = 10,
    DMA_TRIGGER_INPUT_CAPTURE_3_ERROR = 15,
    DMA_TRIGGER_INPUT_CAPTURE_4_ERROR = 20,
    DMA_TRIGGER_INPUT_CAPTURE_5_ERROR = 25,
    DMA_TRIGGER_INPUT_CAPTURE_6_ERROR = 29,
    DMA_TRIGGER_INPUT_CAPTURE_7_ERROR = 33,
    DMA_TRIGGER_INPUT_CAPTURE_8_ERROR = 37,
    DMA_TRIGGER_INPUT_CAPTURE_9_ERROR = 41,
    DMA_TRIGGER_OUTPUT_COMPARE_1 = 7,
    DMA_TRIGGER_OUTPUT_COMPARE_2 = 12,
    DMA_TRIGGER_OUTPUT_COMPARE_3 = 17,
    DMA_TRIGGER_OUTPUT_COMPARE_4 = 22,
    DMA_TRIGGER_OUTPUT_COMPARE_5 = 27,
    DMA_TRIGGER_OUTPUT_COMPARE_6 = 31,
    DMA_TRIGGER_OUTPUT_COMPARE_7 = 35,
    DMA_TRIGGER_OUTPUT_COMPARE_8 = 39,
    DMA_TRIGGER_OUTPUT_COMPARE_9 = 43,
    DMA_TRIGGER_SPI_1_ERROR = 109,
    DMA_TRIGGER_SPI_1_RECEIVE = 110,
    DMA_TRIGGER_SPI_1_TRANSMIT = 111,
    DMA_TRIGGER_SPI_2_ERROR = 142,
    DMA_TRIGGER_SPI_2_RECEIVE = 143,
    DMA_TRIGGER_SPI_2_TRANSMIT = 144,
    DMA_TRIGGER_SPI_3_ERROR = 154,
    DMA_TRIGGER_SPI_3_RECEIVE = 155,
    DMA_TRIGGER_SPI_3_TRANSMIT = 156,
    DMA_TRIGGER_SPI_4_ERROR = 163,
    DMA_TRIGGER_SPI_4_RECEIVE = 164,
    DMA_TRIGGER_SPI_4_TRANSMIT = 165,
    DMA_TRIGGER_SPI_5_ERROR = 176,
    DMA_TRIGGER_SPI_5_RECEIVE = 177,
    DMA_TRIGGER_SPI_5_TRANSMIT = 178,
    DMA_TRIGGER_SPI_6_ERROR = 185,
    DMA_TRIGGER_SPI_6_RECEIVE = 186,
    DMA_TRIGGER_SPI_6_TRANSMIT = 187,
    DMA_TRIGGER_I2C_1_ERROR = 115,
    DMA_TRIGGER_I2C_1_SLAVE = 116,
    DMA_TRIGGER_I2C_1_MASTER = 117,
    DMA_TRIGGER_I2C_2_ERROR = 148,
    DMA_TRIGGER_I2C_2_SLAVE = 149,
    DMA_TRIGGER_I2C_2_MASTER = 150,
    DMA_TRIGGER_I2C_3_ERROR = 160,
    DMA_TRIGGER_I2C_3_SLAVE = 161,
    DMA_TRIGGER_I2C_3_MASTER = 162,
    DMA_TRIGGER_I2C_4_ERROR = 173,
    DMA_TRIGGER_I2C_4_SLAVE = 174,
    DMA_TRIGGER_I2C_4_MASTER = 175,
    DMA_TRIGGER_I2C_5_ERROR = 182,
    DMA_TRIGGER_I2C_5_SLAVE = 183,
    DMA_TRIGGER_I2C_5_MASTER = 184,
    DMA_TRIGGER_USART_1_ERROR = 112,
    DMA_TRIGGER_USART_1_RECEIVE = 113,
    DMA_TRIGGER_USART_1_TRANSMIT = 114,
    DMA_TRIGGER_USART_2_ERROR = 145,
    DMA_TRIGGER_USART_2_RECEIVE = 146,
    DMA_TRIGGER_USART_2_TRANSMIT = 147,
    DMA_TRIGGER_USART_3_ERROR = 157,
    DMA_TRIGGER_USART_3_RECEIVE = 158,
    DMA_TRIGGER_USART_3_TRANSMIT = 159,
    DMA_TRIGGER_USART_4_ERROR = 170,
    DMA_TRIGGER_USART_4_RECEIVE = 171,
    DMA_TRIGGER_USART_4_TRANSMIT = 172,
    DMA_TRIGGER_USART_5_ERROR = 179,
    DMA_TRIGGER_USART_5_RECEIVE = 180,
    DMA_TRIGGER_USART_5_TRANSMIT = 181,
    DMA_TRIGGER_USART_6_ERROR = 188,
    DMA_TRIGGER_USART_6_RECEIVE = 189,
    DMA_TRIGGER_USART_6_TRANSMIT = 190,
    DMA_TRIGGER_CHANGE_NOTICE_A = 118,
    DMA_TRIGGER_CHANGE_NOTICE_B = 119,
    DMA_TRIGGER_CHANGE_NOTICE_C = 120,
    DMA_TRIGGER_CHANGE_NOTICE_D = 121,
    DMA_TRIGGER_CHANGE_NOTICE_E = 122,
    DMA_TRIGGER_CHANGE_NOTICE_F = 123,
    DMA_TRIGGER_CHANGE_NOTICE_G = 124,
    DMA_TRIGGER_CHANGE_NOTICE_H = 125,
    DMA_TRIGGER_CHANGE_NOTICE_J = 126,
    DMA_TRIGGER_CHANGE_NOTICE_K = 127,
    DMA_TRIGGER_DMA_0 = 134,
    DMA_TRIGGER_DMA_1 = 135,
    DMA_TRIGGER_DMA_2 = 136,
    DMA_TRIGGER_DMA_3 = 137,
    DMA_TRIGGER_DMA_4 = 138,
    DMA_TRIGGER_DMA_5 = 139,
    DMA_TRIGGER_DMA_6 = 140,
    DMA_TRIGGER_DMA_7 = 141,
    DMA_TRIGGER_COMPARATOR_1 = 130,
    DMA_TRIGGER_COMPARATOR_2 = 131,
    DMA_TRIGGER_ADC_1 = 44,
    DMA_TRIGGER_ADC1_DC1 = 46,
    DMA_TRIGGER_ADC1_DC2 = 47,
    DMA_TRIGGER_ADC1_DC3 = 48,
    DMA_TRIGGER_ADC1_DC4 = 49,
    DMA_TRIGGER_ADC1_DC5 = 50,
    DMA_TRIGGER_ADC1_DC6 = 51,
    DMA_TRIGGER_ADC1_DF1 = 52,
    DMA_TRIGGER_ADC1_DF2 = 53,
    DMA_TRIGGER_ADC1_DF3 = 54,
    DMA_TRIGGER_ADC1_DF4 = 55,
    DMA_TRIGGER_ADC1_DF5 = 56,
    DMA_TRIGGER_ADC1_DF6 = 57,
    DMA_TRIGGER_ADC1_DATA0 = 59,
    DMA_TRIGGER_ADC1_DATA1 = 60,
    DMA_TRIGGER_ADC1_DATA2 = 61,
    DMA_TRIGGER_ADC1_DATA3 = 62,
    DMA_TRIGGER_ADC1_DATA4 = 63,
    DMA_TRIGGER_ADC1_DATA5 = 64,
    DMA_TRIGGER_ADC1_DATA6 = 65,
    DMA_TRIGGER_ADC1_DATA7 = 66,
    DMA_TRIGGER_ADC1_DATA8 = 67,
    DMA_TRIGGER_ADC1_DATA9 = 68,
    DMA_TRIGGER_ADC1_DATA10 = 69,
    DMA_TRIGGER_ADC1_DATA11 = 70,
    DMA_TRIGGER_ADC1_DATA12 = 71,
    DMA_TRIGGER_ADC1_DATA13 = 72,
    DMA_TRIGGER_ADC1_DATA14 = 73,
    DMA_TRIGGER_ADC1_DATA15 = 74,
    DMA_TRIGGER_ADC1_DATA16 = 75,
    DMA_TRIGGER_ADC1_DATA17 = 76,
    DMA_TRIGGER_ADC1_DATA18 = 77,
    DMA_TRIGGER_ADC1_DATA19 = 78,
    DMA_TRIGGER_ADC1_DATA20 = 79,
    DMA_TRIGGER_ADC1_DATA21 = 80,
    DMA_TRIGGER_ADC1_DATA22 = 81,
    DMA_TRIGGER_ADC1_DATA23 = 82,
    DMA_TRIGGER_ADC1_DATA24 = 83,
    DMA_TRIGGER_ADC1_DATA25 = 84,
    DMA_TRIGGER_ADC1_DATA26 = 85,
    DMA_TRIGGER_ADC1_DATA27 = 86,
    DMA_TRIGGER_ADC1_DATA28 = 87,
    DMA_TRIGGER_ADC1_DATA29 = 88,
    DMA_TRIGGER_ADC1_DATA30 = 89,
    DMA_TRIGGER_ADC1_DATA31 = 90,
    DMA_TRIGGER_ADC1_DATA32 = 91,
    DMA_TRIGGER_ADC1_DATA33 = 92,
    DMA_TRIGGER_ADC1_DATA34 = 93,
    DMA_TRIGGER_ADC1_DATA35 = 94,
    DMA_TRIGGER_ADC1_DATA36 = 95,
    DMA_TRIGGER_ADC1_DATA37 = 96,
    DMA_TRIGGER_ADC1_DATA38 = 97,
    DMA_TRIGGER_ADC1_DATA39 = 98,
    DMA_TRIGGER_ADC1_DATA40 = 99,
    DMA_TRIGGER_ADC1_DATA41 = 100,
    DMA_TRIGGER_ADC1_DATA42 = 101,
    DMA_TRIGGER_ADC1_DATA43 = 102,
    DMA_TRIGGER_ADC1_DATA44 = 103,
    DMA_TRIGGER_CORE_PERF_COUNT = 104,
    DMA_TRIGGER_CORE_FAST_DEBUG_CHAN = 105,
    DMA_TRIGGER_SYSTEM_BUS_PROTECTION = 106,
    DMA_TRIGGER_CRYPTO = 107,
    DMA_TRIGGER_PARALLEL_PORT = 128,
    DMA_TRIGGER_CAN_1 = 151,
    DMA_TRIGGER_CAN_2 = 152,
    DMA_TRIGGER_RTCC = 166,
    DMA_TRIGGER_FLASH_CONTROL = 167,
    DMA_TRIGGER_USB_1 = 132,
    DMA_TRIGGER_USB_DMA = 133,
    DMA_TRIGGER_ETH_1 = 153,
    DMA_TRIGGER_PARALLEL_PORT_ERROR = 129,
    DMA_TRIGGER_PREFETCH = 168,
    DMA_TRIGGER_SQI1 = 169

} DMA_TRIGGER_SOURCE;

typedef enum {

    DMA_CRC_IP_HEADER = 0x1,
    DMA_CRC_LFSR = 0x0

} DMA_CRC_TYPE;

typedef enum {

    DMA_CRC_BYTEORDER_NO_SWAPPING = 0x0,
    DMA_CRC_SWAP_BYTE_ON_WORD_BOUNDARY = 0x1,
    DMA_CRC_SWAP_HALF_WORD_ON_WORD_BOUNDARY = 0x2,
    DMA_CRC_SWAP_BYTE_ON_HALF_WORD_BOUNDARY = 0x3

} DMA_CRC_BYTE_ORDER;

typedef enum {

    DMA_INT_ADDRESS_ERROR = 0x1,
    DMA_INT_TRANSFER_ABORT = 0x2,
    DMA_INT_CELL_TRANSFER_COMPLETE = 0x4,
    DMA_INT_BLOCK_TRANSFER_COMPLETE = 0x8,
    DMA_INT_DESTINATION_HALF_FULL = 0x10,
    DMA_INT_DESTINATION_DONE = 0x20,
    DMA_INT_SOURCE_HALF_EMPTY = 0x40,
    DMA_INT_SOURCE_DONE = 0x80

} DMA_INT_TYPE;

typedef enum {

    DMA_CRC_BIT_ORDER_LSB = 0x1,
    DMA_CRC_BIT_ORDER_MSB = 0x0

} DMA_CRC_BIT_ORDER;

typedef enum {

    DMA_PATTERN_MATCH_LENGTH_1BYTE = 0x0,
    DMA_PATTERN_MATCH_LENGTH_2BYTES = 0x1

} DMA_PATTERN_LENGTH;

typedef enum {

    DMA_CHANNEL_COLLISION_NOT_SUPPORTED = 0x0

} DMA_CHANNEL_COLLISION;

typedef enum {

    DMA_PING_PONG_MODE_NOT_SUPPORTED = 0x0

} DMA_PING_PONG_MODE;

typedef enum {

    DMA_CHANNEL_TRANSFER_DIRECTION_NOT_SUPPORTED = 0

} DMA_CHANNEL_TRANSFER_DIRECTION;

typedef enum {

    DMA_ADDRESS_OFFSET_TYPE_NOT_SUPPORTED = 0

} DMA_ADDRESS_OFFSET_TYPE;

typedef enum {

    DMA_SOURCE_ADDRESSING_MODE_NOT_SUPPORTED = 0

} DMA_SOURCE_ADDRESSING_MODE;

typedef enum {

    DMA_DESTINATION_ADDRESSING_MODE_NOT_SUPPORTED = 0

} DMA_DESTINATION_ADDRESSING_MODE;

typedef enum {

    DMA_CHANNEL_ADDRESSING_MODE_NOT_SUPPORTED = 0

} DMA_CHANNEL_ADDRESSING_MODE;

typedef enum {

    DMA_CHANNEL_DATA_SIZE_NOT_SUPPORTED = 0

} DMA_CHANNEL_DATA_SIZE;

typedef enum {

    DMA_TRANSFER_MODE_NOT_SUPPORTED = 0

} DMA_TRANSFER_MODE;

/* Section 2 - Feature variant inclusion */

#define PLIB_TEMPLATE PLIB_INLINE
#include "../templates/dma_Busy_Default.h"
#include "../templates/dma_Suspend_Default.h"
#include "../templates/dma_StopInIdle_Unsupported.h"
#include "../templates/dma_EnableControl_Default.h"
#include "../templates/dma_ChannelBits_Default.h"
#include "../templates/dma_LastBusAccess_Default.h"
#include "../templates/dma_RecentAddress_Default.h"
#include "../templates/dma_CRCChannel_Default.h"
#include "../templates/dma_CRCType_Default.h"
#include "../templates/dma_CRCAppendMode_Default.h"
#include "../templates/dma_Crc_Default.h"
#include "../templates/dma_CRCPolynomialLength_Default.h"
#include "../templates/dma_CRCBitOrder_Default.h"
#include "../templates/dma_CRCWriteByteOrder_Default.h"
#include "../templates/dma_CRCByteOrder_Default.h"
#include "../templates/dma_CRCData_Default.h"
#include "../templates/dma_CRCXOREnable_Default.h"
#include "../templates/dma_ChannelXPriority_Default.h"
#include "../templates/dma_ChannelXEvent_Default.h"
#include "../templates/dma_ChannelXAuto_Default.h"
#include "../templates/dma_ChannelXChainEnbl_Default.h"
#include "../templates/dma_ChannelXDisabled_Default.h"
#include "../templates/dma_ChannelX_Default.h"
#include "../templates/dma_ChannelXChain_Default.h"
#include "../templates/dma_ChannelXPatternLength_Default.h"
#include "../templates/dma_ChannelXPatternIgnoreByte_Default.h"
#include "../templates/dma_ChannelXBusy_Default.h"
#include "../templates/dma_ChannelXPatternIgnore_Default.h"
#include "../templates/dma_ChannelXTrigger_Default.h"
#include "../templates/dma_AbortTransfer_Default.h"
#include "../templates/dma_StartTransfer_Default.h"
#include "../templates/dma_ChannelXStartIRQ_Default.h"
#include "../templates/dma_ChannelXAbortIRQ_Default.h"
#include "../templates/dma_ChannelXINTSourceFlag_Default.h"
#include "../templates/dma_ChannelXINTSource_Default.h"
#include "../templates/dma_ChannelXSourceStartAddress_Default.h"
#include "../templates/dma_ChannelXDestinationStartAddress_Default.h"
#include "../templates/dma_ChannelXSourceSize_Default.h"
#include "../templates/dma_ChannelXDestinationSize_Default.h"
#include "../templates/dma_ChannelXSourcePointer_Default.h"
#include "../templates/dma_ChannelXDestinationPointer_Default.h"
#include "../templates/dma_ChannelXCellSize_Default.h"
#include "../templates/dma_ChannelXCellProgressPointer_Default.h"
#include "../templates/dma_ChannelXPatternData_Default.h"

/* Section 3 - PLIB dispatch function definitions */

PLIB_INLINE_API bool PLIB_DMA_ExistsBusy(DMA_MODULE_ID index)
{
     return DMA_ExistsBusy_Default(index);
}

PLIB_INLINE_API void PLIB_DMA_BusyActiveSet(DMA_MODULE_ID index)
{
     DMA_BusyActiveSet_Default(index);
}

PLIB_INLINE_API void PLIB_DMA_BusyActiveReset(DMA_MODULE_ID index)
{
     DMA_BusyActiveReset_Default(index);
}

PLIB_INLINE_API bool PLIB_DMA_IsBusy(DMA_MODULE_ID index)
{
     return DMA_IsBusy_Default(index);
}

PLIB_INLINE_API bool PLIB_DMA_ExistsSuspend(DMA_MODULE_ID index)
{
     return DMA_ExistsSuspend_Default(index);
}

PLIB_INLINE_API void PLIB_DMA_SuspendEnable(DMA_MODULE_ID index)
{
     DMA_SuspendEnable_Default(index);
}

PLIB_INLINE_API void PLIB_DMA_SuspendDisable(DMA_MODULE_ID index)
{
     DMA_SuspendDisable_Default(index);
}

PLIB_INLINE_API bool PLIB_DMA_SuspendIsEnabled(DMA_MODULE_ID index)
{
     return DMA_SuspendIsEnabled_Default(index);
}

PLIB_INLINE_API bool PLIB_DMA_ExistsStopInIdle(DMA_MODULE_ID index)
{
     return DMA_ExistsStopInIdle_Unsupported(index);
}

PLIB_INLINE_API void _PLIB_UNSUPPORTED PLIB_DMA_StopInIdleEnable(DMA_MODULE_ID index)
{
     DMA_StopInIdleEnable_Unsupported(index);
}

PLIB_INLINE_API void _PLIB_UNSUPPORTED PLIB_DMA_StopInIdleDisable(DMA_MODULE_ID index)
{
     DMA_StopInIdleDisable_Unsupported(index);
}

PLIB_INLINE_API bool PLIB_DMA_ExistsEnableControl(DMA_MODULE_ID index)
{
     return DMA_ExistsEnableControl_Default(index);
}

PLIB_INLINE_API void PLIB_DMA_Enable(DMA_MODULE_ID index)
{
     DMA_Enable_Default(index);
}

PLIB_INLINE_API void PLIB_DMA_Disable(DMA_MODULE_ID index)
{
     DMA_Disable_Default(index);
}

PLIB_INLINE_API bool PLIB_DMA_IsEnabled(DMA_MODULE_ID index)
{
     return DMA_IsEnabled_Default(index);
}

PLIB_INLINE_API bool PLIB_DMA_ExistsChannelBits(DMA_MODULE_ID index)
{
     return DMA_ExistsChannelBits_Default(index);
}

PLIB_INLINE_API uint8_t PLIB_DMA_ChannelBitsGet(DMA_MODULE_ID index)
{
     return DMA_ChannelBitsGet_Default(index);
}

PLIB_INLINE_API bool PLIB_DMA_ExistsLastBusAccess(DMA_MODULE_ID index)
{
     return DMA_ExistsLastBusAccess_Default(index);
}

PLIB_INLINE_API bool PLIB_DMA_LastBusAccessIsRead(DMA_MODULE_ID index)
{
     return DMA_LastBusAccessIsRead_Default(index);
}

PLIB_INLINE_API bool PLIB_DMA_LastBusAccessIsWrite(DMA_MODULE_ID index)
{
     return DMA_LastBusAccessIsWrite_Default(index);
}

PLIB_INLINE_API bool PLIB_DMA_ExistsRecentAddress(DMA_MODULE_ID index)
{
     return DMA_ExistsRecentAddress_Default(index);
}

PLIB_INLINE_API uint32_t PLIB_DMA_RecentAddressAccessed(DMA_MODULE_ID index)
{
     return DMA_RecentAddressAccessed_Default(index);
}

PLIB_INLINE_API bool PLIB_DMA_ExistsCRCChannel(DMA_MODULE_ID index)
{
     return DMA_ExistsCRCChannel_Default(index);
}

PLIB_INLINE_API void PLIB_DMA_CRCChannelSelect(DMA_MODULE_ID index, DMA_CHANNEL channel)
{
     DMA_CRCChannelSelect_Default(index, channel);
}

PLIB_INLINE_API DMA_CHANNEL PLIB_DMA_CRCChannelGet(DMA_MODULE_ID index)
{
     return DMA_CRCChannelGet_Default(index);
}

PLIB_INLINE_API bool PLIB_DMA_ExistsCRCType(DMA_MODULE_ID index)
{
     return DMA_ExistsCRCType_Default(index);
}

PLIB_INLINE_API DMA_CRC_TYPE PLIB_DMA_CRCTypeGet(DMA_MODULE_ID index)
{
     return DMA_CRCTypeGet_Default(index);
}

PLIB_INLINE_API void PLIB_DMA_CRCTypeSet(DMA_MODULE_ID index, DMA_CRC_TYPE CRCType)
{
     DMA_CRCTypeSet_Default(index, CRCType);
}

PLIB_INLINE_API bool PLIB_DMA_ExistsCRCAppendMode(DMA_MODULE_ID index)
{
     return DMA_ExistsCRCAppendMode_Default(index);
}

PLIB_INLINE_API void PLIB_DMA_CRCAppendModeEnable(DMA_MODULE_ID index)
{
     DMA_CRCAppendModeEnable_Default(index);
}

PLIB_INLINE_API void PLIB_DMA_CRCAppendModeDisable(DMA_MODULE_ID index)
{
     DMA_CRCAppendModeDisable_Default(index);
}

PLIB_INLINE_API bool PLIB_DMA_CRCAppendModeIsEnabled(DMA_MODULE_ID index)
{
     return DMA_CRCAppendModeIsEnabled_Default(index);
}

PLIB_INLINE_API bool PLIB_DMA_ExistsCRC(DMA_MODULE_ID index)
{
     return DMA_ExistsCRC_Default(index);
}

PLIB_INLINE_API void PLIB_DMA_CRCEnable(DMA_MODULE_ID index)
{
     DMA_CRCEnable_Default(index);
}

PLIB_INLINE_API void PLIB_DMA_CRCDisable(DMA_MODULE_ID index)
{
     DMA_CRCDisable_Default(index);
}

PLIB_INLINE_API bool PLIB_DMA_CRCIsEnabled(DMA_MODULE_ID index)
{
     return DMA_CRCIsEnabled_Default(index);
}

PLIB_INLINE_API bool PLIB_DMA_ExistsCRCPolynomialLength(DMA_MODULE_ID index)
{
     return DMA_ExistsCRCPolynomialLength_Default(index);
}

PLIB_INLINE_API void PLIB_DMA_CRCPolynomialLengthSet(DMA_MODULE_ID index, uint8_t polyLength)
{
     DMA_CRCPolynomialLengthSet_Default(index, polyLength);
}

PLIB_INLINE_API uint8_t PLIB_DMA_CRCPolynomialLengthGet(DMA_MODULE_ID index)
{
     return DMA_CRCPolynomialLengthGet_Default(index);
}

PLIB_INLINE_API bool PLIB_DMA_ExistsCRCBitOrder(DMA_MODULE_ID index)
{
     return DMA_ExistsCRCBitOrder_Default(index);
}

PLIB_INLINE_API void PLIB_DMA_CRCBitOrderSelect(DMA_MODULE_ID index, DMA_CRC_BIT_ORDER bitOrder)
{
     DMA_CRCBitOrderSelect_Default(index, bitOrder);
}

PLIB_INLINE_API bool PLIB_DMA_ExistsCRCWriteByteOrder(DMA_MODULE_ID index)
{
     return DMA_ExistsCRCWriteByteOrder_Default(index);
}

PLIB_INLINE_API void PLIB_DMA_CRCWriteByteOrderAlter(DMA_MODULE_ID index)
{
     DMA_CRCWriteByteOrderAlter_Default(index);
}

PLIB_INLINE_API void PLIB_DMA_CRCWriteByteOrderMaintain(DMA_MODULE_ID index)
{
     DMA_CRCWriteByteOrderMaintain_Default(index);
}

PLIB_INLINE_API bool PLIB_DMA_ExistsCRCByteOrder(DMA_MODULE_ID index)
{
     return DMA_ExistsCRCByteOrder_Default(index);
}

PLIB_INLINE_API void PLIB_DMA_CRCByteOrderSelect(DMA_MODULE_ID index, DMA_CRC_BYTE_ORDER byteOrder)
{
     DMA_CRCByteOrderSelect_Default(index, byteOrder);
}

PLIB_INLINE_API DMA_CRC_BYTE_ORDER PLIB_DMA_CRCByteOrderGet(DMA_MODULE_ID index)
{
     return DMA_CRCByteOrderGet_Default(index);
}

PLIB_INLINE_API bool PLIB_DMA_ExistsCRCData(DMA_MODULE_ID index)
{
     return DMA_ExistsCRCData_Default(index);
}

PLIB_INLINE_API uint32_t PLIB_DMA_CRCDataRead(DMA_MODULE_ID index)
{
     return DMA_CRCDataRead_Default(index);
}

PLIB_INLINE_API void PLIB_DMA_CRCDataWrite(DMA_MODULE_ID index, uint32_t DMACRCdata)
{
     DMA_CRCDataWrite_Default(index, DMACRCdata);
}

PLIB_INLINE_API bool PLIB_DMA_ExistsCRCXOREnable(DMA_MODULE_ID index)
{
     return DMA_ExistsCRCXOREnable_Default(index);
}

PLIB_INLINE_API void PLIB_DMA_CRCXOREnableSet(DMA_MODULE_ID index, uint32_t DMACRCXOREnableMask)
{
     DMA_CRCXOREnableSet_Default(index, DMACRCXOREnableMask);
}

PLIB_INLINE_API uint32_t PLIB_DMA_CRCXOREnableGet(DMA_MODULE_ID index)
{
     return DMA_CRCXOREnableGet_Default(index);
}

PLIB_INLINE_API bool PLIB_DMA_ExistsChannelXPriority(DMA_MODULE_ID index)
{
     return DMA_ExistsChannelXPriority_Default(index);
}

PLIB_INLINE_API void PLIB_DMA_ChannelXPrioritySelect(DMA_MODULE_ID index, DMA_CHANNEL channel, DMA_CHANNEL_PRIORITY channelPriority)
{
     DMA_ChannelXPrioritySelect_Default(index, channel, channelPriority);
}

PLIB_INLINE_API DMA_CHANNEL_PRIORITY PLIB_DMA_ChannelXPriorityGet(DMA_MODULE_ID index, DMA_CHANNEL channel)
{
     return DMA_ChannelXPriorityGet_Default(index, channel);
}

PLIB_INLINE_API bool PLIB_DMA_ExistsChannelXEvent(DMA_MODULE_ID index)
{
     return DMA_ExistsChannelXEvent_Default(index);
}

PLIB_INLINE_API bool PLIB_DMA_ChannelXEventIsDetected(DMA_MODULE_ID index, DMA_CHANNEL channel)
{
     return DMA_ChannelXEventIsDetected_Default(index, channel);
}

PLIB_INLINE_API bool PLIB_DMA_ExistsChannelXAuto(DMA_MODULE_ID index)
{
     return DMA_ExistsChannelXAuto_Default(index);
}

PLIB_INLINE_API void PLIB_DMA_ChannelXAutoEnable(DMA_MODULE_ID index, DMA_CHANNEL channel)
{
     DMA_ChannelXAutoEnable_Default(index, channel);
}

PLIB_INLINE_API void PLIB_DMA_ChannelXAutoDisable(DMA_MODULE_ID index, DMA_CHANNEL channel)
{
     DMA_ChannelXAutoDisable_Default(index, channel);
}

PLIB_INLINE_API bool PLIB_DMA_ChannelXAutoIsEnabled(DMA_MODULE_ID index, DMA_CHANNEL channel)
{
     return DMA_ChannelXAutoIsEnabled_Default(index, channel);
}

PLIB_INLINE_API bool PLIB_DMA_ExistsChannelXChainEnbl(DMA_MODULE_ID index)
{
     return DMA_ExistsChannelXChainEnbl_Default(index);
}

PLIB_INLINE_API void PLIB_DMA_ChannelXChainEnable(DMA_MODULE_ID index, DMA_CHANNEL channel)
{
     DMA_ChannelXChainEnable_Default(index, channel);
}

PLIB_INLINE_API void PLIB_DMA_ChannelXChainDisable(DMA_MODULE_ID index, DMA_CHANNEL channel)
{
     DMA_ChannelXChainDisable_Default(index, channel);
}

PLIB_INLINE_API bool PLIB_DMA_ChannelXChainIsEnabled(DMA_MODULE_ID index, DMA_CHANNEL channel)
{
     return DMA_ChannelXChainIsEnabled_Default(index, channel);
}

PLIB_INLINE_API bool PLIB_DMA_ExistsChannelXDisabled(DMA_MODULE_ID index)
{
     return DMA_ExistsChannelXDisabled_Default(index);
}

PLIB_INLINE_API void PLIB_DMA_ChannelXDisabledEnablesEvents(DMA_MODULE_ID index, DMA_CHANNEL channel)
{
     DMA_ChannelXDisabledEnablesEvents_Default(index, channel);
}

PLIB_INLINE_API void PLIB_DMA_ChannelXDisabledDisablesEvents(DMA_MODULE_ID index, DMA_CHANNEL channel)
{
     DMA_ChannelXDisabledDisablesEvents_Default(index, channel);
}

PLIB_INLINE_API bool PLIB_DMA_ExistsChannelX(DMA_MODULE_ID index)
{
     return DMA_ExistsChannelX_Default(index);
}

PLIB_INLINE_API void PLIB_DMA_ChannelXEnable(DMA_MODULE_ID index, DMA_CHANNEL channel)
{
     DMA_ChannelXEnable_Default(index, channel);
}

PLIB_INLINE_API bool PLIB_DMA_ChannelXIsEnabled(DMA_MODULE_ID index, DMA_CHANNEL channel)
{
     return DMA_ChannelXIsEnabled_Default(index, channel);
}

PLIB_INLINE_API void PLIB_DMA_ChannelXDisable(DMA_MODULE_ID index, DMA_CHANNEL channel)
{
     DMA_ChannelXDisable_Default(index, channel);
}

PLIB_INLINE_API bool PLIB_DMA_ExistsChannelXChain(DMA_MODULE_ID index)
{
     return DMA_ExistsChannelXChain_Default(index);
}

PLIB_INLINE_API void PLIB_DMA_ChannelXChainToLower(DMA_MODULE_ID index, DMA_CHANNEL channel)
{
     DMA_ChannelXChainToLower_Default(index, channel);
}

PLIB_INLINE_API void PLIB_DMA_ChannelXChainToHigher(DMA_MODULE_ID index, DMA_CHANNEL channel)
{
     DMA_ChannelXChainToHigher_Default(index, channel);
}

PLIB_INLINE_API bool PLIB_DMA_ExistsChannelXPatternLength(DMA_MODULE_ID index)
{
     return DMA_ExistsChannelXPatternLength_Default(index);
}

PLIB_INLINE_API void PLIB_DMA_ChannelXPatternLengthSet(DMA_MODULE_ID index, DMA_CHANNEL dmaChannel, DMA_PATTERN_LENGTH patternLen)
{
     DMA_ChannelXPatternLengthSet_Default(index, dmaChannel, patternLen);
}

PLIB_INLINE_API DMA_PATTERN_LENGTH PLIB_DMA_ChannelXPatternLengthGet(DMA_MODULE_ID index, DMA_CHANNEL dmaChannel)
{
     return DMA_ChannelXPatternLengthGet_Default(index, dmaChannel);
}

PLIB_INLINE_API bool PLIB_DMA_ExistsChannelXPatternIgnoreByte(DMA_MODULE_ID index)
{
     return DMA_ExistsChannelXPatternIgnoreByte_Default(index);
}

PLIB_INLINE_API void PLIB_DMA_ChannelXPatternIgnoreByteEnable(DMA_MODULE_ID index, DMA_CHANNEL channel)
{
     DMA_ChannelXPatternIgnoreByteEnable_Default(index, channel);
}

PLIB_INLINE_API bool PLIB_DMA_ChannelXPatternIgnoreByteIsEnabled(DMA_MODULE_ID index, DMA_CHANNEL channel)
{
     return DMA_ChannelXPatternIgnoreByteIsEnabled_Default(index, channel);
}

PLIB_INLINE_API void PLIB_DMA_ChannelXPatternIgnoreByteDisable(DMA_MODULE_ID index, DMA_CHANNEL channel)
{
     DMA_ChannelXPatternIgnoreByteDisable_Default(index, channel);
}

PLIB_INLINE_API bool PLIB_DMA_ExistsChannelXBusy(DMA_MODULE_ID index)
{
     return DMA_ExistsChannelXBusy_Default(index);
}

PLIB_INLINE_API void PLIB_DMA_ChannelXBusyActiveSet(DMA_MODULE_ID index, DMA_CHANNEL channel)
{
     DMA_ChannelXBusyActiveSet_Default(index, channel);
}

PLIB_INLINE_API void PLIB_DMA_ChannelXBusyInActiveSet(DMA_MODULE_ID index, DMA_CHANNEL channel)
{
     DMA_ChannelXBusyInActiveSet_Default(index, channel);
}

PLIB_INLINE_API bool PLIB_DMA_ChannelXBusyIsBusy(DMA_MODULE_ID index, DMA_CHANNEL channel)
{
     return DMA_ChannelXBusyIsBusy_Default(index, channel);
}

PLIB_INLINE_API bool PLIB_DMA_ExistsChannelXPatternIgnore(DMA_MODULE_ID index)
{
     return DMA_ExistsChannelXPatternIgnore_Default(index);
}

PLIB_INLINE_API void PLIB_DMA_ChannelXPatternIgnoreSet(DMA_MODULE_ID index, DMA_CHANNEL channel, uint8_t pattern)
{
     DMA_ChannelXPatternIgnoreSet_Default(index, channel, pattern);
}

PLIB_INLINE_API uint8_t PLIB_DMA_ChannelXPatternIgnoreGet(DMA_MODULE_ID index, DMA_CHANNEL channel)
{
     return DMA_ChannelXPatternIgnoreGet_Default(index, channel);
}

PLIB_INLINE_API bool PLIB_DMA_ExistsChannelXTrigger(DMA_MODULE_ID index)
{
     return DMA_ExistsChannelXTrigger_Default(index);
}

PLIB_INLINE_API void PLIB_DMA_ChannelXTriggerEnable(DMA_MODULE_ID index, DMA_CHANNEL channel, DMA_CHANNEL_TRIGGER_TYPE trigger)
{
     DMA_ChannelXTriggerEnable_Default(index, channel, trigger);
}

PLIB_INLINE_API bool PLIB_DMA_ChannelXTriggerIsEnabled(DMA_MODULE_ID index, DMA_CHANNEL channel, DMA_CHANNEL_TRIGGER_TYPE trigger)
{
     return DMA_ChannelXTriggerIsEnabled_Default(index, channel, trigger);
}

PLIB_INLINE_API void PLIB_DMA_ChannelXTriggerDisable(DMA_MODULE_ID index, DMA_CHANNEL channel, DMA_CHANNEL_TRIGGER_TYPE trigger)
{
     DMA_ChannelXTriggerDisable_Default(index, channel, trigger);
}

PLIB_INLINE_API DMA_CHANNEL_INT_SOURCE PLIB_DMA_ChannelXTriggerSourceNumberGet(DMA_MODULE_ID index, DMA_CHANNEL channel)
{
     return DMA_ChannelXTriggerSourceNumberGet_Default(index, channel);
}

PLIB_INLINE_API bool PLIB_DMA_ExistsAbortTransfer(DMA_MODULE_ID index)
{
     return DMA_ExistsAbortTransfer_Default(index);
}

PLIB_INLINE_API void PLIB_DMA_AbortTransferSet(DMA_MODULE_ID index, DMA_CHANNEL channel)
{
     DMA_AbortTransferSet_Default(index, channel);
}

PLIB_INLINE_API bool PLIB_DMA_ExistsStartTransfer(DMA_MODULE_ID index)
{
     return DMA_ExistsStartTransfer_Default(index);
}

PLIB_INLINE_API void PLIB_DMA_StartTransferSet(DMA_MODULE_ID index, DMA_CHANNEL channel)
{
     DMA_StartTransferSet_Default(index, channel);
}

PLIB_INLINE_API bool PLIB_DMA_ExistsChannelXStartIRQ(DMA_MODULE_ID index)
{
     return DMA_ExistsChannelXStartIRQ_Default(index);
}

PLIB_INLINE_API void PLIB_DMA_ChannelXStartIRQSet(DMA_MODULE_ID index, DMA_CHANNEL channel, DMA_TRIGGER_SOURCE IRQnum)
{
     DMA_ChannelXStartIRQSet_Default(index, channel, IRQnum);
}

PLIB_INLINE_API bool PLIB_DMA_ExistsChannelXAbortIRQ(DMA_MODULE_ID index)
{
     return DMA_ExistsChannelXAbortIRQ_Default(index);
}

PLIB_INLINE_API void PLIB_DMA_ChannelXAbortIRQSet(DMA_MODULE_ID index, DMA_CHANNEL channel, DMA_TRIGGER_SOURCE IRQ)
{
     DMA_ChannelXAbortIRQSet_Default(index, channel, IRQ);
}

PLIB_INLINE_API bool PLIB_DMA_ExistsChannelXINTSourceFlag(DMA_MODULE_ID index)
{
     return DMA_ExistsChannelXINTSourceFlag_Default(index);
}

PLIB_INLINE_API bool PLIB_DMA_ChannelXINTSourceFlagGet(DMA_MODULE_ID index, DMA_CHANNEL dmaChannel, DMA_INT_TYPE dmaINTSource)
{
     return DMA_ChannelXINTSourceFlagGet_Default(index, dmaChannel, dmaINTSource);
}

PLIB_INLINE_API void PLIB_DMA_ChannelXINTSourceFlagSet(DMA_MODULE_ID index, DMA_CHANNEL dmaChannel, DMA_INT_TYPE dmaINTSource)
{
     DMA_ChannelXINTSourceFlagSet_Default(index, dmaChannel, dmaINTSource);
}

PLIB_INLINE_API void PLIB_DMA_ChannelXINTSourceFlagClear(DMA_MODULE_ID index, DMA_CHANNEL dmaChannel, DMA_INT_TYPE dmaINTSource)
{
     DMA_ChannelXINTSourceFlagClear_Default(index, dmaChannel, dmaINTSource);
}

PLIB_INLINE_API bool PLIB_DMA_ExistsChannelXINTSource(DMA_MODULE_ID index)
{
     return DMA_ExistsChannelXINTSource_Default(index);
}

PLIB_INLINE_API void PLIB_DMA_ChannelXINTSourceEnable(DMA_MODULE_ID index, DMA_CHANNEL dmaChannel, DMA_INT_TYPE dmaINTSource)
{
     DMA_ChannelXINTSourceEnable_Default(index, dmaChannel, dmaINTSource);
}

PLIB_INLINE_API void PLIB_DMA_ChannelXINTSourceDisable(DMA_MODULE_ID index, DMA_CHANNEL dmaChannel, DMA_INT_TYPE dmaINTSource)
{
     DMA_ChannelXINTSourceDisable_Default(index, dmaChannel, dmaINTSource);
}

PLIB_INLINE_API bool PLIB_DMA_ChannelXINTSourceIsEnabled(DMA_MODULE_ID index, DMA_CHANNEL dmaChannel, DMA_INT_TYPE dmaINTSource)
{
     return DMA_ChannelXINTSourceIsEnabled_Default(index, dmaChannel, dmaINTSource);
}

PLIB_INLINE_API bool PLIB_DMA_ExistsChannelXSourceStartAddress(DMA_MODULE_ID index)
{
     return DMA_ExistsChannelXSourceStartAddress_Default(index);
}

PLIB_INLINE_API uint32_t PLIB_DMA_ChannelXSourceStartAddressGet(DMA_MODULE_ID index, DMA_CHANNEL dmaChannel)
{
     return DMA_ChannelXSourceStartAddressGet_Default(index, dmaChannel);
}

PLIB_INLINE_API void PLIB_DMA_ChannelXSourceStartAddressSet(DMA_MODULE_ID index, DMA_CHANNEL dmaChannel, uint32_t sourceStartAddress)
{
     DMA_ChannelXSourceStartAddressSet_Default(index, dmaChannel, sourceStartAddress);
}

PLIB_INLINE_API bool PLIB_DMA_ExistsChannelXDestinationStartAddress(DMA_MODULE_ID index)
{
     return DMA_ExistsChannelXDestinationStartAddress_Default(index);
}

PLIB_INLINE_API uint32_t PLIB_DMA_ChannelXDestinationStartAddressGet(DMA_MODULE_ID index, DMA_CHANNEL dmaChannel)
{
     return DMA_ChannelXDestinationStartAddressGet_Default(index, dmaChannel);
}

PLIB_INLINE_API void PLIB_DMA_ChannelXDestinationStartAddressSet(DMA_MODULE_ID index, DMA_CHANNEL dmaChannel, uint32_t destinationStartAddress)
{
     DMA_ChannelXDestinationStartAddressSet_Default(index, dmaChannel, destinationStartAddress);
}

PLIB_INLINE_API bool PLIB_DMA_ExistsChannelXSourceSize(DMA_MODULE_ID index)
{
     return DMA_ExistsChannelXSourceSize_Default(index);
}

PLIB_INLINE_API uint16_t PLIB_DMA_ChannelXSourceSizeGet(DMA_MODULE_ID index, DMA_CHANNEL dmaChannel)
{
     return DMA_ChannelXSourceSizeGet_Default(index, dmaChannel);
}

PLIB_INLINE_API void PLIB_DMA_ChannelXSourceSizeSet(DMA_MODULE_ID index, DMA_CHANNEL dmaChannel, uint16_t sourceSize)
{
     DMA_ChannelXSourceSizeSet_Default(index, dmaChannel, sourceSize);
}

PLIB_INLINE_API bool PLIB_DMA_ExistsChannelXDestinationSize(DMA_MODULE_ID index)
{
     return DMA_ExistsChannelXDestinationSize_Default(index);
}

PLIB_INLINE_API uint16_t PLIB_DMA_ChannelXDestinationSizeGet(DMA_MODULE_ID index, DMA_CHANNEL dmaChannel)
{
     return DMA_ChannelXDestinationSizeGet_Default(index, dmaChannel);
}

PLIB_INLINE_API void PLIB_DMA_ChannelXDestinationSizeSet(DMA_MODULE_ID index, DMA_CHANNEL dmaChannel, uint16_t destinationSize)
{
     DMA_ChannelXDestinationSizeSet_Default(index, dmaChannel, destinationSize);
}

PLIB_INLINE_API bool PLIB_DMA_ExistsChannelXSourcePointer(DMA_MODULE_ID index)
{
     return DMA_ExistsChannelXSourcePointer_Default(index);
}

PLIB_INLINE_API uint16_t PLIB_DMA_ChannelXSourcePointerGet(DMA_MODULE_ID index, DMA_CHANNEL dmaChannel)
{
     return DMA_ChannelXSourcePointerGet_Default(index, dmaChannel);
}

PLIB_INLINE_API bool PLIB_DMA_ExistsChannelXDestinationPointer(DMA_MODULE_ID index)
{
     return DMA_ExistsChannelXDestinationPointer_Default(index);
}

PLIB_INLINE_API uint16_t PLIB_DMA_ChannelXDestinationPointerGet(DMA_MODULE_ID index, DMA_CHANNEL dmaChannel)
{
     return DMA_ChannelXDestinationPointerGet_Default(index, dmaChannel);
}

PLIB_INLINE_API bool PLIB_DMA_ExistsChannelXCellSize(DMA_MODULE_ID index)
{
     return DMA_ExistsChannelXCellSize_Default(index);
}

PLIB_INLINE_API uint16_t PLIB_DMA_ChannelXCellSizeGet(DMA_MODULE_ID index, DMA_CHANNEL dmaChannel)
{
     return DMA_ChannelXCellSizeGet_Default(index, dmaChannel);
}

PLIB_INLINE_API void PLIB_DMA_ChannelXCellSizeSet(DMA_MODULE_ID index, DMA_CHANNEL dmaChannel, uint16_t CellSize)
{
     DMA_ChannelXCellSizeSet_Default(index, dmaChannel, CellSize);
}

PLIB_INLINE_API bool PLIB_DMA_ExistsChannelXCellProgressPointer(DMA_MODULE_ID index)
{
     return DMA_ExistsChannelXCellProgressPointer_Default(index);
}

PLIB_INLINE_API uint16_t PLIB_DMA_ChannelXCellProgressPointerGet(DMA_MODULE_ID index, DMA_CHANNEL dmaChannel)
{
     return DMA_ChannelXCellProgressPointerGet_Default(index, dmaChannel);
}

PLIB_INLINE_API bool PLIB_DMA_ExistsChannelXPatternData(DMA_MODULE_ID index)
{
     return DMA_ExistsChannelXPatternData_Default(index);
}

PLIB_INLINE_API uint16_t PLIB_DMA_ChannelXPatternDataGet(DMA_MODULE_ID index, DMA_CHANNEL dmaChannel)
{
     return DMA_ChannelXPatternDataGet_Default(index, dmaChannel);
}

PLIB_INLINE_API void PLIB_DMA_ChannelXPatternDataSet(DMA_MODULE_ID index, DMA_CHANNEL dmaChannel, uint16_t patternData)
{
     DMA_ChannelXPatternDataSet_Default(index, dmaChannel, patternData);
}

#endif
