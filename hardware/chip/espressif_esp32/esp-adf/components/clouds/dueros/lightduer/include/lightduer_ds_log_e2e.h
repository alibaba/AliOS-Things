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
 * Desc: Record the end to end delay
 */

#ifndef BAIDU_DUER_LIBDUER_DEVICE_MODULES_DEVICE_STATUS_LIGHTDUER_DS_LOG_E2E_H
#define BAIDU_DUER_LIBDUER_DEVICE_MODULES_DEVICE_STATUS_LIGHTDUER_DS_LOG_E2E_H

#include "baidu_json.h"
#include "lightduer_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    DUER_E2E_1ST_WAKEUP,
    DUER_E2E_RECORD_FINISH,
    DUER_E2E_SEND,
    DUER_E2E_RESPONSE,
    DUER_E2E_PLAY,
    DUER_E2E_CODEC,
#ifdef DUER_SECOND_WAKEUP_ENABLE
    DUER_E2E_WAKEUP_SEND_ENQUEUE,
    DUER_E2E_WAKEUP_SEND,
    DUER_E2E_2ND_WAKEUP,
#endif//DUER_SECOND_WAKEUP_ENABLE
    DUER_E2E_EVENT_TOTAL,
} duer_ds_e2e_event_t;

typedef const char* (*duer_ds_e2e_get_dialog_id_cb)(void);

#ifdef DUER_STATISTICS_E2E

void duer_ds_e2e_event(duer_ds_e2e_event_t evt);

void duer_ds_e2e_set_report_codec_timestamp(void);

void duer_ds_e2e_set_not_report_codec_timestamp(void);

void duer_ds_e2e_set_vad_silence_time(duer_u32_t silence_time);

void duer_ds_e2e_set_dialog_id_cb(duer_ds_e2e_get_dialog_id_cb cb);

duer_bool duer_ds_e2e_wait_response(void);

#else

#define duer_ds_e2e_set_report_codec_timestamp(...)

#define duer_ds_e2e_set_not_report_codec_timestamp(...)

#define duer_ds_e2e_set_vad_silence_time(...)

#define duer_ds_e2e_event(...)

#define duer_ds_e2e_set_dialog_id_cb(...)

#define duer_ds_e2e_wait_response(...)

#endif

#ifdef __cplusplus
}
#endif

#endif // BAIDU_DUER_LIBDUER_DEVICE_MODULES_DEVICE_STATUS_LIGHTDUER_DS_LOG_E2E_H
