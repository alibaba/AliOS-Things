/**
 * \file hash.h
 *
 * \brief Generic message digest wrapper
 *
 * \author Adriaan de Jong <dejong@fox-it.com>
 *
 *  Copyright (C) 2006-2015, ARM Limited, All Rights Reserved
 *  SPDX-License-Identifier: Apache-2.0
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *  This file is part of mbed TLS (https://tls.mbed.org)
 */
#ifndef MBEDTLS_HMAC_H
#define MBEDTLS_HMAC_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

int mbedtls_hmac_starts( mbedtls_hash_context_t *ctx, const unsigned char *key, size_t keylen );

int mbedtls_hmac_update( mbedtls_hash_context_t *ctx, const unsigned char *input, size_t ilen );

int mbedtls_hmac_finish( mbedtls_hash_context_t *ctx, unsigned char *output );

int mbedtls_hash_hmac( const mbedtls_hash_info_t *md_info, const unsigned char *key, size_t keylen,
                const unsigned char *input, size_t ilen,
                unsigned char *output );

int mbedtls_hmac_reset( mbedtls_hash_context_t *ctx );

#ifdef __cplusplus
}
#endif

#endif /* MBEDTLS_HMAC_H */
