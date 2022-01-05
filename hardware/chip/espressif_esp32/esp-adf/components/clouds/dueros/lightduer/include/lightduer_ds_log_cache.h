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
/*
 * Auth: Leliang Zhang(zhangleliang@baidu.com)
 * Desc: trace log cache, cache the trace log when ca disconnect
 */

#ifndef BAIDU_DUER_LIGHTDUER_DS_LOG_CACHE_H
#define BAIDU_DUER_LIGHTDUER_DS_LOG_CACHE_H

#include "baidu_json.h"
#include "lightduer_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * try to cache the tracelog info
 *  @param code: the trace log code, see lightduer_ds_log.h for more info
 *  @param message: the additional message for the @code
 *  @param timestamp: the timestamp when generate the trace log
 */
duer_status_t duer_ds_log_cache_push(duer_u32_t code, const baidu_json *message, duer_u32_t timestamp);

/**
 *  invoke this after CA connected, try to report the tracelog cached when CA disconnect.
 */
duer_status_t duer_ds_log_cache_report(void);

/**
 * initialize the resource
 */
duer_status_t duer_ds_log_cache_initialize(void);

/**
 * release the resources
 */
duer_status_t duer_ds_log_cache_finalize(void);

#ifdef __cplusplus
}
#endif

#endif // BAIDU_DUER_LIGHTDUER_DS_LOG_CACHE_H
