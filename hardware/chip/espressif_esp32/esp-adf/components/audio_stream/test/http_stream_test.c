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
#include "http_stream.h"
#include "i2s_stream.h"
#include "fatfs_stream.h"
#include "aac_decoder.h"

#include "esp_peripherals.h"
#include "periph_wifi.h"
#include "board.h"

static const char *TAG = "HTTP STREAM UNITEST";

static const char URL_RANDOM[] = "0123456789abcdefghijklmnopqrstuvwxyuzABCDEFGHIJKLMNOPQRSTUVWXYUZ-_.!@#$&*()=:/,;?+~";
#define AAC_STREAM_URI "http://open.ls.qingting.fm/live/274/64k.m3u8?format=aac"
#define UNITEST_HTTP_SERVRE_URI  "http://192.168.199.168:8000/upload"

#define UNITETS_HTTP_STREAM_WIFI_SSID    "ESPRESSIF"   
#define UNITETS_HTTP_STREAM_WIFI_PASSWD    "espressif"   

TEST_CASE("http stream init memory", "[esp-adf-stream]")
{
    esp_log_level_set("AUDIO_ELEMENT", ESP_LOG_DEBUG);
    audio_element_handle_t http_stream_reader;
    http_stream_cfg_t http_cfg = HTTP_STREAM_CFG_DEFAULT();
    http_cfg.event_handle = NULL;
    http_cfg.type = AUDIO_STREAM_READER;
    http_cfg.enable_playlist_parser = true;
    int cnt = 2000;
    AUDIO_MEM_SHOW("BEFORE HTTP_STREAM_INIT MEMORY TEST");
    while (cnt--) {
        http_stream_reader = http_stream_init(&http_cfg);
        audio_element_deinit(http_stream_reader);
    }
    AUDIO_MEM_SHOW("AFTER HTTP_STREAM_INIT MEMORY TEST");
}

TEST_CASE("http stream url test", "[esp-adf-stream]")
{
    int url_len = 0;
    int url_real_len = 0;
    char s[2] = {0};
    char url_rand[1024];
    int url_rand_t = strlen(URL_RANDOM);
    char *url = (char *)malloc(1024 + sizeof("http://"));

    audio_element_handle_t http_stream_reader;
    http_stream_cfg_t http_cfg = HTTP_STREAM_CFG_DEFAULT();
    http_cfg.event_handle = NULL;
    http_cfg.type = AUDIO_STREAM_READER;
    http_cfg.enable_playlist_parser = true;
    http_stream_reader = http_stream_init(&http_cfg);

    srand((unsigned int)time((time_t*)NULL));
    for (int cnt = 0; cnt < 100; cnt++) {
        memset(url, 0x00, sizeof("http://"));
        strcpy(url, "http://");
        url_len = (rand() % 1024);
        for ( url_real_len = 0; url_real_len < url_len; url_real_len++) {
            sprintf(s, "%c", URL_RANDOM[rand() % (url_rand_t) ]);
            strcat(url_rand, s);
        }
        strcat(url, url_rand);
        url[url_len + sizeof("http://") - 1] = '\0';
        audio_element_set_uri(http_stream_reader, AAC_STREAM_URI);
    }
    free(url);
    url = NULL;
}

int _http_living_stream_event_handle(http_stream_event_msg_t *msg)
{
    if (msg->event_id == HTTP_STREAM_RESOLVE_ALL_TRACKS) {
        return ESP_OK;
    }

    if (msg->event_id == HTTP_STREAM_FINISH_TRACK) {
        return http_stream_next_track(msg->el);
    }
    if (msg->event_id == HTTP_STREAM_FINISH_PLAYLIST) {
        return http_stream_restart(msg->el);
    }
    return ESP_OK;

}

static esp_err_t _http_stream_event_handle(http_stream_event_msg_t *msg)
{
    esp_http_client_handle_t http = (esp_http_client_handle_t)msg->http_client;
    char len_buf[16];
    static int total_write = 0;

    if (msg->event_id == HTTP_STREAM_PRE_REQUEST) {
        return ESP_OK;
    }

    if (msg->event_id == HTTP_STREAM_ON_REQUEST) {
        // write data
        int wlen = sprintf(len_buf, "%x\r\n", msg->buffer_len);
        if (esp_http_client_write(http, len_buf, wlen) <= 0) {
            return ESP_FAIL;
        }
        if (esp_http_client_write(http, msg->buffer, msg->buffer_len) <= 0) {
            return ESP_FAIL;
        }
        if (esp_http_client_write(http, "\r\n", 2) <= 0) {
            return ESP_FAIL;
        }
        total_write += msg->buffer_len;
        printf("\033[A\33[2K\rTotal bytes written: %d\n", total_write);
        return msg->buffer_len;
    }

    if (msg->event_id == HTTP_STREAM_POST_REQUEST) {
        if (esp_http_client_write(http, "0\r\n\r\n", 5) <= 0) {
            return ESP_FAIL;
        }
        return ESP_OK;
    }
    if (msg->event_id == HTTP_STREAM_FINISH_REQUEST) {
        char *buf = calloc(1, 64);
        assert(buf);
        int read_len = esp_http_client_read(http, buf, 64);
        if (read_len <= 0) {
            free(buf);
            return ESP_FAIL;
        }
        buf[read_len] = 0;
        free(buf);
        return ESP_OK;
    }
    return ESP_OK;
}

TEST_CASE("http stream read", "[esp-adf-stream]")
{
    audio_pipeline_handle_t pipeline;
    audio_element_handle_t http_stream_reader, fatfs_stream_writer;

    esp_log_level_set("AUDIO_PIPELINE", ESP_LOG_DEBUG);
    esp_log_level_set("AUDIO_ELEMENT", ESP_LOG_DEBUG);
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    tcpip_adapter_init();

    esp_periph_config_t periph_cfg = DEFAULT_ESP_PERIPH_SET_CONFIG();
    esp_periph_set_handle_t set = esp_periph_set_init(&periph_cfg);
    TEST_ASSERT_NOT_NULL(set);

    TEST_ASSERT_EQUAL(ESP_OK, audio_board_sdcard_init(set, SD_MODE_1_LINE));

    periph_wifi_cfg_t wifi_cfg = {
        .ssid = UNITETS_HTTP_STREAM_WIFI_SSID,
        .password = UNITETS_HTTP_STREAM_WIFI_SSID,
    };
    esp_periph_handle_t wifi_handle = periph_wifi_init(&wifi_cfg);
    TEST_ASSERT_NOT_NULL(wifi_handle);

    TEST_ASSERT_EQUAL(ESP_OK, esp_periph_start(set, wifi_handle));
    TEST_ASSERT_EQUAL(ESP_OK, periph_wifi_wait_for_connected(wifi_handle, portMAX_DELAY));

    audio_board_handle_t board_handle = audio_board_init();
    TEST_ASSERT_EQUAL(ESP_OK, audio_hal_ctrl_codec(board_handle->audio_hal, AUDIO_HAL_CODEC_MODE_ENCODE, AUDIO_HAL_CTRL_START));

    audio_pipeline_cfg_t pipeline_cfg = DEFAULT_AUDIO_PIPELINE_CONFIG();
    pipeline = audio_pipeline_init(&pipeline_cfg);
    TEST_ASSERT_NOT_NULL(pipeline);

    fatfs_stream_cfg_t fatfs_cfg = FATFS_STREAM_CFG_DEFAULT();
    fatfs_cfg.type = AUDIO_STREAM_WRITER;
    fatfs_stream_writer = fatfs_stream_init(&fatfs_cfg);
    TEST_ASSERT_NOT_NULL(fatfs_stream_writer);
    TEST_ASSERT_EQUAL(ESP_OK, audio_element_set_uri(fatfs_stream_writer, "/sdcard/test.mp3"));

    http_stream_cfg_t http_cfg = HTTP_STREAM_CFG_DEFAULT();
    http_cfg.type = AUDIO_STREAM_READER;
    http_stream_reader = http_stream_init(&http_cfg);
    TEST_ASSERT_NOT_NULL(http_stream_reader);

    TEST_ASSERT_EQUAL(ESP_OK, audio_pipeline_register(pipeline, http_stream_reader, "http"));
    TEST_ASSERT_EQUAL(ESP_OK, audio_pipeline_register(pipeline, fatfs_stream_writer,  "fatfs"));

    TEST_ASSERT_EQUAL(ESP_OK, audio_pipeline_link(pipeline, (const char *[]) { "http", "fatfs" }, 2));

    audio_event_iface_cfg_t evt_cfg = AUDIO_EVENT_IFACE_DEFAULT_CFG();
    audio_event_iface_handle_t evt = audio_event_iface_init(&evt_cfg);

    TEST_ASSERT_EQUAL(ESP_OK, audio_pipeline_set_listener(pipeline, evt));

    TEST_ASSERT_EQUAL(ESP_OK, audio_event_iface_set_listener(esp_periph_set_get_event_iface(set), evt));

    TEST_ASSERT_EQUAL(ESP_OK, audio_element_set_uri(http_stream_reader, "https://dl.espressif.com/dl/audio/ff-16b-2c-44100hz.mp3"));
    TEST_ASSERT_EQUAL(ESP_OK, audio_pipeline_run(pipeline));

    while (1) {
        audio_event_iface_msg_t msg;
        esp_err_t ret = audio_event_iface_listen(evt, &msg, portMAX_DELAY);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "[ * ] Event interface error : %d", ret);
            continue;
        }

      if (msg.source_type == AUDIO_ELEMENT_TYPE_ELEMENT && msg.source == (void *) http_stream_reader
            && msg.cmd == AEL_MSG_CMD_REPORT_STATUS && (int) msg.data == AEL_STATUS_ERROR_OPEN) {
                break;
            continue;
        }
    }
    TEST_ASSERT_EQUAL(ESP_OK, audio_pipeline_terminate(pipeline));
    TEST_ASSERT_EQUAL(ESP_OK, audio_pipeline_unregister(pipeline, http_stream_reader));
    TEST_ASSERT_EQUAL(ESP_OK, audio_pipeline_unregister(pipeline, fatfs_stream_writer));
    TEST_ASSERT_EQUAL(ESP_OK, audio_pipeline_remove_listener(pipeline));
    TEST_ASSERT_EQUAL(ESP_OK, esp_periph_set_stop_all(set));
    TEST_ASSERT_EQUAL(ESP_OK, audio_event_iface_remove_listener(esp_periph_set_get_event_iface(set), evt));
    TEST_ASSERT_EQUAL(ESP_OK, audio_event_iface_destroy(evt));
    TEST_ASSERT_EQUAL(ESP_OK, audio_pipeline_deinit(pipeline));
    TEST_ASSERT_EQUAL(ESP_OK, audio_element_deinit(http_stream_reader));
    TEST_ASSERT_EQUAL(ESP_OK, audio_element_deinit(fatfs_stream_writer));
    TEST_ASSERT_EQUAL(ESP_OK, esp_periph_set_destroy(set));
}

/* 
 * Note : Before run this unitest, please run the http_server_read.py, and Confirm server ip in UNITEST_HTTP_SERVRE_URI
 */
TEST_CASE("http stream write", "[esp-adf-stream]")
{
    esp_log_level_set("AUDIO_PIPELINE", ESP_LOG_DEBUG);
    esp_log_level_set("AUDIO_ELEMENT", ESP_LOG_DEBUG);
    audio_pipeline_handle_t pipeline;
    audio_element_handle_t http_stream_writer, fatfs_stream_reader;

    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    tcpip_adapter_init();

    esp_periph_config_t periph_cfg = DEFAULT_ESP_PERIPH_SET_CONFIG();
    esp_periph_set_handle_t set = esp_periph_set_init(&periph_cfg);
    TEST_ASSERT_NOT_NULL(set);

    TEST_ASSERT_EQUAL(ESP_OK, audio_board_sdcard_init(set, SD_MODE_1_LINE));

    periph_wifi_cfg_t wifi_cfg = {
        .ssid = UNITETS_HTTP_STREAM_WIFI_SSID,
        .password = UNITETS_HTTP_STREAM_WIFI_PASSWD,
    };
    esp_periph_handle_t wifi_handle = periph_wifi_init(&wifi_cfg);
    TEST_ASSERT_NOT_NULL(wifi_handle);

    TEST_ASSERT_EQUAL(ESP_OK, esp_periph_start(set, wifi_handle));
    TEST_ASSERT_EQUAL(ESP_OK, periph_wifi_wait_for_connected(wifi_handle, portMAX_DELAY));

    audio_board_handle_t board_handle = audio_board_init();
    TEST_ASSERT_EQUAL(ESP_OK, audio_hal_ctrl_codec(board_handle->audio_hal, AUDIO_HAL_CODEC_MODE_ENCODE, AUDIO_HAL_CTRL_START));

    audio_pipeline_cfg_t pipeline_cfg = DEFAULT_AUDIO_PIPELINE_CONFIG();
    pipeline = audio_pipeline_init(&pipeline_cfg);
    TEST_ASSERT_NOT_NULL(pipeline);


    fatfs_stream_cfg_t fatfs_cfg = FATFS_STREAM_CFG_DEFAULT();
    fatfs_cfg.type = AUDIO_STREAM_READER;
    fatfs_stream_reader = fatfs_stream_init(&fatfs_cfg);
    TEST_ASSERT_EQUAL(ESP_OK, audio_element_set_uri(fatfs_stream_reader, "/sdcard/test.mp3"));

    http_stream_cfg_t http_cfg = HTTP_STREAM_CFG_DEFAULT();
    http_cfg.type = AUDIO_STREAM_WRITER;
    http_cfg.event_handle = _http_stream_event_handle;
    http_stream_writer = http_stream_init(&http_cfg);
    TEST_ASSERT_NOT_NULL(http_stream_writer);

    TEST_ASSERT_EQUAL(ESP_OK, audio_pipeline_register(pipeline, fatfs_stream_reader,  "fatfs"));
    TEST_ASSERT_EQUAL(ESP_OK, audio_pipeline_register(pipeline, http_stream_writer, "http"));

    TEST_ASSERT_EQUAL(ESP_OK, audio_pipeline_link(pipeline, (const char *[]) {"fatfs", "http"}, 2));

    audio_event_iface_cfg_t evt_cfg = AUDIO_EVENT_IFACE_DEFAULT_CFG();
    audio_event_iface_handle_t evt = audio_event_iface_init(&evt_cfg);

    TEST_ASSERT_EQUAL(ESP_OK, audio_pipeline_set_listener(pipeline, evt));

    TEST_ASSERT_EQUAL(ESP_OK, audio_event_iface_set_listener(esp_periph_set_get_event_iface(set), evt));

    TEST_ASSERT_EQUAL(ESP_OK, audio_element_set_uri(http_stream_writer, UNITEST_HTTP_SERVRE_URI));
    TEST_ASSERT_EQUAL(ESP_OK, audio_pipeline_run(pipeline));

    while (1) {
        audio_event_iface_msg_t msg;
        esp_err_t ret = audio_event_iface_listen(evt, &msg, portMAX_DELAY);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "[ * ] Event interface error : %d", ret);
            continue;
        }

      if (msg.source_type == AUDIO_ELEMENT_TYPE_ELEMENT && msg.source == (void *) http_stream_writer
            && msg.cmd == AEL_MSG_CMD_REPORT_STATUS && (int) msg.data == AEL_STATUS_ERROR_OPEN) {
                break;
            continue;
        }
    }
    TEST_ASSERT_EQUAL(ESP_OK, audio_pipeline_terminate(pipeline));
    TEST_ASSERT_EQUAL(ESP_OK, audio_pipeline_unregister(pipeline, http_stream_writer));
    TEST_ASSERT_EQUAL(ESP_OK, audio_pipeline_unregister(pipeline, fatfs_stream_reader));
    TEST_ASSERT_EQUAL(ESP_OK, audio_pipeline_remove_listener(pipeline));
    TEST_ASSERT_EQUAL(ESP_OK, esp_periph_set_stop_all(set));
    TEST_ASSERT_EQUAL(ESP_OK, audio_event_iface_remove_listener(esp_periph_set_get_event_iface(set), evt));
    TEST_ASSERT_EQUAL(ESP_OK, audio_event_iface_destroy(evt));
    TEST_ASSERT_EQUAL(ESP_OK, audio_pipeline_deinit(pipeline));
    TEST_ASSERT_EQUAL(ESP_OK, audio_element_deinit(http_stream_writer));
    TEST_ASSERT_EQUAL(ESP_OK, audio_element_deinit(fatfs_stream_reader));

    TEST_ASSERT_EQUAL(ESP_OK,esp_periph_set_destroy(set));
}

TEST_CASE("http stream living test", "[esp-adf-stream]")
{
    esp_log_level_set("AUDIO_PIPELINE", ESP_LOG_DEBUG);
    esp_log_level_set("AUDIO_ELEMENT", ESP_LOG_DEBUG);
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES) {
        TEST_ASSERT_EQUAL(ESP_OK, nvs_flash_erase());
        err = nvs_flash_init();
    }
    tcpip_adapter_init();

    audio_pipeline_handle_t pipeline;
    audio_element_handle_t http_stream_reader, i2s_stream_writer, aac_decoder;

    audio_board_handle_t board_handle = audio_board_init();
    TEST_ASSERT_NOT_NULL(board_handle);
    TEST_ASSERT_EQUAL(ESP_OK,audio_hal_ctrl_codec(board_handle->audio_hal, AUDIO_HAL_CODEC_MODE_DECODE, AUDIO_HAL_CTRL_START));

    audio_pipeline_cfg_t pipeline_cfg = DEFAULT_AUDIO_PIPELINE_CONFIG();
    pipeline = audio_pipeline_init(&pipeline_cfg);
    TEST_ASSERT_NOT_NULL(pipeline);

    http_stream_cfg_t http_cfg = HTTP_STREAM_CFG_DEFAULT();
    http_cfg.event_handle = _http_living_stream_event_handle;
    http_cfg.type = AUDIO_STREAM_READER;
    http_cfg.enable_playlist_parser = true;
    http_stream_reader = http_stream_init(&http_cfg);
    TEST_ASSERT_NOT_NULL(http_stream_reader);

    i2s_stream_cfg_t i2s_cfg = I2S_STREAM_CFG_DEFAULT();
    i2s_cfg.type = AUDIO_STREAM_WRITER;
    i2s_stream_writer = i2s_stream_init(&i2s_cfg);
    TEST_ASSERT_NOT_NULL(i2s_stream_writer);

    aac_decoder_cfg_t aac_cfg = DEFAULT_AAC_DECODER_CONFIG();
    aac_decoder = aac_decoder_init(&aac_cfg);
    TEST_ASSERT_NOT_NULL(aac_decoder);

    TEST_ASSERT_EQUAL(ESP_OK, audio_pipeline_register(pipeline, http_stream_reader, "http"));
    TEST_ASSERT_EQUAL(ESP_OK, audio_pipeline_register(pipeline, aac_decoder, "aac"));
    TEST_ASSERT_EQUAL(ESP_OK, audio_pipeline_register(pipeline, i2s_stream_writer,  "i2s"));
    TEST_ASSERT_EQUAL(ESP_OK, audio_pipeline_link(pipeline, (const char *[]) {"http",  "aac", "i2s"}, 3));

    TEST_ASSERT_EQUAL(ESP_OK, audio_element_set_uri(http_stream_reader, AAC_STREAM_URI));

    esp_periph_config_t periph_cfg = DEFAULT_ESP_PERIPH_SET_CONFIG();
    esp_periph_set_handle_t set = esp_periph_set_init(&periph_cfg);
    TEST_ASSERT_NOT_NULL(set);
    periph_wifi_cfg_t wifi_cfg = {
        .ssid = UNITETS_HTTP_STREAM_WIFI_SSID,
        .password = UNITETS_HTTP_STREAM_WIFI_PASSWD,
    };
    esp_periph_handle_t wifi_handle = periph_wifi_init(&wifi_cfg);
    TEST_ASSERT_NOT_NULL(wifi_handle);
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

        if (msg.source_type == AUDIO_ELEMENT_TYPE_ELEMENT
            && msg.source == (void *) aac_decoder
            && msg.cmd == AEL_MSG_CMD_REPORT_MUSIC_INFO) {
            audio_element_info_t music_info = {0};
            TEST_ASSERT_EQUAL(ESP_OK, audio_element_getinfo(aac_decoder, &music_info));

            ESP_LOGI(TAG, "[ * ] Receive music info from aac decoder, sample_rates=%d, bits=%d, ch=%d",
                     music_info.sample_rates, music_info.bits, music_info.channels);

            TEST_ASSERT_EQUAL(ESP_OK, audio_element_setinfo(i2s_stream_writer, &music_info));
            TEST_ASSERT_EQUAL(ESP_OK, i2s_stream_set_clk(i2s_stream_writer, music_info.sample_rates, music_info.bits, music_info.channels));
            continue;
        }

        /* restart stream when the first pipeline element (http_stream_reader in this case) receives stop event (caused by reading errors) */
        if (msg.source_type == AUDIO_ELEMENT_TYPE_ELEMENT && msg.source == (void *) http_stream_reader
            && msg.cmd == AEL_MSG_CMD_REPORT_STATUS && (int) msg.data == AEL_STATUS_ERROR_OPEN) {
            ESP_LOGW(TAG, "[ * ] Restart stream");
            TEST_ASSERT_EQUAL(ESP_OK, audio_pipeline_stop(pipeline));
            TEST_ASSERT_EQUAL(ESP_OK, audio_pipeline_wait_for_stop(pipeline));
            TEST_ASSERT_EQUAL(ESP_OK, audio_element_reset_state(aac_decoder));
            TEST_ASSERT_EQUAL(ESP_OK, audio_element_reset_state(i2s_stream_writer));
            TEST_ASSERT_EQUAL(ESP_OK, audio_pipeline_reset_ringbuffer(pipeline));
            TEST_ASSERT_EQUAL(ESP_OK, audio_pipeline_reset_items_state(pipeline));
            TEST_ASSERT_EQUAL(ESP_OK, audio_pipeline_run(pipeline));
            continue;
        }
    }

    TEST_ASSERT_EQUAL(ESP_OK, audio_pipeline_terminate(pipeline));
    TEST_ASSERT_EQUAL(ESP_OK, audio_pipeline_unregister(pipeline, http_stream_reader));
    TEST_ASSERT_EQUAL(ESP_OK, audio_pipeline_unregister(pipeline, i2s_stream_writer));
    TEST_ASSERT_EQUAL(ESP_OK, audio_pipeline_unregister(pipeline, aac_decoder));
    TEST_ASSERT_EQUAL(ESP_OK, audio_pipeline_remove_listener(pipeline));
    TEST_ASSERT_EQUAL(ESP_OK, esp_periph_set_stop_all(set));
    TEST_ASSERT_EQUAL(ESP_OK, audio_event_iface_remove_listener(esp_periph_set_get_event_iface(set), evt));
    TEST_ASSERT_EQUAL(ESP_OK, audio_event_iface_destroy(evt));
    TEST_ASSERT_EQUAL(ESP_OK, audio_pipeline_deinit(pipeline));
    TEST_ASSERT_EQUAL(ESP_OK, audio_element_deinit(http_stream_reader));
    TEST_ASSERT_EQUAL(ESP_OK, audio_element_deinit(i2s_stream_writer));
    TEST_ASSERT_EQUAL(ESP_OK, audio_element_deinit(aac_decoder));
    TEST_ASSERT_EQUAL(ESP_OK, esp_periph_set_destroy(set));
}


