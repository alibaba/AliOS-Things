/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */




#include <stdlib.h>
#include <string.h>
#include "iot_import.h"
#include "iotx_log.h"
#include "utils_sha256.h"
/* Shift-right (used in SHA-256, SHA-384, and SHA-512): */
#define R(b,x)      ((x) >> (b))
/* 32-bit Rotate-right (used in SHA-256): */
#define _S32(b,x)   (((x) >> (b)) | ((x) << (32 - (b))))

/* Two of six logical functions used in SHA-256, SHA-384, and SHA-512: */
#define Ch(x,y,z)   (((x) & (y)) ^ ((~(x)) & (z)))
#define Maj(x,y,z)  (((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))

/* Four of six logical functions used in SHA-256: */
#define Sigma0_256(x)   (_S32(2,  (x)) ^ _S32(13, (x)) ^ _S32(22, (x)))
#define Sigma1_256(x)   (_S32(6,  (x)) ^ _S32(11, (x)) ^ _S32(25, (x)))
#define sigma0_256(x)   (_S32(7,  (x)) ^ _S32(18, (x)) ^ R(3 ,   (x)))
#define sigma1_256(x)   (_S32(17, (x)) ^ _S32(19, (x)) ^ R(10,   (x)))

/* Hash constant words K for SHA-256: */
const static uint32_t K256[64] = {
    0x428a2f98UL, 0x71374491UL, 0xb5c0fbcfUL, 0xe9b5dba5UL,
    0x3956c25bUL, 0x59f111f1UL, 0x923f82a4UL, 0xab1c5ed5UL,
    0xd807aa98UL, 0x12835b01UL, 0x243185beUL, 0x550c7dc3UL,
    0x72be5d74UL, 0x80deb1feUL, 0x9bdc06a7UL, 0xc19bf174UL,
    0xe49b69c1UL, 0xefbe4786UL, 0x0fc19dc6UL, 0x240ca1ccUL,
    0x2de92c6fUL, 0x4a7484aaUL, 0x5cb0a9dcUL, 0x76f988daUL,
    0x983e5152UL, 0xa831c66dUL, 0xb00327c8UL, 0xbf597fc7UL,
    0xc6e00bf3UL, 0xd5a79147UL, 0x06ca6351UL, 0x14292967UL,
    0x27b70a85UL, 0x2e1b2138UL, 0x4d2c6dfcUL, 0x53380d13UL,
    0x650a7354UL, 0x766a0abbUL, 0x81c2c92eUL, 0x92722c85UL,
    0xa2bfe8a1UL, 0xa81a664bUL, 0xc24b8b70UL, 0xc76c51a3UL,
    0xd192e819UL, 0xd6990624UL, 0xf40e3585UL, 0x106aa070UL,
    0x19a4c116UL, 0x1e376c08UL, 0x2748774cUL, 0x34b0bcb5UL,
    0x391c0cb3UL, 0x4ed8aa4aUL, 0x5b9cca4fUL, 0x682e6ff3UL,
    0x748f82eeUL, 0x78a5636fUL, 0x84c87814UL, 0x8cc70208UL,
    0x90befffaUL, 0xa4506cebUL, 0xbef9a3f7UL, 0xc67178f2UL
};
/* Initial hash value H for SHA-256: */
const static uint32_t sha256_initial_hash_value[8] = {
    0x6a09e667UL,
    0xbb67ae85UL,
    0x3c6ef372UL,
    0xa54ff53aUL,
    0x510e527fUL,
    0x9b05688cUL,
    0x1f83d9abUL,
    0x5be0cd19UL
};

extern uint32_t os_htobe32(uint32_t data);
extern uint64_t os_htobe64(uint64_t data);
extern uint32_t os_be32toh(uint32_t data);

#ifndef BUILD_AOS
static uint8_t is_little_endian()
{
    static uint32_t _endian_x_ = 1;
    return ((const uint8_t *)(& _endian_x_))[0];
}

static uint8_t  is_big_endian()
{
    return !is_little_endian();
}
//reverse byte order
static  uint32_t reverse_32bit(uint32_t data)
{
    data = (data >> 16) | (data << 16);
    return ((data & 0xff00ff00UL) >> 8) | ((data & 0x00ff00ffUL) << 8);
}
#endif

#ifndef BUILD_AOS
//host byte order to big endian
uint32_t os_htobe32(uint32_t data)
{
    if (is_big_endian()) {
        return data;
    }
    return reverse_32bit(data);
}
//big endian to host byte order
uint32_t os_be32toh(uint32_t data)
{
    return os_htobe32(data);
}
static inline uint64_t reverse_64bit(uint64_t data)
{
    data = (data >> 32) | (data << 32);
    data = ((data & 0xff00ff00ff00ff00ULL) >> 8) | ((data & 0x00ff00ff00ff00ffULL) << 8);

    return ((data & 0xffff0000ffff0000ULL) >> 16) | ((data & 0x0000ffff0000ffffULL) << 16);
}

//host to big endian
uint64_t os_htobe64(uint64_t data)
{
    if (is_big_endian()) {
        return data;
    }

    return reverse_64bit(data);
}
#endif
static void utils_sha256_zeroize(void *v, size_t n)
{
    volatile unsigned char *p = v;
    while (n--) {
        *p++ = 0;
    }
}
void utils_sha256_init(iot_sha256_context *ctx)
{
    memset(ctx, 0, sizeof(iot_sha256_context));
}
void utils_sha256_free(iot_sha256_context *ctx)
{
    if (NULL == ctx) {
        return;
    }

    utils_sha256_zeroize(ctx, sizeof(iot_sha256_context));
}
void utils_sha256_clone(iot_sha256_context *dst,
                        const iot_sha256_context *src)
{
    *dst = *src;
}
void utils_sha256_starts(iot_sha256_context *ctx)
{
    if (NULL == ctx) {
        return;
    }
    memcpy(ctx->state, sha256_initial_hash_value, SHA256_DIGEST_LENGTH);
    memset(ctx->buffer, 0, SHA256_BLOCK_LENGTH);
    ctx->bitcount = 0;
}
void utils_sha256_process(iot_sha256_context *ctx, const uint32_t *data)
{
    uint32_t a, b, c, d, e, f, g, h, s0, s1;
    uint32_t T1, T2, *W256;
    int j;

    W256 = (uint32_t *) ctx->buffer;

    /* Initialize registers with the prev. intermediate value */
    a = ctx->state[0];
    b = ctx->state[1];
    c = ctx->state[2];
    d = ctx->state[3];
    e = ctx->state[4];
    f = ctx->state[5];
    g = ctx->state[6];
    h = ctx->state[7];

    j = 0;

    do {
        /* Copy data while converting to host byte order */
        W256[j] = os_htobe32(*data++);

        /* Apply the SHA-256 compression function to update a..h */
        T1 = h + Sigma1_256(e) + Ch(e, f, g) + K256[j] + W256[j];

        T2 = Sigma0_256(a) + Maj(a, b, c);
        h = g;
        g = f;
        f = e;
        e = d + T1;
        d = c;
        c = b;
        b = a;
        a = T1 + T2;

        j++;
    } while (j < 16);

    do {
        /* Part of the message block expansion: */
        s0 = W256[(j + 1) & 0x0f];
        s0 = sigma0_256(s0);
        s1 = W256[(j + 14) & 0x0f];
        s1 = sigma1_256(s1);

        /* Apply the SHA-256 compression function to update a..h */
        T1 = h + Sigma1_256(e) + Ch(e, f, g) + K256[j] + (W256[j & 0x0f] += s1 + W256[(j + 9) & 0x0f] + s0);
        T2 = Sigma0_256(a) + Maj(a, b, c);
        h = g;
        g = f;
        f = e;
        e = d + T1;
        d = c;
        c = b;
        b = a;
        a = T1 + T2;

        j++;
    } while (j < 64);

    /* Compute the current intermediate hash value */
    ctx->state[0] += a;
    ctx->state[1] += b;
    ctx->state[2] += c;
    ctx->state[3] += d;
    ctx->state[4] += e;
    ctx->state[5] += f;
    ctx->state[6] += g;
    ctx->state[7] += h;

    /* Clean up */
    a = b = c = d = e = f = g = h = T1 = T2 = 0;
}
void utils_sha256_update(iot_sha256_context *ctx, const unsigned char *input, size_t ilen)
{
    unsigned int freespace, usedspace;

    if (ilen == 0) {
        /* Calling with no data is valid - we do nothing */
        return;
    }

    /* Sanity check: */
    if (ctx == (iot_sha256_context *) 0 || input == (unsigned char *) 0) {
        return;
    }

    usedspace = (ctx->bitcount >> 3) % SHA256_BLOCK_LENGTH;
    if (usedspace > 0) {
        /* Calculate how much free space is available in the buffer */
        freespace = SHA256_BLOCK_LENGTH - usedspace;

        if (ilen >= freespace) {
            /* Fill the buffer completely and process it */
            memcpy(&ctx->buffer[usedspace], input, freespace);
            ctx->bitcount += freespace << 3;
            ilen -= freespace;
            input += freespace;
            utils_sha256_process(ctx, (uint32_t *) ctx->buffer);
        } else {
            /* The buffer is not yet full */
            memcpy(&ctx->buffer[usedspace], input, ilen);
            ctx->bitcount += ilen << 3;
            /* Clean up: */
            usedspace = freespace = 0;
            return;
        }
    }
    while (ilen >= SHA256_BLOCK_LENGTH) {
        /* Process as many complete blocks as we can */
        utils_sha256_process(ctx, (uint32_t *) input);
        ctx->bitcount += SHA256_BLOCK_LENGTH << 3;
        ilen -= SHA256_BLOCK_LENGTH;
        input += SHA256_BLOCK_LENGTH;
    }
    if (ilen > 0) {
        /* There's left-overs, so save 'em */
        memcpy(ctx->buffer, input, ilen);
        ctx->bitcount += ilen << 3;
    }
    /* Clean up: */
    usedspace = freespace = 0;
}
void utils_sha256_finish(iot_sha256_context *ctx, unsigned char output[32])
{
    //  int icount = 0;
    uint32_t *d = (uint32_t *) output;
    unsigned int usedspace;

    /* Sanity check: */
    if (ctx == (iot_sha256_context *) 0) {
        return;
    }

    /* If no digest buffer is passed, we don't bother doing this: */
    if (output != (unsigned char *) 0) {
        usedspace = (ctx->bitcount >> 3) % SHA256_BLOCK_LENGTH;
        ctx->bitcount = os_htobe64(ctx->bitcount);
        if (usedspace > 0) {
            /* Begin padding with a 1 bit: */
            ctx->buffer[usedspace++] = 0x80;

            if (usedspace <= SHA256_SHORT_BLOCK_LENGTH) {
                /* Set-up for the last transform: */
                memset(&ctx->buffer[usedspace], 0, SHA256_SHORT_BLOCK_LENGTH - usedspace);
            } else {
                if (usedspace < SHA256_BLOCK_LENGTH) {
                    memset(&ctx->buffer[usedspace], 0, SHA256_BLOCK_LENGTH - usedspace);
                }
                /* Do second-to-last transform: */
                utils_sha256_process(ctx, (uint32_t *) ctx->buffer);

                /* And set-up for the last transform: */
                memset(ctx->buffer, 0, SHA256_SHORT_BLOCK_LENGTH);
            }
        } else {
            /* Set-up for the last transform: */
            memset(ctx->buffer, 0, SHA256_SHORT_BLOCK_LENGTH);

            /* Begin padding with a 1 bit: */
            *ctx->buffer = 0x80;
        }
        /* Set the bit count: */
        u_retLen tmp;
        tmp.lint = ctx->bitcount;
        memcpy(&ctx->buffer[SHA256_SHORT_BLOCK_LENGTH], tmp.sptr, 8);


        /* Final transform: */
        utils_sha256_process(ctx, (uint32_t *) ctx->buffer);

        {
            /* Convert TO host byte order */
            int j;
            for (j = 0; j < 8; j++) {
                ctx->state[j] = os_be32toh(ctx->state[j]);
                *d++ = ctx->state[j];
            }
        }
    }

    /* Clean up state data: */
    memset(ctx, 0, sizeof(iot_sha256_context));
    usedspace = 0;
}
void utils_sha256(const unsigned char *input, size_t ilen, unsigned char output[32])
{
    iot_sha256_context ctx;

    utils_sha256_init(&ctx);
    utils_sha256_starts(&ctx);
    utils_sha256_update(&ctx, input, ilen);
    utils_sha256_finish(&ctx, output);
    utils_sha256_free(&ctx);
}
