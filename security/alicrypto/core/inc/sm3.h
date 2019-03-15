/**
* Copyright (C) 2018  Alibaba Group Holding Limited.
**/

#ifndef ALI_ALGO_SM3_H
#define ALI_ALGO_SM3_H

#if !defined(ALI_ALGO_CONFIG_FILE)
#include "config.h"
#else
#include ALI_ALGO_CONFIG_FILE
#endif

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief          SM3 context structure
 */
typedef struct {
    uint32_t total[2];          /*!< number of bytes processed  */
    uint32_t state[8];          /*!< intermediate digest state  */
    unsigned char buffer[64];   /*!< data block being processed */
} ali_algo_sm3_context;

/**
 * \brief          Initialize SM3 context
 *
 * \param ctx      SM3 context to be initialized
 */
void ali_algo_sm3_init( ali_algo_sm3_context *ctx );
/**
 * \brief          Clear SM3 context
 *
 * \param ctx      SM3 context to be cleared
 */
void ali_algo_sm3_free( ali_algo_sm3_context *ctx );

/**
 * \brief          Clone (the state of) a SHA-256 context
 *
 * \param dst      The destination context
 * \param src      The context to be cloned
 */
void ali_algo_sm3_clone( ali_algo_sm3_context *dst,
                           const ali_algo_sm3_context *src );

/**
 * \brief          Sm3 context setup
 *
 * \param ctx      context to be initialized
 */
void ali_algo_sm3_starts( ali_algo_sm3_context *ctx );

/**
 * \brief          SM3 process buffer
 *
 * \param ctx      SM3 context
 * \param input    buffer holding the  data
 * \param ilen     length of the input data
 */
void ali_algo_sm3_update( ali_algo_sm3_context *ctx, const unsigned char *input,
                    size_t ilen );

/**
 * \brief          SM3 final digest
 *
 * \param ctx      SM3 context
 * \param output   SM3 checksum result
 */
void ali_algo_sm3_finish( ali_algo_sm3_context *ctx, unsigned char output[32] );

/* Internal use */
void ali_algo_sm3_process( ali_algo_sm3_context *ctx, const unsigned char data[64] );

/**
 * \brief          Output = SM3( input buffer )
 *
 * \param input    buffer holding the  data
 * \param ilen     length of the input data
 * \param output   SM3 checksum result
 */
void ali_algo_sm3( const unsigned char *input, size_t ilen,
           unsigned char output[32]  );

#ifdef __cplusplus
}
#endif

#endif /* ali_algo_sm3.h */
