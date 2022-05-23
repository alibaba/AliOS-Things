/*
 * ESPRESSIF MIT License
 *
 * Copyright (c) 2018 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
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

#ifndef __AUDIO_MUTEX_H__
#define __AUDIO_MUTEX_H__

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief       Create a mutex instance
 *
 * @return      - Others:      A mutex handle is returned
 *              - NULL:         Failed to create mutex
 */
void *mutex_create(void);

/**
 * @brief       Delete the mutex instance
 *
 * @param       mutex       The pointer to mutex handle
 *
 * @return      - 0:       Success to delete mutex
 */
int mutex_destroy(void *mutex);

/**
 * @brief       Take the mutex
 *
 * @param       mutex        The pointer to mutex handle
 *
 * @return      - 0:        The lock was obtained
 */
int mutex_lock(void *mutex);

/**
 * @brief       Release the mutex
 *
 * @param       mutex        The pointer to mutex handle
 *
 * @return      - 0:           The lock was released
 */
int mutex_unlock(void *mutex);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef __AUDIO_MUTEX_H__ */
