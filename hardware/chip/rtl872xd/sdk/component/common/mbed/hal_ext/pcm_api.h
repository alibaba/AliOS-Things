/** mbed Microcontroller Library
  ******************************************************************************
  * @file    pcm_api.h
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

#ifndef MBED_EXT_PCM_API_EXT_H
#define MBED_EXT_PCM_API_EXT_H

#include "device.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup pcm PCM
 *  @ingroup    hal
 *  @brief      pcm functions
 *  @{
 */

typedef enum {
	U_LAW = PCM_U_LAW,
	A_LAW = PCM_A_LAW
} pcm_ua_law;

typedef enum {
    CH0  = PCM_CH0,
    CH1  = PCM_CH1,
    CH2  = PCM_CH2,
    CH3  = PCM_CH3,
    CH4  = PCM_CH4,
    CH5  = PCM_CH5,
    CH6  = PCM_CH6,
    CH7  = PCM_CH7,
    CH8  = PCM_CH8,
    CH9  = PCM_CH9,
    CH10 = PCM_CH10,
    CH11 = PCM_CH11,
    CH12 = PCM_CH12,
    CH13 = PCM_CH13,
    CH14 = PCM_CH14,
    CH15 = PCM_CH15
} pcm_ch;

typedef enum {
    SLOT0  = PCM_SLOT0,
    SLOT1  = PCM_SLOT1,
    SLOT2  = PCM_SLOT2,
    SLOT3  = PCM_SLOT3,
    SLOT4  = PCM_SLOT4,
    SLOT5  = PCM_SLOT5,
    SLOT6  = PCM_SLOT6,
    SLOT7  = PCM_SLOT7,
    SLOT8  = PCM_SLOT8,
    SLOT9  = PCM_SLOT9,
    SLOT10 = PCM_SLOT10,
    SLOT11 = PCM_SLOT11,
    SLOT12 = PCM_SLOT12,
    SLOT13 = PCM_SLOT13,
    SLOT14 = PCM_SLOT14,
    SLOT15 = PCM_SLOT15,
    SLOT16 = PCM_SLOT16,
    SLOT17 = PCM_SLOT17,
    SLOT18 = PCM_SLOT18,
    SLOT19 = PCM_SLOT19,
    SLOT20 = PCM_SLOT20,
    SLOT21 = PCM_SLOT21,
    SLOT22 = PCM_SLOT22,
    SLOT23 = PCM_SLOT23,
    SLOT24 = PCM_SLOT24,
    SLOT25 = PCM_SLOT25,
    SLOT26 = PCM_SLOT26,
    SLOT27 = PCM_SLOT27,
    SLOT28 = PCM_SLOT28,
    SLOT29 = PCM_SLOT29,
    SLOT30 = PCM_SLOT30,
    SLOT31 = PCM_SLOT31
} pcm_slot;

typedef enum {
    PCM_MASTER = PCM_MASTER_MODE,
    PCM_SLAVE = PCM_SLAVE_MODE
} pcm_ms_mode;

typedef enum {
	FRAME_HIGH_ACTIVE = PCM_FRAME_HIGH_ACTIVE,
	FRAME_LOW_ACTIVE = PCM_FRAME_LOW_ACTIVE
} pcm_frame_active;

typedef enum {
	COMPENDER_MODE = PCM_COMPENDER_MODE,
	LINEAR_MODE = PCM_LINEAR_MODE
} pcm_mode;

typedef enum {
	PCM_LOOPBACK_DIS = PCM_LOOPBACK_DISABLE,
	PCM_LOOPBACK_EN = PCM_LOOPBACK_ENABLE
} pcm_loopback_mode;

typedef enum {
	ENDIANSWAP_DISABLE = PCM_ENDIANSWAP_DISABLE,
	ENDIANSWAP_ENABLE = PCM_ENDIANSWAP_ENABLE
} pcm_endian_swap;

typedef enum {
	PCM_PAGE0 = 0,
	PCM_PAGE1 = 1
} pcm_page;

typedef void (*pcm_irq_handler)(void *data, u16 *pbuf);

typedef struct pcm_s pcm_t;


/**
  * @brief  Initializes the PCM device, include clock/function/interrupt/PCM registers.
  * @param  obj: PCM object define in application software.
  * @param  sync: Fram sync PinName according to pinmux spec.
  * @param  clk: Clock PinName according to pinmux spec.
  * @param  out: Tx PinName according to pinmux spec.
  * @param  in: Rx PinName according to pinmux spec.
  * @retval none
  */
void pcm_init(pcm_t *obj, PinName sync, PinName clk, PinName out, PinName in);

/**
  * @brief  Set tx interrupt handler according to channel
  * @param  obj: PCM object define in application software.
  * @param  chan: Set the PCM channel, CH0 ~ CH15.
  * @param  handler: User defined IRQ callback function.
  * @retval none
  */
void pcm_tx_irq_handler(pcm_t *obj, pcm_ch chan, pcm_irq_handler handler);

/**
  * @brief  Set rx interrupt handler according to channel
  * @param  obj: PCM object define in application software.
  * @param  chan: Set the PCM channel, CH0 ~ CH15.
  * @param  handler: User defined IRQ callback function.
  * @retval none
  */
void pcm_rx_irq_handler(pcm_t *obj, pcm_ch chan, pcm_irq_handler handler);

/**
  * @brief  Set the PCM format and parameters.
  * @param  obj: PCM object define in application software.
  * @param  ms_mode: Set master or slave mode
  * @param  linear_mode: Set linear or compender mode. when choose the compender mode, need to set A-law or U-law.
  * @param  loop_back: Use lookback mode.
  * @param  endian_swap: Use endian swap.
  * @param  fs_inv: Set frame sync is the high or low active.
  * @retval none
  */
void pcm_set_format(pcm_t *obj, pcm_ms_mode ms_mode, pcm_mode linear_mode, pcm_loopback_mode loop_back, pcm_endian_swap endian_swap, pcm_frame_active fs_inv);

/**
  * @brief  Set the buffer address
  * @param  obj: PCM object define in application software.
  * @param  chan: Set the PCM channel, CH0 ~ CH15.
  * @param  tx_buf: Set the address of tx buffer. Need 4-byte aligned.
  * @param  rx_buf: Set the address of rx buffer. Need 4-byte aligned.
  * @retval none
  */
void pcm_set_dma_buffer(pcm_t *obj, pcm_ch chan, u16 *tx_buf, u16 *rx_buf);

/**
  * @brief  Control the tx and rx enable by the channel.
  * @param  obj: PCM object define in application software.
  * @param  chan: Set the PCM channel, CH0 ~ CH15.
  * @param  tx_en: To enable or disable.
  * @param  rx_en: To enable or disable.
  * @retval none
  */
void pcm_chan_trx_ctrl(pcm_t *obj, pcm_ch chan, BOOL tx_en, BOOL rx_en);

/**
  * @brief  Set the page size of the channel.
  * @param  obj: PCM object define in application software.
  * @param  chan: Set the PCM channel, CH0 ~ CH15.
  * @param  page_size: Need the even number 2~512, page units: 2 bytes
  * @retval none
  */
void pcm_chan_page_size(pcm_t *obj, pcm_ch chan, u32 page_size);

/**
  * @brief  Set the time slot of the channel. If configured as 16 bit linear mode, only even number time slot is allowed.
  * @param  obj: PCM object define in application software.
  * @param  chan: Set the PCM channel, CH0 ~ CH15.
  * @param  slot: Set the time slot, 0~31.
  * @retval none
  */
void pcm_chan_time_slot(pcm_t *obj, pcm_ch chan, pcm_slot slot);

/**
  * @brief  Set the time slot for the wideband. If configured as 16 bit linear mode, only even number time slot is allowed.
  * @param  obj: PCM object define in application software.
  * @param  chan: Set the PCM channel. The channel is only valid for CH0~CH7.
  * @param  wband_en: To enable or disable.
  * @param  wtsa: Set the time slot, SLOT0~SLOT31.
  * @retval none
  */
void pcm_chan_wideband(pcm_t *obj, pcm_ch chan, BOOL wband_en, pcm_slot wtsa);

/**
  * @brief  Set U-law or A-law in the compender mode.
  * @param  obj: PCM object define in application software.
  * @param  chan: Set the PCM channel, CH0 ~ CH15.
  * @param  ua_law: Set U-law or A-law.
  *            @arg U_LAW: U-law.
  *            @arg A_LAW: A-law.
  * @retval none
  */
void pcm_chan_compender_law(pcm_t *obj, pcm_ch chan, pcm_ua_law ua_law);

/**
  * @brief  Reset all channel setting and state machine.
  * @param  obj: PCM object define in application software.
  * @retval none
  */
void pcm_reset(pcm_t *obj);

/**
  * @brief  Clear interrupt status for all channels.
  * @param  obj: PCM object define in application software.
  * @retval none
  */
void pcm_clear_all_Intr(pcm_t *obj);

/**
  * @brief  Disable interrupt for all channels.
  * @param  obj: PCM object define in application software.
  * @retval none
  */
void pcm_disable_all_Intr(pcm_t *obj);

/**
  * @brief  Reset all logic and registers to initial state. Gate PCM Clock.
  * @param  obj: PCM object define in application software.
  * @retval none
  */
void pcm_disable(pcm_t *obj);

/**
  * @brief  Enable PCM and Clock
  * @param  obj: PCM object define in application software.
  * @retval none
  */
void pcm_enable(pcm_t *obj);

/**
  * @brief  Deinitializes the PCM device, include function/interrupt/PCM registers.
  * @param  obj: PCM object define in application software.
  * @retval none
  */
void pcm_deinit(pcm_t *obj);

/**
  * @brief  Start the transmission for all enabled channels.
  * @param  obj: PCM object define in application software.
  * @retval none
  */
void pcm_start_trx(pcm_t *obj);

/**
  * @brief  Start this tx channel. This channel needs to enable first by pcm_chan_trx_ctrl.
  * @param  obj: PCM object define in application software.
  * @param  chan: Set the PCM channel, CH0 ~ CH15.
  * @retval none
  */
void pcm_start_tx_chan(pcm_t *obj, pcm_ch chan);

/**
  * @brief  Start this rx channel. This channel needs to enable first by pcm_chan_trx_ctrl.
  * @param  obj: PCM object define in application software.
  * @param  chan: Set the PCM channel, CH0 ~ CH15.
  * @retval none
  */
void pcm_start_rx_chan(pcm_t *obj, pcm_ch chan);

/**
  * @brief  Stop the transmission for all channels.
  * @param  obj: PCM object define in application software.
  * @retval none
  */
void pcm_stop_trx_all_chan(pcm_t *obj);

/**
  * @brief  Stop the tx transmission for all channels.
  * @param  obj: PCM object define in application software.
  * @retval none
  */
void pcm_stop_tx_all_chan(pcm_t *obj);

/**
  * @brief  Stop the rx transmission for all channels.
  * @param  obj: PCM object define in application software.
  * @retval none
  */
void pcm_stop_rx_all_chan(pcm_t *obj);

/**
  * @brief  Stop the tx transmission for this channel.
  * @param  obj: PCM object define in application software.
  * @param  chan: Set the PCM channel, CH0 ~ CH15.
  * @retval none
  */
void pcm_stop_tx_chan(pcm_t *obj, pcm_ch chan);

/**
  * @brief  Stop the rx transmission for this channel.
  * @param  obj: PCM object define in application software.
  * @param  chan: Set the PCM channel, CH0 ~ CH15.
  * @retval none
  */
void pcm_stop_rx_chan(pcm_t *obj, pcm_ch chan);

/**
  * @brief  Get the next address to be processed.
  *          This function needs in the interrupt handler.
  * @param  obj: PCM object define in application software.
  * @retval none
  */
u8 *pcm_irq_get_tx_next_page_adr(pcm_t *obj);

/**
  * @brief  Inform PCM the tx page data is ready.
  *          This function needs in the interrupt handler.
  * @param  obj: PCM object define in application software.
  * @retval none
  */
void pcm_irq_set_txpage(pcm_t *obj);

/**
  * @brief  Inform PCM that finish receiving the rx page data.
  *          This function needs in the interrupt handler.
  * @param  obj: PCM object define in application software.
  * @retval none
  */
void pcm_irq_set_rxpage(pcm_t *obj);

/**
  * @brief  Get the processing channel number in the interrupt handler.
  *          This function needs in the interrupt handler.
  * @param  obj: PCM object define in application software.
  * @retval none
  */
u8 pcm_irq_get_channel(pcm_t *obj);

/**
  * @brief  Get the buffer size of the processing channel in the interrupt handler.
  *          This function needs in the interrupt handler.
  * @param  obj: PCM object define in application software.
  * @retval none
  */
u32 pcm_irq_get_buffer_size(pcm_t *obj);

/**
  * @brief  Get the the page number of the processing channel in the interrupt handler.
  *          This function needs in the interrupt handler.
  * @param  obj: PCM object define in application software.
  * @retval none
  */
u8 pcm_irq_get_page_0_or_1(pcm_t *obj);

/**
  * @brief  Get the next address of the channel to be processed.
  * @param  obj: PCM object define in application software.
  * @param  chan: Set the PCM channel, CH0 ~ CH15.
  * @retval none
  */
u8 *pcm_get_tx_next_page_adr(pcm_t *obj, pcm_ch chan);

/**
  * @brief  Inform PCM the tx page data of the channel is ready.
  * @param  obj: PCM object define in application software.
  * @param  chan: Set the PCM channel, CH0 ~ CH15.
  * @retval none
  */
void pcm_set_txpage(pcm_t *obj, pcm_ch chan);

/**
  * @brief  Inform PCM that finish receiving the rx page data of the channel.
  * @param  obj: PCM object define in application software.
  * @param  chan: Set the PCM channel, CH0 ~ CH15.
  * @retval none
  */
void pcm_set_rxpage(pcm_t *obj, pcm_ch chan);

/**
  * @brief  Get the buffer size of the channel.
  * @param  obj: PCM object define in application software.
  * @param  chan: Set the PCM channel, CH0 ~ CH15.
  * @retval none
  */
u32 pcm_get_buffer_size(pcm_t *obj, pcm_ch chan);

/**
  * @brief  Get the address of the tx page0 data.
  * @param  obj: PCM object define in application software.
  * @param  chan: Set the PCM channel, CH0 ~ CH15.
  * @retval none
  */
u32 pcm_get_tx_page0_adr(pcm_t *obj, pcm_ch chan);

/**
  * @brief  Get the address of the tx page1 data.
  * @param  obj: PCM object define in application software.
  * @param  chan: Set the PCM channel, CH0 ~ CH15.
  * @retval none
  */
u32 pcm_get_tx_page1_adr(pcm_t *obj, pcm_ch chan);

/**
  * @brief  Get the address of the rx page0 data.
  * @param  obj: PCM object define in application software.
  * @param  chan: Set the PCM channel, CH0 ~ CH15.
  * @retval none
  */
u32 pcm_get_rx_page0_adr(pcm_t *obj, pcm_ch chan);

/**
  * @brief  Get the address of the rx page1 data.
  * @param  obj: PCM object define in application software.
  * @param  chan: Set the PCM channel, CH0 ~ CH15.
  * @retval none
  */
u32 pcm_get_rx_page1_adr(pcm_t *obj, pcm_ch chan);


///@}



#ifdef __cplusplus
}
#endif

#endif
