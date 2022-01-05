/*
 * ESPRESSIF MIT License
 *
 * Copyright (c) 2019 <ESPRESSIF SYSTEMS (SHANGHAI) CO., LTD>
 *
 * Permission is hereby granted for use on all ESPRESSIF SYSTEMS products, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#ifndef _AUDIO_SYS_H_
#define _AUDIO_SYS_H_

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ___STR___(x) #x
#define STR_AUDIO(x) ___STR___(x)

#if defined(__GNUC__) && (__GNUC__ >= 7)
#define FALL_THROUGH __attribute__ ((fallthrough))
#else
#define FALL_THROUGH ((void)0)
#endif /* __GNUC__ >= 7 */

/**
 * @brief       Get system ticks by given millisecond
 *
 * @param[in]   ms millisecond
 *
 * @return
 *     - tick
 */
int audio_sys_get_tick_by_time_ms(int ms);

/**
 * @brief   Get system time with millisecond
 *
 * @return
 *     -  time with millisecond
 */
int64_t audio_sys_get_time_ms(void);

/**
 * @brief   Function to print the CPU usage of tasks over a given AUDIO_SYS_TASKS_ELAPSED_TIME_MS.
 *
 * This function will measure and print the CPU usage of tasks over a specified
 * number of ticks (i.e. real time stats). This is implemented by simply calling
 * uxTaskGetSystemState() twice separated by a delay, then calculating the
 * differences of task run times before and after the delay.
 *
 * @note    If any tasks are added or removed during the delay, the stats of
 *          those tasks will not be printed.
 * @note    This function should be called from a high priority task to minimize
 *          inaccuracies with delays.
 * @note    When running in dual core mode, each core will correspond to 50% of
 *          the run time.
 *
 * @return
 *  - ESP_OK
 *  - ESP_FIAL
 */
esp_err_t audio_sys_get_real_time_stats(void);

#ifdef __cplusplus
}
#endif

#endif
