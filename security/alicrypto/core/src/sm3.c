/**
 * Copyright (C) 2018  Alibaba Group Holding Limited.
 **/

#include "config.h"
#include "osa.h"

#if defined(ALI_ALGO_SM3_C)
#include "sm3.h"


/* Implementation that should never be optimized out by the compiler */
static void ali_algo_zeroize( void *v, size_t n ) {
    volatile unsigned char *p = v; while( n-- ) *p++ = 0;
}

/*
 * 32-bit integer manipulation macros (big endian)
 */
#ifndef GET_UINT32_BE
#define GET_UINT32_BE(n,b,i)                            \
do {                                                    \
    (n) = ( (uint32_t) (b)[(i)    ] << 24 )             \
        | ( (uint32_t) (b)[(i) + 1] << 16 )             \
        | ( (uint32_t) (b)[(i) + 2] <<  8 )             \
        | ( (uint32_t) (b)[(i) + 3]       );            \
} while( 0 )
#endif

#ifndef PUT_UINT32_BE
#define PUT_UINT32_BE(n,b,i)                                  \
do {                                                          \
    (b)[(i)    ] = (unsigned char) (((n) >> 24) & 0xFF);      \
    (b)[(i) + 1] = (unsigned char) (((n) >> 16) & 0xFF);      \
    (b)[(i) + 2] = (unsigned char) (((n) >>  8) & 0xFF);      \
    (b)[(i) + 3] = (unsigned char) (((n)      ) & 0xFF);      \
} while( 0 )
#endif

void ali_algo_sm3_init( ali_algo_sm3_context *ctx )
{
    memset( ctx, 0, sizeof( ali_algo_sm3_context ) );
}

void ali_algo_sm3_free( ali_algo_sm3_context *ctx )
{
    if( ctx == NULL )
        return;
    ali_algo_zeroize( ctx, sizeof( ali_algo_sm3_context ) );
}

void ali_algo_sm3_clone( ali_algo_sm3_context *dst,
                           const ali_algo_sm3_context *src )
{
    *dst = *src;
}

/*
 * SM3 context setup
 */
void ali_algo_sm3_starts( ali_algo_sm3_context *ctx )
{
    ctx->total[0] = 0;
    ctx->total[1] = 0;

    ctx->state[0] = 0x7380166f;
    ctx->state[1] = 0x4914b2b9;
    ctx->state[2] = 0x172442d7;
    ctx->state[3] = 0xda8a0600;
    ctx->state[4] = 0xa96f30bc;
    ctx->state[5] = 0x163138aa;
    ctx->state[6] = 0xe38dee4d;
    ctx->state[7] = 0xb0fb0e4e;
}

#if !defined(ALI_ALGO_SM3_PROCESS_ALT)
static const uint32_t T[] =
{
    0x79cc4519, 0x79cc4519, 0x79cc4519, 0x79cc4519, 
    0x79cc4519, 0x79cc4519, 0x79cc4519, 0x79cc4519,
    0x79cc4519, 0x79cc4519, 0x79cc4519, 0x79cc4519,
    0x79cc4519, 0x79cc4519, 0x79cc4519, 0x79cc4519,
    0x7a879d8a, 0x7a879d8a, 0x7a879d8a, 0x7a879d8a,
    0x7a879d8a, 0x7a879d8a, 0x7a879d8a, 0x7a879d8a,
    0x7a879d8a, 0x7a879d8a, 0x7a879d8a, 0x7a879d8a,
    0x7a879d8a, 0x7a879d8a, 0x7a879d8a, 0x7a879d8a,
    0x7a879d8a, 0x7a879d8a, 0x7a879d8a, 0x7a879d8a,
    0x7a879d8a, 0x7a879d8a, 0x7a879d8a, 0x7a879d8a,
    0x7a879d8a, 0x7a879d8a, 0x7a879d8a, 0x7a879d8a,
    0x7a879d8a, 0x7a879d8a, 0x7a879d8a, 0x7a879d8a,
    0x7a879d8a, 0x7a879d8a, 0x7a879d8a, 0x7a879d8a,
    0x7a879d8a, 0x7a879d8a, 0x7a879d8a, 0x7a879d8a,
    0x7a879d8a, 0x7a879d8a, 0x7a879d8a, 0x7a879d8a,
    0x7a879d8a, 0x7a879d8a, 0x7a879d8a, 0x7a879d8a
};

#define ROTL(x,n) (((x) << n) | ((x) >> (32 - n)))

#define FF0(x,y,z) ((x) ^ (y) ^ (z))
#define FF1(x,y,z) (((x) & (y)) | ((x) & (z)) | ((y) & (z)))

#define GG0(x,y,z) ((x) ^ (y) ^ (z))
#define GG1(x,y,z) (((x) & (y)) | ((~x) & (z)))

#define P0(x) ((x) ^ (ROTL(x,9)) ^ (ROTL(x,17)))
#define P1(x) ((x) ^ (ROTL(x,15)) ^ (ROTL(x,23)))

void ali_algo_sm3_process( ali_algo_sm3_context *ctx, const unsigned char data[64] )
{
    uint32_t SS1, SS2, TT1, TT2, W[68], WW[64];
    uint32_t A[8];
    unsigned int i;

    for( i = 0; i < 8; i++ )
        A[i] = ctx->state[i];

    /*  expansion   */
    for( i = 0; i < 68; i++ )
    {
        if( i < 16 )
            GET_UINT32_BE( W[i], data, 4 * i );
        else
            W[i] = (P1(((W[i-16]) ^ (W[i-9]) ^ (ROTL(W[i-3],15)))) ^ (ROTL(W[i-13],7)) ^ (W[i-6]));
    }
    for( i = 0; i < 64; i++ )
    {
        WW[i] = (W[i]) ^ (W[i+4]);
        /* compression  */
        SS1 = ROTL(((ROTL(A[0],12)) + A[4] + (ROTL(T[i],i))),7);
        SS2 = SS1 ^ (ROTL(A[0],12));
        if (i < 16)
        {
            TT1 = FF0(A[0], A[1], A[2]) + A[3] + SS2 + WW[i];
            TT2 = GG0(A[4], A[5], A[6]) + A[7] + SS1 + W[i]; 
        }
        else
        {
            TT1 = FF1(A[0], A[1], A[2]) + A[3] + SS2 + WW[i];
            TT2 = GG1(A[4], A[5], A[6]) + A[7] + SS1 + W[i];  
        }
        A[3] = A[2];
        A[2] = ROTL(A[1],9);
        A[1] = A[0];
        A[0] = TT1;
        A[7] = A[6];
        A[6] = ROTL(A[5],19);
        A[5] = A[4];
        A[4] = P0(TT2);
    }
    
    for( i = 0; i < 8; i++ )
    {
        ctx->state[i] ^= A[i];
    }
}
#endif /* !ALI_ALGO_SM3_PROCESS_ALT */

/*
 * SM3 process buffer
 */
void ali_algo_sm3_update( ali_algo_sm3_context *ctx, const unsigned char *input, size_t ilen)
{
    size_t fill;
    uint32_t left;

    if( ilen == 0 )
        return;

    left = ctx->total[0] & 0x3F;
    fill = 64 - left;

    ctx->total[0] += (uint32_t) ilen;
    ctx->total[0] &= 0xFFFFFFFF;

    if( ctx->total[0] < (uint32_t) ilen )
        ctx->total[1]++;

    if( left && ilen >= fill )
    {
        memcpy( (void *) (ctx->buffer + left), input, fill );
        ali_algo_sm3_process( ctx, ctx->buffer );
        input += fill;
        ilen  -= fill;
        left = 0;
    }

    while( ilen >= 64 )
    {
        ali_algo_sm3_process( ctx, input );
        input += 64;
        ilen  -= 64;
    }

    if( ilen > 0 )
        memcpy( (void *) (ctx->buffer + left), input, ilen );
}

static const unsigned char sm3_padding[64] =
{
 0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

/*
 * SM3 final digest
 */
void ali_algo_sm3_finish( ali_algo_sm3_context *ctx, unsigned char output[32] )
{
    uint32_t last, padn;
    uint32_t high, low;
    unsigned char msglen[8];

    high = ( ctx->total[0] >> 29 )
         | ( ctx->total[1] <<  3 );
    low  = ( ctx->total[0] <<  3 );

    PUT_UINT32_BE( high, msglen, 0 );
    PUT_UINT32_BE( low,  msglen, 4 );

    last = ctx->total[0] & 0x3F;
    padn = ( last < 56 ) ? ( 56 - last ) : ( 120 - last );
    ali_algo_sm3_update( ctx, sm3_padding, padn );
    ali_algo_sm3_update( ctx, msglen, 8 );

    PUT_UINT32_BE( ctx->state[0], output,  0 );
    PUT_UINT32_BE( ctx->state[1], output,  4 );
    PUT_UINT32_BE( ctx->state[2], output,  8 );
    PUT_UINT32_BE( ctx->state[3], output, 12 );
    PUT_UINT32_BE( ctx->state[4], output, 16 );
    PUT_UINT32_BE( ctx->state[5], output, 20 );
    PUT_UINT32_BE( ctx->state[6], output, 24 );
    PUT_UINT32_BE( ctx->state[7], output, 28 );
}

/*
 * output = SM3( input buffer )
 */
void ali_algo_sm3( const unsigned char *input, size_t ilen,
             unsigned char output[32] )
{
    ali_algo_sm3_context ctx;

    ali_algo_sm3_init( &ctx );
    ali_algo_sm3_starts( &ctx );
    ali_algo_sm3_update( &ctx, input, ilen );
    ali_algo_sm3_finish( &ctx, output );
    ali_algo_sm3_free( &ctx );
}

#endif /* ALI_ALGO_SM3_C */
