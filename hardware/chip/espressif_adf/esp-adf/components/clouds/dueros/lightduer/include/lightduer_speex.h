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
 * File: lightduer_speex.h
 * Auth: Su Hao (suhao@baidu.com)
 * Desc: Light duer Speex encoder.
 */

#ifndef BAIDU_DUER_LIGHTDUER_INCLUDE_LIGHTDUER_SPEEX_H
#define BAIDU_DUER_LIGHTDUER_INCLUDE_LIGHTDUER_SPEEX_H

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void *duer_speex_handler;

typedef void (*duer_encoded_func)(const void *, size_t);

duer_speex_handler duer_speex_create(int rate, int quality);

void duer_speex_encode(duer_speex_handler handler, const void *data, size_t size, duer_encoded_func func);

void duer_speex_destroy(duer_speex_handler handler);

#ifdef __cplusplus
}
#endif

#endif/*BAIDU_DUER_LIGHTDUER_INCLUDE_LIGHTDUER_SPEEX_H*/
