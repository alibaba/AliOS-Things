#ifndef __HASHSUM_H__
#define __HASHSUM_H__

typedef enum
{
    DIGEST_TYPE_SHA1              = 0x00,
    DIGEST_TYPE_SHA224            = 0x01,
    DIGEST_TYPE_SHA256            = 0x02,
    DIGEST_TYPE_SHA384            = 0x03,
    DIGEST_TYPE_SHA512            = 0x04,
    DIGEST_TYPE_SM3               = 0x05,
} digest_t;


irot_result_t hash_sum(const uint8_t* in, uint32_t in_len, uint8_t* out, uint32_t* out_len, digest_t type);

#endif

