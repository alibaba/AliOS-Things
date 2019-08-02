/*
 * Copyright (C) 2016-2019 Alibaba Group Holding Limited
 */

#ifndef __MD5_H__
#define __MD5_H__

#include "ls_osa.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief          MD5 context structure
 */
typedef struct {
    uint32_t total[2];          /*!< number of bytes processed  */
    uint32_t state[4];          /*!< intermediate digest state  */
    unsigned char buffer[64];   /*!< data block being processed */
} impl_md5_context;

typedef struct {
    impl_md5_context context;
} impl_md5_ctx_t;

/**
 * \brief          Initialize MD5 context
 *
 * \param ctx      MD5 context to be initialized
 */
void impl_md5_init(impl_md5_context *ctx);

/**
 * \brief          Clear MD5 context
 *
 * \param ctx      MD5 context to be cleared
 */
void impl_md5_free(impl_md5_context *ctx);

/**
 * \brief          Clone (the state of) an MD5 context
 *
 * \param dst      The destination context
 * \param src      The context to be cloned
 */
void impl_md5_clone(impl_md5_context *dst,
                  const impl_md5_context *src);

/**
 * \brief          MD5 context setup
 *
 * \param ctx      context to be initialized
 */
void impl_md5_starts(impl_md5_context *ctx);

/**
 * \brief          MD5 process buffer
 *
 * \param ctx      MD5 context
 * \param input    buffer holding the  data
 * \param ilen     length of the input data
 */
void impl_md5_update(impl_md5_context *ctx, const unsigned char *input, size_t ilen);

/**
 * \brief          MD5 final digest
 *
 * \param ctx      MD5 context
 * \param output   MD5 checksum result
 */
void impl_md5_finish(impl_md5_context *ctx, unsigned char output[16]);

/* Internal use */
void impl_md5_process(impl_md5_context *ctx, const unsigned char data[64]);

#ifdef __cplusplus
}
#endif

#endif /* __MD5_H__ */
