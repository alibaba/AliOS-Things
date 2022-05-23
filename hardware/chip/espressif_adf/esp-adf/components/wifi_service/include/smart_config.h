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

#ifndef _SMART_CONFIG_H_
#define _SMART_CONFIG_H_

#include "esp_wifi_setting.h"
#include "esp_smartconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   esp smartconfig configuration
 */
typedef struct {
    smartconfig_type_t type;         /*!< Type of smartconfig */
} smart_config_info_t;

#define SMART_CONFIG_INFO_DEFAULT() { \
    .type = SC_TYPE_ESPTOUCH, \
}

/**
 * brief        Create smartconfig setting handle instance
 *
 * @param info  Configuration of the smartconfig
 *
 * @return
 *     - NULL, Fail
 *     - Others, Success
 */
esp_wifi_setting_handle_t smart_config_create(smart_config_info_t *info);

#ifdef __cplusplus
}
#endif

#endif
