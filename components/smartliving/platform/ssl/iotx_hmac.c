/*
 * Copyright (c) 2014-2016 Alibaba Group. All rights reserved.
 * License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */


#include <string.h>
#include "mbedtls/md5.h"
#include "mbedtls/sha1.h"

#define KEY_IOPAD_SIZE 64
#define MD5_DIGEST_SIZE 16
#define SHA1_DIGEST_SIZE 20

void iotx_digest_2_base16(char *out, unsigned char *digest, int n) {
    static char const encode[] = "0123456789ABCDEF";
    int j = 0;
    int i = 0;
    for (i = 0; i < n; i ++) {
        int a = digest[i];
        out[j++] = encode[(a>>4)&0xf];
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
    for(index = 0; index < key_len; index++ )
    {
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

    iotx_digest_2_base16(digest, out, 16);

    mbedtls_md5_free(&ctx);
}

void iotx_hmac_sha1(const char *msg, int msg_len, char *digest, const char *key, int key_len)
{
    int index = 0;
    mbedtls_sha1_context ctx;
    unsigned char k_ipad[KEY_IOPAD_SIZE];    /* inner padding - key XORd with ipad  */
    unsigned char k_opad[KEY_IOPAD_SIZE];    /* outer padding - key XORd with opad */
    unsigned char out[SHA1_DIGEST_SIZE];


    memset(k_ipad, 0x36, sizeof(k_ipad));
    memset(k_opad, 0x5C, sizeof(k_opad));
    for(index = 0; index < key_len; index++ )
    {
        k_ipad[index] = (unsigned char)(k_ipad[index] ^ key[index]);
        k_opad[index] = (unsigned char)(k_opad[index] ^ key[index]);
    }
    /* perform inner sha1 */
    mbedtls_sha1_init(&ctx);
    mbedtls_sha1_starts(&ctx);
    mbedtls_sha1_update(&ctx, k_ipad, KEY_IOPAD_SIZE);
    mbedtls_sha1_update(&ctx, (unsigned char *) msg, msg_len);
    mbedtls_sha1_finish(&ctx, out);

    /* perform outer sha1 */
    mbedtls_sha1_init(&ctx);
    mbedtls_sha1_starts(&ctx);
    mbedtls_sha1_update(&ctx, k_opad, KEY_IOPAD_SIZE);
    mbedtls_sha1_update(&ctx, out, SHA1_DIGEST_SIZE);
    mbedtls_sha1_finish(&ctx, out);

    iotx_digest_2_base16(digest, out, 20);

    mbedtls_sha1_free(&ctx);
}
