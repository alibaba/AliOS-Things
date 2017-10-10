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

static char *url_temp = NULL;

char md5[33];

char *const get_url()
{
    return url_temp;
}

int set_url(const char *value)
{
    if (url_temp == NULL) {
        url_temp = aos_malloc(OTA_URL_MAX_LEN);
    }
    if (url_temp == NULL) {
        return -1;
    }
    int len = strlen(value);
    len = len < OTA_URL_MAX_LEN ? len : OTA_URL_MAX_LEN;
    memset(url_temp, 0, OTA_URL_MAX_LEN);
    memcpy(url_temp, value, len);
    return 0;
}

void free_url()
{
    if (url_temp) {
        aos_free(url_temp);
        url_temp = NULL;
    }
}

extern int  check_md5(const char *buffer, const int32_t len);

extern int ota_hal_init(uint32_t offset);

int8_t ota_if_need(ota_response_params *response_parmas, ota_request_params *request_parmas)
{
    if (strncmp(response_parmas->primary_version,
                request_parmas->primary_version,
                sizeof response_parmas->primary_version) > 0) {
        if (strlen(request_parmas->secondary_version)) {
            ota_set_update_type(OTA_KERNEL);
        }
        return 1;
    }

    if (strlen(request_parmas->secondary_version) && strncmp(response_parmas->secondary_version,
                                                             request_parmas->secondary_version,
                                                             sizeof response_parmas->secondary_version) > 0) {
        ota_set_update_type(OTA_APP);
        return 1;
    }

    return 0;
}

void ota_download_start(void *buf)
{
    OTA_LOG_I("task update start");
#ifdef STM32_SPI_NET
    notify_ota_start();
#endif
    ota_hal_init(ota_get_update_breakpoint());
    ota_status_init();

    ota_set_status(OTA_INIT);
    ota_status_post(100);

    ota_set_status(OTA_DOWNLOAD);
    ota_status_post(0);
    int ret = http_download(get_url(), g_write_func, md5);
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
    free_url();
    ota_status_deinit();
#ifdef STM32_SPI_NET
    notify_ota_end();
#endif
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
    ret = ota_if_need(response_parmas, request_parmas);
    if (1 != ret) {
        return ret;
    }

    //ota_set_version(response_parmas->primary_version);
    g_write_func = func;
    g_finish_cb = fcb;

    memset(md5, 0, sizeof md5);
    strncpy(md5, response_parmas->md5, sizeof md5);
    md5[(sizeof md5) - 1] = '\0';

    if (set_url(response_parmas->download_url)) {
        ret = -1;
        return ret;
    }
    // memset(url, 0, sizeof url);
    // strncpy(url, response_parmas->download_url, sizeof url);
    ret = aos_task_new("ota", ota_download_start, 0, 4096);
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





