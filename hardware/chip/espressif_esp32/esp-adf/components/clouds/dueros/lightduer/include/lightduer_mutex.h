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
// Description: Wrapper for mutex

#ifndef BAIDU_IOT_TINYDU_IOT_OS_SRC_IOT_BAIDU_CA_SOURCE_BAIDU_CA_MUTEX_H
#define BAIDU_IOT_TINYDU_IOT_OS_SRC_IOT_BAIDU_CA_SOURCE_BAIDU_CA_MUTEX_H

#include "lightduer_types.h"
//#include "lightduer_ca.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void* duer_mutex_t;

/*
 * Create mutex context
 *
 * @Return duer_mutex_t, the created mutex context
 */
DUER_INT duer_mutex_t duer_mutex_create(void);

/*
 * Mutex lock
 *
 * @Return duer_mutex_t, the mutex context
 */
DUER_INT duer_status_t duer_mutex_lock(duer_mutex_t mutex);

/*
 * Mutex unlock
 *
 * @Return duer_mutex_t, the mutex context
 */
DUER_INT duer_status_t duer_mutex_unlock(duer_mutex_t mutex);

/*
 * Destroy the mutex context
 *
 * @Return duer_mutex_t, the mutex context
 */
DUER_INT duer_status_t duer_mutex_destroy(duer_mutex_t mutex);

/*
 * The mutex callbacks
 */
typedef duer_mutex_t (*duer_mutex_create_f)();
typedef duer_status_t (*duer_mutex_lock_f)(duer_mutex_t mtx);
typedef duer_status_t (*duer_mutex_unlock_f)(duer_mutex_t mtx);
typedef duer_status_t (*duer_mutex_destroy_f)(duer_mutex_t mtx);

/*
 * Initial the mutex callbacks for Baidu CA
 *
 * @Param f_create, in, the function create mutex context
 * @Param f_lock, in, the function mutex lock
 * @Param f_unlock, in, the function mutex unlock
 * @Param f_destroy, in, the function destroy mutex context
 */
DUER_EXT void baidu_ca_mutex_init(duer_mutex_create_f f_create,
                                 duer_mutex_lock_f f_lock,
                                 duer_mutex_unlock_f f_unlock,
                                 duer_mutex_destroy_f f_destroy);

#ifdef __cplusplus
}
#endif

#endif // BAIDU_IOT_TINYDU_IOT_OS_SRC_IOT_BAIDU_CA_SOURCE_BAIDU_CA_MUTEX_H
