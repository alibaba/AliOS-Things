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
 * File: lightduer_ota_package_info.h
 * Auth: Zhong Shuai (zhongshuai@baidu.com)
 * Desc: OTA Package Information Head File
 */

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#ifndef BAIDU_DUER_LIGHTDUER_INCLUDE_LIGHTDUER_OTA_PACKAGE_INFO_H
#define BAIDU_DUER_LIGHTDUER_INCLUDE_LIGHTDUER_OTA_PACKAGE_INFO_H

#define KEY_LEN                      (128)
#define PACKAGE_NAME_LENGTH          (15)
#define META_VERSION_LENGTH          (3)
#define PACKAGE_VERSION_LENGTH       (15)
#define INSTALL_PATH_LENGTH          (255)
#define MODULE_NAME_LENGTH           (15)
#define MODULE_VERSION_LENGTH        (15)
#define MODULE_SIGNATURE_LENGTH      (33)

#ifdef __cplusplus
extern "C" {
#endif

typedef enum _duer_ota_package_type_s {
    PACKAGE_TYPE_OS,                     // Operation system
    PACKAGE_TYPE_APP,                    // Application
    PACKAGE_TYPE_FIRMWARE,               // Chip firmware
    PACKAGE_TYPE_MIXED,                  // MIXED
} duer_ota_package_type_t;

typedef enum _duer_ota_module_type_s {
    MODULE_TYPE_BIN,
    MODULE_TYPE_IMG,
    MODULE_TYPE_JSON,
    MODULE_TYPE_SO,
} duer_ota_module_type_t;

// package header
typedef struct _duer_ota_package_header_s {
    // 'mbed' verify tag
    uint8_t tag[4];
    // package header size
    uint32_t package_header_size;
    // package signature size: 1024 bits
    uint32_t package_signature_size;
    // package signature: include meta.json and all modules
    unsigned char package_signature[KEY_LEN];
    // meta.json signature size: 1024 bits
    uint32_t meta_signature_size;
    // meta.json signature
    unsigned char meta_signature[KEY_LEN];
    // meta.json size: used for decompress meta.json from package body
    uint32_t meta_size;
    // size of meta.json + all modules (before compress)
    uint32_t ori_package_size;
} duer_ota_package_header_t;

typedef struct _duer_ota_package_basic_info_s {
    uint8_t package_name[PACKAGE_NAME_LENGTH + 1];
    uint8_t meta_version[META_VERSION_LENGTH + 1];
    duer_ota_package_type_t package_type;
    // Enforce the update
    bool update;
} duer_ota_package_basic_info_t;

typedef struct _duer_ota_module_info_s {
    uint8_t module_name[MODULE_NAME_LENGTH + 1];
    uint32_t module_size;
    duer_ota_module_type_t module_type;
    // Enforce the update
    bool update;
    uint8_t module_signature[MODULE_SIGNATURE_LENGTH + 1];
    uint8_t module_version[MODULE_VERSION_LENGTH + 1];
    uint8_t module_support_hardware_version[MODULE_VERSION_LENGTH + 1];
    uint8_t module_install_path[INSTALL_PATH_LENGTH + 1];
} duer_ota_module_info_t;

typedef struct _duer_ota_package_install_info_s {
    uint8_t package_install_path[INSTALL_PATH_LENGTH + 1];
    uint8_t module_count;
    duer_ota_module_info_t *module_list;
} duer_ota_package_install_info_t;

typedef struct _duer_ota_update_info_s {
    uint8_t package_version[PACKAGE_VERSION_LENGTH + 1];
} duer_ota_update_info_t;

#ifdef __cplusplus
}
#endif

#endif // BAIDU_DUER_LIGHTDUER_INCLUDE_LIGHTDUER_OTA_PACKAGE_INFO_H
