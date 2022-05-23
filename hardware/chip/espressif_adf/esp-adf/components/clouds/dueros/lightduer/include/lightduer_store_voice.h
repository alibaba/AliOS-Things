/**
 * Copyright (2017) Baidu Inc. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
/**
 * File: lightduer_store_voice.h
 * Auth: Zhang Leliang (zhangleliang@baidu.com)
 * Desc: the voice/speex store interface,
 *       different platform with different implementations,
         make sure its implementation before use
 */
#ifndef BAIDU_DUER_LIGHTDUER_INCLUDE_LIGHTDUER_STORE_VOICE_H
#define BAIDU_DUER_LIGHTDUER_INCLUDE_LIGHTDUER_STORE_VOICE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Initialise store
 *
 * @param name_id: use this to identify the differernt voices
 *
 * @return int: Success: 0
 *              Failed:  Other
 */
int duer_store_voice_start(int name_id);

/*
 * store the voice data, e.g. PCM data
 *
 * @param data: voice data
 * @param size: data size
 *
 * @return int: Success: 0
 *              Failed:  Other
 */
int duer_store_voice_write(const void *data, uint32_t size);

/*
 * store the speex data
 *
 * @param data: speex data
 * @param size: data size
 *
 * @return int: Success: 0
 *              Failed:  Other
 */
int duer_store_speex_write(const void *data, uint32_t size);

/*
 * finilize store
 *
 * @return int: Success: 0
 *              Failed:  Other
 */
int duer_store_voice_end(void);

#ifdef __cplusplus
}
#endif

#endif /*BAIDU_DUER_LIGHTDUER_INCLUDE_LIGHTDUER_STORE_VOICE_H*/
