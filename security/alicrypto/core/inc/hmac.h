/**
* Copyright (C) 2017  Alibaba Group Holding Limited.
**/

#ifndef ALI_ALGO_HMAC_H
#define ALI_ALGO_HMAC_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

int ali_algo_hmac_starts( ali_algo_hash_context_t *ctx, const unsigned char *key, size_t keylen );

int ali_algo_hmac_update( ali_algo_hash_context_t *ctx, const unsigned char *input, size_t ilen );

int ali_algo_hmac_finish( ali_algo_hash_context_t *ctx, unsigned char *output );

int ali_algo_hash_hmac( const ali_algo_hash_info_t *md_info, const unsigned char *key, size_t keylen,
                const unsigned char *input, size_t ilen,
                unsigned char *output );

int ali_algo_hmac_reset( ali_algo_hash_context_t *ctx );

#ifdef __cplusplus
}
#endif

#endif /* ALI_ALGO_HMAC_H */
