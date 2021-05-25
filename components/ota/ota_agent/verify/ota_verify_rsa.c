/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
#include "string.h"
#include "ota_log.h"
#include "ota_import.h"
#include "ota_hal_digest.h"
#include "ota_hal_os.h"

static char ota_get_sign_hash_method(void)
{
    return OTA_SHA256;
}

static int ota_verify_rsa_hash(unsigned char *src, int src_len, unsigned char *dig, unsigned char type)
{
    int ret             = 0;
    ota_hash_ctx_t  ctx = {0};

    if (src == NULL || dig == NULL) {
        ret = OTA_VERIFY_RSA_FAIL;
        return ret;
    }

    ret = ota_hash_init(&ctx, type);
    if (ret != 0) {
        ret = OTA_VERIFY_RSA_FAIL;
        return ret;
    }
    ret = ota_hash_update(&ctx, (const unsigned char *)src, src_len);
    if (ret != 0) {
        ret = OTA_VERIFY_RSA_FAIL;
        return ret;
    }
    ret = ota_hash_final(&ctx, dig);
    if (ret != 0) {
        ret = OTA_VERIFY_RSA_FAIL;
        return ret;
    }
    return ret;
}

static int ota_verify_rsa_sign(unsigned char *src, int src_len, int rsabitnumb, unsigned char *signature)
{
    int                 ret     = 0;
    int                 dig_len = 0;
    unsigned char       *dig    = NULL;
    unsigned char       hash_method   = OTA_HASH_NONE;
    unsigned int        pubkey_n_size = 0;
    unsigned int        pubkey_e_size = 0;
    const unsigned char *pubkey_n     = NULL;
    const unsigned char *pubkey_e     = NULL;

    if ((rsabitnumb < OTA_SIGN_BITNUMB) || (NULL == src) ||
        (src_len == 0) || (NULL == signature)) {
        ret = OTA_INVALID_PARAMETER;
        goto EXIT;
    }
    pubkey_n = ota_rsa_pubkey_n();
    pubkey_e = ota_rsa_pubkey_e();
    pubkey_n_size = ota_rsa_pubkey_n_size();
    pubkey_e_size = ota_rsa_pubkey_e_size();
    if ((pubkey_n_size != (rsabitnumb >> 3)) || (pubkey_e_size != 3)) {
        ret = OTA_INVALID_PARAMETER;
        goto EXIT;
    }
    hash_method = ota_get_sign_hash_method();
    if (hash_method != OTA_SHA256) {
        ret = OTA_INVALID_PARAMETER;
        goto EXIT;
    }
    dig_len = OTA_SHA256_HASH_SIZE;
    dig = ota_malloc(dig_len);
    if (NULL == dig) {
        ret = OTA_INVALID_PARAMETER;
        goto EXIT;
    }
    ret = ota_verify_rsa_hash(src, src_len, dig, hash_method);
    if (ret != 0) {
        ret = OTA_VERIFY_RSA_FAIL;
        goto EXIT;
    }
    ret = ota_rsa_pubkey_verify(pubkey_n, pubkey_e, pubkey_n_size, pubkey_e_size, dig, dig_len,
                                signature, rsabitnumb >> 3);
EXIT:
    if (NULL != dig) {
        ota_free(dig);
        dig = NULL;
    }
    if (ret != 0)
        OTA_LOG_E("rsa verify err:%d", ret);
    return ret;
}

int ota_verify_rsa(unsigned char *sign_dat, const char *hash_dat, unsigned char type)
{
    int  ret                            = OTA_VERIFY_RSA_FAIL;
    int  src_dat_len                    = 0;
    unsigned char tmp_buf[OTA_SHA256_HASH_SIZE]  = {0};
    if ((sign_dat != NULL) && (hash_dat != NULL)) {
        ret = 0;
        if (type == OTA_MD5) {
            src_dat_len = OTA_MD5_HASH_SIZE;
        } else if (type == OTA_SHA256) {
            src_dat_len = OTA_SHA256_HASH_SIZE;
        }
        if (ret == 0) {
            ret = ota_str2hex(hash_dat, (char *)tmp_buf, sizeof(tmp_buf));
            if (ret == 0) {
                ret = ota_verify_rsa_sign(tmp_buf, src_dat_len, OTA_SIGN_BITNUMB, sign_dat);
            }
        }
    }
    OTA_LOG_E("verify rsa ret:%d", ret);
    return ret;
}
