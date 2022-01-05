/*
 * ESPRESSIF MIT License
 *
 * Copyright (c) 2018 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
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

#ifndef _SPIFFS_DEV_H_
#define _SPIFFS_DEV_H_

#include "sys/queue.h"
#include "audio_error.h"
#include "audio_common.h"
#include "esp_peripherals.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Peripheral spiffs event id
 */
typedef enum  {
    SPIFFS_STATUS_UNKNOWN,              /*!< No event */
    SPIFFS_STATUS_MOUNTED,              /*!< SPIFFS mounted successfully */
    SPIFFS_STATUS_UNMOUNTED,            /*!< SPIFFS unmounted successfully */
    SPIFFS_STATUS_MOUNT_ERROR,          /*!< SPIFFS mount error */
    SPIFFS_STATUS_UNMOUNT_ERROR,        /*!< SPIFFS unmount error */
} periph_spiffs_event_id_t;

/**
 * @brief   The SPIFFS Peripheral configuration
 */
typedef struct {
    const char* root;               /*!< Base path for vfs */
    const char* partition_label;    /*!< Optional, label of SPIFFS partition to use. If set to NULL, first partition with subtype=spiffs will be used. */
    size_t max_files;               /*!< Maximum number of files that could be open at the same time. */
    bool format_if_mount_failed;    /*!< If true, it will format the file system if it fails to mount. */
} periph_spiffs_cfg_t;


/**
 * @brief      Create the spiffs peripheral handle for esp_peripherals
 *
 * @note       The handle created by this function will be automatically destroyed when `esp_periph_destroy` is called
 *
 * @param      spiffs_config  The spiffs configuration
 *
 * @return     The esp peripheral handle
 */
esp_periph_handle_t periph_spiffs_init(periph_spiffs_cfg_t* spiffs_config);

/**
 * @brief      Check if the SPIFFS is mounted or not.
 *
 * @param[in]  periph  The periph
 *
 * @return     SPIFFS mounted state
 */
bool periph_spiffs_is_mounted(esp_periph_handle_t periph);

#ifdef __cplusplus
}
#endif

#endif
