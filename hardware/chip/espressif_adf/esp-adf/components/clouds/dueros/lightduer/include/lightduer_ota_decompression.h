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
 * File: lightduer_ota_decompression.h
 * Auth: Zhong Shuai (zhongshuai@baidu.com)
 * Desc: OTA decompression abstraction layer header file
 */

#ifndef BAIDU_DUER_LIGHTDUER_INCLUDE_LIGHTDUER_OTA_DECOMPRESSION_H
#define BAIDU_DUER_LIGHTDUER_INCLUDE_LIGHTDUER_OTA_DECOMPRESSION_H

#include <stdint.h>
#include "lightduer_mutex.h"

#ifdef __cplusplus
extern "C" {
#endif

#define NAME_LENGTH (20)
#define ERR_MSG_LEN (60)

typedef enum _duer_ota_decompression_type_s {
    ZLIB = 0,
    DECOMPRESSION_MAX = 1,
} duer_ota_decompress_type_t;

typedef int (*DECOMPRESS_DATA_HANDLER_FUN)(void *custom_data, char const *buf, size_t len);

typedef struct _duer_ota_unpacker_decom_s {
    char name[NAME_LENGTH + 1];
    duer_mutex_t lock;
    void *pdata;
    char err_msg[ERR_MSG_LEN + 1];
    int (*init)(struct _duer_ota_unpacker_decom_s *decompression, void **pdata);
    int (*unzip)(
            struct _duer_ota_unpacker_decom_s *decompression,
            void *pdata,
            char const *compressed_data,
            size_t data_size,
            void *custom_data,
            DECOMPRESS_DATA_HANDLER_FUN data_handler);
    int (*recovery)(struct _duer_ota_unpacker_decom_s *decompression, void *pdata);
    int (*sync)(struct _duer_ota_unpacker_decom_s *decompression, void *pdata);
    int (*unint)(struct _duer_ota_unpacker_decom_s *decompression, void *pdata);
} duer_ota_decompression_t;

/*
 * Initialize decompression module
 *
 * @param void:
 *
 * @return: Success: DUER_OK
 *           Failed: Other
 */
extern int duer_ota_init_decompression(void);

/*
 * Create a decompression object
 *
 * @param name: the name of decompression
 *
 * @return: Success: duer_ota_decompression_t *
 *           Failed: NULL
 */
extern duer_ota_decompression_t *duer_ota_unpacker_create_decompression(char const *name);

/*
 * Get decompression object
 *
 * @param name: the name of decompression
 *
 * @return: Success: duer_ota_decompression_t *
 *           Failed: NULL
 */
extern duer_ota_decompression_t *duer_ota_unpacker_get_decompression(
        duer_ota_decompress_type_t type);

/*
 * Destroy decompression object
 *
 * @param void:
 *
 * @return: Success: duer_ota_decompression_t *
 *           Failed: NULL
 */
extern int duer_ota_unpacker_destroy_decompression(duer_ota_decompression_t *decompression);

/*
 * Get decompression name
 *
 * @param decompression: decompression object
 *
 * @return: Success: char const *
 *           Failed: NULL
 */
extern const char *duer_ota_unpacker_get_decompression_name(
        duer_ota_decompression_t const *decompression);

/*
 * Register a decompression library
 *
 * @param decompression: decompression object
 *        type:          decompression library type
 *
 * @return: Success: DUER_OK
 *           Failed: Other
 */
extern int duer_ota_unpacker_register_decompression(
        duer_ota_decompression_t *decompression,
        duer_ota_decompress_type_t type);

/*
 * Unregister a decompression library
 *
 * @param type: decompression library type
 *
 * @return: Success: DUER_OK
 *           Failed: Other
 */
extern int duer_ota_unpacker_unregister_decompression(duer_ota_decompress_type_t type);

/*
 * Initialize decompression
 *
 * @param decompression: decompression object
 *
 * @return: Success: DUER_OK
 *           Failed: Other
 */
extern int duer_ota_unpacker_init_decompression(duer_ota_decompression_t *decompression);

/*
 * Unzip data
 *
 * @param decompression: decompression object
 *      compressed_data: compressed data
 * compressed_data_size: size of corpressed data
 *             unpacker: Unpacker object
 *         data_handler: A function which receive the decompressed data
 *
 * @return: Success: DUER_OK
 *           Failed: Other
 */
extern int duer_ota_unpacker_unzip(
        duer_ota_decompression_t *decompression,
        char const *compressed_data,
        size_t compressed_data_size,
        void *custom_data,
        DECOMPRESS_DATA_HANDLER_FUN data_handler);

/*
 * Uninitialize decompression
 *
 * @param decompression: decompression object
 *
 * @return: Success: DUER_OK
 *           Failed: Other
 */
extern int duer_ota_unpacker_uninit_decompression(duer_ota_decompression_t *decompression);

/*
 * Try to repair data
 *
 * @param decompression: decompression object
 *
 * @return: Success: DUER_OK
 *           Failed: Other
 */
extern int duer_ota_unpacker_recovery_decompression(duer_ota_decompression_t *decompression);

/*
 * Synchronous data
 *
 * @param decompression: decompression object
 *
 * @return: Success: DUER_OK
 *           Failed: Other
 */
extern int duer_ota_unpacker_sync_decompression(duer_ota_decompression_t *decompression);

/*
 * Report error message
 *
 * @param decompression: OTA Decompression
 *              err_msg: error message
 *             err_code: error code
 *
 * @return void:
 */
extern void duer_ota_decompression_report_err(
        duer_ota_decompression_t *decompression,
        char const *err_msg,
        int err_code);
/*
 * Get decompression error message
 *
 * @param decompression: decompression object
 *
 * @return: Success: error message
 *           Failed: NULL
 */
extern char const *duer_ota_decompression_get_err_msg(
        duer_ota_decompression_t const *decompression);

/*
 * Check wheater the error message exists
 *
 * @param decompression: decompression object
 *
 * @return: Success: 1
 *           Failed: -1
 */
extern int duer_ota_decompression_check_err_msg(duer_ota_decompression_t *decompression);

#ifdef __cplusplus
}
#endif

#endif // BAIDU_DUER_LIGHTDUER_INCLUDE_LIGHTDUER_OTA_DECOMPRESSION_H
