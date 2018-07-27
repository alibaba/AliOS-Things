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

#define KEY_OTA_BREAKPOINT  "key_ota_breakpoint"
#define KEY_OTA_SIGN        "key_ota_sign"
#define KEY_OTA_SIGN_CTX    "key_ota_sign_ctx"

#define OTA_BUFFER_MAX_SIZE  1536

static ota_sign_ctx_params g_ctx = {0, 0, NULL};

int ota_malloc_global_context(hash_type_t type)
{
    g_ctx.sign_method = type;
    ali_hash_get_ctx_size(type, (size_t *)&g_ctx.ctx_size);
    if (g_ctx.ctx_sign == NULL) {
        g_ctx.ctx_sign = (void*)ota_malloc(g_ctx.ctx_size);
    }
    if (g_ctx.ctx_sign == NULL) {
        g_ctx.sign_method = 0;
        g_ctx.ctx_size = 0;
        return -1;
    }

    memset(g_ctx.ctx_sign, 0, g_ctx.ctx_size);
    return 0;
}

ota_sign_ctx_params *ota_get_global_sign_context()
{
    return &g_ctx;
}

void ota_free_global_context()
{
    if (g_ctx.ctx_sign) {
        ota_free(g_ctx.ctx_sign);

    }
    g_ctx.ctx_sign = NULL;
    g_ctx.sign_method = 0;
    g_ctx.ctx_size = 0;
}

void ota_save_state(uint32_t breakpoint, ota_sign_ctx_params *signctx)
{
#ifdef OTA_BREAKPOINT_SUPPORT
    ota_set_update_breakpoint(breakpoint);
    ota_set_cur_sign_context(signctx);
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
    return  ota_kv_set(KEY_OTA_BREAKPOINT, &offset, 4, 1);
#else
    return 0;
#endif
}

int ota_get_last_sign(char *value)
{
#ifdef OTA_BREAKPOINT_SUPPORT
    int len = 66;
    int ret = ota_kv_get(KEY_OTA_SIGN, value, &len);
    return ret;
#else
    return 0;
#endif
}

int ota_set_cur_sign(char *value)
{
#ifdef OTA_BREAKPOINT_SUPPORT
    return  ota_kv_set(KEY_OTA_SIGN, value, 66, 1);
#else
    return 0;
#endif
}

int ota_get_last_sign_context(ota_sign_ctx_params *signctx)
{
#ifdef OTA_BREAKPOINT_SUPPORT
    if (signctx == NULL || signctx->ctx_sign == NULL || signctx->ctx_size == 0) {
        return 0;
    }
    int len = sizeof signctx->sign_method + sizeof signctx->ctx_size + signctx->ctx_size;
    return ota_kv_get(KEY_OTA_SIGN_CTX, signctx, &len);
#else
    return 0;
#endif
}

int ota_set_cur_sign_context(ota_sign_ctx_params *signctx)
{
#ifdef OTA_BREAKPOINT_SUPPORT
    if (signctx == NULL || signctx->ctx_sign == NULL || signctx->ctx_size == 0) {
        return 0;
    }
    int len = sizeof signctx->sign_method + sizeof signctx->ctx_size + signctx->ctx_size;
    return  ota_kv_set(KEY_OTA_SIGN_CTX, signctx, len, 1);
#else
    return 0;
#endif
}

int ota_verify_sign(ota_sign_params last_sign, ota_sign_params cur_sign)
{
    int ret = 0;
    int compare_len = 0;
    if (last_sign.sign_method != cur_sign.sign_method) {
        ret = -1;
    }

    if (last_sign.sign_method == MD5) {
        compare_len = 32;
    } else {
        compare_len = 64;
    }

    if (strncmp(last_sign.sign_value, cur_sign.sign_value, compare_len) != 0) {
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
    char digest_str[33] = {0};
    int i = 0;
    for (; i < 16 ; i++) {
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
    char digest_str[65] = {0};
    int i = 0;
    for (; i < 32 ; i++) {
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

int ota_check_sign(ota_sign_params *download_sign)
{
    if (download_sign == NULL) {
        return -1;
    }
    int ret = -1;
    uint8_t digest[64] = {0};
    ota_sign_ctx_params *sign_ctx = ota_get_global_sign_context();
    if (ALI_CRYPTO_SUCCESS != ali_hash_final(digest, sign_ctx->ctx_sign)) {
        return -1;
    }
    switch (sign_ctx->sign_method) {
        case SHA256: {
            ret = ota_check_sha256(digest, download_sign->sign_value);
            break;
        }
        case MD5: {
            ret = ota_check_md5(digest, download_sign->sign_value);
            break;
        }
        default:
            return -1;
    }
    return ret;
}

