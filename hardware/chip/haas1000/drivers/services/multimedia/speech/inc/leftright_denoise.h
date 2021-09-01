/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef __LEFTRIGHT_DENOISE_H__
#define __LEFTRIGHT_DENOISE_H__

#include <stdint.h>
#include "ae_macros.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int32_t     bypass;
    float       delay_taps;             // MIC L/R delay samples. 0: 适用于麦克距离为<2cm; 1: 适用于麦克距离为2cm左右; 2: 适用于麦克距离为4cm左右
    int32_t     freq_smooth_enable;     // 1: 频域滤波打开; 0: 频域滤波关闭; 默认打开
    int32_t     wnr_enable;             // wind noise reduction enable
    int32_t     delay_enable;
} LeftRightDenoiseConfig;

struct LeftRightDenoiseState_;

typedef struct LeftRightDenoiseState_ LeftRightDenoiseState;

#ifndef LEFTRIGHT_DENOISE_IMPL
#define LEFTRIGHT_DENOISE_IMPL float
#endif

#define leftright_denoise_create    CONSTRUCT_FUNC_NAME(leftright_denoise, LEFTRIGHT_DENOISE_IMPL, create)
#define leftright_denoise_destroy    CONSTRUCT_FUNC_NAME(leftright_denoise, LEFTRIGHT_DENOISE_IMPL, destroy)
#define leftright_denoise_set_config CONSTRUCT_FUNC_NAME(leftright_denoise, LEFTRIGHT_DENOISE_IMPL, set_config)
#define leftright_denoise_process   CONSTRUCT_FUNC_NAME(leftright_denoise, LEFTRIGHT_DENOISE_IMPL, process)
#define leftright_denoise_get_delay   CONSTRUCT_FUNC_NAME(leftright_denoise, LEFTRIGHT_DENOISE_IMPL, get_delay)

LeftRightDenoiseState *leftright_denoise_create(int32_t sample_rate, int32_t frame_size, const LeftRightDenoiseConfig *cfg);

int32_t leftright_denoise_destroy(LeftRightDenoiseState *st);

int32_t leftright_denoise_set_config(LeftRightDenoiseState *st, const LeftRightDenoiseConfig *cfg);

int32_t leftright_denoise_process(LeftRightDenoiseState *st, short *pcm_buf, int32_t pcm_len, short *out_buf);

int32_t leftright_denoise_get_delay(LeftRightDenoiseState *st);

#ifdef __cplusplus
}
#endif

#endif
