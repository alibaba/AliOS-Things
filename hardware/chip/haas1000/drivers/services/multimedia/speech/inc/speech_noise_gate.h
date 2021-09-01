/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef _SPEECH_NOISE_GATE_H_
#define _SPEECH_NOISE_GATE_H_

#include <stdint.h>

typedef struct {
    int32_t     bypass;
    int32_t     data_threshold;
    int32_t     data_shift;
    float       factor_up;
    float       factor_down;
} NoisegateConfig;

struct NoisegateState_;

typedef struct NoisegateState_ NoisegateState;

#ifdef __cplusplus
extern "C" {
#endif

NoisegateState* speech_noise_gate_create(int32_t sample_rate, int32_t frame_size, const NoisegateConfig *cfg);
int32_t speech_noise_gate_destroy(NoisegateState *st);
int32_t speech_noise_gate_set_config(NoisegateState *st, const NoisegateConfig *cfg);

int32_t speech_noise_gate_process(NoisegateState *st, int16_t *pcm_buf, int32_t pcm_len);

#ifdef __cplusplus
}
#endif

#endif
