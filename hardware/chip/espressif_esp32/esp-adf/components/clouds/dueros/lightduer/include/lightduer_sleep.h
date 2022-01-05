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
// Author: Chang Li(changli@baidu.com)
//
// Description: The sleep APIs.

#ifndef BAIDU_IOT_TINYDU_IOT_OS_SRC_IOT_BAIDU_CA_SOURCE_BAIDU_CA_SLEEP_H
#define BAIDU_IOT_TINYDU_IOT_OS_SRC_IOT_BAIDU_CA_SOURCE_BAIDU_CA_SLEEP_H

#include "lightduer_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * sleep function
 *
 * @param ms, the sleep in ms.
 */
DUER_INT void duer_sleep(duer_u32_t ms);

/*
 * The sleep callbacks
 */
typedef void (*duer_sleep_f_t)(duer_u32_t ms);

/*
 * Initial the sleep callbacks for Baidu CA
 *
 * @Param _f_sleep, in, sleep function implementation from different platform
 */
DUER_EXT void baidu_ca_sleep_init(duer_sleep_f_t _f_sleep);

#ifdef __cplusplus
}
#endif

#endif // BAIDU_IOT_TINYDU_IOT_OS_SRC_IOT_BAIDU_CA_SOURCE_BAIDU_CA_SLEEP_H
