/**
 * Copyright (2017) Baidu Inc. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
/*
 * File: lightduer_ota_unpacker.h
 * Auth: Zhong Shuai (zhongshuai@baidu.com)
 * Desc: OTA unpacker Head File
 */

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "lightduer_ota_verifier.h"
#include "lightduer_ota_installer.h"
#include "lightduer_ota_package_info.h"
#include "lightduer_ota_decompression.h"

#ifndef BAIDU_DUER_LIGHTDUER_INCLUDE_LIGHTDUER_OTA_UNPACKER_H
#define BAIDU_DUER_LIGHTDUER_INCLUDE_LIGHTDUER_OTA_UNPACKER_H

#define ERR_MSG_LEN (60)

#ifdef __cplusplus
extern "C" {
#endif

typedef enum _duer_ota_unpack_mode_s {
    SAVE_PACKAGE_HEADER     = 1,
    PARSE_PACKAGE_HEADER    = 2,
    SAVE_META_DATA          = 3,
    UNZIP_DATA              = 4,
    UNZIP_DATA_DONE         = 5,
    PARSE_META_DATA         = 6,
    GET_OTA_INSTALLER       = 7,
    UNZIP_MODLE_INFO        = 8,
    UNZIP_MODLE_DATA        = 9,
    NOTIFY_OTA_BEGIN        = 10,
    NOTIFY_OTA_END          = 11,
    DISTRIBUTE_MODULE_INFO  = 12,
    DISTRIBUTE_MODULE_DATA  = 13,
    VERIFY_MODULE_DATA      = 14,
    PUBLIC_KEY_VERIFICATION = 15,
    UPDATE_IMAGE_BEGIN      = 16,
    UPDATE_IMAGE            = 17,
    VERIFY_IMAGE            = 18,
    OTA_UPDATE_DONE         = 19,
    CANCEL_OTA_UPDATE       = 20,
    UNPACKER_MODE_MAX       = 21,
} duer_ota_unpacker_mode_t;

typedef struct _duer_ota_unpack_state_s {
    duer_ota_unpacker_mode_t mode;
    size_t received_data_size;
    size_t processed_data_size;
    size_t decompress_data_size;
    size_t used_decompress_data_offset;
    size_t used_data_offset;
    size_t meta_data_offset;
    size_t module_data_offset;
    size_t package_header_data_offset;
} duer_ota_unpack_state_t;

typedef struct _duer_ota_unpacker_s {
    // Lock the unpacker
    duer_mutex_t lock;
    duer_ota_verifier_t *verifier;
    duer_ota_unpack_state_t state;
    duer_ota_package_header_t package_header;
    duer_ota_package_basic_info_t basic_info;
    duer_ota_package_install_info_t install_info;
    void *meta_data;
    duer_ota_decompression_t *decompression;
    duer_ota_installer_t *installer;
    char err_msg[ERR_MSG_LEN];
} duer_ota_unpacker_t;

/*
 * Create a OTA unpacker
 *
 * @param void:
 *
 * @return: Success: duer_ota_unpacker_t *
 *           Failed: NULL
 */
extern duer_ota_unpacker_t *duer_ota_unpacker_create_unpacker(void);

/*
 * Destroy a OTA unpacker
 *
 * @param unpacker: OTA unpacker
 *
 * @return int: Success: DUER_OK
 *              Failed:  Other
 */
extern int duer_ota_unpacker_destroy_unpacker(duer_ota_unpacker_t *unpacker);


/*
 * Initialize a OTA unpacker
 *
 * @param unpacker: OTA unpacker
 *
 * @return int: Success: DUER_OK
 *              Failed:  Other
 */
extern int duer_ota_unpacker_init_unpacker(duer_ota_unpacker_t *unpacker);

/*
 * Get the basic information of the package
 *
 * @param unpacker: OTA unpacker
 *
 * @return : Success:duer_ota_package_basic_info_t *
 *           Failed: NULL
 */
extern duer_ota_package_basic_info_t const *duer_ota_unpacker_get_package_basic_info(
        duer_ota_unpacker_t const *unpacker);

/*
 * Get the header information of the package
 *
 * @param unpacker: OTA unpacker
 *
 * @return : Success: duer_ota_package_header_t *
 *           Failed: NULL
 */
extern duer_ota_package_header_t const *duer_ota_unpacker_get_package_header_info(
        duer_ota_unpacker_t const *unpacker);

/*
 * Unpacke a OTA upadate package
 *
 * @param unpacker: OTA unpacker
 *            data: data of the package
 *            size: data size
 *
 * @return int: Success: DUER_OK
 *              Failed:  Other
 */
extern int duer_ota_unpacker_unpack_package(
        duer_ota_unpacker_t *unpacker,
        uint8_t const *data,
        size_t size);

/*
 * Set the unpacker mode
 *
 * @param unpacker: OTA unpacker
 *            mode: Unpacker mode
 *
 * @return int: Success: DUER_OK
 *              Failed:  Other
 */
extern int duer_ota_unpacker_set_unpacker_mode(
        duer_ota_unpacker_t *unpacker,
        duer_ota_unpacker_mode_t mode);

/*
 * Get the unpacker mode
 *
 * @param unpacker: OTA unpacker
 *
 * @return mode: Unpacker mode
 */
extern duer_ota_unpacker_mode_t duer_ota_unpacker_get_unpacker_mode(
        duer_ota_unpacker_t const *unpacker);

/*
 * Get the unpacker error message
 *
 * @param unpacker: OTA unpacker
 *
 * @return char const *: Success: error message
 *                       Failed:  NULL
 */
extern char const *duer_ota_unpacker_get_err_msg(duer_ota_unpacker_t const *unpacker);

/*
 * Check wheater the error message exists
 *
 * @param unpacker: unpacker object
 *
 * @return: Success: 1
 *           Failed: -1
 */
extern int duer_ota_unpacker_check_err_msg(duer_ota_unpacker_t *unpacker);

#ifdef __cplusplus
}
#endif

#endif // BAIDU_DUER_LIGHTDUER_INCLUDE_LIGHTDUER_OTA_UNPACKER_H
