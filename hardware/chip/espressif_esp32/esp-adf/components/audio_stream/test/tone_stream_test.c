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
#include "esp_log.h"

#include "audio_pipeline.h"
#include "audio_mem.h"
#include "audio_element.h"
#include "audio_event_iface.h"
#include "tone_stream.h"
#include "fatfs_stream.h"

#include "esp_peripherals.h"
#include "board.h"

const char* tone_uri[] = {
   "flash://tone/0_Bt_Reconnect.mp3",
   "flash://tone/1_Wechat.mp3",
   "flash://tone/2_Welcome_To_Wifi.mp3",
   "flash://tone/3_New_Version_Available.mp3",
   "flash://tone/4_Bt_Success.mp3",
   "flash://tone/5_Freetalk.mp3",
   "flash://tone/6_Upgrade_Done.mp3",
   "flash://tone/7_shutdown.mp3",
   "flash://tone/8_Alarm.mp3",
   "flash://tone/9_Wifi_Success.mp3",
   "flash://tone/10_Under_Smartconfig.mp3",
   "flash://tone/11_Out_Of_Power.mp3",
   "flash://tone/12_server_connect.mp3",
   "flash://tone/13_hello.mp3",
   "flash://tone/14_new_message.mp3",
   "flash://tone/15_Please_Retry_Wifi.mp3",
   "flash://tone/16_please_setting_wifi.mp3",
   "flash://tone/17_Welcome_To_Bt.mp3",
   "flash://tone/18_Wifi_Time_Out.mp3",
   "flash://tone/19_Wifi_Reconnect.mp3",
   "flash://tone/20_server_disconnect.mp3",
};

#define TONE_URL_MAX		21

TEST_CASE("tone stream init memory", "esp-adf-stream")
{
    audio_element_handle_t tone_stream_reader;
    tone_stream_cfg_t tone_cfg = TONE_STREAM_CFG_DEFAULT();
    tone_cfg.type = AUDIO_STREAM_READER;
    
    int cnt = 2000;
    AUDIO_MEM_SHOW("BEFORE HTTP_STREAM_INIT MEMORY TEST");
    while (cnt--) {
        tone_stream_reader = tone_stream_init(&tone_cfg);
        audio_element_deinit(tone_stream_reader);
    }
    AUDIO_MEM_SHOW("AFTER TONE_STREAM_INIT MEMORY TEST");
}

TEST_CASE("tone stream read test", "esp-adf-stream")
{
    audio_pipeline_handle_t pipeline;
    audio_element_handle_t tone_stream_reader, fatfs_stream_writer;

    esp_periph_config_t periph_cfg = DEFAULT_ESP_PERIPH_SET_CONFIG();
    esp_periph_set_handle_t set = esp_periph_set_init(&periph_cfg);
    TEST_ASSERT_NOT_NULL(set);

    TEST_ASSERT_EQUAL(ESP_OK, audio_board_sdcard_init(set, SD_MODE_1_LINE));

    audio_pipeline_cfg_t pipeline_cfg = DEFAULT_AUDIO_PIPELINE_CONFIG();
    pipeline = audio_pipeline_init(&pipeline_cfg);
    TEST_ASSERT_NOT_NULL(pipeline);

    tone_stream_cfg_t tone_cfg = TONE_STREAM_CFG_DEFAULT();
    tone_cfg.type = AUDIO_STREAM_READER;
    tone_stream_reader = tone_stream_init(&tone_cfg);
    TEST_ASSERT_NOT_NULL(tone_stream_reader);

    fatfs_stream_cfg_t fatfs_cfg = FATFS_STREAM_CFG_DEFAULT();
    fatfs_cfg.type = AUDIO_STREAM_WRITER;
    fatfs_stream_writer = fatfs_stream_init(&fatfs_cfg);
    TEST_ASSERT_NOT_NULL(fatfs_stream_writer);

    TEST_ASSERT_EQUAL(ESP_OK, audio_pipeline_register(pipeline, tone_stream_reader, "tone"));
    TEST_ASSERT_EQUAL(ESP_OK, audio_pipeline_register(pipeline, fatfs_stream_writer, "fatfs"));
    TEST_ASSERT_EQUAL(ESP_OK, audio_pipeline_link(pipeline, (const char *[]) {"tone", "fatfs"}, 2));

    uint32_t tone_type = esp_random() % TONE_URL_MAX;
    TEST_ASSERT_EQUAL(ESP_OK, audio_element_set_uri(tone_stream_reader, tone_uri[tone_type]));
    TEST_ASSERT_EQUAL(ESP_OK, audio_element_set_uri(fatfs_stream_writer, "/sdcard/test.mp3"));

    audio_event_iface_cfg_t evt_cfg = AUDIO_EVENT_IFACE_DEFAULT_CFG();
    audio_event_iface_handle_t evt = audio_event_iface_init(&evt_cfg);
    TEST_ASSERT_NOT_NULL(evt);

    TEST_ASSERT_EQUAL(ESP_OK, audio_pipeline_set_listener(pipeline, evt));
    TEST_ASSERT_EQUAL(ESP_OK, audio_pipeline_run(pipeline));

    while (1) {
        audio_event_iface_msg_t msg = { 0 };
        esp_err_t ret = audio_event_iface_listen(evt, &msg, portMAX_DELAY);
        if (ret != ESP_OK) {
            continue;
        }
        if (msg.source_type == AUDIO_ELEMENT_TYPE_ELEMENT && msg.source == (void *) tone_stream_reader
            && msg.cmd == AEL_MSG_CMD_REPORT_STATUS
            && (((int)msg.data == AEL_STATUS_STATE_STOPPED) || ((int)msg.data == AEL_STATUS_STATE_FINISHED))) {
            break;
        }
    }

    TEST_ASSERT_EQUAL(ESP_OK, audio_pipeline_terminate(pipeline));
    TEST_ASSERT_EQUAL(ESP_OK, audio_pipeline_unregister(pipeline, tone_stream_reader));
    TEST_ASSERT_EQUAL(ESP_OK, audio_pipeline_unregister(pipeline, fatfs_stream_writer));
    TEST_ASSERT_EQUAL(ESP_OK, audio_pipeline_remove_listener(pipeline));
    TEST_ASSERT_EQUAL(ESP_OK, audio_event_iface_destroy(evt));
    TEST_ASSERT_EQUAL(ESP_OK, audio_pipeline_deinit(pipeline));
    TEST_ASSERT_EQUAL(ESP_OK, audio_element_deinit(tone_stream_reader));
    TEST_ASSERT_EQUAL(ESP_OK, audio_element_deinit(fatfs_stream_writer));
}