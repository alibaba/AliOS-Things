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

#ifndef _ESPOS_SPINLOCK_H_
#define _ESPOS_SPINLOCK_H_

#include "espos_types.h"
#include "espos_errno.h"
#include "espos_task.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct espos_spinlock {
    espos_spinlock_arch_t lock;
    espos_task_t holder;
} espos_spinlock_t;

#define ESPOS_SPINLOCK_UNLOCK_INITIALIZER       \
    {                                           \
        ESPOS_SPINLOCK_ARCH_UNLOCK_INITIALIZER, \
        ESPOS_OBJ_NONE,                                      \
    }

#define ESPOS_DEFINE_SPINLOCK(l) \
    espos_spinlock_t l = ESPOS_SPINLOCK_UNLOCK_INITIALIZER

#define ESPOS_DEFINE_STATIC_SPINLOCK(l) \
    static espos_spinlock_t l = ESPOS_SPINLOCK_UNLOCK_INITIALIZER

/**
 * @brief create a spinlock
 *
 * @param spinlock spinlock handle point
 *
 * @return the result
 *             0 : successful
 *       -ENOMEM : no enough memory
 *       -EINVAL : input parameter error
 */
esp_err_t espos_spinlock_create (espos_spinlock_t *spinlock);

/**
 * @brief set a spinlock name
 *
 * @param spinlock spinlock handle
 * @param name spinlock's name
 *
 * @return the result
 *             0 : successful
 *       -EINVAL : input parameter error
 */
esp_err_t espos_spinlock_set_name(espos_spinlock_t *spinlock, const char *name);

/**
 * @brief spin to lock a spinlock until successfully
 *
 * @param spinlock spinlock handle
 *
 * @return the result
 *             0 : successful
 *       -EINVAL : input parameter error
 */
esp_err_t espos_spinlock_lock(espos_spinlock_t *spinlock);

/**
 * @brief try to lock a spinlock
 *
 * @param spinlock spinlock handle
 *
 * @return the result
 *             0 : successful
 *       -EAGAIN : no spinlock is valid
 *       -EINVAL : input parameter error
 */
esp_err_t espos_spinlock_trylock(espos_spinlock_t *spinlock);

/**
 * @brief get spinlock holder task handle
 *
 * @param spinlock spinlock handle
 *
 * @return holder task handle
 */
espos_task_t espos_spinlock_get_holder(espos_spinlock_t *spinlock);

/**
 * @brief unlock a spinlock
 *
 * @param spinlock spinlock handle
 *
 * @return the result
 *             0 : successful
 *       -EAGAIN : no spinlock is locked
 *       -EINVAL : input parameter error
 */
esp_err_t espos_spinlock_unlock(espos_spinlock_t *spinlock);

/**
 * @brief delete a spinlock
 *
 * @param spinlock spinlock handle
 *
 * @return the result
 *             0 : successful
 *       -EACCES : failed to do it with some special reason
 *       -EINVAL : input parameter error
 */
esp_err_t espos_spinlock_del(espos_spinlock_t *spinlock);

#ifdef __cplusplus
}
#endif

#endif

