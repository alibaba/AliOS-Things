/*
 *Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include <string.h>
#include <cJSON.h>
#include "ota/ota_service.h"
#include "ota_hal_os.h"
#include "ota_verify.h"
#include "ota_hal_plat.h"
#include "ota_log.h"
#include "aos/kernel.h"

extern ota_hal_module_t ota_hal_module;
const char *ota_to_capital(char *value, int len)
{
    if (value == NULL || len <= 0) {
        return NULL;
    }
    int i = 0;
    for (; i < len; i++) {
        if (*(value + i) >= 'a' && *(value + i) <= 'z') {
            *(value + i) -= 'a' - 'A';
        }
    }
    return value;
}

int ota_hex_str2buf(const char* src, char* dest, unsigned int dest_len)
{
    int i, n = 0;
    if(src == NULL || dest == NULL) {
        return -1;
    }
    if(dest_len < strlen(src) / 2) {
        return -1;
    }
    for(i = 0; src[i]; i += 2) {
        if(src[i] >= 'A' && src[i] <= 'F')
            dest[n] = src[i] - 'A' + 10;
        else
            dest[n] = src[i] - '0';
        if(src[i + 1] >= 'A' && src[i + 1] <= 'F')
            dest[n] = (dest[n] << 4) | (src[i + 1] - 'A' + 10);
        else dest[n] = (dest[n] << 4) | (src[i + 1] - '0');
        ++n;
    }
    return n;
}

static int ota_parse(void* pctx, const char *json)
{
    int ret = 0;
    cJSON *root = NULL;
    ota_service_t* ctx = pctx;
    if(!ctx) {
        ret = OTA_PARAM_FAIL;
        goto parse_failed;
    }
    char *url = ctx->url;
    char *hash = ctx->hash;
    unsigned char *sign = ctx->sign;
    ota_boot_param_t *ota_param = (ota_boot_param_t *)ctx->boot_param;
    if(!url || !hash || !ota_param) {
        ret = OTA_PARAM_FAIL;
        goto parse_failed;
    }

    root = cJSON_Parse(json);
    if (!root) {
        ret = OTA_PARAM_FAIL;
        goto parse_failed;
    } else {
        cJSON *message = cJSON_GetObjectItem(root, "message");
        if (NULL == message) {
            ret = OTA_PARSE_FAIL;
            goto parse_failed;
        }
        if ((strncmp(message->valuestring, "success", strlen("success")))) {
            ret = OTA_PARSE_FAIL;
            goto parse_failed;
        }
        cJSON *json_obj = cJSON_GetObjectItem(root, "data");
        if (!json_obj) {
            ret = OTA_PARSE_FAIL;
            goto parse_failed;
        }
        cJSON *resourceUrl = cJSON_GetObjectItem(json_obj, "url");
        if (!resourceUrl) {
            ret = OTA_PARSE_FAIL;
            goto parse_failed;
        }
        cJSON *version = cJSON_GetObjectItem(json_obj, "version");
        if (!version) {
            ret = OTA_PARSE_FAIL;
            goto parse_failed;
        }
        strncpy(ctx->ota_ver, version->valuestring, sizeof(ctx->ota_ver));
        strncpy(url, resourceUrl->valuestring, OTA_URL_LEN - 1);
        cJSON *signMethod = cJSON_GetObjectItem(json_obj, "signMethod");
        if (signMethod) {
            memset(hash, 0x00, OTA_HASH_LEN);
            ota_to_capital(signMethod->valuestring, strlen(signMethod->valuestring));
            if (0 == strncmp(signMethod->valuestring, "MD5", strlen("MD5"))) {
                cJSON *md5 = cJSON_GetObjectItem(json_obj, "sign");
                if (!md5) {
                    ret = OTA_PARSE_FAIL;
                    goto parse_failed;
                }
                ctx->hash_type = MD5;
                strncpy(hash, md5->valuestring, strlen(md5->valuestring) + 1);
                hash[strlen(md5->valuestring)] = '\0';
                ota_to_capital(hash, strlen(hash));
            } else if (0 == strncmp(signMethod->valuestring, "SHA256", strlen("SHA256"))) {
                cJSON *sha256 = cJSON_GetObjectItem(json_obj, "sign");
                if (!sha256) {
                    ret = OTA_PARSE_FAIL;
                    goto parse_failed;
                }
                ctx->hash_type = SHA256;
                strncpy(hash, sha256->valuestring, strlen(sha256->valuestring) + 1);
                hash[strlen(sha256->valuestring)] = '\0';
                ota_to_capital(hash, strlen(hash));
            } else {
                ret = OTA_PARSE_FAIL;
                goto parse_failed;
            }
        } else { /* old protocol*/
            memset(hash, 0x00, OTA_HASH_LEN);
            cJSON *md5 = cJSON_GetObjectItem(json_obj, "md5");
            if (!md5) {
                ret = OTA_PARSE_FAIL;
                goto parse_failed;
            }
            ctx->hash_type = MD5;
            strncpy(hash, md5->valuestring, strlen(md5->valuestring) + 1);
            hash[strlen(md5->valuestring)] = '\0';
            ota_to_capital(hash, strlen(hash));
        }
        cJSON *size = cJSON_GetObjectItem(json_obj, "size");
        if (!size) {
            ret = OTA_PARSE_FAIL;
            goto parse_failed;
        }
        ota_param->len = size->valueint;
        ota_param->upg_flag = OTA_RAW;
        cJSON *diff = cJSON_GetObjectItem(json_obj, "isDiff");
        if (diff) {
            int is_diff = diff->valueint;
            if (is_diff) {
                ota_param->rec_size = size->valueint;
                ota_param->upg_flag = OTA_DIFF;
                cJSON *splictSize = cJSON_GetObjectItem(json_obj, "splictSize");
                if (splictSize) {
                    ota_param->splict_size = splictSize->valueint;
                }
            }
        }
        cJSON *digestSign = cJSON_GetObjectItem(json_obj, "digestsign");
        if (digestSign) {
           memset(sign, 0x00, OTA_SIGN_LEN);
           ctx->sign_en = OTA_SIGN_ON;
           ctx->sign_len = OTA_SIGN_LEN;
           if(ota_base64_decode((const unsigned char*)digestSign->valuestring, strlen(digestSign->valuestring), sign, &ctx->sign_len) != 0 ) {
                ret = OTA_PARSE_FAIL;
                goto parse_failed;
            }
            else if(ctx->sign_len != OTA_SIGN_LEN) {
                ret = OTA_PARSE_FAIL;
                goto parse_failed;
            }
        } else {
           ctx->sign_en = OTA_SIGN_OFF;
        }
    }
    goto parse_success;
parse_failed:
    OTA_LOG_E("parse failed err:%d", ret);
    if (root) {
        cJSON_Delete(root);
    }
    return -1;

parse_success:
    if (root) {
        cJSON_Delete(root);
    }
    return 0;
}

static void ota_download_thread(void *hand)
{
    int ret = 0;
    int tmp_breakpoint = 0;
    ota_hash_t last_hash  = {0};
    ota_service_t* ctx = hand;
    if (!ctx) {
        OTA_LOG_E("ctx is NULL.\n");
        return;
    }
    ota_boot_param_t *ota_param = (ota_boot_param_t *)ctx->boot_param;
    if (!ctx->boot_param) {
        ret = OTA_PARAM_FAIL;
        ctx->upg_status = OTA_DOWNLOAD_FAIL;
        goto ERR;
    }
#if (defined BOARD_ESP8266)
    aos_task_delete("linkkit");
    ota_msleep(500);
    #ifdef WIFI_PROVISION_ENABLED
       extern int awss_suc_notify_stop(void);
       awss_suc_notify_stop();
    #endif
    #ifdef DEV_BIND_ENABLED
    extern int awss_dev_bind_notify_stop(void);
    awss_dev_bind_notify_stop();
    #endif
    aos_task_delete("event_task");
    ota_msleep(500);
#endif
    tmp_breakpoint = ota_get_break_point();
    memset(&last_hash, 0x00, sizeof(last_hash));
    ota_get_last_hash((char *)&last_hash);
    if (tmp_breakpoint && (strncmp((char*)&last_hash, ctx->hash, OTA_HASH_LEN) == 0)) {
        ota_param->off_bp = ota_get_break_point();
    } else {
        ota_param->off_bp = 0;
    }
    ota_param->res_type = OTA_FINISH;
    ret = ota_hal_init((void *)(ota_param));
    if(ret < 0) {
        ctx->upg_status = OTA_DOWNLOAD_FAIL;
        goto ERR;
    }
    ret = ota_malloc_hash_ctx(ctx->hash_type);
    if (ret < 0) {
        ret = OTA_PARAM_FAIL;
        ctx->upg_status = OTA_DOWNLOAD_FAIL;
        goto ERR;
    }
    ctx->upg_status = OTA_DOWNLOAD;
    ctx->h_tr->status(0, ctx);
    ret = ctx->h_dl->start((void*)ctx);
    if (ret < 0) {
        ota_param->res_type = OTA_BREAKPOINT;
        ret = ota_hal_boot((void *)(ota_param));
        ctx->upg_status = OTA_DOWNLOAD_FAIL;
        goto ERR;
    }
    if (ret == OTA_CANCEL) {
        ota_param->res_type = OTA_BREAKPOINT;
        ret = ota_hal_boot((void *)(ota_param));
        ctx->upg_status = OTA_CANCEL;
        goto ERR;
    }
    ret = ota_check_hash((OTA_HASH_E)ctx->hash_type, ctx->hash);
    if (ret < 0) {
        ctx->upg_status = OTA_VERIFY_HASH_FAIL;
        goto ERR;
    }
    if( ctx->sign_en == OTA_SIGN_ON) {
#if defined OTA_CONFIG_RSA
        ret = ota_verify_download_rsa_sign((unsigned char*)ctx->sign, (const char*)ctx->hash, (OTA_HASH_E)ctx->hash_type);
        if(ret < 0) {
            ctx->upg_status = OTA_VERIFY_RSA_FAIL;
            goto ERR;
        }
#endif
    }
    if(ota_param->upg_flag != OTA_DIFF){
        ret = ota_check_image(ota_param->len);
        if (ret < 0) {
            ctx->upg_status = OTA_VERIFY_HASH_FAIL;
            goto ERR;
        }
    }

    ota_param->res_type = OTA_FINISH;
    ret = ota_hal_boot(ota_param);
    ctx->upg_status = OTA_REBOOT;
    ota_set_break_point(0);
ERR:
    OTA_LOG_E("upgrade over err:%d", ret);
#if (!defined BOARD_ESP8266)
    ctx->h_tr->status(100, ctx);
#endif
    ota_free_hash_ctx();
    ota_msleep(3000);
    ota_reboot();
}

int ota_upgrade_cb(void* pctx, char *json)
{
    ota_service_t* ctx = pctx;
    if (!ctx || !json) {
        return -1;
    }
    if (0 == ota_parse(ctx, json)) {
        int is_ota = strncmp(ctx->ota_ver, ctx->sys_ver, strlen(ctx->ota_ver));
        if(is_ota > 0) {
            void *thread = NULL;
#if defined(OTA_CONFIG_TLS)
            ota_thread_create(&thread, (void *)ota_download_thread, (void *)ctx, NULL, 1024 * 6);
#else
            ota_thread_create(&thread, (void *)ota_download_thread, (void *)ctx, NULL, 1024 * 4);
#endif
        } else {
            OTA_LOG_E("ota version is too old, discard it.");
            ctx->upg_status = OTA_INIT_VER_FAIL;
            ctx->h_tr->status(0, ctx);
            return -1;
        }
    }
    return 0;
}

int ota_service_init(ota_service_t *ctx)
{
    int ret = 0;
    if (!ctx) {
        ctx = ota_malloc(sizeof(ota_service_t));
        memset(ctx, 0, sizeof(ota_service_t));
    }
    if(!ctx) {
        ret = OTA_INIT_FAIL;
        return ret;
    }
    ota_hal_register_module(&ota_hal_module);
    ctx->upgrade_cb = ota_upgrade_cb;
    ctx->boot_param = ota_malloc(sizeof(ota_boot_param_t));
    if(!ctx->boot_param) {
        ret = OTA_INIT_FAIL;
        return ret;
    }
    memset(ctx->boot_param, 0, sizeof(ota_boot_param_t));
    if(ctx->inited) {
        ret = OTA_INIT_FAIL;
        return ret;
    }
    ctx->inited = 1;
    ctx->url = ota_malloc(OTA_URL_LEN);
    if(NULL == ctx->url){
        ret = OTA_INIT_FAIL;
        return ret;
    }
    memset(ctx->url, 0, OTA_URL_LEN);
    ctx->hash = ota_malloc(OTA_HASH_LEN);
    if(NULL == ctx->hash){
        ret = OTA_INIT_FAIL;
        return ret;
    }
    memset(ctx->hash, 0, OTA_HASH_LEN);
    ctx->sign = ota_malloc(OTA_SIGN_LEN);
    if(NULL == ctx->sign){
        ret = OTA_INIT_FAIL;
        return ret;
    }
    strncpy(ctx->sys_ver, ota_hal_get_version(ctx->dev_type), sizeof(ctx->sys_ver) - 1);
    memset(ctx->sign, 0, OTA_SIGN_LEN);
    ctx->h_tr = ota_get_transport();
    ctx->h_dl = ota_get_download();
    ctx->h_tr->init();
    ret = ctx->h_tr->inform(ctx);
    if(ret < 0){
        return ret;
    }
    ret = ctx->h_tr->upgrade(ctx);
    OTA_LOG_I("ota init success, ver:%s type:%d", ctx->sys_ver, ctx->dev_type);
    ota_hal_rollback(NULL);
    return ret;
}

int ota_service_deinit(ota_service_t *ctx)
{
    if(!ctx) {
        return -1;
    }
    if(ctx->h_ch) {
        ota_free(ctx->h_ch);
        ctx->h_ch = NULL;
    }
    ctx->h_tr = NULL;
    ctx->h_dl = NULL;
    ctx->inited = 0;
    if(ctx->url){
        ota_free(ctx->url);
        ctx->url = NULL;
    }
    if(ctx->hash){
        ota_free(ctx->hash);
        ctx->hash = NULL;
    }
    if(ctx->sign){
        ota_free(ctx->sign);
        ctx->sign = NULL;
    }
    if(ctx->boot_param){
        ota_free(ctx->boot_param);
        ctx->boot_param = NULL;
    }
    if(ctx){
        ota_free(ctx);
        ctx = NULL;
    }
    return 0;
}
