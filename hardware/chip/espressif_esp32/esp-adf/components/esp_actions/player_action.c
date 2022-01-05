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

#include "audio_error.h"
#include "esp_action_def.h"
#include "esp_audio.h"
#include "esp_log.h"

static const char *TAG = "PLAYER_ACTION";

esp_err_t player_action_play(void *instance, action_arg_t *arg, action_result_t *result)
{
    esp_audio_handle_t handle = (esp_audio_handle_t)instance;
    ESP_LOGI(TAG, "%s", __func__);
    int ret = esp_audio_play(handle, AUDIO_CODEC_TYPE_DECODER, NULL, 0);
    return ret;
}

esp_err_t player_action_pause(void *instance, action_arg_t *arg, action_result_t *result)
{
    ESP_LOGI(TAG, "%s", __func__);
    esp_audio_handle_t handle = (esp_audio_handle_t)instance;
    int ret = esp_audio_pause(handle);
    return ret;
}

esp_err_t player_action_next(void *instance, action_arg_t *arg, action_result_t *result)
{
    ESP_LOGI(TAG, "%s", __func__);

    return ESP_OK;
}

esp_err_t player_action_prev(void *instance, action_arg_t *arg, action_result_t *result)
{

    ESP_LOGI(TAG, "%s", __func__);

    return ESP_OK;
}

esp_err_t player_action_vol_up(void *instance, action_arg_t *arg, action_result_t *result)
{
    int player_volume = 0;
    esp_audio_handle_t handle = (esp_audio_handle_t)instance;
    esp_audio_vol_get(handle, &player_volume);
    player_volume += 10;
    if (player_volume > 100) {
        player_volume = 100;
    }
    esp_audio_vol_set(handle, player_volume);
    ESP_LOGI(TAG, "%s, vol:[%d]", __func__, player_volume);
    return ESP_OK;
}

esp_err_t player_action_vol_down(void *instance, action_arg_t *arg, action_result_t *result)
{
    int player_volume = 0;
    esp_audio_handle_t handle = (esp_audio_handle_t)instance;
    esp_audio_vol_get(handle, &player_volume);
    player_volume -= 10;
    if (player_volume < 0) {
        player_volume = 0;
    }
    esp_audio_vol_set(handle, player_volume);
    ESP_LOGI(TAG, "%s, vol:[%d]", __func__, player_volume);
    return ESP_OK;
}

esp_err_t player_action_mute_on(void *instance, action_arg_t *arg, action_result_t *result)
{
    ESP_LOGI(TAG, "%s", __func__);
    return ESP_OK;
}

esp_err_t player_action_mute_off(void *instance, action_arg_t *arg, action_result_t *result)
{
    ESP_LOGI(TAG, "%s", __func__);
    return ESP_OK;
}
