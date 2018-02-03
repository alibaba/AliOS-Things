/**
 * \file config.h
 *
 * \brief Configuration options (set of defines)
 *
 *  This set of compile-time options may be used to enable
 *  or disable features selectively, and reduce the global
 *  memory footprint.
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

#ifndef MBEDTLS_CONFIG_H
#define MBEDTLS_CONFIG_H

/* System support */
#define MBEDTLS_HAVE_ASM
#define MBEDTLS_HAVE_TIME
#define MBEDTLS_PLATFORM_C
#define MBEDTLS_PLATFORM_ALT

/* mbed TLS feature support */
#define MBEDTLS_CIPHER_MODE_CBC
#define MBEDTLS_PKCS1_V15
#define MBEDTLS_KEY_EXCHANGE_RSA_ENABLED
#define MBEDTLS_SSL_PROTO_TLS1_2
#define MBEDTLS_THREADING_C
//#define MBEDTLS_THREADING_PTHREAD
#define MBEDTLS_THREADING_ALT

/* mbed TLS modules */
#define MBEDTLS_AES_C
#define MBEDTLS_ASN1_PARSE_C
#define MBEDTLS_BIGNUM_C
#define MBEDTLS_CIPHER_C
#define MBEDTLS_MD_C
#define MBEDTLS_MD5_C
#define MBEDTLS_NET_C
#define MBEDTLS_PK_C
#define MBEDTLS_PK_PARSE_C
#define MBEDTLS_RSA_C
#define MBEDTLS_SHA1_C
#define MBEDTLS_SHA256_C
#define MBEDTLS_SSL_CLI_C
#define MBEDTLS_SSL_TLS_C
#define MBEDTLS_X509_CRT_PARSE_C
#define MBEDTLS_X509_USE_C
#define MBEDTLS_BASE64_C
#define MBEDTLS_PEM_PARSE_C

/* mbed DTLS modules */
#define MBEDTLS_TIMING_C
#define MBEDTLS_SSL_COOKIE_C
#define MBEDTLS_SSL_PROTO_DTLS
#define MBEDTLS_SSL_DTLS_ANTI_REPLAY

/* mbed TLS debug */
//#define MBEDTLS_DEBUG_C

/* OEM configure */
#define MBEDTLS_IOT_SPECIFIC
#define MBEDTLS_NET_ALT
#define MBEDTLS_PK_ALT
#define MBEDTLS_AES_ALT

#include "mbedtls/check_config.h"

#endif /* MBEDTLS_CONFIG_H */
