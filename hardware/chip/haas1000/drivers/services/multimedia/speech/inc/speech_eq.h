/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef SPEECH_EQ_H
#define SPEECH_EQ_H

#include "iirfilt.h"

// use more frame_size * sizeof(float) ram
#ifdef __arm__
#define EQ_USE_CMSIS_IIR
#endif

#define MAX_VQE_EQ_BAND 6

typedef struct
{
    enum IIR_BIQUARD_TYPE type;
    union
    {
        /* Raw config, used when type is IIR_BIQUARD_RAW */
        struct
        {
            float a1; float a2; float b0; float b1; float b2;
        } raw;
        /* Generate coeffs using user defined params, used in other type */
        struct
        {
            float f0; float gain; float q;
        } design;
    };
} BiquardParam;

typedef struct
{
    int32_t     bypass;
    float       gain;
    int32_t     num;
	BiquardParam params[MAX_VQE_EQ_BAND];
} EqConfig;

typedef struct EqState_ EqState;

#ifdef __cplusplus
extern "C" {
#endif

EqState *eq_init(int32_t sample_rate, int32_t frame_size, const EqConfig *cfg);

int32_t eq_destroy(EqState *st);

int32_t eq_set_config(EqState *st, const EqConfig *cfg);

int32_t eq_process(EqState *st, int16_t *pcm_buf, int32_t pcm_len);

int32_t eq_process_int24(EqState *st, int32_t *pcm_buf, int32_t pcm_len);

int32_t eq_process_float(EqState *st, float *pcm_buf, int32_t pcm_len);

int32_t eq_process2(EqState *st, int16_t *pcm_buf, int32_t pcm_len, int32_t stride);

int32_t eq_process2_int24(EqState *st, int32_t *pcm_buf, int32_t pcm_len, int32_t stride);

int32_t eq_process2_float(EqState *st, float *pcm_buf, int32_t pcm_len, int32_t stride);

float eq_get_required_mips(EqState *st);

#ifdef __cplusplus
}
#endif

#endif