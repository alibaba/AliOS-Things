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

#ifndef _ESPOS_SEMAPHORE_H_
#define _ESPOS_SEMAPHORE_H_

#include "espos_types.h"
#include "espos_errno.h"
#include "espos_time.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief create a semaphore
 *
 * @param sem semaphore handle point
 * @param max_count semaphore maximum count
 * @param init_count semaphore initialized count
 *
 * @return the result
 *             0 : successful
 *        ENOMEM : no enough memory
 *        EINVAL : input parameter error
 *
 * @note if low-level module is YunOS, "max_count" does not work, and the reachable count is "0xFFFFFFFF"
 */
esp_err_t espos_sem_create(espos_sem_t *sem, espos_sem_count_t max_count, espos_sem_count_t init_count);

/**
 * @brief set a semaphore name
 *
 * @param semaphore semaphore handle
 * @param name semaphore's name
 *
 * @return the result
 *             0 : successful
 *       -EINVAL : input parameter error
 */
esp_err_t espos_sem_set_name(espos_sem_t sem, const char *name);

/**
 * @brief take semaphore
 *
 * @param sem semaphore handle
 * @param wait_ticks sleep for system ticks if the semaphore is empty. Otherwise if semaphore is given,
 *                   oldest blocked task will wake up.
 *                   maximum time is "ESPOS_MAX_DELAY", no time is "ESPOS_NO_DELAY"
 *
 * @return the result
 *             0 : successful
 *       -EINVAL : input parameter error
 *    -ETIMEDOUT : timeout and not take semaphore
 *
 * @note you can transform the millisecond to ticks by "espos_ms_to_ticks"
 */
esp_err_t espos_sem_take(espos_sem_t sem, espos_tick_t wait_ticks);

/**
 * @brief try to take semaphore
 *
 * @param s semaphore handle
 *
 * @return the result
 *             0 : successful
 *       -EINVAL : input parameter error
 *    -ETIMEDOUT : not take semaphore
 *
 * @note you can transform the millisecond to ticks by "espos_ms_to_ticks"
 */
#define espos_sem_trytake(s) espos_sem_take(s, ESPOS_NO_DELAY)

/**
 * @brief give up semaphore
 *
 * @param sem semaphore handle
 * @param wait_ticks no meaning
 *
 * @return the result
 *             0 : successful
 *       -EINVAL : input parameter error
 *       -EAGAIN : the maximum count is reached
 *
 * @note you can transform the millisecond to ticks by "espos_ms_to_ticks"
 */
esp_err_t espos_sem_give(espos_sem_t sem);

/**
 * @brief delete the semaphore
 *
 * @param sem semaphore handle
 *
 * @return the result
 *             0 : successful
 *        -EINTR : you do this at interrupt state, and it is not supported
 *       -EINVAL : input parameter error
 *
 * @note if low-level module is YunOS, this function will awake all task blocked here
 */
esp_err_t espos_sem_del(espos_sem_t sem);

#ifdef __cplusplus
}
#endif

#endif

