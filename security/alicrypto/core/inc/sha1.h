/**
* Copyright (C) 2017  Alibaba Group Holding Limited.
**/

#ifndef ALI_ALGO_SHA1_H
#define ALI_ALGO_SHA1_H

#include "../../config/config.h"
#include <stddef.h>
#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief          SHA-1 context structure
 */
typedef struct
{
    uint32_t total[2];          /*!< number of bytes processed  */
    uint32_t state[5];          /*!< intermediate digest state  */
    unsigned char buffer[64];   /*!< data block being processed */
}
ali_algo_sha1_context;

/**
 * \brief          Initialize SHA-1 context
 *
 * \param ctx      SHA-1 context to be initialized
 */
void ali_algo_sha1_init( ali_algo_sha1_context *ctx );

/**
 * \brief          Clear SHA-1 context
 *
 * \param ctx      SHA-1 context to be cleared
 */
void ali_algo_sha1_free( ali_algo_sha1_context *ctx );

/**
 * \brief          Clone (the state of) a SHA-1 context
 *
 * \param dst      The destination context
 * \param src      The context to be cloned
 */
void ali_algo_sha1_clone( ali_algo_sha1_context *dst,
                         const ali_algo_sha1_context *src );

/**
 * \brief          SHA-1 context setup
 *
 * \param ctx      context to be initialized
 */
void ali_algo_sha1_starts( ali_algo_sha1_context *ctx );

/**
 * \brief          SHA-1 process buffer
 *
 * \param ctx      SHA-1 context
 * \param input    buffer holding the  data
 * \param ilen     length of the input data
 */
void ali_algo_sha1_update( ali_algo_sha1_context *ctx, const unsigned char *input, size_t ilen );

/**
 * \brief          SHA-1 final digest
 *
 * \param ctx      SHA-1 context
 * \param output   SHA-1 checksum result
 */
void ali_algo_sha1_finish( ali_algo_sha1_context *ctx, unsigned char output[20] );

/* Internal use */
void ali_algo_sha1_process( ali_algo_sha1_context *ctx, const unsigned char data[64] );

#ifdef __cplusplus
}
#endif


#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief          Output = SHA-1( input buffer )
 *
 * \param input    buffer holding the  data
 * \param ilen     length of the input data
 * \param output   SHA-1 checksum result
 */
void ali_algo_sha1( const unsigned char *input, size_t ilen, unsigned char output[20] );

/**
 * \brief          Checkup routine
 *
 * \return         0 if successful, or 1 if the test failed
 */
int ali_algo_sha1_self_test( int verbose );

#ifdef __cplusplus
}
#endif

#endif /* ali_algo_sha1.h */
