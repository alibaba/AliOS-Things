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

#include <stdio.h>
#include <string.h>

#include "esp_log.h"

#include "audio_element.h"
#include "audio_error.h"
#include "audio_mem.h"
#include "tone_partition.h"
#include "tone_stream.h"

static const char *TAG = "TONE_STREAM";

/**
 * @brief  Parameters of each tone
 */
typedef struct tone_stream {
    audio_stream_type_t type;            /*!< File operation type */
    bool is_open;                        /*!< Tone stream status */
    bool use_delegate;                   /*!< Tone read with delegate*/
    tone_partition_handle_t tone_handle; /*!< Tone partition's operation handle*/
    tone_file_info_t cur_file;           /*!< Address to read tone file */
    const char *partition_label;         /*!< Label of tone stored in flash */
} tone_stream_t;

static esp_err_t _tone_open(audio_element_handle_t self)
{
    tone_stream_t *stream = (tone_stream_t *)audio_element_getdata(self);
    if (stream->is_open) {
        ESP_LOGE(TAG, "already opened");
        return ESP_FAIL;
    }
    stream->tone_handle = tone_partition_init(stream->partition_label, stream->use_delegate);
    if (stream->tone_handle == NULL) {
        return ESP_FAIL;
    }

    char *flash_url = audio_element_get_uri(self);

    flash_url += strlen("flash://tone/");
    char *temp = strchr(flash_url, '_');
    char find_num[2] = { 0 };
    int file_index = 0;
    if (temp != NULL) {
        strncpy(find_num, flash_url, temp - flash_url);
        file_index = strtoul(find_num, 0, 10);
        ESP_LOGD(TAG, "Wanted read flash tone index is %d", file_index);
    } else {
        ESP_LOGE(TAG, "Tone file name is not correct!");
        return ESP_FAIL;
    }

    tone_partition_get_file_info(stream->tone_handle, file_index, &stream->cur_file);
    ESP_LOGI(TAG, "Tone offset:%08x, Tone length:%d, pos:%d\n", stream->cur_file.song_adr, stream->cur_file.song_len, file_index);
    if (stream->cur_file.song_len <= 0) {
        ESP_LOGE(TAG, "Mayebe the flash tone is empty, please ensure the flash's contex");
        return ESP_FAIL;
    }

    audio_element_info_t info = { 0 };
    info.total_bytes = stream->cur_file.song_len;
    audio_element_setdata(self, stream);
    audio_element_set_total_bytes(self, info.total_bytes);

    stream->is_open = true;
    return ESP_OK;
}

static int _tone_read(audio_element_handle_t self, char *buffer, int len, TickType_t ticks_to_wait, void *context)
{
    audio_element_info_t info = { 0 };
    tone_stream_t *stream = NULL;

    stream = (tone_stream_t *)audio_element_getdata(self);
    audio_element_getinfo(self, &info);

    if (info.byte_pos + len > info.total_bytes) {
        len = info.total_bytes - info.byte_pos;
    }
    if (ESP_OK != tone_partition_file_read(stream->tone_handle, &stream->cur_file, info.byte_pos, buffer, len)) {
        ESP_LOGE(TAG, "get tone data error, line:%d", __LINE__);
        return ESP_FAIL;
    }
    if (len <= 0) {
        ESP_LOGW(TAG, "No more data,ret:%d ,info.byte_pos:%llu", len, info.byte_pos);
        return ESP_OK;
    }
    audio_element_update_byte_pos(self, len);

    return len;
}

static int _tone_process(audio_element_handle_t self, char *in_buffer, int in_len)
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

static esp_err_t _tone_close(audio_element_handle_t self)
{
    tone_stream_t *stream = (tone_stream_t *)audio_element_getdata(self);
    if (stream->is_open) {
        stream->is_open = false;
    }
    tone_partition_deinit(stream->tone_handle);
    stream->tone_handle = NULL;
    if (AEL_STATE_PAUSED != audio_element_get_state(self)) {
        audio_element_set_byte_pos(self, 0);
    }
    return ESP_OK;
}

static esp_err_t _tone_destroy(audio_element_handle_t self)
{
    tone_stream_t *stream = (tone_stream_t *)audio_element_getdata(self);
    audio_free(stream);
    return ESP_OK;
}

audio_element_handle_t tone_stream_init(tone_stream_cfg_t *config)
{
    audio_element_handle_t el;
    tone_stream_t *stream = audio_calloc(1, sizeof(tone_stream_t));

    AUDIO_MEM_CHECK(TAG, stream, return NULL);

    audio_element_cfg_t cfg = DEFAULT_AUDIO_ELEMENT_CONFIG();
    cfg.open = _tone_open;
    cfg.close = _tone_close;
    cfg.process = _tone_process;
    cfg.destroy = _tone_destroy;
    cfg.task_stack = config->task_stack;
    cfg.task_prio = config->task_prio;
    cfg.task_core = config->task_core;
    cfg.out_rb_size = config->out_rb_size;
    cfg.buffer_len = config->buf_sz;
    cfg.stack_in_ext = config->extern_stack;
    if (cfg.stack_in_ext == true && config->use_delegate == false) {
        ESP_LOGE(TAG, "Tone stream must read flash with delegate when stack is allocate in external ram");
        goto _tone_init_exit;
    }
    if (cfg.buffer_len == 0) {
        cfg.buffer_len = TONE_STREAM_BUF_SIZE;
    }
    cfg.tag = "flash";
    stream->type = config->type;
    stream->use_delegate = config->use_delegate;

    if (config->label == NULL) {
        ESP_LOGE(TAG, "Please set your tone label");
        return NULL;
    } else {
        stream->partition_label = config->label;
    }
    if (config->type == AUDIO_STREAM_READER) {
        cfg.read = _tone_read;
    } else if (config->type == AUDIO_STREAM_WRITER) {
        ESP_LOGE(TAG, "No writer for tone stream");
        goto _tone_init_exit;
    }
    el = audio_element_init(&cfg);

    AUDIO_MEM_CHECK(TAG, el, goto _tone_init_exit);
    audio_element_setdata(el, stream);
    return el;
_tone_init_exit:
    audio_free(stream);
    return NULL;
}
