#include "polarssl/config_rom.h"
#include "platform_autoconf.h"

#ifdef CONFIG_SSL_ROM_TEST

#define polarssl_printf DiagPrintf

#define AES_POLARSSL_SELF_TEST
#define ARC4_POLARSSL_SELF_TEST
#define BASE64_POLARSSL_SELF_TEST
#define BIGNUM_POLARSSL_SELF_TEST
#define CTR_DRBG_POLARSSL_SELF_TEST
#define DES_POLARSSL_SELF_TEST
#define DHM_POLARSSL_SELF_TEST
//#define ECP_POLARSSL_SELF_TEST //need open part of ecp rom code self test
#define HMAC_DRBG_POLARSSL_SELF_TEST
#define MD5_POLARSSL_SELF_TEST
#define RSA_POLARSSL_SELF_TEST
#define SHA1_POLARSSL_SELF_TEST
#define SHA256_POLARSSL_SELF_TEST
#define SHA512_POLARSSL_SELF_TEST

#if defined(AES_POLARSSL_SELF_TEST)
#if defined(POLARSSL_AES_C)

#include "polarssl/aes.h"
#if defined(POLARSSL_PADLOCK_C)
#include "polarssl/padlock.h"
#endif
#if defined(POLARSSL_AESNI_C)
#include "polarssl/aesni.h"
#endif

#include <stdio.h>
#include "platform_autoconf.h"

/*
 * AES test vectors from:
 *
 * http://csrc.nist.gov/archive/aes/rijndael/rijndael-vals.zip
 */
static const unsigned char aes_test_ecb_dec[3][16] =
{
    { 0x44, 0x41, 0x6A, 0xC2, 0xD1, 0xF5, 0x3C, 0x58,
      0x33, 0x03, 0x91, 0x7E, 0x6B, 0xE9, 0xEB, 0xE0 },
    { 0x48, 0xE3, 0x1E, 0x9E, 0x25, 0x67, 0x18, 0xF2,
      0x92, 0x29, 0x31, 0x9C, 0x19, 0xF1, 0x5B, 0xA4 },
    { 0x05, 0x8C, 0xCF, 0xFD, 0xBB, 0xCB, 0x38, 0x2D,
      0x1F, 0x6F, 0x56, 0x58, 0x5D, 0x8A, 0x4A, 0xDE }
};

static const unsigned char aes_test_ecb_enc[3][16] =
{
    { 0xC3, 0x4C, 0x05, 0x2C, 0xC0, 0xDA, 0x8D, 0x73,
      0x45, 0x1A, 0xFE, 0x5F, 0x03, 0xBE, 0x29, 0x7F },
    { 0xF3, 0xF6, 0x75, 0x2A, 0xE8, 0xD7, 0x83, 0x11,
      0x38, 0xF0, 0x41, 0x56, 0x06, 0x31, 0xB1, 0x14 },
    { 0x8B, 0x79, 0xEE, 0xCC, 0x93, 0xA0, 0xEE, 0x5D,
      0xFF, 0x30, 0xB4, 0xEA, 0x21, 0x63, 0x6D, 0xA4 }
};

#if defined(POLARSSL_CIPHER_MODE_CBC)
static const unsigned char aes_test_cbc_dec[3][16] =
{
    { 0xFA, 0xCA, 0x37, 0xE0, 0xB0, 0xC8, 0x53, 0x73,
      0xDF, 0x70, 0x6E, 0x73, 0xF7, 0xC9, 0xAF, 0x86 },
    { 0x5D, 0xF6, 0x78, 0xDD, 0x17, 0xBA, 0x4E, 0x75,
      0xB6, 0x17, 0x68, 0xC6, 0xAD, 0xEF, 0x7C, 0x7B },
    { 0x48, 0x04, 0xE1, 0x81, 0x8F, 0xE6, 0x29, 0x75,
      0x19, 0xA3, 0xE8, 0x8C, 0x57, 0x31, 0x04, 0x13 }
};

static const unsigned char aes_test_cbc_enc[3][16] =
{
    { 0x8A, 0x05, 0xFC, 0x5E, 0x09, 0x5A, 0xF4, 0x84,
      0x8A, 0x08, 0xD3, 0x28, 0xD3, 0x68, 0x8E, 0x3D },
    { 0x7B, 0xD9, 0x66, 0xD5, 0x3A, 0xD8, 0xC1, 0xBB,
      0x85, 0xD2, 0xAD, 0xFA, 0xE8, 0x7B, 0xB1, 0x04 },
    { 0xFE, 0x3C, 0x53, 0x65, 0x3E, 0x2F, 0x45, 0xB5,
      0x6F, 0xCD, 0x88, 0xB2, 0xCC, 0x89, 0x8F, 0xF0 }
};
#endif /* POLARSSL_CIPHER_MODE_CBC */

#if defined(POLARSSL_CIPHER_MODE_CFB)
/*
 * AES-CFB128 test vectors from:
 *
 * http://csrc.nist.gov/publications/nistpubs/800-38a/sp800-38a.pdf
 */
static const unsigned char aes_test_cfb128_key[3][32] =
{
    { 0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6,
      0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C },
    { 0x8E, 0x73, 0xB0, 0xF7, 0xDA, 0x0E, 0x64, 0x52,
      0xC8, 0x10, 0xF3, 0x2B, 0x80, 0x90, 0x79, 0xE5,
      0x62, 0xF8, 0xEA, 0xD2, 0x52, 0x2C, 0x6B, 0x7B },
    { 0x60, 0x3D, 0xEB, 0x10, 0x15, 0xCA, 0x71, 0xBE,
      0x2B, 0x73, 0xAE, 0xF0, 0x85, 0x7D, 0x77, 0x81,
      0x1F, 0x35, 0x2C, 0x07, 0x3B, 0x61, 0x08, 0xD7,
      0x2D, 0x98, 0x10, 0xA3, 0x09, 0x14, 0xDF, 0xF4 }
};

static const unsigned char aes_test_cfb128_iv[16] =
{
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F
};

static const unsigned char aes_test_cfb128_pt[64] =
{
    0x6B, 0xC1, 0xBE, 0xE2, 0x2E, 0x40, 0x9F, 0x96,
    0xE9, 0x3D, 0x7E, 0x11, 0x73, 0x93, 0x17, 0x2A,
    0xAE, 0x2D, 0x8A, 0x57, 0x1E, 0x03, 0xAC, 0x9C,
    0x9E, 0xB7, 0x6F, 0xAC, 0x45, 0xAF, 0x8E, 0x51,
    0x30, 0xC8, 0x1C, 0x46, 0xA3, 0x5C, 0xE4, 0x11,
    0xE5, 0xFB, 0xC1, 0x19, 0x1A, 0x0A, 0x52, 0xEF,
    0xF6, 0x9F, 0x24, 0x45, 0xDF, 0x4F, 0x9B, 0x17,
    0xAD, 0x2B, 0x41, 0x7B, 0xE6, 0x6C, 0x37, 0x10
};

static const unsigned char aes_test_cfb128_ct[3][64] =
{
    { 0x3B, 0x3F, 0xD9, 0x2E, 0xB7, 0x2D, 0xAD, 0x20,
      0x33, 0x34, 0x49, 0xF8, 0xE8, 0x3C, 0xFB, 0x4A,
      0xC8, 0xA6, 0x45, 0x37, 0xA0, 0xB3, 0xA9, 0x3F,
      0xCD, 0xE3, 0xCD, 0xAD, 0x9F, 0x1C, 0xE5, 0x8B,
      0x26, 0x75, 0x1F, 0x67, 0xA3, 0xCB, 0xB1, 0x40,
      0xB1, 0x80, 0x8C, 0xF1, 0x87, 0xA4, 0xF4, 0xDF,
      0xC0, 0x4B, 0x05, 0x35, 0x7C, 0x5D, 0x1C, 0x0E,
      0xEA, 0xC4, 0xC6, 0x6F, 0x9F, 0xF7, 0xF2, 0xE6 },
    { 0xCD, 0xC8, 0x0D, 0x6F, 0xDD, 0xF1, 0x8C, 0xAB,
      0x34, 0xC2, 0x59, 0x09, 0xC9, 0x9A, 0x41, 0x74,
      0x67, 0xCE, 0x7F, 0x7F, 0x81, 0x17, 0x36, 0x21,
      0x96, 0x1A, 0x2B, 0x70, 0x17, 0x1D, 0x3D, 0x7A,
      0x2E, 0x1E, 0x8A, 0x1D, 0xD5, 0x9B, 0x88, 0xB1,
      0xC8, 0xE6, 0x0F, 0xED, 0x1E, 0xFA, 0xC4, 0xC9,
      0xC0, 0x5F, 0x9F, 0x9C, 0xA9, 0x83, 0x4F, 0xA0,
      0x42, 0xAE, 0x8F, 0xBA, 0x58, 0x4B, 0x09, 0xFF },
    { 0xDC, 0x7E, 0x84, 0xBF, 0xDA, 0x79, 0x16, 0x4B,
      0x7E, 0xCD, 0x84, 0x86, 0x98, 0x5D, 0x38, 0x60,
      0x39, 0xFF, 0xED, 0x14, 0x3B, 0x28, 0xB1, 0xC8,
      0x32, 0x11, 0x3C, 0x63, 0x31, 0xE5, 0x40, 0x7B,
      0xDF, 0x10, 0x13, 0x24, 0x15, 0xE5, 0x4B, 0x92,
      0xA1, 0x3E, 0xD0, 0xA8, 0x26, 0x7A, 0xE2, 0xF9,
      0x75, 0xA3, 0x85, 0x74, 0x1A, 0xB9, 0xCE, 0xF8,
      0x20, 0x31, 0x62, 0x3D, 0x55, 0xB1, 0xE4, 0x71 }
};
#endif /* POLARSSL_CIPHER_MODE_CFB */

#if defined(POLARSSL_CIPHER_MODE_CTR)
/*
 * AES-CTR test vectors from:
 *
 * http://www.faqs.org/rfcs/rfc3686.html
 */

static const unsigned char aes_test_ctr_key[3][16] =
{
    { 0xAE, 0x68, 0x52, 0xF8, 0x12, 0x10, 0x67, 0xCC,
      0x4B, 0xF7, 0xA5, 0x76, 0x55, 0x77, 0xF3, 0x9E },
    { 0x7E, 0x24, 0x06, 0x78, 0x17, 0xFA, 0xE0, 0xD7,
      0x43, 0xD6, 0xCE, 0x1F, 0x32, 0x53, 0x91, 0x63 },
    { 0x76, 0x91, 0xBE, 0x03, 0x5E, 0x50, 0x20, 0xA8,
      0xAC, 0x6E, 0x61, 0x85, 0x29, 0xF9, 0xA0, 0xDC }
};

static const unsigned char aes_test_ctr_nonce_counter[3][16] =
{
    { 0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01 },
    { 0x00, 0x6C, 0xB6, 0xDB, 0xC0, 0x54, 0x3B, 0x59,
      0xDA, 0x48, 0xD9, 0x0B, 0x00, 0x00, 0x00, 0x01 },
    { 0x00, 0xE0, 0x01, 0x7B, 0x27, 0x77, 0x7F, 0x3F,
      0x4A, 0x17, 0x86, 0xF0, 0x00, 0x00, 0x00, 0x01 }
};

static const unsigned char aes_test_ctr_pt[3][48] =
{
    { 0x53, 0x69, 0x6E, 0x67, 0x6C, 0x65, 0x20, 0x62,
      0x6C, 0x6F, 0x63, 0x6B, 0x20, 0x6D, 0x73, 0x67 },

    { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
      0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
      0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
      0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F },

    { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
      0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
      0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
      0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
      0x20, 0x21, 0x22, 0x23 }
};

static const unsigned char aes_test_ctr_ct[3][48] =
{
    { 0xE4, 0x09, 0x5D, 0x4F, 0xB7, 0xA7, 0xB3, 0x79,
      0x2D, 0x61, 0x75, 0xA3, 0x26, 0x13, 0x11, 0xB8 },
    { 0x51, 0x04, 0xA1, 0x06, 0x16, 0x8A, 0x72, 0xD9,
      0x79, 0x0D, 0x41, 0xEE, 0x8E, 0xDA, 0xD3, 0x88,
      0xEB, 0x2E, 0x1E, 0xFC, 0x46, 0xDA, 0x57, 0xC8,
      0xFC, 0xE6, 0x30, 0xDF, 0x91, 0x41, 0xBE, 0x28 },
    { 0xC1, 0xCF, 0x48, 0xA8, 0x9F, 0x2F, 0xFD, 0xD9,
      0xCF, 0x46, 0x52, 0xE9, 0xEF, 0xDB, 0x72, 0xD7,
      0x45, 0x40, 0xA4, 0x2B, 0xDE, 0x6D, 0x78, 0x36,
      0xD5, 0x9A, 0x5C, 0xEA, 0xAE, 0xF3, 0x10, 0x53,
      0x25, 0xB2, 0x07, 0x2F }
};

static const int aes_test_ctr_len[3] =
    { 16, 32, 36 };
#endif /* POLARSSL_CIPHER_MODE_CTR */

/*
 * Checkup routine
 */
int aes_self_test( int verbose )
{
    int ret = 0, i, j, u, v;
    unsigned char key[32];
    unsigned char buf[64];
    unsigned char iv[16];
#if defined(POLARSSL_CIPHER_MODE_CBC)
    unsigned char prv[16];
#endif
#if defined(POLARSSL_CIPHER_MODE_CTR) || defined(POLARSSL_CIPHER_MODE_CFB)
    size_t offset;
#endif
#if defined(POLARSSL_CIPHER_MODE_CTR)
    int len;
    unsigned char nonce_counter[16];
    unsigned char stream_block[16];
#endif
    aes_context ctx;

    memset( key, 0, 32 );
    aes_init( &ctx );

    /*
     * ECB mode
     */
    for( i = 0; i < 6; i++ )
    {
        u = i >> 1;
        v = i  & 1;

        if( verbose != 0 )
            polarssl_printf( "  AES-ECB-%3d (%s): ", 128 + u * 64,
                             ( v == AES_DECRYPT ) ? "dec" : "enc" );

        memset( buf, 0, 16 );

        if( v == AES_DECRYPT )
        {
            aes_setkey_dec( &ctx, key, 128 + u * 64 );

            for( j = 0; j < 10000; j++ )
                aes_crypt_ecb( &ctx, v, buf, buf );

            if( memcmp( buf, aes_test_ecb_dec[u], 16 ) != 0 )
            {
                if( verbose != 0 )
                    polarssl_printf( "failed\n" );

                ret = 1;
                goto exit;
            }
        }
        else
        {
            aes_setkey_enc( &ctx, key, 128 + u * 64 );

            for( j = 0; j < 10000; j++ )
                aes_crypt_ecb( &ctx, v, buf, buf );

            if( memcmp( buf, aes_test_ecb_enc[u], 16 ) != 0 )
            {
                if( verbose != 0 )
                    polarssl_printf( "failed\n" );

                ret = 1;
                goto exit;
            }
        }

        if( verbose != 0 )
            polarssl_printf( "passed\n" );
    }

    if( verbose != 0 )
        polarssl_printf( "\n" );

#if defined(POLARSSL_CIPHER_MODE_CBC)
    /*
     * CBC mode
     */
    for( i = 0; i < 6; i++ )
    {
        u = i >> 1;
        v = i  & 1;

        if( verbose != 0 )
            polarssl_printf( "  AES-CBC-%3d (%s): ", 128 + u * 64,
                             ( v == AES_DECRYPT ) ? "dec" : "enc" );

        memset( iv , 0, 16 );
        memset( prv, 0, 16 );
        memset( buf, 0, 16 );

        if( v == AES_DECRYPT )
        {
            aes_setkey_dec( &ctx, key, 128 + u * 64 );

            for( j = 0; j < 10000; j++ )
                aes_crypt_cbc( &ctx, v, 16, iv, buf, buf );

            if( memcmp( buf, aes_test_cbc_dec[u], 16 ) != 0 )
            {
                if( verbose != 0 )
                    polarssl_printf( "failed\n" );

                ret = 1;
                goto exit;
            }
        }
        else
        {
            aes_setkey_enc( &ctx, key, 128 + u * 64 );

            for( j = 0; j < 10000; j++ )
            {
                unsigned char tmp[16];

                aes_crypt_cbc( &ctx, v, 16, iv, buf, buf );

                memcpy( tmp, prv, 16 );
                memcpy( prv, buf, 16 );
                memcpy( buf, tmp, 16 );
            }

            if( memcmp( prv, aes_test_cbc_enc[u], 16 ) != 0 )
            {
                if( verbose != 0 )
                    polarssl_printf( "failed\n" );

                ret = 1;
                goto exit;
            }
        }

        if( verbose != 0 )
            polarssl_printf( "passed\n" );
    }

    if( verbose != 0 )
        polarssl_printf( "\n" );
#endif /* POLARSSL_CIPHER_MODE_CBC */

#if defined(POLARSSL_CIPHER_MODE_CFB)
    /*
     * CFB128 mode
     */
    for( i = 0; i < 6; i++ )
    {
        u = i >> 1;
        v = i  & 1;

        if( verbose != 0 )
            polarssl_printf( "  AES-CFB128-%3d (%s): ", 128 + u * 64,
                             ( v == AES_DECRYPT ) ? "dec" : "enc" );

        memcpy( iv,  aes_test_cfb128_iv, 16 );
        memcpy( key, aes_test_cfb128_key[u], 16 + u * 8 );

        offset = 0;
        aes_setkey_enc( &ctx, key, 128 + u * 64 );

        if( v == AES_DECRYPT )
        {
            memcpy( buf, aes_test_cfb128_ct[u], 64 );
            aes_crypt_cfb128( &ctx, v, 64, &offset, iv, buf, buf );

            if( memcmp( buf, aes_test_cfb128_pt, 64 ) != 0 )
            {
                if( verbose != 0 )
                    polarssl_printf( "failed\n" );

                ret = 1;
                goto exit;
            }
        }
        else
        {
            memcpy( buf, aes_test_cfb128_pt, 64 );
            aes_crypt_cfb128( &ctx, v, 64, &offset, iv, buf, buf );

            if( memcmp( buf, aes_test_cfb128_ct[u], 64 ) != 0 )
            {
                if( verbose != 0 )
                    polarssl_printf( "failed\n" );

                ret = 1;
                goto exit;
            }
        }

        if( verbose != 0 )
            polarssl_printf( "passed\n" );
    }

    if( verbose != 0 )
        polarssl_printf( "\n" );
#endif /* POLARSSL_CIPHER_MODE_CFB */

#if defined(POLARSSL_CIPHER_MODE_CTR)
    /*
     * CTR mode
     */
    for( i = 0; i < 6; i++ )
    {
        u = i >> 1;
        v = i  & 1;

        if( verbose != 0 )
            polarssl_printf( "  AES-CTR-128 (%s): ",
                             ( v == AES_DECRYPT ) ? "dec" : "enc" );

        memcpy( nonce_counter, aes_test_ctr_nonce_counter[u], 16 );
        memcpy( key, aes_test_ctr_key[u], 16 );

        offset = 0;
        aes_setkey_enc( &ctx, key, 128 );

        if( v == AES_DECRYPT )
        {
            len = aes_test_ctr_len[u];
            memcpy( buf, aes_test_ctr_ct[u], len );

            aes_crypt_ctr( &ctx, len, &offset, nonce_counter, stream_block,
                           buf, buf );

            if( memcmp( buf, aes_test_ctr_pt[u], len ) != 0 )
            {
                if( verbose != 0 )
                    polarssl_printf( "failed\n" );

                ret = 1;
                goto exit;
            }
        }
        else
        {
            len = aes_test_ctr_len[u];
            memcpy( buf, aes_test_ctr_pt[u], len );

            aes_crypt_ctr( &ctx, len, &offset, nonce_counter, stream_block,
                           buf, buf );

            if( memcmp( buf, aes_test_ctr_ct[u], len ) != 0 )
            {
                if( verbose != 0 )
                    polarssl_printf( "failed\n" );

                ret = 1;
                goto exit;
            }
        }

        if( verbose != 0 )
            polarssl_printf( "passed\n" );
    }

    if( verbose != 0 )
        polarssl_printf( "\n" );
#endif /* POLARSSL_CIPHER_MODE_CTR */

    ret = 0;

exit:
    aes_free( &ctx );

    return( ret );
}
#endif /* POLARSSL_SELF_TEST */

#if defined(ARC4_POLARSSL_SELF_TEST)

#include "polarssl/arc4.h"
#include <string.h>
#include <stdio.h>

/*
 * ARC4 tests vectors as posted by Eric Rescorla in sep. 1994:
 *
 * http://groups.google.com/group/comp.security.misc/msg/10a300c9d21afca0
 */
static const unsigned char arc4_test_key[3][8] =
{
    { 0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF },
    { 0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF },
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }
};

static const unsigned char arc4_test_pt[3][8] =
{
    { 0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF },
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }
};

static const unsigned char arc4_test_ct[3][8] =
{
    { 0x75, 0xB7, 0x87, 0x80, 0x99, 0xE0, 0xC5, 0x96 },
    { 0x74, 0x94, 0xC2, 0xE7, 0x10, 0x4B, 0x08, 0x79 },
    { 0xDE, 0x18, 0x89, 0x41, 0xA3, 0x37, 0x5D, 0x3A }
};

/*
 * Checkup routine
 */
int arc4_self_test( int verbose )
{
    int i, ret = 0;
    unsigned char ibuf[8];
    unsigned char obuf[8];
    arc4_context ctx;

    arc4_init( &ctx );

    for( i = 0; i < 3; i++ )
    {
        if( verbose != 0 )
            polarssl_printf( "  ARC4 test #%d: ", i + 1 );

        memcpy( ibuf, arc4_test_pt[i], 8 );

        arc4_setup( &ctx, arc4_test_key[i], 8 );
        arc4_crypt( &ctx, 8, ibuf, obuf );

        if( memcmp( obuf, arc4_test_ct[i], 8 ) != 0 )
        {
            if( verbose != 0 )
                polarssl_printf( "failed\n" );

            ret = 1;
            goto exit;
        }

        if( verbose != 0 )
            polarssl_printf( "passed\n" );
    }

    if( verbose != 0 )
        polarssl_printf( "\n" );

exit:
    arc4_free( &ctx );

    return( ret );
}

#endif /* POLARSSL_SELF_TEST */

#if defined(BASE64_POLARSSL_SELF_TEST)

#include "polarssl/base64.h"
#include <string.h>
#include <stdio.h>

static const unsigned char base64_test_dec[64] =
{
    0x24, 0x48, 0x6E, 0x56, 0x87, 0x62, 0x5A, 0xBD,
    0xBF, 0x17, 0xD9, 0xA2, 0xC4, 0x17, 0x1A, 0x01,
    0x94, 0xED, 0x8F, 0x1E, 0x11, 0xB3, 0xD7, 0x09,
    0x0C, 0xB6, 0xE9, 0x10, 0x6F, 0x22, 0xEE, 0x13,
    0xCA, 0xB3, 0x07, 0x05, 0x76, 0xC9, 0xFA, 0x31,
    0x6C, 0x08, 0x34, 0xFF, 0x8D, 0xC2, 0x6C, 0x38,
    0x00, 0x43, 0xE9, 0x54, 0x97, 0xAF, 0x50, 0x4B,
    0xD1, 0x41, 0xBA, 0x95, 0x31, 0x5A, 0x0B, 0x97
};

static const unsigned char base64_test_enc[] =
    "JEhuVodiWr2/F9mixBcaAZTtjx4Rs9cJDLbpEG8i7hPK"
    "swcFdsn6MWwINP+Nwmw4AEPpVJevUEvRQbqVMVoLlw==";

/*
 * Checkup routine
 */
int base64_self_test( int verbose )
{
    size_t len;
    const unsigned char *src;
    unsigned char buffer[128];

    if( verbose != 0 )
        polarssl_printf( "  Base64 encoding test: " );

    len = sizeof( buffer );
    src = base64_test_dec;

    if( base64_encode( buffer, &len, src, 64 ) != 0 ||
         memcmp( base64_test_enc, buffer, 88 ) != 0 )
    {
        if( verbose != 0 )
            polarssl_printf( "failed\n" );

        return( 1 );
    }

    if( verbose != 0 )
        polarssl_printf( "passed\n  Base64 decoding test: " );

    len = sizeof( buffer );
    src = base64_test_enc;

    if( base64_decode( buffer, &len, src, 88 ) != 0 ||
         memcmp( base64_test_dec, buffer, 64 ) != 0 )
    {
        if( verbose != 0 )
            polarssl_printf( "failed\n" );

        return( 1 );
    }

    if( verbose != 0 )
        polarssl_printf( "passed\n\n" );

    return( 0 );
}

#endif /* POLARSSL_SELF_TEST */

#if defined(BIGNUM_POLARSSL_SELF_TEST)

#include "polarssl/bignum.h"
#include "polarssl/bn_mul.h"

#define GCD_PAIR_COUNT  3

static const int gcd_pairs[GCD_PAIR_COUNT][3] =
{
    { 693, 609, 21 },
    { 1764, 868, 28 },
    { 768454923, 542167814, 1 }
};

/*
 * Checkup routine
 */
int mpi_self_test( int verbose )
{
    int ret, i;
    mpi A, E, N, X, Y, U, V;

    mpi_init( &A ); mpi_init( &E ); mpi_init( &N ); mpi_init( &X );
    mpi_init( &Y ); mpi_init( &U ); mpi_init( &V );

    MPI_CHK( mpi_read_string( &A, 16,
        "EFE021C2645FD1DC586E69184AF4A31E" \
        "D5F53E93B5F123FA41680867BA110131" \
        "944FE7952E2517337780CB0DB80E61AA" \
        "E7C8DDC6C5C6AADEB34EB38A2F40D5E6" ) );

    MPI_CHK( mpi_read_string( &E, 16,
        "B2E7EFD37075B9F03FF989C7C5051C20" \
        "34D2A323810251127E7BF8625A4F49A5" \
        "F3E27F4DA8BD59C47D6DAABA4C8127BD" \
        "5B5C25763222FEFCCFC38B832366C29E" ) );

    MPI_CHK( mpi_read_string( &N, 16,
        "0066A198186C18C10B2F5ED9B522752A" \
        "9830B69916E535C8F047518A889A43A5" \
        "94B6BED27A168D31D4A52F88925AA8F5" ) );

    MPI_CHK( mpi_mul_mpi( &X, &A, &N ) );

    MPI_CHK( mpi_read_string( &U, 16,
        "602AB7ECA597A3D6B56FF9829A5E8B85" \
        "9E857EA95A03512E2BAE7391688D264A" \
        "A5663B0341DB9CCFD2C4C5F421FEC814" \
        "8001B72E848A38CAE1C65F78E56ABDEF" \
        "E12D3C039B8A02D6BE593F0BBBDA56F1" \
        "ECF677152EF804370C1A305CAF3B5BF1" \
        "30879B56C61DE584A0F53A2447A51E" ) );

    if( verbose != 0 )
        polarssl_printf( "  MPI test #1 (mul_mpi): " );

    if( mpi_cmp_mpi( &X, &U ) != 0 )
    {
        if( verbose != 0 )
            polarssl_printf( "failed\n" );

        ret = 1;
        goto cleanup;
    }

    if( verbose != 0 )
        polarssl_printf( "passed\n" );

    MPI_CHK( mpi_div_mpi( &X, &Y, &A, &N ) );

    MPI_CHK( mpi_read_string( &U, 16,
        "256567336059E52CAE22925474705F39A94" ) );

    MPI_CHK( mpi_read_string( &V, 16,
        "6613F26162223DF488E9CD48CC132C7A" \
        "0AC93C701B001B092E4E5B9F73BCD27B" \
        "9EE50D0657C77F374E903CDFA4C642" ) );

    if( verbose != 0 )
        polarssl_printf( "  MPI test #2 (div_mpi): " );

    if( mpi_cmp_mpi( &X, &U ) != 0 ||
        mpi_cmp_mpi( &Y, &V ) != 0 )
    {
        if( verbose != 0 )
            polarssl_printf( "failed\n" );

        ret = 1;
        goto cleanup;
    }

    if( verbose != 0 )
        polarssl_printf( "passed\n" );

    MPI_CHK( mpi_exp_mod( &X, &A, &E, &N, NULL ) );

    MPI_CHK( mpi_read_string( &U, 16,
        "36E139AEA55215609D2816998ED020BB" \
        "BD96C37890F65171D948E9BC7CBAA4D9" \
        "325D24D6A3C12710F10A09FA08AB87" ) );

    if( verbose != 0 )
        polarssl_printf( "  MPI test #3 (exp_mod): " );

    if( mpi_cmp_mpi( &X, &U ) != 0 )
    {
        if( verbose != 0 )
            polarssl_printf( "failed\n" );

        ret = 1;
        goto cleanup;
    }

    if( verbose != 0 )
        polarssl_printf( "passed\n" );

    MPI_CHK( mpi_inv_mod( &X, &A, &N ) );

    MPI_CHK( mpi_read_string( &U, 16,
        "003A0AAEDD7E784FC07D8F9EC6E3BFD5" \
        "C3DBA76456363A10869622EAC2DD84EC" \
        "C5B8A74DAC4D09E03B5E0BE779F2DF61" ) );

    if( verbose != 0 )
        polarssl_printf( "  MPI test #4 (inv_mod): " );

    if( mpi_cmp_mpi( &X, &U ) != 0 )
    {
        if( verbose != 0 )
            polarssl_printf( "failed\n" );

        ret = 1;
        goto cleanup;
    }

    if( verbose != 0 )
        polarssl_printf( "passed\n" );

    if( verbose != 0 )
        polarssl_printf( "  MPI test #5 (simple gcd): " );

    for( i = 0; i < GCD_PAIR_COUNT; i++ )
    {
        MPI_CHK( mpi_lset( &X, gcd_pairs[i][0] ) );
        MPI_CHK( mpi_lset( &Y, gcd_pairs[i][1] ) );

        MPI_CHK( mpi_gcd( &A, &X, &Y ) );

        if( mpi_cmp_int( &A, gcd_pairs[i][2] ) != 0 )
        {
            if( verbose != 0 )
                polarssl_printf( "failed at %d\n", i );

            ret = 1;
            goto cleanup;
        }
    }

    if( verbose != 0 )
        polarssl_printf( "passed\n" );

cleanup:

    if( ret != 0 && verbose != 0 )
        polarssl_printf( "Unexpected error, return code = %08X\n", ret );

    mpi_free( &A ); mpi_free( &E ); mpi_free( &N ); mpi_free( &X );
    mpi_free( &Y ); mpi_free( &U ); mpi_free( &V );

    if( verbose != 0 )
        polarssl_printf( "\n" );

    return( ret );
}

#endif /* POLARSSL_SELF_TEST */

#if defined(CTR_DRBG_POLARSSL_SELF_TEST)

#include "polarssl/ctr_drbg.h"
#include <stdio.h>

static unsigned char entropy_source_pr[96] =
    { 0xc1, 0x80, 0x81, 0xa6, 0x5d, 0x44, 0x02, 0x16,
      0x19, 0xb3, 0xf1, 0x80, 0xb1, 0xc9, 0x20, 0x02,
      0x6a, 0x54, 0x6f, 0x0c, 0x70, 0x81, 0x49, 0x8b,
      0x6e, 0xa6, 0x62, 0x52, 0x6d, 0x51, 0xb1, 0xcb,
      0x58, 0x3b, 0xfa, 0xd5, 0x37, 0x5f, 0xfb, 0xc9,
      0xff, 0x46, 0xd2, 0x19, 0xc7, 0x22, 0x3e, 0x95,
      0x45, 0x9d, 0x82, 0xe1, 0xe7, 0x22, 0x9f, 0x63,
      0x31, 0x69, 0xd2, 0x6b, 0x57, 0x47, 0x4f, 0xa3,
      0x37, 0xc9, 0x98, 0x1c, 0x0b, 0xfb, 0x91, 0x31,
      0x4d, 0x55, 0xb9, 0xe9, 0x1c, 0x5a, 0x5e, 0xe4,
      0x93, 0x92, 0xcf, 0xc5, 0x23, 0x12, 0xd5, 0x56,
      0x2c, 0x4a, 0x6e, 0xff, 0xdc, 0x10, 0xd0, 0x68 };

static unsigned char entropy_source_nopr[64] =
    { 0x5a, 0x19, 0x4d, 0x5e, 0x2b, 0x31, 0x58, 0x14,
      0x54, 0xde, 0xf6, 0x75, 0xfb, 0x79, 0x58, 0xfe,
      0xc7, 0xdb, 0x87, 0x3e, 0x56, 0x89, 0xfc, 0x9d,
      0x03, 0x21, 0x7c, 0x68, 0xd8, 0x03, 0x38, 0x20,
      0xf9, 0xe6, 0x5e, 0x04, 0xd8, 0x56, 0xf3, 0xa9,
      0xc4, 0x4a, 0x4c, 0xbd, 0xc1, 0xd0, 0x08, 0x46,
      0xf5, 0x98, 0x3d, 0x77, 0x1c, 0x1b, 0x13, 0x7e,
      0x4e, 0x0f, 0x9d, 0x8e, 0xf4, 0x09, 0xf9, 0x2e };

static const unsigned char nonce_pers_pr[16] =
    { 0xd2, 0x54, 0xfc, 0xff, 0x02, 0x1e, 0x69, 0xd2,
      0x29, 0xc9, 0xcf, 0xad, 0x85, 0xfa, 0x48, 0x6c };

static const unsigned char nonce_pers_nopr[16] =
    { 0x1b, 0x54, 0xb8, 0xff, 0x06, 0x42, 0xbf, 0xf5,
      0x21, 0xf1, 0x5c, 0x1c, 0x0b, 0x66, 0x5f, 0x3f };

static const unsigned char ctr_drbg_result_pr[16] =
    { 0x34, 0x01, 0x16, 0x56, 0xb4, 0x29, 0x00, 0x8f,
      0x35, 0x63, 0xec, 0xb5, 0xf2, 0x59, 0x07, 0x23 };

static const unsigned char ctr_drbg_result_nopr[16] =
    { 0xa0, 0x54, 0x30, 0x3d, 0x8a, 0x7e, 0xa9, 0x88,
      0x9d, 0x90, 0x3e, 0x07, 0x7c, 0x6f, 0x21, 0x8f };

static size_t test_offset;
static int ctr_drbg_self_test_entropy( void *data, unsigned char *buf,
                                       size_t len )
{
    const unsigned char *p = data;
    memcpy( buf, p + test_offset, len );
    test_offset += len;
    return( 0 );
}

#define CHK( c )    if( (c) != 0 )                          \
                    {                                       \
                        if( verbose != 0 )                  \
                            polarssl_printf( "failed\n" );  \
                        return( 1 );                        \
                    }

/*
 * Checkup routine
 */
int ctr_drbg_self_test( int verbose )
{
    ctr_drbg_context ctx;
    unsigned char buf[16];

    /*
     * Based on a NIST CTR_DRBG test vector (PR = True)
     */
    if( verbose != 0 )
        polarssl_printf( "  CTR_DRBG (PR = TRUE) : " );

    test_offset = 0;
    CHK( ctr_drbg_init_entropy_len( &ctx, ctr_drbg_self_test_entropy,
                                entropy_source_pr, nonce_pers_pr, 16, 32 ) );
    ctr_drbg_set_prediction_resistance( &ctx, CTR_DRBG_PR_ON );
    CHK( ctr_drbg_random( &ctx, buf, CTR_DRBG_BLOCKSIZE ) );
    CHK( ctr_drbg_random( &ctx, buf, CTR_DRBG_BLOCKSIZE ) );
    CHK( memcmp( buf, ctr_drbg_result_pr, CTR_DRBG_BLOCKSIZE ) );

    if( verbose != 0 )
        polarssl_printf( "passed\n" );

    /*
     * Based on a NIST CTR_DRBG test vector (PR = FALSE)
     */
    if( verbose != 0 )
        polarssl_printf( "  CTR_DRBG (PR = FALSE): " );

    test_offset = 0;
    CHK( ctr_drbg_init_entropy_len( &ctx, ctr_drbg_self_test_entropy,
                            entropy_source_nopr, nonce_pers_nopr, 16, 32 ) );
    CHK( ctr_drbg_random( &ctx, buf, 16 ) );
    CHK( ctr_drbg_reseed( &ctx, NULL, 0 ) );
    CHK( ctr_drbg_random( &ctx, buf, 16 ) );
    CHK( memcmp( buf, ctr_drbg_result_nopr, 16 ) );

    if( verbose != 0 )
        polarssl_printf( "passed\n" );

    if( verbose != 0 )
            polarssl_printf( "\n" );

    return( 0 );
}
#endif /* POLARSSL_SELF_TEST */

#if defined(DES_POLARSSL_SELF_TEST)

#include "polarssl/des.h"
#include <stdio.h>

/*
 * DES and 3DES test vectors from:
 *
 * http://csrc.nist.gov/groups/STM/cavp/documents/des/tripledes-vectors.zip
 */
static const unsigned char des3_test_keys[24] =
{
    0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF,
    0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF, 0x01,
    0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF, 0x01, 0x23
};

static const unsigned char des3_test_buf[8] =
{
    0x4E, 0x6F, 0x77, 0x20, 0x69, 0x73, 0x20, 0x74
};

static const unsigned char des3_test_ecb_dec[3][8] =
{
    { 0xCD, 0xD6, 0x4F, 0x2F, 0x94, 0x27, 0xC1, 0x5D },
    { 0x69, 0x96, 0xC8, 0xFA, 0x47, 0xA2, 0xAB, 0xEB },
    { 0x83, 0x25, 0x39, 0x76, 0x44, 0x09, 0x1A, 0x0A }
};

static const unsigned char des3_test_ecb_enc[3][8] =
{
    { 0x6A, 0x2A, 0x19, 0xF4, 0x1E, 0xCA, 0x85, 0x4B },
    { 0x03, 0xE6, 0x9F, 0x5B, 0xFA, 0x58, 0xEB, 0x42 },
    { 0xDD, 0x17, 0xE8, 0xB8, 0xB4, 0x37, 0xD2, 0x32 }
};

#if defined(POLARSSL_CIPHER_MODE_CBC)
static const unsigned char des3_test_iv[8] =
{
    0x12, 0x34, 0x56, 0x78, 0x90, 0xAB, 0xCD, 0xEF,
};

static const unsigned char des3_test_cbc_dec[3][8] =
{
    { 0x12, 0x9F, 0x40, 0xB9, 0xD2, 0x00, 0x56, 0xB3 },
    { 0x47, 0x0E, 0xFC, 0x9A, 0x6B, 0x8E, 0xE3, 0x93 },
    { 0xC5, 0xCE, 0xCF, 0x63, 0xEC, 0xEC, 0x51, 0x4C }
};

static const unsigned char des3_test_cbc_enc[3][8] =
{
    { 0x54, 0xF1, 0x5A, 0xF6, 0xEB, 0xE3, 0xA4, 0xB4 },
    { 0x35, 0x76, 0x11, 0x56, 0x5F, 0xA1, 0x8E, 0x4D },
    { 0xCB, 0x19, 0x1F, 0x85, 0xD1, 0xED, 0x84, 0x39 }
};
#endif /* POLARSSL_CIPHER_MODE_CBC */

/*
 * Checkup routine
 */
int des_self_test( int verbose )
{
    int i, j, u, v, ret = 0;
    des_context ctx;
    des3_context ctx3;
    unsigned char buf[8];
#if defined(POLARSSL_CIPHER_MODE_CBC)
    unsigned char prv[8];
    unsigned char iv[8];
#endif

    des_init( &ctx );
    des3_init( &ctx3 );
    /*
     * ECB mode
     */
    for( i = 0; i < 6; i++ )
    {
        u = i >> 1;
        v = i  & 1;

        if( verbose != 0 )
            polarssl_printf( "  DES%c-ECB-%3d (%s): ",
                             ( u == 0 ) ? ' ' : '3', 56 + u * 56,
                             ( v == DES_DECRYPT ) ? "dec" : "enc" );

        memcpy( buf, des3_test_buf, 8 );

        switch( i )
        {
        case 0:
            des_setkey_dec( &ctx, des3_test_keys );
            break;

        case 1:
            des_setkey_enc( &ctx, des3_test_keys );
            break;

        case 2:
            des3_set2key_dec( &ctx3, des3_test_keys );
            break;

        case 3:
            des3_set2key_enc( &ctx3, des3_test_keys );
            break;

        case 4:
            des3_set3key_dec( &ctx3, des3_test_keys );
            break;

        case 5:
            des3_set3key_enc( &ctx3, des3_test_keys );
            break;

        default:
            return( 1 );
        }

        for( j = 0; j < 10000; j++ )
        {
            if( u == 0 )
                des_crypt_ecb( &ctx, buf, buf );
            else
                des3_crypt_ecb( &ctx3, buf, buf );
        }

        if( ( v == DES_DECRYPT &&
                memcmp( buf, des3_test_ecb_dec[u], 8 ) != 0 ) ||
            ( v != DES_DECRYPT &&
                memcmp( buf, des3_test_ecb_enc[u], 8 ) != 0 ) )
        {
            if( verbose != 0 )
                polarssl_printf( "failed\n" );

            ret = 1;
            goto exit;
        }

        if( verbose != 0 )
            polarssl_printf( "passed\n" );
    }

    if( verbose != 0 )
        polarssl_printf( "\n" );

#if defined(POLARSSL_CIPHER_MODE_CBC)
    /*
     * CBC mode
     */
    for( i = 0; i < 6; i++ )
    {
        u = i >> 1;
        v = i  & 1;

        if( verbose != 0 )
            polarssl_printf( "  DES%c-CBC-%3d (%s): ",
                             ( u == 0 ) ? ' ' : '3', 56 + u * 56,
                             ( v == DES_DECRYPT ) ? "dec" : "enc" );

        memcpy( iv,  des3_test_iv,  8 );
        memcpy( prv, des3_test_iv,  8 );
        memcpy( buf, des3_test_buf, 8 );

        switch( i )
        {
        case 0:
            des_setkey_dec( &ctx, des3_test_keys );
            break;

        case 1:
            des_setkey_enc( &ctx, des3_test_keys );
            break;

        case 2:
            des3_set2key_dec( &ctx3, des3_test_keys );
            break;

        case 3:
            des3_set2key_enc( &ctx3, des3_test_keys );
            break;

        case 4:
            des3_set3key_dec( &ctx3, des3_test_keys );
            break;

        case 5:
            des3_set3key_enc( &ctx3, des3_test_keys );
            break;

        default:
            return( 1 );
        }

        if( v == DES_DECRYPT )
        {
            for( j = 0; j < 10000; j++ )
            {
                if( u == 0 )
                    des_crypt_cbc( &ctx, v, 8, iv, buf, buf );
                else
                    des3_crypt_cbc( &ctx3, v, 8, iv, buf, buf );
            }
        }
        else
        {
            for( j = 0; j < 10000; j++ )
            {
                unsigned char tmp[8];

                if( u == 0 )
                    des_crypt_cbc( &ctx, v, 8, iv, buf, buf );
                else
                    des3_crypt_cbc( &ctx3, v, 8, iv, buf, buf );

                memcpy( tmp, prv, 8 );
                memcpy( prv, buf, 8 );
                memcpy( buf, tmp, 8 );
            }

            memcpy( buf, prv, 8 );
        }

        if( ( v == DES_DECRYPT &&
                memcmp( buf, des3_test_cbc_dec[u], 8 ) != 0 ) ||
            ( v != DES_DECRYPT &&
                memcmp( buf, des3_test_cbc_enc[u], 8 ) != 0 ) )
        {
            if( verbose != 0 )
                polarssl_printf( "failed\n" );

            ret = 1;
            goto exit;
        }

        if( verbose != 0 )
            polarssl_printf( "passed\n" );
    }
#endif /* POLARSSL_CIPHER_MODE_CBC */

    if( verbose != 0 )
        polarssl_printf( "\n" );

exit:
    des_free( &ctx );
    des3_free( &ctx3 );

    return( ret );
}

#endif /* POLARSSL_SELF_TEST */

#if defined(DHM_POLARSSL_SELF_TEST)

#include "polarssl/dhm.h"
#include "polarssl/certs.h"

/*
 * Checkup routine
 */
int dhm_self_test( int verbose )
{
#if defined(POLARSSL_CERTS_C)
    int ret;
    dhm_context dhm;

    dhm_init( &dhm );

    if( verbose != 0 )
        polarssl_printf( "  DHM parameter load: " );

    if( ( ret = dhm_parse_dhm( &dhm, (const unsigned char *) test_dhm_params,
                               strlen( test_dhm_params ) ) ) != 0 )
    {
        if( verbose != 0 )
            polarssl_printf( "failed\n" );

        ret = 1;
        goto exit;
    }

    if( verbose != 0 )
        polarssl_printf( "passed\n\n" );

exit:
    dhm_free( &dhm );

    return( ret );
#else
    if( verbose != 0 )
        polarssl_printf( "  DHM parameter load: skipped\n" );

    return( 0 );
#endif /* POLARSSL_CERTS_C */
}

#endif /* POLARSSL_SELF_TEST */

#if defined(ECP_POLARSSL_SELF_TEST)

#include "polarssl/ecp.h"

/*
 * Counts of point addition and doubling, and field multiplications.
 * Used to test resistance of point multiplication to simple timing attacks.
 */
extern unsigned long add_count, dbl_count, mul_count;

/*
 * Checkup routine
 */
int ecp_self_test_1( int verbose )
{
    int ret;
    size_t i;
    ecp_group grp;
    ecp_point R, P;
    mpi m;
    unsigned long add_c_prev, dbl_c_prev, mul_c_prev;
    /* exponents especially adapted for secp192r1 */
    const char *exponents[] =
    {
        "000000000000000000000000000000000000000000000001", /* one */
        "FFFFFFFFFFFFFFFFFFFFFFFF99DEF836146BC9B1B4D22830", /* N - 1 */
        "5EA6F389A38B8BC81E767753B15AA5569E1782E30ABE7D25", /* random */
        "400000000000000000000000000000000000000000000000", /* one and zeros */
        "7FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF", /* all ones */
        "555555555555555555555555555555555555555555555555", /* 101010... */
    };

    ecp_group_init( &grp );
    ecp_point_init( &R );
    ecp_point_init( &P );
    mpi_init( &m );

    /* Use secp192r1 if available, or any available curve */
#if defined(POLARSSL_ECP_DP_SECP192R1_ENABLED)
    MPI_CHK( ecp_use_known_dp( &grp, POLARSSL_ECP_DP_SECP192R1 ) );
#else
    MPI_CHK( ecp_use_known_dp( &grp, ecp_curve_list()->grp_id ) );
#endif

    if( verbose != 0 )
        polarssl_printf( "  ECP test #1 (constant op_count, base point G): " );

    /* Do a dummy multiplication first to trigger precomputation */
    MPI_CHK( mpi_lset( &m, 2 ) );
    MPI_CHK( ecp_mul( &grp, &P, &m, &grp.G, NULL, NULL ) );

    add_count = 0;
    dbl_count = 0;
    mul_count = 0;
    MPI_CHK( mpi_read_string( &m, 16, exponents[0] ) );
    MPI_CHK( ecp_mul( &grp, &R, &m, &grp.G, NULL, NULL ) );

    for( i = 1; i < sizeof( exponents ) / sizeof( exponents[0] ); i++ )
    {
        add_c_prev = add_count;
        dbl_c_prev = dbl_count;
        mul_c_prev = mul_count;
        add_count = 0;
        dbl_count = 0;
        mul_count = 0;

        MPI_CHK( mpi_read_string( &m, 16, exponents[i] ) );
        MPI_CHK( ecp_mul( &grp, &R, &m, &grp.G, NULL, NULL ) );

        if( add_count != add_c_prev ||
            dbl_count != dbl_c_prev ||
            mul_count != mul_c_prev )
        {
            if( verbose != 0 )
                polarssl_printf( "failed (%u)\n", (unsigned int) i );

            ret = 1;
            goto cleanup;
        }
    }

    if( verbose != 0 )
        polarssl_printf( "passed\n" );

    if( verbose != 0 )
        polarssl_printf( "  ECP test #2 (constant op_count, other point): " );
    /* We computed P = 2G last time, use it */

    add_count = 0;
    dbl_count = 0;
    mul_count = 0;
    MPI_CHK( mpi_read_string( &m, 16, exponents[0] ) );
    MPI_CHK( ecp_mul( &grp, &R, &m, &P, NULL, NULL ) );

    for( i = 1; i < sizeof( exponents ) / sizeof( exponents[0] ); i++ )
    {
        add_c_prev = add_count;
        dbl_c_prev = dbl_count;
        mul_c_prev = mul_count;
        add_count = 0;
        dbl_count = 0;
        mul_count = 0;

        MPI_CHK( mpi_read_string( &m, 16, exponents[i] ) );
        MPI_CHK( ecp_mul( &grp, &R, &m, &P, NULL, NULL ) );

        if( add_count != add_c_prev ||
            dbl_count != dbl_c_prev ||
            mul_count != mul_c_prev )
        {
            if( verbose != 0 )
                polarssl_printf( "failed (%u)\n", (unsigned int) i );

            ret = 1;
            goto cleanup;
        }
    }

    if( verbose != 0 )
        polarssl_printf( "passed\n" );

cleanup:

    if( ret < 0 && verbose != 0 )
        polarssl_printf( "Unexpected error, return code = %08X\n", ret );

    ecp_group_free( &grp );
    ecp_point_free( &R );
    ecp_point_free( &P );
    mpi_free( &m );

    if( verbose != 0 )
        polarssl_printf( "\n" );

    return( ret );
}

#endif /* POLARSSL_SELF_TEST */

#if defined(HMAC_DRBG_POLARSSL_SELF_TEST)

#include "polarssl/hmac_drbg.h"
#include <stdio.h>

#if !defined(POLARSSL_SHA1_C)
/* Dummy checkup routine */
int hmac_drbg_self_test( int verbose )
{

    if( verbose != 0 )
        polarssl_printf( "\n" );

    return( 0 );
}
#else

#define OUTPUT_LEN  80

/* From a NIST PR=true test vector */
static unsigned char entropy_pr[] = {
    0xa0, 0xc9, 0xab, 0x58, 0xf1, 0xe2, 0xe5, 0xa4, 0xde, 0x3e, 0xbd, 0x4f,
    0xf7, 0x3e, 0x9c, 0x5b, 0x64, 0xef, 0xd8, 0xca, 0x02, 0x8c, 0xf8, 0x11,
    0x48, 0xa5, 0x84, 0xfe, 0x69, 0xab, 0x5a, 0xee, 0x42, 0xaa, 0x4d, 0x42,
    0x17, 0x60, 0x99, 0xd4, 0x5e, 0x13, 0x97, 0xdc, 0x40, 0x4d, 0x86, 0xa3,
    0x7b, 0xf5, 0x59, 0x54, 0x75, 0x69, 0x51, 0xe4 };
static const unsigned char hmac_drbg_result_pr[OUTPUT_LEN] = {
    0x9a, 0x00, 0xa2, 0xd0, 0x0e, 0xd5, 0x9b, 0xfe, 0x31, 0xec, 0xb1, 0x39,
    0x9b, 0x60, 0x81, 0x48, 0xd1, 0x96, 0x9d, 0x25, 0x0d, 0x3c, 0x1e, 0x94,
    0x10, 0x10, 0x98, 0x12, 0x93, 0x25, 0xca, 0xb8, 0xfc, 0xcc, 0x2d, 0x54,
    0x73, 0x19, 0x70, 0xc0, 0x10, 0x7a, 0xa4, 0x89, 0x25, 0x19, 0x95, 0x5e,
    0x4b, 0xc6, 0x00, 0x1d, 0x7f, 0x4e, 0x6a, 0x2b, 0xf8, 0xa3, 0x01, 0xab,
    0x46, 0x05, 0x5c, 0x09, 0xa6, 0x71, 0x88, 0xf1, 0xa7, 0x40, 0xee, 0xf3,
    0xe1, 0x5c, 0x02, 0x9b, 0x44, 0xaf, 0x03, 0x44 };

/* From a NIST PR=false test vector */
static unsigned char entropy_nopr[] = {
    0x79, 0x34, 0x9b, 0xbf, 0x7c, 0xdd, 0xa5, 0x79, 0x95, 0x57, 0x86, 0x66,
    0x21, 0xc9, 0x13, 0x83, 0x11, 0x46, 0x73, 0x3a, 0xbf, 0x8c, 0x35, 0xc8,
    0xc7, 0x21, 0x5b, 0x5b, 0x96, 0xc4, 0x8e, 0x9b, 0x33, 0x8c, 0x74, 0xe3,
    0xe9, 0x9d, 0xfe, 0xdf };
static const unsigned char hmac_drbg_result_nopr[OUTPUT_LEN] = {
    0xc6, 0xa1, 0x6a, 0xb8, 0xd4, 0x20, 0x70, 0x6f, 0x0f, 0x34, 0xab, 0x7f,
    0xec, 0x5a, 0xdc, 0xa9, 0xd8, 0xca, 0x3a, 0x13, 0x3e, 0x15, 0x9c, 0xa6,
    0xac, 0x43, 0xc6, 0xf8, 0xa2, 0xbe, 0x22, 0x83, 0x4a, 0x4c, 0x0a, 0x0a,
    0xff, 0xb1, 0x0d, 0x71, 0x94, 0xf1, 0xc1, 0xa5, 0xcf, 0x73, 0x22, 0xec,
    0x1a, 0xe0, 0x96, 0x4e, 0xd4, 0xbf, 0x12, 0x27, 0x46, 0xe0, 0x87, 0xfd,
    0xb5, 0xb3, 0xe9, 0x1b, 0x34, 0x93, 0xd5, 0xbb, 0x98, 0xfa, 0xed, 0x49,
    0xe8, 0x5f, 0x13, 0x0f, 0xc8, 0xa4, 0x59, 0xb7 };

/* "Entropy" from buffer */
static size_t test_offset;
static int hmac_drbg_self_test_entropy( void *data,
                                        unsigned char *buf, size_t len )
{
    const unsigned char *p = data;
    memcpy( buf, p + test_offset, len );
    test_offset += len;
    return( 0 );
}

#define CHK( c )    if( (c) != 0 )                          \
                    {                                       \
                        if( verbose != 0 )                  \
                            polarssl_printf( "failed\n" );  \
                        return( 1 );                        \
                    }

/*
 * Checkup routine for HMAC_DRBG with SHA-1
 */
int hmac_drbg_self_test( int verbose )
{
    hmac_drbg_context ctx;
    unsigned char buf[OUTPUT_LEN];
    const md_info_t *md_info = md_info_from_type( POLARSSL_MD_SHA1 );

    /*
     * PR = True
     */
    if( verbose != 0 )
        polarssl_printf( "  HMAC_DRBG (PR = True) : " );

    test_offset = 0;
    CHK( hmac_drbg_init( &ctx, md_info,
                         hmac_drbg_self_test_entropy, entropy_pr,
                         NULL, 0 ) );
    hmac_drbg_set_prediction_resistance( &ctx, POLARSSL_HMAC_DRBG_PR_ON );
    CHK( hmac_drbg_random( &ctx, buf, OUTPUT_LEN ) );
    CHK( hmac_drbg_random( &ctx, buf, OUTPUT_LEN ) );
    CHK( memcmp( buf, hmac_drbg_result_pr, OUTPUT_LEN ) );
    hmac_drbg_free( &ctx );

    if( verbose != 0 )
        polarssl_printf( "passed\n" );

    /*
     * PR = False
     */
    if( verbose != 0 )
        polarssl_printf( "  HMAC_DRBG (PR = False) : " );

    test_offset = 0;
    CHK( hmac_drbg_init( &ctx, md_info,
                         hmac_drbg_self_test_entropy, entropy_nopr,
                         NULL, 0 ) );
    CHK( hmac_drbg_reseed( &ctx, NULL, 0 ) );
    CHK( hmac_drbg_random( &ctx, buf, OUTPUT_LEN ) );
    CHK( hmac_drbg_random( &ctx, buf, OUTPUT_LEN ) );
    CHK( memcmp( buf, hmac_drbg_result_nopr, OUTPUT_LEN ) );
    hmac_drbg_free( &ctx );

    if( verbose != 0 )
        polarssl_printf( "passed\n" );

    if( verbose != 0 )
        polarssl_printf( "\n" );

    return( 0 );
}
#endif /* POLARSSL_SHA1_C */
#endif /* POLARSSL_SELF_TEST */

#if defined(MD5_POLARSSL_SELF_TEST)

#include "polarssl/md5.h"

/*
 * RFC 1321 test vectors
 */
static unsigned char md5_test_buf[7][81] =
{
    { "" },
    { "a" },
    { "abc" },
    { "message digest" },
    { "abcdefghijklmnopqrstuvwxyz" },
    { "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789" },
    { "12345678901234567890123456789012345678901234567890123456789012" \
      "345678901234567890" }
};

static const int md5_test_buflen[7] =
{
    0, 1, 3, 14, 26, 62, 80
};

static const unsigned char md5_test_sum[7][16] =
{
    { 0xD4, 0x1D, 0x8C, 0xD9, 0x8F, 0x00, 0xB2, 0x04,
      0xE9, 0x80, 0x09, 0x98, 0xEC, 0xF8, 0x42, 0x7E },
    { 0x0C, 0xC1, 0x75, 0xB9, 0xC0, 0xF1, 0xB6, 0xA8,
      0x31, 0xC3, 0x99, 0xE2, 0x69, 0x77, 0x26, 0x61 },
    { 0x90, 0x01, 0x50, 0x98, 0x3C, 0xD2, 0x4F, 0xB0,
      0xD6, 0x96, 0x3F, 0x7D, 0x28, 0xE1, 0x7F, 0x72 },
    { 0xF9, 0x6B, 0x69, 0x7D, 0x7C, 0xB7, 0x93, 0x8D,
      0x52, 0x5A, 0x2F, 0x31, 0xAA, 0xF1, 0x61, 0xD0 },
    { 0xC3, 0xFC, 0xD3, 0xD7, 0x61, 0x92, 0xE4, 0x00,
      0x7D, 0xFB, 0x49, 0x6C, 0xCA, 0x67, 0xE1, 0x3B },
    { 0xD1, 0x74, 0xAB, 0x98, 0xD2, 0x77, 0xD9, 0xF5,
      0xA5, 0x61, 0x1C, 0x2C, 0x9F, 0x41, 0x9D, 0x9F },
    { 0x57, 0xED, 0xF4, 0xA2, 0x2B, 0xE3, 0xC9, 0x55,
      0xAC, 0x49, 0xDA, 0x2E, 0x21, 0x07, 0xB6, 0x7A }
};

/*
 * RFC 2202 test vectors
 */
static unsigned char md5_hmac_test_key[7][26] =
{
    { "\x0B\x0B\x0B\x0B\x0B\x0B\x0B\x0B\x0B\x0B\x0B\x0B\x0B\x0B\x0B\x0B" },
    { "Jefe" },
    { "\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA" },
    { "\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A\x0B\x0C\x0D\x0E\x0F\x10"
      "\x11\x12\x13\x14\x15\x16\x17\x18\x19" },
    { "\x0C\x0C\x0C\x0C\x0C\x0C\x0C\x0C\x0C\x0C\x0C\x0C\x0C\x0C\x0C\x0C" },
    { "" }, /* 0xAA 80 times */
    { "" }
};

static const int md5_hmac_test_keylen[7] =
{
    16, 4, 16, 25, 16, 80, 80
};

static unsigned char md5_hmac_test_buf[7][74] =
{
    { "Hi There" },
    { "what do ya want for nothing?" },
    { "\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD"
      "\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD"
      "\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD"
      "\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD"
      "\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD" },
    { "\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD"
      "\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD"
      "\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD"
      "\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD"
      "\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD" },
    { "Test With Truncation" },
    { "Test Using Larger Than Block-Size Key - Hash Key First" },
    { "Test Using Larger Than Block-Size Key and Larger"
      " Than One Block-Size Data" }
};

static const int md5_hmac_test_buflen[7] =
{
    8, 28, 50, 50, 20, 54, 73
};

static const unsigned char md5_hmac_test_sum[7][16] =
{
    { 0x92, 0x94, 0x72, 0x7A, 0x36, 0x38, 0xBB, 0x1C,
      0x13, 0xF4, 0x8E, 0xF8, 0x15, 0x8B, 0xFC, 0x9D },
    { 0x75, 0x0C, 0x78, 0x3E, 0x6A, 0xB0, 0xB5, 0x03,
      0xEA, 0xA8, 0x6E, 0x31, 0x0A, 0x5D, 0xB7, 0x38 },
    { 0x56, 0xBE, 0x34, 0x52, 0x1D, 0x14, 0x4C, 0x88,
      0xDB, 0xB8, 0xC7, 0x33, 0xF0, 0xE8, 0xB3, 0xF6 },
    { 0x69, 0x7E, 0xAF, 0x0A, 0xCA, 0x3A, 0x3A, 0xEA,
      0x3A, 0x75, 0x16, 0x47, 0x46, 0xFF, 0xAA, 0x79 },
    { 0x56, 0x46, 0x1E, 0xF2, 0x34, 0x2E, 0xDC, 0x00,
      0xF9, 0xBA, 0xB9, 0x95 },
    { 0x6B, 0x1A, 0xB7, 0xFE, 0x4B, 0xD7, 0xBF, 0x8F,
      0x0B, 0x62, 0xE6, 0xCE, 0x61, 0xB9, 0xD0, 0xCD },
    { 0x6F, 0x63, 0x0F, 0xAD, 0x67, 0xCD, 0xA0, 0xEE,
      0x1F, 0xB1, 0xF5, 0x62, 0xDB, 0x3A, 0xA5, 0x3E }
};

/*
 * Checkup routine
 */
int md5_self_test( int verbose )
{
    int i, buflen;
    unsigned char buf[1024];
    unsigned char md5sum[16];
    md5_context ctx;

    for( i = 0; i < 7; i++ )
    {
        if( verbose != 0 )
            polarssl_printf( "  MD5 test #%d: ", i + 1 );

        md5( md5_test_buf[i], md5_test_buflen[i], md5sum );

        if( memcmp( md5sum, md5_test_sum[i], 16 ) != 0 )
        {
            if( verbose != 0 )
                polarssl_printf( "failed\n" );

            return( 1 );
        }

        if( verbose != 0 )
            polarssl_printf( "passed\n" );
    }

    if( verbose != 0 )
        polarssl_printf( "\n" );

    for( i = 0; i < 7; i++ )
    {
        if( verbose != 0 )
            polarssl_printf( "  HMAC-MD5 test #%d: ", i + 1 );

        if( i == 5 || i == 6 )
        {
            memset( buf, '\xAA', buflen = 80 );
            md5_hmac_starts( &ctx, buf, buflen );
        }
        else
            md5_hmac_starts( &ctx, md5_hmac_test_key[i],
                                   md5_hmac_test_keylen[i] );

        md5_hmac_update( &ctx, md5_hmac_test_buf[i],
                               md5_hmac_test_buflen[i] );

        md5_hmac_finish( &ctx, md5sum );

        buflen = ( i == 4 ) ? 12 : 16;

        if( memcmp( md5sum, md5_hmac_test_sum[i], buflen ) != 0 )
        {
            if( verbose != 0 )
                polarssl_printf( "failed\n" );

            return( 1 );
        }

        if( verbose != 0 )
            polarssl_printf( "passed\n" );
    }

    if( verbose != 0 )
        polarssl_printf( "\n" );

    return( 0 );
}

#endif /* POLARSSL_SELF_TEST */

#if defined(RSA_POLARSSL_SELF_TEST)

#include "polarssl/rsa.h"
#include "polarssl/oid.h"
#include "polarssl/sha1.h"

/*
 * Example RSA-1024 keypair, for test purposes
 */
#define KEY_LEN 128

#define RSA_N   "9292758453063D803DD603D5E777D788" \
                "8ED1D5BF35786190FA2F23EBC0848AEA" \
                "DDA92CA6C3D80B32C4D109BE0F36D6AE" \
                "7130B9CED7ACDF54CFC7555AC14EEBAB" \
                "93A89813FBF3C4F8066D2D800F7C38A8" \
                "1AE31942917403FF4946B0A83D3D3E05" \
                "EE57C6F5F5606FB5D4BC6CD34EE0801A" \
                "5E94BB77B07507233A0BC7BAC8F90F79"

#define RSA_E   "10001"

#define RSA_D   "24BF6185468786FDD303083D25E64EFC" \
                "66CA472BC44D253102F8B4A9D3BFA750" \
                "91386C0077937FE33FA3252D28855837" \
                "AE1B484A8A9A45F7EE8C0C634F99E8CD" \
                "DF79C5CE07EE72C7F123142198164234" \
                "CABB724CF78B8173B9F880FC86322407" \
                "AF1FEDFDDE2BEB674CA15F3E81A1521E" \
                "071513A1E85B5DFA031F21ECAE91A34D"

#define RSA_P   "C36D0EB7FCD285223CFB5AABA5BDA3D8" \
                "2C01CAD19EA484A87EA4377637E75500" \
                "FCB2005C5C7DD6EC4AC023CDA285D796" \
                "C3D9E75E1EFC42488BB4F1D13AC30A57"

#define RSA_Q   "C000DF51A7C77AE8D7C7370C1FF55B69" \
                "E211C2B9E5DB1ED0BF61D0D9899620F4" \
                "910E4168387E3C30AA1E00C339A79508" \
                "8452DD96A9A5EA5D9DCA68DA636032AF"

#define RSA_DP  "C1ACF567564274FB07A0BBAD5D26E298" \
                "3C94D22288ACD763FD8E5600ED4A702D" \
                "F84198A5F06C2E72236AE490C93F07F8" \
                "3CC559CD27BC2D1CA488811730BB5725"

#define RSA_DQ  "4959CBF6F8FEF750AEE6977C155579C7" \
                "D8AAEA56749EA28623272E4F7D0592AF" \
                "7C1F1313CAC9471B5C523BFE592F517B" \
                "407A1BD76C164B93DA2D32A383E58357"

#define RSA_QP  "9AE7FBC99546432DF71896FC239EADAE" \
                "F38D18D2B2F0E2DD275AA977E2BF4411" \
                "F5A3B2A5D33605AEBBCCBA7FEB9F2D2F" \
                "A74206CEC169D74BF5A8C50D6F48EA08"

#define PT_LEN  24
#define RSA_PT  "\xAA\xBB\xCC\x03\x02\x01\x00\xFF\xFF\xFF\xFF\xFF" \
                "\x11\x22\x33\x0A\x0B\x0C\xCC\xDD\xDD\xDD\xDD\xDD"

#if defined(POLARSSL_PKCS1_V15)
static int myrand( void *rng_state, unsigned char *output, size_t len )
{
#if !defined(__OpenBSD__)
    size_t i;

    if( rng_state != NULL )
        rng_state  = NULL;

    for( i = 0; i < len; ++i )
        output[i] = Rand();
#else
    if( rng_state != NULL )
        rng_state = NULL;

    arc4random_buf( output, len );
#endif /* !OpenBSD */

    return( 0 );
}
#endif /* POLARSSL_PKCS1_V15 */

/*
 * Checkup routine
 */
int rsa_self_test( int verbose )
{
    int ret = 0;
#if defined(POLARSSL_PKCS1_V15)
    size_t len;
    rsa_context rsa;
    unsigned char rsa_plaintext[PT_LEN];
    unsigned char rsa_decrypted[PT_LEN];
    unsigned char rsa_ciphertext[KEY_LEN];
#if defined(POLARSSL_SHA1_C)
    unsigned char sha1sum[20];
#endif

    rsa_init( &rsa, RSA_PKCS_V15, 0 );

    rsa.len = KEY_LEN;
    MPI_CHK( mpi_read_string( &rsa.N , 16, RSA_N  ) );
    MPI_CHK( mpi_read_string( &rsa.E , 16, RSA_E  ) );
    MPI_CHK( mpi_read_string( &rsa.D , 16, RSA_D  ) );
    MPI_CHK( mpi_read_string( &rsa.P , 16, RSA_P  ) );
    MPI_CHK( mpi_read_string( &rsa.Q , 16, RSA_Q  ) );
    MPI_CHK( mpi_read_string( &rsa.DP, 16, RSA_DP ) );
    MPI_CHK( mpi_read_string( &rsa.DQ, 16, RSA_DQ ) );
    MPI_CHK( mpi_read_string( &rsa.QP, 16, RSA_QP ) );

    if( verbose != 0 )
        polarssl_printf( "  RSA key validation: " );

    if( rsa_check_pubkey(  &rsa ) != 0 ||
        rsa_check_privkey( &rsa ) != 0 )
    {
        if( verbose != 0 )
            polarssl_printf( "failed\n" );

        return( 1 );
    }

    if( verbose != 0 )
        polarssl_printf( "passed\n  PKCS#1 encryption : " );

    memcpy( rsa_plaintext, RSA_PT, PT_LEN );

    if( rsa_pkcs1_encrypt( &rsa, myrand, NULL, RSA_PUBLIC, PT_LEN,
                           rsa_plaintext, rsa_ciphertext ) != 0 )
    {
        if( verbose != 0 )
            polarssl_printf( "failed\n" );

        return( 1 );
    }

    if( verbose != 0 )
        polarssl_printf( "passed\n  PKCS#1 decryption : " );

    if( rsa_pkcs1_decrypt( &rsa, myrand, NULL, RSA_PRIVATE, &len,
                           rsa_ciphertext, rsa_decrypted,
                           sizeof(rsa_decrypted) ) != 0 )
    {
        if( verbose != 0 )
            polarssl_printf( "failed\n" );

        return( 1 );
    }

    if( memcmp( rsa_decrypted, rsa_plaintext, len ) != 0 )
    {
        if( verbose != 0 )
            polarssl_printf( "failed\n" );

        return( 1 );
    }

#if defined(POLARSSL_SHA1_C)
    if( verbose != 0 )
        polarssl_printf( "passed\n  PKCS#1 data sign  : " );

    sha1( rsa_plaintext, PT_LEN, sha1sum );

    if( rsa_pkcs1_sign( &rsa, myrand, NULL, RSA_PRIVATE, POLARSSL_MD_SHA1, 0,
                        sha1sum, rsa_ciphertext ) != 0 )
    {
        if( verbose != 0 )
            polarssl_printf( "failed\n" );

        return( 1 );
    }

    if( verbose != 0 )
        polarssl_printf( "passed\n  PKCS#1 sig. verify: " );

    if( rsa_pkcs1_verify( &rsa, NULL, NULL, RSA_PUBLIC, POLARSSL_MD_SHA1, 0,
                          sha1sum, rsa_ciphertext ) != 0 )
    {
        if( verbose != 0 )
            polarssl_printf( "failed\n" );

        return( 1 );
    }

    if( verbose != 0 )
        polarssl_printf( "passed\n\n" );
#endif /* POLARSSL_SHA1_C */

cleanup:
    rsa_free( &rsa );
#else /* POLARSSL_PKCS1_V15 */
    ((void) verbose);
#endif /* POLARSSL_PKCS1_V15 */
    return( ret );
}

#endif /* POLARSSL_SELF_TEST */

#if defined(SHA1_POLARSSL_SELF_TEST)

#include "polarssl/sha1.h"

/*
 * FIPS-180-1 test vectors
 */
static unsigned char sha1_test_buf[3][57] =
{
    { "abc" },
    { "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq" },
    { "" }
};

static const int sha1_test_buflen[3] =
{
    3, 56, 1000
};

static const unsigned char sha1_test_sum[3][20] =
{
    { 0xA9, 0x99, 0x3E, 0x36, 0x47, 0x06, 0x81, 0x6A, 0xBA, 0x3E,
      0x25, 0x71, 0x78, 0x50, 0xC2, 0x6C, 0x9C, 0xD0, 0xD8, 0x9D },
    { 0x84, 0x98, 0x3E, 0x44, 0x1C, 0x3B, 0xD2, 0x6E, 0xBA, 0xAE,
      0x4A, 0xA1, 0xF9, 0x51, 0x29, 0xE5, 0xE5, 0x46, 0x70, 0xF1 },
    { 0x34, 0xAA, 0x97, 0x3C, 0xD4, 0xC4, 0xDA, 0xA4, 0xF6, 0x1E,
      0xEB, 0x2B, 0xDB, 0xAD, 0x27, 0x31, 0x65, 0x34, 0x01, 0x6F }
};

/*
 * RFC 2202 test vectors
 */
static unsigned char sha1_hmac_test_key[7][26] =
{
    { "\x0B\x0B\x0B\x0B\x0B\x0B\x0B\x0B\x0B\x0B\x0B\x0B\x0B\x0B\x0B\x0B"
      "\x0B\x0B\x0B\x0B" },
    { "Jefe" },
    { "\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA"
      "\xAA\xAA\xAA\xAA" },
    { "\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A\x0B\x0C\x0D\x0E\x0F\x10"
      "\x11\x12\x13\x14\x15\x16\x17\x18\x19" },
    { "\x0C\x0C\x0C\x0C\x0C\x0C\x0C\x0C\x0C\x0C\x0C\x0C\x0C\x0C\x0C\x0C"
      "\x0C\x0C\x0C\x0C" },
    { "" }, /* 0xAA 80 times */
    { "" }
};

static const int sha1_hmac_test_keylen[7] =
{
    20, 4, 20, 25, 20, 80, 80
};

static unsigned char sha1_hmac_test_buf[7][74] =
{
    { "Hi There" },
    { "what do ya want for nothing?" },
    { "\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD"
      "\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD"
      "\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD"
      "\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD"
      "\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD" },
    { "\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD"
      "\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD"
      "\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD"
      "\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD"
      "\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD" },
    { "Test With Truncation" },
    { "Test Using Larger Than Block-Size Key - Hash Key First" },
    { "Test Using Larger Than Block-Size Key and Larger"
      " Than One Block-Size Data" }
};

static const int sha1_hmac_test_buflen[7] =
{
    8, 28, 50, 50, 20, 54, 73
};

static const unsigned char sha1_hmac_test_sum[7][20] =
{
    { 0xB6, 0x17, 0x31, 0x86, 0x55, 0x05, 0x72, 0x64, 0xE2, 0x8B,
      0xC0, 0xB6, 0xFB, 0x37, 0x8C, 0x8E, 0xF1, 0x46, 0xBE, 0x00 },
    { 0xEF, 0xFC, 0xDF, 0x6A, 0xE5, 0xEB, 0x2F, 0xA2, 0xD2, 0x74,
      0x16, 0xD5, 0xF1, 0x84, 0xDF, 0x9C, 0x25, 0x9A, 0x7C, 0x79 },
    { 0x12, 0x5D, 0x73, 0x42, 0xB9, 0xAC, 0x11, 0xCD, 0x91, 0xA3,
      0x9A, 0xF4, 0x8A, 0xA1, 0x7B, 0x4F, 0x63, 0xF1, 0x75, 0xD3 },
    { 0x4C, 0x90, 0x07, 0xF4, 0x02, 0x62, 0x50, 0xC6, 0xBC, 0x84,
      0x14, 0xF9, 0xBF, 0x50, 0xC8, 0x6C, 0x2D, 0x72, 0x35, 0xDA },
    { 0x4C, 0x1A, 0x03, 0x42, 0x4B, 0x55, 0xE0, 0x7F, 0xE7, 0xF2,
      0x7B, 0xE1 },
    { 0xAA, 0x4A, 0xE5, 0xE1, 0x52, 0x72, 0xD0, 0x0E, 0x95, 0x70,
      0x56, 0x37, 0xCE, 0x8A, 0x3B, 0x55, 0xED, 0x40, 0x21, 0x12 },
    { 0xE8, 0xE9, 0x9D, 0x0F, 0x45, 0x23, 0x7D, 0x78, 0x6D, 0x6B,
      0xBA, 0xA7, 0x96, 0x5C, 0x78, 0x08, 0xBB, 0xFF, 0x1A, 0x91 }
};

/*
 * Checkup routine
 */
int sha1_self_test( int verbose )
{
    int i, j, buflen, ret = 0;
    unsigned char buf[1024];
    unsigned char sha1sum[20];
    sha1_context ctx;

    sha1_init( &ctx );

    /*
     * SHA-1
     */
    for( i = 0; i < 3; i++ )
    {
        if( verbose != 0 )
            polarssl_printf( "  SHA-1 test #%d: ", i + 1 );

        sha1_starts( &ctx );

        if( i == 2 )
        {
            memset( buf, 'a', buflen = 1000 );

            for( j = 0; j < 1000; j++ )
                sha1_update( &ctx, buf, buflen );
        }
        else
            sha1_update( &ctx, sha1_test_buf[i],
                               sha1_test_buflen[i] );

        sha1_finish( &ctx, sha1sum );

        if( memcmp( sha1sum, sha1_test_sum[i], 20 ) != 0 )
        {
            if( verbose != 0 )
                polarssl_printf( "failed\n" );

            ret = 1;
            goto exit;
        }

        if( verbose != 0 )
            polarssl_printf( "passed\n" );
    }

    if( verbose != 0 )
        polarssl_printf( "\n" );

    for( i = 0; i < 7; i++ )
    {
        if( verbose != 0 )
            polarssl_printf( "  HMAC-SHA-1 test #%d: ", i + 1 );

        if( i == 5 || i == 6 )
        {
            memset( buf, '\xAA', buflen = 80 );
            sha1_hmac_starts( &ctx, buf, buflen );
        }
        else
            sha1_hmac_starts( &ctx, sha1_hmac_test_key[i],
                                    sha1_hmac_test_keylen[i] );

        sha1_hmac_update( &ctx, sha1_hmac_test_buf[i],
                                sha1_hmac_test_buflen[i] );

        sha1_hmac_finish( &ctx, sha1sum );

        buflen = ( i == 4 ) ? 12 : 20;

        if( memcmp( sha1sum, sha1_hmac_test_sum[i], buflen ) != 0 )
        {
            if( verbose != 0 )
                polarssl_printf( "failed\n" );

            ret = 1;
            goto exit;
        }

        if( verbose != 0 )
            polarssl_printf( "passed\n" );
    }

    if( verbose != 0 )
        polarssl_printf( "\n" );

exit:
    sha1_free( &ctx );

    return( ret );
}

#endif /* POLARSSL_SELF_TEST */

#if defined(SHA256_POLARSSL_SELF_TEST)

#include "polarssl/sha256.h"

/*
 * FIPS-180-2 test vectors
 */
static unsigned char sha256_test_buf[3][57] =
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
     * SHA-224 test vectors
     */
    { 0x23, 0x09, 0x7D, 0x22, 0x34, 0x05, 0xD8, 0x22,
      0x86, 0x42, 0xA4, 0x77, 0xBD, 0xA2, 0x55, 0xB3,
      0x2A, 0xAD, 0xBC, 0xE4, 0xBD, 0xA0, 0xB3, 0xF7,
      0xE3, 0x6C, 0x9D, 0xA7 },
    { 0x75, 0x38, 0x8B, 0x16, 0x51, 0x27, 0x76, 0xCC,
      0x5D, 0xBA, 0x5D, 0xA1, 0xFD, 0x89, 0x01, 0x50,
      0xB0, 0xC6, 0x45, 0x5C, 0xB4, 0xF5, 0x8B, 0x19,
      0x52, 0x52, 0x25, 0x25 },
    { 0x20, 0x79, 0x46, 0x55, 0x98, 0x0C, 0x91, 0xD8,
      0xBB, 0xB4, 0xC1, 0xEA, 0x97, 0x61, 0x8A, 0x4B,
      0xF0, 0x3F, 0x42, 0x58, 0x19, 0x48, 0xB2, 0xEE,
      0x4E, 0xE7, 0xAD, 0x67 },

    /*
     * SHA-256 test vectors
     */
    { 0xBA, 0x78, 0x16, 0xBF, 0x8F, 0x01, 0xCF, 0xEA,
      0x41, 0x41, 0x40, 0xDE, 0x5D, 0xAE, 0x22, 0x23,
      0xB0, 0x03, 0x61, 0xA3, 0x96, 0x17, 0x7A, 0x9C,
      0xB4, 0x10, 0xFF, 0x61, 0xF2, 0x00, 0x15, 0xAD },
    { 0x24, 0x8D, 0x6A, 0x61, 0xD2, 0x06, 0x38, 0xB8,
      0xE5, 0xC0, 0x26, 0x93, 0x0C, 0x3E, 0x60, 0x39,
      0xA3, 0x3C, 0xE4, 0x59, 0x64, 0xFF, 0x21, 0x67,
      0xF6, 0xEC, 0xED, 0xD4, 0x19, 0xDB, 0x06, 0xC1 },
    { 0xCD, 0xC7, 0x6E, 0x5C, 0x99, 0x14, 0xFB, 0x92,
      0x81, 0xA1, 0xC7, 0xE2, 0x84, 0xD7, 0x3E, 0x67,
      0xF1, 0x80, 0x9A, 0x48, 0xA4, 0x97, 0x20, 0x0E,
      0x04, 0x6D, 0x39, 0xCC, 0xC7, 0x11, 0x2C, 0xD0 }
};

/*
 * RFC 4231 test vectors
 */
static unsigned char sha256_hmac_test_key[7][26] =
{
    { "\x0B\x0B\x0B\x0B\x0B\x0B\x0B\x0B\x0B\x0B\x0B\x0B\x0B\x0B\x0B\x0B"
      "\x0B\x0B\x0B\x0B" },
    { "Jefe" },
    { "\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA"
      "\xAA\xAA\xAA\xAA" },
    { "\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A\x0B\x0C\x0D\x0E\x0F\x10"
      "\x11\x12\x13\x14\x15\x16\x17\x18\x19" },
    { "\x0C\x0C\x0C\x0C\x0C\x0C\x0C\x0C\x0C\x0C\x0C\x0C\x0C\x0C\x0C\x0C"
      "\x0C\x0C\x0C\x0C" },
    { "" }, /* 0xAA 131 times */
    { "" }
};

static const int sha256_hmac_test_keylen[7] =
{
    20, 4, 20, 25, 20, 131, 131
};

static unsigned char sha256_hmac_test_buf[7][153] =
{
    { "Hi There" },
    { "what do ya want for nothing?" },
    { "\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD"
      "\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD"
      "\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD"
      "\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD"
      "\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD" },
    { "\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD"
      "\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD"
      "\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD"
      "\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD"
      "\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD" },
    { "Test With Truncation" },
    { "Test Using Larger Than Block-Size Key - Hash Key First" },
    { "This is a test using a larger than block-size key "
      "and a larger than block-size data. The key needs to "
      "be hashed before being used by the HMAC algorithm." }
};

static const int sha256_hmac_test_buflen[7] =
{
    8, 28, 50, 50, 20, 54, 152
};

static const unsigned char sha256_hmac_test_sum[14][32] =
{
    /*
     * HMAC-SHA-224 test vectors
     */
    { 0x89, 0x6F, 0xB1, 0x12, 0x8A, 0xBB, 0xDF, 0x19,
      0x68, 0x32, 0x10, 0x7C, 0xD4, 0x9D, 0xF3, 0x3F,
      0x47, 0xB4, 0xB1, 0x16, 0x99, 0x12, 0xBA, 0x4F,
      0x53, 0x68, 0x4B, 0x22 },
    { 0xA3, 0x0E, 0x01, 0x09, 0x8B, 0xC6, 0xDB, 0xBF,
      0x45, 0x69, 0x0F, 0x3A, 0x7E, 0x9E, 0x6D, 0x0F,
      0x8B, 0xBE, 0xA2, 0xA3, 0x9E, 0x61, 0x48, 0x00,
      0x8F, 0xD0, 0x5E, 0x44 },
    { 0x7F, 0xB3, 0xCB, 0x35, 0x88, 0xC6, 0xC1, 0xF6,
      0xFF, 0xA9, 0x69, 0x4D, 0x7D, 0x6A, 0xD2, 0x64,
      0x93, 0x65, 0xB0, 0xC1, 0xF6, 0x5D, 0x69, 0xD1,
      0xEC, 0x83, 0x33, 0xEA },
    { 0x6C, 0x11, 0x50, 0x68, 0x74, 0x01, 0x3C, 0xAC,
      0x6A, 0x2A, 0xBC, 0x1B, 0xB3, 0x82, 0x62, 0x7C,
      0xEC, 0x6A, 0x90, 0xD8, 0x6E, 0xFC, 0x01, 0x2D,
      0xE7, 0xAF, 0xEC, 0x5A },
    { 0x0E, 0x2A, 0xEA, 0x68, 0xA9, 0x0C, 0x8D, 0x37,
      0xC9, 0x88, 0xBC, 0xDB, 0x9F, 0xCA, 0x6F, 0xA8 },
    { 0x95, 0xE9, 0xA0, 0xDB, 0x96, 0x20, 0x95, 0xAD,
      0xAE, 0xBE, 0x9B, 0x2D, 0x6F, 0x0D, 0xBC, 0xE2,
      0xD4, 0x99, 0xF1, 0x12, 0xF2, 0xD2, 0xB7, 0x27,
      0x3F, 0xA6, 0x87, 0x0E },
    { 0x3A, 0x85, 0x41, 0x66, 0xAC, 0x5D, 0x9F, 0x02,
      0x3F, 0x54, 0xD5, 0x17, 0xD0, 0xB3, 0x9D, 0xBD,
      0x94, 0x67, 0x70, 0xDB, 0x9C, 0x2B, 0x95, 0xC9,
      0xF6, 0xF5, 0x65, 0xD1 },

    /*
     * HMAC-SHA-256 test vectors
     */
    { 0xB0, 0x34, 0x4C, 0x61, 0xD8, 0xDB, 0x38, 0x53,
      0x5C, 0xA8, 0xAF, 0xCE, 0xAF, 0x0B, 0xF1, 0x2B,
      0x88, 0x1D, 0xC2, 0x00, 0xC9, 0x83, 0x3D, 0xA7,
      0x26, 0xE9, 0x37, 0x6C, 0x2E, 0x32, 0xCF, 0xF7 },
    { 0x5B, 0xDC, 0xC1, 0x46, 0xBF, 0x60, 0x75, 0x4E,
      0x6A, 0x04, 0x24, 0x26, 0x08, 0x95, 0x75, 0xC7,
      0x5A, 0x00, 0x3F, 0x08, 0x9D, 0x27, 0x39, 0x83,
      0x9D, 0xEC, 0x58, 0xB9, 0x64, 0xEC, 0x38, 0x43 },
    { 0x77, 0x3E, 0xA9, 0x1E, 0x36, 0x80, 0x0E, 0x46,
      0x85, 0x4D, 0xB8, 0xEB, 0xD0, 0x91, 0x81, 0xA7,
      0x29, 0x59, 0x09, 0x8B, 0x3E, 0xF8, 0xC1, 0x22,
      0xD9, 0x63, 0x55, 0x14, 0xCE, 0xD5, 0x65, 0xFE },
    { 0x82, 0x55, 0x8A, 0x38, 0x9A, 0x44, 0x3C, 0x0E,
      0xA4, 0xCC, 0x81, 0x98, 0x99, 0xF2, 0x08, 0x3A,
      0x85, 0xF0, 0xFA, 0xA3, 0xE5, 0x78, 0xF8, 0x07,
      0x7A, 0x2E, 0x3F, 0xF4, 0x67, 0x29, 0x66, 0x5B },
    { 0xA3, 0xB6, 0x16, 0x74, 0x73, 0x10, 0x0E, 0xE0,
      0x6E, 0x0C, 0x79, 0x6C, 0x29, 0x55, 0x55, 0x2B },
    { 0x60, 0xE4, 0x31, 0x59, 0x1E, 0xE0, 0xB6, 0x7F,
      0x0D, 0x8A, 0x26, 0xAA, 0xCB, 0xF5, 0xB7, 0x7F,
      0x8E, 0x0B, 0xC6, 0x21, 0x37, 0x28, 0xC5, 0x14,
      0x05, 0x46, 0x04, 0x0F, 0x0E, 0xE3, 0x7F, 0x54 },
    { 0x9B, 0x09, 0xFF, 0xA7, 0x1B, 0x94, 0x2F, 0xCB,
      0x27, 0x63, 0x5F, 0xBC, 0xD5, 0xB0, 0xE9, 0x44,
      0xBF, 0xDC, 0x63, 0x64, 0x4F, 0x07, 0x13, 0x93,
      0x8A, 0x7F, 0x51, 0x53, 0x5C, 0x3A, 0x35, 0xE2 }
};

/*
 * Checkup routine
 */
int sha256_self_test( int verbose )
{
    int i, j, k, buflen, ret = 0;
    unsigned char buf[1024];
    unsigned char sha256sum[32];
    sha256_context ctx;

    sha256_init( &ctx );

    for( i = 0; i < 6; i++ )
    {
        j = i % 3;
        k = i < 3;

        if( verbose != 0 )
            polarssl_printf( "  SHA-%d test #%d: ", 256 - k * 32, j + 1 );

        sha256_starts( &ctx, k );

        if( j == 2 )
        {
            memset( buf, 'a', buflen = 1000 );

            for( j = 0; j < 1000; j++ )
                sha256_update( &ctx, buf, buflen );
        }
        else
            sha256_update( &ctx, sha256_test_buf[j],
                                 sha256_test_buflen[j] );

        sha256_finish( &ctx, sha256sum );

        if( memcmp( sha256sum, sha256_test_sum[i], 32 - k * 4 ) != 0 )
        {
            if( verbose != 0 )
                polarssl_printf( "failed\n" );

            ret = 1;
            goto exit;
        }

        if( verbose != 0 )
            polarssl_printf( "passed\n" );
    }

    if( verbose != 0 )
        polarssl_printf( "\n" );

    for( i = 0; i < 14; i++ )
    {
        j = i % 7;
        k = i < 7;

        if( verbose != 0 )
            polarssl_printf( "  HMAC-SHA-%d test #%d: ", 256 - k * 32, j + 1 );

        if( j == 5 || j == 6 )
        {
            memset( buf, '\xAA', buflen = 131 );
            sha256_hmac_starts( &ctx, buf, buflen, k );
        }
        else
            sha256_hmac_starts( &ctx, sha256_hmac_test_key[j],
                                      sha256_hmac_test_keylen[j], k );

        sha256_hmac_update( &ctx, sha256_hmac_test_buf[j],
                                  sha256_hmac_test_buflen[j] );

        sha256_hmac_finish( &ctx, sha256sum );

        buflen = ( j == 4 ) ? 16 : 32 - k * 4;

        if( memcmp( sha256sum, sha256_hmac_test_sum[i], buflen ) != 0 )
        {
            if( verbose != 0 )
                polarssl_printf( "failed\n" );

            ret = 1;
            goto exit;
        }

        if( verbose != 0 )
            polarssl_printf( "passed\n" );
    }

    if( verbose != 0 )
        polarssl_printf( "\n" );

exit:
    sha256_free( &ctx );

    return( ret );
}

#endif /* POLARSSL_SELF_TEST */

#if defined(SHA512_POLARSSL_SELF_TEST)

#include "polarssl/sha512.h"

/*
 * FIPS-180-2 test vectors
 */
static unsigned char sha512_test_buf[3][113] =
{
    { "abc" },
    { "abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmn"
      "hijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu" },
    { "" }
};

static const int sha512_test_buflen[3] =
{
    3, 112, 1000
};

static const unsigned char sha512_test_sum[6][64] =
{
    /*
     * SHA-384 test vectors
     */
    { 0xCB, 0x00, 0x75, 0x3F, 0x45, 0xA3, 0x5E, 0x8B,
      0xB5, 0xA0, 0x3D, 0x69, 0x9A, 0xC6, 0x50, 0x07,
      0x27, 0x2C, 0x32, 0xAB, 0x0E, 0xDE, 0xD1, 0x63,
      0x1A, 0x8B, 0x60, 0x5A, 0x43, 0xFF, 0x5B, 0xED,
      0x80, 0x86, 0x07, 0x2B, 0xA1, 0xE7, 0xCC, 0x23,
      0x58, 0xBA, 0xEC, 0xA1, 0x34, 0xC8, 0x25, 0xA7 },
    { 0x09, 0x33, 0x0C, 0x33, 0xF7, 0x11, 0x47, 0xE8,
      0x3D, 0x19, 0x2F, 0xC7, 0x82, 0xCD, 0x1B, 0x47,
      0x53, 0x11, 0x1B, 0x17, 0x3B, 0x3B, 0x05, 0xD2,
      0x2F, 0xA0, 0x80, 0x86, 0xE3, 0xB0, 0xF7, 0x12,
      0xFC, 0xC7, 0xC7, 0x1A, 0x55, 0x7E, 0x2D, 0xB9,
      0x66, 0xC3, 0xE9, 0xFA, 0x91, 0x74, 0x60, 0x39 },
    { 0x9D, 0x0E, 0x18, 0x09, 0x71, 0x64, 0x74, 0xCB,
      0x08, 0x6E, 0x83, 0x4E, 0x31, 0x0A, 0x4A, 0x1C,
      0xED, 0x14, 0x9E, 0x9C, 0x00, 0xF2, 0x48, 0x52,
      0x79, 0x72, 0xCE, 0xC5, 0x70, 0x4C, 0x2A, 0x5B,
      0x07, 0xB8, 0xB3, 0xDC, 0x38, 0xEC, 0xC4, 0xEB,
      0xAE, 0x97, 0xDD, 0xD8, 0x7F, 0x3D, 0x89, 0x85 },

    /*
     * SHA-512 test vectors
     */
    { 0xDD, 0xAF, 0x35, 0xA1, 0x93, 0x61, 0x7A, 0xBA,
      0xCC, 0x41, 0x73, 0x49, 0xAE, 0x20, 0x41, 0x31,
      0x12, 0xE6, 0xFA, 0x4E, 0x89, 0xA9, 0x7E, 0xA2,
      0x0A, 0x9E, 0xEE, 0xE6, 0x4B, 0x55, 0xD3, 0x9A,
      0x21, 0x92, 0x99, 0x2A, 0x27, 0x4F, 0xC1, 0xA8,
      0x36, 0xBA, 0x3C, 0x23, 0xA3, 0xFE, 0xEB, 0xBD,
      0x45, 0x4D, 0x44, 0x23, 0x64, 0x3C, 0xE8, 0x0E,
      0x2A, 0x9A, 0xC9, 0x4F, 0xA5, 0x4C, 0xA4, 0x9F },
    { 0x8E, 0x95, 0x9B, 0x75, 0xDA, 0xE3, 0x13, 0xDA,
      0x8C, 0xF4, 0xF7, 0x28, 0x14, 0xFC, 0x14, 0x3F,
      0x8F, 0x77, 0x79, 0xC6, 0xEB, 0x9F, 0x7F, 0xA1,
      0x72, 0x99, 0xAE, 0xAD, 0xB6, 0x88, 0x90, 0x18,
      0x50, 0x1D, 0x28, 0x9E, 0x49, 0x00, 0xF7, 0xE4,
      0x33, 0x1B, 0x99, 0xDE, 0xC4, 0xB5, 0x43, 0x3A,
      0xC7, 0xD3, 0x29, 0xEE, 0xB6, 0xDD, 0x26, 0x54,
      0x5E, 0x96, 0xE5, 0x5B, 0x87, 0x4B, 0xE9, 0x09 },
    { 0xE7, 0x18, 0x48, 0x3D, 0x0C, 0xE7, 0x69, 0x64,
      0x4E, 0x2E, 0x42, 0xC7, 0xBC, 0x15, 0xB4, 0x63,
      0x8E, 0x1F, 0x98, 0xB1, 0x3B, 0x20, 0x44, 0x28,
      0x56, 0x32, 0xA8, 0x03, 0xAF, 0xA9, 0x73, 0xEB,
      0xDE, 0x0F, 0xF2, 0x44, 0x87, 0x7E, 0xA6, 0x0A,
      0x4C, 0xB0, 0x43, 0x2C, 0xE5, 0x77, 0xC3, 0x1B,
      0xEB, 0x00, 0x9C, 0x5C, 0x2C, 0x49, 0xAA, 0x2E,
      0x4E, 0xAD, 0xB2, 0x17, 0xAD, 0x8C, 0xC0, 0x9B }
};

/*
 * RFC 4231 test vectors
 */
static unsigned char sha512_hmac_test_key[7][26] =
{
    { "\x0B\x0B\x0B\x0B\x0B\x0B\x0B\x0B\x0B\x0B\x0B\x0B\x0B\x0B\x0B\x0B"
      "\x0B\x0B\x0B\x0B" },
    { "Jefe" },
    { "\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA"
      "\xAA\xAA\xAA\xAA" },
    { "\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A\x0B\x0C\x0D\x0E\x0F\x10"
      "\x11\x12\x13\x14\x15\x16\x17\x18\x19" },
    { "\x0C\x0C\x0C\x0C\x0C\x0C\x0C\x0C\x0C\x0C\x0C\x0C\x0C\x0C\x0C\x0C"
      "\x0C\x0C\x0C\x0C" },
    { "" }, /* 0xAA 131 times */
    { "" }
};

static const int sha512_hmac_test_keylen[7] =
{
    20, 4, 20, 25, 20, 131, 131
};

static unsigned char sha512_hmac_test_buf[7][153] =
{
    { "Hi There" },
    { "what do ya want for nothing?" },
    { "\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD"
      "\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD"
      "\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD"
      "\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD"
      "\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD" },
    { "\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD"
      "\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD"
      "\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD"
      "\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD"
      "\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD" },
    { "Test With Truncation" },
    { "Test Using Larger Than Block-Size Key - Hash Key First" },
    { "This is a test using a larger than block-size key "
      "and a larger than block-size data. The key needs to "
      "be hashed before being used by the HMAC algorithm." }
};

static const int sha512_hmac_test_buflen[7] =
{
    8, 28, 50, 50, 20, 54, 152
};

static const unsigned char sha512_hmac_test_sum[14][64] =
{
    /*
     * HMAC-SHA-384 test vectors
     */
    { 0xAF, 0xD0, 0x39, 0x44, 0xD8, 0x48, 0x95, 0x62,
      0x6B, 0x08, 0x25, 0xF4, 0xAB, 0x46, 0x90, 0x7F,
      0x15, 0xF9, 0xDA, 0xDB, 0xE4, 0x10, 0x1E, 0xC6,
      0x82, 0xAA, 0x03, 0x4C, 0x7C, 0xEB, 0xC5, 0x9C,
      0xFA, 0xEA, 0x9E, 0xA9, 0x07, 0x6E, 0xDE, 0x7F,
      0x4A, 0xF1, 0x52, 0xE8, 0xB2, 0xFA, 0x9C, 0xB6 },
    { 0xAF, 0x45, 0xD2, 0xE3, 0x76, 0x48, 0x40, 0x31,
      0x61, 0x7F, 0x78, 0xD2, 0xB5, 0x8A, 0x6B, 0x1B,
      0x9C, 0x7E, 0xF4, 0x64, 0xF5, 0xA0, 0x1B, 0x47,
      0xE4, 0x2E, 0xC3, 0x73, 0x63, 0x22, 0x44, 0x5E,
      0x8E, 0x22, 0x40, 0xCA, 0x5E, 0x69, 0xE2, 0xC7,
      0x8B, 0x32, 0x39, 0xEC, 0xFA, 0xB2, 0x16, 0x49 },
    { 0x88, 0x06, 0x26, 0x08, 0xD3, 0xE6, 0xAD, 0x8A,
      0x0A, 0xA2, 0xAC, 0xE0, 0x14, 0xC8, 0xA8, 0x6F,
      0x0A, 0xA6, 0x35, 0xD9, 0x47, 0xAC, 0x9F, 0xEB,
      0xE8, 0x3E, 0xF4, 0xE5, 0x59, 0x66, 0x14, 0x4B,
      0x2A, 0x5A, 0xB3, 0x9D, 0xC1, 0x38, 0x14, 0xB9,
      0x4E, 0x3A, 0xB6, 0xE1, 0x01, 0xA3, 0x4F, 0x27 },
    { 0x3E, 0x8A, 0x69, 0xB7, 0x78, 0x3C, 0x25, 0x85,
      0x19, 0x33, 0xAB, 0x62, 0x90, 0xAF, 0x6C, 0xA7,
      0x7A, 0x99, 0x81, 0x48, 0x08, 0x50, 0x00, 0x9C,
      0xC5, 0x57, 0x7C, 0x6E, 0x1F, 0x57, 0x3B, 0x4E,
      0x68, 0x01, 0xDD, 0x23, 0xC4, 0xA7, 0xD6, 0x79,
      0xCC, 0xF8, 0xA3, 0x86, 0xC6, 0x74, 0xCF, 0xFB },
    { 0x3A, 0xBF, 0x34, 0xC3, 0x50, 0x3B, 0x2A, 0x23,
      0xA4, 0x6E, 0xFC, 0x61, 0x9B, 0xAE, 0xF8, 0x97 },
    { 0x4E, 0xCE, 0x08, 0x44, 0x85, 0x81, 0x3E, 0x90,
      0x88, 0xD2, 0xC6, 0x3A, 0x04, 0x1B, 0xC5, 0xB4,
      0x4F, 0x9E, 0xF1, 0x01, 0x2A, 0x2B, 0x58, 0x8F,
      0x3C, 0xD1, 0x1F, 0x05, 0x03, 0x3A, 0xC4, 0xC6,
      0x0C, 0x2E, 0xF6, 0xAB, 0x40, 0x30, 0xFE, 0x82,
      0x96, 0x24, 0x8D, 0xF1, 0x63, 0xF4, 0x49, 0x52 },
    { 0x66, 0x17, 0x17, 0x8E, 0x94, 0x1F, 0x02, 0x0D,
      0x35, 0x1E, 0x2F, 0x25, 0x4E, 0x8F, 0xD3, 0x2C,
      0x60, 0x24, 0x20, 0xFE, 0xB0, 0xB8, 0xFB, 0x9A,
      0xDC, 0xCE, 0xBB, 0x82, 0x46, 0x1E, 0x99, 0xC5,
      0xA6, 0x78, 0xCC, 0x31, 0xE7, 0x99, 0x17, 0x6D,
      0x38, 0x60, 0xE6, 0x11, 0x0C, 0x46, 0x52, 0x3E },

    /*
     * HMAC-SHA-512 test vectors
     */
    { 0x87, 0xAA, 0x7C, 0xDE, 0xA5, 0xEF, 0x61, 0x9D,
      0x4F, 0xF0, 0xB4, 0x24, 0x1A, 0x1D, 0x6C, 0xB0,
      0x23, 0x79, 0xF4, 0xE2, 0xCE, 0x4E, 0xC2, 0x78,
      0x7A, 0xD0, 0xB3, 0x05, 0x45, 0xE1, 0x7C, 0xDE,
      0xDA, 0xA8, 0x33, 0xB7, 0xD6, 0xB8, 0xA7, 0x02,
      0x03, 0x8B, 0x27, 0x4E, 0xAE, 0xA3, 0xF4, 0xE4,
      0xBE, 0x9D, 0x91, 0x4E, 0xEB, 0x61, 0xF1, 0x70,
      0x2E, 0x69, 0x6C, 0x20, 0x3A, 0x12, 0x68, 0x54 },
    { 0x16, 0x4B, 0x7A, 0x7B, 0xFC, 0xF8, 0x19, 0xE2,
      0xE3, 0x95, 0xFB, 0xE7, 0x3B, 0x56, 0xE0, 0xA3,
      0x87, 0xBD, 0x64, 0x22, 0x2E, 0x83, 0x1F, 0xD6,
      0x10, 0x27, 0x0C, 0xD7, 0xEA, 0x25, 0x05, 0x54,
      0x97, 0x58, 0xBF, 0x75, 0xC0, 0x5A, 0x99, 0x4A,
      0x6D, 0x03, 0x4F, 0x65, 0xF8, 0xF0, 0xE6, 0xFD,
      0xCA, 0xEA, 0xB1, 0xA3, 0x4D, 0x4A, 0x6B, 0x4B,
      0x63, 0x6E, 0x07, 0x0A, 0x38, 0xBC, 0xE7, 0x37 },
    { 0xFA, 0x73, 0xB0, 0x08, 0x9D, 0x56, 0xA2, 0x84,
      0xEF, 0xB0, 0xF0, 0x75, 0x6C, 0x89, 0x0B, 0xE9,
      0xB1, 0xB5, 0xDB, 0xDD, 0x8E, 0xE8, 0x1A, 0x36,
      0x55, 0xF8, 0x3E, 0x33, 0xB2, 0x27, 0x9D, 0x39,
      0xBF, 0x3E, 0x84, 0x82, 0x79, 0xA7, 0x22, 0xC8,
      0x06, 0xB4, 0x85, 0xA4, 0x7E, 0x67, 0xC8, 0x07,
      0xB9, 0x46, 0xA3, 0x37, 0xBE, 0xE8, 0x94, 0x26,
      0x74, 0x27, 0x88, 0x59, 0xE1, 0x32, 0x92, 0xFB },
    { 0xB0, 0xBA, 0x46, 0x56, 0x37, 0x45, 0x8C, 0x69,
      0x90, 0xE5, 0xA8, 0xC5, 0xF6, 0x1D, 0x4A, 0xF7,
      0xE5, 0x76, 0xD9, 0x7F, 0xF9, 0x4B, 0x87, 0x2D,
      0xE7, 0x6F, 0x80, 0x50, 0x36, 0x1E, 0xE3, 0xDB,
      0xA9, 0x1C, 0xA5, 0xC1, 0x1A, 0xA2, 0x5E, 0xB4,
      0xD6, 0x79, 0x27, 0x5C, 0xC5, 0x78, 0x80, 0x63,
      0xA5, 0xF1, 0x97, 0x41, 0x12, 0x0C, 0x4F, 0x2D,
      0xE2, 0xAD, 0xEB, 0xEB, 0x10, 0xA2, 0x98, 0xDD },
    { 0x41, 0x5F, 0xAD, 0x62, 0x71, 0x58, 0x0A, 0x53,
      0x1D, 0x41, 0x79, 0xBC, 0x89, 0x1D, 0x87, 0xA6 },
    { 0x80, 0xB2, 0x42, 0x63, 0xC7, 0xC1, 0xA3, 0xEB,
      0xB7, 0x14, 0x93, 0xC1, 0xDD, 0x7B, 0xE8, 0xB4,
      0x9B, 0x46, 0xD1, 0xF4, 0x1B, 0x4A, 0xEE, 0xC1,
      0x12, 0x1B, 0x01, 0x37, 0x83, 0xF8, 0xF3, 0x52,
      0x6B, 0x56, 0xD0, 0x37, 0xE0, 0x5F, 0x25, 0x98,
      0xBD, 0x0F, 0xD2, 0x21, 0x5D, 0x6A, 0x1E, 0x52,
      0x95, 0xE6, 0x4F, 0x73, 0xF6, 0x3F, 0x0A, 0xEC,
      0x8B, 0x91, 0x5A, 0x98, 0x5D, 0x78, 0x65, 0x98 },
    { 0xE3, 0x7B, 0x6A, 0x77, 0x5D, 0xC8, 0x7D, 0xBA,
      0xA4, 0xDF, 0xA9, 0xF9, 0x6E, 0x5E, 0x3F, 0xFD,
      0xDE, 0xBD, 0x71, 0xF8, 0x86, 0x72, 0x89, 0x86,
      0x5D, 0xF5, 0xA3, 0x2D, 0x20, 0xCD, 0xC9, 0x44,
      0xB6, 0x02, 0x2C, 0xAC, 0x3C, 0x49, 0x82, 0xB1,
      0x0D, 0x5E, 0xEB, 0x55, 0xC3, 0xE4, 0xDE, 0x15,
      0x13, 0x46, 0x76, 0xFB, 0x6D, 0xE0, 0x44, 0x60,
      0x65, 0xC9, 0x74, 0x40, 0xFA, 0x8C, 0x6A, 0x58 }
};

/*
 * Checkup routine
 */
int sha512_self_test( int verbose )
{
    int i, j, k, buflen, ret = 0;
    unsigned char buf[1024];
    unsigned char sha512sum[64];
    sha512_context ctx;

    sha512_init( &ctx );

    for( i = 0; i < 6; i++ )
    {
        j = i % 3;
        k = i < 3;

        if( verbose != 0 )
            polarssl_printf( "  SHA-%d test #%d: ", 512 - k * 128, j + 1 );

        sha512_starts( &ctx, k );

        if( j == 2 )
        {
            memset( buf, 'a', buflen = 1000 );

            for( j = 0; j < 1000; j++ )
                sha512_update( &ctx, buf, buflen );
        }
        else
            sha512_update( &ctx, sha512_test_buf[j],
                                 sha512_test_buflen[j] );

        sha512_finish( &ctx, sha512sum );

        if( memcmp( sha512sum, sha512_test_sum[i], 64 - k * 16 ) != 0 )
        {
            if( verbose != 0 )
                polarssl_printf( "failed\n" );

            ret = 1;
            goto exit;
        }

        if( verbose != 0 )
            polarssl_printf( "passed\n" );
    }

    if( verbose != 0 )
        polarssl_printf( "\n" );

    for( i = 0; i < 14; i++ )
    {
        j = i % 7;
        k = i < 7;

        if( verbose != 0 )
            polarssl_printf( "  HMAC-SHA-%d test #%d: ", 512 - k * 128, j + 1 );

        if( j == 5 || j == 6 )
        {
            memset( buf, '\xAA', buflen = 131 );
            sha512_hmac_starts( &ctx, buf, buflen, k );
        }
        else
            sha512_hmac_starts( &ctx, sha512_hmac_test_key[j],
                                      sha512_hmac_test_keylen[j], k );

        sha512_hmac_update( &ctx, sha512_hmac_test_buf[j],
                                  sha512_hmac_test_buflen[j] );

        sha512_hmac_finish( &ctx, sha512sum );

        buflen = ( j == 4 ) ? 16 : 64 - k * 16;

        if( memcmp( sha512sum, sha512_hmac_test_sum[i], buflen ) != 0 )
        {
            if( verbose != 0 )
                polarssl_printf( "failed\n" );

            ret = 1;
            goto exit;
        }

        if( verbose != 0 )
            polarssl_printf( "passed\n" );
    }

    if( verbose != 0 )
        polarssl_printf( "\n" );

exit:
    sha512_free( &ctx );

    return( ret );
}

#endif /* POLARSSL_SELF_TEST */

extern void *pvPortMalloc(unsigned int xWantedSize);
extern void vPortFree(void *pv);

int ssl_self_test( int use_hw_crypto )
{
	platform_set_malloc_free(pvPortMalloc, vPortFree);

	/* test SW or HW crypt */
	if (use_hw_crypto)
		rom_ssl_ram_map.use_hw_crypto_func = 1;
	else
		rom_ssl_ram_map.use_hw_crypto_func = 0;
	
	if (rom_ssl_ram_map.use_hw_crypto_func && rtl_cryptoEngine_init() != SUCCESS) {
		polarssl_printf("Use HW Crypto, but Crypto Engine Init fail!!!\n");
		return -1;
	}
	
	aes_self_test(1);
	arc4_self_test(1);
	base64_self_test(1);
	mpi_self_test(1);
	ctr_drbg_self_test(1); //bignum
	des_self_test(1);
	dhm_self_test(1);
#if defined(ECP_POLARSSL_SELF_TEST)
	//should open #define POLARSSL_SELF_TEST in ecp.c (rom)
	ecp_self_test_1(1);
#endif
	hmac_drbg_self_test(1);
	md5_self_test(1);
	rsa_self_test(1);
	sha1_self_test(1);
	sha256_self_test(1);
	sha512_self_test(1);
		
	return 0;
}
#endif
#endif //CONFIG_SSL_ROM_TEST
