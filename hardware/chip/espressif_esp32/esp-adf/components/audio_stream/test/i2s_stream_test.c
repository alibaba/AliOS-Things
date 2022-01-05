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
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "sdkconfig.h"
#include "audio_element.h"
#include "audio_pipeline.h"
#include "audio_event_iface.h"
#include "audio_common.h"
#include "fatfs_stream.h"
#include "i2s_stream.h"
#include "mp3_decoder.h"
#include "esp_peripherals.h"
#include "periph_sdcard.h"
#include "audio_alc.h"
#include "audio_mem.h"
#include "board.h"


static const char *TAG = "USE_ALC_EXAMPLE";

typedef struct {
    int rate;
    int bits;
    int ch;
} unitest_i2s_clk__t;

static unitest_i2s_clk__t unitest_i2s_clk[] = {
    {8000, 16, 1},
    {8000, 32, 1},
    {8000, 32, 2},
    {16000, 16, 1},
    {16000, 32, 1},
    {16000, 32, 2},
    {32000, 16, 1},
    {32000, 32, 1},
    {32000, 32, 2},
    {48000, 16, 1},
    {48000, 16, 2},
    {48000, 32, 2},
    {48000, 32, 2},
};

static void i2s_volume_alc_task(void *pvParamters)
{
    int volume_val = -20;
    int vol = 0;
    TEST_ASSERT_NOT_NULL(pvParamters);
    audio_element_handle_t i2s_stream = (audio_element_handle_t)pvParamters;
    while (1)
    {
        volume_val += 1;
        if (volume_val > 20) {
            volume_val  = -20;
        }
        ESP_LOGI(TAG, "set vol is %d\n", volume_val);
        TEST_ASSERT_EQUAL(ESP_OK, i2s_alc_volume_set(i2s_stream, volume_val));
        TEST_ASSERT_EQUAL(ESP_OK, i2s_alc_volume_get(i2s_stream, &vol));
        ESP_LOGI(TAG, "get vol is %d\n", vol);
        vTaskDelay(200 / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}

TEST_CASE("i2s stream init memory test", "[esp-adf-stream]")
{
    audio_element_handle_t i2s_stream_writer;
    i2s_stream_cfg_t i2s_cfg = I2S_STREAM_CFG_DEFAULT();
    i2s_cfg.type = AUDIO_STREAM_WRITER;
    int cnt = 2000;
    AUDIO_MEM_SHOW("BEFORE I2S_STREAM_INIT MEMORY TEST");
    while (cnt--) {
        i2s_stream_writer = i2s_stream_init(&i2s_cfg);
        audio_element_deinit(i2s_stream_writer);
    }
    AUDIO_MEM_SHOW("AFTER I2S_STREAM_INIT MEMORY TEST");
}

TEST_CASE("i2s_stream alc test", "[esp-adf-stream]")
{
    audio_pipeline_handle_t pipeline;
    audio_element_handle_t fatfs_stream_reader, i2s_stream_writer, mp3_decoder;

    esp_log_level_set("AUDIO_PIPELINE", ESP_LOG_DEBUG);
    esp_log_level_set("AUDIO_ELEMENT", ESP_LOG_DEBUG);
    esp_periph_config_t periph_cfg = DEFAULT_ESP_PERIPH_SET_CONFIG();
    esp_periph_set_handle_t set = esp_periph_set_init(&periph_cfg);
    TEST_ASSERT_NOT_NULL(set);

    TEST_ASSERT_EQUAL(ESP_OK, audio_board_sdcard_init(set, SD_MODE_1_LINE));

    audio_board_handle_t board_handle = audio_board_init();
    TEST_ASSERT_EQUAL(ESP_OK, audio_hal_ctrl_codec(board_handle->audio_hal, AUDIO_HAL_CODEC_MODE_DECODE, AUDIO_HAL_CTRL_START));

    audio_pipeline_cfg_t pipeline_cfg = DEFAULT_AUDIO_PIPELINE_CONFIG();
    pipeline = audio_pipeline_init(&pipeline_cfg);
    TEST_ASSERT_NOT_NULL(pipeline);

    fatfs_stream_cfg_t fatfs_cfg = FATFS_STREAM_CFG_DEFAULT();
    fatfs_cfg.type = AUDIO_STREAM_READER;
    fatfs_stream_reader = fatfs_stream_init(&fatfs_cfg);
    TEST_ASSERT_NOT_NULL(fatfs_stream_reader);

    i2s_stream_cfg_t i2s_cfg = I2S_STREAM_CFG_DEFAULT();
    i2s_cfg.type = AUDIO_STREAM_WRITER;

    i2s_cfg.use_alc = true;
    i2s_stream_writer = i2s_stream_init(&i2s_cfg);
    TEST_ASSERT_NOT_NULL(fatfs_stream_reader);

    mp3_decoder_cfg_t mp3_cfg = DEFAULT_MP3_DECODER_CONFIG();
    mp3_decoder = mp3_decoder_init(&mp3_cfg);
    TEST_ASSERT_NOT_NULL(mp3_decoder);

    TEST_ASSERT_EQUAL(ESP_OK, audio_pipeline_register(pipeline, fatfs_stream_reader, "file"));
    TEST_ASSERT_EQUAL(ESP_OK, audio_pipeline_register(pipeline, mp3_decoder, "mp3"));
    TEST_ASSERT_EQUAL(ESP_OK, audio_pipeline_register(pipeline, i2s_stream_writer, "i2s"));
    TEST_ASSERT_EQUAL(ESP_OK, audio_pipeline_link(pipeline, (const char *[]) {"file", "mp3", "i2s"}, 3));

    TEST_ASSERT_EQUAL(ESP_OK, audio_element_set_uri(fatfs_stream_reader, "/sdcard/test.mp3"));

    audio_event_iface_cfg_t evt_cfg = AUDIO_EVENT_IFACE_DEFAULT_CFG();
    audio_event_iface_handle_t evt = audio_event_iface_init(&evt_cfg);
    TEST_ASSERT_NOT_NULL(evt);

    TEST_ASSERT_EQUAL(ESP_OK, audio_pipeline_set_listener(pipeline, evt));

    TEST_ASSERT_EQUAL(ESP_OK, audio_event_iface_set_listener(esp_periph_set_get_event_iface(set), evt));

    TEST_ASSERT_EQUAL(ESP_OK, audio_pipeline_run(pipeline));

    xTaskCreate(i2s_volume_alc_task, "i2s_volume_alc_task", 4096, (void *)i2s_stream_writer, 9, NULL);

    while (1) {
        audio_event_iface_msg_t msg;
        esp_err_t ret = audio_event_iface_listen(evt, &msg, portMAX_DELAY);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "[ * ] Event interface error : %d", ret);
            continue;
        }

        if (msg.source_type == AUDIO_ELEMENT_TYPE_ELEMENT && msg.source == (void *)mp3_decoder
            && msg.cmd == AEL_MSG_CMD_REPORT_MUSIC_INFO) {
            audio_element_info_t music_info = {0};
            TEST_ASSERT_EQUAL(ESP_OK, audio_element_getinfo(mp3_decoder, &music_info));
            TEST_ASSERT_EQUAL(ESP_OK, audio_element_setinfo(i2s_stream_writer, &music_info));

            TEST_ASSERT_EQUAL(ESP_OK, i2s_stream_set_clk(i2s_stream_writer, music_info.sample_rates, music_info.bits,
                               music_info.channels));
            continue;
        }
        if (msg.source_type == AUDIO_ELEMENT_TYPE_ELEMENT && msg.source == (void *)i2s_stream_writer
            && msg.cmd == AEL_MSG_CMD_REPORT_STATUS && (int)msg.data == AEL_STATUS_STATE_STOPPED) {
            ESP_LOGW(TAG, "[ * ] Stop event received");
            break;
        }
    }

    TEST_ASSERT_EQUAL(ESP_OK, audio_pipeline_terminate(pipeline));
    TEST_ASSERT_EQUAL(ESP_OK, audio_pipeline_unregister(pipeline, fatfs_stream_reader));
    TEST_ASSERT_EQUAL(ESP_OK, audio_pipeline_unregister(pipeline, i2s_stream_writer));
    TEST_ASSERT_EQUAL(ESP_OK, audio_pipeline_unregister(pipeline, mp3_decoder));
    TEST_ASSERT_EQUAL(ESP_OK, audio_pipeline_remove_listener(pipeline));
    TEST_ASSERT_EQUAL(ESP_OK, esp_periph_set_stop_all(set));
    TEST_ASSERT_EQUAL(ESP_OK, audio_event_iface_remove_listener(esp_periph_set_get_event_iface(set), evt));
    TEST_ASSERT_EQUAL(ESP_OK, audio_event_iface_destroy(evt));
    TEST_ASSERT_EQUAL(ESP_OK, audio_pipeline_deinit(pipeline));
    TEST_ASSERT_EQUAL(ESP_OK, audio_element_deinit(fatfs_stream_reader));
    TEST_ASSERT_EQUAL(ESP_OK, audio_element_deinit(i2s_stream_writer));
    TEST_ASSERT_EQUAL(ESP_OK, audio_element_deinit(mp3_decoder));
    TEST_ASSERT_EQUAL(ESP_OK, esp_periph_set_destroy(set));
}

TEST_CASE("i2s_stream write_read_loop", "[esp-adf-stream]")
{
    audio_pipeline_handle_t pipeline;
    audio_element_handle_t i2s_stream_reader, i2s_stream_writer;

    audio_board_handle_t board_handle = audio_board_init();
    audio_hal_ctrl_codec(board_handle->audio_hal, AUDIO_HAL_CODEC_MODE_ENCODE, AUDIO_HAL_CTRL_START);

    audio_pipeline_cfg_t pipeline_cfg = DEFAULT_AUDIO_PIPELINE_CONFIG();
    pipeline = audio_pipeline_init(&pipeline_cfg);
    TEST_ASSERT_NOT_NULL(pipeline);

    i2s_stream_cfg_t i2s_cfg_read = I2S_STREAM_CFG_DEFAULT();
    i2s_cfg_read.type = AUDIO_STREAM_READER;
    i2s_stream_reader = i2s_stream_init(&i2s_cfg_read);
    TEST_ASSERT_NOT_NULL(i2s_stream_reader);

    i2s_stream_cfg_t i2s_cfg_writer = I2S_STREAM_CFG_DEFAULT();
    i2s_cfg_writer.type = AUDIO_STREAM_WRITER;
    i2s_stream_writer = i2s_stream_init(&i2s_cfg_writer);
    TEST_ASSERT_NOT_NULL(i2s_stream_writer);

    TEST_ASSERT_EQUAL(ESP_OK, audio_pipeline_register(pipeline, i2s_stream_reader, "i2s_read"));
    TEST_ASSERT_EQUAL(ESP_OK, audio_pipeline_register(pipeline, i2s_stream_writer, "i2s_write"));
    TEST_ASSERT_EQUAL(ESP_OK, audio_pipeline_link(pipeline, (const char *[]) {"i2s_read", "i2s_write"}, 2));

    TEST_ASSERT_EQUAL(ESP_OK, audio_pipeline_run(pipeline));
    
    for (size_t i = 10; i > 0; i--) {
        ESP_LOGI(TAG, "test time: %d", i);
    }
    
    TEST_ASSERT_EQUAL(ESP_OK, audio_pipeline_terminate(pipeline));
    TEST_ASSERT_EQUAL(ESP_OK, audio_pipeline_unregister(pipeline, i2s_stream_reader));
    TEST_ASSERT_EQUAL(ESP_OK, audio_pipeline_unregister(pipeline, i2s_stream_writer));
    TEST_ASSERT_EQUAL(ESP_OK, audio_pipeline_remove_listener(pipeline));
    TEST_ASSERT_EQUAL(ESP_OK, audio_pipeline_deinit(pipeline));
    TEST_ASSERT_EQUAL(ESP_OK, audio_element_deinit(i2s_stream_reader));
    TEST_ASSERT_EQUAL(ESP_OK, audio_element_deinit(i2s_stream_writer));
    ESP_LOGI(TAG, "finished the [i2s read write loop test]");
}

TEST_CASE("i2s_stream_clk", "[esp-adf-stream]")
{

    audio_element_handle_t i2s_stream_reader;

    i2s_stream_cfg_t i2s_cfg = I2S_STREAM_CFG_DEFAULT();
    i2s_cfg.type = AUDIO_STREAM_READER;
    i2s_stream_reader = i2s_stream_init(&i2s_cfg); 
    TEST_ASSERT_NOT_NULL(i2s_stream_reader);

    for (size_t i = 0; i < (sizeof(unitest_i2s_clk)/sizeof(unitest_i2s_clk[0])); i++) {
        ESP_LOGI(TAG, "rate is %d, bits is %d, ch is %d", unitest_i2s_clk[i].rate, unitest_i2s_clk[i].bits, unitest_i2s_clk[i].ch);
        TEST_ASSERT_EQUAL(ESP_OK, i2s_stream_set_clk(i2s_stream_reader, unitest_i2s_clk[i].rate, unitest_i2s_clk[i].bits, unitest_i2s_clk[i].ch));
    }
}