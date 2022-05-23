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
#include "esp_log.h"
#include "wifi_action.h"
#include "wifi_service.h"

static const char *TAG = "WIFI_ACTION";


esp_err_t wifi_action_connect(void *ctx, action_arg_t *arg, action_result_t *result)
{
    periph_service_handle_t wifi_serv = (periph_service_handle_t)ctx;
    wifi_service_connect(wifi_serv);
    ESP_LOGI(TAG, "%s", __func__);
    return ESP_OK;
}

esp_err_t wifi_action_disconnect(void *ctx, action_arg_t *arg, action_result_t *result)
{
    periph_service_handle_t wifi_serv = (periph_service_handle_t)ctx;
    wifi_service_disconnect(wifi_serv);
    ESP_LOGI(TAG, "%s", __func__);
    return ESP_OK;
}

esp_err_t wifi_action_setting_start(void *ctx, action_arg_t *arg, action_result_t *result)
{
    periph_service_handle_t wifi_serv = (periph_service_handle_t)ctx;
    wifi_service_setting_start(wifi_serv, 0);
    ESP_LOGI(TAG, "%s", __func__);
    return ESP_OK;
}

esp_err_t wifi_action_setting_stop(void *ctx, action_arg_t *arg, action_result_t *result)
{
    periph_service_handle_t wifi_serv = (periph_service_handle_t)ctx;
    ESP_LOGI(TAG, "%s", __func__);
    wifi_service_setting_stop(wifi_serv, 0);
    return ESP_OK;
}
