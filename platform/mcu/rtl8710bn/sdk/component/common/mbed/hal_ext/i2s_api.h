/** mbed Microcontroller Library
  ******************************************************************************
  * @file    i2s_api.h
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

#ifndef MBED_EXT_I2S_API_EXT_H
#define MBED_EXT_I2S_API_EXT_H


#include "device.h"
#include "ameba_soc.h"


#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup i2s I2S
 *  @ingroup    hal
 *  @brief      i2s functions
 *  @{
 */

///@name Ameba Common
///@{
enum {
	SR_8KHZ = I2S_SR_8KHZ, 
	SR_16KHZ = I2S_SR_16KHZ,
	SR_24KHZ = I2S_SR_24KHZ,
	SR_32KHZ = I2S_SR_32KHZ,
	SR_48KHZ = I2S_SR_48KHZ,
	SR_96KHZ = I2S_SR_96KHZ,
	SR_7p35KHZ = I2S_SR_7p35KHZ,
	SR_14p7KHZ = I2S_SR_14p7KHZ,
	SR_22p05KHZ = I2S_SR_22p05KHZ,
	SR_29p4KHZ = I2S_SR_29p4KHZ,
	SR_44p1KHZ = I2S_SR_44p1KHZ,
	SR_88p2KHZ = I2S_SR_88p2KHZ
};

enum {
	CH_STEREO = I2S_CH_STEREO,
	CH_MONO = I2S_CH_MONO
};

enum {
	WL_16b = I2S_WL_16,
	WL_24b = I2S_WL_24
};

enum {
    I2S_DIR_RX   = I2S_ONLY_RX,    // Rx Only
    I2S_DIR_TX   = I2S_ONLY_TX,    // Tx Only
    I2S_DIR_TXRX = I2S_TXRX     // Tx & Rx (BiDirection)
};

typedef void (*i2s_irq_handler)(uint32_t id, char *pbuf);

typedef struct i2s_s i2s_t;

/**
  * @brief  Deinitializes the I2S device, include function/interrupt/I2S registers.
  * @param  obj: i2s object define in application software.
  * @retval none
  */
void i2s_deinit(i2s_t *obj);

/**
  * @brief  Sets page number, page size, page address.
  * @param  obj: i2s object define in application software.
  * @param  tx_buf: pointer to the start address of Tx page.
  * @param  rx_buf: pointer to the start address of Rx page.
  * @param  page_num: page number. This parameter must be set to a value in the 2~4 range
  * @param  page_size: page size. This parameter must be set to a value in the 4~16384 bytes range
  * @retval none
  */
void i2s_set_dma_buffer(i2s_t *obj, char *tx_buf, char *rx_buf, uint32_t page_num, uint32_t page_size);

/**
  * @brief  Sets TX interrupt handler.
  * @param  obj: i2s object define in application software.
  * @param  handler: TX interrupt callback function.
  * @param  id: TX interrupt callback function parameter.
  * @retval none
  */
void i2s_tx_irq_handler(i2s_t *obj, i2s_irq_handler handler, uint32_t id);

/**
  * @brief  Sets RX interrupt handler.
  * @param  obj: i2s object define in application software.
  * @param  handler: RX interrupt callback function.
  * @param  id: RX interrupt callback function parameter.
  * @retval none
  */
void i2s_rx_irq_handler(i2s_t *obj, i2s_irq_handler handler, uint32_t id);

/**
  * @brief  Sets i2s data transfer direction.
  * @param  obj: i2s object define in application software.
  * @param  trx_type: transfer direction.
  *          This parameter can be one of the following values:
  *            @arg I2S_DIR_RX: Rx receive direction
  *            @arg I2S_DIR_TX: Tx transmission direction
  *            @arg I2S_DIR_TXRX: Tx & Rx bi-direction
  * @retval none
  */
void i2s_set_direction(i2s_t *obj, int trx_type);

/**
  * @brief  Sets i2s channel number, sample rate, word length.
  * @param  obj: i2s object define in application software.
  * @param  channel_num: this parameter can be one of the following values:
  *            @arg CH_STEREO: stereo channel
  *            @arg CH_MONO: mono channel
  * @param  rate: this parameter can be one of the following values:
  *            @arg SR_8KHZ: sample rate is 8kHz
  *            @arg SR_16KHZ: sample rate is 16kHz
  *            @arg SR_24KHZ: sample rate is 24kHz
  *            @arg SR_32KHZ: sample rate is 32kHz
  *            @arg SR_48KHZ: sample rate is 48kHz
  *            @arg SR_96KHZ: sample rate is 96kHz
  *            @arg SR_7p35KHZ: sample rate is 7.35kHz
  *            @arg SR_14p7KHZ: sample rate is 14.7kHz
  *            @arg SR_22p05KHZ: sample rate is 22.05kHz
  *            @arg SR_29p4KHZ: sample rate is 29.4kHz
  *            @arg SR_44p1KHZ: sample rate is 44.1kHz
  *            @arg SR_88p2KHZ: sample rate is 88.2kHz
  * @param  word_len: this parameter can be one of the following values:
  *            @arg WL_16b: sample bit is 16 bit
  *            @arg WL_24b: sample bit is 24 bit
  * @retval none
  */
void i2s_set_param(i2s_t *obj, int channel_num, int rate, int word_len);

/**
  * @brief  Gets current tx page address.
  * @param  obj: i2s object define in application software.
  * @retval : address of current tx page or NULL
  * @note current page own by cpu, return address of current tx page
  * @note current page own by i2s, return NULL
  */
int* i2s_get_tx_page(i2s_t *obj);

/**
  * @brief  Sets current tx page own by i2s.
  * @param  obj: i2s object define in application software.
  * @param  pbuf: tx buffer adderss.
  * @retval none
  */
void i2s_send_page(i2s_t *obj, uint32_t *pbuf);

/**
  * @brief  Sets current rx page own by i2s.
  * @param  obj: i2s object define in application software.
  * @retval none
  */
void i2s_recv_page(i2s_t *obj);

/**
  * @brief  Enable i2s interrupt and function.
  * @param  obj: i2s object define in application software.
  * @retval none
  */
void i2s_enable(i2s_t *obj);

/**
  * @brief  Disable i2s interrupt and function.
  * @param  obj: i2s object define in application software.
  * @retval none
  */
void i2s_disable(i2s_t *obj);

///@}

#if CONFIG_PLATFORM_8195A
///@name Ameba1 Only 
///@{
/**
  * @brief  Initializes the I2S device, include clock/function/interrupt/I2S registers.
  * @param  obj: i2s object define in application software.
  * @param  sck: Serial clock PinName according to pinmux spec.
  * @param  ws: Word select PinName according to pinmux spec.
  * @param  sd: PinName according to pinmux spec.
  * @retval none
  */
void i2s_init(i2s_t *obj, PinName sck, PinName ws, PinName sd);

///@}
#endif //CONFIG_PLATFORM_8195A

#if CONFIG_PLATFORM_8711B
///@name AmebaZ Only 
///@{
/**
  * @brief  Initializes the I2S device, include clock/function/interrupt/I2S registers.
  * @param  obj: i2s object define in application software.
  * @param  sck: Serial clock PinName according to pinmux spec.
  * @param  ws: Word select PinName according to pinmux spec.
  * @param  sd_tx: Tx PinName according to pinmux spec.
  * @param  sd_rx: Rx PinName according to pinmux spec.
  * @param  mck: Master clock PinName according to pinmux spec.
  * @retval none
  */
void i2s_init(i2s_t *obj, PinName sck, PinName ws, PinName sd_tx, PinName sd_rx, PinName mck);
///@}
#endif //CONFIG_PLATFORM_8711B


/*\@}*/

#ifdef __cplusplus
}
#endif

#endif
