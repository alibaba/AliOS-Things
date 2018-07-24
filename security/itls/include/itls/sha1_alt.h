/**
 * Copyright (C) 2018 The YunOS IoT Project. All rights reserved.
 */

#ifndef MBEDTLS_SHA1_ALT_H
#define MBEDTLS_SHA1_ALT_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct
    {
        size_t size;
        void * ali_ctx;
    } mbedtls_sha1_context;

    void mbedtls_sha1_init_alt(mbedtls_sha1_context *ctx);
    void mbedtls_sha1_free_alt(mbedtls_sha1_context *ctx);

    void mbedtls_sha1_clone_alt(mbedtls_sha1_context *      dst,
                                const mbedtls_sha1_context *src);

    void mbedtls_sha1_starts_alt(mbedtls_sha1_context *ctx);
    void mbedtls_sha1_update_alt(mbedtls_sha1_context *ctx,
                                 const unsigned char *input, size_t ilen);
    void mbedtls_sha1_finish_alt(mbedtls_sha1_context *ctx,
                                 unsigned char         output[20]);

    void mbedtls_sha1_alt(const unsigned char *input, size_t ilen,
                          unsigned char output[20]);

#ifdef __cplusplus
}
#endif

#endif /* MBEDTLS_SHA1_ALT_H */
