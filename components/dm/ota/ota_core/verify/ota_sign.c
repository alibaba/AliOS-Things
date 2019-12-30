/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
#include "string.h"
#include "ota_log.h"
#include "ota_import.h"
#include "ota_verify.h"
#include "ota_public_key.h"

static char ota_get_sign_hash_method(void)
{
    return OTA_SHA256;
}

static const unsigned char *ota_get_pubkey_n(void)
{
    return ota_pubn_buf;
}

static const unsigned char *ota_get_pubkey_e(void)
{
    return ota_pube_buf;
}

static unsigned int ota_get_pubkey_n_size(void)
{
    return sizeof(ota_pubn_buf);
}

static unsigned int ota_get_pubkey_e_size(void)
{
    return sizeof(ota_pube_buf);
}

int ota_make_rsa_verify_digital_data(unsigned char *src_data, int src_len, unsigned char *dig_data, unsigned char hash_type)
{
    int ret                  = OTA_CRYPTO_INVALID_ARG;
    ota_hash_ctx_t *hash_ctx = NULL;
    if((src_data != NULL) && (src_len > 0) && (dig_data != NULL)) {
        hash_ctx = ota_malloc_hash_ctx(hash_type);
        ret = OTA_CRYPTO_MALLOC_FAIL;
        if(hash_ctx != NULL) {
            ret = ota_hash_init(hash_ctx);
            if(ret == OTA_CRYPTO_RET_SUCCESS) {
                ret = ota_hash_update((const unsigned char *)src_data, src_len, hash_ctx);
                if(ret == OTA_CRYPTO_RET_SUCCESS) {
                    ret = ota_hash_final(dig_data, hash_ctx);
                }
            }
        }
    }
    if(hash_ctx != NULL) {
        ota_free(hash_ctx);
        hash_ctx = NULL;
    }
    if(ret != OTA_CRYPTO_RET_SUCCESS) {
        OTA_LOG_E("ota:make_rsa_dig_data_err:%x", ret);
    }
    return ret;
}

static int ota_verify_rsa_sign(unsigned char *src_value, int src_len, int rsabitnumb, unsigned char *signature_value)
{
    int                 ret           = OTA_CRYPTO_RET_SUCCESS;
    int                 dig_value_len = 0;
    unsigned char       *dig_value    = NULL;
    unsigned char       hash_method   = OTA_HASH_NONE;
    unsigned int        pubkey_n_size = 0;
    unsigned int        pubkey_e_size = 0;
    const unsigned char *pubkey_n     = NULL;
    const unsigned char *pubkey_e     = NULL;

    if((rsabitnumb < OTA_SIGN_BITNUMB) || (NULL == src_value) ||
        (src_len == 0) || (NULL == signature_value)) {
        ret = OTA_CRYPTO_INVALID_ARG;
        goto OTA_RSA_OVER;
    }
    pubkey_n = ota_get_pubkey_n();
    pubkey_e = ota_get_pubkey_e();
    pubkey_n_size = ota_get_pubkey_n_size();
    pubkey_e_size = ota_get_pubkey_e_size();
    if((pubkey_n_size != (rsabitnumb >> 3)) || (pubkey_e_size != 3)) {
        ret = OTA_CRYPTO_INVALID_ARG;
        goto OTA_RSA_OVER;
    }
    hash_method = ota_get_sign_hash_method();
    if(hash_method != OTA_SHA256) {
        ret = OTA_CRYPTO_INVALID_HASH_TYPE;
        goto OTA_RSA_OVER;
    }
    dig_value_len = OTA_SHA256_HASH_SIZE;
    dig_value = ota_malloc(dig_value_len);
    if(NULL == dig_value) {
        ret = OTA_CRYPTO_MALLOC_FAIL;
        goto OTA_RSA_OVER;
    }
    ret = ota_make_rsa_verify_digital_data(src_value, src_len, dig_value, hash_method);
    if (ret != 0) {
        ret = OTA_CRYPTO_MAKE_DIG_HASH_FAIL;
        goto OTA_RSA_OVER;
    }
    ret = ota_rsa_pubkey_verify(pubkey_n, pubkey_e, pubkey_n_size, pubkey_e_size, dig_value, dig_value_len,
                                signature_value, rsabitnumb >> 3);
OTA_RSA_OVER:
    if(NULL != dig_value) {
        ota_free(dig_value);
        dig_value = NULL;
    }
    if(ret != OTA_CRYPTO_RET_SUCCESS) {
        OTA_LOG_E("ota_verify_rsa_sign err= %d", ret);
    }
    else {
        OTA_LOG_I("rsa verify OK:%d", ret);
    }
    return ret;
}

int ota_verify_download_rsa_sign(unsigned char *sign_dat, const char *src_hash_dat, unsigned char src_hash_method)
{
    int  ret                            = OTA_CRYPTO_INVALID_ARG;
    int  src_dat_len                    = 0;
    unsigned char tmp_buf[OTA_SHA256_HASH_SIZE]  = {0};
    if((sign_dat != NULL) && (src_hash_dat != NULL)) {
        ret = OTA_CRYPTO_RET_SUCCESS;
        if(src_hash_method == OTA_MD5) {
            src_dat_len = OTA_MD5_HASH_SIZE;
        }
        else if(src_hash_method == OTA_SHA256) {
            src_dat_len = OTA_SHA256_HASH_SIZE;
        }
        else {
            ret = OTA_CRYPTO_INVALID_HASH_TYPE;
        }
        if(ret == OTA_CRYPTO_RET_SUCCESS) {
            ret = ota_str2hex(src_hash_dat, (char*)tmp_buf, sizeof(tmp_buf));
            if(ret == 0) {
                ret = ota_verify_rsa_sign(tmp_buf, src_dat_len, OTA_SIGN_BITNUMB, sign_dat);
            }
        }  
    }
    if(ret != OTA_CRYPTO_RET_SUCCESS) {
       OTA_LOG_E("ota_verify_download_rsa_sign err = %d", ret);
    }
    return ret;
}


