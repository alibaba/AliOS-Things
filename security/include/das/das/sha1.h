/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef __DAS_SHA1_H__
#define __DAS_SHA1_H__

#include <das/platform.h>

#ifdef DAS_PLATFORM_SHA1_ALT
#include <das/sha1_alt.h>
#else
/**
 * \brief          SHA-1 context structure
 */
typedef struct _das_sha1_context {
    uint32_t total[2];          /*!< number of bytes processed  */
    uint32_t state[5];          /*!< intermediate digest state  */
    unsigned char buffer[64];   /*!< data block being processed */
} das_sha1_context_t;
#endif

#define DAS_SHA1_LENGTH   20

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief          Initialize SHA-1 context
 *
 * \param ctx      SHA-1 context to be initialized
 */
void das_sha1_start(das_sha1_context_t *context);

/**
 * \brief          SHA-1 process buffer
 *
 * \param ctx      SHA-1 context
 * \param input    buffer holding the  data
 * \param ilen     length of the input data
 */
void das_sha1_update(das_sha1_context_t *ctx, const unsigned char *input, size_t ilen);

/**
 * \brief          SHA-1 final digest
 *
 * \param ctx      SHA-1 context
 * \param output   SHA-1 checksum result
 */
void das_sha1_finish(das_sha1_context_t *ctx, unsigned char output[DAS_SHA1_LENGTH]);


#ifdef __cplusplus
}
#endif

#endif /* mbedtls_sha1.h */
