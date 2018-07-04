/**
 * Copyright (C) 2017 The YunOS IoT Project. All rights reserved.
 */

#ifndef MBEDTLS_AUTH_H
#define MBEDTLS_AUTH_H

#include <stdint.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Key Groups */
#define MBEDTLS_KEY_GROUP_ALIBABA_ID2    (0x10001000)

int mbedtls_write_auth_extra_ext(
        mbedtls_ssl_context *ssl, unsigned char *buf, size_t *olen);

int mbedtls_write_key_id_ext(
        mbedtls_ssl_context *ssl, unsigned char *buf, size_t *olen);

int mbedtls_write_auth_code_ext(
        mbedtls_ssl_context *ssl, unsigned char *buf, size_t *olen);

int mbedtls_parse_auth_code_ext(
        mbedtls_ssl_context *ssl, unsigned char *buf, size_t len);

int mbedtls_parse_pre_master_secret_ext(
        mbedtls_ssl_context *ssl, unsigned char *buf, size_t len);

#ifdef __cplusplus
}
#endif

#endif /* MBEDTLS_AUTH_H */
