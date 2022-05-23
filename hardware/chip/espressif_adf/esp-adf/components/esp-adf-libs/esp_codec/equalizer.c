// Copyright 2018 Espressif Systems (Shanghai) PTE LTD
// All rights reserved.

#include <string.h>
#include "esp_log.h"
#include "audio_error.h"
#include "audio_mem.h"
#include "audio_element.h"
#include "esp_equalizer.h"
#include "equalizer.h"
#include "audio_type_def.h"
static const char *TAG = "EQUALIZER";

#define BUF_SIZE (100)
#define NUMBER_BAND (10)
#define USE_XMMS_ORIGINAL_FREQENT (0)
// #define EQUALIZER_MEMORY_ANALYSIS
// #define DEBUG_EQUALIZER_ENC_ISSUE

typedef struct equalizer {
    int  samplerate;
    int  channel;
    int  *set_gain;
    int  num_band;
    int  use_xmms_original_freqs;
    unsigned char *buf;
    void *eq_handle;
    int  byte_num;
    int  at_eof;
    int  gain_flag;
} equalizer_t;

int set_value_gain[NUMBER_BAND * 2] = { -13, -13, -13, -13, -13, -13, -13, -13, -13, -13, -13, -13, -13, -13, -13, -13, -13, -13, -13, -13};

#ifdef DEBUG_EQUALIZER_ENC_ISSUE
static FILE *infile;
#endif

static esp_err_t is_valid_equalizer_samplerate(int samplerate)
{
    if ((samplerate != 11025)
        && (samplerate != 22050)
        && (samplerate != 44100)
        && (samplerate != 48000)) {
        ESP_LOGE(TAG, "The sample rate should be only 11025Hz, 22050Hz, 44100Hz, 48000Hz, here is %dHz. (line %d)", samplerate, __LINE__);
        return ESP_ERR_INVALID_ARG;
    }
    return ESP_OK;
}

static esp_err_t is_valid_equalizer_channel(int channel)
{
    if ((channel != 1)
        && (channel != 2)) {
        ESP_LOGE(TAG, "The number of channels should be only 1 or 2, here is %d. (line %d)", channel, __LINE__);
        return ESP_ERR_INVALID_ARG;
    }
    return ESP_OK;
}

esp_err_t equalizer_set_info(audio_element_handle_t self, int rate, int ch)
{
    equalizer_t *equalizer = (equalizer_t *)audio_element_getdata(self);
    if (equalizer->samplerate == rate && equalizer->channel == ch) {
        return ESP_OK;
    }
    if ((is_valid_equalizer_samplerate(rate) != ESP_OK)
        || (is_valid_equalizer_channel(ch) != ESP_OK)) {
        return ESP_ERR_INVALID_ARG;
    } else {
        ESP_LOGI(TAG, "The reset sample rate and channel of audio stream are %d %d.", rate, ch);
        equalizer->gain_flag = 1;
        equalizer->samplerate = rate;
        equalizer->channel = ch;
    }
    return ESP_OK;
}

esp_err_t equalizer_set_gain_info(audio_element_handle_t self, int index, int value_gain, bool is_channels_gain_equal)
{
    equalizer_t *equalizer = (equalizer_t *)audio_element_getdata(self);
    if ((equalizer->channel == 2)
        && (is_channels_gain_equal == true)) {
        if ((index < 0)
            || (index > NUMBER_BAND)) {
            ESP_LOGE(TAG, "The range of index for audio gain of equalizer should be [0 9]. Here is %d. (line %d)", index, __LINE__);
            return ESP_ERR_INVALID_ARG;
        }
        int pos_index_channel = index + (equalizer->channel - 1) * NUMBER_BAND;
        if ((equalizer->set_gain[index] == value_gain)
            && (equalizer->set_gain[pos_index_channel] == value_gain)) {
            return ESP_OK;
        }
        ESP_LOGI(TAG, "The reset gain[%d] and gain[%d] of audio stream are %d.", index, pos_index_channel, value_gain);
        equalizer->gain_flag = 1;
        equalizer->set_gain[index] = value_gain;
        equalizer->set_gain[pos_index_channel] = value_gain;
        return ESP_OK;
    } else {
        if ((index < 0)
            || (index > NUMBER_BAND * 2)) {
            ESP_LOGE(TAG, "The number of channels is %d. The range of index for audio gain of equalizer should be [0 %d]. Here is %d. (line %d)", equalizer->channel, (equalizer->channel * NUMBER_BAND - 1), index, __LINE__);
            return ESP_ERR_INVALID_ARG;
        }
        if (equalizer->set_gain[index] == value_gain) {
            return ESP_OK;
        }
        ESP_LOGI(TAG, "The reset gain[%d] of audio stream is %d.", index, value_gain);
        equalizer->gain_flag = 1;
        equalizer->set_gain[index] = value_gain;
        return ESP_OK;
    }

}

static esp_err_t equalizer_destroy(audio_element_handle_t self)
{
    equalizer_t *equalizer = (equalizer_t *)audio_element_getdata(self);
    audio_free(equalizer);
    return ESP_OK;
}

static esp_err_t equalizer_open(audio_element_handle_t self)
{
#ifdef EQUALIZER_MEMORY_ANALYSIS
    AUDIO_MEM_SHOW(TAG);
#endif
    ESP_LOGD(TAG, "equalizer_open");
    equalizer_t *equalizer = (equalizer_t *)audio_element_getdata(self);
    if (equalizer->set_gain == NULL) {
        ESP_LOGE(TAG, "The gain array should be set. (line %d)", __LINE__);
        return ESP_ERR_INVALID_ARG;
    }
    audio_element_info_t info = {0};
    audio_element_getinfo(self, &info);
    if (info.sample_rates
        && info.channels) {
        equalizer->samplerate = info.sample_rates;
        equalizer->channel = info.channels;
    }
    equalizer->num_band = NUMBER_BAND;
    equalizer->use_xmms_original_freqs = USE_XMMS_ORIGINAL_FREQENT;
    equalizer->eq_handle = NULL;
    equalizer->at_eof = 0;
    equalizer->gain_flag = 0;
    if (equalizer->num_band != 10
        && equalizer->num_band != 15
        && equalizer->num_band != 25
        && equalizer->num_band != 31) {
        ESP_LOGE(TAG, "The number of bands should be one of 10, 15, 25, 31, here is %d. (line %d)", equalizer->num_band, __LINE__);
        return ESP_ERR_INVALID_ARG;
    }
    if (is_valid_equalizer_samplerate(equalizer->samplerate) != ESP_OK
        || is_valid_equalizer_channel(equalizer->channel) != ESP_OK) {
        return ESP_ERR_INVALID_ARG;
    }
    if (equalizer->use_xmms_original_freqs != 0
        && equalizer->use_xmms_original_freqs != 1) {
        ESP_LOGE(TAG, "The use_xmms_original_freqs should be only 0 or 1, here is %d. (line %d)", equalizer->use_xmms_original_freqs, __LINE__);
        return ESP_ERR_INVALID_ARG;
    }
    equalizer->buf = (unsigned char *)calloc(1, BUF_SIZE);
    if (equalizer->buf == NULL) {
        ESP_LOGE(TAG, "calloc buffer failed. (line %d)", __LINE__);
        return ESP_ERR_NO_MEM;
    }
    memset(equalizer->buf, 0, BUF_SIZE);
    equalizer->eq_handle = esp_equalizer_init(equalizer->channel, equalizer->samplerate, equalizer->num_band,
                           equalizer->use_xmms_original_freqs);
    if (equalizer->eq_handle == NULL) {
        ESP_LOGE(TAG, "failed to do equalizer initialization. (line %d)", __LINE__);
        return ESP_FAIL;
    }
    for (int i = 0; i < equalizer->channel; i++) {
        for (int j = 0; j < NUMBER_BAND; j++) {
            esp_equalizer_set_band_value(equalizer->eq_handle, equalizer->set_gain[NUMBER_BAND * i + j], j, i);
        }
    }

#ifdef DEBUG_EQUALIZER_ENC_ISSUE
    char fileName[100] = {'//', 's', 'd', 'c', 'a', 'r', 'd', '//', 't', 'e', 's', 't', '.', 'p', 'c', 'm', '\0'};
    infile = fopen(fileName, "rb");
    if (!infile) {
        perror(fileName);
        return ESP_FAIL;
    }
#endif

    return ESP_OK;
}

static esp_err_t equalizer_close(audio_element_handle_t self)
{
    ESP_LOGD(TAG, "equalizer_close");
    equalizer_t *equalizer = (equalizer_t *)audio_element_getdata(self);
    esp_equalizer_uninit(equalizer->eq_handle);
    if (equalizer->buf == NULL) {
        audio_free(equalizer->buf);
        equalizer->buf = NULL;
    }

#ifdef EQUALIZER_MEMORY_ANALYSIS
    AUDIO_MEM_SHOW(TAG);
#endif
#ifdef DEBUG_EQUALIZER_ENC_ISSUE
    fclose(infile);
#endif

    return ESP_OK;
}

static int equalizer_process(audio_element_handle_t self, char *in_buffer, int in_len)
{
    equalizer_t *equalizer = (equalizer_t *)audio_element_getdata(self);
    int ret = 0;
    if (equalizer->gain_flag == 1) {
        equalizer_close(self);
        if (ret != ESP_OK) {
            return AEL_PROCESS_FAIL;
        }
        equalizer_open(self);
        if (ret != ESP_OK) {
            return AEL_PROCESS_FAIL;
        }
        ESP_LOGI(TAG, "Reopen equalizer");
        return ESP_CODEC_ERR_CONTINUE;
    }
    int r_size = 0;
    if (equalizer->at_eof == 0) {
#ifdef DEBUG_EQUALIZER_ENC_ISSUE
        r_size = fread((char *)equalizer->buf, 1, BUF_SIZE, infile);
#else
        r_size = audio_element_input(self, (char *)equalizer->buf, BUF_SIZE);
#endif
    }
    if (r_size > 0) {
        if (r_size != BUF_SIZE) {
            equalizer->at_eof = 1;
        }
        equalizer->byte_num += r_size;
        ret = esp_equalizer_process(equalizer->eq_handle, (unsigned char *)equalizer->buf, r_size,
                                    equalizer->samplerate, equalizer->channel);
        ret = audio_element_output(self, (char *)equalizer->buf, BUF_SIZE);
    } else {
        ret = r_size;
    }
    return ret;
}

audio_element_handle_t equalizer_init(equalizer_cfg_t *config)
{
    if (config == NULL) {
        ESP_LOGE(TAG, "config is NULL. (line %d)", __LINE__);
        return NULL;
    }
    equalizer_t *equalizer = audio_calloc(1, sizeof(equalizer_t));
    AUDIO_MEM_CHECK(TAG, equalizer, return NULL);
    if (equalizer == NULL) {
        ESP_LOGE(TAG, "audio_calloc failed for equalizer. (line %d)", __LINE__);
        return NULL;
    }
    audio_element_cfg_t cfg = DEFAULT_AUDIO_ELEMENT_CONFIG();
    cfg.destroy = equalizer_destroy;
    cfg.process = equalizer_process;
    cfg.open = equalizer_open;
    cfg.close = equalizer_close;
    cfg.buffer_len = 0;
    cfg.tag = "equalizer";
    cfg.task_stack = config->task_stack;
    cfg.task_prio = config->task_prio;
    cfg.task_core = config->task_core;
    cfg.out_rb_size = config->out_rb_size;
    cfg.stack_in_ext = config->stack_in_ext;
    audio_element_handle_t el = audio_element_init(&cfg);
    AUDIO_MEM_CHECK(TAG, el, {audio_free(equalizer); return NULL;});
    equalizer->samplerate = config->samplerate;
    equalizer->channel = config->channel;
    equalizer->set_gain = config->set_gain;
    audio_element_setdata(el, equalizer);
    audio_element_info_t info = {0};
    audio_element_setinfo(el, &info);
    ESP_LOGD(TAG, "equalizer_init");
    return el;
}
