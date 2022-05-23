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

#ifndef __NVS_ACTION_H__
#define __NVS_ACTION_H__

#include "nvs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief The arguments structure of nvs open action
 */
typedef struct {
    const char *name;
    nvs_open_mode open_mode;
} nvs_action_open_args_t;

/**
 * @brief The arguments structure of nvs open partition action
 */
typedef struct {
    const char *partition;
    char *name;
    nvs_open_mode open_mode;
} nvs_action_open_partition_args_t;

/**
 * @brief The arguments structure of nvs write action
 */
typedef struct {
    const char *key;
    nvs_type_t type;
    union {
        int8_t i8;
        int16_t i16;
        int32_t i32;
        int64_t i64;
        uint8_t u8;
        uint16_t u16;
        uint32_t u32;
        uint64_t u64;
        char* string;
        void* blob;
    } value;
    int len;
} nvs_action_set_args_t;

/**
 * @brief The arguments structure of nvs read action
 */
typedef struct {
    const char *key;
    nvs_type_t type;
    size_t wanted_size;
} nvs_action_get_args_t;

/**
 * @brief      NVS Open
 *
 * @param instance          The execution instance
 * @param arg               The arguments of execution function
 * @param result            The result of execution function
 *
 * @return
 *     - ESP_OK, success
 *     - Others, error
 */
esp_err_t nvs_action_open(void *instance, action_arg_t *arg, action_result_t *result);

/**
 * @brief      NVS Open form partition
 *
 * @param instance          The execution instance
 * @param arg               The arguments of execution function
 * @param result            The result of execution function
 *
 * @return
 *     - ESP_OK, success
 *     - Others, error
 */
esp_err_t nvs_action_open_from_partion(void *instance, action_arg_t *arg, action_result_t *result);

/**
 * @brief      Write data to NVS
 *
 * @param instance          The NVS instance
 * @param arg               The arguments of execution function
 * @param result            The result of execution function
 *
 * @return
 *     - ESP_OK, success
 *     - Others, error
 */
esp_err_t nvs_action_set(void *instance, action_arg_t *arg, action_result_t *result);

/**
 * @brief      Read data from NVS
 *
 * @param instance          The NVS instance
 * @param arg               The arguments of execution function
 * @param result            The result of execution function
 *
 * @return
 *     - ESP_OK, success
 *     - Others, error
 */
esp_err_t nvs_action_get(void *instance, action_arg_t *arg, action_result_t *result);

/**
 * @brief      Erase key in NVS
 *
 * @param instance          The NVS instance
 * @param arg               The arguments of execution function
 * @param result            The result of execution function
 *
 * @return
 *     - ESP_OK, success
 *     - Others, error
 */
esp_err_t nvs_action_erase_key(void *instance, action_arg_t *arg, action_result_t *result);

/**
 * @brief      Erase all keys in NVS
 *
 * @param instance          The NVS instance
 * @param arg               The arguments of execution function
 * @param result            The result of execution function
 *
 * @return
 *     - ESP_OK, success
 *     - Others, error
 */
esp_err_t nvs_action_erase_all(void *instance, action_arg_t *arg, action_result_t *result);

/**
 * @brief      Commit the modification.
 *
 * @param instance          The NVS instance
 * @param arg               The arguments of execution function
 * @param result            The result of execution function
 *
 * @return
 *     - ESP_OK, success
 *     - Others, error
 */
esp_err_t nvs_action_commit(void *instance, action_arg_t *arg, action_result_t *result);

/**
 * @brief      Close the NVS handle.
 *
 * @param instance          The NVS instance
 * @param arg               The arguments of execution function
 * @param result            The result of execution function
 *
 * @return
 *     - ESP_OK, success
 *     - Others, error
 */
esp_err_t nvs_action_close(void *instance, action_arg_t *arg, action_result_t *result);

/**
 * @brief      Get NVS status
 *
 * @param instance          The NVS instance
 * @param arg               The arguments of execution function
 * @param result            The result of execution function
 *
 * @return
 *     - ESP_OK, success
 *     - Others, error
 */
esp_err_t nvs_action_get_stats(void *instance, action_arg_t *arg, action_result_t *result);

/**
 * @brief      Get NVS used entry count
 *
 * @param instance          The NVS instance
 * @param arg               The arguments of execution function
 * @param result            The result of execution function
 *
 * @return
 *     - ESP_OK, success
 *     - Others, error
 */
esp_err_t nvs_action_get_used_entry_count(void *instance, action_arg_t *arg, action_result_t *result);

#ifdef __cplusplus
}
#endif

#endif /* __NVS_ACTION_H__ */