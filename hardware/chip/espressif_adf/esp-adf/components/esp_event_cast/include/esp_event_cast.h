/*
 * ESPRESSIF MIT License
 *
 * Copyright (c) 2020 <ESPRESSIF SYSTEMS (SHANGHAI) CO., LTD>
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

#ifndef __ESP_EVENT_CAST_H__
#define __ESP_EVENT_CAST_H__

typedef struct esp_event_cast *esp_event_cast_handle_t;

/**
 * @brief Create esp_event_cast instance
 *
 * @return
 *     - Valid pointer on success
 *     - NULL when any errors
 */
esp_event_cast_handle_t esp_event_cast_create(void);

/**
 * @brief Destroy the specific esp_event_cast instance
 *
 * @praram handle: A poniter to esp_event_cast_handle_t
 *
 * @return
 *     - ESP_OK:    success
 *     - ESP_FAIL:  Parameter is NULL
 */
esp_err_t esp_event_cast_destroy(esp_event_cast_handle_t handle);

/**
 * @brief Add queue to esp_event_cast_handle_t object
 *
 * @param  handle:  A poniter to esp_event_cast_handle_t
 * @praram que:     The specific queue as receiver added to esp_event_cast_handle_t instance
 *
 * @return
 *     - ESP_OK: success
 *     - ESP_FAIL: others
 */
esp_err_t esp_event_cast_register(esp_event_cast_handle_t handle, xQueueHandle que);

/**
 * @brief Remove queue item from esp_event_cast_handle_t object, but does't delete the queue
 *
 * @param  handle : A poniter to esp_event_cast_handle_t
 * @praram que:     A queue will be removed from `handle` instance
 *
 * @return
 *     - ESP_OK: success
 *     - ESP_FAIL: others
 */
esp_err_t esp_event_cast_unregister(esp_event_cast_handle_t handle, xQueueHandle que);

/**
 * @brief Broadcasting the data to receiver
 *
 * @param  handle: A poniter to esp_event_cast_handle_t
 * @param  data:   Data packet will be broadcasting
 *
 * @return
 *     - ESP_OK: success
 *     - ESP_FAIL: others
 */
esp_err_t esp_event_cast_broadcasting(esp_event_cast_handle_t handle, void *data);

/**
 * @brief Broadcasting the data to receiver from ISR
 *
 * @note This API is untested
 *
 * @param  handle: A poniter to esp_event_cast_handle_t
 * @param  data:   Data packet will be broadcasting
 *
 * @return
 *     - ESP_OK: success
 *     - ESP_FAIL: others
 */
esp_err_t esp_event_cast_broadcasting_isr(esp_event_cast_handle_t handle, void *data);

/**
 * @brief Get the count of registered receiver
 *
 * @param  handle: A poniter to esp_event_cast_handle_t
 *
 * @return
 *     - ESP_FAIL: Fail
 *     - Others: Number of registered receiver
 */
esp_err_t esp_event_cast_get_count(esp_event_cast_handle_t handle);

#endif  //__ESP_EVENT_CAST_H__
