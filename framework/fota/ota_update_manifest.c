/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <aos/aos.h>

#include "ota_update_manifest.h"
#include "ota_log.h"
#include "ota_transport.h"
#include "ota_util.h"
#include "ota_platform_os.h"
#include "ota_version.h"
#include "ota_download.h"

write_flash_cb_t g_write_func;
ota_finish_cb_t g_finish_cb;

static char *msg_temp = NULL;

char md5[33];

static char * get_download_url()
{
    return msg_temp;
}

static int set_download_url(const char *value)
{
    if (value==NULL) {
        return -1;
    }
    if (msg_temp == NULL) {
        msg_temp = aos_malloc(OTA_RESP_MAX_LEN);
    }
    if (msg_temp == NULL) {
        return -1;
    }

    memset(msg_temp, 0, OTA_RESP_MAX_LEN);
    strncpy(msg_temp, value,OTA_RESP_MAX_LEN-1);

    return 0;
}

static void free_msg_temp()
{
    if (msg_temp) {
        aos_free(msg_temp);
        msg_temp = NULL;
    }
}

const char * ota_get_resp_msg()
{
    return msg_temp;
}

int ota_set_resp_msg(const char *value)
{
    return set_download_url(value);  
}


extern int  check_md5(const char *buffer, const int32_t len);

extern int ota_hal_init(uint32_t offset);

int8_t ota_if_need(ota_response_params *response_parmas, ota_request_params *request_parmas)
{
    int is_primary_ota = strncmp(response_parmas->primary_version,
                                 request_parmas->primary_version,
                                 strlen(response_parmas->primary_version));

    int is_secondary_ota = strncmp(response_parmas->secondary_version,
                                   request_parmas->secondary_version,
                                   strlen(response_parmas->secondary_version));

    int is_need_ota = 0;
    char ota_version[MAX_VERSION_LEN] = {0};
    if (is_primary_ota > 0 ) {
        if (strlen(request_parmas->secondary_version) ) {
            snprintf(ota_version, MAX_VERSION_LEN, "%s_%s", response_parmas->primary_version, aos_get_app_version());
            if (is_secondary_ota == 0) {
                ota_set_update_type(OTA_KERNEL);
                is_need_ota = 1;
            }
        } else {
            snprintf(ota_version, MAX_VERSION_LEN, "%s", response_parmas->primary_version);
            is_need_ota = 1;
        }
    }

    if (is_primary_ota == 0 && is_secondary_ota > 0) {
        snprintf(ota_version, MAX_VERSION_LEN, "%s_%s", response_parmas->primary_version,
                 response_parmas->secondary_version);
        ota_set_update_type(OTA_APP);
        is_need_ota = 1;
    }

    OTA_LOG_I("ota_version %s", ota_version);
    ota_set_ota_version(ota_version);
    return is_need_ota;
}

void ota_download_start(void *buf)
{
    OTA_LOG_I("task update start");
    ota_hal_init(ota_get_update_breakpoint());

    ota_set_status(OTA_DOWNLOAD);
    ota_status_post(0);
    int ret = ota_download(get_download_url(), g_write_func, md5);
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

    ota_status_post(100);
    ota_set_status(OTA_CHECK);
    ret = check_md5(md5, sizeof md5);
    if (ret < 0 ) {
        OTA_LOG_E("ota check md5 error");
        ota_set_status(OTA_CHECK_FAILED);
        goto OTA_END;
    }
    ota_status_post(100);
    // memset(url, 0, sizeof url);

    OTA_LOG_I("ota status %d", ota_get_status());
    ota_set_status(OTA_UPGRADE);
    if (NULL != g_finish_cb) {
        int type = ota_get_update_type();
        g_finish_cb(OTA_FINISH, &type);
    }
    ota_status_post(100);
    ota_set_status(OTA_REBOOT);

OTA_END:
    ota_status_post(100);
    free_msg_temp();
    ota_status_deinit();
    OTA_LOG_I("reboot system after 3 second!");
    aos_msleep(3000);
    OTA_LOG_I("task update over");
    ota_reboot();
}

int8_t ota_post_version_msg()
{
    int ret = -1, ota_success = 0;
    OTA_LOG_I("ota_post_version_msg  [%s][%s] [%s]", ota_get_system_version(), ota_get_version(), ota_get_dev_version());
    if (strlen(ota_get_version()) > 0) {
        ota_success = !strncmp((char *)ota_get_system_version(),
                               (char *)ota_get_version(), strlen(ota_get_system_version()));
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

    ret = ota_result_post();
    if (ret == 0) {
        if (strncmp((char *)ota_get_system_version(), (char *)ota_get_dev_version(), strlen(ota_get_system_version()))) {
            OTA_LOG_I("Save dev version to config");
            ota_set_dev_version(ota_get_system_version());
        }
    }


    return 0;
}

int8_t ota_do_update_packet(ota_response_params *response_parmas, ota_request_params *request_parmas,
                            write_flash_cb_t func, ota_finish_cb_t fcb)
{
    int ret = 0;

    ota_status_init();
    ota_set_status(OTA_INIT);

    ret = ota_if_need(response_parmas, request_parmas);
    if (1 != ret) {
        OTA_LOG_E("ota cancel,ota version don't match dev version ! ");
        ota_set_status(OTA_INIT_FAILED);
        ota_status_post(100);
        ota_status_deinit();
        return ret;
    }
    ota_status_post(100);

    //ota_set_version(response_parmas->primary_version);
    g_write_func = func;
    g_finish_cb = fcb;

    memset(md5, 0, sizeof md5);
    strncpy(md5, response_parmas->md5, sizeof md5);
    md5[(sizeof md5) - 1] = '\0';

    if (set_download_url(response_parmas->download_url)) {
        OTA_LOG_E("set_url failed");
        ret = -1;
        return ret;
    }
    // memset(url, 0, sizeof url);
    // strncpy(url, response_parmas->download_url, sizeof url);
    ret = aos_task_new("ota", ota_download_start, 0, 4096);
#ifdef STM32_USE_SPI_WIFI
    aos_task_exit(0);
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

    if (!strncmp(response_parmas->device_uuid , platform_ota_get_id(), sizeof response_parmas->device_uuid)) {
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
    ret = ota_if_cancel(response_parmas);
    if (ret) {
        ota_set_status(OTA_CANCEL);
    }
    return ret;
}
