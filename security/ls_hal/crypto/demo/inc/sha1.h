/*
 * Copyright (C) 2016-2019 Alibaba Group Holding Limited
 */

#ifndef __SHA1_H__
#define __SHA1_H__

#include "ls_osa.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief          SHA-1 context structure
 */
typedef struct {
    uint32_t total[2];          /*!< number of bytes processed  */
    uint32_t state[5];          /*!< intermediate digest state  */
    unsigned char buffer[64];   /*!< data block being processed */
} hal_sha1_context;

/**
 * \brief          Initialize SHA-1 context
 *
 * \param ctx      SHA-1 context to be initialized
 */
void hal_sha1_init(hal_sha1_context *ctx);

/**
 * \brief          Clear SHA-1 context
 *
 * \param ctx      SHA-1 context to be cleared
 */
void hal_sha1_free(hal_sha1_context *ctx);

/**
 * \brief          Clone (the state of) a SHA-1 context
 *
 * \param dst      The destination context
 * \param src      The context to be cloned
 */
void hal_sha1_clone(hal_sha1_context *dst,
                   const hal_sha1_context *src);

/**
 * \brief          SHA-1 context setup
 *
 * \param ctx      context to be initialized
 */
void hal_sha1_starts(hal_sha1_context *ctx);

/**
 * \brief          SHA-1 process buffer
 *
 * \param ctx      SHA-1 context
 * \param input    buffer holding the  data
 * \param ilen     length of the input data
 */
void hal_sha1_update(hal_sha1_context *ctx, const unsigned char *input, size_t ilen);

/**
 * \brief          SHA-1 final digest
 *
 * \param ctx      SHA-1 context
 * \param output   SHA-1 checksum result
 */
void hal_sha1_finish(hal_sha1_context *ctx, unsigned char output[20]);

/* Internal use */
void hal_sha1_process(hal_sha1_context *ctx, const unsigned char data[64]);

#ifdef __cplusplus
}
#endif

#endif /* __SHA1_H__ */
