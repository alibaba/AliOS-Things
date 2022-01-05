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
#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "unity.h"
#include "esp_wifi.h"
#include "esp_http_client.h"
#include "nvs_flash.h"
#include "esp_log.h"
#include "audio_pipeline.h"
#include "audio_mem.h"
#include "audio_element.h"
#include "audio_event_iface.h"

#include "tcp_client_stream.h"
#include "fatfs_stream.h"

#include "esp_peripherals.h"
#include "periph_wifi.h"

#define CONFIG_TCP_URL   "192.168.199.118"
#define CONFIG_TCP_PORT  8080

static const char *TAG =  "TCP_CLIENT_STREAM_TEST"

TEST_CASE("tcp client stream init memory", "[esp-adf-stream]")
{
    esp_log_level_set("AUDIO_ELEMENT", ESP_LOG_DEBUG);
    audio_element_handle_t tcp_stream_writer
    tcp_stream_cfg_t tcp_cfg = TCP_STREAM_CFG_DEFAULT();
    tcp_cfg.type = AUDIO_STREAM_WRITER;
    int cnt = 2000;
    AUDIO_MEM_SHOW("BEFORE TCP_CLIENT_STREAM_INIT MEMORY TEST");
    while (cnt--) {
    	tcp_stream_writer = tcp_stream_init(&tcp_cfg);
    	TEST_ASSERT_NOT_NULL(tcp_stream_writer);
        TEST_ASSERT_EQUAL(ESP_OK, audio_element_deinit(tcp_stream_writer));
    }
    AUDIO_MEM_SHOW("AFTER TCP_CLIENT_STREAM_INIT MEMORY TEST");
}

TEST_CASE("tcp client stream write", "[esp-adf-stream]")
{
	 esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    tcpip_adapter_init();

    audio_pipeline_handle_t pipeline;
    audio_element_handle_t tcp_stream_writer, fatfs_stream_reader;

    esp_log_level_set("*", ESP_LOG_WARN);
    esp_log_level_set(TAG, ESP_LOG_DEBUG);

    esp_periph_config_t periph_cfg = DEFAULT_ESP_PERIPH_SET_CONFIG();
    esp_periph_set_handle_t set = esp_periph_set_init(&periph_cfg);
    TEST_ASSERT_NOT_NULL(set);

    TEST_ASSERT_EQUAL(ESP_OK, audio_board_sdcard_init(set, SD_MODE_1_LINE));

    audio_pipeline_cfg_t pipeline_cfg = DEFAULT_AUDIO_PIPELINE_CONFIG();
    pipeline = audio_pipeline_init(&pipeline_cfg);
    TEST_ASSERT_NOT_NULL(pipeline);

    fatfs_stream_cfg_t fatfs_cfg = FATFS_STREAM_CFG_DEFAULT();
    fatfs_cfg.type = AUDIO_STREAM_READER;
    fatfs_stream_reader = fatfs_stream_init(&fatfs_cfg);
    TEST_ASSERT_NOT_NULL(fatfs_stream_reader);

    tcp_stream_cfg_t tcp_cfg = TCP_STREAM_CFG_DEFAULT();
    tcp_cfg.type = AUDIO_STREAM_WRITER;
    tcp_cfg.port = CONFIG_TCP_PORT;
    tcp_cfg.host = CONFIG_TCP_URL;
    tcp_stream_writer = tcp_stream_init(&tcp_cfg);
    TEST_ASSERT_NOT_NULL(tcp_stream_writer);

    TEST_ASSERT_EQUAL(ESP_OK, audio_pipeline_register(pipeline, fatfs_stream_reader, "file"));
    TEST_ASSERT_EQUAL(ESP_OK, audio_pipeline_register(pipeline, tcp_stream_writer, "tcp"));
    TEST_ASSERT_EQUAL(ESP_OK, audio_pipeline_link(pipeline, (const char *[]) {"file", "tcp"}, 2));
    TEST_ASSERT_EQUAL(ESP_OK, audio_element_set_uri(fatfs_stream_reader, "/sdcard/test.mp3"));

    periph_wifi_cfg_t wifi_cfg = {
        .ssid = CONFIG_WIFI_SSID,
        .password = CONFIG_WIFI_PASSWORD,
    };
    esp_periph_handle_t wifi_handle = periph_wifi_init(&wifi_cfg);
    TEST_ASSERT_EQUAL(ESP_OK, esp_periph_start(set, wifi_handle));
    TEST_ASSERT_EQUAL(ESP_OK, periph_wifi_wait_for_connected(wifi_handle, portMAX_DELAY));

    audio_event_iface_cfg_t evt_cfg = AUDIO_EVENT_IFACE_DEFAULT_CFG();
    audio_event_iface_handle_t evt = audio_event_iface_init(&evt_cfg);
    TEST_ASSERT_NOT_NULL(evt);

    TEST_ASSERT_EQUAL(ESP_OK, audio_pipeline_set_listener(pipeline, evt));
    TEST_ASSERT_EQUAL(ESP_OK, audio_event_iface_set_listener(esp_periph_set_get_event_iface(set), evt));
    TEST_ASSERT_EQUAL(ESP_OK, audio_pipeline_run(pipeline));

    while (1) {
        audio_event_iface_msg_t msg;
        esp_err_t ret = audio_event_iface_listen(evt, &msg, portMAX_DELAY);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "[ * ] Event interface error : %d", ret);
            continue;
        }

        /* Stop when the last pipeline element (fatfs_stream_reader in this case) read stop event */
        if (msg.source_type == AUDIO_ELEMENT_TYPE_ELEMENT && msg.source == (void *) fatfs_stream_reader
            && msg.cmd == AEL_MSG_CMD_REPORT_STATUS
            && (((int)msg.data == AEL_STATUS_STATE_STOPPED) || ((int)msg.data == AEL_STATUS_STATE_FINISHED))) {
            ESP_LOGW(TAG, "[ * ] Stop event received");
            break;
        }
    }

    TEST_ASSERT_EQUAL(ESP_OK, audio_pipeline_terminate(pipeline));
    TEST_ASSERT_EQUAL(ESP_OK, audio_pipeline_unregister(pipeline, tcp_stream_writer));
    TEST_ASSERT_EQUAL(ESP_OK, audio_pipeline_unregister(pipeline, fatfs_stream_reader));
    TEST_ASSERT_EQUAL(ESP_OK, audio_pipeline_remove_listener(pipeline));
    TEST_ASSERT_EQUAL(ESP_OK, esp_periph_set_stop_all(set));
    TEST_ASSERT_EQUAL(ESP_OK, audio_event_iface_remove_listener(esp_periph_set_get_event_iface(set), evt));
    TEST_ASSERT_EQUAL(ESP_OK, audio_event_iface_destroy(evt));
    TEST_ASSERT_EQUAL(ESP_OK, audio_pipeline_deinit(pipeline));
    TEST_ASSERT_EQUAL(ESP_OK, audio_element_deinit(tcp_stream_writer));
    TEST_ASSERT_EQUAL(ESP_OK, audio_element_deinit(fatfs_stream_reader));
    TEST_ASSERT_EQUAL(ESP_OK, esp_periph_set_destroy(set));
}

TEST_CASE("tcp client stream read", "[esp-adf-stream]")
{
	 esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    tcpip_adapter_init();

    audio_pipeline_handle_t pipeline;
    audio_element_handle_t tcp_stream_writer, fatfs_stream_reader;

    esp_log_level_set("*", ESP_LOG_WARN);
    esp_log_level_set(TAG, ESP_LOG_DEBUG);

    esp_periph_config_t periph_cfg = DEFAULT_ESP_PERIPH_SET_CONFIG();
    esp_periph_set_handle_t set = esp_periph_set_init(&periph_cfg);
    TEST_ASSERT_NOT_NULL(set);

    TEST_ASSERT_EQUAL(ESP_OK, audio_board_sdcard_init(set, SD_MODE_1_LINE));

    audio_pipeline_cfg_t pipeline_cfg = DEFAULT_AUDIO_PIPELINE_CONFIG();
    pipeline = audio_pipeline_init(&pipeline_cfg);
    TEST_ASSERT_NOT_NULL(pipeline);

    fatfs_stream_cfg_t fatfs_cfg = FATFS_STREAM_CFG_DEFAULT();
    fatfs_cfg.type = AUDIO_STREAM_WRITER;
    fatfs_stream_reader = fatfs_stream_init(&fatfs_cfg);
    TEST_ASSERT_NOT_NULL(fatfs_stream_reader);

    tcp_stream_cfg_t tcp_cfg = TCP_STREAM_CFG_DEFAULT();
    tcp_cfg.type = AUDIO_STREAM_READER;
    tcp_cfg.port = CONFIG_TCP_PORT;
    tcp_cfg.host = CONFIG_TCP_URL;
    tcp_stream_writer = tcp_stream_init(&tcp_cfg);
    TEST_ASSERT_NOT_NULL(tcp_stream_writer);

    TEST_ASSERT_EQUAL(ESP_OK, audio_pipeline_register(pipeline, tcp_stream_writer, "tcp"));
    TEST_ASSERT_EQUAL(ESP_OK, audio_pipeline_register(pipeline, fatfs_stream_reader, "file"));
    TEST_ASSERT_EQUAL(ESP_OK, audio_pipeline_link(pipeline, (const char *[]) {"tcp", "file"}, 2));
    TEST_ASSERT_EQUAL(ESP_OK, audio_element_set_uri(fatfs_stream_reader, "/sdcard/test.mp3"));

    periph_wifi_cfg_t wifi_cfg = {
        .ssid = CONFIG_WIFI_SSID,
        .password = CONFIG_WIFI_PASSWORD,
    };
    esp_periph_handle_t wifi_handle = periph_wifi_init(&wifi_cfg);
    TEST_ASSERT_EQUAL(ESP_OK, esp_periph_start(set, wifi_handle));
    TEST_ASSERT_EQUAL(ESP_OK, periph_wifi_wait_for_connected(wifi_handle, portMAX_DELAY));

    audio_event_iface_cfg_t evt_cfg = AUDIO_EVENT_IFACE_DEFAULT_CFG();
    audio_event_iface_handle_t evt = audio_event_iface_init(&evt_cfg);
    TEST_ASSERT_NOT_NULL(evt);

    TEST_ASSERT_EQUAL(ESP_OK, audio_pipeline_set_listener(pipeline, evt));
    TEST_ASSERT_EQUAL(ESP_OK, audio_event_iface_set_listener(esp_periph_set_get_event_iface(set), evt));
    TEST_ASSERT_EQUAL(ESP_OK, audio_pipeline_run(pipeline));

    while (1) {
        audio_event_iface_msg_t msg;
        esp_err_t ret = audio_event_iface_listen(evt, &msg, portMAX_DELAY);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "[ * ] Event interface error : %d", ret);
            continue;
        }

        /* Stop when the last pipeline element (tcp_stream_reader in this case) read stop event */
        if (msg.source_type == AUDIO_ELEMENT_TYPE_ELEMENT && msg.source == (void *) tcp_stream_reader
            && msg.cmd == AEL_MSG_CMD_REPORT_STATUS
            && (((int)msg.data == AEL_STATUS_STATE_STOPPED) || ((int)msg.data == AEL_STATUS_STATE_FINISHED))) {
            ESP_LOGW(TAG, "[ * ] Stop event received");
            break;
        }
    }

    TEST_ASSERT_EQUAL(ESP_OK, audio_pipeline_terminate(pipeline));
    TEST_ASSERT_EQUAL(ESP_OK, audio_pipeline_unregister(pipeline, tcp_stream_writer));
    TEST_ASSERT_EQUAL(ESP_OK, audio_pipeline_unregister(pipeline, fatfs_stream_reader));
    TEST_ASSERT_EQUAL(ESP_OK, audio_pipeline_remove_listener(pipeline));
    TEST_ASSERT_EQUAL(ESP_OK, esp_periph_set_stop_all(set));
    TEST_ASSERT_EQUAL(ESP_OK, audio_event_iface_remove_listener(esp_periph_set_get_event_iface(set), evt));
    TEST_ASSERT_EQUAL(ESP_OK, audio_event_iface_destroy(evt));
    TEST_ASSERT_EQUAL(ESP_OK, audio_pipeline_deinit(pipeline));
    TEST_ASSERT_EQUAL(ESP_OK, audio_element_deinit(tcp_stream_writer));
    TEST_ASSERT_EQUAL(ESP_OK, audio_element_deinit(fatfs_stream_reader));
    TEST_ASSERT_EQUAL(ESP_OK, esp_periph_set_destroy(set));
}