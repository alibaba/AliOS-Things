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

#include <string.h>
#include "tts_stream.h"
#include "audio_mem.h"
#include "esp_log.h"
#include "esp_partition.h"
#include "esp_tts_voice_template.h"

static const char *TAG = "TTS_STREAM";

typedef struct tts_stream {
    audio_stream_type_t type;
    esp_tts_voice_t *voice;
    esp_tts_handle_t *tts_handle;
    char *prompt;
    unsigned int speed;
    bool is_open;
} tts_stream_t;

static esp_err_t _tts_stream_open(audio_element_handle_t self)
{
    tts_stream_t *tts_stream = (tts_stream_t *)audio_element_getdata(self);

    tts_stream->is_open = false;
    char *uri = audio_element_get_uri(self);
    if (uri == NULL) {
        ESP_LOGE(TAG, "The TTS string is not set");
        return ESP_FAIL;
    }

    if (esp_tts_parse_chinese(tts_stream->tts_handle, uri)) {
        tts_stream->is_open = true;
        ESP_LOGW(TAG, "%s", uri);
        return ESP_OK;
    } else {
        ESP_LOGE(TAG, "The Chinese string parse failed");
    }

    return ESP_FAIL;
}

static int _tts_stream_read(audio_element_handle_t self, char *buffer, int len, TickType_t ticks_to_wait, void *context)
{
    tts_stream_t *tts_stream = (tts_stream_t *)audio_element_getdata(self);
    int rlen = 0;
    uint8_t *pcm_data = (uint8_t *)esp_tts_stream_play(tts_stream->tts_handle, &rlen, tts_stream->speed);
    if (rlen <= 0) {
        ESP_LOGW(TAG, "No more data,ret:%d", rlen);
    } else {
        memcpy(buffer, pcm_data, rlen * 2);
        audio_element_update_byte_pos(self, rlen * 2);
    }

    return rlen * 2 ;
}

static int _tts_stream_process(audio_element_handle_t self, char *in_buffer, int in_len)
{
    int r_size = audio_element_input(self, in_buffer, in_len);
    int w_size = 0;
    if (r_size > 0) {
        w_size = audio_element_output(self, in_buffer, r_size);
    } else {
        w_size = r_size;
    }
    return w_size;
}

static esp_err_t _tts_stream_close(audio_element_handle_t self)
{
    tts_stream_t *tts_stream = (tts_stream_t *)audio_element_getdata(self);
    if (tts_stream->is_open) {
        esp_tts_stream_reset(tts_stream->tts_handle);
        tts_stream->is_open = false;
    }

    return ESP_OK;
}

static esp_err_t _tts_stream_destroy(audio_element_handle_t self)
{
    tts_stream_t *tts_stream = (tts_stream_t *)audio_element_getdata(self);
    esp_tts_voice_set_free(tts_stream->voice);
    esp_tts_destroy(tts_stream->tts_handle);
    audio_free(tts_stream);

    return ESP_OK;
}

esp_err_t tts_stream_set_strings(audio_element_handle_t el, const char *strings)
{
    return audio_element_set_uri(el, strings);
}

esp_err_t tts_stream_set_speed(audio_element_handle_t el, tts_voice_speed_t speed)
{
    if (speed > TTS_VOICE_SPEED_MAX) {
        ESP_LOGE(TAG, "Invalid parameter, voice speed range is [0 - 5]");
        return ESP_FAIL;
    }
    tts_stream_t *tts_stream = (tts_stream_t *)audio_element_getdata(el);
    tts_stream->speed = speed;

    return ESP_OK;
}

esp_err_t tts_stream_get_speed(audio_element_handle_t el, tts_voice_speed_t *speed)
{
    if(speed == NULL){
        ESP_LOGE(TAG, "The speed parameter is NULL");
        return ESP_FAIL;
    }

    tts_stream_t *tts_stream = (tts_stream_t *)audio_element_getdata(el);
    *speed = tts_stream->speed;

    return ESP_OK;
}

audio_element_handle_t tts_stream_init(tts_stream_cfg_t *config)
{
    audio_element_handle_t el;
    tts_stream_t *tts_stream = audio_calloc(1, sizeof(tts_stream_t));

    AUDIO_MEM_CHECK(TAG, tts_stream, return NULL);

    audio_element_cfg_t cfg = DEFAULT_AUDIO_ELEMENT_CONFIG();
    cfg.open = _tts_stream_open;
    cfg.close = _tts_stream_close;
    cfg.process = _tts_stream_process;
    cfg.destroy = _tts_stream_destroy;
    cfg.task_stack = config->task_stack;
    cfg.task_prio = config->task_prio;
    cfg.task_core = config->task_core;
    cfg.out_rb_size = config->out_rb_size;
    cfg.buffer_len = config->buf_sz;
    cfg.stack_in_ext = config->ext_stack;
    if (cfg.buffer_len == 0) {
        cfg.buffer_len = TTS_STREAM_BUF_SIZE;
    }

    cfg.tag = "tts";
    tts_stream->type = config->type;

    const esp_partition_t* part = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_FAT, "voice_data");
    AUDIO_MEM_CHECK(TAG, part, {
        ESP_LOGE(TAG, "Couldn't find voice data partition!");
        audio_free(tts_stream);
        return NULL;
    });

    spi_flash_mmap_handle_t mmap;
    uint16_t* voicedata;
    esp_err_t err = esp_partition_mmap(part, 0, 3 * 1024 * 1024, SPI_FLASH_MMAP_DATA, (const void**)&voicedata, &mmap);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Couldn't map voice data partition!");
        audio_free(tts_stream);
        return NULL;
    }

    tts_stream->voice = esp_tts_voice_set_init(&esp_tts_voice_template, voicedata);
    AUDIO_MEM_CHECK(TAG, tts_stream->voice, {
        ESP_LOGE(TAG, "Couldn't init tts voice set!");
        audio_free(tts_stream);
        return NULL;
    });

    tts_stream->tts_handle = esp_tts_create(tts_stream->voice);
    AUDIO_MEM_CHECK(TAG, tts_stream->tts_handle, {
        ESP_LOGE(TAG, "Couldn't create tts voice handle!");
        esp_tts_voice_set_free(tts_stream->voice);
        audio_free(tts_stream);
        return NULL;
    });

    tts_stream->speed = TTS_VOICE_SPEED_3;
    cfg.read = _tts_stream_read;
    el = audio_element_init(&cfg);

    AUDIO_MEM_CHECK(TAG, el, goto _tts_stream_init_exit);
    audio_element_setdata(el, tts_stream);
    return el;

_tts_stream_init_exit:
    esp_tts_voice_set_free(tts_stream->voice);
    esp_tts_destroy(tts_stream->tts_handle);
    audio_free(tts_stream);
    return NULL;
}
