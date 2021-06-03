/*
 *Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */
#include <string.h>
#include "ota_log.h"
#include "ota_import.h"
#include "ota_hal_os.h"
#include "ota_hal.h"

#ifdef AOS_COMP_PWRMGMT
#include "aos/pwrmgmt.h"
#endif

#if !defined(OTA_LINUX) && defined OTA_CONFIG_SECURE_DL_MODE
static wdg_dev_t ota_wdg = {0};
static aos_timer_t  ota_mon_tmr = {0};
static void ota_monitor_task(void *arg1, void* arg2)
{
    hal_wdg_reload(&ota_wdg);
}
#endif

int ota_download_to_fs_service(void *ota_ctx , char *file_path)
{
    /* OTA download init */
    int ret = 0;
    ota_boot_param_t ota_param = {0};
    ota_service_t *ctx = ota_ctx;
    if (ctx == NULL || file_path == NULL) {
        goto SERVER_OVER;
    }
    ret = ota_read_parameter(&ota_param);
    if (ret < 0) {
        goto SERVER_OVER;
    }
    ota_param.url[sizeof(ota_param.url) - 1] = 0;
    ret = ota_download_store_fs_start(ota_param.url, strlen(ota_param.url), file_path,
          ctx->report_func.report_status_cb, ctx->report_func.param);
    if (ret < 0) {
        goto SERVER_OVER;
    }
    OTA_LOG_I("verify subdev ota fs file\n");
    ret = ota_verify_fsfile(&ota_param, file_path);
    if (ret < 0) {
        OTA_LOG_E("verify subdev ota fs file failed\n");
        goto SERVER_OVER;
    }

SERVER_OVER:
    OTA_LOG_I("Subdev download complete ret = %d\n", ret);
    if (ret < 0) {
        if ((ctx != NULL) && (ctx->report_func.report_status_cb !=  NULL)) {
            ctx->report_func.report_status_cb(ctx->report_func.param, ret);
        }
    }
    if ((ctx != NULL) && (ctx->feedback_func.on_user_event_cb != NULL)) {
        ctx->feedback_func.on_user_event_cb(OTA_EVENT_DOWNLOAD, ret, ctx->feedback_func.param);
    }
    return ret;
}

int ota_report_module_version(void *ota_ctx, char *module_name, char *version)
{
    int ret = -1;
    ota_service_t *ctx = ota_ctx;
    OTA_LOG_I("report submode version\n");
    if (ctx == NULL) {
        OTA_LOG_E("report submode version ctx is null\n");
        goto OTA_REPORT_M_VER_OVER;
    }
    if (version == NULL) {
        OTA_LOG_E("submode version is null\n");
        goto OTA_REPORT_M_VER_OVER;
    }
    ret = ota_transport_inform(ctx->mqtt_client, ctx->pk, ctx->dn, module_name, version);
    if (ctx->feedback_func.on_user_event_cb != NULL) {
        ctx->feedback_func.on_user_event_cb(OTA_EVENT_REPORT_VER, ret, ctx->feedback_func.param);
    }
OTA_REPORT_M_VER_OVER:
    if (ret < 0) {
        OTA_LOG_E("module report version failed\n");
    }
    return ret;
}

/**
 * ota_service_start  OTA service start: download and upgrade
 *
 * @param[in] void
 *
 * @return int
 */
int ota_service_start(ota_service_t *ctx)
{
    int ret = 0;
    unsigned int url_len = 0;
    ota_boot_param_t ota_param = {0};
    if (ctx == NULL) {
        ret = OTA_DOWNLOAD_INIT_FAIL;
        return ret;
    }
    ret = ota_read_parameter(&ota_param);
    if (ret < 0) {
        goto EXIT;
    }
    ota_param.url[sizeof(ota_param.url) - 1] = 0;
    url_len = strlen(ota_param.url);
    OTA_LOG_I("download start upg_flag:0x%x\n", ota_param.upg_flag);
#if !defined(OTA_LINUX) && defined OTA_CONFIG_SECURE_DL_MODE
    ota_wdg.config.timeout = 180000;
    hal_wdg_init(&ota_wdg);
    aos_timer_new(&ota_mon_tmr, ota_monitor_task, NULL, 3000, 1);
    if (ota_is_download_mode() == 0) {
        ret = OTA_INIT_FAIL;
        goto EXIT;
    }
#endif
#ifdef AOS_COMP_PWRMGMT
    aos_pwrmgmt_lowpower_suspend(PWRMGMT_OTA);
#endif
    /* parse image header */
    ctx->header.image_info = ota_malloc(sizeof(ota_image_info_t));
    if (ctx->header.image_info == NULL) {
        OTA_LOG_E("mem err.\n");
        ret = OTA_INIT_FAIL;
        goto EXIT;
    }
#ifdef OTA_CONFIG_LOCAL_RSA
    ctx->header.sign_info = ota_malloc(sizeof(ota_sign_info_t));
    if (ctx->header.sign_info == NULL) {
        OTA_LOG_E("mem err.\n");
        ret = OTA_INIT_FAIL;
        goto EXIT;
    }
#endif
    if (ota_param.upg_flag != OTA_UPGRADE_DIFF) {
        ret = ota_download_image_header(ctx, ota_param.url, url_len, ota_param.len);
        OTA_LOG_I("image header magic:0x%x ret:%d\n", ctx->header.image_info->image_magic, ret);
        if (ret < 0) {
            goto EXIT;
        }
        ota_param.upg_magic = ctx->header.image_info->image_magic;
#ifdef OTA_CONFIG_LOCAL_RSA
        OTA_LOG_I("image sign magic:0x%x\n", ctx->header.sign_info->encrypto_magic);
        if (ctx->header.sign_info->encrypto_magic == 0xAABBCCDD) {
            char tmp_buf[65];
            int result = 0;
            memset(tmp_buf, 0x00, sizeof(tmp_buf));
            memcpy(ota_param.sign, ctx->header.sign_info->signature, sizeof(ota_param.sign));
            ota_param.hash_type = OTA_SHA256;
            result = ota_hex2str(tmp_buf, (const unsigned char *)ctx->header.sign_info->hash, sizeof(tmp_buf), sizeof(ctx->header.sign_info->hash));
            if (result >= 0) {
                memcpy(ota_param.hash, tmp_buf, sizeof(tmp_buf));
            } else {
                OTA_LOG_E("sign info cpy err.");
                ret = OTA_DOWNLOAD_HEAD_FAIL;
                goto EXIT;
            }
        } else {
            OTA_LOG_E("sign info err.");
            ret = OTA_DOWNLOAD_HEAD_FAIL;
            goto EXIT;
        }
#endif
    }
    /* init ota partition */
    ret = ota_int(&ota_param);
    if (ret < 0) {
        ret = OTA_INIT_FAIL;
        goto EXIT;
    }
    OTA_LOG_I("ota init over\n");
    /* download start */
    ret = ota_download_start(ota_param.url, url_len, ctx->report_func.report_status_cb,
                            ctx->report_func.param);
    if (ret < 0) {
        goto EXIT;
    }
    /* verify image */
    ret = ota_verify(&ota_param);
    if (ret < 0) {
        goto EXIT;
    }
EXIT:
    OTA_LOG_I("Download complete, rebooting ret:%d.\n", ret);
    if (ret < 0) {
        ota_param.upg_status = ret;
        if ((ctx != NULL) && (ctx->report_func.report_status_cb !=  NULL)) {
            ctx->report_func.report_status_cb(ctx->report_func.param, ret);
        }
        ret = ota_clear();
        if (ret < 0) {
            OTA_LOG_E("clear ota failed\n");
        }
        ota_msleep(3000); /*give same time to sync err msg to cloud*/
    } else {
        ota_param.crc = 0;
        ret = ota_hal_boot(&ota_param);
        if (ret < 0) {
            if ((ctx != NULL) && (ctx->report_func.report_status_cb !=  NULL)) {
                ctx->report_func.report_status_cb(ctx->report_func.param, ret);
            }
        }
    }
    if (ctx->header.image_info != NULL) {
        ota_free(ctx->header.image_info);
        ctx->header.image_info = NULL;
    }
    if (ctx->header.sign_info != NULL) {
        ota_free(ctx->header.sign_info);
        ctx->header.sign_info = NULL;
    }
#ifdef AOS_COMP_PWRMGMT
    aos_pwrmgmt_lowpower_resume(PWRMGMT_OTA);
#endif
    if (ctx->on_boot != NULL) {
        ctx->on_boot(&ota_param);
    } else {
        if (ota_param.upg_flag != OTA_UPGRADE_DIFF) {
            ret = ota_hal_reboot_bank();
        }
        ota_reboot();
    }
    ota_thread_destroy(NULL);
    return ret;
}

int ota_register_boot_cb(ota_service_t *ctx, void *cb, void *param)
{
    int ret = OTA_CUSTOM_CALLBAK_FAIL;
    if (ctx != NULL) {
        ret = 0;
        ctx->on_boot = cb;
    }
    return ret;
}

int ota_register_trigger_msg_cb(ota_service_t *ctx, void *cb, void *param)
{
    int ret = OTA_CUSTOM_CALLBAK_FAIL;
    if (ctx != NULL) {
        ret = 0;
        ctx->ota_triggered_func.triggered_ota_cb = cb;
        ctx->ota_triggered_func.param = param;
    }
    return ret;
}

int ota_register_report_percent_cb(ota_service_t *ctx, void *cb, void *param)
{
    int ret = OTA_CUSTOM_CALLBAK_FAIL;
    if (ctx != NULL) {
        ret = 0;
        ctx->report_func.report_status_cb = cb;
        ctx->report_func.param = param;
    }
    return ret;
}

int ota_register_feedback_msg_cb(ota_service_t *ctx, void *cb, void *param)
{
    int ret = OTA_CUSTOM_CALLBAK_FAIL;
    if (ctx != NULL) {
        ret = 0;
        ctx->feedback_func.on_user_event_cb = cb;
        ctx->feedback_func.param = param;
    }
    return ret;
}

int ota_register_module_store(ota_service_t *ctx, ota_store_module_info_t *queue, int queue_len)
{
    int ret = OTA_CUSTOM_CALLBAK_FAIL;
    if ((queue != NULL) && (ctx != NULL)) {
        ctx->module_queue_ptr = queue;
        ctx->module_numb = queue_len;
        ret = 0;
    }
    return ret;
}

int ota_set_module_information(ota_service_t *ctx, char *module_name,
                               char *store_path, int module_type)
{
    int ret = OTA_CUSTOM_CALLBAK_FAIL;
    int module_name_len = 0;
    int store_path_len = 0;
    int i = 0;
    if ((ctx == NULL) || (module_name == NULL) || (store_path == NULL)) {
        OTA_LOG_E("store module info input param err!");
        return ret;
    }
    if (ctx->module_queue_ptr == NULL) {
        OTA_LOG_E("store module need init, ptr is null!");
        return ret;
    }
    module_name_len = strlen(module_name);
    store_path_len = strlen(store_path);
    if ((module_name_len <= sizeof(ctx->module_queue_ptr[0].module_name)) &&
        (store_path_len <= sizeof(ctx->module_queue_ptr[0].store_path))) {
        for (i = 0; i < ctx->module_numb; i++) {
            if (ctx->module_queue_ptr[i].module_type == 0) {
                strncpy(ctx->module_queue_ptr[i].module_name, module_name, module_name_len);
                strncpy(ctx->module_queue_ptr[i].store_path, store_path, store_path_len);
                ctx->module_queue_ptr[i].module_type = module_type;
                ret = 0;
                break;
            }
        }
        if (i >= ctx->module_numb) {
            OTA_LOG_E("module buf is full!");
        }
    }
    return ret;
}

int ota_get_module_information(ota_service_t *ctx, char *module_name, ota_store_module_info_t *module_info)
{
    int ret = OTA_CUSTOM_CALLBAK_FAIL;
    int module_name_len = 0;
    int i = 0;
    if ((ctx == NULL) || (module_info == NULL) || (module_name == NULL)) {
        OTA_LOG_E("store module info input param err!");
        return ret;
    }
    if (ctx->module_queue_ptr == NULL) {
        OTA_LOG_E("store module need init, ptr is null!");
        return ret;
    }
    module_name_len = strlen(module_name);
    if (module_name_len <= sizeof(ctx->module_queue_ptr[0].module_name)) {
        for (i = 0; i < ctx->module_numb; i++) {
            if (strncmp(module_name, ctx->module_queue_ptr[i].module_name, module_name_len) == 0) {
                strncpy(module_info->module_name, module_name, strlen(module_name));
                strncpy(module_info->store_path, ctx->module_queue_ptr[i].store_path,
                        strlen(ctx->module_queue_ptr[i].store_path));
                module_info->module_type = ctx->module_queue_ptr[i].module_type;
                ret = 0;
                break;
            }
        }
        if (i >= ctx->module_numb) {
            OTA_LOG_E("module buf is full!");
        }
    }
    return ret;
}
/**
 * ota_service_init  OTA service init
 *
 * @param[in] ota_service_t *ctx OTA service context
 *
 * @return OTA_SUCCESS         OTA service init success.
 * @return OTA_INIT_FAIL       OTA service init fail.
 */
int ota_service_init(ota_service_t *ctx)
{
    int ret = 0;
    if (ctx == NULL) {
        OTA_LOG_E("input ctx is null!");
        ret = OTA_INIT_FAIL;
        goto EXIT;
    }
    ctx->ota_process = OTA_PROCESS_NORMAL;
    if (ctx->report_func.report_status_cb == NULL) {
        OTA_LOG_I("register default status cb");
        ota_register_report_percent_cb(ctx, (void *)ota_transport_status, (void *)ctx);
    }
    ret = ota_transport_upgrade(ctx);
    if (ret < 0) {
        OTA_LOG_E("transport upgrade sub failed!");
        goto EXIT;
    }
    ret = ota_transport_inform_otaver(ctx);
    if (ret < 0) {
        OTA_LOG_E("infor otaver failed!");
        goto EXIT;
    }
    if (ctx->dev_type == 0) {
        ret = ota_hal_rollback();
        if (ret < 0) {
            ret = OTA_INIT_FAIL;
            goto EXIT;
        }
    }
    OTA_LOG_I("ota init success, ret:%d", ret);
    return ret;
EXIT:
    OTA_LOG_E("ota init fail, ret:%d", ret);
    return ret;
}

/**
 * ota_service_param_reset  OTA service deinit
 *
 * @param[in] ota_service_t *ctx OTA service context
 *
 * @return NULL
 */
void ota_service_param_reset(ota_service_t *ctx)
{
    ota_set_upgrade_status(0);
    if (ctx != NULL) {
        ctx->report_func.report_status_cb = NULL;
        ctx->report_func.param = NULL;
        ctx->feedback_func.on_user_event_cb = NULL;
        ctx->feedback_func.param = NULL;
        ctx->mqtt_client = NULL;
        ctx->ota_triggered_func.param = NULL;
        ctx->ota_triggered_func.triggered_ota_cb = NULL;
        ctx->on_boot = NULL;
        ctx->module_queue_ptr = NULL;
        ctx->module_numb = 0;
        ctx->ota_process = 0;
        memset(ctx->pk, 0, sizeof(ctx->pk));
        memset(ctx->dn, 0, sizeof(ctx->dn));
        memset(ctx->ds, 0, sizeof(ctx->ds));
        memset(ctx->ps, 0, sizeof(ctx->ps));
        memset(ctx->module_name, 0, sizeof(ctx->module_name));
    } else {
        OTA_LOG_E("param reset input ctx is null.");
    }
}
