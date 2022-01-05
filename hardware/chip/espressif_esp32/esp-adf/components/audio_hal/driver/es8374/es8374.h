/*
 * ESPRESSIF MIT License
 *
 * Copyright (c) 2018 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
 *
 * Permission is hereby granted for use on all ESPRESSIF SYSTEMS products, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#ifndef __ES8374_H__
#define __ES8374_H__

#include "esp_types.h"
#include "audio_hal.h"
#include "esxxx_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ES8374 address */
#define ES8374_ADDR 0x20  // 0x22:CE=1;0x20:CE=0

/**
 * @brief Initialize ES8374 codec chip
 *
 * @param cfg configuration of ES8374
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t es8374_codec_init(audio_hal_codec_config_t *cfg);

/**
 * @brief Deinitialize ES8374 codec chip
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t es8374_codec_deinit(void);

/**
 * @brief Configure ES8374 I2S format
 *
 * @param mode:  set ADC or DAC or both
 * @param fmt:  ES8374 I2S format
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t es8374_config_fmt(es_module_t mode, es_i2s_fmt_t fmt);

/**
 * @brief Configure I2S clock in MSATER mode
 *
 * @param cfg:  set bits clock and WS clock
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t es8374_i2s_config_clock(es_i2s_clock_t cfg);

/**
 * @brief Configure ES8374 data sample bits
 *
 * @param mode:  set ADC or DAC or both
 * @param bit_per_sample:  bit number of per sample
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t es8374_set_bits_per_sample(es_module_t mode, es_bits_length_t bit_per_sample);

/**
 * @brief  Start ES8374 codec chip
 *
 * @param mode:  set ADC or DAC or both
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t es8374_start(es_module_t mode);

/**
 * @brief  Stop ES8374 codec chip
 *
 * @param mode:  set ADC or DAC or both
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t es8374_stop(es_module_t mode);

/**
 * @brief  Set voice volume
 *
 * @param volume:  voice volume (0~100)
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t es8374_codec_set_voice_volume(int volume);

/**
 * @brief Get voice volume
 *
 * @param[out] *volume:  voice volume (0~100)
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t es8374_codec_get_voice_volume(int *volume);

/**
 * @brief Mute or unmute ES8374 DAC. Basically you can use this function to mute or unmute the output
 *
 * @param enable mute(1) or unmute(0)
 *
 * @return
 *     - ESP_FAIL Parameter error
 *     - ESP_OK   Success
 */
esp_err_t es8374_set_voice_mute(bool enable);

/**
 * @brief Get ES8374 DAC mute status
 *
 * @return
 *     - ESP_FAIL
 *     - ESP_OK
 */
esp_err_t es8374_get_voice_mute(void);

/**
 * @brief Set ES8374 mic gain
 *
 * @param gain db of mic gain
 *
 * @return
 *     - ESP_FAIL Parameter error
 *     - ESP_OK   Success
 */
esp_err_t es8374_set_mic_gain(es_mic_gain_t gain);

/**
 * @brief Set ES8374 ADC input mode
 *
 * @param input adc input mode
 *
 * @return
 *     - ESP_FAIL Parameter error
 *     - ESP_OK   Success
 */
esp_err_t es8374_config_adc_input(es_adc_input_t input);

/**
 * @brief Set ES8374 DAC output mode
 *
 * @param output dac output mode
 *
 * @return
 *     - ESP_FAIL Parameter error
 *     - ESP_OK   Success
 */
esp_err_t es8374_config_dac_output(es_dac_output_t output);

/**
 * @brief Write ES8374 register
 *
 * @param reg_add address of register
 * @param data data of register
 *
 * @return
 *     - ESP_FAIL Parameter error
 *     - ESP_OK   Success
 */
esp_err_t es8374_write_reg(uint8_t reg_add, uint8_t data);

/**
 * @brief Print all ES8374 registers
 *
 * @return
 *    - void
 */
void es8374_read_all();

/**
 * @brief Configure ES8374 codec mode and I2S interface
 *
 * @param mode codec mode
 * @param iface I2S config
 *
 * @return
 *     - ESP_FAIL Parameter error
 *     - ESP_OK   Success
 */
esp_err_t es8374_codec_config_i2s(audio_hal_codec_mode_t mode, audio_hal_codec_i2s_iface_t *iface);

/**
 * @brief Control ES8374 codec chip
 *
 * @param mode codec mode
 * @param ctrl_state start or stop decode or encode progress
 *
 * @return
 *     - ESP_FAIL Parameter error
 *     - ESP_OK   Success
 */
esp_err_t es8374_codec_ctrl_state(audio_hal_codec_mode_t mode, audio_hal_ctrl_t ctrl_state);

/**
 * @brief Set ES8374 PA power
 *
 * @param enable true for enable PA power, false for disable PA power
 *
 * @return
 *     - void
 */
void es8374_pa_power(bool enable);

#ifdef __cplusplus
}
#endif

#endif  //__ES8374_H__

