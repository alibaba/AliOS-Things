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
 * File: lightduer_thread.h
 * Auth: Zhang Leliang(zhangleliang@baidu.com)
 * Desc: Provide the thread APIs.
 */

#ifndef LIBDUER_DEVICE_FRAMEWORK_CORE_LIGHTDUER_THREAD_H
#define LIBDUER_DEVICE_FRAMEWORK_CORE_LIGHTDUER_THREAD_H

#include "lightduer_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * get the thread id
 */
duer_u32_t duer_get_thread_id(void);

/*
 * The thread callbacks
 */
typedef duer_u32_t (*duer_get_thread_id_f_t)();

/*
 * Initial the thread callbacks for lightduer
 *
 * @Param _f_get_thread_id, in, get threadid  implementation from different platform
 */
DUER_EXT void duer_thread_init(duer_get_thread_id_f_t _f_get_thread_id);

#ifdef __cplusplus
}
#endif

#endif // LIBDUER_DEVICE_FRAMEWORK_CORE_LIGHTDUER_THREAD_H
