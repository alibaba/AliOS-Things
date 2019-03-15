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

#include <stdlib.h>
#include "k_api.h"
#include "espos_err.h"
#include "espos_mutex.h"
#include "espos_scheduler.h"

/**
 * @brief create a mutex
 */
esp_err_t espos_mutex_create (
    espos_mutex_t *mutex,
    espos_opt_t opt
)
{
    kstat_t    ret;
    kmutex_t **pmutex = (kmutex_t **)mutex;

    ret = krhino_mutex_dyn_create(pmutex, "default_mutex");

    return espos_err_map(ret);
}

/**
 * @brief set a mutex name
 */
esp_err_t espos_mutex_set_name(espos_mutex_t mutex, const char *name)
{
    return 0;
}

/**
 * @brief lock a mutex
 */
esp_err_t espos_mutex_lock (
    espos_mutex_t mutex,
    espos_tick_t wait_ticks
)
{
    kstat_t ret;
    kmutex_t *pmutex = (kmutex_t *)mutex;

    ret = krhino_mutex_lock(pmutex, wait_ticks);

    if (ret == RHINO_MUTEX_OWNER_NESTED) {
        return 0;
    }

    return espos_err_map(ret);
}

/**
 * @brief unlock a mutex
 */
esp_err_t espos_mutex_unlock (
    espos_mutex_t mutex
)
{
    kstat_t ret;
    kmutex_t *pmutex = (kmutex_t *)mutex;

    ret = krhino_mutex_unlock(pmutex);
    if (ret == RHINO_MUTEX_OWNER_NESTED) {
        return 0;
    }

    return espos_err_map(ret);
}

/**
 * @brief get task handle whick lock the mutex
 */
espos_task_t espos_mutex_get_holder (
    espos_mutex_t mutex
)
{
    espos_task_t tmp;

    kmutex_t *pmutex = (kmutex_t *)mutex;
    tmp = (espos_task_t)(pmutex->mutex_task);
    return tmp;
}

/**
 * @brief delete the mutex
 */
esp_err_t espos_mutex_del (
    espos_mutex_t mutex
)
{
    kstat_t ret;
    kmutex_t *pmutex = (kmutex_t *)mutex;

    ret = krhino_mutex_dyn_del(pmutex);

    return espos_err_map(ret);
}

