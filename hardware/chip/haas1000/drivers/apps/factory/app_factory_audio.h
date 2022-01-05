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
#ifndef __APP_FACTORY_BT_H__
#define __APP_FACTORY__BTH__

#include "app_utils.h"

typedef struct {
    uint16_t *buf;
    uint32_t len;
    uint32_t cuur_buf_pos;
}audio_test_pcmpatten_t;

int app_factorymode_audioloop(bool on, enum APP_SYSFREQ_FREQ_T freq);

int app_factorymode_output_pcmpatten(audio_test_pcmpatten_t *pcmpatten, uint8_t *buf, uint32_t len);

int app_factorymode_mic_cancellation_run(void * mic_st, signed short *inbuf, int sample);

void *app_factorymode_mic_cancellation_init(void* (* alloc_ext)(int));

#endif
