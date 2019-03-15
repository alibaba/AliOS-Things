/**
 * Copyright (C) 2018  Alibaba Group Holding Limited.
 **/

#include "config.h"
#include "osa.h"

#if defined(ALI_ALGO_SM4_C)
#include "sm4.h"

/* Implementation that should never be optimized out by the compiler */
static void ali_algo_zeroize( void *v, size_t n ) {
    volatile unsigned char *p = (unsigned char*)v; 
    while( n-- )
        *p++ = 0;
}

/*
 * 32-bit integer manipulation macros (big endian)
 */
#ifndef GET_UINT32_BE
#define GET_UINT32_BE(n,b,i)                            \
{                                                       \
    (n) = ( (uint32_t) (b)[(i)    ] << 24 )             \
        | ( (uint32_t) (b)[(i) + 1] << 16 )             \
        | ( (uint32_t) (b)[(i) + 2] <<  8 )             \
        | ( (uint32_t) (b)[(i) + 3]       );            \
}
#endif

#ifndef PUT_UINT32_BE
#define PUT_UINT32_BE(n,b,i)                                  \
{                                                             \
    (b)[(i)    ] = (unsigned char) (((n) >> 24) & 0xFF);      \
    (b)[(i) + 1] = (unsigned char) (((n) >> 16) & 0xFF);      \
    (b)[(i) + 2] = (unsigned char) (((n) >>  8) & 0xFF);      \
    (b)[(i) + 3] = (unsigned char) (((n)      ) & 0xFF);      \
}
#endif
    
/* non-linear tau function */
#define TAU(A)                           \
    ((SBOX[( A >> 24)]         << 24)    \
    |(SBOX[((A >> 16) & 0xFF)] << 16)    \
    |(SBOX[((A >> 8 ) & 0xFF)] <<  8)    \
    |(SBOX[( A        & 0xFF)])) 

/* rotation left shift */
#define RLS(A,i)     (((A) << (i)) | ((A) >> (32-i)))         

/* linear transform */
#define L(B)         ((B) ^ (RLS((B),2)) ^ (RLS((B),10)) ^ (RLS((B),18)) ^ (RLS((B),24)))  

/* linear transform for key extension */
#define Lp(B)	    ((B) ^ (RLS((B),13)) ^ (RLS((B),23)))                                   

const uint32_t CK[] = {
    0x00070E15, 0x1C232A31, 0x383F464D, 0x545B6269,
    0x70777E85, 0x8C939AA1, 0xA8AFB6BD, 0xC4CBD2D9,
    0xE0E7EEF5, 0xFC030A11, 0x181F262D, 0x343B4249,
    0x50575E65, 0x6C737A81, 0x888F969D, 0xA4ABB2B9,
    0xC0C7CED5, 0xDCE3EAF1, 0xF8FF060D, 0x141B2229,
    0x30373E45, 0x4C535A61, 0x686F767D, 0x848B9299,
    0xA0A7AEB5, 0xBCC3CAD1, 0xD8DFE6ED, 0xF4FB0209,
    0x10171E25, 0x2C333A41, 0x484F565D, 0x646B7279
};

const uint8_t SBOX[/*256*/] = {
    /*     0     1     2     3     4     5     6     7     8     9     A     B     C     D     E     F*/
    /*0*/ 0xD6, 0x90, 0xE9, 0xFE, 0xCC, 0xE1, 0x3D, 0xB7, 0x16, 0xB6, 0x14, 0xC2, 0x28, 0xFB, 0x2C, 0x05,
    /*1*/ 0x2B, 0x67, 0x9A, 0x76, 0x2A, 0xBE, 0x04, 0xC3, 0xAA, 0x44, 0x13, 0x26, 0x49, 0x86, 0x06, 0x99,
    /*2*/ 0x9C, 0x42, 0x50, 0xF4, 0x91, 0xEF, 0x98, 0x7A, 0x33, 0x54, 0x0B, 0x43, 0xED, 0xCF, 0xAC, 0x62,
    /*3*/ 0xE4, 0xB3, 0x1C, 0xA9, 0xC9, 0x08, 0xE8, 0x95, 0x80, 0xDF, 0x94, 0xFA, 0x75, 0x8F, 0x3F, 0xA6,
    /*4*/ 0x47, 0x07, 0xA7, 0xFC, 0xF3, 0x73, 0x17, 0xBA, 0x83, 0x59, 0x3C, 0x19, 0xE6, 0x85, 0x4F, 0xA8,
    /*5*/ 0x68, 0x6B, 0x81, 0xB2, 0x71, 0x64, 0xDA, 0x8B, 0xF8, 0xEB, 0x0F, 0x4B, 0x70, 0x56, 0x9D, 0x35,
    /*6*/ 0x1E, 0x24, 0x0E, 0x5E, 0x63, 0x58, 0xD1, 0xA2, 0x25, 0x22, 0x7C, 0x3B, 0x01, 0x21, 0x78, 0x87,
    /*7*/ 0xD4, 0x00, 0x46, 0x57, 0x9F, 0xD3, 0x27, 0x52, 0x4C, 0x36, 0x02, 0xE7, 0xA0, 0xC4, 0xC8, 0x9E,
    /*8*/ 0xEA, 0xBF, 0x8A, 0xD2, 0x40, 0xC7, 0x38, 0xB5, 0xA3, 0xF7, 0xF2, 0xCE, 0xF9, 0x61, 0x15, 0xA1,
    /*9*/ 0xE0, 0xAE, 0x5D, 0xA4, 0x9B, 0x34, 0x1A, 0x55, 0xAD, 0x93, 0x32, 0x30, 0xF5, 0x8C, 0xB1, 0xE3,
    /*A*/ 0x1D, 0xF6, 0xE2, 0x2E, 0x82, 0x66, 0xCA, 0x60, 0xC0, 0x29, 0x23, 0xAB, 0x0D, 0x53, 0x4E, 0x6F,
    /*B*/ 0xD5, 0xDB, 0x37, 0x45, 0xDE, 0xFD, 0x8E, 0x2F, 0x03, 0xFF, 0x6A, 0x72, 0x6D, 0x6C, 0x5B, 0x51,
    /*C*/ 0x8D, 0x1B, 0xAF, 0x92, 0xBB, 0xDD, 0xBC, 0x7F, 0x11, 0xD9, 0x5C, 0x41, 0x1F, 0x10, 0x5A, 0xD8,
    /*D*/ 0x0A, 0xC1, 0x31, 0x88, 0xA5, 0xCD, 0x7B, 0xBD, 0x2D, 0x74, 0xD0, 0x12, 0xB8, 0xE5, 0xB4, 0xB0,
    /*E*/ 0x89, 0x69, 0x97, 0x4A, 0x0C, 0x96, 0x77, 0x7E, 0x65, 0xB9, 0xF1, 0x09, 0xC5, 0x6E, 0xC6, 0x84,
    /*F*/ 0x18, 0xF0, 0x7D, 0xEC, 0x3A, 0xDC, 0x4D, 0x20, 0x79, 0xEE, 0x5F, 0x3E, 0xD7, 0xCB, 0x39, 0x48
};


void ali_algo_sm4_init(ali_algo_sm4_context *ctx)
{
    osa_memset(ctx, 0, sizeof(ali_algo_sm4_context));
}

void ali_algo_sm4_free(ali_algo_sm4_context *ctx)
{
    if(ctx == NULL) return;
    ali_algo_zeroize(ctx, sizeof(ali_algo_sm4_context));
}

/*
 * SM4 key schedule 
 */
#if !defined(ALI_ALGO_SM4_SETKEY_ALT)
int ali_algo_sm4_setkey(ali_algo_sm4_context *ctx, 
                        const unsigned char *key, 
                        unsigned int keybits)
{
    unsigned int i;
    uint32_t *rk, K[5];

    if (keybits == 128) 
    	ctx->nr = 32;
    else
        return(ALI_ALGO_ERR_SM4_INVALID_KEY_LENGTH);
#if defined(ALI_ALGO_PADLOCK_C) && defined(ALI_ALGO_PADLOCK_ALIGN16)
    if( sm4_padlock_ace == -1 )
        sm4_padlock_ace = ali_algo_padlock_has_support(ALI_ALGO_PADLOCK_ACE );

    if( sm4_padlock_ace )
        ctx->rk = rk = ALI_ALGO_PADLOCK_ALIGN16(ctx->buf);
    else
#endif
    ctx->rk = rk = ctx->buf;

    GET_UINT32_BE( K[0], key, 0 );  K[0] ^= 0xA3B1BAC6; 
    GET_UINT32_BE( K[1], key, 4 );  K[1] ^= 0x56AA3350;
    GET_UINT32_BE( K[2], key, 8 );  K[2] ^= 0x677D9197;
    GET_UINT32_BE( K[3], key, 12 ); K[3] ^= 0xB27022DC;
    for( i = 0; i < 32; i++ )
    {
        K[4]= ((K[1]) ^ (K[2]) ^ (K[3]) ^ (CK[i]));
        K[4] = TAU( K[4] );
        K[4] = (( K[0] ) ^ ( Lp( K[4] )));
        rk[i] = K[4];
        K[0] = K[1];
        K[1] = K[2];
        K[2] = K[3];
        K[3] = K[4];
    }
    return( 0 );
}

#endif /*!ALI_ALGO_SM4_SETKEY_ALT */

/*
 * SM4-ECB block encryption/decryption
 */
void ali_algo_sm4(int is_enc, ali_algo_sm4_context *ctx,
                          const unsigned char input[16],
                          unsigned char output[16])
{
    int i;
    uint32_t *rk, X[5];

    rk = ctx->rk;

    GET_UINT32_BE(X[0], input, 0 );
    GET_UINT32_BE(X[1], input, 4 );
    GET_UINT32_BE(X[2], input, 8 );
    GET_UINT32_BE(X[3], input, 12);
    for( i=0; i < 32; i++ )
    {
        if(is_enc){
            X[4] = (X[1] ^ X[2] ^ X[3] ^ rk[i]);
        } else {
            X[4] = X[1] ^ X[2] ^ X[3] ^ rk[31-i];
        }
        X[4] = TAU(X[4]);
        X[4] = (( X[0] ) ^ ( L( X[4] )));
        X[0] = X[1];
        X[1] = X[2];
        X[2] = X[3];
        X[3] = X[4];
    }
    PUT_UINT32_BE( X[3], output,  0 );
    PUT_UINT32_BE( X[2], output,  4 );
    PUT_UINT32_BE( X[1], output,  8 );
    PUT_UINT32_BE( X[0], output, 12 );
}   

/*
 * SM4-ECB block encryption/decryption
 */
int ali_algo_sm4_crypt_ecb( ali_algo_sm4_context *ctx,
                    int mode,
                    const unsigned char input[16],
                    unsigned char output[16] )
{
#if defined(ALI_ALGO_PADLOCK_C) && defined(ALI_ALGO_HAVE_X86)
    if(sm4_padlock_ace) {
        if( ali_algo_padlock_xcryptecb(ctx, mode, input, output) == 0)
            return(0);

        // If padlock data misaligned, we just fall back to
        // unaccelerated mode
        //
    }
#endif
    ali_algo_sm4(mode, ctx, input, output);
    return(0);
}


#if defined(ALI_ALGO_CIPHER_MODE_CBC)
/*
 * SM4-CBC buffer encryption/decryption
 */
int ali_algo_sm4_crypt_cbc( ali_algo_sm4_context *ctx,
                    int mode,
                    size_t length,
                    unsigned char iv[16],
                    const unsigned char *input,
                    unsigned char *output )
{
    int i;
    unsigned char temp[16];

    if(length % 16)
        return(ALI_ALGO_ERR_SM4_INVALID_INPUT_LENGTH);

#if defined(ALI_ALGO_PADLOCK_C) && defined(ALI_ALGO_HAVE_X86)
    if(sm4_padlock_ace)  {
        if(ali_algo_padlock_xcryptcbc(ctx, mode, length, iv, input, output) == 0)
            return(0);

        // If padlock data misaligned, we just fall back to
        // unaccelerated mode
        //
    }
#endif

    if (mode == ALI_ALGO_SM4_DECRYPT) {
        while(length > 0) {
            ali_algo_sm4_crypt_ecb(ctx, mode, input, output);

            for( i = 0; i < 16; i++ )
                output[i] = (unsigned char)(output[i] ^ iv[i]);

            osa_memcpy(iv, input, 16);
            input  += 16;
            output += 16;
            length -= 16;
        }
    } else {
        while(length > 0) {
            for( i = 0; i < 16; i++ )
                temp[i] = (unsigned char)(input[i] ^ iv[i]);

            ali_algo_sm4_crypt_ecb(ctx, mode, temp, output);
            osa_memcpy(iv, output, 16);
            input  += 16;
            output += 16;
            length -= 16;
       }
    }
    return(0);
}
#endif /* ALI_ALGO_CIPHER_MODE_CBC */
#endif /* ALI_ALGO_SM4_C */
