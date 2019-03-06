/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
#include "string.h"
#include "ota_log.h"
#include "ota/ota_service.h"
#include "ota_public_key_config.h"
#include "ota_verify.h"

static char ota_get_sign_hash_method(void)
{
    return SHA256;
}

static const unsigned char *ota_get_pubkey_n(void)
{
    return ota_pubn_buf;
}

static const unsigned char *ota_get_pubkey_e(void)
{
    return ota_pube_buf;
}

int ota_get_public_key_bitnumb(void)
{
    unsigned int ota_rsabitnumb = 0;
    ota_rsabitnumb = sizeof(ota_pubn_buf) * 8;
    return ota_rsabitnumb;
}

int ota_make_public_key(unsigned char* pub_key)
{
    int pubkey_n_size = 0;
    int pubkey_e_size = 0;
    int bitnumb = ota_get_public_key_bitnumb();
    const unsigned char *pubkey_n = ota_get_pubkey_n();
    const unsigned char *pubkey_e = ota_get_pubkey_e();
    if((NULL == pubkey_n) || (NULL == pubkey_e) || (bitnumb < 2048)) {
        OTA_LOG_E("init_pubkey: get parameter error!");
        return -1;
    }
    pubkey_n_size = bitnumb >> 3;
    pubkey_e_size = 3;
    return ota_rsa_init_pubkey(bitnumb, pubkey_n, pubkey_n_size, pubkey_e, pubkey_e_size, (ota_rsa_pubkey_t *)pub_key);
}

int ota_make_rsa_verify_dig_data(unsigned char* src_data, int src_len, unsigned char* dig_data, unsigned char hash_type)
{
    int ret = 0;
    unsigned int ctx_size = 0;
    void* hash_ctx = NULL;

    if((NULL == src_data) || (0 == src_len) || (NULL == dig_data) || ((hash_type != SHA256) && (hash_type != MD5))) {
        return -1;
    }

    ota_hash_get_ctx_size(hash_type, (unsigned int*)&ctx_size);
    hash_ctx = ota_malloc(ctx_size);
    if(NULL == hash_ctx) {
        return -1;
    }

    if(OTA_CRYPTO_SUCCESS != ota_hash_init(hash_type, hash_ctx)) {
        ret = -1;
        goto OTA_DIG_OVER;
    }
    if(OTA_CRYPTO_SUCCESS != ota_hash_update((const uint8_t *)src_data, src_len, hash_ctx)) {
        ret = -1;
        goto OTA_DIG_OVER;
    }
    if(OTA_CRYPTO_SUCCESS != ota_hash_final(dig_data, hash_ctx)) {
        OTA_LOG_E("ota verify rsa hash final fail\n ");
        ret = -1;
        goto OTA_DIG_OVER;
    }

OTA_DIG_OVER:
    ota_free(hash_ctx);
    return ret;
}

static int ota_verify_rsa_sign(unsigned char* src_value, int src_len, int rsabitnumb, unsigned char* signature_value)
{
    if((rsabitnumb < 2048) || (NULL == src_value) ||
        (src_len == 0) || (NULL == signature_value)) {
        OTA_LOG_E("ota verify rsa sign: input parameters error ");
        return -1;
    }
    int ret = 0;
    OTA_HASH_E hash_method;
    bool result = false;
    ota_rsa_padding_t rsa_padding;
    unsigned int pub_key_len = 0;
    int dig_value_len = 0;
    unsigned char *pubkey = NULL;
    unsigned char *dig_value = NULL;

    ret = ota_rsa_get_pubkey_size(rsabitnumb, &pub_key_len);
    if (ret != OTA_CRYPTO_SUCCESS) {
        OTA_LOG_E("init_key: get pubkey size fail");
        return -1;
    }
    pubkey = ota_malloc(pub_key_len);
    if (NULL == pubkey) {
        OTA_LOG_E("init_key: public key malloc fail");
        return -1;
    }
    hash_method = ota_get_sign_hash_method();
    switch (hash_method) {
        case MD5:
            dig_value_len = 16;
            break;
        case SHA256:
            dig_value_len = 32;
            break;
        default:
            OTA_LOG_E("ota verify rsa sign: don't support your hash type");
            ota_free(pubkey);
            return -1;
    }
    dig_value = ota_malloc(dig_value_len);
    if(NULL == dig_value) {
        OTA_LOG_E("ota verify rsa sign: malloc dig_value failed");
        ota_free(pubkey);
        return -1;
    }

    memset(pubkey, 0, pub_key_len);
    if(ota_make_public_key(pubkey) != 0) {
        ret = -1;
        OTA_LOG_E("make pub key failed");
        goto OTA_RSA_OVER;
    }

    memset(dig_value, 0, dig_value_len);
    if (ota_make_rsa_verify_dig_data(src_value, src_len, dig_value, hash_method) != 0) {
        ret = -1;
        OTA_LOG_E("make rsa dig data fail");
        goto OTA_RSA_OVER;
    }

    rsa_padding.type = RSASSA_PKCS1_V1_5;
    rsa_padding.pad.rsassa_v1_5.type = hash_method;

    ret = ota_rsa_verify((ota_rsa_pubkey_t *)pubkey, dig_value, dig_value_len,
                             signature_value, rsabitnumb >> 3, rsa_padding, &result);
OTA_RSA_OVER:
    ota_free(pubkey);
    ota_free(dig_value);
    OTA_LOG_I("rsa verify OK:%d res:%d",ret, result);
    return ret;
}

int ota_verify_download_rsa_sign(unsigned char* sign_dat, const char* src_hash_dat, OTA_HASH_E src_hash_method)
{
    char tmp_buf[32] = {0};
    int sign_bitnumb = 0;
    int src_dat_len = 0;

    if((NULL == sign_dat) || (NULL == src_hash_dat)) {
        OTA_LOG_E("ota verify download sign input parameter NULL");
        return -1;
    }
    switch(src_hash_method) {
        case MD5:
            src_dat_len = 16;
            break;
        case SHA256:
            src_dat_len = 32;
            break;
        default:
            OTA_LOG_E("ota rsa sign input hash type error!");
            return -1;
    }

    if(ota_hex_str2buf(src_hash_dat, tmp_buf, sizeof(tmp_buf)) < 0) {
        OTA_LOG_E("rsa verify:str2buf translate failed!");
        return -1;
    }
    sign_bitnumb = ota_get_public_key_bitnumb();
    return ota_verify_rsa_sign((unsigned char*)tmp_buf, src_dat_len, sign_bitnumb, sign_dat);
}


