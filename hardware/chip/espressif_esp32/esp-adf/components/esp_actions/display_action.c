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
#include "display_action.h"
#include "esp_log.h"
#include "display_service.h"

static const char *TAG = "DIS_ACTION";

esp_err_t display_action_wifi_disconnected(void *instance, action_arg_t *arg, action_result_t *result)
{
    ESP_LOGI(TAG, "%s", __func__);
    display_service_handle_t dis = (display_service_handle_t)instance;
    int ret = display_service_set_pattern(dis, DISPLAY_PATTERN_WIFI_DISCONNECTED, 0);
    return ret;
}

esp_err_t display_action_wifi_connected(void *instance, action_arg_t *arg, action_result_t *result)
{
    ESP_LOGI(TAG, "%s", __func__);
    display_service_handle_t dis = (display_service_handle_t)instance;
    int ret = display_service_set_pattern(dis, DISPLAY_PATTERN_WIFI_CONNECTED, 0);
    return ret;
}

esp_err_t display_action_wifi_setting(void *instance, action_arg_t *arg, action_result_t *result)
{
    ESP_LOGI(TAG, "%s", __func__);
    display_service_handle_t dis = (display_service_handle_t)instance;
    int ret = display_service_set_pattern(dis, DISPLAY_PATTERN_WIFI_SETTING, 0);
    return ret;
}

esp_err_t display_action_turn_off(void *instance, action_arg_t *arg, action_result_t *result)
{
    ESP_LOGI(TAG, "%s", __func__);
    display_service_handle_t dis = (display_service_handle_t)instance;
    int ret = display_service_set_pattern(dis, DISPLAY_PATTERN_TURN_OFF, 0);
    return ret;
}

esp_err_t display_action_turn_on(void *instance, action_arg_t *arg, action_result_t *result)
{
    ESP_LOGI(TAG, "%s", __func__);
    display_service_handle_t dis = (display_service_handle_t)instance;
    int ret = display_service_set_pattern(dis, DISPLAY_PATTERN_TURN_ON, 0);
    return ret;
}
