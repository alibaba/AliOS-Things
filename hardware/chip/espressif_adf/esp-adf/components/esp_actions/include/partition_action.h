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

#ifndef __PARTITION_ACTION__
#define __PARTITION_ACTION__

#include "esp_partition.h"
#include "esp_action_def.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   The arguments structure of partition find first action
 */
typedef struct partition_find_args_s {
    esp_partition_type_t type;
    esp_partition_subtype_t subtype;
    const char *label;
} partition_find_args_t;

/**
 * @brief   The arguments structure of partition read action
 */
typedef struct partition_read_args_s {
    const esp_partition_t *partition;
    size_t src_offset;
    void *dst;
    size_t size;
} partition_read_args_t;

/**
 * @brief   The arguments structure of partition write action
 */
typedef struct partition_write_args_s {
    const esp_partition_t *partition;
    size_t dst_offset;
    void *src;
    size_t size;
} partition_write_args_t;

/**
 * @brief      Partition find first
 *
 * @param instance          The execution instance
 * @param arg               The arguments of execution function
 * @param result            The result of execution function
 *
 * @return
 *     - ESP_OK, success
 *     - Others, error
 */
esp_err_t partition_find_action(void *instance, action_arg_t *arg, action_result_t *result);

/**
 * @brief      Partition read
 *
 * @param instance          The execution instance
 * @param arg               The arguments of execution function
 * @param result            The result of execution function
 *
 * @return
 *     - ESP_OK, success
 *     - Others, error
 */
esp_err_t partition_read_action(void *instance, action_arg_t *arg, action_result_t *result);

/**
 * @brief      Partition write
 *
 * @param instance          The execution instance
 * @param arg               The arguments of execution function
 * @param result            The result of execution function
 *
 * @return
 *     - ESP_OK, success
 *     - Others, error
 */
esp_err_t partition_write_action(void *instance, action_arg_t *arg, action_result_t *result);

#ifdef __cplusplus
}
#endif

#endif /*__PARTITION_ACTION__*/
