/**
* Copyright (C) 2017  Alibaba Group Holding Limited.
**/

#ifndef ALI_ALGO_MD5_H
#define ALI_ALGO_MD5_H

#include "../../config/config.h"
#include <stddef.h>
#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief          MD5 context structure
 */
typedef struct
{
    uint32_t total[2];          /*!< number of bytes processed  */
    uint32_t state[4];          /*!< intermediate digest state  */
    unsigned char buffer[64];   /*!< data block being processed */
}
ali_algo_md5_context;

/**
 * \brief          Initialize MD5 context
 *
 * \param ctx      MD5 context to be initialized
 */
void ali_algo_md5_init( ali_algo_md5_context *ctx );

/**
 * \brief          Clear MD5 context
 *
 * \param ctx      MD5 context to be cleared
 */
void ali_algo_md5_free( ali_algo_md5_context *ctx );

/**
 * \brief          Clone (the state of) an MD5 context
 *
 * \param dst      The destination context
 * \param src      The context to be cloned
 */
void ali_algo_md5_clone( ali_algo_md5_context *dst,
                        const ali_algo_md5_context *src );

/**
 * \brief          MD5 context setup
 *
 * \param ctx      context to be initialized
 */
void ali_algo_md5_starts( ali_algo_md5_context *ctx );

/**
 * \brief          MD5 process buffer
 *
 * \param ctx      MD5 context
 * \param input    buffer holding the  data
 * \param ilen     length of the input data
 */
void ali_algo_md5_update( ali_algo_md5_context *ctx, const unsigned char *input, size_t ilen );

/**
 * \brief          MD5 final digest
 *
 * \param ctx      MD5 context
 * \param output   MD5 checksum result
 */
void ali_algo_md5_finish( ali_algo_md5_context *ctx, unsigned char output[16] );

/* Internal use */
void ali_algo_md5_process( ali_algo_md5_context *ctx, const unsigned char data[64] );

#ifdef __cplusplus
}
#endif


#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief          Output = MD5( input buffer )
 *
 * \param input    buffer holding the  data
 * \param ilen     length of the input data
 * \param output   MD5 checksum result
 */
void ali_algo_md5( const unsigned char *input, size_t ilen, unsigned char output[16] );

/**
 * \brief          Checkup routine
 *
 * \return         0 if successful, or 1 if the test failed
 */
int ali_algo_md5_self_test( int verbose );

#ifdef __cplusplus
}
#endif

#endif /* ali_algo_md5.h */
