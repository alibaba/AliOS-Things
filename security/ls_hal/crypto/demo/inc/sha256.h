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
} hal_sha256_context;

/**
 * \brief          Initialize SHA-256 context
 *
 * \param ctx      SHA-256 context to be initialized
 */
void hal_sha256_init(hal_sha256_context *ctx);

/**
 * \brief          Clear SHA-256 context
 *
 * \param ctx      SHA-256 context to be cleared
 */
void hal_sha256_free(hal_sha256_context *ctx);

/**
 * \brief          Clone (the state of) a SHA-256 context
 *
 * \param dst      The destination context
 * \param src      The context to be cloned
 */
void hal_sha256_clone(hal_sha256_context *dst,
                     const hal_sha256_context *src);

/**
 * \brief          SHA-256 context setup
 *
 * \param ctx      context to be initialized
 * \param is224    0 = use SHA256, 1 = use SHA224
 */
void hal_sha256_starts(hal_sha256_context *ctx, int is224);

/**
 * \brief          SHA-256 process buffer
 *
 * \param ctx      SHA-256 context
 * \param input    buffer holding the  data
 * \param ilen     length of the input data
 */
void hal_sha256_update(hal_sha256_context *ctx, const unsigned char *input,
                      size_t ilen);

/**
 * \brief          SHA-256 final digest
 *
 * \param ctx      SHA-256 context
 * \param output   SHA-224/256 checksum result
 */
void hal_sha256_finish(hal_sha256_context *ctx, unsigned char output[32]);

/* Internal use */
void hal_sha256_process(hal_sha256_context *ctx, const unsigned char data[64]);

#ifdef __cplusplus
}
#endif

#endif /* __SHA256_H__ */
