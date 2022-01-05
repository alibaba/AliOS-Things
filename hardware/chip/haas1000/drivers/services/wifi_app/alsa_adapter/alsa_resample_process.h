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
#ifndef __ALSA_RESAMPLE_PROCESS_H__
#define __ALSA_RESAMPLE_PROCESS_H__

#include <stdint.h>
#include "hal_aud.h"
#include "audio_resample_ex.h"

#define ALSA_SRC_BUF_SIZE (12 * 1024)

int new_alsa_resample(enum AUD_CHANNEL_NUM_T channels, enum AUD_BITS_T bits,
                      uint32_t sample_rate_src, uint32_t sample_rate_dst);
void del_alsa_resample(int id);
int alsa_resample_process(int id, const int16_t *data_in, int16_t **data_out, uint32_t *in_out_len);

#endif