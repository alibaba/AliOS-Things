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
#ifndef __AUDIO_CFG_H__
#define __AUDIO_CFG_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"

enum AUDIO_PROCESS_TYPE_T {
    AUDIO_PROCESS_TYPE_IIR_EQ = 0,
    AUDIO_PROCESS_TYPE_DRC,
    AUDIO_PROCESS_TYPE_LIMITER,
    AUDIO_PROCESS_TYPE_NUM,
};

struct AUDIO_CFG_T_;
typedef struct AUDIO_CFG_T_ AUDIO_CFG_T;

int sizeof_audio_cfg(void);
int store_audio_cfg_into_audio_section(AUDIO_CFG_T *cfg);
void *load_audio_cfg_from_audio_section(enum AUDIO_PROCESS_TYPE_T type);

#ifdef __cplusplus
}
#endif

#endif
