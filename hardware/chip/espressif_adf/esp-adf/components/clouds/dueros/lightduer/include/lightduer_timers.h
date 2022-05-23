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
 * File: lightduer_timers.h
 * Auth: Su Hao (suhao@baidu.com)
 * Desc: Provide the timer APIs.
 */

#ifndef BAIDU_DUER_LIGHTDUER_PORT_INCLUDE_LIGHTDUER_TIMERS_H
#define BAIDU_DUER_LIGHTDUER_PORT_INCLUDE_LIGHTDUER_TIMERS_H

#include "lightduer_types.h"

#ifdef __cplusplus
extern "C" {
#endif

enum _duer_timer_type_enum {
    DUER_TIMER_ONCE,
    DUER_TIMER_PERIODIC,
    DUER_TIMER_TOTAL
};

typedef void *duer_timer_handler;

typedef void (*duer_timer_callback)(void *);

duer_timer_handler duer_timer_acquire(duer_timer_callback callback, void *param, int type);

int duer_timer_start(duer_timer_handler handle, size_t delay);

int duer_timer_stop(duer_timer_handler handle);

void duer_timer_release(duer_timer_handler handle);

#ifdef __cplusplus
}
#endif

#endif/*BAIDU_DUER_LIGHTDUER_PORT_INCLUDE_LIGHTDUER_TIMERS_H*/
