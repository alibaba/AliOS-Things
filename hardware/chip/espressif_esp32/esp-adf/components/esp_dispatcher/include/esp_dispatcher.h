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

#ifndef __ESP_DISPATCHER_H__
#define __ESP_DISPATCHER_H__

#include <stdbool.h>

#include "esp_err.h"
#include "esp_action_def.h"

#ifdef __cplusplus
extern "C" {
#endif

#define DEFAULT_ESP_DISPATCHER_STACK_SIZE      (4*1024)
#define DEFAULT_ESP_DISPATCHER_TASK_PRIO       (10)
#define DEFAULT_ESP_DISPATCHER_TASK_CORE       (0)

/**
 * @brief The dispatcher configuration
 */
typedef struct {
    int                         task_stack;             /*!< >0 Task stack; =0 with out task created */
    int                         task_prio;              /*!< Task priority (based on freeRTOS priority) */
    int                         task_core;              /*!< Task running in core (0 or 1) */
    bool                        stack_in_ext;           /*!< Try to allocate stack in external memory */
} esp_dispatcher_config_t;

typedef struct esp_dispatcher *esp_dispatcher_handle_t;

/**
 * @brief the delegate result callback type
 */
typedef void (*func_ret_cb_t)(action_result_t ret, void *user_data);

#define ESP_DISPATCHER_CONFIG_DEFAULT() { \
    .task_stack = DEFAULT_ESP_DISPATCHER_STACK_SIZE, \
    .task_prio = DEFAULT_ESP_DISPATCHER_TASK_PRIO, \
    .task_core = DEFAULT_ESP_DISPATCHER_TASK_CORE, \
    .stack_in_ext = false, \
}

/**
 * brief      Create ESP dispatcher instance
 *
 * @param cfg   Configuration of the ESP dispatcher instance
 *
 * @return
 *     - NULL,  Fail
 *     - Others, Success
 */
esp_dispatcher_handle_t esp_dispatcher_create(esp_dispatcher_config_t *cfg);

/**
 * brief      Destroy ESP dispatcher instance
 *
 * @param handle   The ESP dispatcher instance
 *
 * @return
 *     - ESP_OK
 *     - ESP_ERR_INVALID_ARG
 */
esp_err_t esp_dispatcher_destroy(esp_dispatcher_handle_t handle);

/**
 * brief      Register index of event and execution function to ESP dispatcher instance
 *
 * @param handle            The ESP dispatcher instance
 * @param exe_inst          The execution instance
 * @param sub_event_index   The index of event
 * @param func              The execution function
 *
 * @return
 *     - ESP_OK
 *     - ESP_ERR_ADF_ALREADY_EXISTS
 *     - ESP_ERR_INVALID_ARG
 */
esp_err_t esp_dispatcher_reg_exe_func(esp_dispatcher_handle_t handle, void *exe_inst, int sub_event_index, esp_action_exe func);

/**
 * brief      Execution function with specific index of event.
 *            This is a synchronization interface.
 *
 * @param handle            The ESP dispatcher instance
 * @param sub_event_index   The index of event
 * @param arg               The arguments of execution function
 * @param result            The result of execution function
 *
 * @return
 *     - ESP_OK
 *     - ESP_ERR_INVALID_ARG
 *     - ESP_ERR_ADF_TIMEOUT, send request command timeout.
 *     - Others, execute function result.
 */
esp_err_t esp_dispatcher_execute(esp_dispatcher_handle_t handle, int sub_event_index,
                                    action_arg_t *arg, action_result_t *result);

/**
 * brief      Execution function with specific index of event.
 *            This is a asynchronous interface.
 *
 * @param handle            The ESP dispatcher instance
 * @param sub_event_index   The index of event
 * @param arg               The arguments of execution function
 * @param ret_cb            The call back used to receive the function execute result
 * @param user_data         The data used in callback
 *
 * @return
 *     - ESP_OK
 *     - ESP_ERR_INVALID_ARG
 *     - ESP_ERR_ADF_TIMEOUT, send request command timeout.
 *     - Others, execute function result.
 */
esp_err_t esp_dispatcher_execute_async(esp_dispatcher_handle_t dh, int sub_event_index,
                                    action_arg_t *in_para, func_ret_cb_t ret_cb, void* user_data);
/**
 * @brief      Synchronize invoke functions in ESP dispatcher
 *
 * @param handle            The ESP dispatcher instance
 * @param func              The function to invoke
 * @param exe_inst          The execution instance
 * @param arg               The arguments of execution function
 * @param result            The result of execution function
 *
 * @return
 *     - ESP_OK
 *     - ESP_ERR_INVALID_ARG
 *     - ESP_ERR_ADF_TIMEOUT, send request command timeout.
 *     - Others, execute function result.
 */
esp_err_t esp_dispatcher_execute_with_func(esp_dispatcher_handle_t handle,
                                        esp_action_exe func,
                                        void *exe_inst,
                                        action_arg_t *arg,
                                        action_result_t *ret);
/**
 * @brief      Asynchronous invoke functions in ESP dispatcher
 *
 * @param handle            The ESP dispatcher instance
 * @param func              The function to invoke
 * @param exe_inst          The execution instance
 * @param arg               The arguments of execution function
 * @param ret_cb            The call back used to receive the function execute result
 * @param user_data         The data used in callback
 *
 * @return
 *     - ESP_OK
 *     - ESP_ERR_INVALID_ARG
 *     - ESP_ERR_ADF_TIMEOUT, send request command timeout.
 *     - Others, execute function result.
 */
esp_err_t esp_dispatcher_execute_with_func_async(esp_dispatcher_handle_t handle,
                                        esp_action_exe func,
                                        void *exe_inst,
                                        action_arg_t *arg,
                                        func_ret_cb_t ret_cb,
                                        void* user_data);

#ifdef __cplusplus
}
#endif

#endif // End of __ESP_DISPATCHER_H__
