/*
 * Copyright (C) 2016 YunOS Project. All rights reserved.
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

#ifndef YUNOS_BSP_TIMER_H
#define YUNOS_BSP_TIMER_H

#include <stdint.h>
#include <stdbool.h>
#include <timer.h>
#include <bsp/yunos_bsp_errno.h>

/**
 * module api error number related definitions: 0x0000AB00
 */
#define BSP_API_TIMER_INIT        0x01
#define BSP_API_TIMER_OPEN        0x02
#define BSP_API_TIMER_CLOSE       0x03
#define BSP_API_TIMER_START       0x04
#define BSP_API_TIMER_STOP        0x05
#define BSP_API_TIMER_SETTIMEOUT  0x06
#define BSP_API_TIMER_SETHANDLER  0x07
#define BSP_API_TIMER_GETSTATUS   0x08
#define BSP_API_TIMER_DELAYUS     0x09

/**
 * User function prototype. Returns true to reload the timer, and the
 * function can modify the next interval if desired.
 */
//typedef bool (*tccb_t)(uint32_t *next_interval_us);

#define BSP_TCFLAGS_INACTIVE 0        ///< bit = 0: The timer is not running
#define BSP_TCFLAGS_ACTIVE   (1 << 0) ///< bit = 1: The timer is running
#define BSP_TCFLAGS_HANDLER  (1 << 1) ///< bit = 1: Call the user function when the timer expires

struct yunos_timer_status_s {
  uint32_t  flags;                    ///< See BSP_TCFLAGS_* definitions above
  uint32_t  timeout;                  ///< The current timeout setting (in microseconds)
  uint32_t  timeleft;                 ///< Time left until the timer expiration (in microseconds)
};

#ifdef __cplusplus
#define EXTERN extern "C"
extern "C"
{
#else
#define EXTERN extern
#endif

/**
 * Init module, and register device to 'devpath' by calling timer_register.
 * For example: timer_register("/dev/timer0", &timer0_private_data);
 * @param[out]  count   the number of timer
 * @return      Zero on success; a negated errno value on failure.
 */
int yunos_bsp_timer_init(uint8_t *count);

/**
 * Open the timer, set the opened flag to be true
 * @param[in]   id      The id of timer
 * @return      Error Code
 */
int yunos_bsp_timer_open(uint8_t id);

/**
 * Close the timer, set the opened flag to be false
 * @param[in]   id      The id of timer
 * @return      Error Code
 */
int yunos_bsp_timer_close(uint8_t id);

/**
 * Start the timer, resetting the time to the current timeout.
 * @param       id      The timer id.
 * @return      Zero on success; a negated errno value on failure.
 */
int yunos_bsp_timer_start(uint8_t id);

/**
 * Stop the timer
 * @param       id      The timer id.
 * @return      Zero on success; a negated errno value on failure.
 */
int yunos_bsp_timer_stop(uint8_t id);

/**
 * Set a new timeout value (and reset the timer)
 * @param[in]   id      The timer id.
 * @param[in]   timeout The new timeout value in millisecnds(us).
 * @return      Zero on success; a negated errno value on failure.
 */
int yunos_bsp_timer_settimeout(uint8_t id, uint32_t timeout);

/**
 * Set timer interrupt handler
 * @param[in]   id      The timer id.
 * @param[in]   handler The function that execute when timer timeout.
 * @return      Zero on success; a negated errno value on failure.
 */
int yunos_bsp_timer_sethandler(uint8_t id, tccb_t handler);

/**
 * Get the current timer status
 * @param[in]   id     The timer id.
 * @param[in]   status The location to return the timer status information.
 * @return      Zero on success; a negated errno value on failure.
 */
int yunos_bsp_timer_getstatus(uint8_t id, struct yunos_timer_status_s *status);

/**
 * Use timer to delay us, even if timer's interrupt is disabled
 * @param[in]   timeout The delay timeout, unit:us
 * @return      Zero on success; a negated errno value on failure.
 */
int yunos_bsp_timer_delayus(uint32_t timeout);

#undef EXTERN
#ifdef __cplusplus
}
#endif

#endif /* YUNOS_BSP_TIMER_H */
