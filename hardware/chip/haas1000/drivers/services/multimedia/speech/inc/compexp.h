/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef COMPEXP_H
#define COMPEXP_H

#include <stdint.h>

typedef struct
{
    int32_t     bypass;
    float       comp_threshold;
    float       comp_ratio;
    float       expand_threshold;
    float       expand_ratio;
    float       attack_time;
    float       release_time;
    float       makeup_gain;
    int32_t     delay;
} CompexpConfig;

typedef struct CompexpState_ CompexpState;

#ifdef __cplusplus
extern "C" {
#endif

CompexpState *compexp_create(int32_t sample_rate, int32_t frame_size, const CompexpConfig *config);

int32_t compexp_destroy(CompexpState *st);

int32_t compexp_set_config(CompexpState *st, const CompexpConfig *cfg);

int32_t compexp_process(CompexpState *st, int16_t *pcm_buf, int32_t pcm_len);

float compexp_get_required_mips(CompexpState *st);

#ifdef __cplusplus
}
#endif

#endif