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
 * Auth: Su Hao(suhao@baidu.com)
 * Desc: Sonic decode related report log
 */

#ifndef BAIDU_DUER_LIBDUER_DEVICE_MODULES_DEVICE_STATUS_LIGHTDUER_DS_LOG_SONIC_DECODE_H
#define BAIDU_DUER_LIBDUER_DEVICE_MODULES_DEVICE_STATUS_LIGHTDUER_DS_LOG_SONIC_DECODE_H

#include "baidu_json.h"
#include "lightduer_types.h"
#include "lightduer_math.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum _duer_ds_log_sonic_decode_code {
    DUER_DS_LOG_SONICCODEC_DECODE_START = 0x101,           // 开始解码  Info    否
    DUER_DS_LOG_SONICCODEC_DECODE_FETCHED_START = 0x102,   // 接收第一人同步信息 Info    否
    DUER_DS_LOG_SONICCODEC_DECODE_BLOCK_ERROR = 0x301,     // 收到错误的block    Debug   否
    DUER_DS_LOG_SONICCODEC_DECODE_FRAGMENT_FETCHED = 0x103,// 收到正确的fragment Debug   否
    DUER_DS_LOG_SONICCODEC_DECODE_FRAGMENT_ERROR = 0x302,  // 收到错误的fragment Debug   否
    DUER_DS_LOG_SONICCODEC_DECODE_FRAME_COMPLETE = 0x104,  // 收到完整的Frame    Debug   否
    DUER_DS_LOG_SONICCODEC_DECODE_FINISH = 0x105,          // 解码结束  Info    否
    DUER_DS_LOG_SONICCODEC_DECODE_FAILED = 0x303,          // 解码失败  Error   否
} duer_ds_log_sonic_decode_code_t;

DUER_INT duer_status_t duer_ds_log_sonicdec_start(void);

DUER_INT duer_status_t duer_ds_log_sonicdec_audio_analysis(const duer_coef_t *data, duer_size_t size);

DUER_INT duer_status_t duer_ds_log_sonicdec_fetched_start(void);

DUER_INT duer_status_t duer_ds_log_sonicdec_block_error(void);

DUER_INT duer_status_t duer_ds_log_sonicdec_fragment_fetched(void);

DUER_INT duer_status_t duer_ds_log_sonicdec_fragment_error(void);

DUER_INT duer_status_t duer_ds_log_sonicdec_frame_complete(int length);

DUER_INT duer_status_t duer_ds_log_sonicdec_finish(void);

DUER_INT duer_status_t duer_ds_log_sonicdec_failed(void);

#ifdef __cplusplus
}
#endif

#endif // BAIDU_DUER_LIBDUER_DEVICE_MODULES_DEVICE_STATUS_LIGHTDUER_DS_LOG_SONIC_DECODE_H
