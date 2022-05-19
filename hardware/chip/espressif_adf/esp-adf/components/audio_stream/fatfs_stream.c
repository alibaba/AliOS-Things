/*
 * ESPRESSIF MIT License
 *
 * Copyright (c) 2018 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
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

#include <sys/unistd.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include "errno.h"

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"

#include "fatfs_stream.h"
#include "audio_common.h"
#include "audio_mem.h"
#include "audio_element.h"
#include "wav_head.h"
#include "esp_log.h"
#include "unistd.h"
#include "fcntl.h"

#define FILE_WAV_SUFFIX_TYPE  "wav"
#define FILE_OPUS_SUFFIX_TYPE "opus"
#define FILE_AMR_SUFFIX_TYPE "amr"
#define FILE_AMRWB_SUFFIX_TYPE "Wamr"

static const char *TAG = "FATFS_STREAM";

typedef enum {
    STREAM_TYPE_UNKNOW,
    STREAM_TYPE_WAV,
    STREAM_TYPE_OPUS,
    STREAM_TYPE_AMR,
    STREAM_TYPE_AMRWB,
} wr_stream_type_t;

typedef struct fatfs_stream {
    audio_stream_type_t type;
    int block_size;
    bool is_open;
    int file;
    wr_stream_type_t w_type;
    bool write_header;
} fatfs_stream_t;


static wr_stream_type_t get_type(const char *str)
{
    char *relt = strrchr(str, '.');
    if (relt != NULL) {
        relt ++;
        ESP_LOGD(TAG, "result = %s", relt);
        if (strncasecmp(relt, FILE_WAV_SUFFIX_TYPE, 3) == 0) {
            return STREAM_TYPE_WAV;
        } else if (strncasecmp(relt, FILE_OPUS_SUFFIX_TYPE, 4) == 0) {
            return STREAM_TYPE_OPUS;
        } else if (strncasecmp(relt, FILE_AMR_SUFFIX_TYPE, 3) == 0) {
            return STREAM_TYPE_AMR;
        } else if (strncasecmp(relt, FILE_AMRWB_SUFFIX_TYPE, 4) == 0) {
            return STREAM_TYPE_AMRWB;
        } else {
            return STREAM_TYPE_UNKNOW;
        }
    } else {
        return STREAM_TYPE_UNKNOW;
    }
}


static esp_err_t _fatfs_open(audio_element_handle_t self)
{
    fatfs_stream_t *fatfs = (fatfs_stream_t *)audio_element_getdata(self);

    audio_element_info_t info;
    char *uri = audio_element_get_uri(self);
    if (uri == NULL) {
        ESP_LOGE(TAG, "Error, uri is not set");
        return ESP_FAIL;
    }
    ESP_LOGD(TAG, "_fatfs_open, uri:%s", uri);
    char *path = strstr(uri, "/sdcard");
    audio_element_getinfo(self, &info);
    if (path == NULL) {
        ESP_LOGE(TAG, "Error, need file path to open");
        return ESP_FAIL;
    }
    if (fatfs->is_open) {
        ESP_LOGE(TAG, "already opened");
        return ESP_FAIL;
    }
    if (fatfs->type == AUDIO_STREAM_READER) {
        fatfs->file = open(path, O_RDONLY);
        if (fatfs->file == -1) {
            ESP_LOGE(TAG, "Failed to open. File name: %s, error message: %s, line: %d", path, strerror(errno), __LINE__);
            return ESP_FAIL;
        }
        struct stat siz =  { 0 };
        stat(path, &siz);
        info.total_bytes = siz.st_size;
        ESP_LOGI(TAG, "File size: %d byte, file position: %d", (int)siz.st_size, (int)info.byte_pos);
        if (info.byte_pos > 0) {
            if (lseek(fatfs->file, info.byte_pos, SEEK_SET) < 0) {
                ESP_LOGE(TAG, "Error seek file. Error message: %s, line: %d", strerror(errno), __LINE__);
                return ESP_FAIL;
            }
        }
    } else if (fatfs->type == AUDIO_STREAM_WRITER) {
        fatfs->file = open(path, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
        if (fatfs->file == -1) {
            ESP_LOGE(TAG, "Failed to open. File name: %s, error message: %s, line: %d", path, strerror(errno), __LINE__);
            return ESP_FAIL;
        }
        fatfs->w_type =  get_type(path);
        if ((STREAM_TYPE_WAV == fatfs->w_type) && (fatfs->write_header == true)) {
            wav_header_t info = {0};
            write(fatfs->file, &info, sizeof(wav_header_t));
            fsync(fatfs->file);
        } else if ((STREAM_TYPE_AMR == fatfs->w_type) && (fatfs->write_header == true)) {
            write(fatfs->file, "#!AMR\n", 6);
            fsync(fatfs->file);
        } else if ((STREAM_TYPE_AMRWB == fatfs->w_type) && (fatfs->write_header == true)) {
            write(fatfs->file, "#!AMR-WB\n", 9);
            fsync(fatfs->file);
        }
    } else {
        ESP_LOGE(TAG, "FATFS must be Reader or Writer");
        return ESP_FAIL;
    }
    fatfs->is_open = true;
    int ret = audio_element_set_total_bytes(self, info.total_bytes);
    return ret;
}

static int _fatfs_read(audio_element_handle_t self, char *buffer, int len, TickType_t ticks_to_wait, void *context)
{
    fatfs_stream_t *fatfs = (fatfs_stream_t *)audio_element_getdata(self);
    audio_element_info_t info;
    audio_element_getinfo(self, &info);

    ESP_LOGD(TAG, "read len=%d, pos=%d/%d", len, (int)info.byte_pos, (int)info.total_bytes);
    /* use file descriptors to access files */
    int rlen = read(fatfs->file, buffer, len);
    if (rlen == 0) {
        ESP_LOGW(TAG, "No more data, ret:%d", rlen);
    } else if (rlen == -1) {
        ESP_LOGE(TAG, "The error is happened in reading data. Error message: %s", strerror(errno));
    } else {
        audio_element_update_byte_pos(self, rlen);
    }
    return rlen;
}

static int _fatfs_write(audio_element_handle_t self, char *buffer, int len, TickType_t ticks_to_wait, void *context)
{
    fatfs_stream_t *fatfs = (fatfs_stream_t *)audio_element_getdata(self);
    audio_element_info_t info;
    audio_element_getinfo(self, &info);
    int wlen =  write(fatfs->file, buffer, len);
    fsync(fatfs->file);
    if (wlen > 0) {
        audio_element_update_byte_pos(self, wlen);
    } if (wlen == -1) {
        ESP_LOGE(TAG, "The error is happened in writing data. Error message: %s", strerror(errno));
    }

    return wlen;
}

static int _fatfs_process(audio_element_handle_t self, char *in_buffer, int in_len)
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

static esp_err_t _fatfs_close(audio_element_handle_t self)
{
    fatfs_stream_t *fatfs = (fatfs_stream_t *)audio_element_getdata(self);

    if (AUDIO_STREAM_WRITER == fatfs->type
        && (-1 != fatfs->file)
        && (true == fatfs->write_header)
        && STREAM_TYPE_WAV == fatfs->w_type) {
        wav_header_t *wav_info = (wav_header_t *) audio_malloc(sizeof(wav_header_t));

        AUDIO_MEM_CHECK(TAG, wav_info, return ESP_ERR_NO_MEM);

        if (lseek(fatfs->file, 0, SEEK_SET) < 0) {
            ESP_LOGE(TAG, "Error seek file. Error message: %s, line: %d", strerror(errno), __LINE__);
        }
        audio_element_info_t info;
        audio_element_getinfo(self, &info);
        wav_head_init(wav_info, info.sample_rates, info.bits, info.channels);
        wav_head_size(wav_info, (uint32_t)info.byte_pos);
        write(fatfs->file, wav_info, sizeof(wav_header_t));
        fsync(fatfs->file);
        audio_free(wav_info);
    }

    if (fatfs->is_open) {
        close(fatfs->file);
        fatfs->is_open = false;
    }
    if (AEL_STATE_PAUSED != audio_element_get_state(self)) {
        audio_element_report_info(self);
        audio_element_set_byte_pos(self, 0);
    }
    return ESP_OK;
}

static esp_err_t _fatfs_destroy(audio_element_handle_t self)
{
    fatfs_stream_t *fatfs = (fatfs_stream_t *)audio_element_getdata(self);
    audio_free(fatfs);
    return ESP_OK;
}

audio_element_handle_t fatfs_stream_init(fatfs_stream_cfg_t *config)
{
    audio_element_handle_t el;
    fatfs_stream_t *fatfs = audio_calloc(1, sizeof(fatfs_stream_t));

    AUDIO_MEM_CHECK(TAG, fatfs, return NULL);

    audio_element_cfg_t cfg = DEFAULT_AUDIO_ELEMENT_CONFIG();
    cfg.open = _fatfs_open;
    cfg.close = _fatfs_close;
    cfg.process = _fatfs_process;
    cfg.destroy = _fatfs_destroy;
    cfg.task_stack = config->task_stack;
    cfg.task_prio = config->task_prio;
    cfg.task_core = config->task_core;
    cfg.out_rb_size = config->out_rb_size;
    cfg.buffer_len = config->buf_sz;
    cfg.stack_in_ext = config->ext_stack;
    if (cfg.buffer_len == 0) {
        cfg.buffer_len = FATFS_STREAM_BUF_SIZE;
    }

    cfg.tag = "file";
    fatfs->type = config->type;
    fatfs->write_header = config->write_header;

    if (config->type == AUDIO_STREAM_WRITER) {
        cfg.write = _fatfs_write;
    } else {
        cfg.read = _fatfs_read;
    }
    el = audio_element_init(&cfg);

    AUDIO_MEM_CHECK(TAG, el, goto _fatfs_init_exit);
    audio_element_setdata(el, fatfs);
    return el;
_fatfs_init_exit:
    audio_free(fatfs);
    return NULL;
}
