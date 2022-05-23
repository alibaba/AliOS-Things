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

#ifndef _AUDIO_THREAD_H_
#define _AUDIO_THREAD_H_

#include "esp_err.h"
#include "stdbool.h"

#ifdef __cplusplus
extern "C" {
#endif

#define audio_thread_t xTaskHandle

/**
 * @brief       Allocate handle if not allocated and create a thread
 *
 * @param       p_handle        pointer to audio_thread_t handle
 * @param       name            Task name
 * @param       main_func       The function which task will execute
 * @param       stack           Task stack
 * @param       prio            Task priority
 * @param       stack_in_ext    If task should reside in external memory
 * @param       core_id         Core to which task will be pinned
 *
 * @return      - ESP_OK :      Task creation successful
 *              - ESP_FAIL:     Failed to create task
 *
 * @note       - Please apply the $ADF_PATH/idf_patches/idf_v3.3/4.x_freertos.patch first.
 *             - Please enable support for external RAM and `Allow external memory as an argument to xTaskCreateStatic`
 *              to be able to use external memory for task stack, namely `CONFIG_SPIRAM_BOOT_INIT=y` and `CONFIG_SPIRAM_ALLOW_STACK_EXTERNAL_MEMORY=y`.
 *
 */
esp_err_t audio_thread_create(audio_thread_t *p_handle, const char* name, void(*main_func)(void* arg), void *arg,
                              uint32_t stack, int prio, bool stack_in_ext, int core_id);

/**
 * @brief       Cleanup all the task memory
 *
 * @param       p_handle        The pointer to audio_thread_t handle
 *
 * @return      - ESP_OK :      Task cleanup successful
 *              - ESP_FAIL:     Task is already cleaned up
 *
 * @note        must be called from different task after this task is deleted.
 */
esp_err_t audio_thread_cleanup(audio_thread_t *p_handle);

/**
 * @brief       Delete the task
 *
 * @param       p_handle        The pointer to audio_thread_t handle
 *
 * @return      - ESP_OK :      Task deleted successfully
 *              - ESP_FAIL:     Task is not running or cleaned up
 *
 * @note        This only deletes the task and all the memory cleanup should be done with `audio_thread_cleanup`
 */
esp_err_t audio_thread_delete_task(audio_thread_t *p_handle);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef _AUDIO_THREAD_H_ */