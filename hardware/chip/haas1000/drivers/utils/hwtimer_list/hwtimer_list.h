/*
 * Copyright (c) 2021 Bestechnic (Shanghai) Co., Ltd. All rights reserved.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef __HWTIMER_LIST_H__
#define __HWTIMER_LIST_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef void *HWTIMER_ID;

typedef void (*HWTIMER_CALLBACK_T)(void *param);

enum E_HWTIMER_T {
    E_HWTIMER_OK = 0,
    E_HWTIMER_FAILED,
    E_HWTIMER_INVAL_ID,
    E_HWTIMER_INVAL_ST,
    E_HWTIMER_IN_CALLBACK,
    E_HWTIMER_NOT_FOUND,
};

void hwtimer_init(void);

HWTIMER_ID hwtimer_alloc(HWTIMER_CALLBACK_T callback, void *param);

enum E_HWTIMER_T hwtimer_free(HWTIMER_ID id);

enum E_HWTIMER_T hwtimer_start(HWTIMER_ID id, unsigned int ticks);

enum E_HWTIMER_T hwtimer_update_then_start(HWTIMER_ID id, HWTIMER_CALLBACK_T callback, void *param, unsigned int ticks);

enum E_HWTIMER_T hwtimer_update(HWTIMER_ID id, HWTIMER_CALLBACK_T callback, void *param);

enum E_HWTIMER_T hwtimer_stop(HWTIMER_ID id);

#ifdef __cplusplus
}
#endif

#endif

