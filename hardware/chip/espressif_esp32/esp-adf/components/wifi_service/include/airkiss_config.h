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

#ifndef __AIRKISS_CONFIG_H__
#define __AIRKISS_CONFIG_H__

#include "esp_wifi_setting.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief airkiss lan data pack
 */
typedef struct {
    void *appid;        /*!< APP identifer data */
    void *deviceid;     /*!< Device identifer data */
} airkiss_lan_pack_param_t;

/**
 * @brief airkiss configurations
 */
typedef struct {
    airkiss_lan_pack_param_t    lan_pack;                /*!< User lan pack parameter */
    bool                        ssdp_notify_enable;      /*!< Notify enable flag */
    char                        *aes_key;                /*!< Airkiss aes key data */
} airkiss_config_info_t;

#define AIRKISS_CONFIG_INFO_DEFAULT() { \
    .lan_pack = { \
        .appid = NULL, \
        .deviceid = NULL, \
    }, \
    .ssdp_notify_enable = true, \
    .aes_key = NULL, \
}

/**
 * brief        Create airkiss setting handle instance
 *
 * @param info  Configuration of the airkiss
 *
 * @return
 *     - NULL, Fail
 *     - Others, Success
 */
esp_wifi_setting_handle_t airkiss_config_create(airkiss_config_info_t *info);

#ifdef __cplusplus
}
#endif

#endif
