/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <string.h>
#include "mbedtls/md5.h"
#include "iotx_hmac.h"

#define KEY_IOPAD_SIZE 64
#define MD5_DIGEST_SIZE 16

void iotx_md5_digest_2_base16(char *out, unsigned char *digest)
{
    static char const encode[] = "0123456789ABCDEF";
    int j = 0;
    int i = 0;
    for (i = 0; i < 16; i ++) {
        int a = digest[i];
        out[j++] = encode[(a >> 4) & 0xf];
        out[j++] = encode[a & 0xf];
    }
}


void iotx_hmac_md5(const char *msg, int msg_len, char *digest, const char *key, int key_len)
{
    int index = 0;
    mbedtls_md5_context ctx;
    unsigned char k_ipad[KEY_IOPAD_SIZE];    /* inner padding - key XORd with ipad  */
    unsigned char k_opad[KEY_IOPAD_SIZE];    /* outer padding - key XORd with opad */
    unsigned char out[MD5_DIGEST_SIZE];


    memset(k_ipad, 0x36, sizeof(k_ipad));
    memset(k_opad, 0x5C, sizeof(k_opad));
    for (index = 0; index < key_len; index++ ) {
        k_ipad[index] = (unsigned char)(k_ipad[index] ^ key[index]);
        k_opad[index] = (unsigned char)(k_opad[index] ^ key[index]);
    }


    /* perform inner MD5 */
    mbedtls_md5_init(&ctx);
    mbedtls_md5_starts(&ctx);
    mbedtls_md5_update(&ctx, k_ipad, KEY_IOPAD_SIZE);
    mbedtls_md5_update(&ctx, (unsigned char *) msg, msg_len);
    mbedtls_md5_finish(&ctx, out);

    /* perform outer MD5 */
    mbedtls_md5_init(&ctx);
    mbedtls_md5_starts(&ctx);
    mbedtls_md5_update(&ctx, k_opad, KEY_IOPAD_SIZE);
    mbedtls_md5_update(&ctx, out, MD5_DIGEST_SIZE);
    mbedtls_md5_finish(&ctx, out);

    iotx_md5_digest_2_base16(digest, out);

    mbedtls_md5_free(&ctx);
}
