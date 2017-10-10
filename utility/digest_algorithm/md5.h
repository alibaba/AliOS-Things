/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef _MD5_H_
#define _MD5_H_
#include <stdint.h>
#if defined(__cplusplus) /* If this is a C++ compiler, use C linkage */
extern "C"
{
#endif
typedef struct {
    uint32_t state[4];                  /* state (ABCD) */
    uint32_t count[2];                  /* number of bits, modulo 2^64 (lsb first) */
    uint8_t buffer[64];                 /* input buffer */
} MD5_CTX;

void MD5_Init(MD5_CTX *ctx);
void MD5_Update(MD5_CTX *ctx, const uint8_t *msg, int len);
void MD5_Final(uint8_t *digest, MD5_CTX *ctx);

#if defined(__cplusplus) /* If this is a C++ compiler, use C linkage */
}
#endif

#endif
