#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#if defined(MBEDTLS_MD_C)

#include "mbedtls/md.h"
#include "mbedtls/hash.h"
#include "mbedtls/md_internal.h"
#include "mbedtls/hmac.h"

#if defined(MBEDTLS_PLATFORM_C)
#include "mbedtls/platform.h"
#else
#include <stdlib.h>
#endif

#include <string.h>

#if defined(MBEDTLS_FS_IO)
#include <stdio.h>
#endif

/* Implementation that should never be optimized out by the compiler */
static void mbedtls_zeroize( void *v, size_t n ) {
    volatile unsigned char *p = v; while( n-- ) *p++ = 0;
}

int mbedtls_hmac_starts( mbedtls_hash_context_t *ctx, const unsigned char *key, size_t keylen )
{
    unsigned char sum[MBEDTLS_MD_MAX_SIZE];
    unsigned char *ipad, *opad;
    size_t i;

    if( ctx == NULL || ctx->md_info == NULL || ctx->hmac_ctx == NULL )
        return( MBEDTLS_ERR_MD_BAD_INPUT_DATA );

    if( keylen > (size_t) ctx->md_info->block_size )
    {
        ctx->md_info->starts_func( ctx->md_ctx );
        ctx->md_info->update_func( ctx->md_ctx, key, keylen );
        ctx->md_info->finish_func( ctx->md_ctx, sum );

        keylen = ctx->md_info->size;
        key = sum;
    }

    ipad = (unsigned char *) ctx->hmac_ctx;
    opad = (unsigned char *) ctx->hmac_ctx + ctx->md_info->block_size;

    memset( ipad, 0x36, ctx->md_info->block_size );
    memset( opad, 0x5C, ctx->md_info->block_size );

    for( i = 0; i < keylen; i++ )
    {
        ipad[i] = (unsigned char)( ipad[i] ^ key[i] );
        opad[i] = (unsigned char)( opad[i] ^ key[i] );
    }

    mbedtls_zeroize( sum, sizeof( sum ) );

    ctx->md_info->starts_func( ctx->md_ctx );
    ctx->md_info->update_func( ctx->md_ctx, ipad, ctx->md_info->block_size );

    return( 0 );
}

int mbedtls_hmac_update( mbedtls_hash_context_t *ctx, const unsigned char *input, size_t ilen )
{
    if( ctx == NULL || ctx->md_info == NULL || ctx->hmac_ctx == NULL )
        return( MBEDTLS_ERR_MD_BAD_INPUT_DATA );

    ctx->md_info->update_func( ctx->md_ctx, input, ilen );

    return( 0 );
}

int mbedtls_hmac_finish( mbedtls_hash_context_t *ctx, unsigned char *output )
{
    unsigned char tmp[MBEDTLS_MD_MAX_SIZE];
    unsigned char *opad;

    if( ctx == NULL || ctx->md_info == NULL || ctx->hmac_ctx == NULL )
        return( MBEDTLS_ERR_MD_BAD_INPUT_DATA );

    opad = (unsigned char *) ctx->hmac_ctx + ctx->md_info->block_size;

    ctx->md_info->finish_func( ctx->md_ctx, tmp );
    ctx->md_info->starts_func( ctx->md_ctx );
    ctx->md_info->update_func( ctx->md_ctx, opad, ctx->md_info->block_size );
    ctx->md_info->update_func( ctx->md_ctx, tmp, ctx->md_info->size );
    ctx->md_info->finish_func( ctx->md_ctx, output );

    return( 0 );
}

int mbedtls_hash_hmac( const mbedtls_hash_info_t *md_info, const unsigned char *key, size_t keylen,
                const unsigned char *input, size_t ilen,
                unsigned char *output )
{
    mbedtls_hash_context_t ctx;
    int ret;

    if( md_info == NULL )
        return( MBEDTLS_ERR_MD_BAD_INPUT_DATA );

    mbedtls_hash_init( &ctx );

    if( ( ret = mbedtls_hash_setup( &ctx, md_info, 1 ) ) != 0 )
        return( ret );

    mbedtls_hmac_starts( &ctx, key, keylen );
    mbedtls_hmac_update( &ctx, input, ilen );
    mbedtls_hmac_finish( &ctx, output );

    mbedtls_hash_free( &ctx );

    return( 0 );
}

int mbedtls_hmac_reset( mbedtls_hash_context_t *ctx )
{
    unsigned char *ipad;

    if( ctx == NULL || ctx->md_info == NULL || ctx->hmac_ctx == NULL )
        return( MBEDTLS_ERR_MD_BAD_INPUT_DATA );

    ipad = (unsigned char *) ctx->hmac_ctx;

    ctx->md_info->starts_func( ctx->md_ctx );
    ctx->md_info->update_func( ctx->md_ctx, ipad, ctx->md_info->block_size );

    return( 0 );
}

#endif /* defined(MBEDTLS_MD_C) */

