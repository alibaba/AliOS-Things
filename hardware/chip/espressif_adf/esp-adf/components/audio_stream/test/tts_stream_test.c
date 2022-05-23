/*
 * ESPRESSIF MIT License
 *
 * Copyright (c) 2020 <ESPRESSIF SYSTEMS (SHANGHAI) CO., LTD>
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

#include "unity.h"
#include "esp_err.h"
#include "esp_log.h"

#include "audio_pipeline.h"
#include "audio_mem.h"
#include "tts_stream.h"
#include "i2s_stream.h"

#include "esp_peripherals.h"
#include "board.h"

static void init_tts_stream_test(audio_pipeline_handle_t *pipeline,
                                    audio_element_handle_t *tts_stream_reader,
                                    audio_element_handle_t *i2s_stream_writer,
                                    esp_periph_set_handle_t *set)
{
    esp_periph_config_t periph_cfg = DEFAULT_ESP_PERIPH_SET_CONFIG();
    *set = esp_periph_set_init(&periph_cfg);
    TEST_ASSERT_NOT_NULL(*set);

    audio_board_handle_t board_handle = audio_board_init();
    audio_hal_ctrl_codec(board_handle->audio_hal, AUDIO_HAL_CODEC_MODE_DECODE, AUDIO_HAL_CTRL_START);

    audio_pipeline_cfg_t pipeline_cfg = DEFAULT_AUDIO_PIPELINE_CONFIG();
    *pipeline = audio_pipeline_init(&pipeline_cfg);
    TEST_ASSERT_NOT_NULL(*pipeline);

    tts_stream_cfg_t tts_cfg = TTS_STREAM_CFG_DEFAULT();
    tts_cfg.type = AUDIO_STREAM_READER;
    *tts_stream_reader = tts_stream_init(&tts_cfg);
    TEST_ASSERT_NOT_NULL(*tts_stream_reader);

    i2s_stream_cfg_t i2s_cfg = I2S_STREAM_CFG_DEFAULT();
    i2s_cfg.type = AUDIO_STREAM_WRITER;
    i2s_cfg.i2s_config.sample_rate = 16000;
    i2s_cfg.i2s_config.channel_format = I2S_CHANNEL_FMT_ONLY_LEFT;
    *i2s_stream_writer = i2s_stream_init(&i2s_cfg);
    TEST_ASSERT_NOT_NULL(*i2s_stream_writer);

    audio_pipeline_register(*pipeline, *tts_stream_reader, "tts");
    audio_pipeline_register(*pipeline, *i2s_stream_writer, "i2s");
    const char *link_tag[2] = {"tts", "i2s"};
    audio_pipeline_link(*pipeline, &link_tag[0], 2);
}

static void init_audio_event_test(audio_pipeline_handle_t *pipeline,
                                    esp_periph_set_handle_t *set,
                                    audio_event_iface_handle_t *evt)
{
    audio_event_iface_cfg_t evt_cfg = AUDIO_EVENT_IFACE_DEFAULT_CFG();
    *evt = audio_event_iface_init(&evt_cfg);
    TEST_ASSERT_EQUAL(ESP_OK,audio_pipeline_set_listener(*pipeline, *evt));
    TEST_ASSERT_EQUAL(ESP_OK,audio_event_iface_set_listener(esp_periph_set_get_event_iface(*set), *evt));
    TEST_ASSERT_EQUAL(ESP_OK,audio_pipeline_run(*pipeline));
}

static void deinit_tts_stream_test(audio_pipeline_handle_t *pipeline,
                                    audio_element_handle_t *tts_stream_reader,
                                    audio_element_handle_t *i2s_stream_writer,
                                    esp_periph_set_handle_t *set,
                                    audio_event_iface_handle_t *evt)
{
    TEST_ASSERT_EQUAL(ESP_OK, audio_pipeline_wait_for_stop(*pipeline));
    TEST_ASSERT_EQUAL(ESP_OK,audio_pipeline_terminate(*pipeline));
    TEST_ASSERT_EQUAL(ESP_OK,audio_pipeline_unregister(*pipeline, *tts_stream_reader));
    TEST_ASSERT_EQUAL(ESP_OK,audio_pipeline_unregister(*pipeline, *i2s_stream_writer));
    TEST_ASSERT_EQUAL(ESP_OK,audio_pipeline_remove_listener(*pipeline));
    TEST_ASSERT_EQUAL(ESP_OK,esp_periph_set_stop_all(*set));
    TEST_ASSERT_EQUAL(ESP_OK,audio_event_iface_remove_listener(esp_periph_set_get_event_iface(*set), *evt));
    TEST_ASSERT_EQUAL(ESP_OK,audio_event_iface_destroy(*evt));
    TEST_ASSERT_EQUAL(ESP_OK,audio_pipeline_deinit(*pipeline));
    TEST_ASSERT_EQUAL(ESP_OK,audio_element_deinit(*tts_stream_reader));
    TEST_ASSERT_EQUAL(ESP_OK,audio_element_deinit(*i2s_stream_writer));
    TEST_ASSERT_EQUAL(ESP_OK,esp_periph_set_destroy(*set));
}

TEST_CASE("tts stream init and deinit memory", "[esp-adf-stream]")
{
    AUDIO_MEM_SHOW("TTS STREAM INIT AND DEINIT MEMORY");
    esp_log_level_set("AUDIO_ELEMENT", ESP_LOG_DEBUG);
    audio_element_handle_t tts_stream_reader;
    tts_stream_cfg_t tts_cfg = TTS_STREAM_CFG_DEFAULT();
    tts_cfg.type = AUDIO_STREAM_READER;
    int cnt = 20;
    AUDIO_MEM_SHOW("BEFORE TTS_STREAM_INIT MEMORY TEST");
    while (cnt--) {
        tts_stream_reader = tts_stream_init(&tts_cfg);

        audio_element_deinit(tts_stream_reader);
    }
    AUDIO_MEM_SHOW("AFTER TTS_STREAM_INIT MEMORY TEST");
}

TEST_CASE("tts stream set and read strings", "[esp-adf-stream]")
{
    AUDIO_MEM_SHOW("TTS STREAM SET AND READ STRINGS");
    static const char *STRINGS = "乐鑫语音开源框架简称ADF";
    esp_log_level_set("AUDIO_ELEMENT", ESP_LOG_DEBUG);
    audio_element_handle_t tts_stream_reader;
    tts_stream_cfg_t tts_cfg = TTS_STREAM_CFG_DEFAULT();
    tts_cfg.type = AUDIO_STREAM_READER;
    tts_stream_reader = tts_stream_init(&tts_cfg);
    tts_stream_set_strings(tts_stream_reader, STRINGS);
    char *uri = audio_element_get_uri(tts_stream_reader);
    TEST_ASSERT_EQUAL_STRING(STRINGS, uri);
    audio_element_deinit(tts_stream_reader);
}

TEST_CASE("tts stream speed write and read test", "[esp-adf-stream]")
{
    AUDIO_MEM_SHOW("TTS STREAM SPEED WRITE AND READ TEST");
    audio_element_handle_t tts_stream_reader;
    tts_voice_speed_t speed;
    esp_log_level_set("AUDIO_ELEMENT", ESP_LOG_DEBUG);
    tts_stream_cfg_t tts_cfg = TTS_STREAM_CFG_DEFAULT();
    tts_cfg.type = AUDIO_STREAM_READER;
    tts_stream_reader = tts_stream_init(&tts_cfg);
    TEST_ASSERT_NOT_NULL(tts_stream_reader);
    TEST_ASSERT_EQUAL(ESP_OK, tts_stream_set_speed(tts_stream_reader, TTS_VOICE_SPEED_0));
    TEST_ASSERT_EQUAL(ESP_OK, tts_stream_get_speed(tts_stream_reader, &speed));
    TEST_ASSERT_EQUAL(TTS_VOICE_SPEED_0, speed);
    TEST_ASSERT_EQUAL(ESP_OK, tts_stream_set_speed(tts_stream_reader, TTS_VOICE_SPEED_1));
    TEST_ASSERT_EQUAL(ESP_OK, tts_stream_get_speed(tts_stream_reader, &speed));
    TEST_ASSERT_EQUAL(TTS_VOICE_SPEED_1, speed);
    TEST_ASSERT_EQUAL(ESP_OK, tts_stream_set_speed(tts_stream_reader, TTS_VOICE_SPEED_2));
    TEST_ASSERT_EQUAL(ESP_OK, tts_stream_get_speed(tts_stream_reader, &speed));
    TEST_ASSERT_EQUAL(TTS_VOICE_SPEED_2, speed);
    TEST_ASSERT_EQUAL(ESP_OK, tts_stream_set_speed(tts_stream_reader, TTS_VOICE_SPEED_3));
    TEST_ASSERT_EQUAL(ESP_OK, tts_stream_get_speed(tts_stream_reader, &speed));
    TEST_ASSERT_EQUAL(TTS_VOICE_SPEED_3, speed);
    TEST_ASSERT_EQUAL(ESP_OK, tts_stream_set_speed(tts_stream_reader, TTS_VOICE_SPEED_4));
    TEST_ASSERT_EQUAL(ESP_OK, tts_stream_get_speed(tts_stream_reader, &speed));
    TEST_ASSERT_EQUAL(TTS_VOICE_SPEED_4, speed);
    TEST_ASSERT_EQUAL(ESP_OK, tts_stream_set_speed(tts_stream_reader, TTS_VOICE_SPEED_5));
    TEST_ASSERT_EQUAL(ESP_OK, tts_stream_get_speed(tts_stream_reader, &speed));
    TEST_ASSERT_EQUAL(TTS_VOICE_SPEED_5, speed);
    audio_element_deinit(tts_stream_reader);
}

TEST_CASE("tts stream play a chinese string", "[esp-adf-stream]")
{
    AUDIO_MEM_SHOW("START PLAY A CHINESE STRING TEST");
    audio_pipeline_handle_t pipeline = NULL;
    audio_element_handle_t tts_stream_reader = NULL, i2s_stream_writer = NULL;
    esp_periph_set_handle_t set = NULL;
    audio_event_iface_handle_t evt = NULL;

    init_tts_stream_test(&pipeline, &tts_stream_reader, &i2s_stream_writer, &set);
    TEST_ASSERT_EQUAL(ESP_OK, tts_stream_set_strings(tts_stream_reader, "乐鑫提供安全、稳定、节能的 AIoT 解决方案"));
    init_audio_event_test(&pipeline, &set, &evt);

    while (1) {
        audio_event_iface_msg_t msg;
        esp_err_t ret = audio_event_iface_listen(evt, &msg, portMAX_DELAY);
        if (ret != ESP_OK) {
            continue;
        }
        /* Stop when the last pipeline element (i2s_stream_writer in this case) receives stop event */
        if (msg.source_type == AUDIO_ELEMENT_TYPE_ELEMENT && msg.source == (void *) i2s_stream_writer
                && msg.cmd == AEL_MSG_CMD_REPORT_STATUS
                && (((int)msg.data == AEL_STATUS_STATE_STOPPED) || ((int)msg.data == AEL_STATUS_STATE_FINISHED))) {
                    break;
        }
    }
    deinit_tts_stream_test(&pipeline, &tts_stream_reader, &i2s_stream_writer, &set, &evt);
}

TEST_CASE("tts stream play multiple strings", "[esp-adf-stream]")
{
    AUDIO_MEM_SHOW("TTS STREAM PLAY MULTIPLE STRINGS");
    static const char STRINGS[][80] = {
        {"乐鑫科技是一家全球化的无晶圆厂半导体公司"},
        {"在中国、捷克、印度、新加坡和巴西均设有办公地"},
        {"团队成员来自全世界的 20 多个国家和地区"},
    } ;

    audio_pipeline_handle_t pipeline = NULL;
    audio_element_handle_t tts_stream_reader = NULL, i2s_stream_writer = NULL;
    esp_periph_set_handle_t set = NULL;
    audio_event_iface_handle_t evt = NULL;

    init_tts_stream_test(&pipeline, &tts_stream_reader, &i2s_stream_writer, &set);
    TEST_ASSERT_EQUAL(ESP_OK, tts_stream_set_strings(tts_stream_reader, STRINGS));
    init_audio_event_test(&pipeline, &set, &evt);

    int cnt = 0;
    while (1){
        audio_event_iface_msg_t msg;
        esp_err_t ret = audio_event_iface_listen(evt, &msg, portMAX_DELAY);
        if (ret != ESP_OK) {
            continue;
        }
        /* Stop when the last pipeline element (i2s_stream_writer in this case) receives stop event */
        if (msg.source_type == AUDIO_ELEMENT_TYPE_ELEMENT && msg.source == (void *) i2s_stream_writer
                && msg.cmd == AEL_MSG_CMD_REPORT_STATUS
                && (((int)msg.data == AEL_STATUS_STATE_STOPPED) || ((int)msg.data == AEL_STATUS_STATE_FINISHED))){
                cnt ++;
                if(cnt < (sizeof(STRINGS)/sizeof(STRINGS[0]))){
                    TEST_ASSERT_EQUAL(ESP_OK, audio_pipeline_reset_ringbuffer(pipeline));
                    TEST_ASSERT_EQUAL(ESP_OK, audio_pipeline_reset_elements(pipeline));
                    TEST_ASSERT_EQUAL(ESP_OK, tts_stream_set_strings(tts_stream_reader, (STRINGS + cnt)));
                    TEST_ASSERT_EQUAL(ESP_OK, audio_pipeline_change_state(pipeline, AEL_STATE_INIT));
                    TEST_ASSERT_EQUAL(ESP_OK, audio_pipeline_run(pipeline));
                }else{
                    AUDIO_MEM_SHOW("ALL PLAY FINISHED");
                    break;
                }
        }
    }
    deinit_tts_stream_test(&pipeline, &tts_stream_reader, &i2s_stream_writer, &set, &evt);
}

TEST_CASE("tts stream play-stop and start new chainese string", "[esp-adf-stream]")
{
    AUDIO_MEM_SHOW("TTS STREAM PLAY-STOP AND START NEW CHAINESE STRING");
    audio_pipeline_handle_t pipeline = NULL;
    audio_element_handle_t tts_stream_reader = NULL, i2s_stream_writer = NULL;
    esp_periph_set_handle_t set = NULL;
    audio_event_iface_handle_t evt = NULL;

    init_tts_stream_test(&pipeline, &tts_stream_reader, &i2s_stream_writer, &set);
    TEST_ASSERT_EQUAL(ESP_OK, tts_stream_set_strings(tts_stream_reader, "第一个测试的 tts 音频流"));
    init_audio_event_test(&pipeline, &set, &evt);

    bool play_once_flag = true;
    while (1) {
        audio_event_iface_msg_t msg;
        esp_err_t ret = audio_event_iface_listen(evt, &msg, portMAX_DELAY);
        if (ret != ESP_OK) {
            continue;
        }

        if (msg.source_type == AUDIO_ELEMENT_TYPE_ELEMENT && msg.source == (void *) tts_stream_reader){
            if(((int)msg.data == AEL_STATUS_STATE_RUNNING) && (play_once_flag)){
                play_once_flag = false;
                vTaskDelay(1000);
                TEST_ASSERT_EQUAL(ESP_OK, audio_pipeline_stop(pipeline));
                TEST_ASSERT_EQUAL(ESP_OK, audio_pipeline_wait_for_stop(pipeline));
                TEST_ASSERT_EQUAL(ESP_OK, audio_element_reset_state(tts_stream_reader));
                TEST_ASSERT_EQUAL(ESP_OK, audio_element_reset_state(i2s_stream_writer));
                TEST_ASSERT_EQUAL(ESP_OK, audio_pipeline_reset_ringbuffer(pipeline));
                TEST_ASSERT_EQUAL(ESP_OK, audio_pipeline_reset_items_state(pipeline));
                TEST_ASSERT_EQUAL(ESP_OK, tts_stream_set_strings(tts_stream_reader, "停止第一个播放，开始第二个测试的 tts 音频播放"));
                TEST_ASSERT_EQUAL(ESP_OK, audio_pipeline_run(pipeline));
            }
        }
        /* Stop when the last pipeline element (i2s_stream_writer in this case) receives stop event */
        if (msg.source_type == AUDIO_ELEMENT_TYPE_ELEMENT && msg.source == (void *) i2s_stream_writer
                && msg.cmd == AEL_MSG_CMD_REPORT_STATUS){
                if(((int)msg.data == AEL_STATUS_STATE_STOPPED) || ((int)msg.data == AEL_STATUS_STATE_FINISHED)){
                    AUDIO_MEM_SHOW("AEL_STATUS_STATE_FINISHED");
                    break;
                }
        }
    }
    deinit_tts_stream_test(&pipeline, &tts_stream_reader, &i2s_stream_writer, &set, &evt);
}
