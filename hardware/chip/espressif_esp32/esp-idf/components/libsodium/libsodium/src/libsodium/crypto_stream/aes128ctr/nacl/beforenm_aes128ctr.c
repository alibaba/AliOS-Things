/* Author: Peter Schwabe, ported from an assembly implementation by Emilia
 * Käsper
 * Date: 2009-03-19
 * Public domain */

#include "common.h"
#include "consts.h"
#include "crypto_stream_aes128ctr.h"
#include "int128.h"

int
crypto_stream_aes128ctr_beforenm(unsigned char *c, const unsigned char *k)
{
    aes_uint128_t xmm0;
    aes_uint128_t xmm1;
    aes_uint128_t xmm2;
    aes_uint128_t xmm3;
    aes_uint128_t xmm4;
    aes_uint128_t xmm5;
    aes_uint128_t xmm6;
    aes_uint128_t xmm7;
    aes_uint128_t xmm8;
    aes_uint128_t xmm9;
    aes_uint128_t xmm10;
    aes_uint128_t xmm11;
    aes_uint128_t xmm12;
    aes_uint128_t xmm13;
    aes_uint128_t xmm14;
    aes_uint128_t xmm15;
    aes_uint128_t t;

    bitslicekey0(k, c);

    keyexpbs1(xmm0, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7, xmm8, xmm9, xmm10,
              xmm11, xmm12, xmm13, xmm14, xmm15, c);
    keyexpbs(xmm0, xmm1, xmm4, xmm6, xmm3, xmm7, xmm2, xmm5, xmm8, xmm9, xmm10,
             xmm11, xmm12, xmm13, xmm14, xmm15, xor_rcon(&xmm1);
             , 2, c);
    keyexpbs(xmm0, xmm1, xmm3, xmm2, xmm6, xmm5, xmm4, xmm7, xmm8, xmm9, xmm10,
             xmm11, xmm12, xmm13, xmm14, xmm15, xor_rcon(&xmm6);
             , 3, c);
    keyexpbs(xmm0, xmm1, xmm6, xmm4, xmm2, xmm7, xmm3, xmm5, xmm8, xmm9, xmm10,
             xmm11, xmm12, xmm13, xmm14, xmm15, xor_rcon(&xmm3);
             , 4, c);

    keyexpbs(xmm0, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7, xmm8, xmm9, xmm10,
             xmm11, xmm12, xmm13, xmm14, xmm15, xor_rcon(&xmm3);
             , 5, c);
    keyexpbs(xmm0, xmm1, xmm4, xmm6, xmm3, xmm7, xmm2, xmm5, xmm8, xmm9, xmm10,
             xmm11, xmm12, xmm13, xmm14, xmm15, xor_rcon(&xmm5);
             , 6, c);
    keyexpbs(xmm0, xmm1, xmm3, xmm2, xmm6, xmm5, xmm4, xmm7, xmm8, xmm9, xmm10,
             xmm11, xmm12, xmm13, xmm14, xmm15, xor_rcon(&xmm3);
             , 7, c);
    keyexpbs(xmm0, xmm1, xmm6, xmm4, xmm2, xmm7, xmm3, xmm5, xmm8, xmm9, xmm10,
             xmm11, xmm12, xmm13, xmm14, xmm15, xor_rcon(&xmm7);
             , 8, c);

    keyexpbs(xmm0, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7, xmm8, xmm9, xmm10,
             xmm11, xmm12, xmm13, xmm14, xmm15, xor_rcon(&xmm0);
             xor_rcon(&xmm1); xor_rcon(&xmm6); xor_rcon(&xmm3);, 9, c);
    keyexpbs10(xmm0, xmm1, xmm4, xmm6, xmm3, xmm7, xmm2, xmm5, xmm8, xmm9,
               xmm10, xmm11, xmm12, xmm13, xmm14, xmm15, c);

    return 0;
}
