#include "ota_hash.h"
#include "ota_rsa.h"

static void _md5_starts_wrap(void *ctx )
{
    ota_hash_init( MD5, ctx );
}

static void _sha256_starts_wrap( void *ctx )
{
    ota_hash_init( SHA256, ctx );
}

static void _hash_update_wrap( void *ctx, const unsigned char *input,
                             size_t ilen )
{
    ota_hash_update( input, ilen, ctx );
}

static void _hash_finish_wrap( void *ctx, unsigned char *output )
{
    ota_hash_final( output, ctx );
}

void _ota_md5( const unsigned char *input, size_t ilen, unsigned char *output )
{
    ota_hash_digest(MD5, input, ilen, output);
}


void _ota_sha256( const unsigned char *input, size_t ilen, unsigned char *output )
{
    ota_hash_digest(SHA256, input, ilen, output);
}

static void *_md5_ctx_alloc( void )
{
    unsigned int len;
    unsigned int ret;
    void *ctx;

    ret = ota_hash_get_ctx_size(MD5, &len);
    if (0 != ret) {
        return NULL;
    }
    ctx = ota_calloc( 1, len );
    return( ctx );
}

static void _md5_ctx_free( void *ctx )
{
    ota_free( ctx );
}


static void *_sha256_ctx_alloc( void )
{
    unsigned int len;
    int32_t ret;
    void *ctx;

    ret = ota_hash_get_ctx_size(SHA256, &len);
    if (0 != ret) {
        return NULL;
    }
    ctx = ota_calloc( 1, len );
    return( ctx );
}

static void _sha256_ctx_free( void *ctx )
{
    ota_free( ctx );
}

static const struct ota_md_info_t _ota_md5_info = {
    OTA_MD_MD5,
    "MD5",
    16,
    64,
    _md5_starts_wrap,
    _hash_update_wrap,
    _hash_finish_wrap,
    _ota_md5,
    _md5_ctx_alloc,
    _md5_ctx_free,
    NULL,
    NULL,
};

static const struct ota_md_info_t _ota_sha256_info = {
    OTA_MD_SHA256,
    "SHA256",
    32,
    64,
    _sha256_starts_wrap,
    _hash_update_wrap,
    _hash_finish_wrap,
    _ota_sha256,
    _sha256_ctx_alloc,
    _sha256_ctx_free,
    NULL,
    NULL,
};

const ota_hash_info_t *ota_hash_info_from_type(ota_md_type_t md_type )
{
    switch( md_type )
    {
        case OTA_MD_MD5:
            return( &_ota_md5_info );
        case OTA_MD_SHA256:
            return( &_ota_sha256_info );
        default:
            return( NULL );
    }
}

unsigned char ota_hash_get_size( const struct ota_md_info_t *md_info )
{
    if( md_info == NULL )
        return( 0 );
    return md_info->size;
}
