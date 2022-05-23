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
// Description: The random APIs.

#ifndef BAIDU_IOT_TINYDU_IOT_OS_SRC_IOT_BAIDU_CA_SOURCE_BAIDU_CA_RANDOM_H
#define BAIDU_IOT_TINYDU_IOT_OS_SRC_IOT_BAIDU_CA_SOURCE_BAIDU_CA_RANDOM_H

#include "lightduer_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Obtain the random number
 *
 * @Return duer_s32_t, the random number.
 */
DUER_INT duer_s32_t duer_random(void);

/*
 * The random callbacks
 * Return the random number
 *        0, means random got fail
 */
typedef duer_s32_t (*duer_random_f)();

/*
 * Initial the random callback for Baidu CA
 *
 * @Param f_random, in, the function obtain the random
 */
DUER_EXT void duer_random_init(duer_random_f f_random);

#ifdef __cplusplus
}
#endif

#endif // BAIDU_IOT_TINYDU_IOT_OS_SRC_IOT_BAIDU_CA_SOURCE_BAIDU_CA_RANDOM_H
