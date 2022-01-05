/*
 * ESPRESSIF MIT License
 *
 * Copyright (c) 2019 <ESPRESSIF SYSTEMS (SHANGHAI) CO., LTD>
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

#ifndef _ES7243_H_
#define _ES7243_H_

#include "esp_err.h"
#include "audio_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize ES7243 adc chip
 *
 * @param codec_cfg configuration of ES7243
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t es7243_adc_init(audio_hal_codec_config_t *codec_cfg);

/**
 * @brief Deinitialize ES7243 adc chip
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t es7243_adc_deinit(void);

/**
 * @brief Control ES7243 adc chip
 *
 * @param mode adc mode
 * @param ctrl_state start or stop decode or encode progress
 *
 * @return
 *     - ESP_FAIL Parameter error
 *     - ESP_OK   Success
 */
esp_err_t es7243_adc_ctrl_state(audio_hal_codec_mode_t mode, audio_hal_ctrl_t ctrl_state);

/**
 * @brief Configure ES7243 adc mode and I2S interface
 *
 * @param mode codec mode
 * @param iface I2S config
 *
 * @return
 *     - ESP_FAIL Parameter error
 *     - ESP_OK   Success
 */
esp_err_t es7243_adc_config_i2s(audio_hal_codec_mode_t mode, audio_hal_codec_i2s_iface_t *iface);

/**
 * @brief  Set mute
 *
 * @param mute  true, false
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t es7243_adc_set_voice_mute(bool mute);

/**
 * @brief  Set adc gain
 *
 * @param volume  value of gain (0~100)
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t es7243_adc_set_voice_volume(int volume);

/**
 * @brief Get adc gain
 *
 * @param[out] *volume:  value of gain (0~100)
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t es7243_adc_get_voice_volume(int *volume);

/**
 * @brief Set adc I2C address
 *
 * @param[in] addr:  value of I2C address
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t es7243_adc_set_addr(int addr);

#ifdef __cplusplus
}
#endif

#endif
