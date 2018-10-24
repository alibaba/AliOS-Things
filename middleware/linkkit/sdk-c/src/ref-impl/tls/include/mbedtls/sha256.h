/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */



#ifndef MBEDTLS_SHA256_H
#define MBEDTLS_SHA256_H

#if !defined(MBEDTLS_CONFIG_FILE)
    #include "config.h"
#else
    #include MBEDTLS_CONFIG_FILE
#endif

#include <stddef.h>
#include <stdint.h>

#if !defined(MBEDTLS_SHA256_ALT)
// Regular implementation
//

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief          SHA-256 context structure
 */
typedef struct {
    uint32_t total[2];          /*!< number of bytes processed  */
    uint32_t state[8];          /*!< intermediate digest state  */
    unsigned char buffer[64];   /*!< data block being processed */
    int is224;                  /*!< 0 => SHA-256, else SHA-224 */
}
mbedtls_sha256_context;

/**
 * \brief          Initialize SHA-256 context
 *
 * \param ctx      SHA-256 context to be initialized
 */
DLL_TLS_API void mbedtls_sha256_init(mbedtls_sha256_context *ctx);

/**
 * \brief          Clear SHA-256 context
 *
 * \param ctx      SHA-256 context to be cleared
 */
DLL_TLS_API void mbedtls_sha256_free(mbedtls_sha256_context *ctx);

/**
 * \brief          Clone (the state of) a SHA-256 context
 *
 * \param dst      The destination context
 * \param src      The context to be cloned
 */
DLL_TLS_API void mbedtls_sha256_clone(mbedtls_sha256_context *dst,
                                      const mbedtls_sha256_context *src);

/**
 * \brief          SHA-256 context setup
 *
 * \param ctx      context to be initialized
 * \param is224    0 = use SHA256, 1 = use SHA224
 */
DLL_TLS_API void mbedtls_sha256_starts(mbedtls_sha256_context *ctx, int is224);

/**
 * \brief          SHA-256 process buffer
 *
 * \param ctx      SHA-256 context
 * \param input    buffer holding the  data
 * \param ilen     length of the input data
 */
DLL_TLS_API void mbedtls_sha256_update(mbedtls_sha256_context *ctx, const unsigned char *input,
                                       size_t ilen);

/**
 * \brief          SHA-256 final digest
 *
 * \param ctx      SHA-256 context
 * \param output   SHA-224/256 checksum result
 */
DLL_TLS_API void mbedtls_sha256_finish(mbedtls_sha256_context *ctx, unsigned char output[32]);

/* Internal use */
DLL_TLS_API void mbedtls_sha256_process(mbedtls_sha256_context *ctx, const unsigned char data[64]);

#ifdef __cplusplus
}
#endif

#else  /* MBEDTLS_SHA256_ALT */
#include "sha256_alt.h"
#endif /* MBEDTLS_SHA256_ALT */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief          Output = SHA-256( input buffer )
 *
 * \param input    buffer holding the  data
 * \param ilen     length of the input data
 * \param output   SHA-224/256 checksum result
 * \param is224    0 = use SHA256, 1 = use SHA224
 */
DLL_TLS_API void mbedtls_sha256(const unsigned char *input, size_t ilen,
                                unsigned char output[32], int is224);

/**
 * \brief          Checkup routine
 *
 * \return         0 if successful, or 1 if the test failed
 */
DLL_TLS_API int mbedtls_sha256_self_test(int verbose);

#ifdef __cplusplus
}
#endif

#endif /* mbedtls_sha256.h */
