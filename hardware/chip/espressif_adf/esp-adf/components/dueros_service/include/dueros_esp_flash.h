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

#ifndef _DUEROS_SERVICE_ESP_FLASH_H
#define _DUEROS_SERVICE_ESP_FLASH_H

#include "lightduer_types.h"
#include "lightduer_flash_strings.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * @brief Initialize flash partition for alert service
 *
 * @param[in] module   The audio service instance
 *
 * @return
 *     - NULL, Fail
 *     - Others, Success
 */
duer_flash_string_context_t *duer_flash_string_init(duer_flash_module module);

/*
 * @brief Initialize flash partition for dueros service
 *
 * @param[in] label     A pointer to partition label
 * @param[in] ctx       A pointer to `duer_flash_context_t`
 * @param[in] config    A pointer to `duer_flash_config_t`
 *
 * @return
 *     - ESP_OK, Success
 *     - Others, Fail
 */
duer_status_t duer_esp_flash_init(const char *label, duer_flash_context_t *ctx, duer_flash_config_t *config);

#ifdef __cplusplus
}
#endif

#endif /* _DUEROS_SERVICE_ESP_FLASH_H */
