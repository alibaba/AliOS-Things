/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include <string.h>
#include "ota/ota_service.h"
#include "ota_log.h"
#include "ota_verify.h"
#include "ota_hal_plat.h"

#define KEY_OTA_BREAKPOINT    "key_ota_breakpoint"
#define KEY_OTA_HASH          "key_ota_hash"
#define KEY_OTA_HASH_CTX      "key_ota_hash_ctx"

#define OTA_BUF_VERIFY 512

static ota_hash_ctx_t *g_ctx = NULL;

ota_hash_ctx_t *ota_get_hash_ctx()
{
    return g_ctx;
}

ota_hash_ctx_t *ota_malloc_hash_ctx(unsigned char hash_method)
{
    int ret = OTA_CRYPTO_MALLOC_FAIL;
    ota_hash_ctx_t* tmp_ctx = NULL;
    int ctx_size = sizeof(ota_hash_ctx_t);
    if((hash_method != OTA_MD5) && (hash_method != OTA_SHA256)) {
        ret = OTA_CRYPTO_INVALID_HASH_TYPE;
        goto OTA_MALLOC_HASH_CTX_OVER;
    }
    tmp_ctx = (void *)ota_malloc(ctx_size);
    if (tmp_ctx != NULL) {
        ret = OTA_CRYPTO_RET_SUCCESS;
        memset(tmp_ctx, 0x00, ctx_size);
        tmp_ctx->hash_method = hash_method;
    }
OTA_MALLOC_HASH_CTX_OVER:
    if(ret != OTA_CRYPTO_RET_SUCCESS) {
        OTA_LOG_E("ota:malloc_ctx_err:%x", ret);
    }
    return tmp_ctx;
}

void ota_free_hash_ctx(ota_hash_ctx_t *tmp_ctx)
{
    if (tmp_ctx != NULL) {
        ota_free(tmp_ctx);
    }
    tmp_ctx = NULL;
}

int ota_make_global_hash_ctx(unsigned char hash_method)
{
    int ret = OTA_CRYPTO_MALLOC_FAIL;
    if(NULL == g_ctx) {
       ret = OTA_CRYPTO_RET_SUCCESS;
       g_ctx = ota_malloc_hash_ctx(hash_method);
    }
    if(ret != OTA_CRYPTO_RET_SUCCESS) {
        OTA_LOG_E("ota:make_global_hash_ctx_err:%x", ret);
    }
    return ret;
}

void ota_free_global_hash_ctx()
{
    ota_free_hash_ctx(g_ctx);
}

int ota_hash_init(ota_hash_ctx_t *context)
{
    int ret = OTA_CRYPTO_RET_SUCCESS;
    ota_hash_ctx_t *hash_ctx = NULL;
    if (NULL == context) {
        ret = OTA_CRYPTO_HASH_CONTEXT_NULL;
        goto OTA_HASH_INIT_OVER;
    }
    hash_ctx = context;
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
            goto OTA_HASH_INIT_OVER;
    }
OTA_HASH_INIT_OVER:
    if(ret != OTA_CRYPTO_RET_SUCCESS) {
        OTA_LOG_E("ota:hash_init_err:%x", ret);
    }
    return ret;
}

int ota_hash_update(const unsigned char *src, unsigned int size, ota_hash_ctx_t *context)
{
    int ret = OTA_CRYPTO_RET_SUCCESS;
    ota_hash_ctx_t *hash_ctx = NULL;
    if (NULL == context || NULL == src) {
        ret = OTA_CRYPTO_PARAM_NULL;
        goto OTA_HASH_UPDATE;
    }
    hash_ctx = context;
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
            goto OTA_HASH_UPDATE;
    }
OTA_HASH_UPDATE:
    if(ret != OTA_CRYPTO_RET_SUCCESS) {
        OTA_LOG_E("ota:hash_update_err:%x", ret);
    }
    return ret;
}

int ota_hash_final(unsigned char *dgst, ota_hash_ctx_t *context)
{
    int ret = OTA_CRYPTO_RET_SUCCESS;
    ota_hash_ctx_t *hash_ctx = NULL;
    if (NULL == context || NULL == dgst) {
        ret = OTA_CRYPTO_PARAM_NULL;
        goto OTA_HASH_FINAL_OVER;
    }
    hash_ctx = context;
    switch(hash_ctx->hash_method) {
        case OTA_SHA256: {
            ota_sha256_finish(&hash_ctx->sha256_ctx, (unsigned char *)dgst);
            ota_sha256_free(&hash_ctx->sha256_ctx);
            break;
        }
        case OTA_MD5: {
            ota_md5_finish(&hash_ctx->md5_ctx, (unsigned char *)dgst);
            ota_md5_free(&hash_ctx->md5_ctx);
            break;
        }
        default:
            ret = OTA_CRYPTO_INVALID_HASH_TYPE;
            goto OTA_HASH_FINAL_OVER;
    }
OTA_HASH_FINAL_OVER:
    if(ret != OTA_CRYPTO_RET_SUCCESS) {
        OTA_LOG_E("ota:hash_final_err:%x", ret);
    }
    return ret;
}

void ota_save_state(int breakpoint, ota_hash_ctx_t *hash_ctx)
{
    ota_set_break_point(breakpoint);
    ota_set_cur_hash_ctx(hash_ctx);
}

int ota_get_break_point(void)
{
    unsigned int offset = 0;
    int len    = 4;
    if (ota_kv_get(KEY_OTA_BREAKPOINT, &offset, &len) != OTA_CRYPTO_RET_SUCCESS) {
        offset = 0;
    }
    return offset;
}

int ota_set_break_point(int offset)
{
    unsigned int len = 4;
    return ota_kv_set(KEY_OTA_BREAKPOINT, &offset, len, 1);
}

int ota_get_last_hash_value(char *value)
{
    int len = OTA_HASH_LEN;
    int ret = ota_kv_get(KEY_OTA_HASH, value, &len);
    return ret;
}

int ota_set_cur_hash_value(char *value)
{
    return ota_kv_set(KEY_OTA_HASH, value, OTA_HASH_LEN, 1);
}

int ota_get_last_hash_ctx(ota_hash_ctx_t *hash_ctx)
{
    int ret = OTA_CRYPTO_RET_SUCCESS;
    int len = 0;
    if (hash_ctx == NULL) {
        ret = OTA_CRYPTO_HASH_CTX_GET_ERR;
        goto OTA_GET_LAST_HASH_CTX_OVER;
    }
    len = sizeof(ota_hash_ctx_t);
    ret = ota_kv_get(KEY_OTA_HASH_CTX, (void*)hash_ctx, &len);
OTA_GET_LAST_HASH_CTX_OVER:
    if(ret != OTA_CRYPTO_RET_SUCCESS) {
        OTA_LOG_E("ota:get_last_hash_ctx_err:%x", ret);
    }
    return ret;
}

int ota_set_cur_hash_ctx(ota_hash_ctx_t *hash_ctx)
{
    int ret = OTA_CRYPTO_RET_SUCCESS;
    unsigned int len = 0;
    if (hash_ctx == NULL) {
        ret = OTA_CRYPTO_HASH_CTX_SET_ERR;
        goto OTA_SET_CUR_HASH_CTX_OVER;
    }
    len = sizeof(ota_hash_ctx_t);
    ret = ota_kv_set(KEY_OTA_HASH_CTX, (void*)hash_ctx, len, 1);
OTA_SET_CUR_HASH_CTX_OVER:
    if(ret != OTA_CRYPTO_RET_SUCCESS) {
        OTA_LOG_E("ota:set_cur_hash_ctx_err:%x", ret);
    }
    return ret;
}

static int ota_check_md5(const unsigned char *cur_hash, const char *download_hash)
{
    int  i              = 0;
    int  ret            = OTA_CRYPTO_RET_SUCCESS;
    char digest_str[33] = {0};
    if (cur_hash == NULL || download_hash == NULL) {
        ret = OTA_CRYPTO_PARAM_NULL;
        goto OTA_CHECK_MD5_OVER;
    }
    for (; i < 16; i++) {
        ota_snprintf(digest_str + i * 2, 2 + 1, "%02X", cur_hash[i]);
    }
    OTA_LOG_I("md5 src=%s dst=%s", download_hash, digest_str);
    if (strncmp(digest_str, download_hash, 32) != 0) {
        ret = OTA_CRYPTO_HASH_CHECK_MD5_FAIL;
        goto OTA_CHECK_MD5_OVER;
    }
OTA_CHECK_MD5_OVER:
    if(ret != OTA_CRYPTO_RET_SUCCESS) {
        OTA_LOG_E("Ota:MD5_check_err:%x", ret);
    }
    return ret;
}

static int ota_check_sha256(const unsigned char *cur_hash, const char *download_hash)
{
    int  ret             = OTA_CRYPTO_RET_SUCCESS;
    int  i              = 0;
    char digest_str[65] = {0};
    if (cur_hash == NULL || download_hash == NULL) {
        ret = OTA_CRYPTO_PARAM_NULL;
        goto OTA_CHECK_SHA256_OVER;
    }
    for (; i < 32; i++) {
        ota_snprintf(digest_str + i * 2, 2 + 1, "%02X", cur_hash[i]);
    }
    OTA_LOG_I("SHA256 src=%s dst:%s", download_hash, digest_str);
    if (strncmp(digest_str, download_hash, 64) != 0) {
        ret = OTA_CRYPTO_HASH_CHECK_SHA256_FAIL;
        goto OTA_CHECK_SHA256_OVER;
    }
OTA_CHECK_SHA256_OVER:
    if(ret != OTA_CRYPTO_RET_SUCCESS) {
        OTA_LOG_E("Ota:SHA256_check_err:%x", ret);
    }
    return ret;
}

int ota_check_hash(unsigned char hash_type, char *hash)
{
    int           ret        = OTA_CRYPTO_RET_SUCCESS;
    unsigned char digest[64] = { 0 };
    ota_hash_ctx_t *hash_ctx = ota_get_hash_ctx();
    if (hash == NULL) {
        ret = OTA_CRYPTO_PARAM_NULL;
        goto OTA_CHECK_HASH_OVER;
    }
    if(hash_type != hash_ctx->hash_method) {
        ret = OTA_CRYPTO_INVALID_HASH_TYPE;
        goto OTA_CHECK_HASH_OVER;
    }
    if (OTA_CRYPTO_RET_SUCCESS != ota_hash_final(digest, hash_ctx)) {
        ret = OTA_CRYPTO_HASH_FINAL_ERR;
        goto OTA_CHECK_HASH_OVER;
    }
    switch (hash_type) {
        case OTA_SHA256: {
            ret = ota_check_sha256(digest, hash);
            break;
        }
        case OTA_MD5: {
            ret = ota_check_md5(digest, hash);
            break;
        }
        default:
            ret = OTA_CRYPTO_INVALID_HASH_TYPE;
            goto OTA_CHECK_HASH_OVER;
    }
OTA_CHECK_HASH_OVER:
    if(ret != OTA_CRYPTO_RET_SUCCESS) {
        OTA_LOG_E("Ota:check_hash_err:%x", ret);
    }
    return ret;
}

/* below is about check image*/
int ota_get_image_data(int *off_set, char *buf, unsigned int len)
{
    int ret = OTA_CRYPTO_RET_SUCCESS;
    if(NULL == buf || len == 0) {
        ret = OTA_CRYPTO_PARAM_NULL;
        goto OTA_GET_IMAGE_DATA_OVER;
    }
    if (ota_hal_read(off_set, (char *)buf, len) < 0) {
        ret = OTA_CRYPTO_READ_FLASH_FAIL;
        goto OTA_GET_IMAGE_DATA_OVER;
    }
OTA_GET_IMAGE_DATA_OVER:
    if(ret != OTA_CRYPTO_RET_SUCCESS) {
        OTA_LOG_E("Ota:get_image_data_err:%x", ret);
    }
    return ret;
}

int ota_calculate_image_hash_value(int offset,
                                   int image_len,
                                   unsigned char hash_method,
                                   char *hash_value,
                                   int len)
{
    int ret                         = OTA_CRYPTO_RET_SUCCESS;
    char *rd_buf                    = NULL;
    unsigned int read_size          = 0;
    ota_hash_ctx_t *tmp_hash_ctx    = NULL;
    if(NULL == hash_value || offset < 0 || image_len < 0) {
        ret = OTA_CRYPTO_ERR;
        goto OTA_CALCUL_IMAGE_HASH_VALUE_OVER;
    }
    if(hash_method == OTA_MD5) {
        if(len < 16) {
            ret = OTA_CRYPTO_LENGTH_ERRO;
            goto OTA_CALCUL_IMAGE_HASH_VALUE_OVER;
        }
    }
    else if(hash_method == OTA_SHA256) {
        if(len < 32) {
            ret = OTA_CRYPTO_LENGTH_ERRO;
            goto OTA_CALCUL_IMAGE_HASH_VALUE_OVER;
        }
    }
    else {
        ret = OTA_CRYPTO_INVALID_HASH_TYPE;
        goto OTA_CALCUL_IMAGE_HASH_VALUE_OVER;
    }

    rd_buf = ota_malloc(OTA_BUF_VERIFY);
    if (rd_buf == NULL) {
        ret = OTA_CRYPTO_MALLOC_FAIL;
        goto OTA_CALCUL_IMAGE_HASH_VALUE_OVER;
    }
    tmp_hash_ctx = ota_malloc_hash_ctx(hash_method);
    if (NULL == tmp_hash_ctx) {
        ret = OTA_CRYPTO_MALLOC_FAIL;
        goto OTA_CALCUL_IMAGE_HASH_VALUE_OVER;
    }
    ret = ota_hash_init(tmp_hash_ctx);
    if (ret != OTA_CRYPTO_RET_SUCCESS) {
        goto OTA_CALCUL_IMAGE_HASH_VALUE_OVER;
    }
    while (offset < image_len) {
        (image_len - offset >= OTA_BUF_VERIFY) ? (read_size = OTA_BUF_VERIFY) : (read_size = image_len - offset);
        ret = ota_get_image_data(&offset, rd_buf, read_size);
        if (ret != OTA_CRYPTO_RET_SUCCESS) {
            ret = OTA_CRYPTO_CALCUL_HASH_FAIL;
            goto OTA_CALCUL_IMAGE_HASH_VALUE_OVER;
        }
        ret = ota_hash_update((const unsigned char *)rd_buf, read_size, tmp_hash_ctx);
        if (ret != OTA_CRYPTO_RET_SUCCESS) {
            ret = OTA_CRYPTO_CALCUL_HASH_FAIL;
            goto OTA_CALCUL_IMAGE_HASH_VALUE_OVER;
        }
    }

    memset(hash_value, 0x00, len);
    ret = ota_hash_final((unsigned char *)hash_value, tmp_hash_ctx);
    if (ret != OTA_CRYPTO_RET_SUCCESS) {
        goto OTA_CALCUL_IMAGE_HASH_VALUE_OVER;
    }

OTA_CALCUL_IMAGE_HASH_VALUE_OVER:
    if(NULL != rd_buf) {
       ota_free(rd_buf);
       rd_buf = NULL;
    }
    ota_free_hash_ctx(tmp_hash_ctx);
    if(ret != OTA_CRYPTO_RET_SUCCESS) {
        OTA_LOG_E("Ota:get_calcul_image_hash_err:%x", ret);
    }
    return ret;
}

int ota_check_image(unsigned int size)
{
    int ret                         = OTA_CRYPTO_RET_SUCCESS;
#if !defined (OTA_CONFIG_BANK_DUAL)
    int i = 0;
    int off_set                     = 0;
    char image_md5_value[16]        = {0};
    char download_md5_str_value[33] = {0};
    int bin_size                    = size;
    ota_image_t ota_image_identity  = { 0};
    if (size <= sizeof(ota_image_t)) {
        ret = OTA_VERIFY_HASH_FAIL;
        goto err;
    }
    off_set = bin_size - sizeof(ota_image_t);
    OTA_LOG_I("bin s:%d off:%d", bin_size, off_set);
    ret = ota_get_image_data(&off_set, (char *)&ota_image_identity, sizeof(ota_image_t));
    if (ret != OTA_CRYPTO_RET_SUCCESS) {
        ret = OTA_VERIFY_HASH_FAIL;
        goto err;
    }
    memset(download_md5_str_value, 0x00, sizeof(download_md5_str_value));
    for (i = 0; i < 16; i++) {
        ota_snprintf((char *)download_md5_str_value + i * 2, 2 + 1, "%02X", ota_image_identity.image_md5_value[i]);
    }
    OTA_LOG_I("magic:0x%04x size:%d md5:%s crc16:0x%02x", ota_image_identity.image_magic,ota_image_identity.image_size, download_md5_str_value, ota_image_identity.image_crc16);
    if ((ota_image_identity.image_magic != AOS_SINGLE_TAG) &&
        (ota_image_identity.image_magic != AOS_KERNEL_TAG) &&
        (ota_image_identity.image_magic != AOS_APP_TAG)) {
        ret = OTA_PARAM_FAIL;
        goto err;
    }
    bin_size = ota_image_identity.image_size;
    ret = ota_calculate_image_hash_value(0, bin_size, OTA_MD5, image_md5_value, 16);
    if(ret != OTA_CRYPTO_RET_SUCCESS) {
        goto err;
    }
    ret = ota_check_md5((const unsigned char *)image_md5_value, (const char *)&download_md5_str_value);
    if (ret != OTA_CRYPTO_RET_SUCCESS) {
        goto err;
    }
err:
    if(ret != OTA_CRYPTO_RET_SUCCESS) {
        OTA_LOG_I("ota:check_iamge_err:%d", ret);
    }
#endif
    return ret;
}
