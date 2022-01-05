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


#ifndef _INPUT_KEY_SERVICE_H_
#define _INPUT_KEY_SERVICE_H_

#include "esp_peripherals.h"
#include "periph_service.h"
#include "input_key_com_user_id.h"
#include "board.h"

#ifdef __cplusplus
extern "C" {
#endif

#define INPUT_KEY_SERVICE_TASK_STACK_SIZE (3 * 1024)
#define INPUT_KEY_SERVICE_TASK_PRIORITY   (5)
#define INPUT_KEY_SERVICE_TASK_ON_CORE    (1)

/**
 * @brief input key action id
 */
typedef enum {
    INPUT_KEY_SERVICE_ACTION_UNKNOWN = 0,    /*!< unknown action id */
    INPUT_KEY_SERVICE_ACTION_CLICK,          /*!< click action id */
    INPUT_KEY_SERVICE_ACTION_CLICK_RELEASE,  /*!< click release action id */
    INPUT_KEY_SERVICE_ACTION_PRESS,          /*!< long press action id */
    INPUT_KEY_SERVICE_ACTION_PRESS_RELEASE   /*!< long press release id */
} input_key_service_action_id_t;

/**
 * @brief input key's infomation
 */
typedef struct {
    esp_periph_id_t  type;             /*!< ID of peripherals */
    int              user_id;          /*!< The key's user id */
    int              act_id;           /*!< The key's action id */
} input_key_service_info_t;

/**
 * @brief input key's configuration
 */
typedef struct {
    periph_service_config_t based_cfg; /*!< Peripheral service configuration */
    esp_periph_set_handle_t handle;    /*!< Peripheral set handle */
} input_key_service_cfg_t;

#define INPUT_KEY_SERVICE_DEFAULT_CONFIG() {             \
    .based_cfg = {                                       \
        .task_stack = INPUT_KEY_SERVICE_TASK_STACK_SIZE, \
        .task_prio = INPUT_KEY_SERVICE_TASK_PRIORITY,    \
        .task_core = INPUT_KEY_SERVICE_TASK_ON_CORE,     \
        .extern_stack = false                            \
    }                                                    \
}

/**
 * @brief Initialize and start the input key service
 *
 * @param input_key_config Configuration of input key service
 *
 * @return NULL    failed
 *         others  input key service handle
 */

periph_service_handle_t input_key_service_create(input_key_service_cfg_t *input_key_config);

/**
 * @brief Get the state of input key service
 *
 * @param input_handle The handle of input key service
 *
 * @return state of input key service
 */
periph_service_state_t get_input_key_service_state(periph_service_handle_t input_handle);

/**
 * @brief Add input key's information to service list
 *
 * @param input_key_handle  handle of service
 * @param input_key_info    input key's information
 * @param add_key_num       number of keys
 *
 * @return ESP_OK   success
 *         ESP_FAIL failed
 */
esp_err_t input_key_service_add_key(periph_service_handle_t input_key_handle, input_key_service_info_t *input_key_info, int add_key_num);

#ifdef __cplusplus
}
#endif

#endif
