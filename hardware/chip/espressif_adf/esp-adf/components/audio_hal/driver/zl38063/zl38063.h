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

#ifndef __ZL38063_H__
#define __ZL38063_H__

#include "audio_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize ZL38063 chip
 *
 * @param cfg configuration of ZL38063
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t zl38063_codec_init(audio_hal_codec_config_t *cfg);
/**
 * @brief Deinitialize ZL38063 chip
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t zl38063_codec_deinit(void);
/**
 * The functions zl38063_ctrl_state and zl38063_config_i2s are not used by this driver.
 * They are kept here to maintain the uniformity and convenience of the interface
 * of the ADF project.
 * These settings for zl38063 are burned in firmware and configuration files.
 * Default i2s configuration: 48000Hz, 16bit, Left-Right channels.
 * Use resampling to be compatible with different file types.
 *
 * @brief Control ZL38063 chip
 *
 * @param mode codec mode
 * @param ctrl_state start or stop decode or encode progress
 *
 * @return
 *     - ESP_FAIL Parameter error
 *     - ESP_OK   Success
 */
esp_err_t zl38063_codec_ctrl_state(audio_hal_codec_mode_t mode, audio_hal_ctrl_t ctrl_state);
/**
 * @brief Configure ZL38063 codec mode and I2S interface
 *
 * @param mode codec mode
 * @param iface I2S config
 *
 * @return
 *     - ESP_FAIL Parameter error
 *     - ESP_OK   Success
 */
esp_err_t zl38063_codec_config_i2s(audio_hal_codec_mode_t mode, audio_hal_codec_i2s_iface_t *iface);

/**
 * @brief mute or unmute the codec
 *
 * @param mute:  true, false
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t zl38063_codec_set_voice_mute(bool mute);

/**
 * @brief  Set voice volume
 *
 * @param volume:  voice volume (0~100)
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t zl38063_codec_set_voice_volume(int volume);

/**
 * @brief Get voice volume
 *
 * @param[out] *volume:  voice volume (0~100)
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t zl38063_codec_get_voice_volume(int *volume);

#ifdef __cplusplus
}
#endif

#endif
