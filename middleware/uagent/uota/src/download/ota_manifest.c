/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include "ota_manifest.h"
#include "ota_log.h"
#include "ota_service.h"
#include "ota_util.h"
#include "ota_hal_os.h"
#include "ota_version.h"
#include "ota_download.h"
#include "ota_verify.h"
#include "ota_hal_plat.h"
#include "iot_import.h"
#include "ota_rsa_verify.h"
#include "ota_hash.h"

#if (defined IS_ESP8266)
#include "k_dbg_api.h"
#endif

ota_write_cb_t  g_write_cb  = NULL;
ota_read_cb_t   g_read_cb   = NULL;
ota_finish_cb_t g_finish_cb = NULL;

static char           *msg_temp = NULL;
static ota_hash_params download_hash_data;
ota_sign_params download_sign_data;

static int set_download_hash(const char hash_method, const char *hash_value)
{
    if (hash_value == NULL) {
        return -1;
    }
    int copy_len;

    OTA_LOG_I("hash mode:%d  value:%s \n", hash_method, hash_value);
    memset(&download_hash_data, 0, sizeof download_hash_data);
    download_hash_data.hash_method = hash_method;
    if (hash_method == MD5) {
        copy_len = OTA_MD5_LEN;
    } else if (hash_method == SHA256) {
        copy_len = OTA_SHA256_LEN;
    } else {
        return -1;
    }
    strncpy(download_hash_data.hash_value, hash_value, copy_len);
    download_hash_data.hash_value[copy_len] = '\0';
    return 0;
}

static int set_download_signature(ota_response_params *response_parmas)
{
    if(response_parmas->sign_enable == OTA_SIGN_OFF) {
        download_sign_data.sign_enable = OTA_SIGN_OFF;
        OTA_LOG_W("set download signature: ota rsa verify function close");
        return 0;
    }
    memset(&download_sign_data, 0, sizeof download_sign_data);
    download_sign_data.sign_enable = response_parmas->sign_enable;
    if(response_parmas->sign_value_len > sizeof(download_sign_data.sign_value)) {
        OTA_LOG_E("ota receive sign value too long!");
        return -1;
    }
    memcpy(download_sign_data.sign_value, response_parmas->sign_value, response_parmas->sign_value_len);
    return 0;
}

static unsigned char is_signa_enable()
{
    return download_sign_data.sign_enable;
}

static char *get_download_url()
{
    return msg_temp;
}

static int set_download_url(const char *value)
{
    if (value == NULL) {
        return -1;
    }
    if (msg_temp == NULL) {
        msg_temp = ota_malloc(OTA_RESP_MAX_LEN);
    }
    if (msg_temp == NULL) {
        return -1;
    }

    memset(msg_temp, 0, OTA_RESP_MAX_LEN);
    strncpy(msg_temp, value, OTA_RESP_MAX_LEN - 1);

    return 0;
}

static void free_msg_temp()
{
    if (msg_temp) {
        ota_free(msg_temp);
        msg_temp = NULL;
    }
}

const char *ota_get_resp_msg()
{
    return msg_temp;
}

int ota_set_resp_msg(const char *value)
{
    return set_download_url(value);
}

int8_t ota_if_need(ota_response_params *response_parmas,
                   ota_request_params  *request_parmas)
{
    int ret = 0;
    int is_primary_ota =
      strncmp(response_parmas->primary_version, request_parmas->primary_version,
              strlen(response_parmas->primary_version));

    int is_secondary_ota = strncmp(response_parmas->secondary_version,
                                   request_parmas->secondary_version,
                                   strlen(response_parmas->secondary_version));

    int  is_need_ota                  = 0;
    char ota_version[MAX_VERSION_LEN] = { 0 };
    if (is_primary_ota > 0) {
        if (strlen(request_parmas->secondary_version)) {
            ret = ota_snprintf(ota_version, MAX_VERSION_LEN, "%s_%s",
                         response_parmas->primary_version,
                         (char *)ota_get_app_version);
            if (is_secondary_ota == 0) {
                ota_set_update_type(OTA_KERNEL);
                is_need_ota = 1;
            }
			if ((ret > MAX_VERSION_LEN)||(ret < 0)) {
		             OTA_LOG_E("HAL_Snprintf failed");
			     return 0;     
			}
        } else {
            ota_snprintf(ota_version, MAX_VERSION_LEN, "%s",
                         response_parmas->primary_version);
            is_need_ota = 1;
        }
    }

    if (is_primary_ota == 0 && is_secondary_ota > 0) {
        ret = ota_snprintf(ota_version, MAX_VERSION_LEN, "%s_%s",
                     response_parmas->primary_version,
                     response_parmas->secondary_version);
		if ((ret > MAX_VERSION_LEN)||(ret < 0)) {
	             OTA_LOG_E("HAL_Snprintf failed");
		     return 0;     
		}
        ota_set_update_type(OTA_APP);
        is_need_ota = 1;
    }

    OTA_LOG_I("version primary req:%s res:%s need:%d",
              request_parmas->primary_version, response_parmas->primary_version,
              is_need_ota);
    ota_set_ota_version(ota_version);
    return is_need_ota;
}

static void ota_download_start(void *buf)
{
    unsigned off = 0;
    OTA_LOG_I("task update start");
#if (defined IS_ESP8266)
    #if !(defined FOTA_RAM_LIMIT_MODE)
    extern int get_awss_notify_running_flag(void);
    while (get_awss_notify_running_flag()) {
        ota_msleep(500);
        OTA_LOG_I("sleep 500ms");
    }
    #endif
    OTA_LOG_I("awss_notify stop.");
    ktask_t* h = NULL;
    h = krhino_task_find("event_task");
    if(h)
    krhino_task_dyn_del(h);
    ota_msleep(1000);
#endif

    ota_service_manager *ctx = (ota_service_manager *)get_ota_service_manager();
    ota_download        *dl = (ota_download *)ota_get_download(ctx->dl_protcol);
    off                     = ota_get_update_breakpoint();
    hal_ota_init((void *)(&off));
    if (ota_malloc_global_context(download_hash_data.hash_method) != 0) {
        OTA_LOG_E("ota sign ctx malloc fail");
        goto OTA_END;
    }
    int ret = dl->start(get_download_url(), g_write_cb, (void *)&download_hash_data);
    if (ret <= 0) {
        OTA_LOG_E("ota download error");
        ota_set_status(OTA_DOWNLOAD_FAILED);
        if (NULL != g_finish_cb) {
            int type = ota_get_update_type();
            g_finish_cb(OTA_BREAKPOINT, &type);
        }
        goto OTA_END;
    }
    if (ret == OTA_DOWNLOAD_CANCEL) {
        OTA_LOG_E("ota download cancel");
        if (NULL != g_finish_cb) {
            int type = ota_get_update_type();
            g_finish_cb(OTA_BREAKPOINT, &type);
        }
        ota_set_status(OTA_CANCEL);
        goto OTA_END;
    }

    ota_set_status(OTA_CHECK);
    ret = ota_check_hash_value(&download_hash_data);
    if (ret < 0) {
        OTA_LOG_E("ota check signature error");
        ota_set_status(OTA_CHECK_FAILED);
        goto OTA_END;
    }
    if(is_signa_enable() == OTA_SIGN_ON) {
#if !defined(OTA_RSA_DISABLE)
        ret = ota_verify_download_rsa_sign(download_sign_data.sign_value,
            download_hash_data.hash_value, download_hash_data.hash_method);
#else
        ret = 0;
#endif
        if(ret < 0) {
            OTA_LOG_E("ota verify rsa error");
            ota_set_status(OTA_RSA_VERIFY_FAILED);
            goto OTA_END;
        }
    }
#if defined(OTA_MD5_CHECK)
    if(ota_get_firmware_type() != OTA_DIFF){
        ret = ota_check_image(g_read_cb);
        if (ret < 0) {
            OTA_LOG_E("ota check image failed");
            ota_set_status(OTA_CHECK_IMAGE_FAILED);
            goto OTA_END;
        }
    }
#endif
    ota_set_status(OTA_UPGRADE);
    if (NULL != g_finish_cb) {
        int type = ota_get_update_type();
        g_finish_cb(OTA_FINISH, &type);
    }
    ota_set_status(OTA_REBOOT);

OTA_END:
#if (!defined IS_ESP8266)
    ota_status_post(100);
#endif
    free_msg_temp();
    ota_free_global_context();
    ota_status_deinit();
    OTA_LOG_I("reboot system after 3 second!");
    ota_msleep(3000);
    OTA_LOG_I("task update over");
    ota_reboot();
}

int8_t ota_post_version_msg()
{
    int ret = -1, ota_success = 0;
    OTA_LOG_I("ota_post_version_msg  [%s][%s]", ota_get_system_version(),
              ota_get_version());
    if (strlen(ota_get_version()) > 0) {
        ota_success =
          !strncmp((char *)ota_get_system_version(), (char *)ota_get_version(),
                   strlen(ota_get_system_version()));
        if (ota_success) {
            ota_set_status(OTA_REBOOT_SUCCESS);
            ret = ota_status_post(100);
        } else {
            ota_set_status(OTA_INIT);
            ret = ota_status_post(0);
        }

        if (ret == 0) {
            OTA_LOG_I("OTA finished, clear ota version in config");
            ota_set_version("");
        }
    }

    ota_service_manager *ctx = (ota_service_manager *)get_ota_service_manager();
    ota_transport       *trans =
      (ota_transport *)ota_get_transport(ctx->trans_protcol);
    ret = trans->result_post();
    if (ret == 0) {
        if (strncmp((char *)ota_get_system_version(),
                    (char *)ota_get_dev_version(),
                    strlen(ota_get_system_version()))) {
            OTA_LOG_I("Save dev version to config");
            ota_set_dev_version(ota_get_system_version());
        }
    }


    return 0;
}


void ota_start_download_task(void* ctx){
    int ret = 0;
    void *thread = NULL;
    hal_os_thread_param_t task_parms = { 0 };

#if (defined IS_ESP8266)
    ktask_t* h = NULL;
    h = krhino_task_find("linkkit");
    if(h)
    krhino_task_dyn_del(h);
    ota_msleep(1000);
#endif

#if defined HTTPS_DOWNLOAD
    task_parms.stack_size = 6144;
#else
    task_parms.stack_size = 4096;
#endif
    task_parms.name = "ota_thread";
    ret             = ota_thread_create(&thread, (void *)ota_download_start,
                    (void *)NULL, &task_parms, 0);
    if (ret != 0) {
        ota_reboot();
    }
}


int8_t ota_do_update_packet(ota_response_params *response_parmas,
                            ota_request_params  *request_parmas,
                            ota_write_cb_t wcb, ota_read_cb_t rcb,
                            ota_finish_cb_t fcb)
{
    int   ret = 0;
    ota_set_status(OTA_INIT);
    OTA_LOG_I("ota_do_update_packet hash:%d value:%s \n",
              response_parmas->hash_method, response_parmas->hash_value);
    ret = ota_if_need(response_parmas, request_parmas);
    if (1 != ret) {
        OTA_LOG_E("ota cancel,ota version don't match dev version ! ");
        ota_set_status(OTA_INIT_FAILED);
        ota_status_post(100);
        ota_status_deinit();
        return ret;
    }
    ota_status_post(100);
    g_write_cb  = wcb;
    g_read_cb   = rcb;
    g_finish_cb = fcb;
    if (set_download_hash(response_parmas->hash_method,
                          response_parmas->hash_value)) {
        OTA_LOG_E("set_signature failed");
        ret = -1;
        return ret;
    }
    if (set_download_signature(response_parmas)) {
        OTA_LOG_E("set_signature failed");
        ret = -1;
        return ret;
    }
    if (set_download_url(response_parmas->download_url)) {
        OTA_LOG_E("set_url failed");
        ret = -1;
        return ret;
    }
    ota_set_status(OTA_DOWNLOAD);
    ota_status_post(0);
#if (defined IS_ESP8266)
    void *timer                      = 0;
    timer = (void *)ota_timer_create("ota_timer", ota_start_download_task,
                                     (void *)NULL);
    ota_timer_start(timer, 200);
#else
    ota_start_download_task(NULL);
#endif

    return ret;
}

static int8_t ota_is_cancelable()
{
    return ota_get_status() != OTA_INIT && ota_get_status() < OTA_UPGRADE;
}

static int8_t ota_if_cancel(ota_response_params *response_parmas)
{
    if (!response_parmas) {
        return 0;
    }

    ota_service_manager *ctx = (ota_service_manager *)get_ota_service_manager();
    if (!strncmp(response_parmas->device_uuid, ctx->uuid,
                 sizeof response_parmas->device_uuid)) {
        return 0;
    }

    if (!ota_is_cancelable()) {
        return 0;
    }
    return 1;
}

int8_t ota_cancel_update_packet(ota_response_params *response_parmas)
{
    int ret = 0;
    ret     = ota_if_cancel(response_parmas);
    if (ret) {
        ota_set_status(OTA_CANCEL);
    }
    return ret;
}

const void *ota_get_download(int protcol)
{
    if (OTA_PROTCOL_COAP == protcol) {
        return ota_get_download_coap();
    } else {
        return ota_get_download_http();
    }
}
