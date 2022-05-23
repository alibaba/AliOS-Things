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

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "input_key_service.h"
#include "esp_peripherals.h"
#include "periph_service.h"
#include "periph_button.h"
#include "periph_touch.h"
#include "periph_adc_button.h"
#include "board.h"
#include "unity.h"

static const char *TAG = "TEST_INPUT_KEY_SERVICE";

static esp_periph_set_handle_t set = NULL;

static esp_err_t test_input_key_service_callback(periph_service_handle_t handle, periph_service_event_t *evt, void *ctx)
{
    TEST_ASSERT_NOT_NULL(handle);
    TEST_ASSERT_NOT_NULL(evt);
    // ESP_LOGI(TAG, "type=>%d, source=>%d, data=>%d, len=>%d", evt->type, (int)evt->source, (int)evt->data, evt->len);
    if (evt->type == INPUT_KEY_SERVICE_ACTION_CLICK_RELEASE) {
        switch ((int) evt->data) {
            case INPUT_KEY_USER_ID_PLAY:
                ESP_LOGI(TAG, "[Play] button press");
                break;
            case INPUT_KEY_USER_ID_SET:
                ESP_LOGI(TAG, "[Set] button press");
                break;
            case INPUT_KEY_USER_ID_VOLUP:
                ESP_LOGI(TAG, "[VOL+] button press");
                break;
            case INPUT_KEY_USER_ID_VOLDOWN:
                ESP_LOGI(TAG, "[VOL-] button press");
                break;
            case INPUT_KEY_USER_ID_MODE:
                ESP_LOGI(TAG, "[Mode] button press");
                break;
            case INPUT_KEY_USER_ID_REC:
                ESP_LOGI(TAG, "[Rec] button press");
                break;
            default:
                ESP_LOGI(TAG, "[Userdefined] button press");
                break;
        }
    }
    return ESP_OK;
}

static periph_service_handle_t test_input_key_service_create()
{
    esp_periph_config_t periph_cfg = DEFAULT_ESP_PERIPH_SET_CONFIG();
    set = esp_periph_set_init(&periph_cfg);
    TEST_ASSERT_NOT_NULL(set);

    TEST_ASSERT_FALSE(audio_board_key_init(set));

    input_key_service_info_t input_info[] = INPUT_KEY_DEFAULT_INFO();
    input_key_service_cfg_t input_cfg = INPUT_KEY_SERVICE_DEFAULT_CONFIG();
    input_cfg.handle = set;
    periph_service_handle_t input_key_handle = input_key_service_create(&input_cfg);
    TEST_ASSERT_NOT_NULL(input_key_handle);
    TEST_ASSERT_FALSE(input_key_service_add_key(input_key_handle, input_info, INPUT_KEY_NUM));
    TEST_ASSERT_FALSE(periph_service_set_callback(input_key_handle, test_input_key_service_callback, NULL));
    return input_key_handle;
}

static void test_input_key_service_destroy(periph_service_handle_t handle)
{
    TEST_ASSERT_FALSE(periph_service_destroy(handle));
    TEST_ASSERT_FALSE(esp_periph_set_destroy(set));
}

TEST_CASE("Operate input_key_service", "[input_key_service]")
{
    periph_service_handle_t input_key_handle = test_input_key_service_create();
    ESP_LOGI(TAG, "input key service start, please press the buttons");
    vTaskDelay(5000 / portTICK_PERIOD_MS);

    test_input_key_service_destroy(input_key_handle);
}

