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

#ifndef _ESPOS_TIMER_H_
#define _ESPOS_TIMER_H_

#include "espos_types.h"
#include "espos_errno.h"
#include "espos_time.h"

#ifdef __cplusplus
extern "C" {
#endif

/* timer just run once after starting it */
#define ESPOS_TIMER_NO_AUTO_RUN   0
/* timer run cycle after starting it */
#define ESPOS_TIMER_AUTO_RUN      (1 << 0)

/* timer configuration command */
typedef enum espos_timer_cmd {
    /* configure time's period */
    ESPOS_TIMER_CHANGE_PERIOD = 0,
    /* configure time to be "once" */
    ESPOS_TIMER_CHANGE_ONCE,
    /* configure time to be "auto" */
    ESPOS_TIMER_CHANGE_AUTO,

    ESPOS_TIMER_CMD_MAX
} espos_timer_cmd_t;

/**
 * @brief create a timer
 *
 * @param timer timer handle point
 * @param name  timer name, if name is NULL, we will use "default_timer" default
 * @param cb    timer callback function
 * @param arg   timer entry function inputting parameter
 * @param period_ticks  timer period ticks
 * @param opt   timer option
 *               ESPOS_TIMER_NO_AUTO_RUN : created timer will run once, even if it is started,
 *               ESPOS_TIMER_AUTO_RUN : created timer will run automatically
 *
 * @return the result
 *             0 : successful
 *       -ENOMEM : no enough memory
 *       -EINVAL : input parameter error
 *       -EINTR  : you do this at interrupt state, and it is not supported
 *
 * @note after starting the created timer by "espos_timer_start", it will only run.
 */
esp_err_t espos_timer_create(espos_timer_t *timer, const char *name, espos_timer_cb_t cb, void *arg,
                               espos_tick_t period_ticks, espos_opt_t opt);

/**
 * @brief start a timer
 *
 * @param timer timer handle
 *
 * @return the result
 *             0 : successful
 *       -EINVAL : input parameter error
 *       -ECANCELED : failed for some reason depend on low-level OS
 *
 * @note after starting the created timer by "espos_timer_start", it will only run
 */
esp_err_t espos_timer_start(espos_timer_t timer);

/**
 * @brief stop a timer
 *
 * @param timer timer handle
 *
 * @return the result
 *             0 : successful
 *       -EINVAL : input parameter error
 *    -ECANCELED : failed for some reason depend on low-level OS
 *
 * @note the timer should be started again, if it is stopped
 */
esp_err_t espos_timer_stop(espos_timer_t timer);

/**
 * @brief configure a timer
 *
 * @param timer timer handle
 * @param opt   timer option command
 * @param arg   timer parameter
 *
 * @return the result
 *             0 : successful
 *       -EINVAL : input parameter error
 *    -ECANCELED : failed for some reason depend on low-level OS
 */
esp_err_t espos_timer_change(espos_timer_t timer, espos_opt_t opt, void *arg);

/**
 * @brief configure period of timer
 *
 * @param t timer handle
 * @param a timer period
 *
 * @return the result
 *             0 : successful
 *       -EINVAL : input parameter error
 *    -ECANCELED : failed for some reason depend on low-level OS
 */
#define espos_timer_set_period(t, a) espos_timer_change(t, ESPOS_TIMER_CHANGE_PERIOD, (void *)a)

/**
 * @brief configure timer to be "once"
 *
 * @param t timer handle
 *
 * @return the result
 *             0 : successful
 *       -EINVAL : input parameter error
 *    -ECANCELED : failed for some reason depend on low-level OS
 */
#define espos_timer_set_once(t) espos_timer_change(t, ESPOS_TIMER_CHANGE_ONCE, NULL)

/**
 * @brief configure timer to be "auto"
 *
 * @param t timer handle
 *
 * @return the result
 *             0 : successful
 *       -EINVAL : input parameter error
 *    -ECANCELED : failed for some reason depend on low-level OS
 */
#define espos_timer_set_auto(t) espos_timer_change(t, ESPOS_TIMER_CHANGE_AUTO, NULL)

/**
 * @brief delete a timer
 *
 * @param t timer handle
 *
 * @return the result
 *             0 : successful
 *       -EACCES : failed to do it with some special reason
 *       -EINTR  : you do this at interrupt state, and it is not supported
 */
esp_err_t espos_timer_del(espos_timer_t timer);

/**
 * @brief get current timer name point
 *
 * @param timer timer handle
 * @param pname pointing at name's point
 *
 * @return the result
 *             0 : successful
 *       -EINVAL : input parameter error
 */
esp_err_t espos_get_timer_name (espos_timer_t timer, char **pname);

#ifdef __cplusplus
}
#endif

#endif

