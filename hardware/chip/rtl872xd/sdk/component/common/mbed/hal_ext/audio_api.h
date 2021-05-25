/** mbed Microcontroller Library
  ******************************************************************************
  * @file    audio_api.h
  * @author
  * @version V1.0.0
  * @brief
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

#ifndef MBED_EXT_AUDIO_API_EXT_H
#define MBED_EXT_AUDIO_API_EXT_H
#if defined(CONFIG_PLATFORM_8195BHP) && (CONFIG_PLATFORM_8195BHP == 1)
///@name AmebaPro Only 
///@{
#include "device.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup audio AUDIO
 *  @ingroup    hal
 *  @brief      audio functions
 *  @{
 */

typedef enum {
	WL_16BIT = AUDIO_WL_16,
    WL_24BIT = AUDIO_WL_24,
    WL_8BIT = AUDIO_WL_8
} audio_wl;

typedef enum {
    ASR_8KHZ = AUDIO_SR_8KHZ,
    ASR_16KHZ = AUDIO_SR_16KHZ,
	ASR_32KHZ = AUDIO_SR_32KHZ,
    ASR_44p1KHZ = AUDIO_SR_44p1KHZ,
	ASR_48KHZ = AUDIO_SR_48KHZ,
    ASR_88p2KHZ = AUDIO_SR_88p2KHZ,
    ASR_96KHZ = AUDIO_SR_96KHZ
} audio_sr;

typedef enum {
    OUTPUT_DISABLE = 0,
    OUTPUT_SINGLE_EDNED = 1,
    OUTPUT_DIFFERENTIAL = 2,
	OUTPUT_CAPLESS = 3
} audio_output_mode;

typedef enum {
    INPUT_DISABLE = 0,
    LINE_IN_MODE = 1,
    MIC_DIFFERENTIAL = 2,
    MIC_SINGLE_EDNED = 3
} audio_input_mode;

typedef enum {
	BIAS_0p9_AVDD = AUDIO_BIAS_0p9_AVDD,
    BIAS_0p86_AVDD = AUDIO_BIAS_0p86_AVDD,
    BIAS_0p75_AVDD = AUDIO_BIAS_0p75_AVDD
} audio_bias_voltage;

typedef enum {
	MIC_0DB = AUDIO_MIC_0DB,
    MIC_20DB = AUDIO_MIC_20DB,
    MIC_30DB = AUDIO_MIC_30DB,
    MIC_40DB = AUDIO_MIC_40DB
} audio_mic_gain;

typedef enum {
	VREF_0p52VDD = AUDIO_VREF_0p52_VDD,
    VREF_0p51VDD = AUDIO_VREF_0p51_VDD,
    VREF_0p50VDD = AUDIO_VREF_0p50_VDD,
    VREF_0p49VDD = AUDIO_VREF_0p49_VDD
} audio_vref_voltage;

typedef enum {
	SIDETONE_120HZ = AUDIO_ST_120HZ,
    SIDETONE_239HZ = AUDIO_ST_239HZ,
    SIDETONE_358HZ = AUDIO_ST_358HZ,
    SIDETONE_477HZ = AUDIO_ST_477HZ,
	SIDETONE_597HZ = AUDIO_ST_597HZ,
    SIDETONE_716HZ = AUDIO_ST_716HZ,
    SIDETONE_835HZ = AUDIO_ST_835HZ,
    SIDETONE_955HZ = AUDIO_ST_955HZ
} audio_sidetone_hpf;

typedef enum {
	DVOL_ADC_0DB = 0x2F,
    DVOL_DAC_0DB = 0xAF,
    SIDETONE_0DB = 0x1F
} audio_dvol_0db;

typedef enum {
    AUDIO_CODEC_1p8V = AUDIO_POWER_1p8V,
    AUDIO_CODEC_2p8V = AUDIO_POWER_2p8V
} audio_power_sel;

typedef void (*audio_irq_handler)(u32 arg, u8 *pbuf);

typedef struct audio_s audio_t;

/**
  * @brief  Initializes the AUDIO device, include clock/function/interrupt/AUDIO registers.
  * @param  obj: Audio object define in application software.
  * @param  output_mode: Select the output mode.
  *            @arg OUTPUT_DISABLE: Disable audio output.
  *            @arg OUTPUT_SINGLE_EDNED: Single-Ended mode.
  *            @arg OUTPUT_DIFFERENTIAL: Differential mode.  
  *            @arg OUTPUT_CAPLESS: Capless mode.   
  * @param  input_mode: Select the input mode.
  *            @arg INPUT_DISABLE: Disable audio input.
  *            @arg LINE_IN_MODE: Line in mode.
  *            @arg MIC_DIFFERENTIAL: MIC differential mode.  
  *            @arg MIC_SINGLE_EDNED: MIC Single-Ended mode.      
  * @param  power_sel: Select audio codec LDO power.
  *            @arg AUDIO_CODEC_1p8V: Audio codec power 1.8V.
  *            @arg AUDIO_CODEC_2p8V: Audio codec power 2.8V.  
  * @retval none
  */
void audio_init(audio_t *obj, audio_output_mode output_mode, audio_input_mode input_mode, audio_power_sel power_sel);

/**
  * @brief  Deinitializes the AUDIO device, include function/interrupt/AUDIO registers.
  * @param  obj: Audio object define in application software.
  * @retval none
  */
void audio_deinit(audio_t *obj);

/**
  * @brief  Set the audio sample rate and word length.
  * @param  obj: Audio object define in application software.
  * @param  sample_rate: this parameter can be one of the following values:
  *            @arg ASR_8KHZ: sample rate is 8kHz
  *            @arg ASR_16KHZ: sample rate is 16kHz
  *            @arg ASR_32KHZ: sample rate is 32kHz
  *            @arg ASR_44p1KHZ: sample rate is 44.1kHz
  *            @arg ASR_48KHZ: sample rate is 48kHz
  *            @arg ASR_88p2KHZ: sample rate is 88.2kHz
  *            @arg ASR_96KHZ: sample rate is 96kHz
  * @param  word_length: this parameter can be one of the following values:
  *            @arg WL_8BIT: sample bit is 8 bit
  *            @arg WL_16BIT: sample bit is 16 bit
  *            @arg WL_24BIT: sample bit is 24 bit (World length is 24bits but ADC and DAC are 16bits)
  * @retval none
  */
void audio_set_param(audio_t *obj, audio_sr sample_rate, audio_wl word_length);

/**
  * @brief  Set the tx buffer address.
  * @param  obj: Audio object define in application software.
  * @param  tx_buf: Set the address of tx buffer. Need 4-byte aligned.
  * @param  tx_page_size: This unit is byte. This number needs to be a multiple of 64, and the maximum value is 4032.
  * @retval none
  */
void audio_set_tx_dma_buffer(audio_t *obj, u8 *tx_buf, u32 tx_page_size);

/**
  * @brief  Set the rx buffer address.
  * @param  obj: Audio object define in application software.
  * @param  rx_buf: Set the address of tx buffer. Need 4-byte aligned.
  * @param  rx_page_size: This unit is byte. This number needs to be a multiple of 64, and the maximum value is 4032.
  * @retval none
  */
void audio_set_rx_dma_buffer(audio_t *obj, u8 *rx_buf, u32 rx_page_size);

/**
  * @brief  Set tx interrupt handler.
  * @param  obj: Audio object define in application software.
  * @param  tx_handler: User defined IRQ callback function.
  * @param  arg:  User defined IRQ callback parameter.
  * @retval none
  */
void audio_tx_irq_handler(audio_t *obj, audio_irq_handler tx_handler, u32 arg);

/**
  * @brief  Set rx interrupt handler.
  * @param  obj: Audio object define in application software.
  * @param  rx_handler: User defined IRQ callback function.
  * @param  arg:  User defined IRQ callback parameter.
  * @retval none
  */
void audio_rx_irq_handler(audio_t *obj, audio_irq_handler rx_handler, u32 arg);

/**
  * @brief  Get the current tx page address.
  * @param  obj: Audio object define in application software.
  * @retval Tx page address
  */
u8 *audio_get_tx_page_adr(audio_t *obj);

/**
  * @brief  Inform Audio the tx page data of the channel is ready.
  * @param  obj: Audio object define in application software.
  * @param  pbuf: tx buffer adderss.
  * @retval none
  */
void audio_set_tx_page(audio_t *obj, u8 *pbuf);

/**
  * @brief  Inform Audio that finish receiving the rx page data of the channel.
  * @param  obj: Audio object define in application software.
  * @retval none
  */
void audio_set_rx_page(audio_t *obj);

/**
  * @brief  Start the tx transmission.
  * @param  obj: Audio object define in application software.
  * @retval none
  */
void audio_tx_start (audio_t *obj);

/**
  * @brief  Start the rx transmission.
  * @param  obj: Audio object define in application software.
  * @retval none
  */
void audio_rx_start (audio_t *obj);

/**
  * @brief  Start the tx and rx transmission.
  * @param  obj: Audio object define in application software.
  * @retval none
  */
void audio_trx_start (audio_t *obj);

/**
  * @brief  Stop the tx transmission.
  * @param  obj: Audio object define in application software.
  * @retval none
  */
void audio_tx_stop (audio_t *obj);

/**
  * @brief  Stop the rx transmission.
  * @param  obj: Audio object define in application software.
  * @retval none
  */
void audio_rx_stop (audio_t *obj);

/**
  * @brief  Stop the tx and rx transmission.
  * @param  obj: Audio object define in application software.
  * @retval none
  */
void audio_trx_stop (audio_t *obj);

/**
  * @brief  Get the error count of tx transmission when happen the interrupt.
  * @param  obj: Audio object define in application software.
  * @retval Tx error count
  */
u8 audio_get_tx_error_cnt (audio_t *obj);

/**
  * @brief  Get the error count of rx transmission when happen the interrupt.
  * @param  obj: Audio object define in application software.
  * @retval Rx error count
  */
u8 audio_get_rx_error_cnt (audio_t *obj);

/**
  * @brief  clean the error count.
  * @param  obj: Audio object define in application software.
  * @retval none
  */
void audio_clean_error_cnt (audio_t *obj);

/**
  * @brief  Make the data from tx to rx and bypass the audio codec.
  * @param  obj: Audio object define in application software.
  * @retval none
  */
void audio_sport_loopback (audio_t *obj, BOOL en);

/**
  * @brief  Control the mute of the microphone.
  * @param  obj: Audio object define in application software.
  * @param  en: To enable or disable mute.
  * @retval none
  */
void audio_mic_analog_mute (audio_t *obj, BOOL en);

/**
  * @brief  Control the boost gain of the microphone.
  * @param  obj: Audio object define in application software.
  * @param  en: To enable or disable.
  * @param  mic_gain: Select the microphone gain.
  *            @arg MIC_0DB: 0dB.
  *            @arg MIC_20DB: 20dB.
  *            @arg MIC_30DB: 30dB.
  *            @arg MIC_40DB: 40dB.
  * @retval none
  */
void audio_mic_analog_gain (audio_t *obj, BOOL en, audio_mic_gain mic_gain);

/**
  * @brief  Control the mute of the line-in.
  * @param  obj: Audio object define in application software.
  * @param  en: To enable or disable mute.
  * @retval none
  */
void audio_line_in_analog_mute (audio_t *obj, BOOL en);

/**
  * @brief  Control the digital gain of ADC.
  * @param  obj: Audio object define in application software.
  * @param  step: The digital volume. Every Step is 0.375dB.
  *            @arg 0x7F: 30dB.
  *            @arg ...
  *            @arg 0x2F: 0dB.
  *            @arg ...
  *            @arg 0x00: -17.625dB.
  * @retval none
  */
void audio_adc_digital_vol (audio_t *obj, u8 step);

/**
  * @brief  Control the digital mute of ADC.
  * @param  obj: Audio object define in application software.
  * @param  mute_en: To enable or disable mute.
  * @retval none
  */
void audio_adc_digital_mute (audio_t *obj, BOOL mute_en);

/**
  * @brief  Control the mute of the headphone.
  * @param  obj: Audio object define in application software.
  * @param  en: To enable or disable mute.
  * @retval none
  */
void audio_headphone_analog_mute (audio_t *obj, BOOL en);

/**
  * @brief  Control the digital gain of DAC.
  * @param  obj: Audio object define in application software.
  * @param  step: The digital volume. Every Step is 0.375dB.
  *            @arg 0xAF: 0dB.
  *            @arg 0xAE: -0.375dB.
  *            @arg ...
  *            @arg 0x00: -65.625dB.
  * @retval none
  */
void audio_dac_digital_vol (audio_t *obj, u8 step);

/**
  * @brief  Control the digital mute of DAC.
  * @param  obj: Audio object define in application software.
  * @param  mute_en: To enable or disable mute.
  * @retval none
  */
void audio_dac_digital_mute (audio_t *obj, BOOL mute_en);

/**
  * @brief  Control the VREF voltage.
  * @param  obj: Audio object define in application software.
  * @param  voltage: Select the VREF voltage.
  *            @arg VREF_0p52VDD: 0.52*VDD.
  *            @arg VREF_0p51VDD: 0.51*VDD.
  *            @arg VREF_0p50VDD: 0.50*VDD.
  *            @arg VREF_0p49VDD: 0.49*VDD.
  * @retval none
  */
void audio_vref_voltage_ctrl (audio_t *obj, audio_vref_voltage voltage);

/**
  * @brief  Initializes the sidetone function. Allows to hear mic voice in the headset.
  * @param  obj: Audio object define in application software.
  * @retval none
  */
void audio_sidetone_init (audio_t *obj);

/**
  * @brief  Deinitializes the sidetone function.
  * @param  obj: Audio object define in application software.
  * @retval none
  */
void audio_sidetone_deinit (audio_t *obj);

/**
  * @brief  Control the digital gain and boost of sidetone.
  * @param  obj: Audio object define in application software.
  * @param  sidetone_boost: Audio object define in application software.
  *            @arg 0x00: 0dB.
  *            @arg 0x01: 12dB.
  * @param  step: The digital volume. Every Step is 1.5dB.
  *            @arg 0x1F: 0dB.
  *            @arg 0x1E: -1.5dB.
  *            @arg ...
  *            @arg 0x00: -46.5dB.
  * @retval none
  */
void audio_sidetone_vol (audio_t *obj, audio_sidetone_boost_t sidetone_boost, u8 step);

/**
  * @brief  Control the high pass filter of sidetone.
  * @param  obj: Audio object define in application software.
  * @param  en: 0 is disable, 1 is enable.
  * @param  sidetone_hpf: Select the cut-off frequency.
  *            @arg SIDETONE_120HZ: 120Hz.
  *            @arg SIDETONE_239HZ: 239Hz.
  *            @arg SIDETONE_358HZ: 358Hz.
  *            @arg SIDETONE_477HZ: 477Hz.
  *            @arg SIDETONE_597HZ: 597Hz.
  *            @arg SIDETONE_716HZ: 716Hz.
  *            @arg SIDETONE_835HZ: 835Hz.
  *            @arg SIDETONE_955HZ: 955Hz.
  * @retval none
  */
void audio_sidetone_hpf_ctrl (audio_t *obj, BOOL en, audio_sidetone_hpf sidetone_hpf);

/**
 * @brief  The wind filter is implemented by a high pass filter.Remove wind noise at application mode.
 *            The cut-off frequency of wind filter is programmable and is varied according to different sample rate.
 * @param  obj: Audio object define in application software.
 * @param  en  Enable control: 0 is disable, 1 is enable.
 * @param  sample_rate: this parameter can be one of the following values:
 *            @arg ASR_8KHZ: sample rate is 8kHz
 *            @arg ASR_16KHZ: sample rate is 16kHz
 *            @arg ASR_32KHZ: sample rate is 32kHz
 *            @arg ASR_44p1KHZ: sample rate is 44.1kHz
 *            @arg ASR_48KHZ: sample rate is 48kHz
 *            @arg ASR_88p2KHZ: sample rate is 88.2kHz
 *            @arg ASR_96KHZ: sample rate is 96kHz
 * @param  coef_num  Set the wind filter coefficient " n ".
 *            For the formula of Fc calculation is also shown as: Fc = (Fs * tan-1(a/(2-a))) / pi.
 *            Sample rate Fs = 8K/12K/16K, a = 2 ^(-6) (1 + n).
 *            Sample rate Fs = 24K/32K, a = 2 ^(-7) (1 + n).
 *            Sample rate Fs = 44.1K/48K, a = 2 ^(-8) (1 + n).
 *            Sample rate Fs = 88.2K/96K, a = 2 ^(-9) (1 + n).
 *            n (coef_num) is the coefficient.
 * @retval none
 */
void audio_wind_filter (audio_t *obj, BOOL en, audio_sr sample_rate, u8 coef_num);

/**
  * @brief  Control the mic bias.
  * @param  obj: Audio object define in application software.
  * @param  en: 0 is disable, 1 is enable.
  * @param  voltage: Select the bias voltage.
  *            @arg 00: 0.9AVDD. (default)
  *            @arg 01: 0.86AVDD.
  *            @arg 10: 0.75AVDD.
  *            @arg 11: reserved.
  * @retval none
  */
void audio_mic_bias_ctrl (audio_t *obj, BOOL en, audio_bias_voltage voltage);

/**
  * @brief  Control the amplifier of the headphone. The default value is enabled. 
  * @param  obj: Audio object define in application software.
  * @param  en: 0 is disable, 1 is enable.
  * @retval none
  */
void audio_hpo_amplifier (audio_t *obj, BOOL en);

/*\@}*/

#ifdef __cplusplus
}
#endif
///@}
#endif  // end of "#if defined(CONFIG_PLATFORM_8195BHP)"
#endif
