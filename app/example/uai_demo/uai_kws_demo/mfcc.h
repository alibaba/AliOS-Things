/*
 * Copyright (C) 2018 Arm Limited or its affiliates. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __KWS_MFCC_H__
#define __KWS_MFCC_H__

#include "arm_math.h"
#include "string.h"

#define SAMP_FREQ 16000
#define NUM_FBANK_BINS 40
#define MEL_LOW_FREQ 20
#define MEL_HIGH_FREQ 4000

#define M_2PI 6.283185307179586476925286766559005

static inline float InverseMelScale(float mel_freq) {
    return 700.0f * (expf (mel_freq / 1127.0f) - 1.0f);
}

static inline float MelScale(float freq) {
    return 1127.0f * logf (1.0f + freq / 700.0f);
}

typedef struct {
    int num_mfcc_features;
    int frame_len;
    int frame_shift;
    int frame_len_padded;
    int mfcc_dec_bits;
    float * frame;
    float * buffer;
    float * mel_energies;
    float * window_func;
    int32_t * fbank_filter_first;
    int32_t * fbank_filter_last;
    float ** mel_fbank;
    float * dct_matrix;
    arm_rfft_fast_instance_f32 * rfft;
    float *(*create_dct_matrix)(int32_t input_length, int32_t coefficient_count);
    float **(*create_mel_fbank)(void *mfccp);

    void (*mfcc_compute)(void *mfccp, const int16_t* data, q7_t* mfcc_out);
}MFCC;

void kws_mfcc_init(int num_mfcc_features, int frame_len, int frame_shift, int mfcc_dec_bits);
void kws_get_mfcc_features(int16_t *data, int16_t num_frames, int8_t *buffer);
int kws_get_top_class(int8_t *prediction, int8_t num);

#endif
