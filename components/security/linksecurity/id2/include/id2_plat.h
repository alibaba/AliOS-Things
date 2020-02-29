/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef __ID2_PLAT_H__
#define __ID2_PLAT_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    DIGEST_TYPE_SHA1              = 0x00,
    DIGEST_TYPE_SHA224            = 0x01,
    DIGEST_TYPE_SHA256            = 0x02,
    DIGEST_TYPE_SHA384            = 0x03,
    DIGEST_TYPE_SHA512            = 0x04,
    DIGEST_TYPE_SM3               = 0x05,
} digest_t;

int id2_plat_get_random(uint8_t* buf, uint32_t len);

int id2_plat_hash_sum(const uint8_t* in, uint32_t in_len,
                            uint8_t* out, uint32_t* out_len, digest_t type);

int id2_plat_base64_encode(const uint8_t* input, uint32_t input_len,
                           uint8_t* output, uint32_t* output_len);

int id2_plat_base64_decode(const uint8_t* input, uint32_t input_len,
                           uint8_t* output, uint32_t* output_len);

#ifdef __cplusplus
}
#endif

#endif  /* __ID2_PLAT_H__ */

