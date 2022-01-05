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

#include <string.h>
#include "esp_log.h"
#include "audio_pipeline.h"
#include "audio_element.h"
#include "audio_mem.h"
#include "audio_error.h"
#include "esp_aec.h"
#include "esp_agc.h"
#include "esp_ns.h"
#include "esp_resample.h"
#include "algorithm_stream.h"

#define NS_FRAME_BYTES       320   // 10ms data frame (10 * 16 * 2)
#define AGC_FRAME_BYTES      320   // 10ms data frame (10 * 16 * 2)
#define AEC_FRAME_BYTES      512   // 16ms data frame (16 * 16 * 2)

#define ALGORITHM_STREAM_DEFAULT_AEC_MODE         3
#define ALGORITHM_STREAM_DEFAULT_AGC_MODE         3
#define ALGORITHM_STREAM_DEFAULT_AGC_FRAME_LENGTH 10    //ms

#define ALGORITHM_STREAM_RESAMPE_DEFAULT_COMPLEXITY     0
#define ALGORITHM_STREAM_RESAMPE_DEFAULT_MAX_INPUT_SIZE (AEC_FRAME_BYTES * 8)

#define ALGORITHM_STREAM_FULL_FUCTION_MASK  (ALGORITHM_STREAM_USE_AEC | ALGORITHM_STREAM_USE_AGC | ALGORITHM_STREAM_USE_NS)

static const char *TAG = "ALGORITHM_STREAM";

typedef struct {
    void *rsp_handle;
    unsigned char *rsp_in;
    unsigned char *rsp_out;
    int  in_offset;
    int  ap_factor;
    int16_t *aec_buff;
    resample_info_t rsp_info;
    ringbuf_handle_t input_rb;
    bool data_need_be_resampled;
    bool data_need_be_divided_after_rsp;  //The encode mode of resample function doesn't support change channels
    bool is_after_reset_res_info;
} algorithm_data_info_t;

typedef struct {
    void *ns_handle;
    void *aec_handle;
    void *agc_handle;
    char *scale_buff;
    int16_t *aec_buff;
    int16_t *ns_buff;
    int16_t *agc_buff;
    int8_t algo_mask;
    algorithm_data_info_t record;
    algorithm_data_info_t reference;
    algorithm_stream_input_type_t input_type;
} algo_stream_t;

static esp_err_t create_record_rsp_handle(algo_stream_t *algo);
static esp_err_t create_reference_rsp_handle(algo_stream_t *algo);
static esp_err_t destroy_record_rsp_handle(algo_stream_t *algo);
static esp_err_t destroy_reference_rsp_handle(algo_stream_t *algo);

static esp_err_t is_valid_algorithm_rsp_samplerate(int samplerate)
{
    if (samplerate < 8000
        || samplerate > 96000) {
        ESP_LOGE(TAG, "The sample rate should be within range [8000,96000], here is %d Hz", samplerate);
        return ESP_FAIL;
    }
    return ESP_OK;
}

static esp_err_t is_valid_algorithm_rsp_channel(int channel)
{
    if (channel != 1 && channel != 2) {
        ESP_LOGE(TAG, "The number of channels should be either 1 or 2, here is %d", channel);
        return ESP_FAIL;
    }
    return ESP_OK;
}

static esp_err_t algorithm_data_info_destroy(algorithm_data_info_t *data_info)
{
    static void *rsp_handle;
    if (rsp_handle == data_info->rsp_handle) {  // Avoid the rsp handle be destroyed twice.
        return ESP_OK;
    }
    if (data_info->rsp_handle) {
        rsp_handle = data_info->rsp_handle;
        esp_resample_destroy(data_info->rsp_handle);
        data_info->rsp_handle = NULL;
    }
    if (data_info->aec_buff) {
        audio_free(data_info->aec_buff);
        data_info->aec_buff = NULL;
    }
    return ESP_OK;
}

static esp_err_t _algo_close(audio_element_handle_t self)
{
    algo_stream_t *algo = (algo_stream_t *)audio_element_getdata(self);
    algorithm_data_info_t *record    = &algo->record;
    algorithm_data_info_t *reference = &algo->reference;

    if (algo->ns_handle) {
        ns_destroy(algo->ns_handle);
        algo->ns_handle = NULL;
    }
    if (algo->aec_handle) {
        aec_destroy(algo->aec_handle);
        algo->aec_handle = NULL;
    }
    if (algo->agc_handle) {
        esp_agc_clse(algo->agc_handle);
        algo->agc_handle = NULL;
    }
    if (algo->ns_buff) {
        audio_free(algo->ns_buff);
        algo->ns_buff = NULL;
    }
    if (algo->aec_buff) {
        audio_free(algo->aec_buff);
        algo->aec_buff = NULL;
    }
    if (algo->agc_buff) {
        audio_free(algo->agc_buff);
        algo->agc_buff = NULL;
    }
    if (algo->scale_buff) {
        audio_free(algo->scale_buff);
        algo->scale_buff = NULL;
    }

    algorithm_data_info_destroy(record);
    algorithm_data_info_destroy(reference);

    return ESP_OK;
}

static esp_err_t _algo_open(audio_element_handle_t self)
{
    algo_stream_t *algo = (algo_stream_t *)audio_element_getdata(self);
    bool _success = true;

    if (algo->algo_mask & ALGORITHM_STREAM_USE_AEC) {
        _success &= ((algo->aec_handle = aec_pro_create(AEC_FRAME_LENGTH_MS, ALGORITHM_STREAM_DEFAULT_CHANNEL, ALGORITHM_STREAM_DEFAULT_AEC_MODE)) != NULL);
    }
    if (algo->algo_mask & ALGORITHM_STREAM_USE_AGC) {
        _success &= ((algo->agc_handle = esp_agc_open(ALGORITHM_STREAM_DEFAULT_AGC_MODE, ALGORITHM_STREAM_DEFAULT_SAMPLE_RATE_HZ)) != NULL);
    }
    if (algo->algo_mask & ALGORITHM_STREAM_USE_NS) {
        _success &= ((algo->ns_handle = ns_create(ALGORITHM_STREAM_DEFAULT_AGC_FRAME_LENGTH)) != NULL);
    }
    AUDIO_NULL_CHECK(TAG, _success, {
        _algo_close(self);
        return ESP_FAIL;
    });
    return ESP_OK;
}

static int algorithm_process(audio_element_handle_t self)
{
    algo_stream_t *algo = (algo_stream_t *)audio_element_getdata(self);
    static int copy_cnt, cur_pos;
    algorithm_data_info_t *record    = &algo->record;
    algorithm_data_info_t *reference = &algo->reference;

    if (algo->algo_mask & ALGORITHM_STREAM_USE_AEC) {
        aec_process(algo->aec_handle, record->aec_buff, reference->aec_buff, algo->aec_buff);
        memcpy(algo->scale_buff + cur_pos, algo->aec_buff, AEC_FRAME_BYTES);
        cur_pos += AEC_FRAME_BYTES;
        copy_cnt = cur_pos / AGC_FRAME_BYTES;

        for (int i = 0; i < copy_cnt; i++) {
            if ((algo->algo_mask & ALGORITHM_STREAM_USE_NS) && (algo->algo_mask & ALGORITHM_STREAM_USE_AGC)) {
                ns_process(algo->ns_handle, (int16_t *)algo->scale_buff + i * (NS_FRAME_BYTES >> 1), algo->ns_buff);
                esp_agc_process(algo->agc_handle, algo->ns_buff, algo->agc_buff, (AGC_FRAME_BYTES >> 1), ALGORITHM_STREAM_DEFAULT_SAMPLE_RATE_HZ);
                audio_element_output(self, (char *)algo->agc_buff, AGC_FRAME_BYTES);
            } else if (algo->algo_mask & ALGORITHM_STREAM_USE_NS) {
                ns_process(algo->ns_handle, (int16_t *)algo->scale_buff + i * (NS_FRAME_BYTES >> 1), algo->ns_buff);
                audio_element_output(self, (char *)algo->ns_buff, NS_FRAME_BYTES);
            } else if (algo->algo_mask & ALGORITHM_STREAM_USE_AGC) {
                esp_agc_process(algo->agc_handle, (int16_t *)algo->scale_buff + i * (AGC_FRAME_BYTES >> 1), algo->agc_buff, (AGC_FRAME_BYTES >> 1), ALGORITHM_STREAM_DEFAULT_SAMPLE_RATE_HZ);
                audio_element_output(self, (char *)algo->ns_buff, AGC_FRAME_BYTES);
            } else {
                audio_element_output(self, (char *)algo->aec_buff, AEC_FRAME_BYTES);
                cur_pos -= AEC_FRAME_BYTES;
                return AEC_FRAME_BYTES;
            }
        }
        memcpy(algo->scale_buff, algo->scale_buff + AGC_FRAME_BYTES * copy_cnt, cur_pos - AGC_FRAME_BYTES * copy_cnt);
        cur_pos -= AGC_FRAME_BYTES * copy_cnt;
        return AGC_FRAME_BYTES * copy_cnt;
    } else {
        if((algo->algo_mask & ALGORITHM_STREAM_USE_AGC) && (algo->algo_mask & ALGORITHM_STREAM_USE_NS)) {
            ns_process(algo->ns_handle, (int16_t *)algo->scale_buff, algo->ns_buff);
            esp_agc_process(algo->agc_handle, algo->ns_buff, algo->agc_buff, (AGC_FRAME_BYTES >> 1), ALGORITHM_STREAM_DEFAULT_SAMPLE_RATE_HZ);
            audio_element_output(self, (char *)algo->agc_buff, AGC_FRAME_BYTES);
            return AGC_FRAME_BYTES;
        } else if (algo->algo_mask & ALGORITHM_STREAM_USE_NS) {
            ns_process(algo->ns_handle, (int16_t *)algo->scale_buff, algo->ns_buff);
            audio_element_output(self, (char *)algo->ns_buff, NS_FRAME_BYTES);
            return NS_FRAME_BYTES;
        } else if (algo->algo_mask & ALGORITHM_STREAM_USE_AGC) {
            esp_agc_process(algo->agc_handle, (int16_t *)algo->scale_buff, algo->agc_buff, (AGC_FRAME_BYTES >> 1), ALGORITHM_STREAM_DEFAULT_SAMPLE_RATE_HZ);
            audio_element_output(self, (char *)algo->agc_buff, AGC_FRAME_BYTES);
            return AGC_FRAME_BYTES;
        } else {
            return AEL_IO_FAIL;
        }
    }
}

static esp_err_t algorithm_data_divided(int16_t *raw_buff, int len, int16_t *left_channel, int linear_lfac, int16_t *right_channel, int linear_rfac)
{
    // To improve efficiency, data splitting and linear amplification are integrated into one function
    for (int i = 0; i < len / 4; i++) {
        if (left_channel) {
            left_channel[i]  = raw_buff[i << 1] * linear_lfac;
        }
        if (right_channel) {
            right_channel[i] = raw_buff[(i << 1) + 1] * linear_rfac;
        }
    }
    return ESP_OK;
}

static esp_err_t algorithm_data_linear_amplication(int16_t *raw_buff, int len, int linear_factor)
{
    for (int i = 0; i < len / 2; i++) {
        raw_buff[i] *= linear_factor;
    }
    return ESP_OK;
}

static int algorithm_data_process_for_type1(audio_element_handle_t self)
{
    algo_stream_t *algo = (algo_stream_t *)audio_element_getdata(self);
    algorithm_data_info_t *record = &algo->record;
    algorithm_data_info_t *reference = &algo->reference;
    int in_ret = 0, bytes_consume = 0, out_len_bytes = record->rsp_info.out_len_bytes;
    char tmp_buffer[2 * AEC_FRAME_BYTES] = {0};

    if (record->data_need_be_resampled) { // When use input type1, use record or reference handle is the same.
        if (record->in_offset > 0) {
            memmove(record->rsp_in, &record->rsp_in[ALGORITHM_STREAM_RESAMPE_DEFAULT_MAX_INPUT_SIZE - record->in_offset], record->in_offset);
        }
        in_ret = audio_element_input(self, (char *)&record->rsp_in[record->in_offset], ALGORITHM_STREAM_RESAMPE_DEFAULT_MAX_INPUT_SIZE - record->in_offset);
        if (in_ret <= 0) {
            return in_ret;
        } else {
            record->in_offset += in_ret;
            bytes_consume = esp_resample_run(record->rsp_handle, (void *)&record->rsp_info,
                                             record->rsp_in, record->rsp_out,
                                             record->in_offset, &out_len_bytes);
            ESP_LOGD(TAG, "in_ret = %d, out_len_bytes = %d, bytes_consume = %d", in_ret, out_len_bytes, bytes_consume);

            if ((bytes_consume > 0) && (out_len_bytes == record->rsp_info.out_len_bytes)) {
                record->in_offset -= bytes_consume;
                algorithm_data_divided((int16_t *)record->rsp_out, out_len_bytes, reference->aec_buff, reference->ap_factor, record->aec_buff, record->ap_factor);
                return algorithm_process(self);
            } else {
                ESP_LOGE(TAG, "Fail to resample");
                return AEL_IO_FAIL;
            }
        }
    } else {
        in_ret = audio_element_input(self, tmp_buffer, AEC_FRAME_BYTES * 2);
        if (in_ret <= 0) {
            return in_ret;
        } else {
            algorithm_data_divided((int16_t *)tmp_buffer, AEC_FRAME_BYTES * 2, reference->aec_buff, reference->ap_factor, record->aec_buff, record->ap_factor);
            return algorithm_process(self);
        }
    }
}

static int algorithm_data_pre_process_for_type2(audio_element_handle_t self, algorithm_data_info_t *data_info, int input_buffer_index)
{
    algo_stream_t *algo = (algo_stream_t *)audio_element_getdata(self);
    int in_ret = 0, bytes_consume = 0, out_len_bytes = data_info->rsp_info.out_len_bytes, basic_frame_size;
    char tmp_buffer[2 * AEC_FRAME_BYTES] = {0};
    int16_t *input_buffer;

    if (algo->algo_mask & ALGORITHM_STREAM_USE_AEC) {
        input_buffer = data_info->aec_buff;
        basic_frame_size = AEC_FRAME_BYTES;
    } else {
        basic_frame_size = AGC_FRAME_BYTES;
        input_buffer = (int16_t *)algo->scale_buff;
    }

    if (data_info->data_need_be_resampled) {
        if (data_info->in_offset > 0) {
            memmove(data_info->rsp_in, &data_info->rsp_in[ALGORITHM_STREAM_RESAMPE_DEFAULT_MAX_INPUT_SIZE - data_info->in_offset], data_info->in_offset);
        }
        if (input_buffer_index < 0) {
            in_ret = audio_element_input(self, (char *)&data_info->rsp_in[data_info->in_offset], ALGORITHM_STREAM_RESAMPE_DEFAULT_MAX_INPUT_SIZE - data_info->in_offset);
        } else {
            in_ret = audio_element_multi_input(self, (char *)&data_info->rsp_in[data_info->in_offset], ALGORITHM_STREAM_RESAMPE_DEFAULT_MAX_INPUT_SIZE - data_info->in_offset, input_buffer_index, portMAX_DELAY);
        }
        if (in_ret <= 0) {
            return in_ret;
        } else {
            data_info->in_offset += in_ret;
            bytes_consume = esp_resample_run(data_info->rsp_handle, (void *)&data_info->rsp_info,
                                             data_info->rsp_in, data_info->rsp_out,
                                             data_info->in_offset, &out_len_bytes);

            ESP_LOGD(TAG, "in_ret = %d, out_len_bytes = %d, bytes_consume = %d", in_ret, out_len_bytes, bytes_consume);
            if ((bytes_consume > 0) && (out_len_bytes == data_info->rsp_info.out_len_bytes)) {
                data_info->in_offset -= bytes_consume;
                if (data_info->data_need_be_divided_after_rsp) {
                    algorithm_data_divided((int16_t *)data_info->rsp_out, out_len_bytes, input_buffer, data_info->ap_factor, NULL, 0);
                } else {
                    memcpy(input_buffer, data_info->rsp_out, out_len_bytes);
                    algorithm_data_linear_amplication(input_buffer, out_len_bytes, data_info->ap_factor);
                }
            } else {
                ESP_LOGE(TAG, "Fail to resample");
                return AEL_IO_FAIL;
            }
        }
    } else {
        if (data_info->data_need_be_divided_after_rsp) {
            if (input_buffer_index < 0) {
                in_ret = audio_element_input(self, tmp_buffer, basic_frame_size * 2);
            } else {
                in_ret = audio_element_multi_input(self, tmp_buffer, basic_frame_size * 2, input_buffer_index, portMAX_DELAY);
            }
            if (in_ret <= 0) {
                return in_ret;
            } else {
                algorithm_data_divided((int16_t *)tmp_buffer, basic_frame_size * 2, input_buffer, data_info->ap_factor, NULL, 0);
            }
        } else {
            if (input_buffer_index < 0) {
                in_ret = audio_element_input(self, (char *)input_buffer, basic_frame_size);
            } else {
                in_ret = audio_element_multi_input(self, (char *)input_buffer, basic_frame_size, input_buffer_index, portMAX_DELAY);
            }
            if (in_ret <= 0) {
                return in_ret;
            } else {
                algorithm_data_linear_amplication(input_buffer, basic_frame_size, data_info->ap_factor);
            }
        }
    }
    return basic_frame_size;
}

static int algorithm_data_process_for_type2(audio_element_handle_t self)
{
    int ret = 0;
    algo_stream_t *algo = (algo_stream_t *)audio_element_getdata(self);
    algorithm_data_info_t *record = &algo->record;
    algorithm_data_info_t *reference = &algo->reference;

    if (algo->algo_mask & ALGORITHM_STREAM_USE_AEC) {
        ret |= algorithm_data_pre_process_for_type2(self, reference, 0);
    }
    ret |= algorithm_data_pre_process_for_type2(self, record, -1);
    if (ret <= 0) {
        return ret;
    }
    return algorithm_process(self);
}

static audio_element_err_t _algo_process(audio_element_handle_t self, char *in_buffer, int in_len)
{
    int ret = ESP_OK;
    algo_stream_t *algo = (algo_stream_t *)audio_element_getdata(self);

    if (algo->record.is_after_reset_res_info) {
        ret = destroy_record_rsp_handle(algo);
        if (ret != ESP_OK) {
            return AEL_PROCESS_FAIL;
        }
        ret = create_record_rsp_handle(algo);
        if (ret != ESP_OK) {
            return AEL_PROCESS_FAIL;
        }
    }

    if (algo->reference.is_after_reset_res_info) {
        ret = destroy_reference_rsp_handle(algo);
        if (ret != ESP_OK) {
            return AEL_PROCESS_FAIL;
        }
        ret = create_reference_rsp_handle(algo);
        if (ret != ESP_OK) {
            return AEL_PROCESS_FAIL;
        }
    }

    if (algo->input_type == ALGORITHM_STREAM_INPUT_TYPE1) {
        ret = algorithm_data_process_for_type1(self);
    } else if (algo->input_type == ALGORITHM_STREAM_INPUT_TYPE2) {
        ret = algorithm_data_process_for_type2(self);
    } else {
        ESP_LOGE(TAG, "Type %d is not supported", algo->input_type);
        return AEL_IO_FAIL;
    }
    return ret;
}

static esp_err_t algorithm_resample_config(resample_info_t *rsp_cfg, algorithm_data_info_t *data_info, algorithm_stream_input_type_t type)
{
    AUDIO_NULL_CHECK(TAG, rsp_cfg, return ESP_FAIL);
    AUDIO_NULL_CHECK(TAG, data_info, return ESP_FAIL);

    rsp_cfg->dest_rate        = ALGORITHM_STREAM_DEFAULT_SAMPLE_RATE_HZ;
    rsp_cfg->dest_ch          = ALGORITHM_STREAM_DEFAULT_CHANNEL;                  // The encode resample cannot process diffrent channel
    rsp_cfg->mode             = RESAMPLE_ENCODE_MODE;
    rsp_cfg->sample_bits      = ALGORITHM_STREAM_DEFAULT_SAMPLE_BIT;
    rsp_cfg->max_indata_bytes = ALGORITHM_STREAM_RESAMPE_DEFAULT_MAX_INPUT_SIZE;   // The max input data maybe 48K 2ch --> 16k 1ch; so max_data = AEC_FRAME_BYTES * 6
    rsp_cfg->complexity       = ALGORITHM_STREAM_RESAMPE_DEFAULT_COMPLEXITY;
    rsp_cfg->type             = ESP_RESAMPLE_TYPE_AUTO;

    if (type == ALGORITHM_STREAM_INPUT_TYPE1) {
        data_info->data_need_be_divided_after_rsp = false;
        if (data_info->rsp_info.src_rate != ALGORITHM_STREAM_DEFAULT_SAMPLE_RATE_HZ) {
            data_info->data_need_be_resampled = true;
        } else {
            data_info->data_need_be_resampled = false;
        }
        return ESP_OK;
    }

    if ((data_info->rsp_info.src_rate == ALGORITHM_STREAM_DEFAULT_SAMPLE_RATE_HZ) && (data_info->rsp_info.src_ch == ALGORITHM_STREAM_DEFAULT_CHANNEL)) {
        data_info->data_need_be_resampled = false;
        data_info->data_need_be_divided_after_rsp = false;
    } else if (data_info->rsp_info.src_rate == ALGORITHM_STREAM_DEFAULT_SAMPLE_RATE_HZ) {
        data_info->data_need_be_resampled = false;
        data_info->data_need_be_divided_after_rsp = true;
    } else {
        data_info->data_need_be_resampled = true;
        if (data_info->rsp_info.src_ch == 2) {
            data_info->data_need_be_divided_after_rsp = true;
        } else if (data_info->rsp_info.src_ch == 1) {
            data_info->data_need_be_divided_after_rsp = false;
        } else {
            ESP_LOGE(TAG, "The channel number should be 0 or 1");
            return ESP_FAIL;
        }
    }
    return ESP_OK;
}

static esp_err_t _esp_algorithm_resample_create(algorithm_data_info_t *info, resample_info_t *rsp_cfg, int src_rate, int src_ch, bool is_divided_after_rsp, int8_t mask)
{
    AUDIO_NULL_CHECK(TAG, info, return ESP_FAIL);
    AUDIO_NULL_CHECK(TAG, rsp_cfg, return ESP_FAIL);

    rsp_cfg->src_rate = src_rate;
    rsp_cfg->dest_ch = src_ch;
    rsp_cfg->src_ch = src_ch;
    if (is_divided_after_rsp) {
        if (mask & ALGORITHM_STREAM_USE_AEC) {
            rsp_cfg->out_len_bytes = AEC_FRAME_BYTES * 2;
        } else {
            rsp_cfg->out_len_bytes = AGC_FRAME_BYTES * 2;
        }
    } else {
        if (mask & ALGORITHM_STREAM_USE_AEC) {
            rsp_cfg->out_len_bytes = AEC_FRAME_BYTES;
        } else {
            rsp_cfg->out_len_bytes = AGC_FRAME_BYTES;
        }
    }

    memcpy(&info->rsp_info, rsp_cfg, sizeof(resample_info_t));
    info->rsp_handle = esp_resample_create(rsp_cfg, &info->rsp_in, &info->rsp_out);
    AUDIO_NULL_CHECK(TAG, info->rsp_handle, {
        ESP_LOGE(TAG, "Fail to create recorder resample handle");
        return ESP_FAIL;
    });
    return ESP_OK;
}

static esp_err_t create_record_rsp_handle(algo_stream_t *algo)
{
    AUDIO_NULL_CHECK(TAG, algo, return ESP_FAIL);
    algorithm_data_info_t *record = &algo->record;

    resample_info_t rsp_cfg;
    algorithm_resample_config(&rsp_cfg, record, algo->input_type);

    algo->record.is_after_reset_res_info = false;
    if (algo->input_type == ALGORITHM_STREAM_INPUT_TYPE1) {
        if (record->data_need_be_resampled) {
            return _esp_algorithm_resample_create(record, &rsp_cfg, record->rsp_info.src_rate, 2, 1, ALGORITHM_STREAM_USE_AEC);
        } else {
            return ESP_OK;
        }
    }

    if (record->data_need_be_resampled) {
        return _esp_algorithm_resample_create(record, &rsp_cfg, record->rsp_info.src_rate, record->rsp_info.src_ch, record->data_need_be_divided_after_rsp, algo->algo_mask);
    }
    return ESP_OK;
}

static esp_err_t create_reference_rsp_handle(algo_stream_t *algo)
{
    AUDIO_NULL_CHECK(TAG, algo, return ESP_FAIL);
    algorithm_data_info_t *reference = &algo->reference;

    resample_info_t rsp_cfg;
    algorithm_resample_config(&rsp_cfg, reference, algo->input_type);

    algo->reference.is_after_reset_res_info = false;
    if (reference->data_need_be_resampled) {
        return _esp_algorithm_resample_create(reference, &rsp_cfg, reference->rsp_info.src_rate, reference->rsp_info.src_ch, reference->data_need_be_divided_after_rsp, algo->algo_mask);
    }
    return ESP_OK;
}

static esp_err_t destroy_record_rsp_handle(algo_stream_t *algo)
{
    AUDIO_NULL_CHECK(TAG, algo, return ESP_FAIL);
    algorithm_data_info_t *record = &algo->record;

    AUDIO_NULL_CHECK(TAG, record, return ESP_FAIL);
    if (record->rsp_handle != NULL) {
        esp_resample_destroy(record->rsp_handle);
        record->rsp_handle = NULL;
    }

    return ESP_OK;
}

static esp_err_t destroy_reference_rsp_handle(algo_stream_t *algo)
{
    AUDIO_NULL_CHECK(TAG, algo, return ESP_FAIL);
    algorithm_data_info_t *reference = &algo->reference;

    AUDIO_NULL_CHECK(TAG, reference, return ESP_FAIL);
    if (reference->rsp_handle != NULL) {
        esp_resample_destroy(reference->rsp_handle);
        reference->rsp_handle = NULL;
    }

    return ESP_OK;
}

ringbuf_handle_t algo_stream_get_multi_input_rb(audio_element_handle_t algo_handle)
{
    AUDIO_NULL_CHECK(TAG, algo_handle, return NULL);
    return audio_element_get_multi_input_ringbuf(algo_handle, 0);
}

audio_element_handle_t algo_stream_init(algorithm_stream_cfg_t *config)
{
    AUDIO_NULL_CHECK(TAG, config, return NULL);
    if ((config->rec_linear_factor <= 0) || (config->ref_linear_factor <= 0)) {
        ESP_LOGE(TAG, "The linear amplication factor should be greater than 0");
        return NULL;
    }
    if ((config->algo_mask < 0) || (config->algo_mask > ALGORITHM_STREAM_FULL_FUCTION_MASK)) { //
        ESP_LOGE(TAG, "Please choose a reasonable mask value");
        return NULL;
    }
    algo_stream_t *algo = (algo_stream_t *)audio_calloc(1, sizeof(algo_stream_t));
    AUDIO_NULL_CHECK(TAG, algo, return NULL);
    algorithm_data_info_t *record = &algo->record;
    algorithm_data_info_t *reference = &algo->reference;

    record->ap_factor = config->rec_linear_factor;
    reference->ap_factor = config->ref_linear_factor;

    audio_element_cfg_t cfg = DEFAULT_AUDIO_ELEMENT_CONFIG();
    cfg.open = _algo_open;
    cfg.close = _algo_close;
    cfg.process = _algo_process;
    cfg.task_stack = config->task_stack;
    cfg.task_prio = config->task_prio;
    cfg.task_core = config->task_core;
    cfg.multi_in_rb_num = config->input_type;
    cfg.tag = "algorithm";
    if (config->input_type == ALGORITHM_STREAM_INPUT_TYPE1) {
        if ((config->ref_sample_rate != config->rec_sample_rate) || (config->ref_ch != config->rec_ch)) {
            ESP_LOGE(TAG, "The frequence and channel number should be the same, please check about that!");
            audio_free(algo);
            return NULL;
        }
        if (config->algo_mask != (ALGORITHM_STREAM_USE_AEC | ALGORITHM_STREAM_USE_AGC | ALGORITHM_STREAM_USE_NS)) {
            ESP_LOGE(TAG, "When type1 is choosen, both these algorithms should be used");
            audio_free(algo);
            return NULL;
        }
    }

    cfg.buffer_len = AEC_FRAME_BYTES;
    algo->input_type = config->input_type;
    algo->algo_mask = config->algo_mask;
    algo->record.rsp_info.src_ch = config->rec_ch;
    algo->record.rsp_info.src_rate = config->rec_sample_rate;
    algo->reference.rsp_info.src_ch = config->ref_ch;
    algo->reference.rsp_info.src_rate = config->ref_sample_rate;
    audio_element_handle_t el = audio_element_init(&cfg);
    AUDIO_NULL_CHECK(TAG, el, {
        audio_free(algo);
        return NULL;
    });
    bool _success = true;
    _success &= (create_record_rsp_handle(algo) == ESP_OK);
    _success &= (create_reference_rsp_handle(algo) == ESP_OK);
    _success &= ((algo->scale_buff = audio_calloc(1, AEC_FRAME_BYTES + AGC_FRAME_BYTES)) != NULL);
    if (algo->algo_mask & ALGORITHM_STREAM_USE_AEC) {
        _success &= (
                        (algo->aec_buff = audio_calloc(1, AEC_FRAME_BYTES)) &&
                        (record->aec_buff = audio_calloc(1, AEC_FRAME_BYTES)) &&
                        (reference->aec_buff = audio_calloc(1, AEC_FRAME_BYTES))
                    );
    }
    if (algo->algo_mask & ALGORITHM_STREAM_USE_AGC) {
        _success &= ((algo->agc_buff = audio_calloc(1, AGC_FRAME_BYTES)) != NULL);
    }
    if (algo->algo_mask & ALGORITHM_STREAM_USE_NS) {
        _success &= ((algo->ns_buff = audio_calloc(1, NS_FRAME_BYTES)) != NULL);

    }
    AUDIO_NULL_CHECK(TAG, _success, {
        ESP_LOGE(TAG, "Error occured");
        _algo_close(el);
        audio_free(algo);
        return NULL;
    });

    audio_element_setdata(el, algo);
    return el;
}

esp_err_t algo_stream_set_record_rate(audio_element_handle_t algo_handle, int rec_ch, int rec_sample_rate)
{
    AUDIO_NULL_CHECK(TAG, algo_handle, return ESP_FAIL);
    algo_stream_t *algo = (algo_stream_t *)audio_element_getdata(algo_handle);

    if (algo->record.rsp_info.src_ch == rec_ch
        && algo->record.rsp_info.src_rate == rec_sample_rate) {
        return ESP_OK;
    }

    if (is_valid_algorithm_rsp_channel(rec_ch) != ESP_OK
        || is_valid_algorithm_rsp_samplerate(rec_sample_rate) != ESP_OK
        ) {
        return ESP_ERR_INVALID_ARG;
    } else {
        if (algo->input_type == ALGORITHM_STREAM_INPUT_TYPE1) {
            algo->reference.is_after_reset_res_info = true;
            algo->reference.rsp_info.src_ch = rec_ch;
            algo->reference.rsp_info.src_rate = rec_sample_rate;
        }

        algo->record.is_after_reset_res_info = true;
        algo->record.rsp_info.src_ch = rec_ch;
        algo->record.rsp_info.src_rate = rec_sample_rate;
        ESP_LOGI(TAG, "reset channel number of record signal : %d, sample rate of record signal: %d",
                 rec_ch, rec_sample_rate);
    }
    return ESP_OK;
}

esp_err_t algo_stream_set_reference_rate(audio_element_handle_t algo_handle, int ref_ch, int ref_sample_rate)
{
    AUDIO_NULL_CHECK(TAG, algo_handle, return ESP_FAIL);
    algo_stream_t *algo = (algo_stream_t *)audio_element_getdata(algo_handle);

    if (algo->reference.rsp_info.src_ch == ref_ch
        && algo->reference.rsp_info.src_rate == ref_sample_rate) {
        return ESP_OK;
    }

    if (is_valid_algorithm_rsp_channel(ref_ch) != ESP_OK
        || is_valid_algorithm_rsp_samplerate(ref_sample_rate) != ESP_OK
        ) {
        return ESP_ERR_INVALID_ARG;
    } else {
        if (algo->input_type == ALGORITHM_STREAM_INPUT_TYPE1) {
            algo->record.is_after_reset_res_info = true;
            algo->record.rsp_info.src_ch = ref_ch;
            algo->record.rsp_info.src_rate = ref_sample_rate;
        }

        algo->reference.is_after_reset_res_info = true;
        algo->reference.rsp_info.src_ch = ref_ch;
        algo->reference.rsp_info.src_rate = ref_sample_rate;
        ESP_LOGI(TAG, "reset channel number of reference signal: %d, sample rate of reference signal %d",
                 ref_ch, ref_sample_rate);
    }
    return ESP_OK;
}
