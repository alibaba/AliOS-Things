// Copyright 2018 Espressif Systems (Shanghai) PTE LTD
// All rights reserved.

#include <string.h>
#include "esp_log.h"
#include "audio_error.h"
#include "audio_mem.h"
#include "audio_element.h"
#include "esp_sonic.h"
#include "audio_sonic.h"
#include "audio_type_def.h"

static const char *TAG = "SONIC";

#define BUF_SIZE (512)
// #define DEBUG_SONIC_ENC_ISSUE
// #define SONIC_MEMORY_ANALYSIS
#ifdef AUDIO_SONIC_SPEED_ANALYSIS
#include "codec_tick.h"
#endif
#ifdef DEBUG_SONIC_ENC_ISSUE
FILE *inone;
FILE *outone;
#endif

typedef struct {
    sonic_info_t sonic_info;
    int   emulate_chord_pitch;
    int   enable_nonlinear_speedup;
    int   quality;
    void  *sonic_handle;
    float volume;
    float rate;
    float pitch;
    float speed;
    int samplerate;
    int channel;
    short *inbuf;
    short *outbuf;
#ifdef AUDIO_SONIC_SPEED_ANALYSIS
    codec_tick_handle_t *sonic_tick_handle;
#endif
} sonic_t;

static esp_err_t is_valid_sonic_samplerate(int samplerate)
{
    if (samplerate < 8000
        || samplerate > 48000) {
        ESP_LOGE(TAG, "The sample rate should be within range [8000,48000], here is %d Hz", samplerate);
        return ESP_ERR_INVALID_ARG;
    }
    return ESP_OK;
}

static esp_err_t is_valid_sonic_channel(int channel)
{
    if (channel != 1 && channel != 2) {
        ESP_LOGE(TAG, "The number of channels should be either 1 or 2, here is %d", channel);
        return ESP_ERR_INVALID_ARG;;
    }
    return ESP_OK;
}

esp_err_t sonic_set_info(audio_element_handle_t self, int rate, int ch)
{
    sonic_t *sonic = (sonic_t *)audio_element_getdata(self);
    if (sonic->samplerate == rate
        && sonic->channel == ch) {
        return ESP_OK;
    }
    if (is_valid_sonic_samplerate(rate) != ESP_OK
        || is_valid_sonic_channel(ch) != ESP_OK) {
        return ESP_ERR_INVALID_ARG;;
    } else {
        sonic->samplerate = rate;
        sonic->channel = ch;
        ESP_LOGI(TAG, "reset sample rate of stream data : %d, reset channel of stream data : %d",
                 sonic->samplerate, sonic->channel);
    }
    return ESP_OK;
}

esp_err_t sonic_set_pitch_and_speed_info(audio_element_handle_t self, float pitch, float speed)
{
    sonic_t *sonic = (sonic_t *)audio_element_getdata(self);
    if ((pitch - 0 < 0.0001)
        && (speed - 0 < 0.0001)) {
        return ESP_OK;
    }

    if ((pitch >= 0.2)
        && (pitch <= 4.0)) {
        sonic->pitch = pitch;
    } else if ((pitch - 0) > 0.0001) {
        ESP_LOGE(TAG, "The pitch must be in [0.2 4.0],reset pitch of stream data is  %.2f .", pitch);
        return ESP_ERR_INVALID_ARG;;
    }

    if ((speed >= 0.1)
        && (speed <= 8.0)) {
        sonic->speed = speed;
    } else if ((speed - 0) > 0.0001) {
        ESP_LOGE(TAG, "The speed must be in [0.1 8.0],reset speed of stream data is  %.2f . ", speed);
        return ESP_ERR_INVALID_ARG;;
    }

    ESP_LOGI(TAG, "The reset pitch and speed of stream data are  %.2f and %.2f respectively ", sonic->pitch, sonic->speed);
    return ESP_OK;
}

static esp_err_t sonic_destroy(audio_element_handle_t self)
{
    sonic_t *sonic = (sonic_t *)audio_element_getdata(self);
    audio_free(sonic);
    return ESP_OK;
}

static esp_err_t sonic_open(audio_element_handle_t self)
{
#ifdef SONIC_MEMORY_ANALYSIS
    AUDIO_MEM_SHOW(TAG);
#endif
    ESP_LOGD(TAG, "sonic_open");
    sonic_t *sonic = (sonic_t *)audio_element_getdata(self);
#ifdef AUDIO_SONIC_SPEED_ANALYSIS
    codec_tick_config_t tick_config[4] = {
        { (const char *)"sonic_whole_time",  esp_tick_time_ms},
        { (const char *)"sonic_write_time",  cpu_tick_time_ms},
        { (const char *)"sonic_read_time",  cpu_tick_time_ms},
        { (const char *)"sonic_write_to_stream_time",  cpu_tick_time_ms},
    };
    sonic->sonic_tick_handle = codec_tick_init(tick_config, 4);
#endif
    sonic->inbuf = (short *)calloc(sizeof(short), BUF_SIZE);
    if (sonic->inbuf == NULL) {
        ESP_LOGE(TAG, "Failed to allocate input buffer");
        return ESP_ERR_NO_MEM;
    }
    sonic->outbuf = (short *)calloc(sizeof(short), BUF_SIZE);
    if (sonic->outbuf == NULL) {
        ESP_LOGE(TAG, "Failed to allocate output buffer");
        return ESP_ERR_NO_MEM;
    }
    sonic->emulate_chord_pitch = 0;
    sonic->enable_nonlinear_speedup = 0;
    sonic->quality = 0;
    sonic->volume = SONIC_SET_VALUE_FOR_INITIALIZATION;
    sonic->rate = SONIC_SET_VALUE_FOR_INITIALIZATION;
    sonic->sonic_info.pitch = sonic->pitch;
    sonic->sonic_info.speed = sonic->speed;
    sonic->sonic_info.samplerate = sonic->samplerate;
    sonic->sonic_info.channel = sonic->channel;

    sonic->sonic_handle = esp_sonic_create_stream(sonic->sonic_info.samplerate, sonic->sonic_info.channel);
    if (sonic->sonic_handle == NULL) {
        ESP_LOGE(TAG, "The handle of sonic is NULL");
        return ESP_FAIL;
    }
    esp_sonic_set_resample_mode(sonic->sonic_handle, sonic->sonic_info.resample_linear_interpolate);
    esp_sonic_set_speed(sonic->sonic_handle, sonic->sonic_info.speed);
    esp_sonic_set_pitch(sonic->sonic_handle, sonic->sonic_info.pitch);
    esp_sonic_set_rate(sonic->sonic_handle, sonic->rate);
    esp_sonic_set_volume(sonic->sonic_handle, sonic->volume);
    esp_sonic_set_chord_pitch(sonic->sonic_handle, sonic->emulate_chord_pitch);
    esp_sonic_set_quality(sonic->sonic_handle, sonic->quality);

#ifdef DEBUG_SONIC_ENC_ISSUE
    char fileName1[100] = {'//', 's', 'd', 'c', 'a', 'r', 'd', '//', 't', 'e', 's', 't', '1', '.', 'p', 'c', 'm', '\0'};
    inone = fopen(fileName1, "rb");
    if (!inone) {
        perror(fileName1);
        return ESP_FAIL;
    }
    char fileName[100] = {'//', 's', 'd', 'c', 'a', 'r', 'd', '//', 't', '.', 'p', 'c', 'm', '\0'};
    outone = fopen(fileName, "w");
    if (!outone) {
        perror(fileName);
        return ESP_FAIL;
    }
#endif
#ifdef AUDIO_SONIC_SPEED_ANALYSIS
    codec_tick_count_start(sonic->sonic_tick_handle, 0);
#endif
    return ESP_OK;
}

static esp_err_t sonic_close(audio_element_handle_t self)
{
    ESP_LOGD(TAG, "sonic_close");
    sonic_t *sonic = (sonic_t *)audio_element_getdata(self);
#ifdef AUDIO_SONIC_SPEED_ANALYSIS
    codec_tick_count_stop(sonic->sonic_tick_handle, 0, false);
    //int realtime = 1000 * audio_forge->pcm_cnt / (audio_forge->rsp_info[0].src_ch * audio_forge->rsp_info[0].src_rate * sizeof(short));
    //int realtime = 1000 * audio_forge->pcm_cnt / (audio_forge->sample_rate * audio_forge->channel * sizeof(short));
    //audio_forge->pcm_cnt = 0;
    codec_tick_printf(sonic->sonic_tick_handle, 1);
    codec_tick_deinit(sonic->sonic_tick_handle);
    sonic->sonic_tick_handle = NULL;
#endif
    if (sonic->sonic_handle != NULL) {
        esp_sonic_destroy_stream(sonic->sonic_handle);
    }
    if (sonic->inbuf != NULL) {
        audio_free(sonic->inbuf);
        sonic->inbuf = NULL;
    }
    if (sonic->outbuf != NULL) {
        audio_free(sonic->outbuf);
        sonic->outbuf = NULL;
    }
#ifdef SONIC_MEMORY_ANALYSIS
    AUDIO_MEM_SHOW(TAG);
#endif
#ifdef DEBUG_SONIC_ENC_ISSUE
    fclose(inone);
    fclose(outone);
#endif
    return ESP_OK;
}

static int sonic_process(audio_element_handle_t self, char *in_buffer, int in_len)
{
    sonic_t *sonic = (sonic_t *)audio_element_getdata(self);
    int ret = 1;
    if ((sonic->samplerate != sonic->sonic_info.samplerate)
        || (sonic->channel != sonic->sonic_info.channel)
        || (sonic->pitch != sonic->sonic_info.pitch)
        || (sonic->speed != sonic->sonic_info.speed)) {
        ret = sonic_close(self);
        if (ret != ESP_OK) {
            return AEL_PROCESS_FAIL;
        }
        sonic_open(self);
        if (ret != ESP_OK) {
            return AEL_PROCESS_FAIL;
        }
        return ESP_CODEC_ERR_CONTINUE;
    }
    int samplesRead = 0;
    int samplesWritten = 0;
    int out_buffer_len = 0;
#ifdef DEBUG_SONIC_ENC_ISSUE
    samplesRead = fread(sonic->inbuf, sizeof(short), BUF_SIZE / sonic->sonic_info.channel, inone);
#else
    samplesRead = audio_element_input(self, (char *)sonic->inbuf, BUF_SIZE * sizeof(short));
#endif
    if (samplesRead > 0) {
        samplesRead = samplesRead / (sonic->sonic_info.channel * sizeof(short));
#ifdef AUDIO_SONIC_SPEED_ANALYSIS
        codec_tick_count_start(sonic->sonic_tick_handle, 3);
#endif
        esp_sonic_write_to_stream(sonic->sonic_handle, sonic->inbuf, samplesRead);
#ifdef AUDIO_SONIC_SPEED_ANALYSIS
        codec_tick_count_stop(sonic->sonic_tick_handle, 3, true);
#endif
        do {
#ifdef AUDIO_SONIC_SPEED_ANALYSIS
            codec_tick_count_start(sonic->sonic_tick_handle, 2);
#endif
            samplesWritten = esp_sonic_read_from_stream(sonic->sonic_handle, sonic->outbuf, BUF_SIZE / sonic->sonic_info.channel);
#ifdef AUDIO_SONIC_SPEED_ANALYSIS
            codec_tick_count_stop(sonic->sonic_tick_handle, 2, true);
#endif
            if (samplesWritten > 0) {
#ifdef DEBUG_SONIC_ENC_ISSUE
                ret = fwrite(sonic->outbuf, sizeof(short), samplesWritten, outone);
#else
                out_buffer_len = samplesWritten * sonic->sonic_info.channel > BUF_SIZE ? BUF_SIZE : samplesWritten * sonic->sonic_info.channel;
#ifdef AUDIO_SONIC_SPEED_ANALYSIS
                codec_tick_count_start(sonic->sonic_tick_handle, 1);
#endif
                ret = audio_element_output(self, (char *)sonic->outbuf, out_buffer_len * sizeof(short));
#ifdef AUDIO_SONIC_SPEED_ANALYSIS
                codec_tick_count_stop(sonic->sonic_tick_handle, 1, true);
#endif
#endif
            }
        } while (samplesWritten > 0);
    } else {
        ret = samplesRead;
    }

    return ret;
}

audio_element_handle_t sonic_init(sonic_cfg_t *config)
{
    if (config == NULL) {
        ESP_LOGE(TAG, "config is NULL. (line %d)", __LINE__);
        return NULL;
    }
    sonic_t *sonic = audio_calloc(1, sizeof(sonic_t));
    AUDIO_MEM_CHECK(TAG, sonic, return NULL);
    audio_element_cfg_t cfg = DEFAULT_AUDIO_ELEMENT_CONFIG();
    cfg.destroy = sonic_destroy;
    cfg.process = sonic_process;
    cfg.open = sonic_open;
    cfg.close = sonic_close;
    cfg.buffer_len = 0;
    cfg.tag = "sonic";
    cfg.task_stack = config->task_stack;
    cfg.task_prio = config->task_prio;
    cfg.task_core = config->task_core;
    cfg.out_rb_size = config->out_rb_size;
    cfg.stack_in_ext = config->stack_in_ext;
    audio_element_handle_t el = audio_element_init(&cfg);
    AUDIO_MEM_CHECK(TAG, el, {audio_free(sonic); return NULL;});
    sonic->sonic_info = config->sonic_info;
    sonic->pitch = sonic->sonic_info.pitch;
    sonic->speed = sonic->sonic_info.speed;
    sonic->samplerate = sonic->sonic_info.samplerate;
    sonic->channel = sonic->sonic_info.channel;

    audio_element_setdata(el, sonic);
    audio_element_info_t info = {0};
    audio_element_setinfo(el, &info);
    ESP_LOGD(TAG, "sonic_init");
    return el;
}
