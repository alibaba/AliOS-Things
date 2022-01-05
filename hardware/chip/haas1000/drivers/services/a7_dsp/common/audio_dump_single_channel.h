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
#ifndef _AUDIO_DUMP_SINGLE_CHANNEL_H_
#define _AUDIO_DUMP_SINGLE_CHANNEL_H_
#include "mcu_audio.h"
#ifdef __cplusplus
extern "C" {
#endif

void audio_dump_a7_cb_register(aud_dump_cb_t cbf, void *param);

void audio_dump_a7_cb_unregister(aud_dump_cb_t cbf, void *param);

#ifdef __cplusplus
}
#endif

#endif /* _AUDIO_DUMP_SINGLE_CHANNEL_H_ */
