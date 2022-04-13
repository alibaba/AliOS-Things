/* ****************************************************************************
 *                                                                          *
 * C-Sky Microsystems Confidential                                          *
 * -------------------------------                                          *
 * This file and all its contents are properties of C-Sky Microsystems. The *
 * information contained herein is confidential and proprietary and is not  *
 * to be disclosed outside of C-Sky Microsystems except under a             *
 * Non-Disclosured Agreement (NDA).                                         *
 *                                                                          *
 ****************************************************************************/

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "mbedtls/aes.h"
#include "aos/kernel.h"
#include "mbedtls/platform_util.h"
#if defined(CONFIG_TEE_AES)
#include "drv/tee.h"
#endif

#if defined(MBEDTLS_AES_ALT)

/* Parameter validation macros based on platform_util.h */
#define AES_VALIDATE_RET( cond )    \
    MBEDTLS_INTERNAL_VALIDATE_RET( cond, MBEDTLS_ERR_AES_BAD_INPUT_DATA )
#define AES_VALIDATE( cond )        \
    MBEDTLS_INTERNAL_VALIDATE( cond )

static void mbedtls_zeroize(void *v, size_t n)
{
    volatile unsigned char *p = (unsigned char *)v;

    while (n--) {
        *p++ = 0;
    }
}

void mbedtls_aes_init(mbedtls_aes_context *ctx)
{
    mbedtls_zeroize(ctx, sizeof(mbedtls_aes_context));
    sc_aes_init(&ctx->sc_ctx, 0);
}

void mbedtls_aes_free(mbedtls_aes_context *ctx)
{
    if (ctx == NULL) {
        return;
    }

    sc_aes_uninit(&ctx->sc_ctx);
    mbedtls_zeroize(ctx, sizeof(mbedtls_aes_context));
}

int mbedtls_aes_setkey_enc(mbedtls_aes_context *ctx, const unsigned char *key, unsigned int keybits)
{
    sc_aes_key_bits_t bits = SC_AES_KEY_LEN_BITS_128;
    if (keybits == 128) {
        bits = SC_AES_KEY_LEN_BITS_128;
    } else if (keybits == 192) {
        bits = SC_AES_KEY_LEN_BITS_192;
    } else if (keybits == 256) {
        bits = SC_AES_KEY_LEN_BITS_256;
    } else {
        return -1;
    }

#if defined(CONFIG_TEE_AES)
    memcpy(ctx->key, key, keybits / 8);
    ctx->key_len = keybits / 8;
#endif
    sc_aes_set_encrypt_key(&ctx->sc_ctx, (void *)key, bits);
    return (0);
}

/*
 * AES key schedule (decryption)
 */
int mbedtls_aes_setkey_dec(mbedtls_aes_context *ctx, const unsigned char *key, unsigned int keybits)
{
    sc_aes_key_bits_t bits = SC_AES_KEY_LEN_BITS_128;
    if (keybits == 128) {
        bits = SC_AES_KEY_LEN_BITS_128;
    } else if (keybits == 192) {
        bits = SC_AES_KEY_LEN_BITS_192;
    } else if (keybits == 256) {
        bits = SC_AES_KEY_LEN_BITS_256;
    } else {
        return -1;
    }

#if defined(CONFIG_TEE_AES)
    return mbedtls_aes_setkey_enc(ctx, key, keybits);
#endif
    sc_aes_set_decrypt_key(&ctx->sc_ctx, (void *)key, bits);
    return 0;
}

/*
 * AES-ECB block encryption/decryption
 */
int mbedtls_aes_crypt_ecb(mbedtls_aes_context *ctx, int mode, const unsigned char input[16],
                          unsigned char output[16])
{

    if (mode == MBEDTLS_AES_ENCRYPT) {
        sc_aes_ecb_encrypt(&ctx->sc_ctx, (void *)input, output, 16);
    } else if (mode == MBEDTLS_AES_DECRYPT) {
        sc_aes_ecb_decrypt(&ctx->sc_ctx, (void *)input, output, 16);
    }

#if defined(CONFIG_TEE_AES)
    if (mode == MBEDTLS_AES_ENCRYPT) {
         csi_tee_aes_encrypt_ecb(input, 16, ctx->key, ctx->key_len, output);
    } else if (mode == MBEDTLS_AES_DECRYPT) {
         csi_tee_aes_decrypt_ecb(input, 16, ctx->key, ctx->key_len, output);
    }
#endif
    return 0;
}
/*
 * AES-CBC buffer encryption/decryption
 */
int mbedtls_aes_crypt_cbc(mbedtls_aes_context *ctx, int mode, size_t length, unsigned char iv[16],
                          const unsigned char *input, unsigned char *output)
{
    int           i;
    unsigned char temp[16];
    if (mode == MBEDTLS_AES_DECRYPT) {
        while (length > 0) {
            memcpy(temp, input, 16);
            aos_kernel_sched_suspend();
            mbedtls_aes_crypt_ecb(ctx, mode, input, output);
            aos_kernel_sched_resume();
            for (i = 0; i < 16; i++)
                output[i] = (unsigned char)(output[i] ^ iv[i]);

            memcpy(iv, temp, 16);

            input += 16;
            output += 16;
            length -= 16;
        }
    } else {
        while (length > 0) {
            for (i = 0; i < 16; i++)
                output[i] = (unsigned char)(input[i] ^ iv[i]);

            mbedtls_aes_crypt_ecb(ctx, mode, output, output);
            memcpy(iv, output, 16);

            input += 16;
            output += 16;
            length -= 16;
        }
    }
    return (0);
}

#if defined(MBEDTLS_CIPHER_MODE_CTR)
/*
 * AES-CTR buffer encryption/decryption
 */
int mbedtls_aes_crypt_ctr(mbedtls_aes_context *ctx, size_t length, size_t *nc_off,
                          unsigned char nonce_counter[16], unsigned char stream_block[16],
                          const unsigned char *input, unsigned char *output)
{
    int    c, i;
    size_t n = *nc_off;

    while (length--) {
        if (n == 0) {
            mbedtls_aes_crypt_ecb(ctx, MBEDTLS_AES_ENCRYPT, nonce_counter, stream_block);

            for (i = 16; i > 0; i--)
                if (++nonce_counter[i - 1] != 0)
                    break;
        }
        c         = *input++;
        *output++ = (unsigned char)(c ^ stream_block[n]);

        n = (n + 1) & 0x0F;
    }

    *nc_off = n;

    return (0);
}
#endif /* MBEDTLS_CIPHER_MODE_CTR */


#if defined(MBEDTLS_CIPHER_MODE_CFB)
/*
 * AES-CFB128 buffer encryption/decryption
 */
int mbedtls_aes_crypt_cfb128( mbedtls_aes_context *ctx,
                       int mode,
                       size_t length,
                       size_t *iv_off,
                       unsigned char iv[16],
                       const unsigned char *input,
                       unsigned char *output )
{
    int c;
    size_t n;

    AES_VALIDATE_RET( ctx != NULL );
    AES_VALIDATE_RET( mode == MBEDTLS_AES_ENCRYPT ||
                      mode == MBEDTLS_AES_DECRYPT );
    AES_VALIDATE_RET( iv_off != NULL );
    AES_VALIDATE_RET( iv != NULL );
    AES_VALIDATE_RET( input != NULL );
    AES_VALIDATE_RET( output != NULL );

    n = *iv_off;

    if( n > 15 )
        return( MBEDTLS_ERR_AES_BAD_INPUT_DATA );

    if( mode == MBEDTLS_AES_DECRYPT )
    {
        while( length-- )
        {
            if( n == 0 )
                mbedtls_aes_crypt_ecb( ctx, MBEDTLS_AES_ENCRYPT, iv, iv );

            c = *input++;
            *output++ = (unsigned char)( c ^ iv[n] );
            iv[n] = (unsigned char) c;

            n = ( n + 1 ) & 0x0F;
        }
    }
    else
    {
        while( length-- )
        {
            if( n == 0 )
                mbedtls_aes_crypt_ecb( ctx, MBEDTLS_AES_ENCRYPT, iv, iv );

            iv[n] = *output++ = (unsigned char)( iv[n] ^ *input++ );

            n = ( n + 1 ) & 0x0F;
        }
    }

    *iv_off = n;

    return( 0 );
}

/*
 * AES-CFB8 buffer encryption/decryption
 */
int mbedtls_aes_crypt_cfb8( mbedtls_aes_context *ctx,
                            int mode,
                            size_t length,
                            unsigned char iv[16],
                            const unsigned char *input,
                            unsigned char *output )
{
    unsigned char c;
    unsigned char ov[17];

    AES_VALIDATE_RET( ctx != NULL );
    AES_VALIDATE_RET( mode == MBEDTLS_AES_ENCRYPT ||
                      mode == MBEDTLS_AES_DECRYPT );
    AES_VALIDATE_RET( iv != NULL );
    AES_VALIDATE_RET( input != NULL );
    AES_VALIDATE_RET( output != NULL );
    while( length-- )
    {
        memcpy( ov, iv, 16 );
        mbedtls_aes_crypt_ecb( ctx, MBEDTLS_AES_ENCRYPT, iv, iv );

        if( mode == MBEDTLS_AES_DECRYPT )
            ov[16] = *input;

        c = *output++ = (unsigned char)( iv[0] ^ *input++ );

        if( mode == MBEDTLS_AES_ENCRYPT )
            ov[16] = c;

        memcpy( iv, ov + 1, 16 );
    }

    return( 0 );
}
#endif /* MBEDTLS_CIPHER_MODE_CFB */

#endif
