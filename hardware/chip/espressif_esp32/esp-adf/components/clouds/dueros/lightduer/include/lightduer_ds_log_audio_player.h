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
 * Auth: Su Hao(suhao@baidu.com)
 * Desc: Record the audio player errors
 */

#ifndef BAIDU_DUER_LIBDUER_DEVICE_MODULES_DEVICES_STATUS_LIGHTDUER_DS_LOG_AUDIO_PLAYER_H
#define BAIDU_DUER_LIBDUER_DEVICE_MODULES_DEVICES_STATUS_LIGHTDUER_DS_LOG_AUDIO_PLAYER_H

#include "baidu_json.h"
#include "lightduer_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum _duer_dslog_audply_code_e {
    DUER_DS_LOG_AUDPLY_FINISHED             = 0x001,
    DUER_DS_LOG_AUDPLY_TERMINATED           = 0x002,

    // Internal Errors
    DUER_DS_LOG_AUDPLY_INTERNAL_ERROR       = 0x500,

    // HTTP errors
    DUER_DS_LOG_AUDPLY_HTTP_REQUEST_ERROR,
    DUER_DS_LOG_AUDPLY_HTTP_RESPONSE_ERROR,

    // File errors
    DUER_DS_LOG_AUDPLY_FILE_ERROR,

    // Codec errors
    DUER_DS_LOG_AUDPLY_CODEC_ERROR,
} duer_dslog_audply_code_t;

/**
 * common function to report the log of audio play moudle
 *
 * @param log_code, duer_dslog_audply_code_t, defined the error type
 * @param uri, const char *, file path or url
 * @param speed, float, download speed average
 * @param code, int, internal error code, 0 means not report
 * @param file, conat char *, file where the error occurs
 * @param line, int, the file line where the error occurs
 */
duer_status_t duer_ds_log_audply(duer_dslog_audply_code_t log_code, const char *uri,
        float speed, int code, const char *file, int line);

#define DUER_DS_LOG_AUDPLY_INFO(_code, _uri, _speed)                    \
        duer_ds_log_audply(_code, _uri, _speed, 0, NULL, 0)
#define DUER_DS_LOG_AUDPLY_ERROR(_code, _uri, _speed, _errcode)         \
        duer_ds_log_audply(_code, _uri, _speed, _errcode, __FILE__, __LINE__)
#define DUER_DS_LOG_AUDPLY_ERROR_ONLY(_code, _uri, _speed, _errcode)    \
        duer_ds_log_audply(_code, _uri, _speed, _errcode, NULL, 0)

#ifdef __cplusplus
}
#endif

#endif // BAIDU_DUER_LIBDUER_DEVICE_MODULES_DEVICES_STATUS_LIGHTDUER_DS_LOG_AUDIO_PLAYER_H
