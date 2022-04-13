/*
 *  Minimal configuration for TLS 1.2 with PSK and AES-CCM ciphersuites
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
/*
 * Minimal configuration for TLS 1.2 with PSK and AES-CCM ciphersuites
 * Distinguishing features:
 * - no bignum, no PK, no X509
 * - fully modern and secure (provided the pre-shared keys have high entropy)
 * - very low record overhead with CCM-8
 * - optimized for low RAM usage
 *
 * See README.txt for usage instructions.
 */
#ifndef MBEDTLS_CONFIG_H
#define MBEDTLS_CONFIG_H

/* System support */
//#define MBEDTLS_HAVE_TIME /* Optionally used in Hello messages */
/* Other MBEDTLS_HAVE_XXX flags irrelevant for this configuration */
//#define MBEDTLS_NO_PLATFORM_ENTROPY
//#define MBEDTLS_NO_DEFAULT_ENTROPY_SOURCES
//#define MBEDTLS_TEST_NULL_ENTROPY
//#define MBEDTLS_HAVEGE_C
//#define MBEDTLS_DEBUG_C
#define MBEDTLS_PLATFORM_C
#define MBEDTLS_PLATFORM_MEMORY
// #define MBEDTLS_TIMING_C

/* mbed TLS feature support */
#define MBEDTLS_CIPHER_MODE_CBC
//#define MBEDTLS_CIPHER_MODE_CTR
#define MBEDTLS_CIPHER_MODE_CFB
#define MBEDTLS_PKCS1_V15
#define MBEDTLS_KEY_EXCHANGE_RSA_ENABLED
//#define MBEDTLS_KEY_EXCHANGE_PSK_ENABLED
#define MBEDTLS_SSL_PROTO_TLS1_2
//#define MBEDTLS_SSL_PROTO_DTLS        /*alicoap dtls need, psk can close */
//#define MBEDTLS_SSL_DTLS_HELLO_VERIFY   /*alicoap dtls need, psk can close */

/* AES */
// #define MBEDTLS_AES_ALT

/* SHA256 */
// #define MBEDTLS_SHA256_ALT

/* RSA */
// #define MBEDTLS_RSA_ALT

#ifdef MBEDTLS_RSA_ALT
#define MBEDTLS_RSA_NO_CRT
#endif

/* mbed TLS modules */
#define MBEDTLS_AES_C
#define MBEDTLS_ASN1_PARSE_C
#define MBEDTLS_BIGNUM_C
// #define MBEDTLS_CCM_C
// #define MBEDTLS_GCM_C
#define MBEDTLS_CIPHER_C
#define MBEDTLS_CIPHER_MODE_CTR
#define MBEDTLS_CTR_DRBG_C
#define MBEDTLS_ENTROPY_C

#define MBEDTLS_MD_C
#define MBEDTLS_MD5_C
//#define MBEDTLS_MD5_WRAP

//#define MBEDTLS_NET_C
#define MBEDTLS_NET_C_ALT
#define MBEDTLS_OID_C
#define MBEDTLS_SHA1_C
#define MBEDTLS_SHA256_C
//#define MBEDTLS_SHA512_C
#define MBEDTLS_PK_C
#define MBEDTLS_PK_PARSE_C
#define MBEDTLS_RSA_C
#define MBEDTLS_SSL_CLI_C
//#define MBEDTLS_SSL_COOKIE_C /*alicoap dtls need, psk can close */
//#define MBEDTLS_SSL_SRV_C


#define MBEDTLS_SSL_TLS_C
#define MBEDTLS_X509_CRT_PARSE_C
#define MBEDTLS_X509_USE_C
#define MBEDTLS_BASE64_C
#define MBEDTLS_CERTS_C
#define MBEDTLS_PEM_PARSE_C

//#define MBEDTLS_ENTROPY_SHA512_ACCUMULATOR

#define MBEDTLS_SSL_SERVER_NAME_INDICATION

/* Save RAM at the expense of ROM */
// #define MBEDTLS_AES_ROM_TABLES

/* Save some RAM by adjusting to your exact needs */
#define MBEDTLS_PSK_MAX_LEN    16 /* 128-bits keys are generally enough */

/*
 * You should adjust this to the exact number of sources you're using: default
 * is the "platform_entropy_poll" source, but you may want to add other ones
 * Minimum is 2 for the entropy test suite.
 */
//#define MBEDTLS_ENTROPY_MAX_SOURCES 128

/*
 * Use only CCM_8 ciphersuites, and
 * save ROM and a few bytes of RAM by specifying our own ciphersuite list
 */
#define MBEDTLS_SSL_CIPHERSUITES  \
        MBEDTLS_TLS_RSA_WITH_AES_128_CBC_SHA

/*
 * Save RAM at the expense of interoperability: do this only if you control
 * both ends of the connection!  (See comments in "mbedtls/ssl.h".)
 * The optimal size here depends on the typical size of records.
 */
//#define MBEDTLS_SSL_MAX_CONTENT_LEN             (1 * 1024)   /*alicoap 1K*/

#if !defined (MBEDTLS_DEBUG_C)
/*reduce readonly date size*/
#define CK_REMOVE_UNUSED_FUNCTION_AND_DATA
#endif

/* save cert in flash, save about 3.7K ram use*/
//#define CK_SAVE_MEM_L2

#ifdef CK_SAVE_MEM_L2

#error "comment this line and set CK_FLASH_BLOCK_START,CK_FLAHS_BLOCK_NUM"

/* CK_FLASH_BLOCK_START flash start address where certificates in*/
/* CK_FLAHS_BLOCK_NUM   flash size, this MUSH larger than all certificates total size*/

/*examples for hobbit, 0x10030000 ~ 0x10031400 */
//#define CK_FLASH_BLOCK_START (384)
//#define CK_FLAHS_BLOCK_NUM   10

/*examples for phobos, 0x1002fc00 ~ 0x1003100*/
//#define CK_FLASH_BLOCK_START (382)
//#define CK_FLAHS_BLOCK_NUM   10

#define ck_alloc(a)      ck_cert_flash_alloc(a)
#define ck_memcpy(d,s,l) ck_cert_flash_copy((uint32_t)d,s,l)

#endif

/*disable rsa private key check,save about 5k rom use*/
//#define CK_SAVE_MEM_L3

/* reduce ram use in certificate handshake message*/
#ifdef MBEDTLS_SSL_PROTO_DTLS
#define CK_SAVE_MEM_L4   /*alicoap can open*/
#endif

#include "mbedtls/check_config.h"

#endif /* MBEDTLS_CONFIG_H */
