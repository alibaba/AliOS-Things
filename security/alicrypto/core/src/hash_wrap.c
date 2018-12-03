/**
 * Copyright (C) 2017  Alibaba Group Holding Limited.
 **/
#include "config.h"
#include "osa.h"


#if defined(ALI_ALGO_MD_C)

#include "ali_crypto.h"
#include "md.h"
#include "hash.h"
#include "md_internal.h"

#if defined(ALI_ALGO_MD5_C)
#include "md5.h"
#endif

#if defined(ALI_ALGO_SHA1_C)
#include "sha1.h"
#endif

#if defined(ALI_ALGO_SHA256_C)
#include "sha256.h"
#endif


#if defined(ALI_ALGO_MD5_C)

static void md5_starts_wrap( void *ctx )
{
    ali_algo_md5_starts( (ali_algo_md5_context *) ctx );
}

static void md5_update_wrap( void *ctx, const unsigned char *input,
                             size_t ilen )
{
    ali_algo_md5_update( (ali_algo_md5_context *) ctx, input, ilen );
}

static void md5_finish_wrap( void *ctx, unsigned char *output )
{
    ali_algo_md5_finish( (ali_algo_md5_context *) ctx, output );
}

static void *md5_ctx_alloc( void )
{
    void *ctx = osa_calloc( 1, sizeof( ali_algo_md5_context ) );

    if ( ctx != NULL ) {
        ali_algo_md5_init( (ali_algo_md5_context *) ctx );
    }

    return ( ctx );
}

static void md5_ctx_free( void *ctx )
{
    ali_algo_md5_free( (ali_algo_md5_context *) ctx );
    osa_free( ctx );
}

static void md5_clone_wrap( void *dst, const void *src )
{
    ali_algo_md5_clone( (ali_algo_md5_context *) dst,
                       (const ali_algo_md5_context *) src );
}

static void md5_process_wrap( void *ctx, const unsigned char *data )
{
    ali_algo_md5_process( (ali_algo_md5_context *) ctx, data );
}

const ali_algo_md_info_t ali_algo_md5_info = {
    ALI_ALGO_MD_MD5,
    "MD5",
    16,
    64,
    md5_starts_wrap,
    md5_update_wrap,
    md5_finish_wrap,
    ali_algo_md5,
    md5_ctx_alloc,
    md5_ctx_free,
    md5_clone_wrap,
    md5_process_wrap,
};

#endif /* ALI_ALGO_MD5_C */


#if defined(ALI_ALGO_SHA1_C)

static void sha1_starts_wrap( void *ctx )
{
    ali_algo_sha1_starts( (ali_algo_sha1_context *) ctx );
}

static void sha1_update_wrap( void *ctx, const unsigned char *input,
                              size_t ilen )
{
    ali_algo_sha1_update( (ali_algo_sha1_context *) ctx, input, ilen );
}

static void sha1_finish_wrap( void *ctx, unsigned char *output )
{
    ali_algo_sha1_finish( (ali_algo_sha1_context *) ctx, output );
}

static void *sha1_ctx_alloc( void )
{
    void *ctx = osa_calloc( 1, sizeof( ali_algo_sha1_context ) );

    if ( ctx != NULL ) {
        ali_algo_sha1_init( (ali_algo_sha1_context *) ctx );
    }

    return ( ctx );
}

static void sha1_clone_wrap( void *dst, const void *src )
{
    ali_algo_sha1_clone( (ali_algo_sha1_context *) dst,
                        (const ali_algo_sha1_context *) src );
}

static void sha1_ctx_free( void *ctx )
{
    ali_algo_sha1_free( (ali_algo_sha1_context *) ctx );
    osa_free( ctx );
}

static void sha1_process_wrap( void *ctx, const unsigned char *data )
{
    ali_algo_sha1_process( (ali_algo_sha1_context *) ctx, data );
}


const ali_algo_md_info_t ali_algo_sha1_info = {
    ALI_ALGO_MD_SHA1,
    "SHA1",
    20,
    64,
    sha1_starts_wrap,
    sha1_update_wrap,
    sha1_finish_wrap,
    ali_algo_sha1,
    sha1_ctx_alloc,
    sha1_ctx_free,
    sha1_clone_wrap,
    sha1_process_wrap,
};

#endif /* ALI_ALGO_SHA1_C */

/*
 * Wrappers for generic message digests
 */
#if defined(ALI_ALGO_SHA256_C)

static void sha224_starts_wrap( void *ctx )
{
    ali_algo_sha256_starts( (ali_algo_sha256_context *) ctx, 1 );
}

static void sha224_update_wrap( void *ctx, const unsigned char *input,
                                size_t ilen )
{
    ali_algo_sha256_update( (ali_algo_sha256_context *) ctx, input, ilen );
}

static void sha224_finish_wrap( void *ctx, unsigned char *output )
{
    ali_algo_sha256_finish( (ali_algo_sha256_context *) ctx, output );
}

static void sha224_wrap( const unsigned char *input, size_t ilen,
                         unsigned char *output )
{
    ali_algo_sha256( input, ilen, output, 1 );
}

static void *sha224_ctx_alloc( void )
{
    void *ctx = osa_calloc( 1, sizeof( ali_algo_sha256_context ) );

    if ( ctx != NULL ) {
        ali_algo_sha256_init( (ali_algo_sha256_context *) ctx );
    }

    return ( ctx );
}

static void sha224_ctx_free( void *ctx )
{
    ali_algo_sha256_free( (ali_algo_sha256_context *) ctx );
    osa_free( ctx );
}

static void sha224_clone_wrap( void *dst, const void *src )
{
    ali_algo_sha256_clone( (ali_algo_sha256_context *) dst,
                          (const ali_algo_sha256_context *) src );
}

static void sha224_process_wrap( void *ctx, const unsigned char *data )
{
    ali_algo_sha256_process( (ali_algo_sha256_context *) ctx, data );
}



const ali_algo_md_info_t ali_algo_sha224_info = {
    ALI_ALGO_MD_SHA224,
    "SHA224",
    28,
    64,
    sha224_starts_wrap,
    sha224_update_wrap,
    sha224_finish_wrap,
    sha224_wrap,
    sha224_ctx_alloc,
    sha224_ctx_free,
    sha224_clone_wrap,
    sha224_process_wrap,
};

static void sha256_starts_wrap( void *ctx )
{
    ali_algo_sha256_starts( (ali_algo_sha256_context *) ctx, 0 );
}

static void sha256_wrap( const unsigned char *input, size_t ilen,
                         unsigned char *output )
{
    ali_algo_sha256( input, ilen, output, 0 );
}

const ali_algo_md_info_t ali_algo_sha256_info = {
    ALI_ALGO_MD_SHA256,
    "SHA256",
    32,
    64,
    sha256_starts_wrap,
    sha224_update_wrap,
    sha224_finish_wrap,
    sha256_wrap,
    sha224_ctx_alloc,
    sha224_ctx_free,
    sha224_clone_wrap,
    sha224_process_wrap,
};

#endif /* ALI_ALGO_SHA256_C */



/* Implementation that should never be optimized out by the compiler */
static void ali_algo_zeroize(void *v, size_t n)
{
    volatile unsigned char *p = v;
    while (n--) {
        *p++ = 0;
    }
}

const ali_algo_hash_info_t *ali_algo_hash_info_from_type(
  ali_algo_md_type_t md_type)
{
    switch (md_type) {
#if defined(ALI_ALGO_MD5_C)
        case ALI_ALGO_MD_MD5:
            return (&ali_algo_md5_info);
#endif
#if defined(ALI_ALGO_SHA1_C)
        case ALI_ALGO_MD_SHA1:
            return (&ali_algo_sha1_info);
#endif
#if defined(ALI_ALGO_SHA256_C)
        case ALI_ALGO_MD_SHA224:
            return (&ali_algo_sha224_info);
        case ALI_ALGO_MD_SHA256:
            return (&ali_algo_sha256_info);
#endif
        default:
            return (NULL);
    }
}

void ali_algo_hash_init(ali_algo_hash_context_t *ctx)
{
    memset(ctx, 0, sizeof(ali_algo_hash_context_t));
}

void ali_algo_hash_free(ali_algo_hash_context_t *ctx)
{
    if (ctx == NULL || ctx->md_info == NULL) {
        return;
    }

    if (ctx->md_ctx != NULL) {
        ctx->md_info->ctx_free_func(ctx->md_ctx);
        ctx->md_ctx = NULL;
    }

    if (ctx->hmac_ctx != NULL) {
        ali_algo_zeroize(ctx->hmac_ctx, 2 * ctx->md_info->block_size);
        osa_free(ctx->hmac_ctx);
        ctx->hmac_ctx = NULL;
    }

    ali_algo_zeroize(ctx, sizeof(ali_algo_hash_context_t));
}

#if !defined(ALI_ALGO_DEPRECATED_REMOVED)
int ali_algo_hash_init_ctx(ali_algo_hash_context_t * ctx,
                          const ali_algo_md_info_t *md_info)
{
    return ali_algo_hash_setup(ctx, md_info, 1);
}
#endif

int ali_algo_hash_setup(ali_algo_hash_context_t * ctx,
                       const ali_algo_md_info_t *md_info, int hmac)
{
    if (md_info == NULL || ctx == NULL) {
        return (ALI_ALGO_ERR_MD_BAD_INPUT_DATA);
    }

    if ((ctx->md_ctx = md_info->ctx_alloc_func()) == NULL) {
        return (ALI_ALGO_ERR_MD_ALLOC_FAILED);
    }

    if (hmac != 0) {
        ctx->hmac_ctx = osa_calloc(2, md_info->block_size);
        if (ctx->hmac_ctx == NULL) {
            md_info->ctx_free_func(ctx->md_ctx);
            ctx->md_ctx = NULL;
            return (ALI_ALGO_ERR_MD_ALLOC_FAILED);
        }
    }

    ctx->md_info = md_info;

    return (0);
}

int ali_algo_hash_starts(ali_algo_hash_context_t *ctx)
{
    if (ctx == NULL || ctx->md_info == NULL) {
        return (ALI_ALGO_ERR_MD_BAD_INPUT_DATA);
    }

    ctx->md_info->starts_func(ctx->md_ctx);

    return (0);
}

int ali_algo_hash_update(ali_algo_hash_context_t *ctx, const unsigned char *input,
                        size_t ilen)
{
    if (ctx == NULL || ctx->md_info == NULL) {
        return (ALI_ALGO_ERR_MD_BAD_INPUT_DATA);
    }

    ctx->md_info->update_func(ctx->md_ctx, input, ilen);

    return (0);
}

int ali_algo_hash_finish(ali_algo_hash_context_t *ctx, unsigned char *output)
{
    if (ctx == NULL || ctx->md_info == NULL) {
        return (ALI_ALGO_ERR_MD_BAD_INPUT_DATA);
    }

    ctx->md_info->finish_func(ctx->md_ctx, output);

    return (0);
}

int ali_algo_hash(const ali_algo_hash_info_t *md_info, const unsigned char *input,
                 size_t ilen, unsigned char *output)
{
    if (md_info == NULL) {
        return (ALI_ALGO_ERR_MD_BAD_INPUT_DATA);
    }

    md_info->digest_func(input, ilen, output);

    return (0);
}

unsigned char ali_algo_hash_get_size(const ali_algo_md_info_t *md_info)
{
    if (md_info == NULL) {
        return (0);
    }

    return md_info->size;
}

ali_algo_md_type_t ali_algo_hash_get_type(const ali_algo_md_info_t *md_info)
{
    if (md_info == NULL) {
        return (ALI_ALGO_MD_NONE);
    }

    return md_info->type;
}

#endif /* ALI_ALGO_MD_C */
