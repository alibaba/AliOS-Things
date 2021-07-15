/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "os.h"
#include "awss_utils.h"
#include "sha256.h"
#include "passwd.h"
#include "awss_wifimgr.h"

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
extern "C"
{
#endif

int softap_decrypt_password(const char *cipher, const uint8_t *random, char *passwd)
{
    uint8_t cipher_hex[AES128_KEY_LEN] = {0};
    uint8_t aes_key[SHA256_DIGEST_SIZE + 1] = {0};
    uint8_t iv[AES128_KEY_LEN] = {0};
    p_aes128_t aes_ctx = NULL;

    /* get cipher hex */
    utils_str_to_hex((char *)cipher, 32, cipher_hex, 16);

    /* setup iv */
    utils_str_to_hex((char *)random, RANDOM_MAX_LEN * 2, iv, RANDOM_MAX_LEN);

    /* generate aes key */
    utils_sha256(random, RANDOM_MAX_LEN * 2, aes_key);

    /* aes decryption with cbc mode */
    aes_ctx = os_aes128_init((const uint8_t *)aes_key, iv, PLATFORM_AES_DECRYPTION);
    os_aes128_cbc_decrypt(aes_ctx, cipher_hex, sizeof(cipher_hex) / AES128_KEY_LEN, passwd);
    os_aes128_destroy(aes_ctx);

    return 0;
}

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
}
#endif
