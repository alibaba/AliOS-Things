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

#ifndef __ESP_ACTION_DEF_H__
#define __ESP_ACTION_DEF_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief The result of action
 */
typedef struct {
    esp_err_t                           err;        /*!< Error code */
    void                                *data;      /*!< Pointer to result data */
    int                                 len;        /*!< Length of the data */
} action_result_t;

/**
 * @brief The action arguments
 */
typedef struct {
    void                                *data;      /*!< Pointer to arguments data */
    int                                 len;        /*!< Length of the data */
} action_arg_t;

typedef esp_err_t (*esp_action_exe) (void *instance, action_arg_t *arg, action_result_t *result);


#ifdef __cplusplus
}
#endif

#endif // __ESP_ACTION_DEF_H__