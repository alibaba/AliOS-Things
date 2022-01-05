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

#ifndef __OTA_PROC_DEFAULT__
#define __OTA_PROC_DEFAULT__

#include "esp_ota_ops.h"
#include "esp_partition.h"

#include "audio_element.h"
#include "ota_service.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
  * @brief     get the default process of `app partition` upgrade
  *
  * @param[in]  handle          pointer to `ota_upgrade_ops_t` structure
  *
  * @return
  *    - void
  */
void ota_app_get_default_proc(ota_upgrade_ops_t *ops);

/**
  * @brief     get the default process of `data partition` upgrade
  *
  * @param[in]  handle          pointer to `ota_upgrade_ops_t` structure
  *
  * @return
  *    - void
  */
void ota_data_get_default_proc(ota_upgrade_ops_t *ops);

/**
  * @brief     read from the stream of upgrading
  *
  * @param[in]  handle          pointer to upgrade handle
  * @param[in]  buf             pointer to receive buffer
  * @param[in]  wanted_size     bytes to read
  *
  * @return
  *    - OTA_SERV_ERR_REASON_SUCCESS:  Success
  *    - Others:  Failed
  */
ota_service_err_reason_t ota_data_image_stream_read(void *handle, char *buf, int wanted_size);

/**
  * @brief     write to the data partition under upgrading
  *
  * @param[in]  handle          pointer to upgrade handle
  * @param[in]  buf             pointer to data buffer
  * @param[in]  size            bytes to write
  *
  * @return
  *    - OTA_SERV_ERR_REASON_SUCCESS:  Success
  *    - Others:  Failed
  */
ota_service_err_reason_t ota_data_partition_write(void *handle, char *buf, int size);

/**
  * @brief     Generate a number by image version
  * @Note      The version should be (V0.0.0 - V255.255.255)
  *
  * @param[in]  handle          pointer to upgrade handle
  *
  * @return
  *    - -1:      Failed
  *    - Others:  version number
  */
int ota_get_version_number(char *version);

#ifdef __cplusplus
}
#endif

#endif /*__OTA_PROC_DEFAULT__*/