// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef _ESPOS_MUTEX_H_
#define _ESPOS_MUTEX_H_

#include "espos_types.h"
#include "espos_errno.h"
#include "espos_time.h"

#ifdef __cplusplus
extern "C" {
#endif

/* use default option to create mutex */
typedef enum espos_mutex_opt {

    /* AT this mode, deadlock detection shall not be provided. Attempting to relock the mutex causes deadlock.
     *
     * If a thread attempts to unlock a mutex that it has not locked or a mutex which is unlocked,
     * undefined behavior results.
     */
    ESPOS_MUTEX_NORMAL = 0,

    ESPOS_MUTEX_RECURSIVE,

    ESPOS_MUTEX_TYPE_MAX
} espos_mutex_type_t;

#define ESPOS_MUTEX_TYPE(type) (type & 0xFF)

/**
 * @brief create a mutex
 *
 * @param mutex mutex handle point
 * @param opt mutex option, if you don't know how to do, just use "ESPOS_MUTEX_NORMAL" here
 *
 * @return the result
 *             0 : successful
 *       -ENOMEM : no enough memory
 *       -EINTR  : you do this at interrupt state, and it is not supported
 *       -EINVAL : input parameter error
 */
esp_err_t espos_mutex_create(espos_mutex_t *mutex, espos_opt_t opt);

/**
 * @brief set a mutex name
 *
 * @param mutex mutex handle
 * @param name mutex's name
 *
 * @return the result
 *             0 : successful
 *       -EINVAL : input parameter error
 */
esp_err_t espos_mutex_set_name(espos_mutex_t mutex, const char *name);

/**
 * @brief lock a mutex
 *
 * @param mutex mutex handle
 * @param wait_ticks sleep for system ticks if the locked mutex is not unlocked. Otherwise if someone
 *                   else unlock the locked mutex, you will wake up.
 *                   maximum time is "ESPOS_MAX_DELAY", no time is "ESPOS_NO_DELAY"
 *
 * @return the result
 *             0 : successful
 *       -EINVAL : input parameter error
 *        -EINTR : you do this at interrupt state, and it is not supported
 *    -ETIMEDOUT : timeout and you have not locked it
 *
 * @note you can transform the millisecond to ticks by "espos_ms_to_ticks"
 */
esp_err_t espos_mutex_lock(espos_mutex_t mutex, espos_tick_t wait_ticks);

/**
 * @bref try to lock a mutex and it will return immediately without being blocked
 *
 * @param m mutex handle
 *
 * @return the result
 *             0 : successful
 *       -EINVAL : input parameter error
 *        -EINTR : you do this at interrupt state, and it is not supported
 */
#define espos_mutex_trylock(m) espos_mutex_lock(m, ESPOS_NO_DELAY)

/**
 * @brief unlock a mutex
 *
 * @param mutex mutex handle
 *
 * @return the result
 *             0 : successful
 *       -EINVAL : input parameter error
 *        -EINTR : you do this at interrupt state, and it is not supported
 *        -EPERM : current task don't lock the mutex
 */
esp_err_t espos_mutex_unlock(espos_mutex_t mutex);

/**
 * @brief get task handle which lock the mutex
 *
 * @param mutex mutex handle
 *
 * @return the result
 *             0 : successful
 *       -EINVAL : input parameter error
 */
espos_task_t espos_mutex_get_holder(espos_mutex_t mutex);

/**
 * @brief delete the mutex
 *
 * @param mutex mutex handle
 *
 * @return the result
 *             0 : successful
 *        -EINTR : you do this at interrupt state, and it is not supported
 *       -EINVAL : input parameter error
 *
 * @note if low-level module is YunOS, this function will awake all task blocked at the mutex
 */
esp_err_t espos_mutex_del(espos_mutex_t mutex);

#ifdef __cplusplus
}
#endif

#endif /* _ESPOS_MUTEX_H_ */
