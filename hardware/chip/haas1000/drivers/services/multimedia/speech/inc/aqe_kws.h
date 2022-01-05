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
#ifndef __AQE_KWS_H__
#define __AQE_KWS_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int32_t     bypass;
} AqeKwsConfig;

struct AqeKwsState_;
typedef struct AqeKwsState_ AqeKwsState;

AqeKwsState *aqe_kws_create(uint32_t sample_rate, uint32_t frame_size, AqeKwsConfig *cfg);
int32_t aqe_kws_destroy(AqeKwsState *st);
int32_t aqe_kws_process(AqeKwsState *st, int16_t *pcm_in, uint32_t pcm_len, int16_t *thres_buf);

float aqe_kws_get_required_mips(AqeKwsState *st);

#ifdef __cplusplus
}
#endif

#endif