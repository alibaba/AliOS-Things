/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include <string.h>
#include "os.h"
#include "passwd.h"
#include "awss_log.h"
#include "awss_cmp.h"
#include "awss_utils.h"
#include "awss_notify.h"
#include "awss_dev_ap.h"
#include "json_parser.h"
#include "awss_packet.h"
#include "awss_crypt.h"
#include "zconfig_utils.h"

#ifndef AWSS_SUPPORT_DEV_AP
#define AWSS_SUPPORT_DEV_AP
#endif

#ifdef AWSS_SUPPORT_DEV_AP

#define AWSS_DEV_AP_PASSWD  "12345678"

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
extern "C"
{
#endif
static void *g_awss_dev_ap_mutex = NULL;
static char awss_dev_ap_switchap_done = 0;
static char awss_dev_ap_ongoing = 0;

static int awss_dev_ap_setup()
{
    char ssid[PLATFORM_MAX_SSID_LEN + 1] = {0};
    char passwd[PLATFORM_MAX_PASSWD_LEN + 1] = {0};

    do {  // reduce stack used
        char pk[OS_PRODUCT_KEY_LEN + 1] = {0};
        char mac_str[OS_MAC_LEN + 1] = {0};

        os_product_get_key(pk);
        os_wifi_get_mac_str(mac_str);
        memcpy(mac_str + 11, mac_str + 12, 2);
        memcpy(mac_str + 13, mac_str + 15, 2);
        mac_str[15] = '\0';
        snprintf(ssid, PLATFORM_MAX_SSID_LEN, "adh_%s_%s", pk, &mac_str[9]);
        //snprintf(passwd, PLATFORM_MAX_PASSWD_LEN, "%s", AWSS_DEV_AP_PASSWD);
    } while (0);

    awss_trace("ssid:%s\n", ssid);

    return os_awss_open_ap(ssid, passwd, 100, 0);
}

int awss_dev_ap_start(void)
{
    int ret = -1;

    if (g_awss_dev_ap_mutex || awss_dev_ap_ongoing) {
        awss_debug("dev ap already running");
        return -1;
    }

    if (g_awss_dev_ap_mutex == NULL)
        g_awss_dev_ap_mutex = HAL_MutexCreate();
    if (g_awss_dev_ap_mutex == NULL) {
        awss_debug("awss dev ap start fail");
        goto AWSS_DEV_AP_FAIL;
    }

    HAL_MutexLock(g_awss_dev_ap_mutex);

    awss_dev_ap_ongoing = 1;
    awss_dev_ap_switchap_done = 0;

    ret = awss_dev_ap_setup();
    os_msleep(1000);  // wait for dev ap to work well
    awss_cmp_local_init();

    while (awss_dev_ap_ongoing) {
        os_msleep(200);
        if (awss_dev_ap_switchap_done)
            break;
    }
    HAL_MutexUnlock(g_awss_dev_ap_mutex);

    ret = awss_dev_ap_switchap_done == 0 ? -1 : 0;

    if (awss_dev_ap_ongoing == 0) {  // interrupt by user
        os_msleep(1000);
        return -1;
    }

    awss_dev_ap_ongoing = 0;
    extern int awss_success_notify(void);
    awss_success_notify();

AWSS_DEV_AP_FAIL:
    if (g_awss_dev_ap_mutex) {
        HAL_MutexUnlock(g_awss_dev_ap_mutex);
        HAL_MutexDestroy(g_awss_dev_ap_mutex);
    }
    g_awss_dev_ap_mutex = NULL;
    return ret;
}

int awss_dev_ap_stop(void)
{
    if (awss_dev_ap_ongoing == 0)
        return 0;

    awss_dev_ap_ongoing = 0;

    awss_trace("%s", __func__);

    if (g_awss_dev_ap_mutex)
        HAL_MutexLock(g_awss_dev_ap_mutex);

    os_awss_close_ap();

    awss_cmp_local_deinit(1);

    if (g_awss_dev_ap_mutex) {
        HAL_MutexUnlock(g_awss_dev_ap_mutex);
        HAL_MutexDestroy(g_awss_dev_ap_mutex);
        g_awss_dev_ap_mutex = NULL;
    }

    awss_dev_ap_switchap_done = 0;

    awss_trace("%s exit", __func__);

    return 0;
}

int wifimgr_process_dev_ap_switchap_request(void *ctx, void *resource, void *remote, void *request)
{
#define AWSS_DEV_AP_SWITCHA_RSP_LEN (512)
    char ssid[PLATFORM_MAX_SSID_LEN * 2 + 1] = {0}, passwd[PLATFORM_MAX_PASSWD_LEN + 1] = {0};
    int str_len = 0, success = 1, i  = 0, len = 0;
    char req_msg_id[MSG_REQ_ID_LEN] = {0};
    char random[RANDOM_MAX_LEN + 1] = {0};
    char *msg = NULL, *dev_info = NULL;
    char *str = NULL, *buf = NULL;
    char ssid_found = 0;
    int ret = -1;

    static char dev_ap_switchap_parsed = 0;
    if (dev_ap_switchap_parsed != 0)
        goto DEV_AP_SWITCHAP_END;
    dev_ap_switchap_parsed = 1;

    msg = os_zalloc(AWSS_DEV_AP_SWITCHA_RSP_LEN); 
    if (msg == NULL)
        goto DEV_AP_SWITCHAP_END;
    dev_info = os_zalloc(AWSS_DEV_AP_SWITCHA_RSP_LEN);
    if (dev_info == NULL)
        goto DEV_AP_SWITCHAP_END;

    buf = awss_cmp_get_coap_payload(request, &len);
    str = json_get_value_by_name(buf, len, "id", &str_len, 0);
    memcpy(req_msg_id, str, str_len > MSG_REQ_ID_LEN - 1 ? MSG_REQ_ID_LEN - 1 : str_len);
    awss_debug("dev ap, len:%u, %s\r\n", len, buf);
    buf = json_get_value_by_name(buf, len, "params", &len, 0);
    if (buf == NULL)
        goto DEV_AP_SWITCHAP_END;

    do {
        produce_random(aes_random, sizeof(aes_random));
        dev_info[0] = '{';
        awss_build_dev_info(AWSS_NOTIFY_DEV_BIND_TOKEN, dev_info + 1, AWSS_DEV_AP_SWITCHA_RSP_LEN - 1);
        dev_info[strlen(dev_info)] = '}';
        dev_info[AWSS_DEV_AP_SWITCHA_RSP_LEN - 1] = '\0';
        snprintf(msg, AWSS_DEV_AP_SWITCHA_RSP_LEN, AWSS_ACK_FMT, req_msg_id, 200, dev_info);

        str_len = 0;
        str = json_get_value_by_name(buf, len, "ssid", &str_len, 0);
        awss_debug("ssid, len:%u, %s\r\n", str_len, str != NULL ? str : "NULL");
        if (str && (str_len < PLATFORM_MAX_SSID_LEN)) {
            memcpy(ssid, str, str_len);
            ssid_found = 1;
        }

        if (!ssid_found) {
            str_len = 0;
            str = json_get_value_by_name(buf, len, "xssid", &str_len, 0);
            if (str && (str_len < PLATFORM_MAX_SSID_LEN * 2 - 1)) {
                memcpy(ssid, str, str_len);
                uint8_t decoded[OS_MAX_SSID_LEN] = {0};
                int len = str_len / 2;
                utils_str_to_hex(ssid, str_len, decoded, OS_MAX_SSID_LEN);
                memcpy(ssid, (const char *)decoded, len);
                ssid[len] = '\0';
            } else {
                snprintf(msg, AWSS_DEV_AP_SWITCHA_RSP_LEN, AWSS_ACK_FMT, req_msg_id, -1, "\"ssid error\"");
                success = 0;
                break;
            }
        }

        str_len = 0;
        str = json_get_value_by_name(buf, len, "random", &str_len, 0);
        if (str && str_len ==  RANDOM_MAX_LEN * 2) {
            utils_str_to_hex(str, str_len, (unsigned char *)random, RANDOM_MAX_LEN);
        } else {
            snprintf(msg, AWSS_DEV_AP_SWITCHA_RSP_LEN, AWSS_ACK_FMT, req_msg_id, -4, "\"random len error\"");
            success = 0;
            break;
        }

        str_len = 0;
        str = json_get_value_by_name(buf, len, "passwd", &str_len, 0);

        if (str_len < (PLATFORM_MAX_PASSWD_LEN * 2) - 1) {
            char encoded[PLATFORM_MAX_PASSWD_LEN * 2 + 1] = {0};
            memcpy(encoded, str, str_len);
            aes_decrypt_string(encoded, passwd, str_len,
                    0, os_get_encrypt_type(), 1, random); // 64bytes=2x32bytes
        } else {
            snprintf(msg, AWSS_DEV_AP_SWITCHA_RSP_LEN, AWSS_ACK_FMT, req_msg_id, -3, "\"passwd len error\"");
            success = 0;
        }

        if (success && is_utf8(passwd, strlen(passwd)) == 0) {
            snprintf(msg, AWSS_DEV_AP_SWITCHA_RSP_LEN, AWSS_ACK_FMT, req_msg_id, -3 , "\"passwd content error\"");
            success = 0;
        }
    } while (0);

    awss_debug("Sending message to app: %s", msg);
    awss_debug("switch to ap: '%s'", ssid);
    char topic[TOPIC_LEN_MAX] = {0};
    awss_build_topic((const char *)TOPIC_AWSS_DEV_AP_SWITCHAP, topic, TOPIC_LEN_MAX);
    for (i = 0; i < 3; i ++) {
        int result = awss_cmp_coap_send_resp(msg, strlen(msg), remote, topic, request);
        awss_debug("sending %s.", result == 0 ? "success" : "fail");
    }

    do {
        if (!success)
            break;

        os_msleep(1000);
        os_awss_close_ap();
        ret = os_awss_connect_ap(30 * 1000, ssid, passwd, 0, 0, NULL, 0);
        if (ret == 0) {
            awss_dev_ap_switchap_done = 1;
        } else {
            awss_dev_ap_setup();
        }
        awss_debug("connect '%s' %s\r\n", ssid, ret == 0 ? "success" : "fail");
    } while (0);

DEV_AP_SWITCHAP_END:
    dev_ap_switchap_parsed = 0;
    if (dev_info) os_free(dev_info);
    if (msg) os_free(msg);
    return ret;
}
#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
}
#endif
#endif
