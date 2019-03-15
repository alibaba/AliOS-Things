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


#include "espos_spinlock.h"
#include "espos_scheduler.h"

/**
 * @brief create a spinlock
 */
esp_err_t espos_spinlock_create (
    espos_spinlock_t *spinlock
)
{
    if (!spinlock) {
        return -EINVAL;
    }

    espos_spinlock_arch_init(&spinlock->lock);

    return 0;
}

/**
 * @brief set a spinlock name
 */
esp_err_t espos_spinlock_set_name(espos_spinlock_t *spinlock, const char *name)
{
    return 0;
}

/**
 * @brief spin to lock a spinlock until sucessfully
 */
esp_err_t espos_spinlock_lock (
    espos_spinlock_t *spinlock
)
{
    if (!spinlock) {
        return -EINVAL;
    }

    espos_preempt_suspend_local();

    espos_spinlock_arch_lock(&spinlock->lock);

    spinlock->holder = espos_task_get_current();

    return 0;
}

/**
 * @brief try to lock a spinlock
 */
esp_err_t espos_spinlock_trylock (
    espos_spinlock_t *spinlock
)
{
    int ret;

    if (!spinlock) {
        return -EINVAL;
    }

    espos_preempt_suspend_local();

    ret = espos_spinlock_arch_trylock(&spinlock->lock);
    if (ret) {
        ret = -EAGAIN;
        espos_preempt_resume_local();
    } else {
        spinlock->holder = espos_task_get_current();
    }

    return ret;
}

/**
 * @brief get spinlock holder task handle
 */
espos_task_t espos_spinlock_get_holder (
    espos_spinlock_t *spinlock
)
{
    return spinlock->holder;
}

/**
 * @brief unlock a spinlock
 */
esp_err_t espos_spinlock_unlock (
    espos_spinlock_t *spinlock
)
{
    if (!spinlock) {
        return -EINVAL;
    }

    spinlock->holder = ESPOS_OBJ_NONE;

    espos_spinlock_arch_unlock(&spinlock->lock);

    espos_preempt_resume_local();

    return 0;
}

/**
 * @brief delete a spinlock
 */
esp_err_t espos_spinlock_del (
    espos_spinlock_t *spinlock
)
{
    if (!spinlock) {
        return -EINVAL;
    }

    espos_spinlock_arch_deinit(&spinlock->lock);

    return 0;
}

