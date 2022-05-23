/**
 * Copyright (2018) Baidu Inc. All rights reserved.
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
 * Auth: Chen Xihao(chenxihao@baidu.com)
 * Desc: airkiss related report log
 */

#ifndef BAIDU_DUER_LIGHTDUER_DS_LOG_AIRKISS_H
#define BAIDU_DUER_LIGHTDUER_DS_LOG_AIRKISS_H

#include "baidu_json.h"
#include "lightduer_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum _duer_ds_log_airkiss_code {
    DUER_DS_LOG_AIRKISS_START          = 0x201,
    DUER_DS_LOG_AIRKISS_FINISHED       = 0x202,

    DUER_DS_LOG_AIRKISS_FAILED         = 0x401,
} duer_ds_log_airkiss_code_t;

/**
 * common function to report the log of airkiss
 */
duer_status_t duer_ds_log_airkiss(duer_ds_log_airkiss_code_t log_code,
                                    const baidu_json *message);

#ifdef __cplusplus
}
#endif

#endif // BAIDU_DUER_LIGHTDUER_DS_LOG_AIRKISS_H
