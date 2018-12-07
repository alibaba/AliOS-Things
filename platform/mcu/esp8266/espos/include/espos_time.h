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

#ifndef _ESPOS_TIME_H_
#define _ESPOS_TIME_H_

#include "espos_types.h"
#include "espos_errno.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * delay or wait for ticks, it is used by mutex, queue,
 * semaphore, timer and so on
 */
/* no delay for ticks, function will return immediately */
#define ESPOS_NO_DELAY  0u
/* delay forever, function will never return until event triggers */
#define ESPOS_MAX_DELAY 0xffffffffu

espos_time_t espos_get_tick_per_ms(void);

/**
 * @brief get current system ticks
 *
 * @return current ticks
 */
espos_tick_t espos_get_tick_count(void);

/**
 * @brief transform milliseconds to system ticks
 *
 * @param ms milliseconds
 *
 * @return system ticks
 *
 * @note the function discards the shortage of digits, for example:
 *       20ms -> 2 ticks ; 21ms -> 2 ticks; 29 -> 2 ticks
 */
espos_tick_t espos_ms_to_ticks(espos_time_t ms);

/**
 * @brief transform system ticks to milliseconds
 *
 * @param ticks system ticks
 *
 * @return milliseconds
 */
espos_time_t espos_ticks_to_ms(espos_tick_t ticks);

#ifdef __cplusplus
}
#endif

#endif

