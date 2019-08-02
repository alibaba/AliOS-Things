/*
 * Copyright (C) 2016-2019 Alibaba Group Holding Limited
 */

#ifndef __SHA256_H__
#define __SHA256_H__

#include "ls_osa.h"

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
} impl_sha256_context;

typedef struct {
    impl_sha256_context context;
} impl_sha256_ctx_t;

/**
 * \brief          Initialize SHA-256 context
 *
 * \param ctx      SHA-256 context to be initialized
 */
void impl_sha256_init(impl_sha256_context *ctx);

/**
 * \brief          Clear SHA-256 context
 *
 * \param ctx      SHA-256 context to be cleared
 */
void impl_sha256_free(impl_sha256_context *ctx);

/**
 * \brief          Clone (the state of) a SHA-256 context
 *
 * \param dst      The destination context
 * \param src      The context to be cloned
 */
void impl_sha256_clone(impl_sha256_context *dst,
                     const impl_sha256_context *src);

/**
 * \brief          SHA-256 context setup
 *
 * \param ctx      context to be initialized
 * \param is224    0 = use SHA256, 1 = use SHA224
 */
void impl_sha256_starts(impl_sha256_context *ctx, int is224);

/**
 * \brief          SHA-256 process buffer
 *
 * \param ctx      SHA-256 context
 * \param input    buffer holding the  data
 * \param ilen     length of the input data
 */
void impl_sha256_update(impl_sha256_context *ctx, const unsigned char *input,
                      size_t ilen);

/**
 * \brief          SHA-256 final digest
 *
 * \param ctx      SHA-256 context
 * \param output   SHA-224/256 checksum result
 */
void impl_sha256_finish(impl_sha256_context *ctx, unsigned char output[32]);

/* Internal use */
void impl_sha256_process(impl_sha256_context *ctx, const unsigned char data[64]);

#ifdef __cplusplus
}
#endif

#endif /* __SHA256_H__ */
