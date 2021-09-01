/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef __SPEECH_3MIC_NS_H__
#define __SPEECH_3MIC_NS_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int32_t     bypass;
	int32_t     blend_en;
	int32_t     authen_en;
    float     	delay_tapsM;            // MIC L/R delay samples. 0: 适用于麦克距离为<2cm; 1: 适用于麦克距离为2cm左右; 2: 适用于麦克距离为4cm左右
    int32_t     delay_tapsS;
	float       denoise_dB;
    // float       coefH[2][5];            // {{a0,a1,a2,a3,a4},{b0,b1,b2,b3,b4}}
    // float       coefL[2][5];            // {{a0,a1,a2,a3,a4},{b0,b1,b2,b3,b4}}
	int32_t     crossover_freq;
    int32_t     freq_smooth_enable;     // 1: 频域滤波打开; 0: 频域滤波关闭; 默认打开
    int32_t     wnr_enable;             // wind noise reduction enable
} Speech3MicNsConfig;

struct Speech3MicNsState_;

typedef struct Speech3MicNsState_ Speech3MicNsState;

Speech3MicNsState *speech_3mic_ns_create(int32_t sample_rate, int32_t frame_size, const Speech3MicNsConfig *cfg);

int32_t speech_3mic_ns_destroy(Speech3MicNsState *st);

int32_t speech_3mic_ns_set_config(Speech3MicNsState *st, const Speech3MicNsConfig *cfg);

int32_t speech_3mic_ns_process(Speech3MicNsState *st, short *pcm_buf, int32_t pcm_len, short *out_buf);

#ifdef __cplusplus
}
#endif

#endif
