/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef __SPEECH_2MIC_NS2_DENOISE_H__
#define __SPEECH_2MIC_NS2_DENOISE_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int32_t     bypass;
    float       delay_taps;             // MIC L/R delay samples. 0: 适用于麦克距离为<2cm; 1: 适用于麦克距离为2cm左右; 2: 适用于麦克距离为4cm左右
    int32_t     freq_smooth_enable;     // 1: 频域滤波打开; 0: 频域滤波关闭; 默认打开
    int32_t     wnr_enable;             // wind noise reduction enable
} Speech2MicNs2Config;

struct Speech2MicNs2State_;

typedef struct Speech2MicNs2State_ Speech2MicNs2State;

#define CONSTRUCT_FUNC_NAME_A(p, c, m)          p ## _ ## c ## _ ## m
#define CONSTRUCT_FUNC_NAME(p, c, m)            CONSTRUCT_FUNC_NAME_A(p, c, m)

#ifndef SPEECH_2MIC_NS2_DENOISE_IMPL
#define SPEECH_2MIC_NS2_DENOISE_IMPL float
#endif

#define speech_2mic_ns2_create    CONSTRUCT_FUNC_NAME(speech_2mic_ns2, SPEECH_2MIC_NS2_DENOISE_IMPL, create)
#define speech_2mic_ns2_destroy    CONSTRUCT_FUNC_NAME(speech_2mic_ns2, SPEECH_2MIC_NS2_DENOISE_IMPL, destroy)
#define speech_2mic_ns2_set_config CONSTRUCT_FUNC_NAME(speech_2mic_ns2, SPEECH_2MIC_NS2_DENOISE_IMPL, set_config)
#define speech_2mic_ns2_process   CONSTRUCT_FUNC_NAME(speech_2mic_ns2, SPEECH_2MIC_NS2_DENOISE_IMPL, process)
#define speech_2mic_ns2_get_delay   CONSTRUCT_FUNC_NAME(speech_2mic_ns2, SPEECH_2MIC_NS2_DENOISE_IMPL, get_delay)
#define speech_2mic_ns2_get_required_mips   CONSTRUCT_FUNC_NAME(speech_2mic_ns2, SPEECH_2MIC_NS2_DENOISE_IMPL, get_required_mips)

Speech2MicNs2State *speech_2mic_ns2_create(int32_t sample_rate, int32_t frame_size, const Speech2MicNs2Config *cfg);

int32_t speech_2mic_ns2_destroy(Speech2MicNs2State *st);

int32_t speech_2mic_ns2_set_config(Speech2MicNs2State *st, const Speech2MicNs2Config *cfg);

int32_t speech_2mic_ns2_process(Speech2MicNs2State *st, short *pcm_buf, int32_t pcm_len, short *out_buf);

int32_t speech_2mic_ns2_get_delay(Speech2MicNs2State *st);

float speech_2mic_ns2_get_required_mips(Speech2MicNs2State *st);

#ifdef __cplusplus
}
#endif

#endif
