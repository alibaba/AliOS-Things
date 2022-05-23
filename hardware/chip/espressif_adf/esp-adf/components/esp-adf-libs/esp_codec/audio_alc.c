// Copyright 2019 Espressif Systems (Shanghai) CO., LTD.
// All rights reserved.

#include <math.h>
#include <string.h>
#include "esp_log.h"
#include "audio_error.h"
#include "audio_element.h"
#include "audio_alc.h"
#include "esp_alc.h"
#include "audio_mem.h"

#define ALC_INBUFFER_LENGTH       (2048)
static const char *TAG = "ALC";
static esp_err_t _alc_volume_setup_open(audio_element_handle_t self)
{
    volume_set_t *vol_setup_info = (volume_set_t *)audio_element_getdata(self);
    vol_setup_info->in_buf = audio_calloc(1, ALC_INBUFFER_LENGTH);
    if (vol_setup_info->in_buf == NULL) {
        ESP_LOGE(TAG, "audio_calloc failed for in_buf. (line %d)", __LINE__);
        return ESP_ERR_NO_MEM;
    }
    vol_setup_info->handle = alc_volume_setup_open();
    if (vol_setup_info->handle == NULL) {
        ESP_LOGE(TAG, "Failed to create ALC handle. (line %d)", __LINE__);
        return ESP_FAIL;
    }
    return ESP_OK;
}

static esp_err_t _alc_volume_setup_close(audio_element_handle_t self)
{
    volume_set_t *vol_setup_info = (volume_set_t *)audio_element_getdata(self);
    alc_volume_setup_close(vol_setup_info->handle);
    if (vol_setup_info->in_buf) {
        audio_free(vol_setup_info->in_buf);
        vol_setup_info->in_buf = NULL;
    }
    return ESP_OK;
}

static esp_err_t _alc_volume_setup_process(audio_element_handle_t self, char *in_buffer, int in_len)
{
    volume_set_t *vol_setup_info = (volume_set_t *)audio_element_getdata(self);
    int read_len = audio_element_input(self, (char *)vol_setup_info->in_buf, ALC_INBUFFER_LENGTH);
    if (read_len < 0) {
        return read_len;
    }
    int ret = alc_volume_setup_process((char *)vol_setup_info->in_buf, read_len,
                                       vol_setup_info->channel, vol_setup_info->handle, vol_setup_info->volume);
    read_len = audio_element_output(self, (char *)vol_setup_info->in_buf, read_len);
    return ret;
}

void alc_volume_setup_set_channel(audio_element_handle_t self, int channel)
{
    volume_set_t *vol_setup_info = (volume_set_t *)audio_element_getdata(self);
    vol_setup_info->channel = channel;
}

void alc_volume_setup_set_volume(audio_element_handle_t self, int volume)
{
    volume_set_t *vol_setup_info = (volume_set_t *)audio_element_getdata(self);
    vol_setup_info->volume = volume;
}

int alc_volume_setup_get_volume(audio_element_handle_t self)
{
    volume_set_t *vol_setup_info = (volume_set_t *)audio_element_getdata(self);
    return vol_setup_info->volume;
}

static esp_err_t alc_volume_setup_destroy(audio_element_handle_t self)
{
    volume_set_t *vol_setup_info = (volume_set_t *)audio_element_getdata(self);
    if (vol_setup_info) {
        audio_free(vol_setup_info);
    }
    return ESP_OK;
}

audio_element_handle_t alc_volume_setup_init(alc_volume_setup_cfg_t *config)
{
    if (config == NULL) {
        ESP_LOGE(TAG, "config is NULL. (line %d)", __LINE__);
        return NULL;
    }
    volume_set_t *vol_setup_info = audio_calloc(1, sizeof(volume_set_t));
    AUDIO_MEM_CHECK(TAG, vol_setup_info, return NULL);
    audio_element_cfg_t cfg = DEFAULT_AUDIO_ELEMENT_CONFIG();
    audio_element_handle_t el;
    cfg.open = _alc_volume_setup_open;
    cfg.close = _alc_volume_setup_close;
    cfg.process = _alc_volume_setup_process;
    cfg.destroy = alc_volume_setup_destroy;
    cfg.task_stack = config->task_stack;
    cfg.task_prio = config->task_prio;
    cfg.task_core = config->task_core;
    cfg.out_rb_size = config->out_rb_size;
    cfg.stack_in_ext = config->stack_in_ext;
    cfg.tag = "alc_volume";
    el = audio_element_init(&cfg);
    AUDIO_MEM_CHECK(TAG, el, {audio_free(vol_setup_info); return NULL;});
    vol_setup_info->channel = config->channel;
    vol_setup_info->volume = config->volume;
    audio_element_setdata(el, vol_setup_info);
    return el;
}

