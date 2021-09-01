/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef AGC_H
#define AGC_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int32_t     bypass;
    int32_t     target_level;
    int32_t     compression_gain;
    int32_t     limiter_enable;
} AgcConfig;

struct AgcState_;

typedef struct AgcState_ AgcState;

AgcState *agc_state_create(int32_t sample_rate, int32_t frame_size, const AgcConfig *cfg);

int32_t agc_state_destroy(AgcState *st);

int32_t agc_set_config(AgcState *st, const AgcConfig *cfg);

int32_t agc_process(AgcState *st, int16_t *pcm_buf, int32_t pcm_len);

#ifdef __cplusplus
}
#endif

#endif