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
#define MBEDTLS_PLATFORM_C
#define MBEDTLS_PLATFORM_ALT

/* mbed TLS feature support */
#define MBEDTLS_CIPHER_MODE_CBC
#define MBEDTLS_SSL_PROTO_TLS1_2
#define MBEDTLS_THREADING_C

/* mbed TLS modules */
#define MBEDTLS_AES_C
#define MBEDTLS_CIPHER_C
#define MBEDTLS_NET_C
#define MBEDTLS_MD_C
#define MBEDTLS_SHA256_C
#define MBEDTLS_SSL_CLI_C
#define MBEDTLS_SSL_TLS_C
#define MBEDTLS_SSL_MAX_FRAGMENT_LENGTH

/* mbed DTLS modules */
#define MBEDTLS_TIMING_C
#define MBEDTLS_SSL_COOKIE_C
#define MBEDTLS_SSL_PROTO_DTLS
#define MBEDTLS_SSL_DTLS_ANTI_REPLAY

/* iTLS modules */
#define MBEDTLS_SSL_PROTO_ITLS
#define MBEDTLS_KEY_EXCHANGE_ID2_ENABLED

/* iTLS debug */
#if defined(CONFIG_SSL_DEBUG)
#define MBEDTLS_DEBUG_C
#endif

#define MBEDTLS_MD_ALT
#define MBEDTLS_AES_ALT

#if defined(MBEDTLS_MD_ALT)
#if defined(MBEDTLS_SHA1_C)
#define MBEDTLS_SHA1_ALT
#endif
#if defined(MBEDTLS_SHA256_C)
#define MBEDTLS_SHA256_ALT
#endif
#endif /* MBEDTLS_MD_ALT */

#if defined(CONFIG_PLAT_AOS)
#define MBEDTLS_THREADING_ALT
#define MBEDTLS_NET_ALT
#else
#define MBEDTLS_THREADING_PTHREAD
#endif

#define MBEDTLS_SSL_MAX_CONTENT_LEN    (2 * 1024) /* Size of the input / output buffer */

#include "check_config.h"

#endif /* MBEDTLS_CONFIG_H */
