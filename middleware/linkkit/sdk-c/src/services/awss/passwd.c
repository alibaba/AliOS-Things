/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */



#include <stdlib.h>

#include "os.h"
#include "log.h"
#include "utils_hmac.h"
#include "passwd.h"
#include "sha256.h"
#include "awss_main.h"
#include "utils.h"
#include "awss_wifimgr.h"
#include "zconfig_utils.h"

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
extern "C"
{
#endif

unsigned char aes_random[RANDOM_MAX_LEN] = {0};

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

int produce_signature(unsigned char *sign, unsigned char *txt,
                      unsigned int txt_len, const char *key)
{
    if (sign == NULL || txt == NULL || txt_len == 0 || key == NULL)
        return -1;

    utils_hmac_sha1_hex((const char *)txt, (int)txt_len,
                    (char *)sign, key, strlen(key));

    return 0;
}

int aes_decrypt_string(char *cipher, char *plain, int len, int sec_lvl, char cbc)
{
    char res = 0;
    char decrypt = 1;
    uint8_t iv[AES128_KEY_LEN] = {0};
    uint8_t key[AES128_KEY_LEN] = {0};
    uint8_t random[RANDOM_MAX_LEN] = {0};

    uint8_t *decoded = os_zalloc(len + 1);
    if (decoded == NULL)
        return -1;

    if (cbc == 0) {  // for smartconfig
        memset(random, 0, sizeof(random));
        memcpy(decoded, cipher, len);
    } else {  // for mobile-ap, router, zconfig
        if (cbc == 2)  // zconfig
            memcpy(decoded, cipher, len);
        else  // mobile-ap, router
            utils_str_to_hex(cipher, len, decoded, len);
        memcpy(random, aes_random, sizeof(random));
    }

    awss_debug("security level: %d", sec_lvl);

    switch (sec_lvl) {
        case SEC_LVL_AES128_PRODUCT:
        {
            char product_sec[OS_PRODUCT_SECRET_LEN + 1] = {0};
            os_product_get_secret(product_sec);
            cal_passwd(product_sec, random, key);
            memcpy(iv, random, sizeof(random));
            break;
        }
        case SEC_LVL_AES128_DEVICE:
        {
            char dev_sec[OS_DEVICE_SECRET_LEN + 1] = {0};
            os_device_get_secret(dev_sec);
            cal_passwd(dev_sec, random, key);
            memcpy(iv, random, sizeof(random));
            break;
        }
#if 0
        case SEC_LVL_AES128_MANU:
        {
            char manu_sec[OS_MANU_SECRET_LEN + 1] = {0};
            os_get_manufacture_secret(manu_sec);
            cal_passwd(manu_sec, random, key);
            memcpy(iv, random, sizeof(random));
            break;
        }
#endif
        default:
        {
            decrypt = 0;
            awss_debug("wrong security level: %d\n", sec_lvl);
            res = -2;
            break;
        }
    }

    plain[0] = '\0';

    if (decrypt) {
        p_aes128_t aes = os_aes128_init(key, iv, PLATFORM_AES_DECRYPTION);
        if (cbc == 1) { // AP
            os_aes128_cbc_decrypt(aes, decoded, len / AES128_KEY_LEN / 2, plain);
        } else {  // smartconfig
            os_aes128_cfb_decrypt(aes, decoded, len, plain);
        }
        os_aes128_destroy(aes);
    }

    // awss_debug("descrypted '%s'\n", plain);

    os_free(decoded);

    return res;
}

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
}
#endif
