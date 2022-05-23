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
/**
 * File: lightduer_engine.h
 * Auth: Su Hao (suhao@baidu.com)
 * Desc: Light duer IoT CA engine.
 */

#ifndef BAIDU_DUER_LIGHTDUER_INCLUDE_LIGHTDUER_ENGINE_H
#define BAIDU_DUER_LIGHTDUER_INCLUDE_LIGHTDUER_ENGINE_H

#include "baidu_json.h"
#include "lightduer_types.h"
#include "lightduer_coap_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

enum duer_engine_events_enum {
    DUER_EVT_CREATE,
    DUER_EVT_START,
    DUER_EVT_ADD_RESOURCES,
    DUER_EVT_SEND_DATA,
    DUER_EVT_DATA_AVAILABLE,
    DUER_EVT_STOP,
    DUER_EVT_DESTROY,
    DUER_EVENTS_TOTAL
};

typedef void (*duer_engine_notify_func)(int event, int status, int what, void *object);

void duer_engine_register_notify(duer_engine_notify_func func);

void duer_engine_create(int what, void *object);

void duer_engine_set_response_callback(int what, void *object);

void duer_engine_start(int what, void *object);

void duer_engine_add_resources(int what, void *object);

void duer_engine_data_available(int what, void *object);

int duer_engine_enqueue_report_data(duer_context_t *context, const baidu_json *data);

void duer_engine_send(int what, void *object);

void duer_engine_stop(int what, void *object);

void duer_engine_destroy(int what, void *object);

int duer_engine_qcache_length(void);

duer_bool duer_engine_is_started(void);

void duer_engine_clear_data(int what, void *object);

const char *duer_engine_get_uuid(void);

const char *duer_engine_get_bind_token(void);

const char *duer_engine_get_rsa_cacrt(void);

#ifdef __cplusplus
}
#endif

#endif/*BAIDU_DUER_LIGHTDUER_INCLUDE_LIGHTDUER_ENGINE_H*/
