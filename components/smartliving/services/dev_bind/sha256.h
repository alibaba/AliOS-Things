/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef __AWSS_SHA256_H__
#define __AWSS_SHA256_H__

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

typedef struct SHA256_CTX {
//  const HASH_VTAB * f;
    uint64_t count;
    uint8_t buf[64];
    uint32_t state[8];  // upto SHA2
} SHA256_CTX;

//typedef HASH_CTX SHA256_CTX;
void SHA256_init(SHA256_CTX* ctx);
void SHA256_update(SHA256_CTX* ctx, const void* data, int len);
const uint8_t* SHA256_final(SHA256_CTX* ctx);
// Convenience method. Returns digest address.
const uint8_t* SHA256_hash(const void* data, int len, uint8_t* digest);

#ifndef SHA256_DIGEST_SIZE
#define SHA256_DIGEST_SIZE 32
#endif

#ifdef __cplusplus
}
#endif // __cplusplus

#endif  // SYSTEM_CORE_INCLUDE_MINCRYPT_SHA256_H_
