/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "os.h"
#include "awss_utils.h"
#include "passwd.h"
#include "awss_log.h"
#include "awss_wifimgr.h"
#include "awss_crypt.h"

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif

static const char *cal_passwd(void *key, void *random, void *passwd)
{
    uint16_t key_len;
    uint8_t digest[SHA256_DIGEST_SIZE + 1] = {0};
    uint8_t passwd_src[KEY_MAX_LEN + RANDOM_MAX_LEN + 2] = {0};

    if (!passwd || !key || !random) {
        return NULL;
    }

    /* combine key and random, with split of comma */
    key_len = strlen(key);
    if (key_len > KEY_MAX_LEN) {
        key_len = KEY_MAX_LEN;
    }
    memcpy(passwd_src, key, key_len);
    passwd_src[key_len ++] = ',';
    memcpy(passwd_src + key_len, random, RANDOM_MAX_LEN);
    key_len += RANDOM_MAX_LEN;

    /* produce digest using combination of key and random */
    utils_sha256(passwd_src, key_len, digest);

    /* use the first 128bits as AES-Key */
    memcpy(passwd, digest, AES128_KEY_LEN);

    return passwd;
}

int aes_decrypt_string(char *cipher, char *plain, int len, int cipher_hex, int sec_lvl, char cbc, const char *rand)
{
    char res = 0;
    char decrypt = 1;
    uint8_t iv[AES128_KEY_LEN] = {0};
    uint8_t key[AES128_KEY_LEN] = {0};
    uint8_t random[RANDOM_MAX_LEN] = {0};

    uint8_t *decoded = (uint8_t *)awss_zalloc(len + 1);
    if (decoded == NULL) {
        return -1;
    }

    if (cipher_hex == 0) {
        /*
         * mobile-ap, router, dev-ap
         */
        utils_str_to_hex(cipher, len, decoded, len);
    } else {
        /*
         * smartconfig/wps, zconfig
         */
        memcpy(decoded, cipher, len);
    }

    if (rand) {
        /*
         * smartconfig/wps uses zero
         * zconfig/dev-ap/mobile-ap/router uses random
         */
        memcpy(random, rand, sizeof(random));
    }

    awss_debug("security level: %d", sec_lvl);

    switch (sec_lvl) {
        case SEC_LVL_AES128_PRODUCT: {
            char product_sec[OS_PRODUCT_SECRET_LEN + 1] = {0};
            os_product_get_secret(product_sec);
            cal_passwd(product_sec, random, key);
            memcpy(iv, random, sizeof(random));
            break;
        }
        case SEC_LVL_AES128_DEVICE: {
            char dev_sec[OS_DEVICE_SECRET_LEN + 1] = {0};
            os_device_get_secret(dev_sec);
            cal_passwd(dev_sec, random, key);
            memcpy(iv, random, sizeof(random));
            break;
        }
        default: {
            decrypt = 0;
            awss_debug("wrong security: %d\n", sec_lvl);
            res = -2;
            break;
        }
    }

    plain[0] = '\0';

    if (decrypt) {
        p_aes128_t aes = os_aes128_init(key, iv, PLATFORM_AES_DECRYPTION);
        if (cbc) { /* AP */
            /*
             * mobile-ap, dev-ap, router
             */
            os_aes128_cbc_decrypt(aes, decoded, len / AES128_KEY_LEN / 2, plain);
        } else {  /* smartconfig */
            /*
             * smartconfig/wps, zconfig
             */
            os_aes128_cfb_decrypt(aes, decoded, len, plain);
        }
        os_aes128_destroy(aes);
    }

    awss_free(decoded);

    return res;
}

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
}
#endif
