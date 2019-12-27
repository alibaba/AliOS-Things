/*
 * Copyright (C) 2018 Alibaba Group Holding Limited
 */

#include "id2_plat.h"
#include "id2_priv.h"
#include "ali_crypto.h"
#if (CONFIG_ID2_HASH_TYPE == ID2_HASH_TYPE_SM3)
#include "irot_hal.h"
#endif

static const unsigned char encoding_table[] =
{
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
    'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
    'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
    'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
    'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
    'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
    'w', 'x', 'y', 'z', '0', '1', '2', '3',
    '4', '5', '6', '7', '8', '9', '+', '/'
};

static const unsigned char decoding_table[] =
{
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 62,  0,  0,  0, 63,
    52, 53, 54, 55, 56, 57, 58, 59, 60, 61,  0,  0,  0,  0,  0,  0,
    0,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
    15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,  0,  0,  0,  0,  0,
    0, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
    41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
};

static const unsigned char mod_table[] = { 0, 2, 1 };

int id2_plat_get_random(uint8_t* buf, uint32_t len)
{
    ali_crypto_result result;
    uint32_t seed;

    seed = (uint32_t)ls_osa_get_time_ms();
    result = ali_seed((uint8_t*)&seed, sizeof(seed));
    if (result != ALI_CRYPTO_SUCCESS) {
        return -1;
    }

    result = ali_rand_gen(buf, len);
    if (result != ALI_CRYPTO_SUCCESS) {
        return -1;
    }

    return 0;
}

#if (CONFIG_ID2_HASH_TYPE == ID2_HASH_TYPE_SHA256)
int id2_plat_hash_sum(const uint8_t* in, uint32_t in_len,
                      uint8_t* out, uint32_t* out_len, digest_t type)
{
    int ret = 0;
    size_t ctx_size;
    void* ctx = NULL;
    ali_crypto_result result;

    if (type != DIGEST_TYPE_SHA256) {
        id2_log_error("not support this type, %d\n", type);
        return -1;
    }

    if (*out_len < 32) {
        id2_log_error("short buffer, %d\n", *out_len);
        return -1;
    }

     result = ali_sha256_get_ctx_size(&ctx_size);
     if (result != ALI_CRYPTO_SUCCESS) {
        id2_log_error("get ctx size fail, 0x%x\n", result);
        return -1;
     }

      ctx = ls_osa_malloc(ctx_size);
      if (ctx == NULL) {
          id2_log_error("out of mem, %d\n", (int)ctx_size);
          ret = -1;
          goto _out;
      }

      result = ali_sha256_init(ctx);
      if (result != ALI_CRYPTO_SUCCESS) {
          id2_log_error("hash init fail, 0x%x\n", result);
          ret = -1;
          goto _out;
      }
      result = ali_sha256_update(in, in_len, ctx);
      if (result != ALI_CRYPTO_SUCCESS) {
          id2_log_error("hash update fail, 0x%x\n", result);
          ret = -1;
          goto _out;
      }
      result = ali_sha256_final(out, ctx);
      if (result != ALI_CRYPTO_SUCCESS) {
          id2_log_error("hash final fail, 0x%x\n", result);
          ret = -1;
          goto _out;
      }

      *out_len = 32;

_out:
    ls_osa_free(ctx);

    return ret;
}
#elif (CONFIG_ID2_HASH_TYPE == ID2_HASH_TYPE_SM3)
int id2_plat_hash_sum(const uint8_t* in, uint32_t in_len,
                      uint8_t* out, uint32_t* out_len, digest_t type) {
    irot_result_t result = 0;
    result = irot_hal_hash_sum(in, in_len, out, out_len, type);
    if (result != IROT_SUCCESS) {
        id2_log_error("se hash sum fail, 0x%x\n", result);
    }
    return result;
}
#endif

int id2_plat_base64_encode(const uint8_t* input, uint32_t input_len,
                           uint8_t* output, uint32_t* output_len)
{
    int i, j;
    uint32_t o_len = 4 * ((input_len + 2) / 3);

    if (!input || !output || !output_len) {
        return -1;
    }

    if (output_len) {
        *output_len = o_len;
    }

    for (i = 0, j = 0; i < input_len;) {
        unsigned int octet_a = i < input_len ? input[i++] : 0;
        unsigned int octet_b = i < input_len ? input[i++] : 0;
        unsigned int octet_c = i < input_len ? input[i++] : 0;

        unsigned int triple = (octet_a << 0x10) + (octet_b << 0x08) + octet_c;

        output[j++] = encoding_table[(triple >> 18) & 0x3F];
        output[j++] = encoding_table[(triple >> 12) & 0x3F];
        output[j++] = encoding_table[(triple >> 6) & 0x3F];
        output[j++] = encoding_table[(triple >> 0) & 0x3F];
    }

    for (i = 0; i < mod_table[input_len % 3]; i++) {
        output[o_len - 1 - i] = '=';
    }

    return 0;
}

int id2_plat_base64_decode(const uint8_t* input, uint32_t input_len,
                           uint8_t* output, uint32_t* output_len)
{
    int i, j;
    uint32_t o_len = input_len / 4 * 3;

    if (!input || !output || !output_len) {
        return -1;
    }

    if (input[input_len - 1] == '=') {
        o_len--;
    }
    if (input[input_len - 2] == '=') {
        o_len--;
    }

    if (output_len) {
        *output_len = o_len;
    }

    for (i = 0, j = 0; i < input_len;)
    {
        unsigned int sextet_a = decoding_table[input[i++]];
        unsigned int sextet_b = decoding_table[input[i++]];
        unsigned int sextet_c = decoding_table[input[i++]];
        unsigned int sextet_d = decoding_table[input[i++]];

        unsigned int triple = (sextet_a << 18)
                              + (sextet_b << 12)
                              + (sextet_c << 6)
                              + (sextet_d << 0);

        if (j < o_len) {
            output[j++] = (triple >> 16) & 0xFF;
        }
        if (j < o_len) {
            output[j++] = (triple >> 8) & 0xFF;
        }
        if (j < o_len) {
            output[j++] = (triple >> 0) & 0xFF;
        }
    }

    return 0;
}

