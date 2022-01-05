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
 * File: lightduer_ota_verification.h
 * Auth: Zhong Shuai (zhongshuai@baidu.com)
 * Desc: OTA Verification Head File
 */

#ifndef BAIDU_DUER_LIGHTDUER_INCLUDE_LIGHTDUER_OTA_VERIFICATION_H
#define BAIDU_DUER_LIGHTDUER_INCLUDE_LIGHTDUER_OTA_VERIFICATION_H

#include "mbedtls/md5.h"
#include "mbedtls/rsa.h"
#include "mbedtls/sha1.h"
#include "lightduer_ota_package_info.h"

#define ERR_MSG_LEN (60)

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _duer_ota_verifier_s {
    mbedtls_rsa_context rsa;
    mbedtls_sha1_context sha1;
    char err_msg[ERR_MSG_LEN + 1];
} duer_ota_verifier_t;

/*
 * Create a OTA verifier
 *
 * @param void:
 *
 * @return: Success: duer_ota_verifier_t *
 *           Failed: NULL
 */
extern duer_ota_verifier_t *duer_ota_verification_create_verifier(void);

/*
 * Initialize a OTA verifier
 *
 * @param void:
 *
 * @return: Success: DUER_OK
 *           Failed: Other
 */
extern int duer_ota_verification_init_verifier(duer_ota_verifier_t *verifier);

/*
 * Destroy a OTA verifier
 *
 * @param verifier: duer_ota_verifier_t *
 *
 * @return void:
 */
extern void duer_ota_verification_destroy_verifier(duer_ota_verifier_t *verifier);

/*
 * Verify data
 *
 * @param verifier: duer_ota_verifier_t *
 *            data: uint8_t *
 *            size: size_t
 *
 * @return void:
 */
extern void duer_ota_verification_update(
        duer_ota_verifier_t *verifier,
        uint8_t const *data,
        size_t size);

/*
 * Verify the OTA package
 *
 * @param verify: duer_ota_verifier_t *
 *        header: Heare information
 *
 * @return: Success: DUER_OK
 *           Failed: Other
 */
extern int duer_ota_verification_verify(duer_ota_verifier_t *verifier, duer_ota_package_header_t const *header);

/*
 * Get verofoer error message
 *
 * @param verify: duer_ota_verifier_t *
 *
 * @return: Success: error message
 *           Failed: NULL
 */
extern char const *duer_ota_verification_get_err_msg(duer_ota_verifier_t const *verifier);

/*
 * Check wheater the error message exists
 *
 * @param verifier: verifier object
 *
 * @return: Success: 1
 *           Failed: -1
 */
extern int duer_ota_verification_check_err_msg(duer_ota_verifier_t *verifier);

#ifdef __cplusplus
}
#endif

#endif // BAIDU_DUER_LIGHTDUER_INCLUDE_LIGHTDUER_OTA_VERIFICATION_H
