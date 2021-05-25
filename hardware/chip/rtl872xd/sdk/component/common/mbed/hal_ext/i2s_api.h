/** mbed Microcontroller Library
  ******************************************************************************
  * @file    i2s_api.h
  * @author 
  * @version V1.0.0
  * @brief   This file provides following mbed I2S API
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2015, Realtek Semiconductor Corp.
  * All rights reserved.
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  ****************************************************************************** 
  */

#ifndef MBED_EXT_I2S_API_EXT_H
#define MBED_EXT_I2S_API_EXT_H


#include "device.h"

#if !(defined(CONFIG_PLATFORM_8195BHP) && (CONFIG_PLATFORM_8195BHP == 1))  &&  !(defined(CONFIG_PLATFORM_8195BLP) && (CONFIG_PLATFORM_8195BLP == 1))
///@name Only Ameba1 
#include "ameba_soc.h"
#endif  // end of "#if !defined(CONFIG_PLATFORM_8195BHP) && !defined(CONFIG_PLATFORM_8195BLP)"



#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup i2s I2S
 *  @ingroup    hal
 *  @brief      i2s functions
 *  @{
 */

#if (defined(CONFIG_PLATFORM_8195BHP) && (CONFIG_PLATFORM_8195BHP == 1))  ||  (defined(CONFIG_PLATFORM_8195BLP) && (CONFIG_PLATFORM_8195BLP == 1))  ||  (defined(CONFIG_PLATFORM_8721D) && (CONFIG_PLATFORM_8721D == 1))
///@name AmebaPro and AmebaD 
///@{
enum {
	SR_8KHZ = I2S_SR_8KHZ,
	SR_12KHZ = I2S_SR_12KHZ,
	SR_16KHZ = I2S_SR_16KHZ,
	SR_24KHZ = I2S_SR_24KHZ,
	SR_32KHZ = I2S_SR_32KHZ,
	SR_48KHZ = I2S_SR_48KHZ,
	SR_64KHZ = I2S_SR_64KHZ,
	SR_96KHZ = I2S_SR_96KHZ,
	SR_192KHZ = I2S_SR_192KHZ,
	SR_384KHZ = I2S_SR_384KHZ,
	SR_7p35KHZ = I2S_SR_7p35KHZ,
	SR_11p025KHZ = I2S_SR_11p025KHZ,
	SR_14p7KHZ = I2S_SR_14p7KHZ,
	SR_22p05KHZ = I2S_SR_22p05KHZ,
	SR_29p4KHZ = I2S_SR_29p4KHZ,
	SR_44p1KHZ = I2S_SR_44p1KHZ,
	SR_58p8KHZ = I2S_SR_58p8KHZ,
	SR_88p2KHZ = I2S_SR_88p2KHZ,
	SR_176p4KHZ = I2S_SR_176p4KHZ
};

enum {
	CH_STEREO = I2S_CH_STEREO,
	CH_5P1 = I2S_CH_5P1,
	CH_MONO = I2S_CH_MONO
};

enum {
	WL_16b = I2S_WL_16,
	WL_24b = I2S_WL_24,
	WL_32b = I2S_WL_32
};

#else

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
///@}
#endif  // end of "#if defined(CONFIG_PLATFORM_8195BHP) || defined(CONFIG_PLATFORM_8195BLP)"
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

#if defined(CONFIG_PLATFORM_8195A) && (CONFIG_PLATFORM_8195A == 1)
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

#if (defined(CONFIG_PLATFORM_8711B) && (CONFIG_PLATFORM_8711B == 1)) || (defined(CONFIG_PLATFORM_8721D) && (CONFIG_PLATFORM_8721D == 1))
///@name AmebaZ and AmebaD 
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
#if (defined(CONFIG_PLATFORM_8195BHP) && (CONFIG_PLATFORM_8195BHP == 1))  ||  (defined(CONFIG_PLATFORM_8195BLP) && (CONFIG_PLATFORM_8195BLP == 1))
///@name AmebaPro Only 
///@{
typedef enum {
    FORMAT_I2S = I2S_FORMAT_I2S,
    LEFT_JUST  = I2S_FORMAT_LEFT_JUST,
    FORMAT_RIGHT_JUST = I2S_FORMAT_RIGHT_JUST
} i2s_format;

typedef enum {
    I2S_MASTER = I2S_MASTER_MODE,
    I2S_SLAVE = I2S_SLAVE_MODE
} i2s_ms_mode;

typedef enum {
    PAGE_0NUM = I2S_USELESS_PAGE,    
    PAGE_2NUM = I2S_2PAGE,
    PAGE_3NUM = I2S_3PAGE,
    PAGE_4NUM = I2S_4PAGE
} i2s_page_num;

typedef enum {
    BURST8 = I2S_BURST8,
    BURST12 = I2S_BURST12,
    BURST16 = I2S_BURST16
} i2s_burst_size;

typedef enum {
    LEFT_PHASE = I2S_LEFT_PHASE,
    RIGHT_PHASE = I2S_RIGHT_PHASE
} i2s_ws_swap;

typedef enum {
    NEGATIVE_EDGE = I2S_NEGATIVE_EDGE,
    POSITIVE_EDGE = I2S_POSITIVE_EDGE
} i2s_edge_sw;

/**
  * @brief  Initializes the I2S device, include clock/function/interrupt/I2S registers.
  * @param  obj: i2s object define in application software.
  * @param  sck: Serial clock PinName according to pinmux spec.
  * @param  ws: Word select PinName according to pinmux spec.
  * @param  sd_tx0: Tx PinName according to pinmux spec.
  * @param  sd_rx: Rx PinName according to pinmux spec. This pin is not configured when set PinName "NC".
  * @param  mck: Master clock PinName according to pinmux spec. This pin is not configured when set PinName "NC".
  * @param  sd_tx1: Tx1 PinName according to pinmux spec. This pin is not configured when set PinName "NC".
  * @param  sd_tx2: Tx2 PinName according to pinmux spec. This pin is not configured when set PinName "NC".
  * @retval none
  */
void i2s_init(i2s_t *obj, PinName sck, PinName ws, PinName sd_tx0, PinName sd_rx, PinName mck, PinName sd_tx1, PinName sd_tx2);

/**
  * @brief  Reset the I2S.
  * @param  obj: i2s object define in application software.
  * @retval none
  */
void i2s_sw_reset(i2s_t *obj);

/**
  * @brief  Set the I2S format.
  * @param  obj: i2s object define in application software.
  * @param  format: The I2S formate.
  *		@FORMAT_I2S : I2S
  *		@LEFT_JUST : Left Justified
  *		@FORMAT_RIGHT_JUST : Right Justified
  * @retval none
  */
void i2s_set_format(i2s_t *obj, i2s_format format);

/**
  * @brief  Set the master or slave mode.
  * @param  obj: i2s object define in application software.
  * @param  ms_mode: Master or slave mode.
  *		@I2S_MASTER : Master mode.
  *		@I2S_SLAVE : Left Justified
  *		@FORMAT_RIGHT_JUST : Right Justified
  * @retval none
  */
void i2s_set_master(i2s_t *obj, i2s_ms_mode ms_mode);

/**
  * @brief  Set the burst size for DMA.
  * @param  obj: i2s object define in application software.
  * @param  burst_size: Set the burst size.
  *		@BURST8 : The burst size is 8-words.
  *		@BURST12 : The burst size is 12-words.
  *		@BURST16 : The burst size is 16-words.
  * @retval none
  */
void i2s_set_dma_burst_size(i2s_t *obj, i2s_burst_size burst_size);

/**
  * @brief  Make the data do the byte switch.
  * @param  obj: i2s object define in application software.
  * @param  byte_swap_en: To enable or disable.
  * @retval none
  */
void i2s_set_byte_swap(i2s_t *obj, BOOL byte_swap_en);

/**
  * @brief  Make the SCK inverse.
  * @param  obj: i2s object define in application software.
  * @param  sck_inv_en: To enable or disable.
  * @retval none
  */
void i2s_set_sck_inv(i2s_t *obj, BOOL sck_inv_en);

/**
  * @brief  Control whether the first data appear in the "right" or "left" phase of WS clock.
  * @param  obj: i2s object define in application software.
  * @param  ws_swap: Select the right or left phase.
  *		@LEFT_PHASE : Select the left phase.
  *		@RIGHT_PHASE : Select the right phase.
  * @retval none
  */
void i2s_set_ws_swap(i2s_t *obj, i2s_ws_swap ws_swap);

/**
  * @brief  Set the I2S loopback mode. Need to set the I2S direction is only TX.
  * @param  obj: i2s object define in application software.
  * @param  loopback_en: To enable or disable.
  * @retval none
  */
void i2s_set_loopback(i2s_t *obj, BOOL loopback_en);

/**
  * @brief  Chose to send data at the negative edge or positive edge of SCK.
  * @param  obj: i2s object define in application software.
  * @param  edge_sw: Select the sending edge.
  *		@NEGATIVE_EDGE : Select the negative edge.
  *		@POSITIVE_EDGE : Select the positive edge.
  * @retval none
  */
void i2s_set_data_start_edge(i2s_t *obj, i2s_edge_sw edge_sw);

/**
  * @brief  Set the I2S mute mode.
  * @param  obj: i2s object define in application software.
  * @param  mute_en: To enable or disable.
  * @retval none
  */
void i2s_set_mute(i2s_t *obj, BOOL mute_en);


///@}
#endif  // end of "#if defined(CONFIG_PLATFORM_8195BHP) || defined(CONFIG_PLATFORM_8195BLP)"

/*\@}*/

#ifdef __cplusplus
}
#endif

#endif
