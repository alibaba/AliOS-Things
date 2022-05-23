/*
 * ESPRESSIF MIT License
 *
 * Copyright (c) 2020 <ESPRESSIF SYSTEMS (SHANGHAI) CO., LTD>
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

#include "esp_log.h"
#include "es7148.h"
#include "driver/gpio.h"
#include "board.h"


static const char *TAG = "es7148";


static bool codec_init_flag = 0;

audio_hal_func_t AUDIO_CODEC_ES7148_DEFAULT_HANDLE = {
    .audio_codec_initialize = es7148_codec_init,
    .audio_codec_deinitialize = es7148_codec_deinit,
    .audio_codec_ctrl = es7148_codec_ctrl_state,
    .audio_codec_config_iface = es7148_codec_config_i2s,
    .audio_codec_set_mute = es7148_codec_set_voice_mute,
    .audio_codec_set_volume = es7148_codec_set_voice_volume,
    .audio_codec_get_volume = es7148_codec_get_voice_volume,
    .audio_hal_lock = NULL,
    .handle = NULL,
};

static bool es7148_codec_initialized()
{
    return codec_init_flag;
}

esp_err_t es7148_codec_init(audio_hal_codec_config_t *cfg)
{
    if (es7148_codec_initialized()) {
        ESP_LOGW(TAG, "The es7148 codec has been already initialized");
        return ESP_OK;
    }
    codec_init_flag = true;
    return ESP_OK;
}

esp_err_t es7148_codec_deinit(void)
{
    codec_init_flag = false;
    return ESP_OK;
}

esp_err_t es7148_codec_ctrl_state(audio_hal_codec_mode_t mode, audio_hal_ctrl_t ctrl_state)
{
    return ESP_OK;
}

esp_err_t es7148_codec_config_i2s(audio_hal_codec_mode_t mode, audio_hal_codec_i2s_iface_t *iface)
{
    return ESP_OK;
}

esp_err_t es7148_codec_set_voice_mute(bool mute)
{
    return ESP_OK;
}

esp_err_t es7148_codec_set_voice_volume(int volume)
{
    int ret = 0;
    return ret;
}

esp_err_t es7148_codec_get_voice_volume(int *volume)
{
    int ret = 0;
    return ret;
}
