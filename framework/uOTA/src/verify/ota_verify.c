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
#define KEY_OTA_HASH         "key_ota_hash"
#define KEY_OTA_HASH_CTX     "key_ota_hash_ctx"

#define OTA_BUFFER_MAX_SIZE  1536

static ota_hash_ctx_params g_ctx = {0, 0, NULL};

int ota_malloc_global_context(hash_type_t type)
{
    g_ctx.hash_method = type;
    ali_hash_get_ctx_size(type, (size_t *)&g_ctx.ctx_size);
    if (g_ctx.ctx_hash == NULL) {
        g_ctx.ctx_hash = (void*)ota_malloc(g_ctx.ctx_size);
    }
    if (g_ctx.ctx_hash == NULL) {
        g_ctx.hash_method = 0;
        g_ctx.ctx_size = 0;
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
    g_ctx.ctx_hash = NULL;
    g_ctx.hash_method = 0;
    g_ctx.ctx_size = 0;
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
    return  ota_kv_set(KEY_OTA_BREAKPOINT, &offset, 4, 1);
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
    return  ota_kv_set(KEY_OTA_HASH, value, 66, 1);
#else
    return 0;
#endif
}

int ota_get_last_hash_context(ota_hash_ctx_params *hash_ctx)
{
#ifdef OTA_BREAKPOINT_SUPPORT
    if (hash_ctx == NULL || hash_ctx->ctx_hash == NULL || hash_ctx->ctx_size == 0) {
        return 0;
    }
    int len = sizeof hash_ctx->hash_method + sizeof hash_ctx->ctx_size + hash_ctx->ctx_size;
    return ota_kv_get(KEY_OTA_HASH_CTX, hash_ctx, &len);
#else
    return 0;
#endif
}

int ota_set_cur_hash_context(ota_hash_ctx_params *hash_ctx)
{
#ifdef OTA_BREAKPOINT_SUPPORT
    if(hash_ctx == NULL || hash_ctx->ctx_hash == NULL || hash_ctx->ctx_size == 0) {
        return 0;
    }
    int len = sizeof hash_ctx->hash_method + sizeof hash_ctx->ctx_size + hash_ctx->ctx_size;
    return  ota_kv_set(KEY_OTA_HASH_CTX, hash_ctx, len, 1);
#else
    return 0;
#endif
}

int ota_verify_hash_value(ota_hash_params last_hash, ota_hash_params cur_hash)
{
    int ret = 0;
    int compare_len = 0;
    if (last_hash.hash_method != cur_hash.hash_method) {
        ret = -1; 
    }
    
    if(last_hash.hash_method == MD5) {
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

int ota_check_hash_value(ota_hash_params *download_hash)
{
    if(download_hash == NULL) {
        return -1;
    }
    int ret = -1;
    uint8_t digest[64] = {0};
    ota_hash_ctx_params *hash_ctx = ota_get_global_hash_context(); 
    if(ALI_CRYPTO_SUCCESS != ali_hash_final(digest, hash_ctx->ctx_hash)) {
        return -1;
    }
    switch(hash_ctx->hash_method) {
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

