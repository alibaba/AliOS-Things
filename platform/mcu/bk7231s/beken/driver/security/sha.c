#include "rtos_pub.h"
#include "uart_pub.h"
#include "arm_arch.h"
#include "icu_pub.h"

#if (CFG_SOC_NAME == SOC_BK7221U)


#include "str_pub.h"
#include "mem_pub.h"
#include "drv_model_pub.h"

#include "security_pub.h"
#include "security.h"


void bk_sha_encrypt(hal_sha_context *ctx, const unsigned char *input)
{
    int err_num;

    SECURITY_SHA_DRV_DESC SHA_drv_desc;

    SHA_drv_desc.mode      = ctx->mode;
    SHA_drv_desc.step      = ctx->step;
    SHA_drv_desc.sha_block = (unsigned long *)input;
    if (security_sha_init(&SHA_drv_desc) != 0)
    {
        os_printf("sha failed \r\n");
        return;
    }

    while (is_secrity_sha_busy() == 0)
    {
    }

    if(ctx->step == STEP1)
    {
        ctx->step = STEP2;
    }

}

void bk_sha256_finish(hal_sha_context *ctx,
                      unsigned char output[32])
{
    get_security_sha_data(ctx, (unsigned long *)output);
}


void *hal_sha256_init(void)
{
    hal_sha_context *ctx;

    ctx = (hal_sha_context *)os_malloc(sizeof(hal_sha_context));
    if (ctx == NULL)
        return NULL;

    os_memset( ctx, 0, sizeof( hal_sha_context ) );
    ctx->mode = SHA256;
    ctx->total[0] = 0;
    ctx->total[1] = 0;
    ctx->step = STEP1;
}

void *hal_sha1_init(void)
{
    hal_sha_context *ctx;

    ctx = (hal_sha_context *)os_malloc(sizeof(hal_sha_context));
    if (ctx == NULL)
        return NULL;

    os_memset( ctx, 0, sizeof( hal_sha_context ) );
    ctx->mode = SHA1;
    ctx->total[0] = 0;
    ctx->total[1] = 0;
    ctx->step = STEP1;

    return (void *)ctx;
}

void hal_sha_update( void *ct, const unsigned char *input,
                     size_t ilen )
{
    size_t fill;
    UINT32 left;
    hal_sha_context *ctx = ct;

    if( ilen == 0 )
        return;

    left = ctx->total[0] & 0x3F;
    fill = 64 - left;

    ctx->total[0] += (UINT32) ilen;
    ctx->total[0] &= 0xFFFFFFFF;

    if( ctx->total[0] < (UINT32) ilen )
        ctx->total[1]++;

    if( left && ilen >= fill )
    {
        os_memcpy( (void *) (ctx->buffer + left), input, fill );
        bk_sha_encrypt( ctx, ctx->buffer );
        input += fill;
        ilen  -= fill;
        left = 0;
    }

    while( ilen >= 64 )
    {
        bk_sha_encrypt( ctx, input );
        input += 64;
        ilen  -= 64;
    }

    if( ilen > 0 )
        os_memcpy( (void *) (ctx->buffer + left), input, ilen );
}

static const unsigned char sha256_padding[64] =
{
    0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

/*
 * SHA-256 final digest
 */
void hal_sha_finish( void *ct, unsigned char output[32] )
{
    UINT32 last, padn;
    UINT32 high, low;
    unsigned char msglen[8];
    hal_sha_context *ctx = ct;

    high = ( ctx->total[0] >> 29 )
           | ( ctx->total[1] <<  3 );
    low  = ( ctx->total[0] <<  3 );

    PUT_UINT32_BE( high, msglen, 0 );
    PUT_UINT32_BE( low,  msglen, 4 );

    last = ctx->total[0] & 0x3F;
    padn = ( last < 56 ) ? ( 56 - last ) : ( 120 - last );

    hal_sha256_update( ctx, sha256_padding, padn );
    hal_sha256_update( ctx, msglen, 8 );

    bk_sha256_finish(ctx, output);

    os_memset(ctx, 0, sizeof(hal_sha_context));
    os_free(ctx);
    ctx = NULL;

}

static void hal_zeroize( void *v, size_t n )
{
    volatile unsigned char *p = v;
    while( n-- ) *p++ = 0;
}


void hal_sha_free( hal_sha_context *ctx )
{
    if( ctx == NULL )
        return;

    hal_zeroize( ctx, sizeof( hal_sha_context ) );
}

#if 1

static const unsigned char sha256_test_buf[3][57] =
{
    { "abc" },
    { "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq" },
    { "" }
};

static const int sha256_test_buflen[3] =
{
    3, 56, 1000
};

static const unsigned char sha256_test_sum[6][32] =
{
    /*
     * SHA-1 test vectors
     */
    {
        0xA9, 0x99, 0x3E, 0x36, 0x47, 0x06, 0x81, 0x6A, 0xBA, 0x3E,
        0x25, 0x71, 0x78, 0x50, 0xC2, 0x6C, 0x9C, 0xD0, 0xD8, 0x9D
    },
    {
        0x84, 0x98, 0x3E, 0x44, 0x1C, 0x3B, 0xD2, 0x6E, 0xBA, 0xAE,
        0x4A, 0xA1, 0xF9, 0x51, 0x29, 0xE5, 0xE5, 0x46, 0x70, 0xF1
    },
    {
        0x34, 0xAA, 0x97, 0x3C, 0xD4, 0xC4, 0xDA, 0xA4, 0xF6, 0x1E,
        0xEB, 0x2B, 0xDB, 0xAD, 0x27, 0x31, 0x65, 0x34, 0x01, 0x6F
    },

    /*
     * SHA-256 test vectors
     */
    {
        0xBA, 0x78, 0x16, 0xBF, 0x8F, 0x01, 0xCF, 0xEA,
        0x41, 0x41, 0x40, 0xDE, 0x5D, 0xAE, 0x22, 0x23,
        0xB0, 0x03, 0x61, 0xA3, 0x96, 0x17, 0x7A, 0x9C,
        0xB4, 0x10, 0xFF, 0x61, 0xF2, 0x00, 0x15, 0xAD
    },
    {
        0x24, 0x8D, 0x6A, 0x61, 0xD2, 0x06, 0x38, 0xB8,
        0xE5, 0xC0, 0x26, 0x93, 0x0C, 0x3E, 0x60, 0x39,
        0xA3, 0x3C, 0xE4, 0x59, 0x64, 0xFF, 0x21, 0x67,
        0xF6, 0xEC, 0xED, 0xD4, 0x19, 0xDB, 0x06, 0xC1
    },
    {
        0xCD, 0xC7, 0x6E, 0x5C, 0x99, 0x14, 0xFB, 0x92,
        0x81, 0xA1, 0xC7, 0xE2, 0x84, 0xD7, 0x3E, 0x67,
        0xF1, 0x80, 0x9A, 0x48, 0xA4, 0x97, 0x20, 0x0E,
        0x04, 0x6D, 0x39, 0xCC, 0xC7, 0x11, 0x2C, 0xD0
    }
};


int hal_sha256_self_test( int verbose )
{
    int i, j, k, buflen, ret = 0;
    unsigned char *buf;
    unsigned char sha256sum[32];
    void *ctx;

    buf = (unsigned char *)os_zalloc( 1024);
    if( NULL == buf )
    {
        if( verbose != 0 )
            os_printf( "Buffer allocation failed\n" );

        return( 1 );
    }


    for( i = 3; i < 6; i++ )
    {
        j = i % 3;
        k = i < 3;


        if( verbose != 0 )
            os_printf( "  SHA-%d test #%d: ", 256 - k * 32, j + 1 );

        ctx = hal_sha256_init();

        if( j == 2 )
        {
            os_memset( buf, 'a', buflen = 1000 );

            for( j = 0; j < 1000; j++ )
                hal_sha256_update( ctx, buf, buflen );
        }
        else
            hal_sha256_update( ctx, sha256_test_buf[j],
                               sha256_test_buflen[j] );

        hal_sha256_finish( ctx, sha256sum );

        if( os_memcmp( sha256sum, sha256_test_sum[i], 32 - k * 4 ) != 0 )
        {
            if( verbose != 0 )
                os_printf( "failed\n" );

            ret = 1;
            goto exit;
        }

        if( verbose != 0 )
            os_printf( "passed\n" );
    }

    if( verbose != 0 )
        os_printf( "\n" );

exit:
    os_free( buf );

    return( ret );
}

#endif

#endif

