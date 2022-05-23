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
 * Desc: bind device module related report log
 */

#ifndef BAIDU_DUER_LIGHTDUER_DS_LOG_BIND_H
#define BAIDU_DUER_LIGHTDUER_DS_LOG_BIND_H

#include "baidu_json.h"
#include "lightduer_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum _duer_ds_log_bind_code {
    DUER_DS_LOG_BIND_CONFIG_WIFI_AIRKISS        = 0x001,
    DUER_DS_LOG_BIND_CONFIG_WIFI_SMARTCONFIG    = 0x002,
    DUER_DS_LOG_BIND_CONFIG_WIFI_SONIC          = 0x003,
    DUER_DS_LOG_BIND_CONFIG_WIFI_BT             = 0x004,
    DUER_DS_LOG_BIND_TASK_START                 = 0x005,
    DUER_DS_LOG_BIND_SEND_RESP                  = 0x006,
    DUER_DS_LOG_BIND_TASK_END                   = 0x007,

    DUER_DS_LOG_BIND_NO_MEMORY                  = 0x301,
    DUER_DS_LOG_BIND_INVALID_PARAMS             = 0x302,
    DUER_DS_LOG_BIND_START_TASK_FAILED          = 0x303,
    DUER_DS_LOG_BIND_INIT_AES_FAILED            = 0x304,
    DUER_DS_LOG_BIND_INIT_SOCKET_FAILED         = 0x305,
    DUER_DS_LOG_BIND_BIND_SOCKET_FAILED         = 0x306,
    DUER_DS_LOG_BIND_ENCRYPT_FAILED             = 0x307,
    DUER_DS_LOG_BIND_SOCKET_ERROR               = 0x308,
} duer_ds_log_bind_code_t;

/**
 * common function to report the log of bind device moudle
 */
duer_status_t duer_ds_log_bind(duer_ds_log_bind_code_t log_code);


int duer_ds_log_report_bind_result(duer_u32_t req_port,
            duer_u32_t resp_success_count, duer_u32_t resp_fail_count);

#ifdef __cplusplus
}
#endif

#endif // BAIDU_DUER_LIGHTDUER_DS_LOG_BIND_H
