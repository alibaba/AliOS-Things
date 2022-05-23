// Copyright 2018 Espressif Systems (Shanghai) PTE LTD
// All rights reserved.

#include <string.h>
#include "esp_log.h"
#include "audio_error.h"
#include "audio_mem.h"
#include "audio_element.h"
#include "filter_resample.h"
#include "audio_type_def.h"

#ifndef MAX
#define MAX(a,b)    ((a) > (b) ? (a) : (b))
#endif

static const char *TAG = "RSP_FILTER";

#define RESAMPLING_POINT_NUM (512)

typedef struct rsp_filter {
    resample_info_t *resample_info;
    unsigned char *out_buf;
    unsigned char *in_buf;
    void *rsp_hd;
    int in_offset;
    int8_t flag;  //the flag must be 0 or 1. 1 means the parameter in `resample_info_t` changed. 0 means the parameter in `resample_info_t` is original.
} rsp_filter_t;

static esp_err_t is_valid_rsp_filter_samplerate(int samplerate)
{
    if (samplerate < 8000
        || samplerate > 96000) {
        ESP_LOGE(TAG, "The sample rate should be within range [8000,96000], here is %d Hz", samplerate);
        return ESP_FAIL;
    }
    return ESP_OK;
}

static esp_err_t is_valid_rsp_filter_channel(int channel)
{
    if (channel != 1 && channel != 2) {
        ESP_LOGE(TAG, "The number of channels should be either 1 or 2, here is %d", channel);
        return ESP_FAIL;
    }
    return ESP_OK;
}

esp_err_t rsp_filter_set_src_info(audio_element_handle_t self, int src_rate, int src_ch)
{
    rsp_filter_t *filter = (rsp_filter_t *)audio_element_getdata(self);
    if (filter->resample_info->src_rate == src_rate
        && filter->resample_info->src_ch == src_ch) {
        return ESP_OK;
    }
    if (is_valid_rsp_filter_samplerate(src_rate) != ESP_OK
        || is_valid_rsp_filter_channel(src_ch) != ESP_OK) {
        return ESP_ERR_INVALID_ARG;
    } else {
        filter->flag = 1;
        filter->resample_info->src_rate = src_rate;
        filter->resample_info->src_ch = src_ch;
        ESP_LOGI(TAG, "reset sample rate of source data : %d, reset channel of source data : %d",
                 filter->resample_info->src_rate, filter->resample_info->src_ch);
    }
    return ESP_OK;
}

static esp_err_t rsp_filter_destroy(audio_element_handle_t self)
{
    rsp_filter_t *filter = (rsp_filter_t *)audio_element_getdata(self);
    if (filter != NULL
        && filter->resample_info != NULL) {
        audio_free(filter->resample_info);
        filter->resample_info = NULL;
    }
    if (filter != NULL) {
        audio_free(filter);
    }
    return ESP_OK;
}

static esp_err_t rsp_filter_open(audio_element_handle_t self)
{
    rsp_filter_t *filter = (rsp_filter_t *)audio_element_getdata(self);
    resample_info_t *resample_info = filter->resample_info;
    if (resample_info->sample_bits != 16) {
        ESP_LOGE(TAG, "Currently, the only supported bit width is 16 bits.");
        return ESP_ERR_INVALID_ARG;
    }
    if (resample_info->complexity < 0) {
        ESP_LOGI(TAG, "Currently, the complexity is %d, that is less than 0, it has been set 0.", resample_info->complexity);
        resample_info->complexity = 0;
    } else if (resample_info->complexity > COMPLEXITY_MAX_NUM ) {
        ESP_LOGI(TAG, "Currently, the complexity is %d, that is more than the maximal of complexity, it has been set the maximal of complexity.", resample_info->complexity);
        resample_info->complexity = COMPLEXITY_MAX_NUM;
    }
    if (filter->resample_info->mode == RESAMPLE_DECODE_MODE) {
        resample_info->max_indata_bytes = MAX(resample_info->max_indata_bytes, RESAMPLING_POINT_NUM * resample_info->src_ch); //`RESAMPLING_POINT_NUM * resample_info->src_ch` is for mininum of `resample_info->max_indata_bytes`, enough extra buffer for safety
        filter->in_offset = resample_info->max_indata_bytes;
    } else if (filter->resample_info->mode == RESAMPLE_ENCODE_MODE) {
        int tmp = (int)((float)resample_info->src_rate * resample_info->src_ch) / ((float)resample_info->dest_rate * resample_info->dest_ch);
        tmp = tmp > 1.0 ? tmp : 1.0;
        resample_info->max_indata_bytes =  (int)(resample_info->out_len_bytes * tmp) + RESAMPLING_POINT_NUM * resample_info->src_ch; //`RESAMPLING_POINT_NUM * resample_info->src_ch` has no meaning, just enought extra buffer for safety
        filter->in_offset = 0;
    }
    filter->flag = 0;
    filter->rsp_hd = esp_resample_create((void *)filter->resample_info,
                                         (unsigned char **)&filter->in_buf, (unsigned char **)&filter->out_buf);
    if (filter->rsp_hd == NULL) {
        ESP_LOGE(TAG, "Failed to create the resample handler");
        return ESP_FAIL;
    }
    ESP_LOGI(TAG, "sample rate of source data : %d, channel of source data : %d, sample rate of destination data : %d, channel of destination data : %d",
             filter->resample_info->src_rate, filter->resample_info->src_ch, filter->resample_info->dest_rate,
             filter->resample_info->dest_ch);
    return ESP_OK;
}

static esp_err_t rsp_filter_close(audio_element_handle_t self)
{
    rsp_filter_t *filter = (rsp_filter_t *)audio_element_getdata(self);
    if (filter->rsp_hd != NULL) {
        esp_resample_destroy(filter->rsp_hd);
        filter->rsp_hd = 0;
    }
    return ESP_OK;
}

static int rsp_filter_process(audio_element_handle_t self, char *in_buffer, int in_len)
{
    rsp_filter_t *filter = (rsp_filter_t *)audio_element_getdata(self);
    int out_len = -1;
    int read_len = 0;
    int in_bytes_consumed = 0;
    int ret = 0;
    if (filter->flag == 1) {
        ret = rsp_filter_close(self);
        if (ret != ESP_OK) {
            return AEL_PROCESS_FAIL;
        }
        ret = rsp_filter_open(self);
        if (ret != ESP_OK) {
            return AEL_PROCESS_FAIL;
        }
    }
    if (filter->resample_info->mode == RESAMPLE_DECODE_MODE) {        
        read_len = audio_element_input(self, (char *)filter->in_buf, filter->in_offset);
        if (read_len == filter->in_offset) {
            in_bytes_consumed = esp_resample_run(filter->rsp_hd, filter->resample_info,
                                                 (unsigned char *)filter->in_buf, (unsigned char *)filter->out_buf, filter->in_offset,
                                                 &filter->resample_info->out_len_bytes);
            if (in_bytes_consumed != filter->in_offset) {
                return ESP_FAIL;
            }
        } else if (read_len > 0) {
            memset(filter->in_buf + read_len, 0, filter->in_offset - read_len);
            in_bytes_consumed = esp_resample_run(filter->rsp_hd, filter->resample_info,
                                                 (unsigned char *)filter->in_buf, (unsigned char *)filter->out_buf, filter->in_offset,
                                                 &filter->resample_info->out_len_bytes);
            if (in_bytes_consumed != filter->in_offset) {
                return ESP_FAIL;
            }
        } else {
            return read_len;
        }
    } else {
        if (filter->in_offset < filter->resample_info->max_indata_bytes) {
            if (filter->in_offset > 0) {
                memmove(filter->in_buf, &filter->in_buf[filter->resample_info->max_indata_bytes - filter->in_offset], filter->in_offset);
            }
            read_len = audio_element_input(self, (char *)&filter->in_buf[filter->in_offset],
                                           filter->resample_info->max_indata_bytes - filter->in_offset);
            if (read_len > 0) {
                filter->in_offset += read_len;
            }
        }
        if (read_len > 0) {
            in_bytes_consumed = esp_resample_run(filter->rsp_hd, filter->resample_info,
                                                 (unsigned char *)filter->in_buf, (unsigned char *)filter->out_buf, filter->in_offset,
                                                 &filter->resample_info->out_len_bytes);
            if (in_bytes_consumed < 0) {
                return ESP_FAIL;
            } else {
                filter->in_offset -= in_bytes_consumed;
            }
        } else {
            return read_len;
        }
    }
    out_len = audio_element_output(self, (char *)filter->out_buf, filter->resample_info->out_len_bytes);
    return out_len;
}

audio_element_handle_t rsp_filter_init(rsp_filter_cfg_t *config)
{
    if (config == NULL) {
        ESP_LOGE(TAG, "config is NULL. (line %d)", __LINE__);
        return NULL;
    }
    rsp_filter_t *filter = (rsp_filter_t *)audio_calloc(1, sizeof(rsp_filter_t));
    AUDIO_MEM_CHECK(TAG, filter, return NULL);
    resample_info_t *resample_info = audio_calloc(1, sizeof(resample_info_t));
    AUDIO_MEM_CHECK(TAG, resample_info, {audio_free(filter); return NULL;});
    audio_element_cfg_t cfg = DEFAULT_AUDIO_ELEMENT_CONFIG();
    cfg.destroy = rsp_filter_destroy;
    cfg.process = rsp_filter_process;
    cfg.open = rsp_filter_open;
    cfg.close = rsp_filter_close;
    cfg.buffer_len = 0;
    cfg.tag = "resample";
    cfg.task_stack = config->task_stack;
    cfg.task_prio = config->task_prio;
    cfg.task_core = config->task_core;
    cfg.out_rb_size = config->out_rb_size;
    cfg.stack_in_ext = config->stack_in_ext;
    audio_element_handle_t el = audio_element_init(&cfg);
    AUDIO_MEM_CHECK(TAG, el, {audio_free(filter); audio_free(resample_info); return NULL;});
    memcpy(resample_info, config, sizeof(resample_info_t));
    filter->resample_info = resample_info;
    audio_element_setdata(el, filter);
    audio_element_info_t info = {0};
    audio_element_setinfo(el, &info);
    ESP_LOGD(TAG, "rsp_filter_init");
    return el;
}
