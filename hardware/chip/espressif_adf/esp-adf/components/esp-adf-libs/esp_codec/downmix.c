// Copyright (c) 2019 <ESPRESSIF SYSTEMS (SHANGHAI) CO., LTD.>
// All rights reserved.

#include <string.h>
#include "esp_log.h"
#include "audio_error.h"
#include "audio_mem.h"
#include "audio_element.h"
#include "esp_downmix.h"
#include "downmix.h"
#include "audio_type_def.h"
static const char *TAG = "DOWNMIX";

// #define DEBUG_DOWNMIX_ISSUE
// #define DOWNMIX_MEMORY_ANALYSIS

typedef struct {
    esp_downmix_info_t downmix_info;
    int max_sample;
    unsigned char **inbuf;
    unsigned char *outbuf;
    void *downmix_handle;
    int* ticks_to_wait;
    int reset_flag;
} downmix_t;

#ifdef DEBUG_DOWNMIX_ISSUE
static FILE *inputtwo[SOURCE_NUM_MAX] = {NULL};
static FILE *output = NULL;
char filein1[10][100] = {0};
char fileOut[100] = {0};
int loocpcm = 0;
#endif

static esp_err_t downmix_destroy(audio_element_handle_t self)
{
    downmix_t *downmix = (downmix_t *)audio_element_getdata(self);
    if (downmix->downmix_info.source_info) {
        audio_free(downmix->downmix_info.source_info);
    }
    if (downmix->ticks_to_wait) {
        audio_free(downmix->ticks_to_wait);
        downmix->ticks_to_wait = NULL;
    }
    audio_free(downmix);
    downmix = NULL;
    return ESP_OK;
}

static esp_err_t downmix_open(audio_element_handle_t self)
{
#ifdef DOWNMIX_MEMORY_ANALYSIS
    AUDIO_MEM_SHOW(TAG);
#endif
    ESP_LOGD(TAG, "downmix_open");
    downmix_t *downmix = (downmix_t *)audio_element_getdata(self);

    if (downmix->downmix_info.source_num <= 0) {
        ESP_LOGE(TAG, "the number of source stream is less than 1");
        return ESP_ERR_INVALID_ARG;
    }

    if (downmix->downmix_info.mode >= ESP_DOWNMIX_WORK_MODE_MAX) {
        ESP_LOGE(TAG, "The downmix work mode is greater than ESP_DOWNMIX_WORK_MODE_SWITCH_OFF, %d",
                 downmix->downmix_info.mode);
        return ESP_ERR_INVALID_ARG;
    }

    if (downmix->downmix_info.output_type >= ESP_DOWNMIX_OUTPUT_TYPE_MAX) {
        ESP_LOGE(TAG, "The downmix work mode is greater than or equal to ESP_DOWNMIX_OUTPUT_TYPE_MAX, %d",
                 downmix->downmix_info.output_type);
        return ESP_ERR_INVALID_ARG;
    }

    if (downmix->downmix_info.out_ctx >= ESP_DOWNMIX_OUT_CTX_MAX) {
        ESP_LOGE(TAG, "The downmix out_ctx is greater than or equal to ESP_DOWNMIX_OUT_CTX_MAX, %d\n",
                 downmix->downmix_info.out_ctx);
        return ESP_ERR_INVALID_ARG;
    }

    if (downmix->max_sample <= 0) {
        ESP_LOGE(TAG, "The number of sample per downmix processing must greater than zero. (line %d)", __LINE__);
        return ESP_ERR_INVALID_ARG;
    }
    downmix->inbuf = (unsigned char **)audio_calloc(1, downmix->downmix_info.source_num * sizeof(unsigned char *));
    if (downmix->inbuf == NULL) {
        ESP_LOGE(TAG, "Failed to audio_calloc. (line %d)", __LINE__);
        return ESP_ERR_NO_MEM;
    }
    int samplerate = downmix->downmix_info.source_info[0].samplerate;
    for (int i = 0; i < downmix->downmix_info.source_num; i++) {
        if ((samplerate != downmix->downmix_info.source_info[i].samplerate)
            && (downmix->downmix_info.source_info[i].samplerate < SAMPLERATE_MIN
                || downmix->downmix_info.source_info[i].samplerate >= SAMPLERATE_MAX)) {
            ESP_LOGE(TAG, "Unsupported sample rate. (line %d)", __LINE__);
            return ESP_ERR_INVALID_ARG;
        }

        if ((downmix->downmix_info.source_info[i].channel != 1)
            && (downmix->downmix_info.source_info[i].channel != 2)) {
            ESP_LOGE(TAG, "The number of channels should be either 1 or 2. (line %d)", __LINE__);
            return ESP_ERR_INVALID_ARG;
        }

        if (downmix->downmix_info.source_info[i].gain[0] < GAIN_MIN
            || downmix->downmix_info.source_info[i].gain[0] > GAIN_MAX
            || downmix->downmix_info.source_info[i].gain[1] < GAIN_MIN
            || downmix->downmix_info.source_info[i].gain[1] > GAIN_MAX) {
            ESP_LOGE(TAG, "The gain is out of [%d, %d] range", GAIN_MIN, GAIN_MAX);
            return ESP_ERR_INVALID_ARG;
        }

        if (downmix->downmix_info.source_info[i].transit_time < 0) {
            ESP_LOGE(TAG, "The transit time (%d) must be greater than or equal to zero",
                     downmix->downmix_info.source_info[0].transit_time);
            return ESP_ERR_INVALID_ARG;
        }

        downmix->downmix_info.source_info[i].bits_num = 16;

        downmix->inbuf[i] = (unsigned char *)audio_calloc(1, downmix->max_sample * 2 * sizeof(short));
        if (downmix->inbuf[i] == NULL) {
            ESP_LOGE(TAG, "Failed to audio_calloc of downmix->inbuf[%d]. (line %d)", i, __LINE__);
            return ESP_ERR_NO_MEM;
        }
    }

    downmix->outbuf = (unsigned char *)audio_calloc(1, downmix->max_sample * 2 * sizeof(short));
    if (downmix->outbuf == NULL) {
        ESP_LOGE(TAG, "audio_calloc failed for downmix->outbuf. (line %d)", __LINE__);
        return ESP_ERR_NO_MEM;
    }
    downmix->reset_flag = 0;
    esp_downmix_info_t esp_downmix_info;
    memcpy(&esp_downmix_info, &downmix->downmix_info, sizeof(esp_downmix_info_t));
    downmix->downmix_handle = esp_downmix_open(&esp_downmix_info);
    if (downmix->downmix_handle == NULL) {
        ESP_LOGE(TAG, "downmix initialization failed, (line %d)", __LINE__);
        return ESP_FAIL;
    }

#ifdef DEBUG_DOWNMIX_ISSUE
    loocpcm = 0;
    for (int i = 0; i < downmix->downmix_info.source_num; i++) {
        inputtwo[i] = fopen(filein1[i], "rb");
        if (!inputtwo[i]) {
            perror(filein1);
            inputtwo[i] = NULL;
            return ESP_FAIL;
        }
    }

    output = fopen(fileOut, "wb");
    if (!output) {
        perror(fileOut);
        return ESP_FAIL;
    }
#endif
#ifdef DOWNMIX_MEMORY_ANALYSIS
    AUDIO_MEM_SHOW(TAG);
#endif
    return ESP_OK;
}

static esp_err_t downmix_close(audio_element_handle_t self)
{
    ESP_LOGD(TAG, "downmix_close");
    downmix_t *downmix = (downmix_t *)audio_element_getdata(self);
    if (downmix->downmix_handle != NULL) {
        esp_downmix_close(downmix->downmix_handle);
    }
#ifdef DEBUG_DOWNMDX_ISSUE
    for (int i = 0; i < downmix->downmix_info.source_num; i++) {
        if (inputtwo[i] != NULL) {
            fclose(inputtwo[i]);
        }
    }

    if (output != NULL) {
        fclose(output);
    }
#endif
    for (int i = 0; i < downmix->downmix_info.source_num; i++) {
        if (downmix->inbuf[i] != NULL) {
            memset(downmix->inbuf[i], 0, downmix->max_sample * 2 * sizeof(short));
            audio_free(downmix->inbuf[i]);
            downmix->inbuf[i] = NULL;
        }
    }

    if (downmix->outbuf != NULL) {
        audio_free(downmix->outbuf);
        downmix->outbuf = NULL;
    }
#ifdef DOWNMIX_MEMORY_ANALYSIS
    AUDIO_MEM_SHOW(TAG);
#endif
    return ESP_OK;
}

static int downmix_process(audio_element_handle_t self, char *in_buffer, int in_len)
{
    downmix_t *downmix = (downmix_t *)audio_element_getdata(self);
    if (downmix == NULL) {
        ESP_LOGE(TAG, "downmix is NULL(line %d)", __LINE__);
        return ESP_FAIL;
    }
    int r_size = downmix->max_sample * sizeof(short);
    int ret = 0;
    int bytes[SOURCE_NUM_MAX] = {0};
    int status_end = 0;
#ifdef DEBUG_DOWNMIX_ISSUE
    loocpcm++;
    if (loocpcm == 0) {
        downmix->downmix_info.mode = ESP_DOWNMIX_WORK_MODE_SWITCH_ON;
    }
    if (loocpcm == 1) {
        downmix->downmix_info.mode = ESP_DOWNMIX_WORK_MODE_SWITCH_OFF;
    }
    if (loocpcm >= 300) {
        return ESP_OK;
    }
#endif
    if (downmix->reset_flag == 1) {
        ret = downmix_close(self);
        if (ret != ESP_OK) {
            return AEL_PROCESS_FAIL;
        }
        ret = downmix_open(self);
        if (ret != ESP_OK) {
            return AEL_PROCESS_FAIL;
        }
        ESP_LOGI(TAG, "Reopen downmix");
        return ESP_CODEC_ERR_CONTINUE;
    }
    esp_downmix_info_t downmix_info;
    memcpy(&downmix_info, &downmix->downmix_info, sizeof(esp_downmix_info_t));
#ifdef DEBUG_DOWNMIX_ISSUE
    for (int i = 0; i < downmix->downmix_info.source_num; i++) {
        bytes[i] = fread((char *)downmix->inbuf[i], 1,
                         r_size * downmix->downmix_info.source_info[i].channel, inputtwo[i]);
        if (bytes[i] <= 0) {
            memset(downmix->inbuf[i], 0, downmix->max_sample * 2 * sizeof(short));
        } else if (bytes[i] != r_size * downmix->downmix_info.source_info[i].channel) {
            memset(downmix->inbuf[i] + bytes[i], 0, downmix->max_sample * 2 * sizeof(short) - bytes[i]);
        }
        if (loocpcm == 3) {
            printf("inputpcm[%d]: %d byte ", i, r_size * downmix->downmix_info.source_info[0].channel);
        }
    }
#else
    int len = downmix_info.source_num;
    if (downmix->downmix_info.mode == ESP_DOWNMIX_WORK_MODE_BYPASS) {
        len = 1;
    }
    for (int index = 0; index < len; index++) {
        memset(downmix->inbuf[index], 0, downmix->max_sample * 2 * sizeof(short));
        bytes[index] = audio_element_multi_input(self, (char *)downmix->inbuf[index],
                       r_size * downmix_info.source_info[index].channel, index, downmix->ticks_to_wait[index]);
        if (bytes[index] < 0) {
            memset(downmix->inbuf[index], 0, downmix->max_sample * 2 * sizeof(short));
            if ((bytes[index] != AEL_IO_TIMEOUT)) {
                status_end++;
            }
        } else if (bytes[index] != r_size * downmix->downmix_info.source_info[index].channel) {
            memset(downmix->inbuf[index] + bytes[index], 0, downmix->max_sample * 2 * sizeof(short) - bytes[index]);
        }
        ESP_LOGD(TAG, "bytes[ %d ] = %d", index, bytes[index]);
    }
#endif
    //down-mixer finished
    if (status_end == downmix->downmix_info.source_num || (status_end == 1 && downmix->downmix_info.mode == ESP_DOWNMIX_WORK_MODE_BYPASS)) {
        return ESP_OK;
    }
    ret = esp_downmix_process(downmix->downmix_handle, downmix->inbuf, downmix->outbuf, downmix->max_sample, downmix->downmix_info.mode);

#ifdef DEBUG_DOWNMIX_ISSUE
    ret = fwrite((char *)downmix->outbuf, 1, ret, output);
    if (loocpcm == 3) {
        printf("outputpcm: %d byte\n", ret);
    }
#else
    ret = audio_element_output(self, (char *)downmix->outbuf, ret);
#endif
    return ret;
}

void downmix_set_input_rb_timeout(audio_element_handle_t self, int ticks_to_wait, int index)
{
    downmix_t *downmix = (downmix_t *)audio_element_getdata(self);
    if (downmix == NULL) {
        ESP_LOGE(TAG, "the down-mix handle is NULL, line %d", __LINE__);
        return;
    }
    if (index >= downmix->downmix_info.source_num) {
        ESP_LOGE(TAG, "the index of source number is out of range. line %d", __LINE__);
        return;
    }
    downmix->ticks_to_wait[index] = ticks_to_wait;
}

void downmix_set_input_rb(audio_element_handle_t self, ringbuf_handle_t rb, int index)
{
    audio_element_set_multi_input_ringbuf(self, rb, index);
}

esp_err_t downmix_set_work_mode(audio_element_handle_t self, esp_downmix_work_mode_t mode)
{
    downmix_t *downmix = (downmix_t *)audio_element_getdata(self);
    if (downmix == NULL) {
        ESP_LOGE(TAG, "the down-mix handle is NULL, line %d", __LINE__);
        return ESP_ERR_INVALID_ARG;
    }
    if (downmix->downmix_info.mode == mode) {
        return ESP_OK;
    }
    if (mode >= ESP_DOWNMIX_WORK_MODE_MAX) {
        ESP_LOGE(TAG, "The set downmix work mode is greater than ESP_DOWNMIX_WORK_MODE_MAX, %d", mode);
        return ESP_ERR_INVALID_ARG;
    }
    downmix->downmix_info.mode = mode;
    return ESP_OK;
}

esp_err_t downmix_set_output_type(audio_element_handle_t self, esp_downmix_output_type_t output_type)
{
    downmix_t *downmix = (downmix_t *)audio_element_getdata(self);
    if (downmix == NULL) {
        ESP_LOGE(TAG, "the down-mix handle is NULL, line %d", __LINE__);
        return ESP_ERR_INVALID_ARG;
    }
    if (downmix->downmix_info.output_type == output_type) {
        return ESP_OK;
    }
    if (output_type >= ESP_DOWNMIX_OUTPUT_TYPE_MAX) {
        ESP_LOGE(TAG, "The set downmix output type is greater than or equal to ESP_DOWNMIX_OUTPUT_TYPE_MAX, %d", output_type);
        return ESP_ERR_INVALID_ARG;
    }
    downmix->downmix_info.output_type = output_type;
    downmix->reset_flag = 1;
    return ESP_OK;
}

esp_err_t downmix_set_out_ctx_info(audio_element_handle_t self, esp_downmix_out_ctx_type_t out_ctx)
{
    downmix_t *downmix = (downmix_t *)audio_element_getdata(self);
    if (downmix == NULL) {
        ESP_LOGE(TAG, "the down-mix handle is NULL, line %d", __LINE__);
        return ESP_ERR_INVALID_ARG;
    }
    if (downmix->downmix_info.out_ctx == out_ctx) {
        return ESP_OK;
    }
    if (out_ctx >= ESP_DOWNMIX_OUT_CTX_MAX) {
        ESP_LOGE(TAG, "The set downmix out_ctx is greater than or equal to ESP_DOWNMIX_OUT_CTX_MAX, %d", out_ctx);
        return ESP_ERR_INVALID_ARG;
    }
    downmix->downmix_info.out_ctx = out_ctx;
    downmix->reset_flag = 1;
    return ESP_OK;
}

esp_err_t downmix_set_source_stream_info(audio_element_handle_t self, int rate, int ch, int index)
{
    downmix_t *downmix = (downmix_t *)audio_element_getdata(self);
    if (downmix == NULL) {
        ESP_LOGE(TAG, "the down-mix handle is NULL, line %d", __LINE__);
        return ESP_ERR_INVALID_ARG;
    }
    if (index >= downmix->downmix_info.source_num) {
        ESP_LOGE(TAG, "the index of source number is out of range. line %d", __LINE__);
        return ESP_ERR_INVALID_ARG;
    }

    if (rate != downmix->downmix_info.source_info[index].samplerate) {
        ESP_LOGE(TAG, "The samplerates must be same as origin. set sample rate: %d, origin sample rate: %d (line %d)", rate, downmix->downmix_info.source_info[index].samplerate, __LINE__);
        return ESP_ERR_INVALID_ARG;
    }
    if (ch == downmix->downmix_info.source_info[index].channel) {
        return ESP_OK;
    } else {
        if ((ch != 1) && (ch != 2)) {
            ESP_LOGE(TAG, "The number of channels for stream is error. (line %d)", __LINE__);
            return ESP_ERR_INVALID_ARG;
        }
    }
    downmix->reset_flag = 1;
    downmix->downmix_info.source_info[index].samplerate = rate;
    downmix->downmix_info.source_info[index].channel = ch;
    return ESP_OK;
}

esp_err_t downmix_set_gain_info(audio_element_handle_t self, float *gain, int index)
{
    downmix_t *downmix = (downmix_t *)audio_element_getdata(self);
    if (downmix == NULL) {
        ESP_LOGE(TAG, "the down-mix handle is NULL, line %d", __LINE__);
        return ESP_ERR_INVALID_ARG;
    }
    if (index >= downmix->downmix_info.source_num) {
        ESP_LOGE(TAG, "the index of source number is out of range. line %d", __LINE__);
        return ESP_ERR_INVALID_ARG;
    }

    if ((downmix->downmix_info.source_info[index].gain[0] - gain[0] < 0.01)
        && (downmix->downmix_info.source_info[index].gain[0] - gain[0] > 0.01)
        && (downmix->downmix_info.source_info[index].gain[1] - gain[1] < 0.01)
        && (downmix->downmix_info.source_info[index].gain[1] - gain[1] > 0.01)) {
        return ESP_OK;
    }
    if (gain[0] < GAIN_MIN || gain[0] > GAIN_MAX || gain[1] < GAIN_MIN || gain[1] > GAIN_MAX) {
        ESP_LOGE(TAG, "The gain is out of range [%d, %d]", GAIN_MIN, GAIN_MAX);
        return ESP_ERR_INVALID_ARG;
    }
    downmix->reset_flag = 1;
    downmix->downmix_info.source_info[index].gain[0] = gain[0];
    downmix->downmix_info.source_info[index].gain[1] = gain[1];
    return ESP_OK;
}

esp_err_t downmix_set_transit_time_info(audio_element_handle_t self, int transit_time, int index)
{
    downmix_t *downmix = (downmix_t *)audio_element_getdata(self);
    if (downmix == NULL) {
        ESP_LOGE(TAG, "the down-mix handle is NULL, line %d", __LINE__);
        return ESP_ERR_INVALID_ARG;
    }
    if (index >= downmix->downmix_info.source_num) {
        ESP_LOGE(TAG, "the index of source number is out of range. line %d", __LINE__);
        return ESP_ERR_INVALID_ARG;
    }

    if (downmix->downmix_info.source_info[index].transit_time == transit_time) {
        return ESP_OK;
    }

    if (transit_time < 0) {
        ESP_LOGE(TAG, "The set transit_time must be greater than or equal to zero (%d)", transit_time);
        return ESP_ERR_INVALID_ARG;
    }
    downmix->reset_flag = 1;
    downmix->downmix_info.source_info[index].transit_time = transit_time;
    return ESP_OK;
}

esp_err_t source_info_init(audio_element_handle_t self, esp_downmix_input_info_t *source_num)
{
    downmix_t *downmix = (downmix_t *)audio_element_getdata(self);
    for (int i = 0; i < downmix->downmix_info.source_num; i++) {
        downmix->downmix_info.source_info[i].samplerate = source_num[i].samplerate;
        downmix->downmix_info.source_info[i].channel = source_num[i].channel;
        downmix->downmix_info.source_info[i].bits_num = source_num[i].bits_num;
        downmix->downmix_info.source_info[i].gain[0] = source_num[i].gain[0];
        downmix->downmix_info.source_info[i].gain[1] = source_num[i].gain[1];
        downmix->downmix_info.source_info[i].transit_time = source_num[i].transit_time;
    }
    return ESP_OK;
}

audio_element_handle_t downmix_init(downmix_cfg_t *config)
{
    if (config == NULL) {
        ESP_LOGE(TAG, "config is NULL. (line %d)", __LINE__);
        return NULL;
    }

    downmix_t *downmix = audio_calloc(1, sizeof(downmix_t));
    AUDIO_MEM_CHECK(TAG, downmix, return NULL);
    audio_element_cfg_t cfg = DEFAULT_AUDIO_ELEMENT_CONFIG();
    cfg.destroy = downmix_destroy;
    cfg.process = downmix_process;
    cfg.open = downmix_open;
    cfg.close = downmix_close;
    cfg.buffer_len = 0;
    cfg.tag = "downmix";
    cfg.task_stack = config->task_stack;
    cfg.task_prio = config->task_prio;
    cfg.task_core = config->task_core;
    cfg.out_rb_size = config->out_rb_size;
    cfg.stack_in_ext = config->stack_in_ext;
    cfg.multi_in_rb_num = config->downmix_info.source_num;
    audio_element_handle_t el = audio_element_init(&cfg);
    AUDIO_MEM_CHECK(TAG, el, {audio_free(downmix); return NULL;});
    memcpy(downmix, config, sizeof(esp_downmix_info_t));
    downmix->max_sample = config->max_sample;
    downmix->reset_flag = 0;
    downmix->downmix_info.source_info = audio_calloc(downmix->downmix_info.source_num, sizeof(esp_downmix_input_info_t));
    if (downmix->downmix_info.source_info == NULL) {
        ESP_LOGE(TAG, "Failed to audio_calloc. (line %d)", __LINE__);
        audio_free(downmix);
        return NULL;
    }
    downmix->ticks_to_wait = (int*)audio_calloc(downmix->downmix_info.source_num, sizeof(int));
    if (downmix->ticks_to_wait == NULL) {
        ESP_LOGE(TAG, "audio_calloc failed for downmix->ticks_to_wait. (line %d)", __LINE__);
        audio_free(downmix->downmix_info.source_info);
        audio_free(downmix);
        return NULL;
    }
    audio_element_setdata(el, downmix);
    ESP_LOGD(TAG, "downmix_init");
    return el;
}
