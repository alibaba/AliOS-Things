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

#ifndef __ESP_DELEGATE_H__
#define __ESP_DELEGATE_H__

#include "esp_dispatcher.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief      Return the shared dispatcher handle, this will hold a dispatcher with its stack in DRAM.
 *
 * @note       This function is not thread safe.
 *             Tasks should not call this function at the same time when device boot up.
 *
 * @param
 *
 * @return
 *     - NULL,  Fail
 *     - Others, Success
 */
esp_dispatcher_handle_t esp_dispatcher_get_delegate_handle(void);

/**
 * @brief      Release the shared dispatcher handle, this will destroy the handle.
 *
 * @param
 *
 * @return
 *     - ESP_OK
 *     - Others, Fail
 */
esp_err_t esp_dispatcher_release_delegate_handle(void);

#ifdef __cplusplus
}
#endif

#endif // End of __ESP_DELEGATE_H__
