/**
 * Copyright (C) 2018 The YunOS IoT Project. All rights reserved.
 */

#ifndef MBEDTLS_MD5_ALT_H
#define MBEDTLS_MD5_ALT_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    size_t size;
    void *ali_ctx;
} mbedtls_md5_context;

void mbedtls_md5_init_alt(mbedtls_md5_context *ctx);
void mbedtls_md5_free_alt(mbedtls_md5_context *ctx);

void mbedtls_md5_clone_alt(mbedtls_md5_context *dst,
                           const mbedtls_md5_context *src);

void mbedtls_md5_starts_alt(mbedtls_md5_context *ctx);
void mbedtls_md5_update_alt(mbedtls_md5_context *ctx, const unsigned char *input, size_t ilen);
void mbedtls_md5_finish_alt(mbedtls_md5_context *ctx, unsigned char output[16]);

void mbedtls_md5_alt( const unsigned char *input, size_t ilen, unsigned char output[16] );

#ifdef __cplusplus
}
#endif

#endif /* MBEDTLS_MD5_ALT_H */
