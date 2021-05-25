/** mbed Microcontroller Library
  ******************************************************************************
  * @file    cir_api.h
  * @author
  * @version V1.0.0
  * @brief   This file provides following mbed I2S API
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2006-2013 ARM Limited
  *
  * Licensed under the Apache License, Version 2.0 (the "License");
  * you may not use this file except in compliance with the License.
  * You may obtain a copy of the License at
  *
  *     http://www.apache.org/licenses/LICENSE-2.0
  *
  * Unless required by applicable law or agreed to in writing, software
  * distributed under the License is distributed on an "AS IS" BASIS,
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  ******************************************************************************
  */

#ifndef MBED_EXT_CIR_API_EXT_H
#define MBED_EXT_CIR_API_EXT_H
#if (defined(CONFIG_PLATFORM_8195BHP) && (CONFIG_PLATFORM_8195BHP == 1))  ||  (defined(CONFIG_PLATFORM_8195BLP) && (CONFIG_PLATFORM_8195BLP == 1))
///@name AmebaPro Only 
///@{
#include "device.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    NEC = 0x00,
    SONY = 0x01,
    PHILIPS = 0x02,
    JVC = 0x03
} cir_protocol;

typedef enum {
    INIT_OUTPUT_LOW = Init_Low,
    INIT_OUTPUT_HIGH = Init_High
} cir_init_value;

typedef enum {
    CIR_NO_CARRIER = No_Carrier,
    CIR_CARRIER = Carrier
} cir_carrier;

typedef enum {
    SCLK_DIV_AUTO = Auto
} cir_sclkdiv;

typedef enum {
    STAT_SUCCESS = cir_success,
    STAT_REPEAT = cir_repeat,
    STAT_FAIL = cir_fail,
    STAT_REPEAT_HEADER = cir_repeat_header
} cir_rx_status;

typedef enum {
    NEC_NORMAL_TX = NEC_Tx_Normal,
    NEC_REPEAT_TX = NEC_Tx_Repeat,
    NEC_EXTEND_TX = NEC_Tx_Extend
} cir_nec_tx_format;

typedef enum {
    NEC_NORMAL_RX = NEC_Rx_Normal,
    NEC_EXTEND_RX = NEC_Rx_Extend
} cir_nec_rx_format;

typedef enum {
    SONY_SIRC_12BITS = SonySIRC_12bits,
    SONY_SIRC_15BITS = SonySIRC_15bits,
    SONY_SIRC_20BITS = SonySIRC_20bits
} cir_sony_sirc_format;

typedef enum {
    PHILIPS_RC5 = Philips_rc5_Normal,
} cir_philips_rc5_format;

typedef enum {
    JVC_NORMAL_TX = Jvc_Tx_Normal,
    JVC_REPEAT_HEADER_TX = Jvc_Tx_Repeat_Header,
    JVC_REPEAT_NO_HEADER_TX = Jvc_Tx_Repeat_No_Header
} cir_jvc_tx_format;

typedef enum {
    JVC_NORMAL_RX = Jvc_Rx_Normal,
} cir_jvc_rx_format;


typedef union {
  __IOM uint32_t w;

  struct {
    __IOM uint32_t address      : 8;
    __IOM uint32_t command      : 8;
  } b;

} recv_nec_normal_format_t, *precv_nec_normal_format_t;

typedef union {
  __IOM uint32_t w;

  struct {
    __IOM uint32_t address_l    : 8;
    __IOM uint32_t address_h    : 8;
    __IOM uint32_t command      : 8;
  } b;

} recv_nec_extend_format_t, *precv_nec_extend_format_t;

typedef union {
  __IOM uint32_t w;

  struct {
    __IOM uint32_t command    : 7;
    __IOM uint32_t address    : 5;
  } b;

} recv_sony_src_12bits_t, *precv_sony_src_12bits_t;

typedef union {
  __IOM uint32_t w;

  struct {
    __IOM uint32_t command    : 7;
    __IOM uint32_t address    : 8;
  } b;

} recv_sony_src_15bits_t, *precv_sony_src_15bits_t;

typedef union {
  __IOM uint32_t w;

  struct {
    __IOM uint32_t command    : 7;
    __IOM uint32_t address    : 5;
    __IOM uint32_t extend     : 8;
  } b;

} recv_sony_src_20bits_t, *precv_sony_src_20bits_t;

typedef union {
  __IOM uint32_t w;

  struct {
    __IOM uint32_t field    : 1;
    __IOM uint32_t toggle   : 1;
    __IOM uint32_t address  : 5;
    __IOM uint32_t command  : 6;
  } b;

} recv_philips_rc5_format_t, *precv_philips_rc5_format_t;

typedef union {
  __IOM uint32_t w;

  struct {
    __IOM uint32_t address      : 8;
    __IOM uint32_t command      : 8;
  } b;

} recv_jvc_format_t, *precv_jvc_format_t;

/**
  \brief Define the CIR symbol.
*/
enum  cir_symbol_e {
    SYMBOL_NONE_RX = 0x00,
    SYMBOL_NONE_TX = 0x00,
    SYMBOL01 = 0x01,
    SYMBOL02 = 0x02,
    SYMBOL03 = 0x03,
    SYMBOL04 = 0x04,
    SYMBOL05 = 0x05,
    SYMBOL06 = 0x06,
    SYMBOL07 = 0x07,
    SYMBOL_END_TX  = 0x08,
    SYMBOL_NO_HEADER_TX  = 0x08
};
typedef uint8_t cir_symbol_t;

/**
  \brief The tx parameter table to describe the tx protocol .
*/
typedef struct cir_tx_protocol_s {
    u32 carrier_frequency;  /*!< Set the tx carrier frequency  */
    u16 symbol01_us;        /*!< Set the symbol01 duration   */
    u16 symbol02_us;        /*!< Set the symbol02 duration   */
    u16 symbol03_us;        /*!< Set the symbol03 duration   */
    u16 symbol04_us;        /*!< Set the symbol04 duration   */
    u16 symbol05_us;        /*!< Set the symbol05 duration   */
    u16 symbol06_us;        /*!< Set the symbol06 duration   */
    u16 symbol07_us;        /*!< Set the symbol07 duration   */

    cir_symbol_t  tx_start_first_symbol   : 4; /*!< Set the first tx symbol of encoding the start symbol  */
    cir_symbol_t  tx_start_second_symbol  : 4; /*!< Set the second tx symbol of encoding the start symbol  */
    cir_symbol_t  tx_bit0_first_symbol    : 4; /*!< Set the first tx symbol of encoding bit0  */
    cir_symbol_t  tx_bit0_second_symbol   : 4; /*!< Set the second tx symbol of encoding bit0  */
    cir_symbol_t  tx_bit1_first_symbol    : 4; /*!< Set the first tx symbol of encoding bit1  */
    cir_symbol_t  tx_bit1_second_symbol   : 4; /*!< Set the second tx symbol of encoding bit1  */
    cir_symbol_t  tx_end_first_symbol     : 4; /*!< Set the first tx symbol of encoding the end symbol  */
    cir_symbol_t  tx_end_second_symbol    : 4; /*!< Set the second tx symbol of encoding the end symbol  */

    u8 symbol01_type  : 1;  /*!< Set symbol01 carrier, 0: No carrier. 1: Carrier. */
    u8 symbol02_type  : 1;  /*!< Set symbol02 carrier, 0: No carrier. 1: Carrier. */
    u8 symbol03_type  : 1;  /*!< Set symbol03 carrier, 0: No carrier. 1: Carrier. */
    u8 symbol04_type  : 1;  /*!< Set symbol04 carrier, 0: No carrier. 1: Carrier. */
    u8 symbol05_type  : 1;  /*!< Set symbol05 carrier, 0: No carrier. 1: Carrier. */
    u8 symbol06_type  : 1;  /*!< Set symbol06 carrier, 0: No carrier. 1: Carrier. */
    u8 symbol07_type  : 1;  /*!< Set symbol07 carrier, 0: No carrier. 1: Carrier. */
} cir_tx_protocol_t, *pcir_tx_protocol_t;

/**
  \brief The rx parameter table to describe the rx protocol .
*/
typedef struct cir_rx_protocol_s {
    u32 carrier_frequency;  /*!< Set the rx carrier frequency   */
    u16 symbol01_us;  /*!< Set the symbol01 duration   */
    u16 symbol02_us;  /*!< Set the symbol02 duration   */
    u16 symbol03_us;  /*!< Set the symbol03 duration   */
    u16 symbol04_us;  /*!< Set the symbol04 duration   */
    u16 symbol05_us;  /*!< Set the symbol05 duration   */
    u16 symbol06_us;  /*!< Set the symbol06 duration   */
    u16 symbol07_us;  /*!< Set the symbol07 duration   */
    u16 symbol08_us;  /*!< Set the symbol08 duration   */

    cir_symbol_t  rx_start_first_symbol_gp0   : 4;     /*!< Start to decode the first symbol of the group 0  */
    cir_symbol_t  rx_start_second_symbol_gp0  : 4;     /*!< Start to decode the second symbol of the group 0  */
    cir_symbol_t  rx_start_first_symbol_gp1   : 4;     /*!< Start to decode the first symbol of the group 1  */
    cir_symbol_t  rx_start_second_symbol_gp1  : 4;     /*!< Start to decode the second symbol of the group 1  */
    cir_symbol_t  rx_start_first_symbol_gp2   : 4;     /*!< Start to decode the first symbol of the group 2  */
    cir_symbol_t  rx_start_second_symbol_gp2  : 4;     /*!< Start to decode the second symbol of the group 2  */
    cir_symbol_t  rx_start_first_symbol_gp3   : 4;     /*!< Start to decode the first symbol of the group 3  */
    cir_symbol_t  rx_start_second_symbol_gp3  : 4;     /*!< Start to decode the second symbol of the group 3  */

    cir_symbol_t  rx_bit0_first_symbol_gp0    : 4;     /*!< Set the first symbol of the group 0 to decode bit 0  */
    cir_symbol_t  rx_bit0_second_symbol_gp0   : 4;     /*!< Set the second symbol of the group 0 to decode bit 0  */
    cir_symbol_t  rx_bit0_first_symbol_gp1    : 4;     /*!< Set the first symbol of the group 1 to decode bit 0  */
    cir_symbol_t  rx_bit0_second_symbol_gp1   : 4;     /*!< Set the second symbol of the group 1 to decode bit 0  */

    cir_symbol_t  rx_bit1_first_symbol_gp0    : 4;     /*!< Set the first symbol of the group 0 to decode bit 1  */
    cir_symbol_t  rx_bit1_second_symbol_gp0   : 4;     /*!< Set the second symbol of the group 0 to decode bit 1  */
    cir_symbol_t  rx_bit1_first_symbol_gp1    : 4;     /*!< Set the first symbol of the group 1 to decode bit 1  */
    cir_symbol_t  rx_bit1_second_symbol_gp1   : 4;     /*!< Set the second symbol of the group 1 to decode bit 1  */

    cir_symbol_t  rx_separate_symbol_in_gp0       : 4; /*!< Separate the rx symbol of the group 0  */
    cir_symbol_t  rx_separate_first_symbol_gp0    : 4; /*!< Set the first generated symbol of the group 0  */
    cir_symbol_t  rx_separate_second_symbol_gp0   : 4; /*!< Set the second generated symbol of the group 0  */
    cir_symbol_t  rx_separate_symbol_in_gp1       : 4; /*!< Separate the rx symbol of the group 1  */
    cir_symbol_t  rx_separate_first_symbol_gp1    : 4; /*!< Set the first generated symbol of the group 1  */
    cir_symbol_t  rx_separate_second_symbol_gp1   : 4; /*!< Set the second generated symbol of the group 1  */

    u8  symbol01_type  : 1;    /*!< Set symbol01 carrier, 0: No carrier. 1: Carrier. */
    u8  symbol02_type  : 1;    /*!< Set symbol02 carrier, 0: No carrier. 1: Carrier. */
    u8  symbol03_type  : 1;    /*!< Set symbol03 carrier, 0: No carrier. 1: Carrier. */
    u8  symbol04_type  : 1;    /*!< Set symbol04 carrier, 0: No carrier. 1: Carrier. */
    u8  symbol05_type  : 1;    /*!< Set symbol05 carrier, 0: No carrier. 1: Carrier. */
    u8  symbol06_type  : 1;    /*!< Set symbol06 carrier, 0: No carrier. 1: Carrier. */
    u8  symbol07_type  : 1;    /*!< Set symbol07 carrier, 0: No carrier. 1: Carrier. */
    u8  symbol08_type  : 1;    /*!< Set symbol08 carrier, 0: No carrier. 1: Carrier. */
} cir_rx_protocol_t, *pcir_rx_protocol_t;


typedef void (*cir_irq_handler)(void *);

typedef struct cir_s cir_t;

/**
  * @brief  Initializes the CIR device, include clock/function/interrupt/CIR registers.
  * @param  obj: CIR object define in application software.
  * @param  tx: Tx PinName according to pinmux spec. This pin is not configured when set PinName "NC".
  * @param  rx: Rx PinName according to pinmux spec. This pin is not configured when set PinName "NC".
  * @retval none
  */
void cir_init (cir_t *obj, PinName tx, PinName rx);

/**
  * @brief  Deinitializes the CIR device, include function/interrupt/CIR registers.
  * @param  obj: CIR object define in application software.
  * @retval none
  */
void cir_deinit (cir_t *obj);

/**
  * @brief  Set the protocol and tx parameters.
  * @param  obj: CIR object define in application software.
  *		@arg 0 : NEC protocol.
  *		@arg 1 : SONY protocol.
  *		@arg 2 : PHILIPS protocol.
  *		@arg 3 : JVC protocol.  
  * @param  output_init: Set the initial output.
  *		@arg 0 : The output is low.
  *		@arg 1 : The output is high.
  * @param  carrier: Set the waveform with carrier or no carrier.
  *		@arg 0 : The carrier symbol has no carrier.
  *		@arg 1 : The carrier symbol has carrier.
  * @param  tx_sclkdiv: The divisor of the system clock to generate a proper tick time for the CIR carrier generation.
  *		@arg 0 : Generate the appropriate value automatically. (Suggestion)
  *		@arg 1 ~ 255 : SCLK / tx_sclkdiv.
  * @param  irq_tx_end_cb: User defined IRQ callback function. When finish the output, generate the interrupt.
  * @param  irq_tx_end_arg: User defined IRQ callback parameter.
  * @param  protocol: Set the protocol.
  * @retval none
  */
void cir_set_tx_protocol (cir_t *obj, cir_protocol protocol, cir_init_value output_init, cir_carrier carrier, cir_sclkdiv tx_sclkdiv,
                            cir_irq_handler irq_tx_end_cb, void *irq_tx_end_arg);

/**
  * @brief  Set the protocol and tx parameters.
  * @param  obj: CIR object define in application software.
  * @param  protocol: Set the protocol.
  *		@arg 0 : NEC protocol.
  *		@arg 1 : SONY protocol.
  *		@arg 2 : PHILIPS protocol.
  *		@arg 3 : JVC protocol.  
  * @param  output_init: Set the initial output.
  *		@arg 0 : The output is low.
  *		@arg 1 : The output is high.
  * @param  carrier: Set the waveform with carrier or no carrier.
  *		@arg 0 : The carrier symbol has no carrier.
  *		@arg 1 : The carrier symbol has carrier.
  * @param  rx_sclkdiv: The divisor of the system clock to generate a proper clock for the input sampling and trigger tick event.
  *		@arg 0 : Set the appropriate value automatically. (Suggestion)
  *		@arg 1 ~ 255 : SCLK / rx_sclkdiv.
  * @param  symbol_time_tolerance_us: Set the time tolerance for demodulating RX symbols. This unit is us.
  * @param  carrier_tolerance_cnt: Set the tolerance of carrier numbers for demodulating RX symbols.
  *		@arg 0 : This setting is "0" when this waveform is no carrier.
  * @param  check_frame_end_time_us: Set the end time after receiving the last symbol. This unit is us.
  * @param  irq_tx_end_cb: User defined IRQ callback function. When finish to receive data, generate the interrupt.
  * @param  irq_tx_end_arg: User defined IRQ callback parameter.
  * @retval none
  */
void cir_set_rx_protocol (cir_t *obj, cir_protocol protocol, cir_init_value input_init, cir_carrier carrier, cir_sclkdiv rx_sclkdiv,
                            u16 symbol_time_tolerance_us, u16 carrier_tolerance_cnt, u16 check_frame_end_time_us,
                            cir_irq_handler irq_rx_end_cb, void *irq_rx_end_arg);

/**
  * @brief  Set the rx data pointer by using the normal NEC protocol.
  * @param  obj: CIR object define in application software.
  * @param  data: Rx data pointer.
  * @retval none
  */
void cir_nec_recv_normal_format (cir_t *obj, u32 *data);

/**
  * @brief  Set the rx data pointer by using the extend NEC protocol.
  * @param  obj: CIR object define in application software.
  * @param  data: Rx data pointer.
  * @retval none
  */
void cir_nec_recv_extend_format (cir_t *obj, u32 *data);

/**
  * @brief  Send the tx data by using the normal NEC protocol.
  * @param  obj: CIR object define in application software.
  * @param  address: The protocol address.
  * @param  command: The protocol command.
  * @retval none
  */
void cir_nec_send_normal_format (cir_t *obj, u8 address, u8 command);

/**
  * @brief  Send the tx data by using the extend NEC protocol.
  * @param  obj: CIR object define in application software.
  * @param  address_l: The protocol high address.
  * @param  address_h: The protocol low address.
  * @param  command: The protocol command.
  * @retval none
  */
void cir_nec_send_extend_format (cir_t *obj, u8 address_l, u8 address_h, u8 command);

/**
  * @brief  Send the repeat format by using the NEC protocol.
  * @param  obj: CIR object define in application software.
  * @retval none
  */
void cir_nec_send_repeat_format (cir_t *obj);

/**
  * @brief  Monitor the specific rx data by using the normal NEC protocol, and generate the interrupt.
  * @param  obj: CIR object define in application software.
  * @param  address: Monitor this address.
  * @param  command: Monitor this command.
  * @param  irq_monitor_cb: User defined IRQ callback function.
  * @param  irq_monitor_arg: User defined IRQ callback parameter.
  * @retval none
  */
void cir_monitor_nec_normal_format (cir_t *obj, u8 address, u8 command, cir_irq_handler irq_monitor_cb, void *irq_monitor_arg);

/**
  * @brief  Monitor the specific rx data by using the extend NEC protocol, and generate the interrupt.
  * @param  obj: CIR object define in application software.
  * @param  address_l: Monitor this low address.
  * @param  address_h: Monitor this high address.
  * @param  command: Monitor this command.
  * @param  irq_monitor_cb: User defined IRQ callback function.
  * @param  irq_monitor_arg: User defined IRQ callback parameter.
  * @retval none
  */
void cir_monitor_nec_extend_format (cir_t *obj, u8 address_l, u8 address_h, u8 command, cir_irq_handler irq_monitor_cb, void *irq_monitor_arg);

/**
  * @brief  Get the address of the normal NEC protocol.
  * @param  obj: CIR object define in application software.
  * @retval The 8-bit address.
  */
u8 cir_nec_get_normal_address (cir_t *obj);

/**
  * @brief  Get the command of the normal NEC protocol.
  * @param  obj: CIR object define in application software.
  * @retval The 8-bit command.
  */
u8 cir_nec_get_normal_command (cir_t *obj);

/**
  * @brief  Get the address of the extend NEC protocol.
  * @param  obj: CIR object define in application software.
  * @retval The 8-bit low address.
  */
u8 cir_nec_get_extend_address_low (cir_t *obj);

/**
  * @brief  Get the address of the extend NEC protocol.
  * @param  obj: CIR object define in application software.
  * @retval The 8-bit high address.
  */
u8 cir_nec_get_extend_address_high (cir_t *obj);

/**
  * @brief  Get the command of the extend NEC protocol.
  * @param  obj: CIR object define in application software.
  * @retval The 8-bit command.
  */
u8 cir_nec_get_extend_command (cir_t *obj);

/**
  * @brief  Set the rx data pointer by using the 12-bit SONY protocol.
  * @param  obj: CIR object define in application software.
  * @param  data: Rx data pointer.
  * @retval none
  */
void cir_sony_recv_12bit_format (cir_t *obj, u32 *data);

/**
  * @brief  Set the rx data pointer by using the 15-bit SONY protocol.
  * @param  obj: CIR object define in application software.
  * @param  data: Rx data pointer.
  * @retval none
  */
void cir_sony_recv_15bit_format (cir_t *obj, u32 *data);

/**
  * @brief  Set the rx data pointer by using the 20-bit SONY protocol.
  * @param  obj: CIR object define in application software.
  * @param  data: Rx data pointer.
  * @retval none
  */
void cir_sony_recv_20bit_format (cir_t *obj, u32 *data);

/**
  * @brief  Send the tx data by using the 12-bit SONY protocol.
  * @param  obj: CIR object define in application software.
  * @param  address_5bits: The 5-bit address.
  * @param  command_7bits: The 7-bit command.
  * @retval none
  */
void cir_sony_send_12bit_format (cir_t *obj, u8 address_5bits, u8 command_7bits);

/**
  * @brief  Send the tx data by using the 15-bit SONY protocol.
  * @param  obj: CIR object define in application software.
  * @param  address_8bits: The 8-bit address.
  * @param  command_7bits: The 7-bit command.
  * @retval none
  */
void cir_sony_send_15bit_format (cir_t *obj, u8 address_8bits, u8 command_7bits);

/**
  * @brief  Send the tx data by using the 20-bit SONY protocol.
  * @param  obj: CIR object define in application software.
  * @param  address_5bits: The 5-bit address.
  * @param  command_7bits: The 7-bit command.
  * @param  extend_8bits: The 8-bit extend.
  * @retval none
  */
void cir_sony_send_20bit_format (cir_t *obj, u8 address_5bits, u8 command_7bits, u8 extend_8bits);

/**
  * @brief  Monitor the specific rx data by using the 12-bit SONY protocol, and generate the interrupt.
  * @param  obj: CIR object define in application software.
  * @param  address_5bits: Monitor this 5-bit address.
  * @param  command_7bits: Monitor this 7-bit command.
  * @param  irq_monitor_cb: User defined IRQ callback function.
  * @param  irq_monitor_arg: User defined IRQ callback parameter.
  * @retval none
  */
void cir_monitor_sony_12bit_format (cir_t *obj, u8 address_5bits, u8 command_7bits, cir_irq_handler irq_monitor_cb, void *irq_monitor_arg);

/**
  * @brief  Monitor the specific rx data by using the 15-bit SONY protocol, and generate the interrupt.
  * @param  obj: CIR object define in application software.
  * @param  address_8bits: Monitor this 8-bit address.
  * @param  command_7bits: Monitor this 7-bit command.
  * @param  irq_monitor_cb: User defined IRQ callback function.
  * @param  irq_monitor_arg: User defined IRQ callback parameter.
  * @retval none
  */
void cir_monitor_sony_15bit_format (cir_t *obj, u8 address_8bits, u8 command_7bits, cir_irq_handler irq_monitor_cb, void *irq_monitor_arg);

/**
  * @brief  Monitor the specific rx data by using the 20-bit SONY protocol, and generate the interrupt.
  * @param  obj: CIR object define in application software.
  * @param  address_5bits: Monitor this 5-bit address.
  * @param  command_7bits: Monitor this 7-bit command.
  * @param  extend_8bits: Monitor this 8-bit extend.
  * @param  irq_monitor_cb: User defined IRQ callback function.
  * @param  irq_monitor_arg: User defined IRQ callback parameter.
  * @retval none
  */
void cir_monitor_sony_20bit_format (cir_t *obj, u8 address_5bits, u8 command_7bits, u8 extend_8bits, cir_irq_handler irq_monitor_cb, void *irq_monitor_arg);

/**
  * @brief  Get the address of the 12-bit SONY protocol.
  * @param  obj: CIR object define in application software.
  * @retval The 5-bit address.
  */
u8 cir_sony_get_12bit_format_address (cir_t *obj);

/**
  * @brief  Get the command of the 12-bit SONY protocol.
  * @param  obj: CIR object define in application software.
  * @retval The 7-bit command.
  */
u8 cir_sony_get_12bit_format_command (cir_t *obj);

/**
  * @brief  Get the address of the 15-bit SONY protocol.
  * @param  obj: CIR object define in application software.
  * @retval The 8-bit address.
  */
u8 cir_sony_get_15bit_format_address (cir_t *obj);

/**
  * @brief  Get the command of the 15-bit SONY protocol.
  * @param  obj: CIR object define in application software.
  * @retval The 7-bit command.
  */
u8 cir_sony_get_15bit_format_command (cir_t *obj);

/**
  * @brief  Get the address of the 20-bit SONY protocol.
  * @param  obj: CIR object define in application software.
  * @retval The 5-bit address.
  */
u8 cir_sony_get_20bit_format_address (cir_t *obj);

/**
  * @brief  Get the command of the 20-bit SONY protocol.
  * @param  obj: CIR object define in application software.
  * @retval The 7-bit command.
  */
u8 cir_sony_get_20bit_format_command (cir_t *obj);

/**
  * @brief  Get the extend of the 20-bit SONY protocol.
  * @param  obj: CIR object define in application software.
  * @retval The 8-bit extend.
  */
u8 cir_sony_get_20bit_format_extend (cir_t *obj);

/**
  * @brief  Set the rx data pointer by using the PHILIPS RC5 protocol.
  * @param  obj: CIR object define in application software.
  * @param  data: Rx data pointer.
  * @retval none
  */
void cir_philips_rc5_recv_format (cir_t *obj, u32 *data);

/**
  * @brief  Send the tx data by using the PHILIPS RC5 protocol.
  * @param  obj: CIR object define in application software.
  * @param  field_1bits: The 1-bit field.
  * @param  toggle_1bits: The 1-bit toggle.
  * @param  address_5bits: The 5-bit address.
  * @param  command_6bits: The 6-bit command.
  * @retval none
  */
void cir_philips_rc5_send_format (cir_t *obj, u8 field_1bits, u8 toggle_1bits, u8 address_5bits, u8 command_6bits);

/**
  * @brief  Monitor the specific rx data by using the PHILIPS RC5 protocol, and generate the interrupt.
  * @param  obj: CIR object define in application software.
  * @param  field_1bits: Monitor this 1-bit field.
  * @param  toggle_1bits: Monitor this 1-bit toggle.
  * @param  address_5bits: Monitor this 5-bit address.
  * @param  command_6bits: Monitor this 6-bit command.
  * @param  irq_monitor_cb: User defined IRQ callback function.
  * @param  irq_monitor_arg: User defined IRQ callback parameter.
  * @retval none
  */
void cir_monitor_philips_rc5_format (cir_t *obj, u8 field_1bits, u8 toggle_1bits, u8 address_5bits, u8 command_6bits, cir_irq_handler irq_monitor_cb, void *irq_monitor_arg);

/**
  * @brief  Get the address of the PHILIPS RC5 protocol.
  * @param  obj: CIR object define in application software.
  * @retval The 5-bit address.
  */
u8 cir_philips_rc5_get_address (cir_t *obj);

/**
  * @brief  Get the command of the PHILIPS RC5 protocol.
  * @param  obj: CIR object define in application software.
  * @retval The 6-bit command.
  */
u8 cir_philips_rc5_get_command (cir_t *obj);

/**
  * @brief  Get the field of the PHILIPS RC5 protocol.
  * @param  obj: CIR object define in application software.
  * @retval The 1-bit field.
  */
u8 cir_philips_rc5_get_format_field (cir_t *obj);

/**
  * @brief  Get the toggle of the PHILIPS RC5 protocol.
  * @param  obj: CIR object define in application software.
  * @retval The 1-bit toggle.
  */
u8 cir_philips_rc5_get_format_toggle (cir_t *obj);

/**
  * @brief  Set the rx data pointer by using the JVC protocol.
  * @param  obj: CIR object define in application software.
  * @param  data: Rx data pointer.
  * @retval none
  */
void cir_jvc_recv_format (cir_t *obj, u32 *data);

/**
  * @brief  Send the tx data by using the normal format of the JVC protocol.
  * @param  obj: CIR object define in application software.
  * @param  address_8bits: The 8-bit address.
  * @param  command_8bits: The 8-bit command.
  * @retval none
  */
void cir_jvc_send_normal_format (cir_t *obj, u8 address_8bits,  u8 command_8bits);

/**
  * @brief  Send the tx data by using the header and repeat format of the JVC protocol.
  * @param  obj: CIR object define in application software.
  * @param  address_8bits: The 8-bit address.
  * @param  command_8bits: The 8-bit command.
  * @retval none
  */
void cir_jvc_send_repeat_header_format (cir_t *obj, u8 address_8bits,  u8 command_8bits);

/**
  * @brief  Send the tx data by using the repeat format of the JVC protocol.
  * @param  obj: CIR object define in application software.
  * @param  address_8bits: The 8-bit address.
  * @param  command_8bits: The 8-bit command.
  * @retval none
  */
void cir_jvc_send_repeat_no_header_format (cir_t *obj, u8 address_8bits,  u8 command_8bits);

/**
  * @brief  Monitor the specific rx data by using the JVC protocol, and generate the interrupt.
  * @param  obj: CIR object define in application software.
  * @param  address_8bits: Monitor this 8-bit address.
  * @param  command_8bits: Monitor this 8-bit command.
  * @param  irq_monitor_cb: User defined IRQ callback function.
  * @param  irq_monitor_arg: User defined IRQ callback parameter.
  * @retval none
  */
void cir_monitor_jvc_format (cir_t *obj, u8 address_8bits,  u8 command_8bits, cir_irq_handler irq_monitor_cb, void *irq_monitor_arg);

/**
  * @brief  Get the address of the JVC protocol.
  * @param  obj: CIR object define in application software.
  * @retval The 8-bit address.
  */
u8 cir_jvc_get_address (cir_t *obj);

/**
  * @brief  Get the command of the JVC protocol.
  * @param  obj: CIR object define in application software.
  * @retval The 8-bit command.
  */
u8 cir_jvc_get_command (cir_t *obj);

/**
  * @brief  Get the receiving status by using NEC, SONY, PHILIPS RC5, or JVC protocol.
  * @param  obj: CIR object define in application software.
  * @retval STAT_SUCCESS: The receiving is the normal format.
  * @retval STAT_REPEAT: The receiving is the repeat format.
  * @retval STAT_FAIL: The receiving is failed.
  * @retval STAT_REPEAT_HEADER: The receiving is the header and repeat format of the JVC protocol.
  */
cir_rx_status cir_get_protocol_rx_status (cir_t *obj);

/**
  * @brief  Monitor the specific rx data, and generate the interrupt.
  * @param  obj: CIR object define in application software.
  * @param  en:  To enable or disable.
  * @param  monitor_data: Monitor the data.
  * @param  monitor_cnt: Monitor the bit length. Monitor counts 1~32 from the LSB. 0 is inactive.
  * @param  irq_monitor_cb: User defined IRQ callback function.
  * @param  irq_monitor_arg: User defined IRQ callback parameter.
  * @retval none
  */
void cir_recv_monitor_mode (cir_t *obj, BOOL en, u32 monitor_data,  u32 monitor_cnt, cir_irq_handler monitor_cb, void *pmonitor_arg);

/**
  * @brief  Make the new tx protocol.
  * @param  obj: CIR object define in application software.
  * @param  ptx_protocol: Set the tx protocol table.
  * @param  output_init - Set the tx initial level.
  * @param  carrier - Let the carrier symbol of the tx protocol table generate the carrier.
  *		@arg 0 : The carrier symbol has no carrier.
  *		@arg 1 : The carrier symbol has carrier.
  * @param  tx_sclkdiv: The divisor of the system clock is to generate a proper tick time for the CIR carrier generation.
  *		@arg 0 : Generate the appropriate value automatically. (Suggestion)
  *		@arg 1 ~ 255 : SCLK / tx_sclkdiv.
  * @param  irq_tx_end_cb: User defined IRQ callback function.
  * @param  irq_tx_end_arg: User defined IRQ callback parameter.
  * @retval none
  */
void cir_make_tx_protocol (cir_t *obj, cir_tx_protocol_t *ptx_protocol, cir_init_value output_init, cir_carrier carrier, cir_sclkdiv tx_sclkdiv,
                            cir_irq_handler irq_tx_end_cb, void *irq_tx_end_arg);

/**
  * @brief  Change the tx start symbol of tx protocol table according to the protocol header.
  * @param  obj: CIR object define in application software.
  * @param  first_symbol: Set the tx protocol table.
  * @param  second_symbol: Set the tx protocol table.
  * @retval none
  */
void cir_change_tx_start_symbol (cir_t *obj, cir_symbol_t first_symbol, cir_symbol_t second_symbol);

/**
  * @brief  Change the tx bit0 symbol of tx protocol table.
  * @param  obj: CIR object define in application software.
  * @param  first_symbol: Set the tx protocol table.
  * @param  second_symbol: Set the tx protocol table.
  * @retval none
  */
void cir_change_tx_bit0_symbol (cir_t *obj, cir_symbol_t first_symbol, cir_symbol_t second_symbol);

/**
  * @brief  Change the tx bit1 symbol of tx protocol table.
  * @param  obj: CIR object define in application software.
  * @param  first_symbol: Set the tx protocol table.
  * @param  second_symbol: Set the tx protocol table.
  * @retval none
  */
void cir_change_tx_bit1_symbol (cir_t *obj, cir_symbol_t first_symbol, cir_symbol_t second_symbol);

/**
  * @brief  Change the tx end symbol of tx protocol table.
  * @param  obj: CIR object define in application software.
  * @param  first_symbol: Set the tx protocol table.
  * @param  second_symbol: Set the tx protocol table.
  * @retval none
  */
void cir_change_tx_end_symbol (cir_t *obj, cir_symbol_t first_symbol, cir_symbol_t second_symbol);

/**
  * @brief  Send the tx data.
  * @param  obj: CIR object define in application software.
  * @param  data : The TX data pinter.
  * @param  length: The TX data length.
  * @retval none
  */
void cir_send_make_protocol (cir_t *obj, u32 *data, u32 length);

/**
  * @brief  Set the protocol and tx parameters.
  * @param  obj: CIR object define in application software.
  * @param  prx_protocol: Set the rx protocol table.
  * @param  output_init: Set the initial output.
  *		@arg 0 : The output is low.
  *		@arg 1 : The output is high.
  * @param  carrier: Set the waveform with carrier or no carrier.
  *		@arg 0 : The carrier symbol has no carrier.
  *		@arg 1 : The carrier symbol has carrier.
  * @param  rx_sclkdiv: The divisor of the system clock to generate a proper clock for the input sampling and trigger tick event.
  *		@arg 0 : Set the appropriate value automatically. (Suggestion)
  *		@arg 1 ~ 255 : SCLK / rx_sclkdiv.
  * @param  symbol_time_tolerance_us: Set the time tolerance for demodulating RX symbols. This unit is us.
  * @param  carrier_tolerance_cnt: Set the tolerance of carrier numbers for demodulating RX symbols.
  *		@arg 0 : This setting is "0" when this waveform is no carrier.
  * @param  check_frame_end_time_us: Set the end time after receiving the last symbol. This unit is us.
  * @param  irq_tx_end_cb: User defined IRQ callback function. When finish to receive data, generate the interrupt.
  * @param  irq_tx_end_arg: User defined IRQ callback parameter.
  * @retval none
  */
void cir_make_rx_protocol (cir_t *obj, cir_rx_protocol_t *prx_protocol, cir_init_value input_init, cir_carrier carrier, cir_sclkdiv rx_sclkdiv,
                            u16 symbol_time_tolerance_us, u16 carrier_tolerance_cnt, u16 check_frame_end_time_us,
                            cir_irq_handler irq_rx_end_cb, void *irq_rx_end_arg);

/**
  * @brief  Set the rx data pointer.
  * @param  obj: CIR object define in application software.
  * @param  data: Set the RX data pointer.
  * @param  data_32bits_num: Set the limit number of RX data, and the unit is 32 bits.
  *		@arg 0 : Do nothing.
  *		@arg 1 : the limit number <= 32bits.
  *		@arg 2 : 32bits < the limit number <= 64bits.
  *		@arg 3 : ...
  *		@arg n : (32*(n-1))bits < the limit number <= (32*n)bits.
  * @retval none
  */
void cir_recv_make_protocol (cir_t *obj, u32 *data, u32 data_32bits_num);

/**
  * @brief  Get the number of decoded bits when receive the data.
  * @param  obj: CIR object define in application software.
  * @retval The number of decoded bits.
  */
u32 cir_recv_bit_length (cir_t *obj);

/**
 *  @brief Check the matching status in order to recognize the decoding start-symbol.
 *         That is to say, use to recognize the different header.
 *  @param  obj: CIR object define in application software.
 *  @retval The start matching status. 0: Not match the decoding symbols of group 0, 1: Match the decoding symbols of group 0.
 */
BOOL cir_get_start_match_gp0_status (cir_t *obj);

/**
 *  @brief Check the matching status in order to recognize the decoding start-symbol.
 *         That is to say, use to recognize the different header.
 *  @param  obj: CIR object define in application software.
 *  @retval The start matching status. 0: Not match the decoding symbols of group 1, 1: Match the decoding symbols of group 1.
 */
BOOL cir_get_start_match_gp1_status (cir_t *obj);

/**
  * @brief  Reset CIR and registers.
  * @retval none
  */
void cir_reset (void);


/*\@}*/

#ifdef __cplusplus
}
#endif
///@}
#endif  // end of "#if defined(CONFIG_PLATFORM_8195BHP) || defined(CONFIG_PLATFORM_8195BLP)"
#endif

