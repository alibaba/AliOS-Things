/*
 * ESPRESSIF MIT License
 *
 * Copyright (c) 2019 <ESPRESSIF SYSTEMS (SHANGHAI) CO., LTD.>
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

/*
 * The module is multifunctional audio forge which contains  resample, downmix, ALC, equalizer and sonic. Choose a combination of several function by `component_select`.
 *
 *  resample: Change sample rate or number of channels for source stream.
 *  downmix: Down-mix different source streams or stereo source stream.The downmix supports less and equal than  `SOURCE_NUM_MAX` source files.
 *  ALC: Change volume of source file.
 *  equalizer: Modify a frequency response to compensate for distortion.
 *  sonic: Change the speed and pitch of source file.
 *
 */

#include <string.h>
#include "esp_log.h"
#include "esp_err.h"
#include "audio_mem.h"
#include "audio_element.h"
#include "audio_forge.h"
#include "audio_mutex.h"

// #define AUDIO_FORGE_SPEED_ANALYSIS
// #define AUDIO_FORGE_MEMORY_ANALYSIS
// #define DEBUG_AUDIO_FORGE_ISSUE

#define RESAMPLE_COMPLEXITY (2)
#define RESAMPLING_POINT_NUM (512)
#define NUMBER_BAND (10)
#define USE_XMMS_ORIGINAL_FREQENT (0)
#define MAX_MEM_PARA (2)

#ifdef AUDIO_FORGE_SPEED_ANALYSIS
#include "codec_tick.h"
int audio_forge_start_time;
int audio_forge_stop_time;
int audio_forge_whole_time;
int audio_forge_decode_time;
int audio_forge_write_time;
long pcmcnt;
#endif

#ifdef AUDIO_FORGE_MEMORY_ANALYSIS
int startmemory_inram;
int stopmemory_inram;
int startmemory_total;
int stopmemory_total;
long pcmcnt;
#endif

#ifdef DEBUG_AUDIO_FORGE_ISSUE
FILE *in_file[SOURCE_NUM_MAX];
FILE *out_file;
char in_name[SOURCE_NUM_MAX][100];
char out_name[100];
#endif

static const char *TAG = "AUDIO_FORGE";

typedef enum {
    ADUIO_FORGE_ISSTART = 0x01,
    ADUIO_FORGE_RSP_RESTART = 0x02,
    ADUIO_FORGE_DM_RESTART = 0x04,
    ADUIO_FORGE_ALC_RESTART = 0x08,
    ADUIO_FORGE_EQ_RESTART = 0x10,
    ADUIO_FORGE_SONIC_RESTART = 0x20,
} audio_forge_restart_flag_t;

typedef struct {
    void *downmix_handle;
    esp_downmix_info_t downmix;
    unsigned char **inbuf;
    int inbuf_size;
    unsigned char *outbuf;
    int *tick_to_wait;
    void **rsp_handle;
    resample_info_t *rsp_info;
    unsigned char **rsp_in;
    unsigned char **rsp_out;
    int *in_offset;
    int rsp_cplx;
    audio_forge_select_t component_select;
    void *volume_handle;
    void *eq_handle;
    int *equalizer_gain;
    void *sonic_handle;
    float sonic_pitch;
    float sonic_speed;

    long long pcm_cnt;
    int sample_rate;
    int channel;
    int volume;
    audio_forge_restart_flag_t reflag;
    int max_sample;
    int sonic_num;
    audio_forge_stop_mode_t stop_mode;
    int status;
    xSemaphoreHandle lock; 
#ifdef AUDIO_FORGE_SPEED_ANALYSIS
    codec_tick_handle_t *audio_forge_tick_handle;
#endif
} audio_forge_t;

#ifndef MAX
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif
static esp_err_t dm_open(audio_forge_t *audio_forge)
{
    for (int i = 0; i < audio_forge->downmix.source_num; i++) {
        if (audio_forge->component_select & AUDIO_FORGE_SELECT_RESAMPLE) {
            audio_forge->downmix.source_info[i].samplerate = audio_forge->sample_rate;
        }
        if ((audio_forge->sample_rate != audio_forge->downmix.source_info[i].samplerate)
            && (audio_forge->downmix.source_info[i].samplerate < SAMPLERATE_MIN
                || audio_forge->downmix.source_info[i].samplerate >= SAMPLERATE_MAX)) {
            ESP_LOGE(TAG, "The stream sample rate is out of range. (line %d)", __LINE__);
            return ESP_ERR_INVALID_ARG;
        }
        if ((audio_forge->inbuf[i] == NULL)) {
            if (i == 0) {
                audio_forge->inbuf[i] = (unsigned char *)audio_realloc(audio_forge->inbuf[i], audio_forge->max_sample * sizeof(short) * MAX_MEM_PARA);
            } else {
                audio_forge->inbuf[i] = (unsigned char *)audio_realloc(audio_forge->inbuf[i], audio_forge->max_sample * sizeof(short) * MAX_MEM_PARA);
            }              
            AUDIO_MEM_CHECK(TAG, audio_forge->inbuf[i], return ESP_ERR_NO_MEM);
        }
        if ((audio_forge->downmix.source_info[i].channel != 1)
            && (audio_forge->downmix.source_info[i].channel != 2)) {
            ESP_LOGE(TAG, "The number of channels should be either 1 or 2. (line %d)", __LINE__);
            return ESP_ERR_INVALID_ARG;
        }
        if (audio_forge->downmix.source_info[i].gain[0] < GAIN_MIN
            || audio_forge->downmix.source_info[i].gain[0] > GAIN_MAX
            || audio_forge->downmix.source_info[i].gain[1] < GAIN_MIN
            || audio_forge->downmix.source_info[i].gain[1] > GAIN_MAX) {
            ESP_LOGE(TAG, "The gain is out (%d, %d) range", GAIN_MIN, GAIN_MAX);
            return ESP_ERR_INVALID_ARG;
        }
        if (audio_forge->downmix.source_info[i].transit_time < 0) {
            ESP_LOGE(TAG, "The transit_time (%d) must be equal or gather than zero, line %d",
                     audio_forge->downmix.source_info[0].transit_time, __LINE__);
            return ESP_ERR_INVALID_ARG;
        }

        audio_forge->downmix.source_info[i].bits_num = ESP_AUDIO_BIT_16;
#ifdef DEBUG_AUDIO_FORGE_ISSUE
        in_file[i] = fopen(in_name[i], "rb");
#endif
    }
    if (audio_forge->downmix.source_num > 1) {
        audio_forge->downmix.mode = ESP_DOWNMIX_WORK_MODE_SWITCH_ON;
    }
    audio_forge->downmix.output_type = audio_forge->channel;
    audio_forge->downmix.out_ctx = ESP_DOWNMIX_OUT_CTX_LEFT_RIGHT;
    audio_forge->downmix_handle = esp_downmix_open(&audio_forge->downmix);
    AUDIO_NULL_CHECK(TAG, audio_forge->downmix_handle, return ESP_FAIL);
    return ESP_OK;
}

static esp_err_t rsp_open(audio_forge_t *audio_forge, int index)
{
    if (index > audio_forge->downmix.source_num) {
        ESP_LOGE(TAG, "index(%d) is out of range (0 %d), line (%d)", index, audio_forge->downmix.source_num, __LINE__);
        return ESP_FAIL;
    }
    audio_forge->rsp_info[index].dest_rate = audio_forge->sample_rate;
    audio_forge->rsp_info[index].dest_ch = audio_forge->rsp_info[index].src_ch;
    audio_forge->rsp_info[index].mode = RESAMPLE_ENCODE_MODE;
    int tmp = ((float)audio_forge->rsp_info[index].src_rate / (float)audio_forge->rsp_info[index].dest_rate + 0.5);
    audio_forge->rsp_info[index].max_indata_bytes = audio_forge->max_sample * sizeof(short) * tmp * audio_forge->rsp_info[index].src_ch + RESAMPLING_POINT_NUM * audio_forge->rsp_info[index].src_ch; //`RESAMPLING_POINT_NUM * resample_info->src_ch` has no meaning, just enought extra buffer for safety;
    audio_forge->rsp_info[index].out_len_bytes = audio_forge->max_sample * sizeof(short) * audio_forge->rsp_info[index].src_ch;
    audio_forge->in_offset[index] = 0;
    audio_forge->rsp_info[index].complexity = audio_forge->rsp_cplx;
    audio_forge->rsp_info[index].type = ESP_RESAMPLE_TYPE_AUTO;
    audio_forge->rsp_info[index].prefer_flag = ESP_RSP_PREFER_TYPE_SPEED;

    audio_forge->rsp_handle[index] = esp_resample_create((void *)&audio_forge->rsp_info[index], &audio_forge->rsp_in[index], &audio_forge->rsp_out[index]);
    AUDIO_NULL_CHECK(TAG, audio_forge->rsp_handle[index], return ESP_FAIL);
    if (audio_forge->inbuf[index]) {
        void *tmp = audio_forge->inbuf[index];
        audio_forge->inbuf[index] = audio_forge->rsp_out[index];
        audio_free(tmp);        
    } else {
        audio_forge->inbuf[index] = audio_forge->rsp_out[index];
    }
    if (audio_forge->rsp_info[index].src_ch != audio_forge->downmix.source_info[index].channel) {
        audio_forge->downmix.source_info[index].channel = audio_forge->rsp_info[index].src_ch;
        audio_forge->component_select |= AUDIO_FORGE_SELECT_DOWNMIX;
        audio_forge->reflag |= ADUIO_FORGE_DM_RESTART;
    }
    audio_forge->downmix.source_info[index].channel = audio_forge->rsp_info[index].src_ch;
    return ESP_OK;
}

static esp_err_t eq_open(audio_forge_t *audio_forge)
{
    audio_forge->eq_handle = esp_equalizer_init(audio_forge->channel, audio_forge->sample_rate, NUMBER_BAND, USE_XMMS_ORIGINAL_FREQENT);
    AUDIO_NULL_CHECK(TAG, audio_forge->eq_handle, return ESP_FAIL);
    for (int i = 0; i < audio_forge->channel; i++) {
        for (int j = 0; j < NUMBER_BAND; j++) {
            esp_equalizer_set_band_value(audio_forge->eq_handle, audio_forge->equalizer_gain[NUMBER_BAND * i + j], j, i);
        }
    }
    return ESP_OK;
}

static esp_err_t sonic_open(audio_forge_t *audio_forge)
{
    audio_forge->sonic_handle = esp_sonic_create_stream(audio_forge->sample_rate, audio_forge->channel);
    AUDIO_NULL_CHECK(TAG, audio_forge->sonic_handle, return ESP_FAIL);
    esp_sonic_set_resample_mode(audio_forge->sonic_handle, 0);
    esp_sonic_set_speed(audio_forge->sonic_handle, audio_forge->sonic_speed);
    esp_sonic_set_pitch(audio_forge->sonic_handle, audio_forge->sonic_pitch);
    audio_forge->sonic_num = (audio_forge->sample_rate << 1) / SONIC_MIN_PITCH;
    audio_forge->sonic_num = audio_forge->sonic_num / audio_forge->max_sample + 1;
    if (audio_forge->component_select & AUDIO_FORGE_SELECT_RESAMPLE) { 
        if (audio_forge->inbuf[0] == audio_forge->rsp_out[0]) {
            audio_forge->inbuf[0] = (unsigned char *)audio_calloc(1, audio_forge->max_sample * sizeof(short) * MAX_MEM_PARA);
        }
    } else {
        audio_forge->inbuf[0] = (unsigned char *)audio_realloc(audio_forge->inbuf[0], audio_forge->max_sample * sizeof(short) * MAX_MEM_PARA);
    }
    AUDIO_MEM_CHECK(TAG, audio_forge->inbuf[0], return ESP_ERR_NO_MEM);
    audio_forge->outbuf = (unsigned char *)audio_realloc(audio_forge->outbuf, audio_forge->max_sample * sizeof(short) * MAX_MEM_PARA);
    AUDIO_MEM_CHECK(TAG, audio_forge->outbuf, return ESP_ERR_NO_MEM);
    return ESP_OK;
}

static esp_err_t audio_forge_open(audio_element_handle_t self)
{
    ESP_LOGI(TAG, "audio_forge opened");
#ifdef AUDIO_FORGE_MEMORY_ANALYSIS
    AUDIO_MEM_SHOW(TAG);
    startmemory_inram = heap_caps_get_free_size(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    startmemory_total = esp_get_free_heap_size();
#endif
    audio_forge_t *audio_forge = (audio_forge_t *)audio_element_getdata(self);
#ifdef AUDIO_FORGE_SPEED_ANALYSIS
    codec_tick_config_t tick_config[6] = {
        { (const char *)"forge_whole_time",  esp_tick_time_ms},
        { (const char *)"forge_process_time",  cpu_tick_time_ms},
        { (const char *)"forge_write_time",  cpu_tick_time_ms},
        { (const char *)"forge_read_time",  cpu_tick_time_ms},
        { (const char *)"sonic_read_time",  cpu_tick_time_ms},
        { (const char *)"sonic_write_time",  cpu_tick_time_ms},
    };
    audio_forge->audio_forge_tick_handle = codec_tick_init(tick_config, 6);
#endif
    if (audio_forge->reflag & ADUIO_FORGE_ISSTART) {
        if (audio_forge->stop_mode == AUDIO_FORGE_STOP_MODE_MANUAL) {
            return ESP_OK;
        }        
    }
    if (!audio_forge->lock) {
        audio_forge->lock = mutex_create(); 
    }
    int ret = 0;
    audio_forge->reflag |= ADUIO_FORGE_ISSTART;
    audio_forge->sonic_num = 1;
    if ((audio_forge->sample_rate < SAMPLERATE_MIN) || (audio_forge->sample_rate >= SAMPLERATE_MAX)) {
        ESP_LOGE(TAG, "The samplerates of stream are error. (line %d)", __LINE__);
        return ESP_ERR_INVALID_ARG;
    }
    if ((audio_forge->channel != 1) && (audio_forge->channel != 2)) {
        ESP_LOGE(TAG, "The number of channels should be either 1 or 2. (line %d)", __LINE__);
        return ESP_ERR_INVALID_ARG;
    }
    mutex_lock(audio_forge->lock);
    if (audio_forge->component_select & AUDIO_FORGE_SELECT_RESAMPLE) {
        for(int i = 0; i < audio_forge->downmix.source_num; i++) {
            if (audio_forge->rsp_handle[i] == NULL){
                ret |= rsp_open(audio_forge, i);
            }
        }
    }
    mutex_unlock(audio_forge->lock);
    if (audio_forge->component_select & AUDIO_FORGE_SELECT_DOWNMIX) {
        //downmix
        ret |= dm_open(audio_forge);
    } else {
#ifdef DEBUG_AUDIO_FORGE_ISSUE
        in_file[0] = fopen(in_name[0], "rb");
#endif
        audio_forge->downmix.source_num = 1;
    }
#ifdef DEBUG_AUDIO_FORGE_ISSUE
    out_file = fopen(out_name, "wb");
#endif
    if (audio_forge->component_select & AUDIO_FORGE_SELECT_ALC) {
        //alc
        audio_forge->volume_handle = alc_volume_setup_open();
        AUDIO_NULL_CHECK(TAG, audio_forge->volume_handle, return ESP_FAIL);
    }
    if (audio_forge->component_select & AUDIO_FORGE_SELECT_EQUALIZER) {
        //equalizer
        ret |= eq_open(audio_forge);
    }
    if (audio_forge->component_select & AUDIO_FORGE_SELECT_SONIC) {
        //sonic
        ret |= sonic_open(audio_forge);
    } else {
        audio_forge->outbuf = (unsigned char *)audio_calloc(1, audio_forge->max_sample * sizeof(short) * MAX_MEM_PARA);
        AUDIO_MEM_CHECK(TAG, audio_forge->outbuf, return ESP_ERR_NO_MEM);
    }
#ifdef AUDIO_FORGE_MEMORY_ANALYSIS
    AUDIO_MEM_SHOW(TAG);
#endif
#ifdef AUDIO_FORGE_SPEED_ANALYSIS
    codec_tick_count_start(audio_forge->audio_forge_tick_handle, 0);
#endif
    return ret;
}

esp_err_t audio_forge_destroy(audio_element_handle_t self)
{
    ESP_LOGI(TAG, "audio_forge_destroy");
    audio_forge_t* audio_forge = (audio_forge_t *)audio_element_getdata(self);   
    if (audio_forge->inbuf) {
        audio_free(audio_forge->inbuf);
        audio_forge->inbuf = NULL;
    }
    if (audio_forge->rsp_handle) {
        audio_free(audio_forge->rsp_handle);
        audio_forge->rsp_handle = NULL;
    }
    if (audio_forge->rsp_info) {
        audio_free(audio_forge->rsp_info);
        audio_forge->rsp_info = NULL;
    }
    if (audio_forge->rsp_in) {
        audio_free(audio_forge->rsp_in);
        audio_forge->rsp_in = NULL;
    }
    if (audio_forge->rsp_out) {
        audio_free(audio_forge->rsp_out);
        audio_forge->rsp_out = NULL;
    }
    if (audio_forge->in_offset) {
        audio_free(audio_forge->in_offset);
        audio_forge->in_offset = NULL;
    }
    if (audio_forge->tick_to_wait) {
        audio_free(audio_forge->tick_to_wait);
        audio_forge->tick_to_wait = NULL;
    }
    if (audio_forge->downmix.source_info) {
        audio_free(audio_forge->downmix.source_info);
        audio_forge->downmix.source_info = NULL;
    }
    if (audio_forge->equalizer_gain) {
        audio_free(audio_forge->equalizer_gain);
        audio_forge->equalizer_gain = NULL;
    }
    audio_free(audio_forge);
    AUDIO_MEM_SHOW(TAG);
    audio_forge = NULL;
    return ESP_OK;
}

esp_err_t audio_forge_close(audio_element_handle_t self)
{
    ESP_LOGI(TAG, "audio forge closed");
#ifdef AUDIO_FORGE_MEMORY_ANALYSIS
    AUDIO_MEM_SHOW(TAG);
#endif
    audio_forge_t *audio_forge = (audio_forge_t *)audio_element_getdata(self);
    audio_forge->status = 0;
    if (audio_forge->stop_mode == AUDIO_FORGE_STOP_MODE_MANUAL) {
        return ESP_OK;
    }

#ifdef AUDIO_FORGE_SPEED_ANALYSIS
    codec_tick_count_stop(audio_forge->audio_forge_tick_handle, 0, false);
    int realtime = 1000 * audio_forge->pcm_cnt / (audio_forge->sample_rate * audio_forge->channel * sizeof(short));
    audio_forge->pcm_cnt = 0;
    codec_tick_printf(audio_forge->audio_forge_tick_handle, realtime);
    codec_tick_deinit(audio_forge->audio_forge_tick_handle);
    audio_forge->audio_forge_tick_handle = NULL;
#endif
    for (int i = 0; i < audio_forge->downmix.source_num; i++) {
        //resample
        if (audio_forge->rsp_handle[i] != NULL) {
            if (audio_forge->inbuf[i] != audio_forge->rsp_out[i]) {
                audio_free(audio_forge->inbuf[i]);
                audio_forge->inbuf[i] = NULL;
            } else {
                audio_forge->inbuf[i] = NULL;
            }
            esp_resample_destroy(audio_forge->rsp_handle[i]);
            audio_forge->rsp_out[i] = NULL;
            audio_forge->rsp_in[i] = NULL;
            audio_forge->rsp_handle[i] = NULL;
        }
        if (audio_forge->inbuf[i] != NULL) {
            audio_free(audio_forge->inbuf[i]);
            audio_forge->inbuf[i] = NULL;
        }
#ifdef DEBUG_AUDIO_FORGE_ISSUE
        if (in_file[i] != NULL) {
            fclose(in_file[i]);
        }
#endif
    }
#ifdef DEBUG_AUDIO_FORGE_ISSUE
    if (out_file != NULL) {
        fclose(out_file);
    }
#endif
    if (audio_forge->lock) {
        mutex_destroy(audio_forge->lock);
        audio_forge->lock = NULL;
    } 
    if (audio_forge->outbuf != NULL) {
        audio_free(audio_forge->outbuf);
        audio_forge->outbuf = NULL;
    }
    //downmix
    if (audio_forge->downmix_handle != NULL) {
        esp_downmix_close(audio_forge->downmix_handle);
        audio_forge->downmix_handle = NULL;
    }
    //alc
    if (audio_forge->volume_handle != NULL) {
        alc_volume_setup_close(audio_forge->volume_handle);
        audio_forge->volume_handle = NULL;
    }
    //equalizer
    if (audio_forge->eq_handle != NULL) {
        esp_equalizer_uninit(audio_forge->eq_handle);
        audio_forge->eq_handle = NULL;
    }
    //sonic
    if (audio_forge->sonic_handle != NULL) {
        esp_sonic_destroy_stream(audio_forge->sonic_handle);
        audio_forge->sonic_handle = NULL;
    }

#ifdef AUDIO_FORGE_MEMORY_ANALYSIS
    AUDIO_MEM_SHOW(TAG);
#endif
    return ESP_OK;
}
//resample
static esp_err_t rsp_process(audio_element_handle_t self, audio_forge_t *audio_forge, int index)
{
    int ret = 0;
    if (!(audio_forge->component_select & AUDIO_FORGE_SELECT_RESAMPLE)) {
        return ESP_ERR_NOT_SUPPORTED;
    }
    if ((audio_forge->in_offset[index] < audio_forge->rsp_info[index].max_indata_bytes)
        && (audio_forge->rsp_info[index].max_indata_bytes > 0)) {
        if (audio_forge->in_offset[index] > 0) {
            memmove(audio_forge->rsp_in[index],
                    &audio_forge->rsp_in[index][audio_forge->rsp_info[index].max_indata_bytes - audio_forge->in_offset[index]],
                    audio_forge->in_offset[index]);
        }
#ifndef DEBUG_AUDIO_FORGE_ISSUE
        if (audio_forge->downmix.source_num != 1) {
            ret = audio_element_multi_input(self,
                                            (char *)&audio_forge->rsp_in[index][audio_forge->in_offset[index]],
                                            audio_forge->rsp_info[index].max_indata_bytes - audio_forge->in_offset[index], index,
                                            audio_forge->tick_to_wait[index]);
        } else {
#ifdef AUDIO_FORGE_SPEED_ANALYSIS
            codec_tick_count_start(audio_forge->audio_forge_tick_handle, 3);
#endif
            ret = audio_element_input(self, (char *)&audio_forge->rsp_in[index][audio_forge->in_offset[index]],
                                      audio_forge->rsp_info[index].max_indata_bytes - audio_forge->in_offset[index]);
#ifdef AUDIO_FORGE_SPEED_ANALYSIS
            codec_tick_count_stop(audio_forge->audio_forge_tick_handle, 3, true);
#endif
        }
#else
        ret = fread(&audio_forge->rsp_in[index][audio_forge->in_offset[index]], 1,
                    audio_forge->rsp_info[index].max_indata_bytes - audio_forge->in_offset[index], in_file[index]);
#endif
        if (ret > 0) {
            int tmp = audio_forge->in_offset[index];
            audio_forge->in_offset[index] += ret;
            if (audio_forge->in_offset[index] != audio_forge->rsp_info[index].max_indata_bytes) {
                memset(&audio_forge->rsp_in[index][tmp] + ret, 0, audio_forge->rsp_info[index].max_indata_bytes - audio_forge->in_offset[index]);
            }
            audio_forge->in_offset[index] = audio_forge->rsp_info[index].max_indata_bytes;
        } else {
            memset(audio_forge->rsp_out[index], 0, audio_forge->rsp_info[index].out_len_bytes);
            switch (ret) {
                case AEL_IO_TIMEOUT:
                    return ESP_ERR_TIMEOUT;
                case AEL_IO_FAIL:
                    return ESP_ERR_INVALID_ARG;
                case AEL_IO_ABORT:
                    return ret;
                default:
                    return ESP_ERR_NOT_FOUND;
            }
        }
    }
    ret = esp_resample_run(audio_forge->rsp_handle[index],
                           (void *)&audio_forge->rsp_info[index], audio_forge->rsp_in[index], audio_forge->rsp_out[index],
                           audio_forge->in_offset[index], &audio_forge->rsp_info[index].out_len_bytes);
    if (ret <= 0) {
        memset(audio_forge->rsp_out[index], 0, audio_forge->rsp_info[index].out_len_bytes);
        ESP_LOGE(TAG, "resample processing happened error, audio forge ways %d ,line %d", index, __LINE__);
        return ESP_FAIL;
    } else {
        audio_forge->in_offset[index] -= ret;
    }
    return ESP_OK;
}

static int audio_forge_process(audio_element_handle_t self, char *in_buffer, int in_len)
{
#ifdef AUDIO_FORGE_MEMORY_ANALYSIS
    if (pcmcnt == 0) {
        AUDIO_MEM_SHOW(TAG);
    }
#endif
    audio_forge_t *audio_forge = (audio_forge_t *)audio_element_getdata(self);
#ifdef AUDIO_FORGE_SPEED_ANALYSIS
    codec_tick_count_start(audio_forge->audio_forge_tick_handle, 1);
#endif
    int ret = 0;
    if (!(audio_forge->reflag == ADUIO_FORGE_ISSTART)) {
        if (audio_forge->reflag & ADUIO_FORGE_DM_RESTART) {
            if (audio_forge->downmix_handle != NULL) {
                esp_downmix_close(audio_forge->downmix_handle);
                audio_forge->downmix_handle = NULL;
            }
            ret = dm_open(audio_forge);
            if (ret != ESP_OK) {
                return ESP_FAIL;
            }
            audio_forge->reflag &= (~ADUIO_FORGE_DM_RESTART);
        }
        if (audio_forge->reflag & ADUIO_FORGE_EQ_RESTART) {
            if (audio_forge->eq_handle != NULL) {
                esp_equalizer_uninit(audio_forge->eq_handle);
                audio_forge->eq_handle = NULL;
            }
            ret = eq_open(audio_forge);
            if (ret != ESP_OK) {
                return ESP_FAIL;
            }
            audio_forge->reflag &= (~ADUIO_FORGE_EQ_RESTART);
        }
        if (audio_forge->reflag & ADUIO_FORGE_SONIC_RESTART) {
            if (audio_forge->sonic_handle != NULL) {
                esp_sonic_destroy_stream(audio_forge->sonic_handle);
                audio_forge->sonic_handle = NULL;
            }
            ret = sonic_open(audio_forge);
            if (ret != ESP_OK) {
                return ESP_FAIL;
            }
            audio_forge->reflag &= (~ADUIO_FORGE_SONIC_RESTART);
        }
        ESP_LOGI(TAG, "audio_forge reopen");
    }
    int w_size = 0;
    int j = 0;
    int rsp_ret = 0;
    for (j = 0; j < audio_forge->sonic_num; j++) {
        for (int i = 0; i < audio_forge->downmix.source_num; i++) {
            //resample
            if (audio_forge->component_select & AUDIO_FORGE_SELECT_RESAMPLE) {
                rsp_ret = 0;
                if (audio_forge->status & (1 << i)) {
                    if (audio_forge->rsp_handle[i] == NULL) {
                        memset(audio_forge->inbuf[i], 0, audio_forge->max_sample * sizeof(short) * MAX_MEM_PARA);
                    }
                    continue;
                }
                if (audio_forge->rsp_handle[i] == NULL) {
                    audio_forge->status |= 1 << i;
                    continue;
                }
                mutex_lock(audio_forge->lock);
                ret = rsp_process(self, audio_forge, i);
                mutex_unlock(audio_forge->lock);
                if (ret == ESP_ERR_NOT_FOUND) {
                    ret = audio_forge->rsp_info[i].out_len_bytes;
                    esp_resample_destroy(audio_forge->rsp_handle[i]);
                    audio_forge->rsp_handle[i] = NULL;
                    if (i == 0) {
                        if (audio_forge->inbuf[0] == audio_forge->rsp_out[0]) {
                            audio_forge->inbuf[0] = (unsigned char *)audio_calloc(1, audio_forge->max_sample * sizeof(short) * MAX_MEM_PARA);
                        } else{
                            audio_forge->inbuf[0] = (unsigned char *)audio_realloc(audio_forge->inbuf[i], audio_forge->max_sample * sizeof(short) * MAX_MEM_PARA);
                        }
                    } else {
                        if (audio_forge->inbuf[i] == audio_forge->rsp_out[i]) {
                            audio_forge->inbuf[i] = (unsigned char *)audio_calloc(1, audio_forge->max_sample * sizeof(short) * MAX_MEM_PARA);
                        } else {
                            audio_forge->inbuf[i] = (unsigned char *)audio_realloc(audio_forge->inbuf[i], audio_forge->max_sample * sizeof(short) * MAX_MEM_PARA);
                        }
                    }
                    AUDIO_MEM_CHECK(TAG, audio_forge->inbuf[i], return ESP_ERR_NO_MEM);
                    audio_forge->rsp_out[i] = NULL;
                    audio_forge->rsp_in[i] = NULL;
                    audio_forge->status |= 1 << i;
                    continue;
                } else if (ret == ESP_OK) {
                    audio_forge->status &= ~(1 << i);
                    ret = audio_forge->rsp_info[i].out_len_bytes;
                } else if (ret == ESP_ERR_INVALID_ARG) {
                    audio_forge->status |= 1 << i;
                    ret = audio_forge->rsp_info[i].out_len_bytes;
                    continue;
                } else if (ret == AEL_IO_ABORT) {
                    audio_forge->status |= 1 << i;
                    ret = audio_forge->rsp_info[i].out_len_bytes;
                    rsp_ret = AEL_IO_ABORT;
                    continue;
                } else if (ret == ESP_ERR_TIMEOUT){
                    ret = audio_forge->rsp_info[i].out_len_bytes;
                    continue;
                } else if (ret == ESP_FAIL) {
                    return ESP_FAIL;
                }
                if (ret > 0) {
                    memcpy(audio_forge->inbuf[i], audio_forge->rsp_out[i], audio_forge->rsp_info[i].out_len_bytes);
                }
            } else {
#ifndef DEBUG_AUDIO_FORGE_ISSUE
                if (audio_forge->downmix.source_num != 1) {
                    audio_forge->in_offset[i] = audio_element_multi_input(self, (char *)audio_forge->inbuf[i],
                                                                          audio_forge->max_sample * sizeof(short) * audio_forge->downmix.source_info[i].channel, i,
                                                                          audio_forge->tick_to_wait[i]);
                } else {
                    audio_forge->in_offset[i] = audio_element_input(self, (char *)audio_forge->inbuf[i],
                                                                    audio_forge->max_sample * sizeof(short) * audio_forge->downmix.source_info[i].channel);
                }
#else
                audio_forge->in_offset[i] = fread((void *)&audio_forge->inbuf[i][0], 1,
                                                  audio_forge->max_sample * sizeof(short) * audio_forge->downmix.source_info[i].channel,
                                                  in_file[i]);
#endif
                if (audio_forge->in_offset[i] <= 0) {
                    memset(audio_forge->inbuf[i], 0, audio_forge->max_sample * sizeof(short) * 2);
                    switch (audio_forge->in_offset[i]) {
                        case AEL_IO_TIMEOUT:
                            break;
                        case AEL_IO_FAIL:
                        case AEL_IO_ABORT:
                        default:
                            audio_forge->status |= 1 << i;
                    }
                    continue;
                } else if (audio_forge->in_offset[i] != audio_forge->max_sample * sizeof(
                               short) * audio_forge->downmix.source_info[i].channel) {
                    memset(audio_forge->inbuf[i] + audio_forge->in_offset[i], 0,
                           audio_forge->max_sample * sizeof(short) * 2 - audio_forge->in_offset[i]);
                }
                ret = audio_forge->in_offset[i];
            }
        }
        if (audio_forge->component_select & AUDIO_FORGE_SELECT_DOWNMIX) {
            ret = esp_downmix_process(audio_forge->downmix_handle, audio_forge->inbuf,
                                        audio_forge->outbuf, audio_forge->max_sample,
                                        audio_forge->downmix.mode);
        } else {
            if (audio_forge->component_select & AUDIO_FORGE_SELECT_RESAMPLE) {
                if (audio_forge->rsp_out[0] != NULL) {
                    memcpy(audio_forge->outbuf, audio_forge->rsp_out[0], ret);
                }
            } else {
                memcpy(audio_forge->outbuf, audio_forge->inbuf[0], ret);
            }
        }
        audio_forge->pcm_cnt += ret;
        if (audio_forge->component_select & AUDIO_FORGE_SELECT_ALC) {
            //alc
            alc_volume_setup_process(audio_forge->outbuf, ret, audio_forge->channel,
                                     audio_forge->volume_handle, audio_forge->volume);
        }
        if (audio_forge->component_select & AUDIO_FORGE_SELECT_EQUALIZER) {
            //equalizer
            esp_equalizer_process(audio_forge->eq_handle,
                                  (unsigned char *)audio_forge->outbuf, ret, audio_forge->sample_rate,
                                  audio_forge->channel);
        }
        //finished
        if ((audio_forge->status == (1 << audio_forge->downmix.source_num) - 1)) {
            if (rsp_ret == AEL_IO_ABORT) {
                return ESP_CODEC_ERR_ABORT;
            }
            break;
        }
        int write_ret = 0;   
        if (audio_forge->component_select & AUDIO_FORGE_SELECT_SONIC) {
        //sonic
#ifdef AUDIO_FORGE_SPEED_ANALYSIS
            codec_tick_count_start(audio_forge->audio_forge_tick_handle, 5);
#endif
            esp_sonic_write_to_stream(audio_forge->sonic_handle, (short *)(audio_forge->outbuf),
                                      ret / (audio_forge->channel * sizeof(short)));
#ifdef AUDIO_FORGE_SPEED_ANALYSIS
            codec_tick_count_stop(audio_forge->audio_forge_tick_handle, 5, true);
#endif
            do {
#ifdef AUDIO_FORGE_SPEED_ANALYSIS
                codec_tick_count_start(audio_forge->audio_forge_tick_handle, 4);
#endif
                write_ret = esp_sonic_read_from_stream(audio_forge->sonic_handle,
                                                      (short *)audio_forge->inbuf[0], (int)audio_forge->max_sample);
#ifdef AUDIO_FORGE_SPEED_ANALYSIS
                codec_tick_count_stop(audio_forge->audio_forge_tick_handle, 4, true);
#endif
                if (write_ret > 0) {
#ifndef DEBUG_AUDIO_FORGE_ISSUE
#ifdef AUDIO_FORGE_SPEED_ANALYSIS
                    codec_tick_count_start(audio_forge->audio_forge_tick_handle, 2);
#endif
                    w_size = audio_element_output(self, (char *)audio_forge->inbuf[0],
                                                  write_ret * audio_forge->channel * sizeof(short));
#ifdef AUDIO_FORGE_SPEED_ANALYSIS
                    codec_tick_count_stop(audio_forge->audio_forge_tick_handle, 2, true);
#endif

#else
                    w_size = fwrite(audio_forge->inbuf[0], 1,
                                    write_ret * audio_forge->channel * sizeof(short) * j / audio_forge->sonic_num, out_file);
#endif
                }

            } while (write_ret > 0);
        } else {
            if (ret > 0) {
#ifndef DEBUG_AUDIO_FORGE_ISSUE
            w_size = audio_element_output(self, (char *)audio_forge->outbuf, ret);
#else
            w_size = fwrite(audio_forge->outbuf, 1, ret, out_file);
#endif
            }
        }
    }
    if (j < audio_forge->sonic_num) {
        w_size = 0;
    }
#ifdef AUDIO_FORGE_MEMORY_ANALYSIS
    pcmcnt += audio_forge->sonic_num;
    if (pcmcnt == audio_forge->sonic_num) {
        AUDIO_MEM_SHOW(TAG);
        stopmemory_inram = heap_caps_get_free_size(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
        stopmemory_total = esp_get_free_heap_size();
        int inram_mem = startmemory_inram - stopmemory_inram;
        int total_mem = startmemory_total - stopmemory_total;
        int flag = (total_mem >= inram_mem) ? 1 : 0;
        printf("MEMORYINFO: samplerate = %dHz channel = %d\n", total_mem, inram_mem, flag);
    }
#endif
    if (audio_forge->stop_mode == AUDIO_FORGE_STOP_MODE_MANUAL) {
        if (w_size <= 0) {
             w_size = 1;
        }
    }
#ifdef AUDIO_FORGE_SPEED_ANALYSIS
    codec_tick_count_stop(audio_forge->audio_forge_tick_handle, 1, true);
#endif
    return w_size;
}

esp_err_t audio_forge_alc_set_volume(audio_element_handle_t self, int volume)
{
    audio_forge_t *audio_forge = (audio_forge_t *)audio_element_getdata(self);
    if (audio_forge->component_select & AUDIO_FORGE_SELECT_ALC) {
        audio_forge->volume = volume;
        return ESP_OK;
    }
    ESP_LOGE(TAG, "The ALC don't be used");
    return ESP_FAIL;
}

esp_err_t audio_forge_alc_get_volume(audio_element_handle_t self, int *volume)
{
    audio_forge_t *audio_forge = (audio_forge_t *)audio_element_getdata(self);
    if (audio_forge->component_select & AUDIO_FORGE_SELECT_ALC) {
        *volume = audio_forge->volume;
        return ESP_OK;
    } else {
        ESP_LOGE(TAG, " The ALC has not been used.");
        return ESP_FAIL;
    }
}

esp_err_t audio_forge_downmix_set_gain(audio_element_handle_t self, int *gain, int index)
{
    audio_forge_t *audio_forge = (audio_forge_t *)audio_element_getdata(self);
    if (!(audio_forge->component_select & AUDIO_FORGE_SELECT_DOWNMIX)) {
        return ESP_OK;
    }
    if (index > audio_forge->downmix.source_num) {
        ESP_LOGE(TAG, "The index is out of source_num range");
        return ESP_ERR_INVALID_ARG;
    }
    if (gain[0] < GAIN_MIN || gain[0] > GAIN_MAX || gain[1] < GAIN_MIN || gain[1] > GAIN_MAX) {
        ESP_LOGE(TAG, "The gain is out (%d, %d) range", GAIN_MIN, GAIN_MAX);
        return ESP_ERR_INVALID_ARG;
    }
    if ((int)(abs((gain[0] - audio_forge->downmix.source_info[index].gain[0]) * 100)) <= 5 //100 and 5 is to determine if two double numbers are equal.
        && (int)(abs((gain[1] - audio_forge->downmix.source_info[index].gain[0]) * 100)) <= 5) {
        return ESP_OK;
    }
    audio_forge->reflag |= ADUIO_FORGE_DM_RESTART;
    audio_forge->downmix.source_info[index].gain[0] = (float)gain[0];
    audio_forge->downmix.source_info[index].gain[1] = (float)gain[1];
    return ESP_OK;
}

esp_err_t audio_forge_downmix_set_transit_time(audio_element_handle_t self, int transit_time,
        int index)
{
    audio_forge_t *audio_forge = (audio_forge_t *)audio_element_getdata(self);
    if (!(audio_forge->component_select & AUDIO_FORGE_SELECT_DOWNMIX)) {
        return ESP_OK;
    }
    if (index > audio_forge->downmix.source_num) {
        ESP_LOGE(TAG, "The index is out of source_num range");
        return ESP_ERR_INVALID_ARG;
    }
    if (transit_time < 0) {
        ESP_LOGE(TAG, "The transit_time (%d) must be equal or gather than zero, line %d", transit_time,__LINE__);
        return ESP_ERR_INVALID_ARG;
    }
    if (audio_forge->downmix.source_info[index].transit_time == transit_time) {
        return ESP_OK;
    }
    audio_forge->reflag |= ADUIO_FORGE_DM_RESTART;
    audio_forge->downmix.source_info[index].transit_time = transit_time;
    return ESP_OK;
}

void audio_forge_downmix_set_input_rb_timeout(audio_element_handle_t self, int tick_to_wait, int index)
{
    audio_forge_t *audio_forge = (audio_forge_t *)audio_element_getdata(self);
    audio_forge->tick_to_wait[index] = tick_to_wait;
}

void audio_forge_downmix_set_input_rb(audio_element_handle_t self, ringbuf_handle_t rb, int index)
{
    audio_element_set_multi_input_ringbuf(self, rb, index);
}

esp_err_t audio_forge_eq_set_gain(audio_element_handle_t self, int eq_gain, int band_index)
{
    audio_forge_t *audio_forge = (audio_forge_t *)audio_element_getdata(self);
    if (!(audio_forge->component_select & AUDIO_FORGE_SELECT_EQUALIZER)) {
        return ESP_OK;
    }
    if ((band_index < 0) || (band_index > NUMBER_BAND)) {
        ESP_LOGE(TAG,"The range of index for audio gain of equalizer should be [0 9]. Here is %d. (line %d)", band_index, __LINE__);
        return ESP_ERR_INVALID_ARG;
    }
    if (audio_forge->channel == 2) {
        if ((audio_forge->equalizer_gain[band_index] == eq_gain)
            && (audio_forge->equalizer_gain[NUMBER_BAND * 1 + band_index] == eq_gain)) {
            return ESP_OK;
        }
        audio_forge->reflag |= ADUIO_FORGE_EQ_RESTART;
        audio_forge->equalizer_gain[NUMBER_BAND * 0 + band_index] = eq_gain;
        audio_forge->equalizer_gain[NUMBER_BAND * 1 + band_index] = eq_gain;
        return ESP_OK;
    }
    if (audio_forge->equalizer_gain[band_index] == eq_gain) {
        return ESP_OK;
    }
    audio_forge->reflag |= ADUIO_FORGE_EQ_RESTART;
    audio_forge->equalizer_gain[band_index] = eq_gain;
    return ESP_OK;
}

esp_err_t audio_forge_sonic_get_speed(audio_element_handle_t self, float *sonic_speed)
{
    audio_forge_t *audio_forge = (audio_forge_t *)audio_element_getdata(self);
    if (!(audio_forge->component_select & AUDIO_FORGE_SELECT_SONIC)) {
        *sonic_speed = 1.0;
        return ESP_OK;
    }
    *sonic_speed = audio_forge->sonic_speed;
    return ESP_OK;
}

esp_err_t audio_forge_sonic_set_speed(audio_element_handle_t self, float sonic_speed)
{   
    if (self == NULL) {
        ESP_LOGE(TAG, "audio forge handle is NULL. (line %d)", __LINE__);
        return ESP_FAIL;
    }
    audio_forge_t *audio_forge = (audio_forge_t *)audio_element_getdata(self);
    if (!(audio_forge->component_select & AUDIO_FORGE_SELECT_SONIC)) {
        return ESP_OK;
    }
    if ((int)(abs((sonic_speed - audio_forge->sonic_speed) * 100)) <= 5) {
        return ESP_OK;
    }
    audio_forge->reflag |= ADUIO_FORGE_SONIC_RESTART;
    audio_forge->sonic_speed = sonic_speed;
    return ESP_OK;
}

esp_err_t audio_forge_sonic_set_component_select(audio_element_handle_t self, int component_select)
{
    if (self == NULL) {
        ESP_LOGE(TAG, "audio forge handle is NULL. (line %d)", __LINE__);
        return ESP_FAIL;
    }
    audio_forge_t *audio_forge = (audio_forge_t *)audio_element_getdata(self);
    audio_forge->component_select = component_select;
    return ESP_OK;
}

esp_err_t audio_forge_sonic_set_pitch(audio_element_handle_t self, float sonic_pitch)
{
    audio_forge_t *audio_forge = (audio_forge_t *)audio_element_getdata(self);
    if (!(audio_forge->component_select & AUDIO_FORGE_SELECT_SONIC)) {
        return ESP_OK;
    }
    if ((int)(abs((sonic_pitch - audio_forge->sonic_pitch) * 100)) <= 5) {
        //100 and 5 is to determine if two double numbers are equal.
        return ESP_OK;
    }
    audio_forge->reflag |= ADUIO_FORGE_SONIC_RESTART;
    audio_forge->sonic_pitch = sonic_pitch;
    return ESP_OK;
}

esp_err_t audio_forge_set_src_info(audio_element_handle_t self, audio_forge_src_info_t source_info, int index)
{
    audio_forge_t *audio_forge = (audio_forge_t *)audio_element_getdata(self);
    if (index > audio_forge->downmix.source_num) {
        ESP_LOGE(TAG, "The index is out of source_num range");
        return ESP_ERR_INVALID_ARG;
    }
    if (source_info.samplerate < SAMPLERATE_MIN || source_info.samplerate > SAMPLERATE_MAX) {
        ESP_LOGE(TAG, "Samplerate is out of (%d %d) range.", SAMPLERATE_MIN, SAMPLERATE_MAX);
        return ESP_ERR_INVALID_ARG;
    }
    if (source_info.channel != 1 && source_info.channel != 2) {
        ESP_LOGE(TAG, "The number of channel must be 1 or 2 ");
        return ESP_ERR_INVALID_ARG;
    }
    if (source_info.bit_num != ESP_AUDIO_BIT_16) {
        ESP_LOGE(TAG, "Currently, the bit width only support 16.  Bit width is set %d ", source_info.bit_num);
        return ESP_ERR_INVALID_ARG;
    }
    if ((audio_forge->component_select & AUDIO_FORGE_SELECT_EQUALIZER)
        && source_info.samplerate != 11025 && source_info.samplerate != 22050
        && source_info.samplerate != 44100 && source_info.samplerate != 48000) {
        ESP_LOGE(TAG, "Unsupported sample rate %d. Currently supported rates are 11025 Hz, 22050 Hz, 44100 Hz and 48000 Hz", source_info.samplerate);
        return ESP_ERR_INVALID_ARG;
    }
    if (!audio_forge->lock){
        audio_forge->lock = mutex_create(); 
    }
    mutex_lock(audio_forge->lock);
    if (audio_forge->component_select & AUDIO_FORGE_SELECT_RESAMPLE) {
        if (audio_forge->rsp_handle[index]) {
            if (audio_forge->rsp_info[index].src_rate == source_info.samplerate
                && audio_forge->rsp_info[index].src_ch == source_info.channel
                && audio_forge->rsp_info[index].sample_bits == source_info.bit_num) {
                audio_forge->status &= ~(1 << index);
                mutex_unlock(audio_forge->lock);
                return ESP_OK;
            } else {
                esp_resample_destroy(audio_forge->rsp_handle[index]);
                if (audio_forge->inbuf[index] == audio_forge->rsp_out[index]) {
                    audio_forge->inbuf[index] = NULL;
                }
                audio_forge->rsp_out[index] = NULL;
                audio_forge->rsp_in[index] = NULL;
                audio_forge->rsp_handle[index]= NULL;
            }
        }
        audio_forge->rsp_info[index].src_rate = source_info.samplerate;
        audio_forge->rsp_info[index].src_ch = source_info.channel;
        audio_forge->rsp_info[index].sample_bits = source_info.bit_num;
        if (rsp_open(audio_forge, index) == ESP_FAIL) {
            mutex_unlock(audio_forge->lock);
            return ESP_FAIL;
        }
        audio_forge->status &= ~(1 << index);
        mutex_unlock(audio_forge->lock);
        return ESP_OK;
    }
    mutex_unlock(audio_forge->lock);
    audio_forge->status &= ~(1 << index);
    if (audio_forge->sample_rate == source_info.samplerate
        && audio_forge->channel == source_info.channel) {
        return ESP_OK;
    }
    audio_forge->sample_rate = source_info.samplerate;
    audio_forge->channel = source_info.channel;
    if (audio_forge->component_select & AUDIO_FORGE_SELECT_DOWNMIX) {
        audio_forge->reflag |= ADUIO_FORGE_DM_RESTART;
        return ESP_OK;
    }
    if (audio_forge->component_select & AUDIO_FORGE_SELECT_EQUALIZER) {
        audio_forge->reflag |= ADUIO_FORGE_EQ_RESTART;
    }
    if (audio_forge->component_select & AUDIO_FORGE_SELECT_SONIC) {
        audio_forge->reflag |= ADUIO_FORGE_SONIC_RESTART;
    }
    return ESP_OK;
}

void audio_forge_set_stop_mode(audio_element_handle_t self, audio_forge_stop_mode_t stop_mode)
{
    audio_forge_t *audio_forge = (audio_forge_t *)audio_element_getdata(self);
    audio_forge->stop_mode = stop_mode;
}

void audio_forge_downmix_start(audio_element_handle_t self)
{
    audio_forge_t *audio_forge = (audio_forge_t *)audio_element_getdata(self);
    audio_forge->downmix.mode = ESP_DOWNMIX_WORK_MODE_SWITCH_ON;
    for (int i = 1; i < audio_forge->downmix.source_num; i++) { 
        audio_forge->status &= ~(1 << i);
    }
}

void audio_forge_downmix_stop(audio_element_handle_t self)
{
    audio_forge_t *audio_forge = (audio_forge_t *)audio_element_getdata(self);
    audio_forge->downmix.mode = ESP_DOWNMIX_WORK_MODE_BYPASS;
    for (int i = 1; i < audio_forge->downmix.source_num; i++) { 
        audio_forge->status |= (1 << i);
    }
}

//if user only uses `downmix`, source streams has same sample rate.
esp_err_t audio_forge_source_info_init(audio_element_handle_t self, audio_forge_src_info_t *source_num, audio_forge_downmix_t *downmix_info)
{
    audio_forge_t *audio_forge = (audio_forge_t *)audio_element_getdata(self);
    for (int i = 0; i < audio_forge->downmix.source_num; i++) {
        audio_forge->downmix.source_info[i].samplerate = source_num[i].samplerate;
        audio_forge->downmix.source_info[i].channel = source_num[i].channel;
        audio_forge->downmix.source_info[i].bits_num = source_num[i].bit_num;
        audio_forge->downmix.source_info[i].gain[0] = downmix_info[i].gain[0];
        audio_forge->downmix.source_info[i].gain[1] = downmix_info[i].gain[1];
        audio_forge->downmix.source_info[i].transit_time = downmix_info[i].transit_time;
        if (audio_forge->component_select & AUDIO_FORGE_SELECT_RESAMPLE) {
            audio_forge->rsp_info[i].src_rate = audio_forge->downmix.source_info[i].samplerate;
            audio_forge->rsp_info[i].src_ch = audio_forge->downmix.source_info[i].channel;
            audio_forge->rsp_info[i].sample_bits = audio_forge->downmix.source_info[i].bits_num;
        }
    }
    return ESP_OK;
}

audio_element_handle_t audio_forge_init(audio_forge_cfg_t *config)
{
    if (config == NULL) {
        ESP_LOGE(TAG, "config is NULL. (line %d)", __LINE__);
        return NULL;
    }
    audio_element_cfg_t cfg = DEFAULT_AUDIO_ELEMENT_CONFIG();
    audio_element_handle_t el;
    cfg.open = audio_forge_open;
    cfg.close = audio_forge_close;
    cfg.process = audio_forge_process;
    cfg.destroy = audio_forge_destroy;
    cfg.task_stack = config->task_stack;
    cfg.task_prio = config->task_prio;
    cfg.task_core = config->task_core;
    cfg.out_rb_size = config->out_rb_size;
    cfg.stack_in_ext = config->stack_in_ext;
    config->audio_forge.source_num = config->audio_forge.source_num >= 1 ? config->audio_forge.source_num : 1;
    cfg.multi_in_rb_num = config->audio_forge.source_num;
    cfg.tag = "audio_forge";

    el = audio_element_init(&cfg);
    AUDIO_MEM_CHECK(TAG, el, return NULL);
    audio_forge_t *audio_forge = audio_calloc(1, sizeof(audio_forge_t));
    AUDIO_MEM_CHECK(TAG, audio_forge, return NULL);
    audio_element_setdata(el, audio_forge);

    audio_forge->sample_rate = config->audio_forge.dest_samplerate;
    audio_forge->channel = config->audio_forge.dest_channel;
    audio_forge->rsp_cplx = config->audio_forge.rsp_cplx;
    audio_forge->volume = config->audio_forge.alc_volume;
    audio_forge->sonic_pitch = config->audio_forge.sonic_pitch;
    audio_forge->sonic_speed = config->audio_forge.sonic_speed;
    audio_forge->component_select = config->audio_forge.component_select;
    audio_forge->max_sample = config->audio_forge.max_sample;
    audio_forge->downmix.source_num = config->audio_forge.source_num;
    audio_forge->stop_mode = config->audio_forge.stop_mode;
    if (audio_forge->equalizer_gain == NULL) {
        audio_forge->equalizer_gain = audio_calloc(1, NUMBER_BAND * 2 * sizeof(int));
        AUDIO_MEM_CHECK(TAG, audio_forge->equalizer_gain, { goto exit; });
    }  
    audio_forge->downmix.source_info = audio_calloc(audio_forge->downmix.source_num, sizeof(esp_downmix_input_info_t));
    AUDIO_MEM_CHECK(TAG, audio_forge->downmix.source_info, { goto exit; });
    audio_forge->inbuf = audio_calloc(audio_forge->downmix.source_num, sizeof(unsigned char *));
    AUDIO_MEM_CHECK(TAG, audio_forge->inbuf, { goto exit; });
    audio_forge->rsp_handle = audio_calloc(audio_forge->downmix.source_num, sizeof(void *));
    AUDIO_MEM_CHECK(TAG, audio_forge->rsp_handle, { goto exit; });
    audio_forge->rsp_info = audio_calloc(audio_forge->downmix.source_num, sizeof(resample_info_t));    
    AUDIO_MEM_CHECK(TAG, audio_forge->rsp_info, { goto exit; });
    audio_forge->rsp_in = audio_calloc(audio_forge->downmix.source_num, sizeof(unsigned char *));
    AUDIO_MEM_CHECK(TAG, audio_forge->rsp_in, { goto exit; });
    audio_forge->rsp_out = audio_calloc(audio_forge->downmix.source_num, sizeof(unsigned char *));
    AUDIO_MEM_CHECK(TAG, audio_forge->rsp_out, { goto exit; });
    audio_forge->in_offset = audio_calloc(audio_forge->downmix.source_num, sizeof(int));
    AUDIO_MEM_CHECK(TAG, audio_forge->in_offset, { goto exit; });
    audio_forge->tick_to_wait = audio_calloc(audio_forge->downmix.source_num, sizeof(int));
    AUDIO_MEM_CHECK(TAG, audio_forge->tick_to_wait, { goto exit; });
    return el;
exit:
    audio_forge_destroy(el);
    return NULL;
}