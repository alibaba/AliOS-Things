/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include <string.h>
#include "ota_download.h"
#include "ota_util.h"
#include "ota_log.h"
#include "ota_manifest.h"
#include "ota_verify.h"
#include "ota_hal_os.h"
#include "ota_hash.h"
#include "crc.h"

#define KEY_OTA_BREAKPOINT    "key_ota_breakpoint"
#define KEY_OTA_HASH          "key_ota_hash"
#define KEY_OTA_HASH_CTX_HEAD "key_ota_hash_ctx_head"
#define KEY_OTA_HASH_CTX_BODY "key_ota_hash_ctx_body"

#define OTA_BUF_VERIFY 512

static ota_hash_ctx_params g_ctx         = { 0, 0, NULL };
static ota_hash_ctx_params image_md5_ctx = { 0, 0, NULL };

static ota_image_info ota_image_identity;

int ota_malloc_global_context(ota_hash_type_t type)
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

ota_hash_ctx_params *ota_get_global_hash_context()
{
    return &g_ctx;
}

void ota_free_global_context()
{
    if (g_ctx.ctx_hash) {
        ota_free(g_ctx.ctx_hash);
    }
    g_ctx.ctx_hash    = NULL;
    g_ctx.hash_method = 0;
    g_ctx.ctx_size    = 0;
}

void ota_save_state(uint32_t breakpoint, ota_hash_ctx_params *hash_ctx)
{
#ifdef OTA_BREAKPOINT_SUPPORT
    ota_set_update_breakpoint(breakpoint);
    ota_set_cur_hash_context(hash_ctx);
#endif
}

uint32_t ota_get_update_breakpoint()
{
    uint32_t offset = 0;
#ifdef OTA_BREAKPOINT_SUPPORT
    int len = 4;
    if (ota_kv_get(KEY_OTA_BREAKPOINT, &offset, &len)) {
        offset = 0;
    }
#endif
    return offset;
}

int ota_set_update_breakpoint(uint32_t offset)
{
#ifdef OTA_BREAKPOINT_SUPPORT
    return ota_kv_set(KEY_OTA_BREAKPOINT, &offset, 4, 1);
#else
    return 0;
#endif
}

int ota_get_last_hash(char *value)
{
#ifdef OTA_BREAKPOINT_SUPPORT
    int len = 66;
    int ret = ota_kv_get(KEY_OTA_HASH, value, &len);
    return ret;
#else
    return 0;
#endif
}

int ota_set_cur_hash(char *value)
{
#ifdef OTA_BREAKPOINT_SUPPORT
    return ota_kv_set(KEY_OTA_HASH, value, 66, 1);
#else
    return 0;
#endif
}

int ota_get_last_hash_context(ota_hash_ctx_params *hash_ctx)
{
#ifdef OTA_BREAKPOINT_SUPPORT
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
#else
    return 0;
#endif
}

int ota_set_cur_hash_context(ota_hash_ctx_params *hash_ctx)
{
#ifdef OTA_BREAKPOINT_SUPPORT
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
#else
    return 0;
#endif
}

int ota_verify_hash_value(ota_hash_params last_hash, ota_hash_params cur_hash)
{
    int ret         = 0;
    int compare_len = 0;
    if (last_hash.hash_method != cur_hash.hash_method) {
        ret = -1;
    }

    if (last_hash.hash_method == MD5) {
        compare_len = 32;
    } else {
        compare_len = 64;
    }

    if (strncmp(last_hash.hash_value, cur_hash.hash_value, compare_len) != 0) {
        ret = -1;
    }
    return ret;
}

static int ota_check_md5(const uint8_t *cur_hash, const char *download_hash)
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

static int ota_check_sha256(const uint8_t *cur_hash, const char *download_hash)
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

int ota_check_hash_value(ota_hash_params *download_hash)
{
    if (download_hash == NULL) {
        return -1;
    }
    int                  ret        = -1;
    uint8_t              digest[64] = { 0 };
    ota_hash_ctx_params *hash_ctx   = ota_get_global_hash_context();
    if (OTA_CRYPTO_SUCCESS != ota_hash_final(digest, hash_ctx->ctx_hash)) {
        return -1;
    }
    switch (hash_ctx->hash_method) {
        case SHA256: {
            ret = ota_check_sha256(digest, download_hash->hash_value);
            break;
        }
        case MD5: {
            ret = ota_check_md5(digest, download_hash->hash_value);
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

int ota_check_image(ota_read_cb_t read_fuc)
{
    int                   ret     = 0;
    int                   i       = 0;
    volatile uint32_t     off_set = 0;
    CRC16_Context         bin_crc16_Context;
    unsigned short        bin_parse_context_crc = 0;
    unsigned char         image_md5_value[33] = {0};
    unsigned char         download_md5_str_value[33] = {0};
    unsigned int          read_size    = 0;
    unsigned char        *rd_buf       = NULL;
    unsigned char         test_buf[33] = { 0 };
    int                   bin_size     = 0;

    ota_service_manager *ctx = (ota_service_manager *)get_ota_service_manager();
    bin_size                 = ctx->firm_size;
    OTA_LOG_I("bin_size = %d", bin_size);
    if ((NULL == read_fuc) || (bin_size <= sizeof(ota_image_info))) {
        ret = -1;
        OTA_LOG_E("check image input parameters error!");
        return ret;
    }
    off_set = bin_size - sizeof(ota_image_info);
    OTA_LOG_I("bin_size = %d", bin_size);
    OTA_LOG_I("off_set = %d", off_set);
    if (read_fuc(&off_set, (unsigned char *)&ota_image_identity,
                 sizeof(ota_image_info), 0x01) < 0) {
        ret = -1;
        OTA_LOG_E("image parse failed!");
        return ret;
    }
    CRC16_Init(&bin_crc16_Context);
    CRC16_Update(&bin_crc16_Context, (void *)&ota_image_identity,
                 sizeof(ota_image_info) -
                   sizeof(ota_image_identity.image_crc16));
    CRC16_Final(&bin_crc16_Context, &bin_parse_context_crc);
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
    /*OTA_LOG_I("bin_parse_context_crc=0x%04x", bin_parse_context_crc);
    if(ota_image_identity.image_crc16 != bin_parse_context_crc) {
        ret = -1;
        OTA_LOG_E("image parse crc16 error!");
        return ret;
    }*/
    if ((ota_image_identity.image_magic != AOS_SINGLE_TAG) &&
        (ota_image_identity.image_magic != AOS_KERNEL_TAG) &&
        (ota_image_identity.image_magic != AOS_APP_TAG)) {
        ret = -1;
        OTA_LOG_E("image magic error");
        return ret;
    }
    if (ota_image_identity.image_size != bin_size - sizeof(ota_image_info)) {
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
        (bin_size - off_set >= OTA_BUF_VERIFY) ? (read_size = OTA_BUF_VERIFY)
                                    : (read_size = bin_size - off_set);
        if (read_fuc(&off_set, rd_buf, read_size, 0x01) < 0) {
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
        ota_hash_final(image_md5_value, image_md5_ctx.ctx_hash)) {
        ret = -1;
        OTA_LOG_E("ota image identity md5 final failed!");
        goto OTA_IMAGE_IDENTITY_FAILED;
    }
    memset(download_md5_str_value, 0x00, sizeof(download_md5_str_value));
    for (i = 0; i < 16; i++) {
        ota_snprintf((char *)download_md5_str_value + i * 2, 2 + 1, "%02X",
                     ota_image_identity.image_md5_value[i]);
    }
    if (ota_check_md5(image_md5_value, (const char *)&download_md5_str_value) <
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
