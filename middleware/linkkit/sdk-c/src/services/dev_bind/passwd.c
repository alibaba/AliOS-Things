/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdlib.h>

#include "os.h"
#include "utils_hmac.h"
#include "passwd.h"
#include "sha256.h"
#include "utils.h"
#ifdef WIFI_AWSS_ENABLED
#include "awss_wifimgr.h"
#endif

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
extern "C"
{
#endif

uint8_t aes_random[RANDOM_MAX_LEN] = {0};
#ifdef WIFI_AWSS_ENABLED
const char *cal_passwd(void *key, void *random, void *passwd)
{
    uint16_t key_len;
    uint8_t digest[SHA256_DIGEST_SIZE + 1] = {0};
    uint8_t passwd_src[KEY_MAX_LEN + RANDOM_MAX_LEN + 2] = {0};

    if (!passwd || !key || !random)
        return NULL;

    // combine key and random, with split of comma
    key_len = strlen(key);
    if (key_len > KEY_MAX_LEN)
        key_len = KEY_MAX_LEN;
    memcpy(passwd_src, key, key_len);
    passwd_src[key_len ++] = ',';
    memcpy(passwd_src + key_len, random, RANDOM_MAX_LEN);
    key_len += RANDOM_MAX_LEN;

    // produce digest using combination of key and random
    SHA256_hash(passwd_src, key_len, digest);

    // use the first 128bits as AES-Key
    memcpy(passwd, digest, AES128_KEY_LEN);

    return passwd;
}

int produce_signature(uint8_t *sign, uint8_t *txt,
                      uint32_t txt_len, const char *key)
{
    if (sign == NULL || txt == NULL || txt_len == 0 || key == NULL)
        return -1;

    utils_hmac_sha1_hex((const char *)txt, (int)txt_len,
                    (char *)sign, key, strlen(key));

    return 0;
}
#endif

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
}
#endif
