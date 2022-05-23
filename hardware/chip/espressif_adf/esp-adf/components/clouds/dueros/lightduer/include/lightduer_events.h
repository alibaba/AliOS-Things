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
 * File: lightduer_events.h
 * Auth: Su Hao (suhao@baidu.com)
 * Desc: Light duer events looper.
 */

#ifndef BAIDU_DUER_LIGHTDUER_INCLUDE_LIGHTDUER_EVENTS_H
#define BAIDU_DUER_LIGHTDUER_INCLUDE_LIGHTDUER_EVENTS_H

#include "lightduer_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*duer_events_func)(int what, void *object);

typedef struct _duer_event_status_message_s {
    int                 _status;
    duer_events_func    _func;
    int                 _what;
    void *              _object;
} duer_evtst_message_t;

typedef void *duer_events_handler;

duer_events_handler duer_events_create(const char *name, size_t stack_size, size_t queue_length);

duer_events_handler duer_events_create_with_priority(const char *name, size_t stack_size,
                                                     size_t queue_length, int priority);

int duer_events_call(duer_events_handler handler, duer_events_func func, int what, void *object);

void duer_events_destroy(duer_events_handler handler);

#ifdef __cplusplus
}
#endif

#endif/*BAIDU_DUER_LIGHTDUER_INCLUDE_LIGHTDUER_EVENTS_H*/
