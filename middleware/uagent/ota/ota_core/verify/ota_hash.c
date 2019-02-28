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
#define KEY_OTA_HASH_CTX_HEAD "key_ota_hash_ctx_head"
#define KEY_OTA_HASH_CTX_BODY "key_ota_hash_ctx_body"

#define OTA_BUF_VERIFY 512

static ota_hash_param_t g_ctx         = { 0, 0, NULL };
static ota_hash_param_t image_md5_ctx = { 0, 0, NULL };

int ota_hash_get_ctx_size(OTA_HASH_E type, unsigned int *size)
{
    if (NULL == size) {
        return OTA_CRYPTO_INVALID_ARG;
    }
    switch(type) {
        case SHA256:
        case MD5:
            break;
        default:
            OTA_LOG_E("invalid type(%d)\n", type);
            return OTA_CRYPTO_INVALID_TYPE;
    }
    *size = sizeof(ota_hash_ctx_t);
    return OTA_CRYPTO_SUCCESS;
}

int ota_hash_init(OTA_HASH_E type, void *context)
{
    ota_hash_ctx_t *hash_ctx;
    if (NULL == context) {
        return OTA_CRYPTO_INVALID_CONTEXT;
    }

    hash_ctx = (ota_hash_ctx_t *)context;
    if ((IS_VALID_CTX_MAGIC(hash_ctx->magic) &&
         hash_ctx->status != CRYPTO_STATUS_FINISHED) &&
         hash_ctx->status != CRYPTO_STATUS_CLEAN) {
         OTA_LOG_E("bad:%d", (int)hash_ctx->status);
         return OTA_CRYPTO_ERR_STATE;
    }

    switch(type) {
        case SHA256: {
            ota_sha256_init(&hash_ctx->sha256_ctx);
            ota_sha256_starts(&hash_ctx->sha256_ctx, 0);
            break;
        }
        case MD5: {
            ota_md5_init(&hash_ctx->md5_ctx);
            ota_md5_starts(&hash_ctx->md5_ctx);
            break;
        }
        default:
            OTA_LOG_E("invalid type:%d", type);
            return OTA_CRYPTO_INVALID_TYPE;
    }

    hash_ctx->type = type;
    hash_ctx->status = CRYPTO_STATUS_INITIALIZED;
    INIT_CTX_MAGIC(hash_ctx->magic);
    return OTA_CRYPTO_SUCCESS;
}

int ota_hash_update(const unsigned char *src, unsigned int size, void *context)
{
    ota_hash_ctx_t *hash_ctx;
    if (context == NULL) {
        return OTA_CRYPTO_INVALID_CONTEXT;
    }

    if (src == NULL && size != 0) {
        return OTA_CRYPTO_INVALID_ARG;
    }

    hash_ctx = (ota_hash_ctx_t *)context;
    if (!IS_VALID_CTX_MAGIC(hash_ctx->magic)) {
        return OTA_CRYPTO_INVALID_CONTEXT;
    }
    if ((hash_ctx->status != CRYPTO_STATUS_INITIALIZED) &&
        (hash_ctx->status != CRYPTO_STATUS_PROCESSING)) {
         OTA_LOG_E("bad :%d", (int)hash_ctx->status);
         return OTA_CRYPTO_ERR_STATE;
    }
    switch(hash_ctx->type) {
        case SHA256: {
            ota_sha256_update(&hash_ctx->sha256_ctx,
                    (const unsigned char *)src, size);
            break;
        }
        case MD5: {
            ota_md5_update(&hash_ctx->md5_ctx,
                    (const unsigned char *)src, size);
            break;
        }
        default:
            OTA_LOG_E("invalid:%d", hash_ctx->type);
            return OTA_CRYPTO_INVALID_TYPE;
    }

    hash_ctx->status = CRYPTO_STATUS_PROCESSING;
    return OTA_CRYPTO_SUCCESS;
}

int ota_hash_final(unsigned char *dgst, void *context)
{
    ota_hash_ctx_t *hash_ctx;
    if (context == NULL) {
        return OTA_CRYPTO_INVALID_CONTEXT;
    }
    if (dgst == NULL) {
        return OTA_CRYPTO_ERROR;
    }
    hash_ctx = (ota_hash_ctx_t *)context;
    if (!IS_VALID_CTX_MAGIC(hash_ctx->magic)) {
        return OTA_CRYPTO_INVALID_CONTEXT;
    }
    if ((hash_ctx->status != CRYPTO_STATUS_INITIALIZED) &&
        (hash_ctx->status != CRYPTO_STATUS_PROCESSING)) {
         OTA_LOG_E("bad :%d", (int)hash_ctx->status);
         return OTA_CRYPTO_ERR_STATE;
    }
    switch(hash_ctx->type) {
        case SHA256: {
            ota_sha256_finish(&hash_ctx->sha256_ctx, (unsigned char *)dgst);
            ota_sha256_free(&hash_ctx->sha256_ctx);
            break;
        }
        case MD5: {
            ota_md5_finish(&hash_ctx->md5_ctx, (unsigned char *)dgst);
            ota_md5_free(&hash_ctx->md5_ctx);
            break;
        }
        default:
            OTA_LOG_E("invalid :%d", hash_ctx->type);
            return OTA_CRYPTO_INVALID_TYPE;
    }

    CLEAN_CTX_MAGIC(hash_ctx->magic);
    hash_ctx->status = CRYPTO_STATUS_FINISHED;
    return OTA_CRYPTO_SUCCESS;
}

int ota_hash_digest(OTA_HASH_E type, const unsigned char *src, unsigned int size, unsigned char *dgst)
{
    ota_hash_ctx_t hash_ctx;
    if ((src == NULL && size != 0) || dgst == NULL) {
        return OTA_CRYPTO_INVALID_ARG;
    }
    switch(type) {
        case SHA256: {
            ota_sha256_init(&hash_ctx.sha256_ctx);
            ota_sha256_starts(&hash_ctx.sha256_ctx, 0);
            ota_sha256_update(&hash_ctx.sha256_ctx, (const unsigned char *)src, size);
            ota_sha256_finish(&hash_ctx.sha256_ctx, (unsigned char *)dgst);
            ota_sha256_free(&hash_ctx.sha256_ctx);
            break;
        }
        case MD5: {
            ota_md5_init(&hash_ctx.md5_ctx);
            ota_md5_starts(&hash_ctx.md5_ctx);
            ota_md5_update(&hash_ctx.md5_ctx, (const unsigned char *)src, size);
            ota_md5_finish(&hash_ctx.md5_ctx, (unsigned char *)dgst);
            ota_md5_free(&hash_ctx.md5_ctx);
            break;
        }

        default:
            OTA_LOG_E("invalid:%d", type);
            return OTA_CRYPTO_INVALID_TYPE;
    }
    return OTA_CRYPTO_SUCCESS;
}

int ota_malloc_hash_ctx(OTA_HASH_E type)
{
    g_ctx.hash_method = type;
    ota_hash_get_ctx_size(type, (unsigned int*)&g_ctx.ctx_size);
    if (g_ctx.ctx_hash == NULL) {
        g_ctx.ctx_hash = (void *)ota_malloc(g_ctx.ctx_size);
    }
    if (g_ctx.ctx_hash == NULL) {
        g_ctx.hash_method = 0;
        g_ctx.ctx_size    = 0;
        return -1;
    }

    memset(g_ctx.ctx_hash, 0, g_ctx.ctx_size);
    return 0;
}

ota_hash_param_t *ota_get_hash_ctx()
{
    return &g_ctx;
}

void ota_free_hash_ctx(void)
{
    if (g_ctx.ctx_hash) {
        ota_free(g_ctx.ctx_hash);
    }
    g_ctx.ctx_hash    = NULL;
    g_ctx.hash_method = 0;
    g_ctx.ctx_size    = 0;
}

void ota_save_state(int breakpoint, ota_hash_param_t *hash_ctx)
{
    ota_set_break_point(breakpoint);
    ota_set_cur_hash_ctx(hash_ctx);
}

int ota_get_break_point(void)
{
    uint32_t offset = 0;
    int len = 4;
    if (ota_kv_get(KEY_OTA_BREAKPOINT, &offset, &len)) {
        offset = 0;
    }
    return offset;
}

int ota_set_break_point(int offset)
{
    return ota_kv_set(KEY_OTA_BREAKPOINT, &offset, 4, 1);
}

int ota_get_last_hash(char *value)
{
    int len = 66;
    int ret = ota_kv_get(KEY_OTA_HASH, value, &len);
    return ret;
}

int ota_set_cur_hash(char *value)
{
    return ota_kv_set(KEY_OTA_HASH, value, 66, 1);
}

int ota_get_last_hash_ctx(ota_hash_param_t *hash_ctx)
{
    int ret = 0;
    if (hash_ctx == NULL || hash_ctx->ctx_hash == NULL ||
        hash_ctx->ctx_size == 0) {
        return 0;
    }
    int head_len = sizeof hash_ctx->hash_method + sizeof hash_ctx->ctx_size;
    int body_len = hash_ctx->ctx_size;
    ret          = ota_kv_get(KEY_OTA_HASH_CTX_HEAD, hash_ctx, &head_len);
    if (ret == 0) {
        return ota_kv_get(KEY_OTA_HASH_CTX_BODY, hash_ctx->ctx_hash, &body_len);
    }
    return ret;
}

int ota_set_cur_hash_ctx(ota_hash_param_t *hash_ctx)
{
    int ret = 0;
    if (hash_ctx == NULL || hash_ctx->ctx_hash == NULL ||
        hash_ctx->ctx_size == 0) {
        return 0;
    }
    int head_len = sizeof hash_ctx->hash_method + sizeof hash_ctx->ctx_size;
    int body_len = hash_ctx->ctx_size;
    ret = ota_kv_set(KEY_OTA_HASH_CTX_HEAD, hash_ctx, head_len, 1);
    if (ret == 0) {
        return ota_kv_set(KEY_OTA_HASH_CTX_BODY, hash_ctx->ctx_hash, body_len, 1);
    } else {
        return ret;
    }
}

static int ota_check_md5(const unsigned char *cur_hash, const char *download_hash)
{
    if (cur_hash == NULL || download_hash == NULL) {
        return -1;
    }
    char digest_str[33] = {0};
    int  i = 0;
    for (; i < 16; i++) {
        ota_snprintf(digest_str + i * 2, 2 + 1, "%02X", cur_hash[i]);
    }
    OTA_LOG_I("md5 src=%s dst=%s", download_hash, digest_str);
    if (strncmp(digest_str, download_hash, 32)) {
        return -1;
    }
    return 0;
}

static int ota_check_sha256(const unsigned char *cur_hash, const char *download_hash)
{
    if (cur_hash == NULL || download_hash == NULL) {
        return -1;
    }
    char digest_str[65] = {0};
    int  i = 0;
    for (; i < 32; i++) {
        ota_snprintf(digest_str + i * 2, 2 + 1, "%02X", cur_hash[i]);
    }
    OTA_LOG_I("SHA256 src=%s dst:%s", download_hash, digest_str);
    if (strncmp(digest_str, download_hash, 64)) {
        OTA_LOG_E("SHA256 check FAIL!");
        return -1;
    }
    return 0;
}

int ota_check_hash(OTA_HASH_E hash_type, char* hash)
{
    if (hash == NULL) {
        return -1;
    }
    int ret = -1;
    unsigned char digest[64] = { 0 };
    ota_hash_param_t *hash_ctx      = ota_get_hash_ctx();
    if (OTA_CRYPTO_SUCCESS != ota_hash_final(digest, hash_ctx->ctx_hash)) {
        return -1;
    }
    switch (hash_type) {
        case SHA256: {
            ret = ota_check_sha256(digest, hash);
            break;
        }
        case MD5: {
            ret = ota_check_md5(digest, hash);
            break;
        }
        default:
            return -1;
    }
    return ret;
}

// below is about check image
int ota_init_bin_md5_context()
{
    image_md5_ctx.hash_method = MD5;
    ota_hash_get_ctx_size(MD5, (unsigned int*)&image_md5_ctx.ctx_size);
    if (image_md5_ctx.ctx_hash == NULL) {
        image_md5_ctx.ctx_hash = (void *)ota_malloc(image_md5_ctx.ctx_size);
    }
    if (image_md5_ctx.ctx_hash == NULL) {
        image_md5_ctx.hash_method = 0;
        image_md5_ctx.ctx_size    = 0;
        return -1;
    }

    memset(image_md5_ctx.ctx_hash, 0, image_md5_ctx.ctx_size);
    return 0;
}

void ota_destroy_bin_md5_context()
{
    if (image_md5_ctx.ctx_hash) {
        ota_free(image_md5_ctx.ctx_hash);
    }
    image_md5_ctx.ctx_hash    = NULL;
    image_md5_ctx.hash_method = 0;
    image_md5_ctx.ctx_size    = 0;
}

int ota_check_image(unsigned int size)
{
    int ret = 0;
#if defined (OTA_CONFIG_FW_MD5)
    int i = 0;
    int off_set = 0;
    char image_md5_value[33] = {0};
    char download_md5_str_value[33] = {0};
    unsigned int read_size = 0;
    char *rd_buf = NULL;
    char test_buf[33] = { 0 };
    int bin_size = size;
    ota_image_t ota_image_identity = { 0};

    if(size <= sizeof(ota_image_t)){
        ret = OTA_VERIFY_HASH_FAIL;
        return ret;
    }
    off_set = bin_size - sizeof(ota_image_t);
    OTA_LOG_I("bin s:%d off:%d", bin_size, off_set);
    if (ota_hal_read(&off_set, (char*)&ota_image_identity, sizeof(ota_image_t)) < 0) {
        ret = OTA_VERIFY_HASH_FAIL;
        return ret;
    }
    memset(test_buf, 0x00, sizeof(test_buf));
    for (i = 0; i < 16; i++) {
        ota_snprintf((char *)(test_buf + i * 2), 2 + 1, "%02X", ota_image_identity.image_md5_value[i]);
    }
    OTA_LOG_I("magic:0x%04x size:%d md5:%s crc16:0x%02x", ota_image_identity.image_magic,ota_image_identity.image_size, test_buf, ota_image_identity.image_crc16);
    if ((ota_image_identity.image_magic != AOS_SINGLE_TAG) &&
        (ota_image_identity.image_magic != AOS_KERNEL_TAG) &&
        (ota_image_identity.image_magic != AOS_APP_TAG)) {
        ret = OTA_PARAM_FAIL;
        return ret;
    }
    bin_size = ota_image_identity.image_size;
    rd_buf   = ota_malloc(OTA_BUF_VERIFY);
    if (rd_buf == NULL) {
        ret = OTA_PARAM_FAIL;
        return ret;
    }
    if (ota_init_bin_md5_context() < 0) {
        ret = OTA_PARAM_FAIL;
        goto err;
    }
    ret = ota_hash_init(image_md5_ctx.hash_method, image_md5_ctx.ctx_hash);
    if (ret < 0) {
        OTA_LOG_I("hash init.\n");
        goto err;
    }
    off_set = 0;
    while (off_set < bin_size) {
        (bin_size - off_set >= OTA_BUF_VERIFY) ? (read_size = OTA_BUF_VERIFY):(read_size = bin_size - off_set);
        if (ota_hal_read(&off_set, rd_buf, read_size) < 0) {
            ret = OTA_UPGRADE_FAIL;
            goto err;
        }
        ret = ota_hash_update((const uint8_t *)rd_buf, read_size, image_md5_ctx.ctx_hash);
        if (ret < 0) {
            OTA_LOG_E("hash update err.\n");
            goto err;
        }
    }

    memset(image_md5_value, 0x00, sizeof(image_md5_value));
    ret = ota_hash_final((unsigned char *)image_md5_value, image_md5_ctx.ctx_hash);
    if (ret < 0) {
        OTA_LOG_E("hash final err.\n");
        goto err;
    }
    memset(download_md5_str_value, 0x00, sizeof(download_md5_str_value));
    for (i = 0; i < 16; i++) {
        ota_snprintf((char *)download_md5_str_value + i * 2, 2 + 1, "%02X", ota_image_identity.image_md5_value[i]);
    }
    ret = ota_check_md5((const unsigned char *)image_md5_value, (const char *)&download_md5_str_value);
    if (ret < 0) {
        OTA_LOG_E("hash check err.\n");
        goto err;
    }
err:
    OTA_LOG_I("OTA md5 ret:%d",ret);
    if(rd_buf) {
       ota_free(rd_buf);
       rd_buf = NULL;
    }
    ota_destroy_bin_md5_context();
#endif
    return ret;
}
