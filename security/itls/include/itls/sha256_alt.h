/**
 * Copyright (C) 2018 The YunOS IoT Project. All rights reserved.
 */

#ifndef MBEDTLS_SHA256_ALT_H
#define MBEDTLS_SHA256_ALT_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    size_t size;
    void *ali_ctx;
} mbedtls_sha256_context;

void mbedtls_sha256_init_alt(mbedtls_sha256_context *ctx);
void mbedtls_sha256_free_alt(mbedtls_sha256_context *ctx);

void mbedtls_sha256_clone_alt(mbedtls_sha256_context *dst,
                              const mbedtls_sha256_context *src);

void mbedtls_sha256_starts_alt(mbedtls_sha256_context *ctx, int is224);
void mbedtls_sha256_update_alt(mbedtls_sha256_context *ctx, const unsigned char *input, size_t ilen);
void mbedtls_sha256_finish_alt(mbedtls_sha256_context *ctx, unsigned char output[32]);

void mbedtls_sha256_alt(const unsigned char *input, size_t ilen, unsigned char output[32], int is224);

#ifdef __cplusplus
}
#endif

#endif /* MBEDTLS_SHA256_ALT_H */
