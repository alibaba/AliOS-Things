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

#include "k_api.h"
#include "espos_err.h"
#include "espos_timer.h"
#include "espos_scheduler.h"

/**
 * @brief create a timer
 */
esp_err_t espos_timer_create (
    espos_timer_t *timer,
    const char *name,
    espos_timer_cb_t cb,
    void *arg,
    espos_tick_t period_ticks,
    espos_opt_t opt
)
{
    kstat_t ret = RHINO_SUCCESS;
    ktimer_t **ptimer = (ktimer_t **)timer;
    tick_t round;

    if (opt == ESPOS_TIMER_NO_AUTO_RUN)
        round = 0;
    else if (opt == ESPOS_TIMER_AUTO_RUN)
        round = period_ticks;
    else
        return EINVAL;

    ret = krhino_timer_dyn_create(ptimer, name, (timer_cb_t)cb, period_ticks,
                        round, arg, 0);
    
    return espos_err_map(ret);
}

/**
 * @brief start a timer
 */
esp_err_t espos_timer_start (espos_timer_t timer)
{
    kstat_t ret;
    ktimer_t *ptimer = (ktimer_t *)timer;

    ret = krhino_timer_start(ptimer);

    return espos_err_map(ret);
}

/**
 * @brief stop a timer
 */
esp_err_t espos_timer_stop (
    espos_timer_t timer
)
{
    kstat_t ret;
    ktimer_t *ptimer = (ktimer_t *)timer;

    ret = krhino_timer_stop(ptimer);

    return espos_err_map(ret);
}

/**
 * @brief configure a timer
 */
esp_err_t espos_timer_change (
    espos_timer_t timer,
    espos_opt_t opt,
    void *arg
)
{
    kstat_t ret;
    ktimer_t *ptimer = (ktimer_t *)timer;

    if (ESPOS_TIMER_CHANGE_PERIOD == opt) {
        espos_tick_t period = (espos_tick_t)arg;
        ret = krhino_timer_change(ptimer, ptimer->init_count, period);
    } else
        return EINVAL;

    return espos_err_map(ret);
}

/**
 * @brief delete a timer
 */
esp_err_t espos_timer_del (
    espos_timer_t timer
)
{
    kstat_t ret;
    ktimer_t *ptimer = (ktimer_t *)timer;

    ret = krhino_timer_dyn_del(ptimer);

    return espos_err_map(ret);
}

/**
 * @brief get current timer name point
 */
esp_err_t espos_get_timer_name (
    espos_timer_t timer,
    char **tname
)
{
    kstat_t ret;
    ktimer_t *ptimer = (ktimer_t *)timer;

   *tname = (char *)ptimer->name;
    ret = RHINO_SUCCESS;

    return espos_err_map(ret);
}

