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

#include <k_api.h>
#include "espos_err.h"
#include "espos_semaphore.h"
#include "espos_scheduler.h"

/**
 * @brief create a semaphore
 */
esp_err_t espos_sem_create (
    espos_sem_t *sem,
    espos_sem_count_t max_count,
    espos_sem_count_t init_count
)
{
    kstat_t ret;
    ksem_t **psem = (ksem_t **)sem;

    ret = krhino_sem_dyn_create(psem, "default_sem", init_count);

    return espos_err_map(ret);
}

/**
 * @brief set a semaphore name
 */
esp_err_t espos_sem_set_name(espos_sem_t sem, const char *name)
{
    return 0;
}

/**
 * @brief take semaphore
 */
esp_err_t espos_sem_take (
    espos_sem_t sem,
    espos_tick_t wait_ticks
)
{
    kstat_t ret;
    ksem_t *psem = (ksem_t *)sem;

    ret = krhino_sem_take(psem, wait_ticks);

    return espos_err_map(ret);
}

/**
 * @brief give up semaphore
 */
esp_err_t espos_sem_give (
    espos_sem_t sem
)
{
    kstat_t ret;
    ksem_t *psem = (ksem_t *)sem;

    ret = krhino_sem_give(psem);

    return espos_err_map(ret);
}

/**
 * @brief delete the semaphore
 */
esp_err_t espos_sem_del (
    espos_sem_t sem
)
{
    kstat_t ret;
    ksem_t *psem = (ksem_t *)sem;

    ret = krhino_sem_dyn_del(psem);

    return espos_err_map(ret);
}

