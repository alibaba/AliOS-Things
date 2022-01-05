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
 * File: lightduer_thread_impl.h
 * Auth: Zhang Leliang(zhangleliang@baidu.com)
 * Desc: Provide the thread APIs.
 */

#ifndef BAIDU_DUER_LIGHTDUER_PORT_INCLUDE_LIGHTDUER_THREAD_IMPL_H
#define BAIDU_DUER_LIGHTDUER_PORT_INCLUDE_LIGHTDUER_THREAD_IMPL_H

#include "lightduer_types.h"

#ifdef __cplusplus
extern "C" {
#endif

duer_u32_t duer_get_thread_id_impl();

#ifdef __cplusplus
}
#endif

#endif // BAIDU_DUER_LIGHTDUER_PORT_INCLUDE_LIGHTDUER_THREAD_IMPL_H
