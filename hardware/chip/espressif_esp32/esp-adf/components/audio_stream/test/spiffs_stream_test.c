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

#include "unity.h"
#include "esp_log.h"
#include "esp_err.h"

#include "audio_mem.h"
#include "audio_pipeline.h"
#include "esp_peripherals.h"
#include "spiffs_stream.h"
#include "periph_spiffs.h"

static const char *TAG = "SPIFFS_STREAM_TEST";


static uint64_t get_file_size(const char *name)
{
    FILE *f;
    int size = 0;
    f = fopen(name, "rb");
    if (f == NULL) {
        return -1;
    }
    fseek(f, 0, SEEK_END);
    size = ftell(f);
    fclose(f);
    return size;
}

static void comparison_file_size(const char *file1, const char *file2)
{
    uint64_t size1 = get_file_size(file1);
    TEST_ASSERT_TRUE(size1 >= 0);
    uint64_t size2 = get_file_size(file2);
    TEST_ASSERT_TRUE(size2 >= 0);
    if (size1 == size2) {
        ESP_LOGI(TAG, "spiffs test successful");
    } else {
        ESP_LOGI(TAG, "spiffs test failed");
    }
}

TEST_CASE("spiffs stream init memory test", "esp-adf-stream")
{
    audio_element_handle_t spiffs_stream_reader;
    periph_spiffs_cfg_t spiffs_cfg = {
        .root = "/spiffs",
        .partition_label = NULL,
        .max_files = 5,
        .format_if_mount_failed = true
    };
    AUDIO_MEM_SHOW("BEFORE SPIFFS_STREAM_INIT MEMORY TEST");
    spiffs_stream_cfg_t spiffs_reader_cfg = SPIFFS_STREAM_CFG_DEFAULT();
    spiffs_reader_cfg.type = AUDIO_STREAM_READER;
    int cnt = 2000;
    while (cnt--) {
        spiffs_stream_reader = spiffs_stream_init(&spiffs_reader_cfg);
        audio_element_deinit(spiffs_stream_reader);
    }
    AUDIO_MEM_SHOW("BEFORE SPIFFS_STREAM_INIT MEMORY TEST");
}

/* 
 * Note Before run this unitest, please prepare the following two steps
 * step1 mofify you partition table, For example add "storage,  data, spiffs,  0x110000,1M" in partition table
 * step2 download esp-audio.bin in flash, the download address is 0x110000
 */
TEST_CASE("Spiffs stream read and write loop test", "[esp-adf-stream]")
{
    audio_pipeline_handle_t pipeline;
    audio_element_handle_t spiffs_stream_reader, spiffs_stream_writer;

    esp_log_level_set("AUDIO_PIPELINE", ESP_LOG_DEBUG);
    esp_log_level_set("AUDIO_ELEMENT", ESP_LOG_DEBUG);
    esp_periph_config_t periph_cfg = DEFAULT_ESP_PERIPH_SET_CONFIG();
    esp_periph_set_handle_t set = esp_periph_set_init(&periph_cfg);
    TEST_ASSERT_NOT_NULL(set);

    periph_spiffs_cfg_t spiffs_cfg = {
        .root = "/spiffs",
        .partition_label = NULL,
        .max_files = 5,
        .format_if_mount_failed = true
    };
    esp_periph_handle_t spiffs_handle = periph_spiffs_init(&spiffs_cfg);
    TEST_ASSERT_NOT_NULL(spiffs_handle);

    TEST_ASSERT_EQUAL(ESP_OK, esp_periph_start(set, spiffs_handle));

    while (!periph_spiffs_is_mounted(spiffs_handle)) {
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }

    audio_pipeline_cfg_t pipeline_cfg = DEFAULT_AUDIO_PIPELINE_CONFIG();
    pipeline = audio_pipeline_init(&pipeline_cfg);
    TEST_ASSERT_NOT_NULL(pipeline);

    spiffs_stream_cfg_t spiffs_reader_cfg = SPIFFS_STREAM_CFG_DEFAULT();
    spiffs_reader_cfg.type = AUDIO_STREAM_READER;
    spiffs_stream_reader = spiffs_stream_init(&spiffs_reader_cfg);
    TEST_ASSERT_NOT_NULL(spiffs_stream_reader);

    spiffs_stream_cfg_t fatfs_writer_cfg = SPIFFS_STREAM_CFG_DEFAULT();
    fatfs_writer_cfg.type = AUDIO_STREAM_WRITER;
    spiffs_stream_writer = spiffs_stream_init(&fatfs_writer_cfg);
    TEST_ASSERT_NOT_NULL(spiffs_stream_writer);

    TEST_ASSERT_EQUAL(ESP_OK, audio_pipeline_register(pipeline, spiffs_stream_reader, "spiffs_reader"));
    TEST_ASSERT_EQUAL(ESP_OK, audio_pipeline_register(pipeline, spiffs_stream_writer, "spiffs_writer"));

    TEST_ASSERT_EQUAL(ESP_OK, audio_pipeline_link(pipeline, (const char *[]) {"spiffs_reader", "spiffs_writer"}, 2));

    TEST_ASSERT_EQUAL(ESP_OK, audio_element_set_uri(spiffs_stream_reader, "/spiffs/adf_music.mp3"));
    TEST_ASSERT_EQUAL(ESP_OK, audio_element_set_uri(spiffs_stream_writer, "/spiffs/adf_music_writer.mp3"));

    audio_event_iface_cfg_t evt_cfg = AUDIO_EVENT_IFACE_DEFAULT_CFG();
    audio_event_iface_handle_t evt = audio_event_iface_init(&evt_cfg);
    TEST_ASSERT_NOT_NULL(evt);

    TEST_ASSERT_EQUAL(ESP_OK, audio_pipeline_set_listener(pipeline, evt));

    TEST_ASSERT_EQUAL(ESP_OK, audio_event_iface_set_listener(esp_periph_set_get_event_iface(set), evt));

    TEST_ASSERT_EQUAL(ESP_OK, audio_pipeline_run(pipeline));

    while (1) {
        audio_event_iface_msg_t msg = { 0 };
        esp_err_t ret = audio_event_iface_listen(evt, &msg, portMAX_DELAY);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "[ * ] Event interface error : %d", ret);
            continue;
        }

        if (msg.source_type == AUDIO_ELEMENT_TYPE_ELEMENT && msg.source == (void *) spiffs_stream_reader
            && msg.cmd == AEL_MSG_CMD_REPORT_STATUS
            && (((int)msg.data == AEL_STATUS_STATE_STOPPED) || ((int)msg.data == AEL_STATUS_STATE_FINISHED))) {
            ESP_LOGW(TAG, "[ * ] Stop event received");
            break;
        }
    }

    comparison_file_size("/spiffs/adf_music.mp3", "/spiffs/adf_music_writer.mp3");
    TEST_ASSERT_EQUAL(ESP_OK, audio_pipeline_terminate(pipeline));
    TEST_ASSERT_EQUAL(ESP_OK, audio_pipeline_unregister(pipeline, spiffs_stream_reader));
    TEST_ASSERT_EQUAL(ESP_OK, audio_pipeline_unregister(pipeline, spiffs_stream_writer));
    TEST_ASSERT_EQUAL(ESP_OK, audio_pipeline_remove_listener(pipeline));
    TEST_ASSERT_EQUAL(ESP_OK, esp_periph_set_stop_all(set));
    TEST_ASSERT_EQUAL(ESP_OK, audio_event_iface_remove_listener(esp_periph_set_get_event_iface(set), evt));
    TEST_ASSERT_EQUAL(ESP_OK, audio_event_iface_destroy(evt));
    TEST_ASSERT_EQUAL(ESP_OK, audio_pipeline_deinit(pipeline));
    TEST_ASSERT_EQUAL(ESP_OK, audio_element_deinit(spiffs_stream_reader));
    TEST_ASSERT_EQUAL(ESP_OK, audio_element_deinit(spiffs_stream_writer));
    TEST_ASSERT_EQUAL(ESP_OK, esp_periph_set_destroy(set));
}
