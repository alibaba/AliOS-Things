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


#include <stdio.h>
#include <string.h>
#include <inttypes.h>

#include "tfs.h"
#include "iot_import.h"

#define BUF_MAX 512

static void hexdump(const uint8_t *str, uint32_t len)
{
    int i;
    for (i = 0; i < len; i++) {
        HAL_Printf("%02X,", *str++);
        if ((i + 1) % 32 == 0) {
            HAL_Printf("\n");
        }
    }
    HAL_Printf("\n\n");
}

static int demo_tfs_get_ID2(void)
{
    int ret = 0;
    uint32_t len = TFS_ID2_LEN + 1;
    uint8_t id2[TFS_ID2_LEN + 1] = {0};

    ret = tfs_get_ID2(id2, &len);

    HAL_Printf("tfs_get_ID2: ret = %d, the ID2(%d): %s\n\n", ret, len, id2);
    return 0;
}

static int demo_tfs_id2_decrypt(void)
{
    int ret = 0;
    uint32_t enc_len = 128;
    uint32_t dec_len = 0;
    /*
     * ciphertext: enc_data[].
     * plaintext: "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789".
     */
    const uint8_t enc_data[128] = { 0x10, 0x6C, 0x75, 0xF6, 0x86, 0x51, 0xA7, 0x61, 0x88, 0x71, 0x64, 0x46, 0x17, 0x49, 0x9B, 0x97,
                                    0xF9, 0x27, 0x8B, 0x18, 0x89, 0xD9, 0x7A, 0x2E, 0xE1, 0x71, 0x96, 0xF7, 0x25, 0x88, 0x90, 0x55,
                                    0x58, 0x17, 0xC2, 0x78, 0xD5, 0x89, 0x76, 0x1F, 0x60, 0x84, 0xA7, 0xC7, 0xDC, 0x68, 0x0F, 0xE0,
                                    0xCA, 0xCA, 0x1E, 0x81, 0x7D, 0x7C, 0x53, 0x04, 0xD5, 0x5B, 0xD0, 0xF3, 0x85, 0xBC, 0x8C, 0x0B,
                                    0x39, 0xDE, 0x64, 0x81, 0x6E, 0x2E, 0xA5, 0xB5, 0x27, 0x05, 0x8A, 0x57, 0xC1, 0x2F, 0x28, 0x9F,
                                    0x47, 0xE4, 0x14, 0x28, 0xB3, 0x25, 0x82, 0x68, 0x1A, 0x6B, 0xCD, 0x95, 0xA8, 0x09, 0xF3, 0x0B,
                                    0xF8, 0x62, 0x77, 0x90, 0xDD, 0xB8, 0x7B, 0xE3, 0x48, 0x7E, 0xA7, 0xCC, 0xCE, 0xB6, 0x54, 0x68,
                                    0x52, 0xC4, 0x32, 0xF2, 0x47, 0x5D, 0xCD, 0xDC, 0x86, 0x8A, 0xE7, 0x16, 0xBD, 0x3D, 0x22, 0x32
                                  };
    uint8_t dec_out[128] = {0};

    ret = tfs_id2_decrypt((uint8_t *)enc_data, enc_len, dec_out, &dec_len);
    HAL_Printf("tfs_id2_decrypt: ret = %d, decrypt out(%d): %s\n\n", ret, dec_len, dec_out);

    return 0;
}

static int demo_tfs_id2_get_challenge_auth_code(void)
{
    int ret = -1;
    uint32_t len = BUF_MAX;
    const char *challenge = "55B83408399FA660F05C82E4F25333DC";
    const char *extra = "abcd1234";
    uint8_t out_data[BUF_MAX] = {0};

    memset(out_data, 0, BUF_MAX);
    ret = tfs_id2_get_challenge_auth_code((const uint8_t *)challenge, NULL, 0, out_data, &len);
    HAL_Printf("tfs_id2_get_challenge_auth_code: ret = %d, the auth_code(%d): %s\n\n", ret, len, out_data);

    len = BUF_MAX;
    memset(out_data, 0, BUF_MAX);
    ret = tfs_id2_get_challenge_auth_code((const uint8_t *)challenge, (uint8_t *)extra, strlen(extra), out_data, &len);
    HAL_Printf("tfs_id2_get_challenge_auth_code: ret = %d, the auth_code(%d): %s\n\n", ret, len, out_data);

    return 0;
}

static int demo_tfs_id2_get_timestamp_auth_code(uint64_t ts)
{
    int ret = -1;
    uint32_t len = BUF_MAX;
    uint8_t timestamp_str[20 + 1] = {0};
    const char *extra = "abcd1234";
    uint8_t out_data[BUF_MAX] = {0};

    sprintf((char *)timestamp_str, "%" PRIu64, ts);

    memset(out_data, 0, BUF_MAX);
    ret = tfs_id2_get_timestamp_auth_code(timestamp_str, NULL, 0, out_data, &len);
    HAL_Printf("tfs_id2_get_timestamp_auth_code: ret = %d, the auth_code(%d): %s\n\n", ret, len, out_data);

    len = BUF_MAX;
    memset(out_data, 0, BUF_MAX);
    ret = tfs_id2_get_timestamp_auth_code(timestamp_str, (uint8_t *)extra, strlen(extra), out_data, &len);
    HAL_Printf("tfs_id2_get_timestamp_auth_code: ret = %d, the auth_code(%d): %s\n\n", ret, len, out_data);

    return 0;
}

static int demo_aes128_enc_dec(void)
{
    int ret = -1;
    uint8_t iv_enc[16] = {0};
    uint8_t iv_dec[16] = {0};
    const uint8_t in[16] = "Hello World!";
    uint8_t enc_out[16];
    uint8_t dec_out[16] = {0};
    int32_t enc_len = 0;
    int32_t dec_len = 0;
    const uint8_t key[16] = "Demo-Test";

    ret = tfs_aes128_cbc_enc(key, iv_enc, strlen((char *)in), in, &enc_len, enc_out, TFS_AES_ZERO_PADDING);
    HAL_Printf("tfs_aes128_cbc_enc: ret = %d, enc_len = %d, encrypt out(%d):\n", ret, enc_len, 16);
    hexdump(enc_out, 16);

    ret = tfs_aes128_cbc_dec(key, iv_dec, 16, enc_out, &dec_len, dec_out, TFS_AES_ZERO_PADDING);
    HAL_Printf("tfs_aes128_cbc_dec: ret = %d, dec_len = %d, decrypt out(%d): %s\n", ret, dec_len, 16, (char *)dec_out);

    return 0;
}

void tfs_demo(uint64_t timestamp)
{
    HAL_Printf(">>>>>>func: demo_tfs_get_ID2 <<<<<<\n");
    demo_tfs_get_ID2();

    HAL_Printf(">>>>>>func: demo_tfs_id2_decrypt <<<<<<\n");
    demo_tfs_id2_decrypt();

    HAL_Printf(">>>>>>func: demo_tfs_id2_get_challenge_auth_code <<<<<<\n");
    demo_tfs_id2_get_challenge_auth_code();

    HAL_Printf(">>>>>>func: demo_tfs_id2_get_timestamp_auth_code <<<<<<\n");
    demo_tfs_id2_get_timestamp_auth_code(timestamp);

    HAL_Printf(">>>>>>func: demo_aes128_enc_dec <<<<<<\n");
    demo_aes128_enc_dec();
}
