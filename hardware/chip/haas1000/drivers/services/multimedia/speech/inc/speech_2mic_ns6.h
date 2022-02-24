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

#ifndef __SPEECH_2MIC_NS6_H__
#define __SPEECH_2MIC_NS6_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int32_t     bypass;
    int32_t     wnr_enable;
	int32_t     denoise_dB;
} Speech2MicNs6Config;

struct Speech2MicNs6State_;
typedef struct Speech2MicNs6State_ Speech2MicNs6State;

Speech2MicNs6State *speech_2mic_ns6_create(int sample_rate, int frame_size, const Speech2MicNs6Config *cfg);
int32_t speech_2mic_ns6_destroy(Speech2MicNs6State *st);
int32_t speech_2mic_ns6_process(Speech2MicNs6State *st, short *channel1,short *channel2, int32_t pcm_len);

float speech_2mic_ns6_get_required_mips(Speech2MicNs6State *st);

#ifdef __cplusplus
}
#endif

#endif