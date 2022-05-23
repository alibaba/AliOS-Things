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
// Author: Su Hao (suhao@baidu.com)
//
// Description: The APIs for debug.

#ifndef BAIDU_IOT_TINYDU_IOT_OS_SRC_IOT_BAIDU_CA_SOURCE_BAIDU_CA_DEBUG_H
#define BAIDU_IOT_TINYDU_IOT_OS_SRC_IOT_BAIDU_CA_SOURCE_BAIDU_CA_DEBUG_H

#include "lightduer_types.h"

#ifndef DUER_DEBUG_BUF_SIZE
#ifdef DUER_RELEASE_VERSION
#define DUER_DEBUG_BUF_SIZE (64)
#else
#define DUER_DEBUG_BUF_SIZE  (512)
#endif
#endif // DUER_DEBUG_BUF_SIZE

#ifdef __cplusplus
extern "C" {
#endif

/*
 * The debug output
 */
typedef void (*duer_debug_f)(duer_context ctx,
                            duer_u32_t level,
                            const char* file,
                            duer_u32_t line,
                            const char* fmt);

/*
 * Set the debug output
 *
 * @Param ctx, duer_context, the debug context for user
 * @Param f_debug, duer_debug_f, the debug output function
 */
DUER_EXT void baidu_ca_debug_init(duer_context ctx, duer_debug_f f_debug);

DUER_INT void duer_debug_print(duer_u32_t level, const char* file,
                             duer_u32_t line, const char* msg);

DUER_INT void duer_debug(duer_u32_t level, const char* file, duer_u32_t line,
                       const char* fmt, ...);

DUER_INT void duer_dump(duer_u32_t level, const char* file, duer_u32_t line,
                        const char *tag, const void *data, duer_size_t size);

#ifdef __cplusplus
}
#endif

#endif // BAIDU_IOT_TINYDU_IOT_OS_SRC_IOT_BAIDU_CA_SOURCE_BAIDU_CA_DEBUG_H
