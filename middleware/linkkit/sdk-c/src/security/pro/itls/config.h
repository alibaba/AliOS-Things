/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
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
#define MBEDTLS_SHA1_C
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

/* mbed TLS debug */
#define MBEDTLS_DEBUG_C

/* OEM configure */
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

#include "itls/check_config.h"

#endif /* MBEDTLS_CONFIG_H */
