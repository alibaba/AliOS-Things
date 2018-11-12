/*
 *Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include <string.h>
#include "ota_service.h"
#include "ota_hal_os.h"
#include "ota_verify.h"
#include "ota_rsa_verify.h"
#include "ota_hal_plat.h"
#include "ota_log.h"

#if (defined BOARD_ESP8266)
#include "aos/aos_debug.h"
#endif

static ota_service_t *ctx = NULL;
static ota_boot_param_t ota_param ={0};
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

const char *ota_get_ota_version(void)
{
    return ctx->ota_ver;
}

void ota_set_ota_version(char* ver)
{
    strncpy(ctx->ota_ver,ver,sizeof(ctx->ota_ver));
}

void ota_set_firm_size(unsigned int size)
{
    ota_param.len = size;
}

unsigned int ota_get_firm_size(void)
{
    return ota_param.len;
}


void ota_set_upg_flag(unsigned int upd_flag)
{
    ota_param.upg_flag = upd_flag;
}

void ota_set_status(char status)
{
    ota_param.upg_status = status;
}

char ota_get_status(void)
{
    return ota_param.upg_status;
}

void ota_set_splict_size(unsigned int size)
{
    ota_param.splict_size = size;
}

static void ota_download_thread(void *hand)
{
    int ret = 0;
#if (defined BOARD_ESP8266)
    ktask_t* h = NULL;
    h = aos_debug_task_find("linkkit");
    if(h)
    krhino_task_dyn_del(h);
    ota_msleep(500);
    #ifdef WIFI_PROVISION_ENABLED
       extern int awss_suc_notify_stop(void);
       awss_suc_notify_stop();
    #endif
    #ifdef DEV_BIND_ENABLED
    extern int awss_dev_bind_notify_stop(void);
    awss_dev_bind_notify_stop();
    #endif
    OTA_LOG_I("awss_notify stop.");
    h = aos_debug_task_find("event_task");
    if(h)
    krhino_task_dyn_del(h);
    ota_msleep(500);
#endif
    ota_param.off_bp = ota_get_break_point();
    ota_param.res_type = OTA_FINISH;
    ret = ota_hal_init((void *)(&ota_param));
    if(ret < 0) {
        OTA_LOG_E("hal init error.");
        ota_set_status(OTA_UPGRADE_FAIL);
        goto ERR;
    }
    ret = ota_malloc_hash_ctx(ctx->h_tr->hash_type);
    if (ret < 0) {
        OTA_LOG_E("ota download error ret:%d",ret);
        ota_set_status(OTA_DOWNLOAD_FAIL);
        goto ERR;
    }
    ret = ctx->h_dl->start(ctx->h_tr->url);
    if (ret < 0) {
        OTA_LOG_E("ota download error ret:%d",ret);
        ota_set_status(OTA_DOWNLOAD_FAIL);
        goto ERR;
    }
    if (ret == OTA_CANCEL) {
        OTA_LOG_E("ota download cancel");
        ota_param.res_type = OTA_BREAKPOINT;
        ret = ota_hal_boot((void *)(&ota_param));
        if(ret < 0) {
            OTA_LOG_E("hal boot error.");
        }
        ota_set_status(OTA_CANCEL);
        goto ERR;
    }
    ret = ota_check_hash((OTA_HASH_E)ctx->h_tr->hash_type, ctx->h_tr->hash);
    if (ret < 0) {
        OTA_LOG_E("ota check hash error");
        ota_set_status(OTA_VERIFY_HASH_FAIL);
        goto ERR;
    }
    if( ctx->h_tr->sign_en == OTA_SIGN_ON) {
        ret = ota_verify_download_rsa_sign((unsigned char*)ctx->h_tr->sign, (const char*)ctx->h_tr->hash, (OTA_HASH_E)ctx->h_tr->hash_type);
        if(ret < 0) {
            OTA_LOG_E("ota verify rsa error");
            ota_set_status(OTA_VERIFY_RSA_FAIL);
            goto ERR;
        }
    }
    if(ota_param.upg_flag != OTA_DIFF){
        ret = ota_check_image();
        if (ret < 0) {
            OTA_LOG_E("ota check image failed");
            ota_set_status(OTA_VERIFY_HASH_FAIL);
            goto ERR;
        }
    }
    ota_set_status(OTA_UPGRADE);
    ota_param.res_type = OTA_FINISH;
    ret = ota_hal_boot((void *)(&ota_param));
    if(ret < 0) {
        OTA_LOG_E("init error.");
    }
    ota_set_status(OTA_REBOOT);
    ota_set_break_point(0);
ERR:
#if (!defined BOARD_ESP8266)
    ctx->h_tr->status(100,ctx->pk, ctx->dn);
#endif
    ota_free_hash_ctx();
    OTA_LOG_I("reboot system after 3 second!");
    ota_msleep(3000);
    OTA_LOG_I("task update over");
    ota_reboot();
}

static int ota_check_version(void) {
    int   ret = 0;
    int is_ota = strncmp(ctx->ota_ver,ota_get_sys_version(),strlen(ctx->ota_ver));
    if(is_ota > 0) {
        OTA_LOG_E("A new OTA update ready, will be enter OTA download.");
    } else {
        OTA_LOG_E("ota version is older than system version, discard it.");
        ota_set_status(OTA_INIT_VER_FAIL);
        ctx->h_tr->status(0, ctx->pk, ctx->dn);
        return -1;
    }
    return ret;
}

void ota_upgrade_cb(char *json) {
    int ret = 0;
    if (!ctx || !ctx->h_tr || !json) {
        OTA_LOG_E("update parameter is null");
        return;
    }
    
    OTA_LOG_I("ota upgrade start: pk:%s dn:%s app ver:%s", ctx->pk, ctx->dn, ota_get_sys_version());
    if (0 == ctx->h_tr->response(json)) {
        ret = ota_check_version();
        if(ret == 0) {
            ota_set_status(OTA_DOWNLOAD);
            ctx->h_tr->status(0, ctx->pk, ctx->dn);
            void *thread = NULL;
            ota_thread_create(&thread, (void *)ota_download_thread, (void *)NULL, NULL, 0);
        }
    }
}

int ota_service_init(ota_service_t *pctx) {
    ota_hal_register_module(&ota_hal_module);
    if (!pctx) {
        pctx = ota_malloc(sizeof(ota_service_t));
        memset(pctx, 0, sizeof(ota_service_t));
        OTA_LOG_I("OTA init default ctx.");
        pctx->trans_protcol = OTA_PROTCOL_MQTT;
        pctx->dl_protcol    = OTA_PROTCOL_HTTPS;
    }
    if(!pctx) {
        OTA_LOG_E("CTX is NULL, failed.");
        return -1;
    }
    ctx = pctx;
    if (ctx->inited) {
        OTA_LOG_E("OTA is inited.");
        return -1;
    }
    memset(&ota_param,0,sizeof(ota_boot_param_t));
    ctx->inited = 1;
    if(OTA_PROTCOL_COAP == ctx->trans_protcol){
        ctx->h_tr = ota_get_transport_coap();
    }else{
        ctx->h_tr = ota_get_transport_mqtt();
    }

    if(OTA_PROTCOL_COAP == ctx->dl_protcol){
        ctx->h_dl = ota_get_download_coap();
    }else{
        ctx->h_dl = ota_get_download_http();
    }
    OTA_LOG_I("OTA service pk:%s dn:%s", ctx->pk, ctx->dn);
    ctx->h_tr->init();
    ota_set_status(OTA_INIT);
    ctx->h_tr->status(0, ctx->pk, ctx->dn);
    ctx->h_tr->inform(ota_get_sys_version(), ctx->pk, ctx->dn);
    ctx->h_tr->upgrade(&ota_upgrade_cb, ctx->pk, ctx->dn);
    ota_hal_rollback(NULL);
    return 0;
}

int ota_service_deinit(ota_service_t *pctx) {
    if(pctx->h_tr)
    pctx->h_tr->deinit();
    if(pctx->h_ch) {
        ota_free(pctx->h_ch);
        pctx->h_ch = NULL;
        OTA_LOG_I("free h_ch");
    }
    pctx->h_tr = NULL;
    pctx->h_dl = NULL;
    pctx->h_ver = NULL;
    pctx->inited = 0;
    if(pctx){
	ota_free(pctx);
        pctx = NULL;
    }
    return 0;
}

ota_service_t *ota_get_service(void) {
    return ctx;
}
