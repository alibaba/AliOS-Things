/* Copyright 2018 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/
#include <cstring>
#include <stdint.h>
#include "aos/kernel.h"
#include "micro_speech/audio_provider.h"
#include "micro_speech/micro_features/micro_model_settings.h"
#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus
#include "uvoice_types.h"
#include "uvoice_init.h"
#include "uvoice_recorder.h"
#include "uvoice_player.h"
#include "ulog/ulog.h"
#include "mcu_audio.h"
#include "data_dump.h"
#include "ringbuf.h"
#include "recorder/recorder.h"
#include "echo_canceller.h"
#include "speech_2mic_ns6.h"
#include "ext_heap.h"
#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus

#define TAG "micro_speech"

#define USE_RING_BUFFER
#define NOT_COPY_HISTORY
#define USE_ECHO_CANCELLATION
// #define USE_UVOICE_RECORDER
// #define PCM_DUMP_DEBUG

#define APP_KWS_BITS 16

namespace
{
    constexpr int kAudioCaptureBufferSize = 10 * 1024;
    int16_t g_captured_audio_data[kAudioCaptureBufferSize];
    static int32_t g_audio_capture_data_start = 0;
    int16_t g_output_audio_data[kMaxAudioSampleSize];
    int32_t g_latest_audio_timestamp = 0;
    int64_t g_total_samples_captured = 0;
    int sample_bytes = APP_KWS_BITS / 8;
    const int duration_sample_count =
        30 /* ms */ * (kAudioSampleFrequency / 1000);
    bool g_is_audio_initialized = false;
} // namespace

static void *handle = NULL;
static aos_task_t capture_task_handle;
static ringbuf_t *g_audio_capture_buffer;

constexpr int32_t new_samples_to_get =
    (kFeatureSliceStrideMs * (kAudioSampleFrequency / 1000));
constexpr int32_t history_samples_to_keep =
    ((kFeatureSliceDurationMs - kFeatureSliceStrideMs) *
     (kAudioSampleFrequency / 1000));
static int16_t g_history_buffer[history_samples_to_keep];

const Speech2MicNs6Config ns6_cfg = {
    .bypass = 0,
    .wnr_enable = 0,
    .denoise_dB = -6,
};

Speech2MicNs6State *ns_st = NULL;
Ec2FloatState *ec_st_l = NULL;
Ec2FloatState *ec_st_r = NULL;
const Ec2FloatConfig ec_cfg = {
    .bypass = 0,
    .hpf_enabled = false,
    .af_enabled = true,
    .nlp_enabled = false,
    .clip_enabled = false,
    .stsupp_enabled = false,
    .ns_enabled = false,
    .cng_enabled = false,
    .blocks = 4,
    .delay = 0,
    .min_ovrd = 2,
    .target_supp = -35,
    .noise_supp = -12,
    .cng_type = 1,
    .cng_level = -60,
    .clip_threshold = -20.f,
    .banks = 64,
};

static int pcm_stream_read(void *hdl, void *buf, unsigned int len)
{
    int ret = -1, i = 0, j = 0;
    int channel_num = 3, frame_size;
    char *tempBuf = NULL;
    char *mic1_pcm_tmp_buf = NULL;
    char *mic2_pcm_tmp_buf = NULL;
    char *mic3_pcm_tmp_buf = NULL;
#ifdef USE_ECHO_CANCELLATION
    short *mic1_pcm_buf = NULL;
    short *mic2_pcm_buf = NULL;
    short *mic3_pcm_buf = NULL;
#endif

#ifdef USE_UVOICE_RECORDER
    ret = recorder_get_stream((uint8_t *)buf, len);
    return ret;
#else
    if (NULL == hdl)
    {
        LOGE(TAG, "%s:%d, invalid hdl.", __func__, __LINE__);
        return ret;
    }

    if (hdl == handle)
    {
        frame_size = channel_num * sample_bytes;
        tempBuf = (char *)malloc(len * channel_num);
#ifdef USE_ECHO_CANCELLATION
        mic2_pcm_tmp_buf = (char *)malloc(len);
        mic3_pcm_tmp_buf = (char *)malloc(len);
#endif
        if (tempBuf)
        {
            ret = data_dump_read(hdl, tempBuf, len * channel_num);
            for (i = 0; i < ret / frame_size; i++)
            {
                for (j = 0; j < sample_bytes; j++)
                {
                    /*  MIC阵列数据：MIC1_B1, MIC1_B0, MIC2_B1, MIC2_B0, MIC3_B1, MIC3_B0 ... */
                    ((char *)buf)[i * sample_bytes + j] = tempBuf[i * frame_size + j]; // MIC1
                    // ((char *)mic1_pcm_tmp_buf)[i * sample_bytes + j] = tempBuf[i * frame_size + j]; // MIC1
#ifdef USE_ECHO_CANCELLATION
                    ((char *)mic2_pcm_tmp_buf)[i * sample_bytes + j] = tempBuf[i * frame_size + j]; // MIC2
                    ((char *)mic3_pcm_tmp_buf)[i * sample_bytes + j] = tempBuf[i * frame_size + j]; // MIC3
#endif
                }
            }
            ret = ret / channel_num;
        }
#ifdef USE_ECHO_CANCELLATION
        mic1_pcm_buf = (short *)buf;
        mic2_pcm_buf = (short *)mic2_pcm_tmp_buf;
        mic3_pcm_buf = (short *)mic3_pcm_tmp_buf;

        for (int j = 0; j < 2; j++)
        {
            ec2float_process(ec_st_l, &mic1_pcm_buf[j * 128], &mic3_pcm_buf[j * 128], 128, &mic1_pcm_buf[j * 128]); // MIC1 回声消除
            // ec2float_process(ec_st_r, &mic1_pcm_buf[j*128], &mic2_pcm_buf[j*128], 128, &mic1_pcm_buf[j*128]);
            // speech_2mic_ns6_process(ns_st, &mic1_pcm_buf[j*128], &mic2_pcm_buf[j*128], 128); // 降噪
        }
#endif
        //LOGD(TAG, "%s:%d， data_dump_read %d bytes", __func__, __LINE__, ret);
    }

    free(tempBuf);
#ifdef USE_ECHO_CANCELLATION
    free(mic2_pcm_buf);
    free(mic3_pcm_buf);
#endif
#endif //USE_UVOICE_RECORDER
    return ret;
}

static void pcm_data_dump(char *buf, int len)
{
    /*dump audio data*/
    FILE *file = fopen("/sdcard/record.pcm", "ab");
    if ((file != NULL) && fwrite((void *)buf, len, 1, file) < 1)
    {
        LOGE(TAG, "write file buf fail\n");
        return;
    }
    fclose(file);
}

static void CaptureSamples(void *arg)
{
    size_t bytes_read;
    char *tempBuf = NULL;
    int32_t read_len;

    tempBuf = (char *)malloc(duration_sample_count * sample_bytes);

    while (1)
    {
        memset(tempBuf, 0, sizeof(tempBuf));
        read_len = pcm_stream_read(handle, (void *)tempBuf, duration_sample_count * sample_bytes);
#ifdef AUDIO_DEBUG
        LOG("read_len: %d\n", read_len);
#endif
#ifdef PCM_DUMP_DEBUG
        pcm_data_dump(tempBuf, read_len);
#endif
        if (read_len > 0)
        {
            /* write bytes read from mic into ring buffer */
            int bytes_written = rb_write(g_audio_capture_buffer,
                                         (uint8_t *)tempBuf, read_len, 10);
            /* update the timestamp (in ms) to let the model know that new data has
        * arrived */
            g_latest_audio_timestamp +=
                ((1000 * (bytes_written / 2)) / kAudioSampleFrequency);
            // LOG("g_latest_audio_timestamp: %d\n", g_latest_audio_timestamp);
            if (bytes_written <= 0)
            {
                LOGE(TAG, "Could Not Write in Ring Buffer: %d ", bytes_written);
            }
            else if (bytes_written < bytes_read)
            {
                LOGE(TAG, "Partial Write");
            }
        }
    }
    free(tempBuf);
    data_dump_close(handle, aud_record_unsubscribe);
    speech_2mic_ns6_destroy(ns_st);
    ec2float_destroy(ec_st_l);
    ec2float_destroy(ec_st_r);
    aos_task_delete(&capture_task_handle);
}

TfLiteStatus InitAudioRecording(tflite::ErrorReporter *error_reporter)
{

    uint8_t *read_buffer = NULL;
    int channels = 1;
    int rate = 16000;
    int bits = 16;
    int buffer_size;
    int i = 0;

    g_audio_capture_buffer = rb_init("tf_ringbuffer", kAudioCaptureBufferSize);
    if (!g_audio_capture_buffer)
    {
        LOGE(TAG, "Error creating ring buffer");
        return kTfLiteError;
    }
    ext_heap_init();
    ec_st_l = ec2float_create(16000, 128, &ec_cfg);
    ec_st_r = ec2float_create(16000, 128, &ec_cfg);
    ns_st = speech_2mic_ns6_create(16000, 128, &ns6_cfg);
#ifdef USE_UVOICE_RECORDER
    recorder_init();
    recorder_start(MEDIA_FMT_PCM, rate, channels, bits, duration_sample_count, NULL);
#else
    handle = data_dump_open(aud_record_subscribe, 64, rate, 1);
#endif
    aos_task_new_ext(&capture_task_handle,
                     "CaptureSamples", CaptureSamples,
                     NULL, 1024 * 10, AOS_DEFAULT_APP_PRI);

    while (!g_latest_audio_timestamp)
    {
        aos_msleep(50);
    }
    return kTfLiteOk;
}

TfLiteStatus GetAudioSamples(tflite::ErrorReporter *error_reporter,
                             int start_ms, int duration_ms,
                             int *audio_samples_size, int16_t **audio_samples)
{
    if (!g_is_audio_initialized)
    {
        TfLiteStatus init_status = InitAudioRecording(error_reporter);
        if (init_status != kTfLiteOk)
        {
            return init_status;
        }
        g_is_audio_initialized = true;
    }
#ifdef NOT_COPY_HISTORY

#else
    /* copy 160 samples (320 bytes) into output_buff from history */
    memcpy((void *)(g_output_audio_data), (void *)(g_history_buffer),
           history_samples_to_keep * sizeof(int16_t));
#endif
    /* copy 320 samples (640 bytes) from rb at ( int16_t*(g_audio_output_buffer) +
   * 160 ), first 160 samples (320 bytes) will be from history */

#ifdef NOT_COPY_HISTORY
    const int duration_sample_count =
        duration_ms * (kAudioSampleFrequency / 1000);
    int32_t bytes_read =
        rb_read(g_audio_capture_buffer,
                ((uint8_t *)g_output_audio_data),
                duration_sample_count * sizeof(int16_t),
                10);
#else
    int32_t bytes_read =
        rb_read(g_audio_capture_buffer,
                ((uint8_t *)(g_output_audio_data + history_samples_to_keep)),
                new_samples_to_get * sizeof(int16_t), 10);
#endif
#ifdef AUDIO_DEBUG
    LOG("bytes_read : %d\n", bytes_read);
#endif
#ifdef PCM_DUMP_DEBUG
    pcm_data_dump((char *)(g_output_audio_data + history_samples_to_keep), new_samples_to_get * sizeof(int16_t));
#endif
#ifdef NOT_COPY_HISTORY

#else
    if (bytes_read < 0)
    {
        LOGE(TAG, " Model Could not read data from Ring Buffer");
    }
    else if (bytes_read < new_samples_to_get * sizeof(int16_t))
    {
        LOGE(TAG, "RB FILLED RIGHT NOW IS %d",
             rb_filled(g_audio_capture_buffer));
        LOGE(TAG, " Partial Read of Data by Model ");
        LOGE(TAG, " Could only read %d bytes when required %d bytes ",
             bytes_read, new_samples_to_get * sizeof(int16_t));
    }
#endif
#ifdef NOT_COPY_HISTORY

#else
    /* copy 320 bytes from output_buff into history */
    memcpy((void *)(g_history_buffer),
           (void *)(g_output_audio_data + new_samples_to_get),
           history_samples_to_keep * sizeof(int16_t));
#endif
#ifdef PCM_DUMP_DEBUG
    pcm_data_dump((char *)g_output_audio_data, 960);
#endif
    *audio_samples_size = kMaxAudioSampleSize;
    *audio_samples = g_output_audio_data;
    return kTfLiteOk;
}

int32_t LatestAudioTimestamp()
{
    //g_latest_audio_timestamp += 100;
    //g_latest_audio_timestamp = g_total_samples_captured * 1000 / kAudioSampleFrequency + 100;
    return g_latest_audio_timestamp;
}
