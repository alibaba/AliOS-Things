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

#ifndef _ESPOS_SCHEDULER_H_
#define _ESPOS_SCHEDULER_H_

#include <stdbool.h>

#include "espos_types.h"
#include "espos_errno.h"
#include "espos_spinlock.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * ESPOS state
 */
typedef enum espos_stat {
    ESPOS_IS_NOT_STARTED = 0,
    ESPOS_IS_RUNNING,
    ESPOS_IS_SUSPENDED
} espos_stat_t;

/************************************ internal function ************************************/
/**
 * @brief enter ESPOS system critical state
 *
 * @param spinlock spinlock handle point
 *
 * @return critical state temple variable(used by "espos_exit_critical")
 */
espos_critical_t _espos_enter_critical(espos_spinlock_t *spinlock);

/**
 * @brief exit ESPOS system critical state
 *
 * @param spinlock spinlock handle point
 * @param tmp critical state temple variable(created by "espos_enter_critical")
 *
 * @return none
 */
void _espos_exit_critical(espos_spinlock_t *spinlock, espos_critical_t tmp);

/*******************************************************************************************/

/**
 * @brief initialize ESPOS system
 *
 * @return the result
 *             0 : successful
 *       -ENOMEM : no enough memory
 */
esp_err_t espos_init(void);

/**
 * @brief start ESPOS system
 *
 * @return the result
 *             0 : successful
 *        -EPERM : failed (it will never happen in a general way)
 */
esp_err_t espos_start(void);

/**
 * @brief start ESPOS system CPU port
 *
 * @param port CPU port ID
 *
 * @return the result
 *             0 : successful
 *        -EPERM : failed (it will never happen in a general way)
 */
esp_err_t espos_start_port(int port);

/**
 * @brief get espos system state
 *
 * @return the state
 *         ESPOS_IS_NOT_STARTED : ESPOS is not started
 *         ESPOS_IS_RUNNING     : ESPOS is running
 *         ESPOS_IS_SUSPENDED   : ESPOS is suspended
 */
espos_stat_t espos_sched_state_get(void);

/**
 * @brief declare espos critical temp data
 */
#define espos_declare_critical(t) espos_critical_t t

/**
 * @brief enter ESPOS system critical state
 *
 * @param sl spinlock handle
 * @param t  critical state
 *
 * @return critical state temple variable(used by "espos_exit_critical")
 */
#define espos_enter_critical(t, sl) (t) = _espos_enter_critical(&(sl))

/**
 * @brief exit ESPOS system critical state
 *
 * @param t critical state temple variable(created by "espos_enter_critical")
 * @param sl spinlock handle point
 *
 * @return none
 */
#define espos_exit_critical(t, sl) _espos_exit_critical(&(sl), t)

/**
 * @brief OS internal function enter ESPOS system critical state
 *
 * @param sl spinlock handle
 * @param t  critical state
 *
 * @return critical state temple variable(used by "espos_exit_critical")
 *
 * @note: ESPOS is application level OS API, so it means all APIs call internal
 *        real OS's function, so if OS's or its core hardware's functions want
 *        to call ESPOS, loop nesting will occur, so we should tell ESPOS it's
 *        at OS internal state now.
 */
#define espos_os_enter_critical(t, sl)  \
    espos_os_enter();                  \
    (t) = _espos_enter_critical(&(sl))

/**
 * @brief OS internal function exit ESPOS system critical state
 *
 * @param t critical state temple variable(created by "espos_enter_critical")
 * @param sl spinlock handle point
 *
 * @return none
 *
 * @note: ESPOS is application level OS API, so it means all APIs call internal
 *        real OS's function, so if OS's or its core hardware's functions want
 *        to call ESPOS, loop nesting will occur, so we should tell ESPOS it's
 *        at OS internal state now.
 */
#define espos_os_exit_critical(t, sl)   \
    _espos_exit_critical(&(sl), t);     \
    espos_os_exit()

/**
 * @brief suspend the local CPU core preempt and the current task
 *        owned by local CPU core  will not be preempted
 *
 * @return the result
 *             0 : successful
 *       -EAGAIN : preempt suspending maximum number is reached, and fail to do it
 *        -EINTR : you do this at interrupt state, and it is not supported
 *
 * @note the function can be used nested
 */
esp_err_t espos_preempt_suspend_local(void);

/**
 * @brief resume the local CPU core preempt
 *
 * @return the result
 *             0 : successful
 *       -EAGAIN : preempt resuming maximum number is reached, and fail to do it
 *        -EINTR : you do this at interrupt state, and it is not supported
 *
 * @note the function can be used nested
 */
esp_err_t espos_preempt_resume_local(void);

/**
 * @brief enter system interrupt server, the function must be used after entering hardware interrupt
 *
 * @return the result
 *             0 : successful
 *       -EAGAIN : nested count is reached
 *
 * @note the function can be used nested
 */
esp_err_t espos_isr_enter (void);

/**
 * @brief exit system interrupt server, the function must be used before exiting hardware interrupt
 *
 * @return none
 *
 * @note the function can be used nested
 */
void espos_isr_exit(void);

/**
 * @brief check if the CPU is at ISR state
 *
 * @param none
 *
 * @return true if the CPU is at ISR state or false
 */
bool espos_in_isr(void);

/**
 * @brief mark it enters real OS interal function
 *
 * @return the result
 *             0 : successful
 *       -EAGAIN : nested count is reached
 *
 * @note the function can be used nested
 */
esp_err_t espos_os_enter(void);

/**
 * @brief remove mark it enters real OS interal function
 *
 * @param none
 *
 * @return none
 */
void espos_os_exit(void);

/**
 * @brief check if the function is at real OS internal fucntion
 *
 * @param none
 *
 * @return true if the CPU is at real OS internal fucntion or false
 */
bool espos_os_isr(void);

#ifdef __cplusplus
}
#endif

#endif

