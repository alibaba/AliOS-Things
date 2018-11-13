/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include <string.h>
#include "ota_service.h"
#include "ota_log.h"
#include "ota_verify.h"
#include "ota_hal_os.h"
#include "ota_hal_plat.h"
#include "ota_hash.h"
#include "ota_crc.h"

#define KEY_OTA_BREAKPOINT    "key_ota_breakpoint"
#define KEY_OTA_HASH          "key_ota_hash"
#define KEY_OTA_HASH_CTX_HEAD "key_ota_hash_ctx_head"
#define KEY_OTA_HASH_CTX_BODY "key_ota_hash_ctx_body"

#define OTA_BUF_VERIFY 512

static ota_hash_param_t g_ctx         = { 0, 0, NULL };
static ota_hash_param_t image_md5_ctx = { 0, 0, NULL };
static ota_image_t ota_image_identity;

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

void ota_free_hash_ctx()
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

int ota_breakpoint_is_valid()
{
    int breakpoint_is_valid = -1;
    int tmp_breakpoint = 0;
    ota_hash_t last_hash  = {0};

    tmp_breakpoint = ota_get_break_point();
    memset(&last_hash, 0x00, sizeof(last_hash));
    ota_get_last_hash((char *)&last_hash);
    if (tmp_breakpoint && (strncmp((char*)&last_hash, ota_get_service()->h_tr->hash, OTA_HASH_LEN) == 0)) {
        breakpoint_is_valid = 0;
    }
    return breakpoint_is_valid;
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
    OTA_LOG_I("SET cur hash context head = %d", head_len);
    ret = ota_kv_set(KEY_OTA_HASH_CTX_HEAD, hash_ctx, head_len, 1);
    if (ret == 0) {
        return ota_kv_set(KEY_OTA_HASH_CTX_BODY, hash_ctx->ctx_hash, body_len,
                          1);
    } else {
        return ret;
    }
}

static int ota_check_md5(const unsigned char *cur_hash, const char *download_hash)
{
    if (cur_hash == NULL || download_hash == NULL) {
        OTA_LOG_E("update_packet MD5 check FAIL!");
        return -1;
    }
    char digest_str[33] = { 0 };
    int  i              = 0;
    for (; i < 16; i++) {
        ota_snprintf(digest_str + i * 2, 2 + 1, "%02X", cur_hash[i]);
    }
    OTA_LOG_I("url md5=%s", download_hash);
    OTA_LOG_I("digestMD5=%s", digest_str);
    if (strncmp(digest_str, download_hash, 32)) {
        OTA_LOG_E("update_packet md5 check FAIL!");
        return -1;
    }
    return 0;
}

static int ota_check_sha256(const unsigned char *cur_hash, const char *download_hash)
{
    if (cur_hash == NULL || download_hash == NULL) {
        OTA_LOG_E("update_packet SHA256 check FAIL!");
        return -1;
    }
    char digest_str[65] = { 0 };
    int  i              = 0;
    for (; i < 32; i++) {
        ota_snprintf(digest_str + i * 2, 2 + 1, "%02X", cur_hash[i]);
    }
    OTA_LOG_I("url SHA256=%s", download_hash);
    OTA_LOG_I("digestSHA256=%s", digest_str);
    if (strncmp(digest_str, download_hash, 64)) {
        OTA_LOG_E("update_packet SHA256 check FAIL!");
        return -1;
    }
    return 0;
}

int ota_check_hash(OTA_HASH_E hash_type, char* hash)
{
    if (hash == NULL) {
        return -1;
    }
    int                  ret        = -1;
    unsigned char        digest[64] = { 0 };
    ota_hash_param_t *hash_ctx   = ota_get_hash_ctx();
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

int ota_check_image(void)
{
    int                   ret     = 0;
    int                   i       = 0;
    int                    off_set = 0;
    char                  image_md5_value[33] = {0};
    char                  download_md5_str_value[33] = {0};
    unsigned int          read_size    = 0;
    char                  *rd_buf      = NULL;
    char                  test_buf[33] = { 0 };
    int                   bin_size     = 0;

    bin_size                 = ota_get_firm_size();
    OTA_LOG_I("bin_size = %d", bin_size);
    if (bin_size <= sizeof(ota_image_t)) {
        ret = -1;
        OTA_LOG_E("check image input parameters error!");
        return ret;
    }
    off_set = bin_size - sizeof(ota_image_t);
    OTA_LOG_I("bin_size = %d", bin_size);
    OTA_LOG_I("off_set = %d", off_set);
    if (ota_hal_read(&off_set, (char*)&ota_image_identity, sizeof(ota_image_t)) < 0) {
        ret = -1;
        OTA_LOG_E("image parse failed!");
        return ret;
    }
    memset(test_buf, 0x00, sizeof(test_buf));
    for (i = 0; i < 16; i++) {
        ota_snprintf((char *)(test_buf + i * 2), 2 + 1, "%02X",
                     ota_image_identity.image_md5_value[i]);
    }
    OTA_LOG_I("ota_image_identity.image_magic=%x",
              ota_image_identity.image_magic);
    OTA_LOG_I("ota_image_identity.image_size=%x",
              ota_image_identity.image_size);
    OTA_LOG_I("ota_image_identity.image_md5_value=%s", test_buf);
    OTA_LOG_I("ota_image_identity.image_crc16=%x",
              ota_image_identity.image_crc16);
    if ((ota_image_identity.image_magic != AOS_SINGLE_TAG) &&
        (ota_image_identity.image_magic != AOS_KERNEL_TAG) &&
        (ota_image_identity.image_magic != AOS_APP_TAG)) {
        ret = -1;
        OTA_LOG_E("image magic error");
        return ret;
    }
    if (ota_image_identity.image_size != bin_size - sizeof(ota_image_t)) {
        ret = -1;
        OTA_LOG_E("image size error!");
        return ret;
    }
    bin_size = ota_image_identity.image_size;
    rd_buf   = ota_malloc(OTA_BUF_VERIFY);
    if (rd_buf == NULL) {
        ret = -1;
        OTA_LOG_E("ota malloc identity read buf failed!");
        return ret;
    }
    if (ota_init_bin_md5_context() < 0) {
        ret = -1;
        OTA_LOG_E("init bin md5 context failed!");
        ota_free(rd_buf);
        return ret;
    }
    if (ota_hash_init(image_md5_ctx.hash_method, image_md5_ctx.ctx_hash) != 0) {
        ret = -1;
        OTA_LOG_E("ota iamge identiy md5 init fail \n ");
        goto OTA_IMAGE_IDENTITY_FAILED;
    }
    off_set = 0;
    while (off_set < bin_size) {
        (bin_size - off_set >= OTA_BUF_VERIFY) ? (read_size = OTA_BUF_VERIFY):(read_size = bin_size - off_set);
        if (ota_hal_read(&off_set, rd_buf, read_size) < 0) {
            ret = -1;
            OTA_LOG_E("image parse read flash failed!");
            goto OTA_IMAGE_IDENTITY_FAILED;
        }
        if (OTA_CRYPTO_SUCCESS != ota_hash_update((const uint8_t *)rd_buf,
                                                  read_size,
                                                  image_md5_ctx.ctx_hash)) {
            OTA_LOG_E("ota image identity md5 update failed!");
            ret = -1;
            goto OTA_IMAGE_IDENTITY_FAILED;
        }
    }

    memset(image_md5_value, 0x00, sizeof(image_md5_value));
    if (OTA_CRYPTO_SUCCESS !=
        ota_hash_final((unsigned char *)image_md5_value, image_md5_ctx.ctx_hash)) {
        ret = -1;
        OTA_LOG_E("ota image identity md5 final failed!");
        goto OTA_IMAGE_IDENTITY_FAILED;
    }
    memset(download_md5_str_value, 0x00, sizeof(download_md5_str_value));
    for (i = 0; i < 16; i++) {
        ota_snprintf((char *)download_md5_str_value + i * 2, 2 + 1, "%02X",
                     ota_image_identity.image_md5_value[i]);
    }
    if (ota_check_md5((const unsigned char *)image_md5_value, (const char *)&download_md5_str_value) <
        0) {
        ret = -1;
        OTA_LOG_E("image parse malloc failed!");
        goto OTA_IMAGE_IDENTITY_FAILED;
    }
    OTA_LOG_I("Great! OTA image is ok ....");
OTA_IMAGE_IDENTITY_FAILED:
    ota_free(rd_buf);
    ota_destroy_bin_md5_context();
    return ret;
}
