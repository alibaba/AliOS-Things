/**
* Copyright (C) 2017  Alibaba Group Holding Limited.
**/

#ifndef ALI_ALGO_MD_WRAP_H
#define ALI_ALGO_MD_WRAP_H

#include "../../config/config.h"
#include "md.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Message digest information.
 * Allows message digest functions to be called in a generic way.
 */
struct ali_algo_md_info_t
{
    /** Digest identifier */
    ali_algo_md_type_t type;

    /** Name of the message digest */
    const char * name;

    /** Output length of the digest function in bytes */
    int size;

    /** Block length of the digest function in bytes */
    int block_size;

    /** Digest initialisation function */
    void (*starts_func)( void *ctx );

    /** Digest update function */
    void (*update_func)( void *ctx, const unsigned char *input, size_t ilen );

    /** Digest finalisation function */
    void (*finish_func)( void *ctx, unsigned char *output );

    /** Generic digest function */
    void (*digest_func)( const unsigned char *input, size_t ilen,
                         unsigned char *output );

    /** Allocate a new context */
    void * (*ctx_alloc_func)( void );

    /** Free the given context */
    void (*ctx_free_func)( void *ctx );

    /** Clone state from a context */
    void (*clone_func)( void *dst, const void *src );

    /** Internal use only */
    void (*process_func)( void *ctx, const unsigned char *input );
};


#if defined(ALI_ALGO_MD5_C)
extern const ali_algo_md_info_t ali_algo_md5_info;
#endif
#if defined(ALI_ALGO_SHA1_C)
extern const ali_algo_md_info_t ali_algo_sha1_info;
#endif
#if defined(ALI_ALGO_SHA256_C)
extern const ali_algo_md_info_t ali_algo_sha224_info;
extern const ali_algo_md_info_t ali_algo_sha256_info;
#endif


#ifdef __cplusplus
}
#endif

#endif /* ALI_ALGO_MD_WRAP_H */
