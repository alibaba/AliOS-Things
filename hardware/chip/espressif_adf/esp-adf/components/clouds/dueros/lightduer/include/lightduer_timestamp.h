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
// Description: The timestamp APIs.

#ifndef BAIDU_IOT_TINYDU_IOT_OS_SRC_IOT_BAIDU_CA_SOURCE_BAIDU_CA_TIMESTAMP_H
#define BAIDU_IOT_TINYDU_IOT_OS_SRC_IOT_BAIDU_CA_SOURCE_BAIDU_CA_TIMESTAMP_H

#include "lightduer_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Macros used to compare time values
 *
 *  These inlines deal with timer wrapping correctly. You are
 *  strongly encouraged to use them
 *  1. Because people otherwise forget
 *  2. Because if the timer wrap changes in future you won't have to
 *     alter your code.
 *
 * DUER_TIME_AFTER(a,b) returns true if the time a is after time b.
 */
#define DUER_TIME_AFTER(a, b)              ((duer_s32_t)(b) - (duer_s32_t)(a) < 0)
#define DUER_TIME_BEFORE(a, b)             DUER_TIME_AFTER(b, a)
#define DUER_TIME_AFTER_EQUAL(a, b)        ((duer_s32_t)(a) - (duer_s32_t)(b) >= 0)
#define DUER_TIME_BEFORE_EQUAL(a, b)       DUER_TIME_AFTER_EQUAL(b, a)

/*
 * Obtain the system timestamp by milliseconds
 *
 * @Return duer_u32_t, the system timestamp by milliseconds
 */

DUER_INT duer_u32_t duer_timestamp(void);

/*
 * The timestamp callbacks
 * Return the timestamp by milliseconds
 */
typedef duer_u32_t (*duer_timestamp_f)();

/*
 * Initial the timestamp callbacks for Baidu CA
 *
 * @Param f_timestamp, in, the function obtain the timestamp
 */
DUER_EXT void baidu_ca_timestamp_init(duer_timestamp_f f_timestamp);

#ifdef __cplusplus
}
#endif

#endif // BAIDU_IOT_TINYDU_IOT_OS_SRC_IOT_BAIDU_CA_SOURCE_BAIDU_CA_TIMESTAMP_H
