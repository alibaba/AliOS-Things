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

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include "esp_log.h"
#include "esp_system.h"
#include "unity.h"
#include "audio_element.h"
#include "audio_pipeline.h"
#include "audio_event_iface.h"
#include "audio_common.h"
#include "i2s_stream.h"
#include "esp_peripherals.h"
#include "periph_touch.h"
#include "board.h"
#include "bluetooth_service.h"

static const char *TAG = "TEST_BLUETOOTH_SERVICE";

/**
 * Usage case test
 */
TEST_CASE("Initialize a2dp sink, create bluetooth stream and destroy stream later", "[bluetooth_service]")
{
    bluetooth_service_cfg_t bt_cfg = {
        .device_name = "ESP-ADF-SPEAKER",
        .mode = BLUETOOTH_A2DP_SINK,
    };
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_LOGI(TAG, "Init Bluetooth service");
    TEST_ASSERT_EQUAL(ESP_OK, bluetooth_service_start(&bt_cfg));

    vTaskDelay(100 / portTICK_PERIOD_MS);
    ESP_LOGI(TAG, "Init Bluetooth service repeatedly");
    TEST_ASSERT_NOT_EQUAL(ESP_OK, bluetooth_service_start(&bt_cfg));

    ESP_LOGI(TAG, "Create Bluetooth stream");
    audio_element_handle_t bt_stream_reader = bluetooth_service_create_stream();
    TEST_ASSERT_NOT_NULL(bt_stream_reader);

    ESP_LOGI(TAG, "Create Bluetooth stream repeatedly");
    TEST_ASSERT_NULL(bluetooth_service_create_stream());

    ESP_LOGI(TAG, "Initialize peripherals");
    esp_periph_config_t periph_cfg = DEFAULT_ESP_PERIPH_SET_CONFIG();
    esp_periph_set_handle_t set = esp_periph_set_init(&periph_cfg);
    TEST_ASSERT_NOT_NULL(set);
 
    ESP_LOGI(TAG, "Create Bluetooth peripheral");
    esp_periph_handle_t bt_periph = bluetooth_service_create_periph();
    TEST_ASSERT_NOT_NULL(bt_periph);

    ESP_LOGI(TAG, "Create Bluetooth peripheral repeatedly");
    TEST_ASSERT_NULL(bluetooth_service_create_periph());

    audio_element_run(bt_stream_reader);

    ESP_LOGI(TAG, "Start bt peripheral");
    TEST_ASSERT_FALSE(esp_periph_start(set, bt_periph));
    vTaskDelay(100 / portTICK_PERIOD_MS);

    ESP_LOGI(TAG, "Destory stream");
    TEST_ASSERT_EQUAL(ESP_OK, esp_periph_set_stop_all(set));
    TEST_ASSERT_EQUAL(ESP_OK, audio_element_stop(bt_stream_reader));
    TEST_ASSERT_EQUAL(ESP_OK, audio_element_deinit(bt_stream_reader));
    TEST_ASSERT_EQUAL(ESP_OK, esp_periph_set_destroy(set));
    TEST_ASSERT_EQUAL(ESP_OK, bluetooth_service_destroy());
    TEST_ASSERT_EQUAL(ESP_OK, nvs_flash_deinit());
}

TEST_CASE("Initialize a2dp source, create bluetooth stream and destroy stream later", "[bluetooth_service]")
{
    bluetooth_service_cfg_t bt_cfg = {
        .mode = BLUETOOTH_A2DP_SOURCE,
    };
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_LOGI(TAG, "Init Bluetooth service");
    TEST_ASSERT_EQUAL(ESP_OK, bluetooth_service_start(&bt_cfg));
    ESP_LOGI(TAG, "Init Bluetooth service repeatedly");
    TEST_ASSERT_NOT_EQUAL(ESP_OK, bluetooth_service_start(&bt_cfg));

    ESP_LOGI(TAG, "Create Bluetooth stream");
    audio_element_handle_t bt_stream_writer = bluetooth_service_create_stream();
    TEST_ASSERT_NOT_NULL(bt_stream_writer);

    ESP_LOGI(TAG, "Create Bluetooth stream repeatedly");
    TEST_ASSERT_NULL(bluetooth_service_create_stream());

    ESP_LOGI(TAG, "Initialize peripherals");
    esp_periph_config_t periph_cfg = DEFAULT_ESP_PERIPH_SET_CONFIG();
    esp_periph_set_handle_t set = esp_periph_set_init(&periph_cfg);
    TEST_ASSERT_NOT_NULL(set);

    ESP_LOGI(TAG, "Create Bluetooth peripheral");
    esp_periph_handle_t bt_periph = bluetooth_service_create_periph();
    TEST_ASSERT_NOT_NULL(bt_periph);

    ESP_LOGI(TAG, "Create Bluetooth peripheral repeatedly");
    TEST_ASSERT_NULL(bluetooth_service_create_periph());

    audio_element_run(bt_stream_writer);

    ESP_LOGI(TAG, "Start bt peripheral");
    TEST_ASSERT_FALSE(esp_periph_start(set, bt_periph));
    vTaskDelay(100 / portTICK_PERIOD_MS);
    ESP_LOGI(TAG, "Destory stream");
    TEST_ASSERT_EQUAL(ESP_OK, esp_periph_set_stop_all(set));
    TEST_ASSERT_EQUAL(ESP_OK, audio_element_stop(bt_stream_writer));
    TEST_ASSERT_EQUAL(ESP_OK, audio_element_deinit(bt_stream_writer));
    TEST_ASSERT_EQUAL(ESP_OK, esp_periph_set_destroy(set));
    TEST_ASSERT_EQUAL(ESP_OK, bluetooth_service_destroy());
    TEST_ASSERT_EQUAL(ESP_OK, nvs_flash_deinit());
}

TEST_CASE("Initialize a2dp sink, test discovery and cancel discovery", "[bluetooth_service]")
{
    bluetooth_service_cfg_t bt_cfg = {
        .device_name = "ESP-ADF-SPEAKER",
        .mode = BLUETOOTH_A2DP_SINK,
    };
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_LOGI(TAG, "Init Bluetooth service");
    TEST_ASSERT_EQUAL(ESP_OK, bluetooth_service_start(&bt_cfg));

    ESP_LOGI(TAG, "Create Bluetooth stream");
    audio_element_handle_t bt_stream_reader = bluetooth_service_create_stream();
    TEST_ASSERT_NOT_NULL(bt_stream_reader);

    ESP_LOGI(TAG, "Initialize peripherals");
    esp_periph_config_t periph_cfg = DEFAULT_ESP_PERIPH_SET_CONFIG();
    esp_periph_set_handle_t set = esp_periph_set_init(&periph_cfg);
    TEST_ASSERT_NOT_NULL(set);

    ESP_LOGI(TAG, "Create Bluetooth peripheral");
    esp_periph_handle_t bt_periph = bluetooth_service_create_periph();
    TEST_ASSERT_NOT_NULL(bt_periph);

    ESP_LOGI(TAG, "Start bt peripheral");
    TEST_ASSERT_EQUAL(ESP_OK, esp_periph_start(set, bt_periph));
    vTaskDelay(100 / portTICK_PERIOD_MS);
    
    TEST_ASSERT_EQUAL(ESP_OK, periph_bluetooth_discover(bt_periph));
    vTaskDelay(1000 / portTICK_PERIOD_MS);

    TEST_ASSERT_EQUAL(ESP_OK, periph_bluetooth_cancel_discover(bt_periph));
    audio_element_run(bt_stream_reader);

    ESP_LOGI(TAG, "Destory stream");
    TEST_ASSERT_EQUAL(ESP_OK, esp_periph_set_stop_all(set));
    TEST_ASSERT_EQUAL(ESP_OK, audio_element_stop(bt_stream_reader));
    TEST_ASSERT_EQUAL(ESP_OK, audio_element_deinit(bt_stream_reader));
    TEST_ASSERT_EQUAL(ESP_OK, esp_periph_set_destroy(set));
    TEST_ASSERT_EQUAL(ESP_OK, bluetooth_service_destroy());
    TEST_ASSERT_EQUAL(ESP_OK, nvs_flash_deinit());
}

TEST_CASE("Initialize a2dp sink, test avrcp passthrough cmd", "[bluetooth_service]")
{
    audio_pipeline_handle_t pipeline;
    audio_element_handle_t bt_stream_reader, i2s_stream_writer;
    bluetooth_service_cfg_t bt_cfg = {
        .device_name = "ESP-ADF-SPEAKER",
        .mode = BLUETOOTH_A2DP_SINK,
    };
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_LOGI(TAG, "Init Bluetooth service");
    TEST_ASSERT_EQUAL(ESP_OK, bluetooth_service_start(&bt_cfg));

    ESP_LOGI(TAG, "Start codec chip");
    audio_board_handle_t board_handle = audio_board_init();
    TEST_ASSERT_NOT_NULL(board_handle);
    TEST_ASSERT_EQUAL(ESP_OK, audio_hal_ctrl_codec(board_handle->audio_hal, AUDIO_HAL_CODEC_MODE_DECODE, AUDIO_HAL_CTRL_START));

    ESP_LOGI(TAG, "Create audio pipeline for playback");
    audio_pipeline_cfg_t pipeline_cfg = DEFAULT_AUDIO_PIPELINE_CONFIG();
    pipeline = audio_pipeline_init(&pipeline_cfg);
    TEST_ASSERT_NOT_NULL(pipeline);

    ESP_LOGI(TAG, "Create i2s stream to write data to codec chip");
    i2s_stream_cfg_t i2s_cfg = I2S_STREAM_CFG_DEFAULT();
    i2s_cfg.type = AUDIO_STREAM_WRITER;
    i2s_stream_writer = i2s_stream_init(&i2s_cfg);
    TEST_ASSERT_NOT_NULL(i2s_stream_writer);

    ESP_LOGI(TAG, "Create Bluetooth stream");
    bt_stream_reader = bluetooth_service_create_stream();
    TEST_ASSERT_NOT_NULL(bt_stream_reader);

    ESP_LOGI(TAG, "Register all elements to audio pipeline");
    TEST_ASSERT_EQUAL(ESP_OK, audio_pipeline_register(pipeline, bt_stream_reader, "bt"));
    TEST_ASSERT_EQUAL(ESP_OK, audio_pipeline_register(pipeline, i2s_stream_writer, "i2s"));

    ESP_LOGI(TAG, "Link it together [Bluetooth]-->bt_stream_reader-->i2s_stream_writer-->[codec_chip]");
    TEST_ASSERT_EQUAL(ESP_OK, audio_pipeline_link(pipeline, (const char *[]) {"bt", "i2s"}, 2));

    ESP_LOGI(TAG, "Initialize peripherals");
    esp_periph_config_t periph_cfg = DEFAULT_ESP_PERIPH_SET_CONFIG();
    esp_periph_set_handle_t set = esp_periph_set_init(&periph_cfg);
    TEST_ASSERT_NOT_NULL(set);

    ESP_LOGI(TAG, "Initialize Touch peripheral");
    TEST_ASSERT_EQUAL(ESP_OK, audio_board_key_init(set));

    ESP_LOGI(TAG, "Create Bluetooth peripheral");
    esp_periph_handle_t bt_periph = bluetooth_service_create_periph();
    TEST_ASSERT_NOT_NULL(bt_periph);

    ESP_LOGI(TAG, "Start bt peripheral");
    TEST_ASSERT_EQUAL(ESP_OK, esp_periph_start(set, bt_periph));

    ESP_LOGI(TAG, "Set up  event listener");
    audio_event_iface_cfg_t evt_cfg = AUDIO_EVENT_IFACE_DEFAULT_CFG();
    audio_event_iface_handle_t evt = audio_event_iface_init(&evt_cfg);
    TEST_ASSERT_NOT_NULL(evt);

    ESP_LOGI(TAG, "Listening event from all elements of pipeline");
    TEST_ASSERT_EQUAL(ESP_OK, audio_pipeline_set_listener(pipeline, evt));

    ESP_LOGI(TAG, "Listening event from peripherals");
    TEST_ASSERT_EQUAL(ESP_OK, audio_event_iface_set_listener(esp_periph_set_get_event_iface(set), evt));

    ESP_LOGI(TAG, "Start audio_pipeline");
    TEST_ASSERT_EQUAL(ESP_OK, audio_pipeline_run(pipeline));

    ESP_LOGI(TAG, "Listen for all pipeline events");
    while (1) {
        audio_event_iface_msg_t msg;
        esp_err_t ret = audio_event_iface_listen(evt, &msg, portMAX_DELAY);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "[ * ] Event interface error : %d", ret);
            continue;
        }

        if (msg.source_type == AUDIO_ELEMENT_TYPE_ELEMENT && msg.source == (void *) bt_stream_reader
            && msg.cmd == AEL_MSG_CMD_REPORT_MUSIC_INFO) {
            audio_element_info_t music_info = {0};
            audio_element_getinfo(bt_stream_reader, &music_info);

            ESP_LOGI(TAG, "[ * ] Receive music info from Bluetooth, sample_rates=%d, bits=%d, ch=%d",
                     music_info.sample_rates, music_info.bits, music_info.channels);

            audio_element_setinfo(i2s_stream_writer, &music_info);
            i2s_stream_set_clk(i2s_stream_writer, music_info.sample_rates, music_info.bits, music_info.channels);
            continue;
        }

        if (msg.source_type == PERIPH_ID_TOUCH
            && msg.cmd == PERIPH_TOUCH_TAP
            && msg.source == (void *)esp_periph_set_get_by_id(set, PERIPH_ID_TOUCH)) {

            if ((int) msg.data == get_input_play_id()) {
                ESP_LOGI(TAG, "[ * ] [Play] touch tap event");
                TEST_ASSERT_EQUAL(ESP_OK, periph_bluetooth_play(bt_periph));
            } else if ((int) msg.data == get_input_set_id()) {
                ESP_LOGI(TAG, "[ * ] [Set] touch tap event");
                TEST_ASSERT_EQUAL(ESP_OK, periph_bluetooth_pause(bt_periph));
            } else if ((int) msg.data == get_input_volup_id()) {
                ESP_LOGI(TAG, "[ * ] [Vol+] touch tap event");
                TEST_ASSERT_EQUAL(ESP_OK, periph_bluetooth_next(bt_periph));
            } else if ((int) msg.data == get_input_voldown_id()) {
                ESP_LOGI(TAG, "[ * ] [Vol-] touch tap event");
                TEST_ASSERT_EQUAL(ESP_OK, periph_bluetooth_prev(bt_periph));
            }
        }

        /* Stop when the Bluetooth is disconnected or suspended */
        if (msg.source_type == PERIPH_ID_BLUETOOTH
            && msg.source == (void *)bt_periph) {
            if (msg.cmd == PERIPH_BLUETOOTH_DISCONNECTED) {
                ESP_LOGW(TAG, "[ * ] Bluetooth disconnected");
                break;
            }
        }
        /* Stop when the last pipeline element (i2s_stream_writer in this case) receives stop event */
        if (msg.source_type == AUDIO_ELEMENT_TYPE_ELEMENT && msg.source == (void *) i2s_stream_writer
            && msg.cmd == AEL_MSG_CMD_REPORT_STATUS
            && (((int)msg.data == AEL_STATUS_STATE_STOPPED) || ((int)msg.data == AEL_STATUS_STATE_FINISHED))) {
            ESP_LOGW(TAG, "[ * ] Stop event received");
            break;
        }
    }

    ESP_LOGI(TAG, "Stop audio_pipeline");
    TEST_ASSERT_EQUAL(ESP_OK, audio_pipeline_terminate(pipeline));
    TEST_ASSERT_EQUAL(ESP_OK, audio_pipeline_unregister(pipeline, bt_stream_reader));
    TEST_ASSERT_EQUAL(ESP_OK, audio_pipeline_unregister(pipeline, i2s_stream_writer));
    TEST_ASSERT_EQUAL(ESP_OK, audio_pipeline_remove_listener(pipeline));
    TEST_ASSERT_EQUAL(ESP_OK, esp_periph_set_stop_all(set));
    TEST_ASSERT_EQUAL(ESP_OK, audio_event_iface_remove_listener(esp_periph_set_get_event_iface(set), evt));
    TEST_ASSERT_EQUAL(ESP_OK, audio_event_iface_destroy(evt));

    ESP_LOGI(TAG, "Release all resources");
    TEST_ASSERT_EQUAL(ESP_OK, audio_pipeline_deinit(pipeline));
    TEST_ASSERT_EQUAL(ESP_OK, audio_element_deinit(bt_stream_reader));
    TEST_ASSERT_EQUAL(ESP_OK, audio_element_deinit(i2s_stream_writer));
    TEST_ASSERT_EQUAL(ESP_OK, esp_periph_set_destroy(set));
    TEST_ASSERT_EQUAL(ESP_OK, bluetooth_service_destroy());
    TEST_ASSERT_EQUAL(ESP_OK, nvs_flash_deinit());
}
