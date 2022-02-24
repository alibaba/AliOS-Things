/*
 * Copyright (c) 2021 Bestechnic (Shanghai) Co., Ltd. All rights reserved.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef ECHO_CANCELLER_H
#define ECHO_CANCELLER_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    int32_t bypass;
    int32_t hpf_enabled;
    int32_t af_enabled;
    int32_t nlp_enabled;
    int32_t clip_enabled;
    int32_t stsupp_enabled;
    int32_t ns_enabled;
    int32_t cng_enabled;
    // af config
    int32_t blocks;
    int32_t delay;
    // nlp config
    float   min_ovrd;
    float   target_supp;
    // ns config
    float   noise_supp;
    // cng config
    int32_t cng_type; // 0 - white noise, 1 - pink noise
    float   cng_level;
    // clip
    float   clip_threshold;
    int32_t banks;
} Ec2FloatConfig;

struct Ec2FloatState_;

typedef struct Ec2FloatState_ Ec2FloatState;

Ec2FloatState *ec2float_create(int sample_rate, int frame_size, const Ec2FloatConfig *cfg);

int32_t ec2float_destroy(Ec2FloatState *st);

int32_t ec2float_set_config(Ec2FloatState *st, const Ec2FloatConfig *cfg);

int32_t ec2float_process(Ec2FloatState *st, int16_t *pcm_in, int16_t *pcm_ref, int32_t pcm_len, int16_t *pcm_out);

int32_t ec2float_set_external_vad(Ec2FloatState *st, bool vad);

float ec2float_get_required_mips(Ec2FloatState *st);

#ifdef __cplusplus
}
#endif

#endif