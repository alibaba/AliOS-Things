/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include <string.h>
#include "ota_log.h"
#include "ota_crypto.h"
#include "ota_import.h"
#include "ota_plat_ctrl.h"

#define OTA_BUF_VERIFY 512

ota_hash_ctx_t *ota_malloc_hash_ctx(unsigned char hash_method)
{
    int ret = OTA_CRYPTO_MALLOC_FAIL;
    ota_hash_ctx_t* tmp_ctx = NULL;
    int ctx_size = sizeof(ota_hash_ctx_t);
    if((hash_method == OTA_MD5) ||  (hash_method == OTA_SHA256)) {
        tmp_ctx = (void *)ota_malloc(ctx_size);
        if (tmp_ctx != NULL) {
            ret = OTA_CRYPTO_RET_SUCCESS;
            memset(tmp_ctx, 0x00, ctx_size);
            tmp_ctx->hash_method = hash_method;
        }
    }
    else {
        ret = OTA_CRYPTO_INVALID_HASH_TYPE;
    }
    if(ret != OTA_CRYPTO_RET_SUCCESS) {
        OTA_LOG_E("ota:malloc_ctx_err:%d", ret);
    }
    return tmp_ctx;
}

int ota_hash_init(ota_hash_ctx_t *context)
{
    int ret = OTA_CRYPTO_HASH_CONTEXT_NULL;
    ota_hash_ctx_t *hash_ctx = NULL;
    if (context != NULL) {
        hash_ctx = context;
        ret = OTA_CRYPTO_RET_SUCCESS;
        switch(hash_ctx->hash_method) {
            case OTA_SHA256:
                ota_sha256_init(&hash_ctx->sha256_ctx);
                ota_sha256_starts(&hash_ctx->sha256_ctx, 0);
                break;
            case OTA_MD5:
                ota_md5_init(&hash_ctx->md5_ctx);
                ota_md5_starts(&hash_ctx->md5_ctx);
               break;
            default:
               ret = OTA_CRYPTO_INVALID_HASH_TYPE;
               break;
        }
    }
    if(ret != OTA_CRYPTO_RET_SUCCESS) {
        OTA_LOG_E("ota:hash_init_err:%d", ret);
    }
    return ret;
}

int ota_hash_update(const unsigned char *src, unsigned int size, ota_hash_ctx_t *context)
{
    int ret = OTA_CRYPTO_PARAM_NULL;
    ota_hash_ctx_t *hash_ctx = NULL;
    if ((context != NULL) && (src != NULL)) {
        hash_ctx = context;
        ret = OTA_CRYPTO_RET_SUCCESS;
        switch(hash_ctx->hash_method) {
            case OTA_SHA256:
                ota_sha256_update(&hash_ctx->sha256_ctx,
                       (const unsigned char *)src, size);
                break;
            case OTA_MD5:
                ota_md5_update(&hash_ctx->md5_ctx,
                       (const unsigned char *)src, size);
                break;
            default:
                ret = OTA_CRYPTO_INVALID_HASH_TYPE;
                break;
        }
    }
    if(ret != OTA_CRYPTO_RET_SUCCESS) {
        OTA_LOG_E("ota:hash_update_err:%d", ret);
    }
    return ret;
}

int ota_hash_final(unsigned char *dgst, ota_hash_ctx_t *context)
{
    int ret = OTA_CRYPTO_PARAM_NULL;
    ota_hash_ctx_t *hash_ctx = NULL;
    if ((context != NULL) && (dgst != NULL)) {
        hash_ctx = context;
        ret = OTA_CRYPTO_RET_SUCCESS;
        switch(hash_ctx->hash_method) {
            case OTA_SHA256:
                ota_sha256_finish(&hash_ctx->sha256_ctx, (unsigned char *)dgst);
                ota_sha256_free(&hash_ctx->sha256_ctx);
                break;
            case OTA_MD5:
                ota_md5_finish(&hash_ctx->md5_ctx, (unsigned char *)dgst);
                ota_md5_free(&hash_ctx->md5_ctx);
                break;
            default:
                ret = OTA_CRYPTO_INVALID_HASH_TYPE;
                break;
        } 
    }
    if(ret != OTA_CRYPTO_RET_SUCCESS) {
        OTA_LOG_E("ota:hash_final_err:%d", ret);
    }
    return ret;
}

int ota_check_hash(unsigned char hash_type, char *dest_hash, char *src_hash)
{
    int ret = OTA_CRYPTO_PARAM_NULL;
    if ((dest_hash != NULL) && (src_hash != NULL)) {
        ret = OTA_CRYPTO_RET_SUCCESS;
        switch (hash_type) {
            case OTA_SHA256:
                OTA_LOG_I("SHA256 src=%s dst:%s", src_hash, dest_hash);
                if (strncmp(dest_hash, src_hash, 64) != 0) {
                    ret = OTA_CRYPTO_HASH_CHECK_SHA256_FAIL;
                 }
                 break;
             case OTA_MD5:
                 OTA_LOG_I("md5 src=%s dst=%s", src_hash, dest_hash);
                 if (strncmp(dest_hash, src_hash, 32) != 0) {
                     ret = OTA_CRYPTO_HASH_CHECK_MD5_FAIL;
                 }
                 break;
             default:
                 ret = OTA_CRYPTO_INVALID_HASH_TYPE;
                 break; 
        }
    }
    if(ret != OTA_CRYPTO_RET_SUCCESS) {
        OTA_LOG_E("Ota:check_hash_err:%d", ret);
    }
    return ret;
}

/* below is about check image*/
int ota_get_image_data(unsigned int *off_set, char *buf, unsigned int len)
{
    int ret = OTA_CRYPTO_RET_SUCCESS;
    if(NULL == buf || len == 0) {
        ret = OTA_CRYPTO_PARAM_NULL;
    }
    else {
        if (ota_read(off_set, (char *)buf, len) < 0) {
            ret = OTA_CRYPTO_READ_FLASH_FAIL;
        }
    }
    if(ret != OTA_CRYPTO_RET_SUCCESS) {
        OTA_LOG_E("Ota:get_image_data_err:%d", ret);
    }
    return ret;
}

int ota_calculate_image_hash_value(unsigned int offset,
                                   int image_len,
                                   unsigned char hash_method,
                                   char *hash_value,
                                   int len)
{
    int ret                         = OTA_CRYPTO_RET_SUCCESS;
    char *rd_buf                    = NULL;
    unsigned int read_size          = 0;
    ota_hash_ctx_t *tmp_hash_ctx    = NULL;
    if(NULL == hash_value || image_len < 0) {
        ret = OTA_CRYPTO_ERR;
        goto OTA_CALCUL_IMAGE_HASH_VALUE_ERR;
    }
    if(hash_method == OTA_MD5) {
        if(len < 16) {
            ret = OTA_CRYPTO_LENGTH_ERRO;
            goto OTA_CALCUL_IMAGE_HASH_VALUE_ERR;
        }
    }
    else if(hash_method == OTA_SHA256) {
        if(len < 32) {
            ret = OTA_CRYPTO_LENGTH_ERRO;
            goto OTA_CALCUL_IMAGE_HASH_VALUE_ERR;
        }
    }
    else {
        ret = OTA_CRYPTO_INVALID_HASH_TYPE;
        goto OTA_CALCUL_IMAGE_HASH_VALUE_ERR;
    }

    rd_buf = ota_malloc(OTA_BUF_VERIFY);
    if (rd_buf == NULL) {
        ret = OTA_CRYPTO_MALLOC_FAIL;
        goto OTA_CALCUL_IMAGE_HASH_VALUE_ERR;
    }
    tmp_hash_ctx = ota_malloc_hash_ctx(hash_method);
    if (NULL == tmp_hash_ctx) {
        ret = OTA_CRYPTO_MALLOC_FAIL;
        goto OTA_CALCUL_IMAGE_HASH_VALUE_ERR;
    }
    ret = ota_hash_init(tmp_hash_ctx);
    if (ret != OTA_CRYPTO_RET_SUCCESS) {
        goto OTA_CALCUL_IMAGE_HASH_VALUE_ERR;
    }
    while (offset < image_len) {
        (image_len - offset >= OTA_BUF_VERIFY) ? (read_size = OTA_BUF_VERIFY) : (read_size = image_len - offset);
        ret = ota_get_image_data(&offset, rd_buf, read_size);
        if (ret != OTA_CRYPTO_RET_SUCCESS) {
            ret = OTA_CRYPTO_CALCUL_HASH_FAIL;
            goto OTA_CALCUL_IMAGE_HASH_VALUE_ERR;
        }
        ret = ota_hash_update((const unsigned char *)rd_buf, read_size, tmp_hash_ctx);
        if (ret != OTA_CRYPTO_RET_SUCCESS) {
            ret = OTA_CRYPTO_CALCUL_HASH_FAIL;
            goto OTA_CALCUL_IMAGE_HASH_VALUE_ERR;
        }
    }

    memset(hash_value, 0x00, len);
    ret = ota_hash_final((unsigned char *)hash_value, tmp_hash_ctx);
OTA_CALCUL_IMAGE_HASH_VALUE_ERR:
    if(NULL != rd_buf) {
       ota_free(rd_buf);
       rd_buf = NULL;
    }
    if(tmp_hash_ctx != NULL) {
        ota_free(tmp_hash_ctx);
    }
    if(ret != OTA_CRYPTO_RET_SUCCESS) {
        OTA_LOG_E("Ota:get_calcul_image_hash_err:%x", ret);
    }
    return ret;
}

int ota_check_image(unsigned int size)
{
    int ret                          = OTA_CRYPTO_RET_SUCCESS;
#if !defined CONFIG_PING_PONG_OTA
    unsigned int off_set             = 0;
    char image_md5_value[16]         = {0};
    char download_md5_str_value[33]  = {0};
    char calculate_md5_str_value[33] = {0};
    int bin_size                     = size;
    ota_image_t ota_image_identity   = { 0};

    if (size <= sizeof(ota_image_t)) {
        ret = OTA_VERIFY_IMAGE_FAIL;
        goto err;
    }
    off_set = bin_size - sizeof(ota_image_t);
    OTA_LOG_I("bin size:%d off:%d", bin_size, off_set);
    ret = ota_get_image_data(&off_set, (char *)&ota_image_identity, sizeof(ota_image_t));
    if (ret != OTA_CRYPTO_RET_SUCCESS) {
        ret = OTA_VERIFY_IMAGE_FAIL;
        goto err;
    }
    ret = ota_hex2str(download_md5_str_value, (const unsigned char *)ota_image_identity.image_md5_value, sizeof(download_md5_str_value), sizeof(ota_image_identity.image_md5_value));
    if (ret != OTA_CRYPTO_RET_SUCCESS) {
        ret = OTA_VERIFY_IMAGE_FAIL;
        goto err;
    }
    OTA_LOG_I("magic:0x%04x size:%d md5:%s crc16:0x%02x", ota_image_identity.image_magic,ota_image_identity.image_size, download_md5_str_value, ota_image_identity.image_crc16);
    if ((ota_image_identity.image_magic != AOS_SINGLE_TAG) &&
        (ota_image_identity.image_magic != AOS_KERNEL_TAG) &&
        (ota_image_identity.image_magic != AOS_APP_TAG)) {
        ret = OTA_VERIFY_IMAGE_FAIL;
        goto err;
    }
    bin_size = ota_image_identity.image_size;
    ret = ota_calculate_image_hash_value(0, bin_size, OTA_MD5, image_md5_value, 16);
    if(ret != OTA_CRYPTO_RET_SUCCESS) {
        goto err;
    }
    ret = ota_hex2str(calculate_md5_str_value, (const unsigned char*)image_md5_value, sizeof(calculate_md5_str_value), sizeof(image_md5_value));
    if (ret != OTA_CRYPTO_RET_SUCCESS) {
        ret = OTA_VERIFY_IMAGE_FAIL;
        goto err;
    }
    ret = ota_check_hash(OTA_MD5, calculate_md5_str_value, download_md5_str_value);
    if (ret != OTA_CRYPTO_RET_SUCCESS) {
        goto err;
    }
err:
    if(ret != OTA_CRYPTO_RET_SUCCESS) {
        OTA_LOG_E("ota:check_iamge_err:%d", ret);
    }
#endif
    return ret;
}
