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
#ifndef __ALSA_DUMP_H__
#define __ALSA_DUMP_H__

#ifdef ALSA_DUMP_EN

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint8_t id;
    uint32_t sample_rate;
    uint8_t channels;
    uint8_t bits;
} alsa_dump_info_t;

void *alsa_dump_if_open(const char *ip_str, alsa_dump_info_t * info);
int alsa_dump_if_close(void *h);
int alsa_dump_if_dump(void *h, uint8_t *buf, uint32_t len);
int alsa_dump_local_printf(uint8_t * buf, uint32_t len);

#ifdef ALSA_DUMP_EN
void alsa_dump_point_init(alsa_handle_t *h);
void alsa_dump_point_denint(alsa_handle_t *h);
void alsa_dump_point_write(alsa_handle_t *h, uint8_t *buf, uint32_t len);
void alsa_dump_point_after_sw_gain(uint8_t *buf, uint32_t len);
void alsa_dump_point_after_audio_process(uint8_t *buf, uint32_t len);
void alsa_dump_point_mixer(uint8_t *buf, uint32_t len);
#endif


#ifdef __cplusplus
}
#endif

#endif /* ALSA_DUMP_EN */

#endif