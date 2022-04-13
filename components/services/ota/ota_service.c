/*
 *Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <string.h>
#include <cJSON.h>
#include "ota_log.h"
#include "ota_import.h"
#include "ota_hal_os.h"
#include "aos/kernel.h"
#include "aos/hal/wdg.h"
#include "ota_hal.h"

#ifdef AOS_COMP_PWRMGMT
#include "aos/pwrmgmt.h"
#endif

#if !defined (OTA_LINUX) && defined OTA_CONFIG_SECURE_DL_MODE
static wdg_dev_t ota_wdg = {0};
static aos_timer_t  ota_mon_tmr = {0};
static void ota_monitor_task(void *arg1, void* arg2)
{
    hal_wdg_reload(&ota_wdg);
}
#endif

int ota_service_submodule_start(ota_service_t *ctx)
{
    /* OTA download init */
    int ret = 0;
    char *version = NULL;
    ota_boot_param_t ota_param = {0};
    char file_name[64] = {0};
    ret = ota_read_parameter(&ota_param);
    if(ret < 0) {
        goto SUBDEV_EXIT;
    }
    if(ctx == NULL) {
        goto SUBDEV_EXIT;
    }
    ret = ota_get_storefile_subdev_name(file_name); 
    if(ret < 0){
        OTA_LOG_E("get store file path failed\n");
        goto SUBDEV_EXIT;
    }
    ret = ota_download_store_fs_start(ctx, ota_param.url, strlen(ota_param.url), file_name);
    if (ret < 0) {
        goto SUBDEV_EXIT;
    }
    OTA_LOG_I("verify subdev ota fs file\n");
    ret = ota_verify_fsfile(&ota_param, file_name);
    if(ret < 0) {
        goto SUBDEV_EXIT;
    }
    OTA_LOG_I("upgrade sub module\n");
    ret = ota_subdev_upgrade_start((void*)ctx, ota_param.len, ota_param.ver);
    if(ret < 0) {
        goto SUBDEV_EXIT;
    }
    OTA_LOG_I("report submode version\n");
    version = (char *)ota_hal_version(ctx->dev_type, ctx->module_name);
    if (version == NULL) {
        ret = OTA_MCU_VERSION_FAIL;
        goto SUBDEV_EXIT;
    }
    ret = ota_transport_inform_subver(ctx, version);
    if (ret < 0) {
        goto SUBDEV_EXIT;
    }

SUBDEV_EXIT:
    OTA_LOG_I("Subdev download complete ret = %d\n", ret);
    if (ret < 0) {
        ota_param.upg_status = ret;
        if (ctx != NULL) {
            ota_transport_status(ctx, ret);
        }
    }
    ret = ota_clear();
    if(ret < 0) {
        ota_transport_status(ctx, ret);
    }
    ota_reboot_module(ctx->module_name);
    ota_thread_destroy(NULL);
    return ret;
}

/**
 * ota_service_maindev_fs_start  OTA service start: download and upgrade
 *
 * @param[in] void
 *
 * @return int
 */
int ota_service_maindev_fs_start(ota_service_t *ctx)
{
    int ret = 0;
    ota_boot_param_t ota_param = {0};
    char file_name[64] = {0};
    ret = ota_read_parameter(&ota_param);
    if(ret < 0) {
        goto MAINDEV_FS_EXIT;
    }
    OTA_LOG_I("download start upg_flag:0x%x \n", ota_param.upg_flag);
    ret = ota_get_storefile_maindev_name(file_name); 
    if(ret < 0){
        OTA_LOG_E("get store file path failed\n");
        goto MAINDEV_FS_EXIT;
    }
    OTA_LOG_I("file_name = %s\rn", file_name);
    ret = ota_download_store_fs_start(ctx, ota_param.url, strlen(ota_param.url), file_name);
    if (ret < 0) {
        goto MAINDEV_FS_EXIT;
    }
    OTA_LOG_I("verify maindev ota fs file\n");
    ret = ota_verify_fsfile(&ota_param, file_name);
    if(ret < 0) {
        goto MAINDEV_FS_EXIT;
    }
MAINDEV_FS_EXIT:
    OTA_LOG_I("Download complete, rebooting ret:%d.\n", ret);
    if (ret < 0) {
        ota_param.upg_status = ret;
        if (ctx != NULL) {
            ota_transport_status(ctx, ret);
        }
        ret = ota_clear();
        if(ret < 0) {
            ota_transport_status(ctx, ret);
        }
        ota_msleep(3000); /* give same time to sync err msg to cloud*/
    } else {
        ota_param.crc = 0;
        ret = ota_hal_boot(&ota_param);
        if(ret < 0) {
            ota_transport_status(ctx, ret);
        }
    }
    if ((ctx != NULL) && (ctx->on_boot != NULL)) {
        ctx->on_boot(&ota_param);
    } else {
        if(ota_param.upg_flag != OTA_UPGRADE_DIFF) {
            ret = ota_hal_reboot_bank();
        }
        ota_reboot();
    }
    ota_thread_destroy(NULL);
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
    if(ctx == NULL) {
        ret = OTA_DOWNLOAD_INIT_FAIL;
        return ret;
    }
    ret = ota_read_parameter(&ota_param);
    if(ret < 0) {
        goto EXIT;
    }
    ota_param.url[sizeof(ota_param.url) - 1] = 0;
    url_len = strlen(ota_param.url);
    if(url_len >= sizeof(ota_param.url)) {
        OTA_LOG_E("download url len err!");
        ret = OTA_DOWNLOAD_INIT_FAIL;
        goto EXIT;
    }
    OTA_LOG_I("download start upg_flag:0x%x \n", ota_param.upg_flag);
#if !defined (OTA_LINUX) && defined OTA_CONFIG_SECURE_DL_MODE
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
#if defined BOARD_ESP8266 && !defined OTA_CONFIG_SECURE_DL_MODE
    aos_task_delete("linkkit");
    ota_msleep(200);
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
    if(ctx->header.sign_info == NULL) {
        OTA_LOG_E("mem err.\n");
        ret = OTA_INIT_FAIL;
        goto EXIT;
    }
#endif
    if (ota_param.upg_flag != OTA_UPGRADE_DIFF) {
        ret = ota_download_image_header(ctx, ota_param.url, url_len, ota_param.len);
        OTA_LOG_I("image header magic:0x%x ret:%d \n", ctx->header.image_info->image_magic, ret);
        if(ret < 0) {
            goto EXIT;
        }
        ota_param.upg_magic = ctx->header.image_info->image_magic;
#ifdef OTA_CONFIG_LOCAL_RSA
        OTA_LOG_I("image sign magic:0x%x\n", ctx->header.sign_info->encrypto_magic);
        if(ctx->header.sign_info->encrypto_magic == 0xAABBCCDD) {
            char tmp_buf[65];
            int result = 0;
            memset(tmp_buf, 0x00, sizeof(tmp_buf));
            memcpy(ota_param.sign, ctx->header.sign_info->signature, sizeof(ota_param.sign));
            ota_param.hash_type = OTA_SHA256;
            result = ota_hex2str(tmp_buf, (const unsigned char*)ctx->header.sign_info->hash, sizeof(tmp_buf), sizeof(ctx->header.sign_info->hash));
            if(result >= 0) {
                memcpy(ota_param.hash, tmp_buf, sizeof(tmp_buf));
            }
            else {
                OTA_LOG_E("sign info cpy err.");
                ret = OTA_DOWNLOAD_HEAD_FAIL;
                goto EXIT;
            }
        }
        else {
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
    ret = ota_download_start(ctx, ota_param.url, url_len);
    if (ret < 0) {
        goto EXIT;
    }
    /* verify image */
    ret = ota_verify(&ota_param);
    if (ret < 0) {
        goto EXIT;
    }
    /* MCU firmware */
    if(ctx->header.image_info->image_magic == OTA_BIN_MAGIC_MCU) {
        OTA_LOG_I("Download complete, start MCU OTA.\n");
        ret = ota_mcu_upgrade_start(ota_param.len, ota_param.ver, ota_param.hash);
    }
EXIT:
    OTA_LOG_E("Download complete, rebooting ret:%d.\n", ret);
    if (ret < 0) {
        ota_param.upg_status = ret;
#if !defined BOARD_ESP8266 && !defined OTA_CONFIG_SECURE_DL_MODE
        ota_transport_status(ctx, ret);
#endif /* !defined BOARD_ESP8266 && !defined OTA_CONFIG_SECURE_DL_MODE */
        ret = ota_clear();
        if(ret < 0) {
            ota_transport_status(ctx, ret);
        }
        ota_msleep(3000); /*give same time to sync err msg to cloud*/
    } else {
        ota_param.crc = 0;
        ret = ota_hal_boot(&ota_param);
        if(ret < 0) {
            ota_transport_status(ctx, ret);
        }
    }
    if(ctx->header.image_info != NULL) {
        ota_free(ctx->header.image_info);
        ctx->header.image_info = NULL;
    }
    if(ctx->header.sign_info != NULL) {
        ota_free(ctx->header.sign_info);
        ctx->header.sign_info = NULL;
    }
#ifdef AOS_COMP_PWRMGMT
    aos_pwrmgmt_lowpower_resume(PWRMGMT_OTA);
#endif
    if (ctx->on_boot != NULL) {
        ctx->on_boot(&ota_param);
    } else {
        if(ota_param.upg_flag != OTA_UPGRADE_DIFF) {
            ret = ota_hal_reboot_bank();
        }
        ota_reboot();
    }
    ota_thread_destroy(NULL);
    return ret;
}

/**
 * ota_register_cb  OTA register callback
 *
 * @param[in] int id, void* cb
 *
 * @return OTA_SUCCESS         OTA register callback success.
 * @return OTA_CUSTOM_CALLBAK_FAIL OTA register callback fail.
 */
int ota_register_cb(ota_service_t *ctx, int id, void* cb)
{
    if(ctx == NULL) {
        return OTA_CUSTOM_CALLBAK_FAIL;
    }
    switch (id) {
        case OTA_CB_ID_UPGRADE:
           ctx->on_upgrade = cb;
        break;
        case OTA_CB_ID_MODULE_UPGRADE:
           ctx->on_module_upgrade = cb;
        break;
        case OTA_CB_ID_DATA:
           ctx->on_data = cb;
        break;
        case OTA_CB_ID_PERCENT:
           ctx->on_percent = cb;
        break;
        case OTA_CB_ID_BOOT:
           ctx->on_boot = cb;
        break;
        default:
           OTA_LOG_E("Err CB ID.");
           return OTA_CUSTOM_CALLBAK_FAIL;
        break;
    }
    return 0;
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
    char *version = NULL;

    if(ctx == NULL) {
        ret = OTA_INIT_FAIL;
        goto EXIT;
    }
    /* inform version to cloud */
    version = (char *)ota_hal_version(ctx->dev_type, NULL);
    if (version == NULL) {
        ret = OTA_INIT_FAIL;
        goto EXIT;
    }
#if defined OTA_CONFIG_BLE
    extern on_ota_bool_pfunc ota_ble_status_get_cb;
    extern on_ota_int_pfunc ota_ble_user_store_data_process_cb;
    ota_ble_status_get_cb = ctx->on_ota_status_cb;
    ota_ble_user_store_data_process_cb = ctx->on_ota_user_store_data_handle;
    ctx->on_message =  ota_ble_transport_msg;
    OTA_LOG_I("BLE OTA init success.\r\n");
    return ret;
#else
    ctx->ota_process = OTA_PROCESS_NORMAL;
    ret = ota_transport_inform(ctx, version);
    if (ret < 0) {
        goto EXIT;
    }
    if(strlen(ctx->module_name) != 0) {
        version = (char *)ota_hal_version(ctx->dev_type, ctx->module_name);
        ret = ota_transport_inform_subver(ctx, version);
        if(ret < 0) {
            OTA_LOG_E("infor submodule ver failed!");
            goto EXIT;
        }
    }
    ret = ota_transport_inform_otaver(ctx);
    if(ret < 0) {
        OTA_LOG_E("infor otaver failed!");
        goto EXIT;
    }
    /* subcribe upgrade */
    ret = ota_transport_upgrade(ctx);
    if (ret < 0) {
        goto EXIT;
    }
    /* rollback */
    if(ctx->dev_type == 0) {//master dev ota
        ret = ota_hal_rollback();
        if (ret < 0) {
            ret = OTA_INIT_FAIL;
            goto EXIT;
        }
    }
    OTA_LOG_I("ota init success, ret:%d", ret);
    return ret;
#endif
EXIT:
    OTA_LOG_E("ota init fail, ret:%d", ret);
    return ret;
}

/**
 * ota_service_deinit  OTA service deinit
 *
 * @param[in] ota_service_t *ctx OTA service context
 *
 * @return OTA_SUCCESS         OTA service deinit success.
 * @return OTA_INIT_FAIL       OTA service deinit fail.
 */
int ota_service_deinit(ota_service_t* ctx)
{
    int ret = 0;
    ret = ota_download_deinit(ctx);
    if(ret < 0) {
        OTA_LOG_E("download deinit failed.");
        return ret;
    }
    return ret;
}
