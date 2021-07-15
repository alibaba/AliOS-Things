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
#include "awss_statis.h"
#include "zconfig_utils.h"
#include "connect_ap.h"
#include "awss_security.h"
#include "sl_config.h"

#ifdef AWSS_SUPPORT_DEV_AP


#define TIMEOUT_CNT   4

typedef struct {
    char ssid[PLATFORM_MAX_SSID_LEN + 1];
    char passwd[PLATFORM_MAX_PASSWD_LEN + 1];
    uint8_t bssid[ETH_ALEN];
    uint8_t token[RANDOM_MAX_LEN + 1];
    uint16_t msgid;
    uint8_t cnt;
    uint8_t token_found;
    uint8_t got_msg;
} ap_info_t;

static int start_connect_ap(char *ssid, char *pwd, uint8_t *bssid, uint8_t *token, uint16_t msgid);
static void do_connect_ap(void);

static ap_info_t  *ap_info_ptr = NULL;
static void *g_awss_dev_ap_mutex = NULL;
static char awss_dev_ap_switchap_done = 0;
static char awss_dev_ap_switchap_resp_suc = 0;
static char awss_dev_ap_ongoing = 0;

static pair_success_callback connect_ap_cb = NULL;

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
    } while (0);

    awss_trace("ssid:%s\n", ssid);

    return os_awss_open_ap(ssid, passwd, 100, 0);
}

int awss_dev_ap_start(pair_success_callback pair_cb, int timeout_ms)
{
    int ret = -1;
    int time_used = 0;

    if (timeout_ms <= 0) {
        awss_err("awss timeout param err");
        return -1;
    }

    connect_ap_cb = pair_cb;
    if (!connect_ap_cb) {
        awss_err("pair success callback not set!\n");
        return -1;
    }

    ap_info_t dev_ap_info;
    if (g_awss_dev_ap_mutex || awss_dev_ap_ongoing) {
        awss_trace("dev ap already running");
        return -1;
    }

    if (g_awss_dev_ap_mutex == NULL) {
        g_awss_dev_ap_mutex = HAL_MutexCreate();
    }
    if (g_awss_dev_ap_mutex == NULL) {
        awss_trace("awss dev ap start fail");
        goto AWSS_DEV_AP_FAIL;
    }

    memset(&dev_ap_info, 0, sizeof(dev_ap_info));
    ap_info_ptr = &dev_ap_info;
    HAL_MutexLock(g_awss_dev_ap_mutex);

    awss_dev_ap_ongoing = 1;
    awss_dev_ap_switchap_done = 0;
    awss_dev_ap_switchap_resp_suc = 0;

    ret = awss_dev_ap_setup();
    os_msleep(1000);  // wait for dev ap to work well
    awss_cmp_local_init(AWSS_LC_INIT_DEV_AP);

    while (awss_dev_ap_ongoing && awss_dev_ap_switchap_done != -1 && time_used < timeout_ms) {
        os_msleep(200);
        time_used += 200;
        if (awss_dev_ap_switchap_done) {
            break;
        }
        do_connect_ap();
    }
    HAL_MutexUnlock(g_awss_dev_ap_mutex);

    ret = awss_dev_ap_switchap_done == 1 ? 0 : -1;

    if (awss_dev_ap_ongoing == 0) {  // interrupt by user
        os_msleep(1000);
        return ret;
    }

    awss_dev_ap_ongoing = 0;
    extern int awss_success_notify(void);

    if (awss_dev_ap_switchap_done == 1) {
        awss_success_notify();
    }

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
    if (awss_dev_ap_ongoing == 0) {
        return 0;
    }

    awss_dev_ap_ongoing = 0;

    awss_trace("%s", __func__);

    if (g_awss_dev_ap_mutex) {
        HAL_MutexLock(g_awss_dev_ap_mutex);
    }

    os_awss_close_ap();

    awss_cmp_local_deinit(1);

    if (g_awss_dev_ap_mutex) {
        HAL_MutexUnlock(g_awss_dev_ap_mutex);
        HAL_MutexDestroy(g_awss_dev_ap_mutex);
        g_awss_dev_ap_mutex = NULL;
    }

    awss_dev_ap_switchap_done = 0;
    awss_dev_ap_switchap_resp_suc = 0;

    awss_trace("%s exit", __func__);

    return 0;
}

static int awss_dev_ap_switchap_resp(void *context, int result,
                                     void *userdata, void *remote,
                                     void *message)
{
    if (result == 2) { /* success */
        awss_dev_ap_switchap_resp_suc = 1;
    }
    return 0;
}

int wifimgr_process_dev_ap_switchap_request(void *ctx, void *resource, void *remote, void *request)
{
#define AWSS_DEV_AP_SWITCHA_RSP_LEN (512)
    char ssid[PLATFORM_MAX_SSID_LEN * 2 + 1] = {0}, passwd[PLATFORM_MAX_PASSWD_LEN + 1] = {0};
    int str_len = 0, success = 1, len = 0;
    char req_msg_id[MSG_REQ_ID_LEN] = {0};
    char random[RANDOM_MAX_LEN + 1] = {0};
    char *msg = NULL, *dev_info = NULL;
    char *str = NULL, *buf = NULL;
    char bssid[ETH_ALEN] = {0};
    char ssid_found = 0;
    uint8_t token[RANDOM_MAX_LEN + 1];
    char token_found = 0;
    uint8_t isRandomKey = 0;
    const char *p_ranodm_str = NULL;
    int ret = -1;

    static char dev_ap_switchap_parsed = 0;
    if (0 == awss_dev_ap_ongoing) {
        awss_trace("dev ap: not in awss mode!\r\n");
        return -1;
    }
    if (dev_ap_switchap_parsed != 0) {
        goto DEV_AP_SWITCHAP_END;
    }
    dev_ap_switchap_parsed = 1;

    AWSS_UPDATE_STATIS(AWSS_STATIS_DAP_IDX, AWSS_STATIS_TYPE_TIME_START);

    msg = os_zalloc(AWSS_DEV_AP_SWITCHA_RSP_LEN);
    if (msg == NULL) {
        goto DEV_AP_SWITCHAP_END;
    }
    dev_info = os_zalloc(AWSS_DEV_AP_SWITCHA_RSP_LEN);
    if (dev_info == NULL) {
        goto DEV_AP_SWITCHAP_END;
    }

    buf = awss_cmp_get_coap_payload(request, &len);
    str = json_get_value_by_name(buf, len, "id", &str_len, 0);
    memcpy(req_msg_id, str, str_len > MSG_REQ_ID_LEN - 1 ? MSG_REQ_ID_LEN - 1 : str_len);
    awss_trace("dev ap, len:%u, %s\r\n", len, buf);
    buf = json_get_value_by_name(buf, len, "params", &len, 0);
    if (buf == NULL) {
        goto DEV_AP_SWITCHAP_END;
    }

    do {
        produce_random(aes_random, sizeof(aes_random));
        dev_info[0] = '{';
        awss_build_dev_info(AWSS_NOTIFY_DEV_BIND_TOKEN, dev_info + 1, AWSS_DEV_AP_SWITCHA_RSP_LEN - 1);
        dev_info[strlen(dev_info)] = '}';
        dev_info[AWSS_DEV_AP_SWITCHA_RSP_LEN - 1] = '\0';
        snprintf(msg, AWSS_DEV_AP_SWITCHA_RSP_LEN, AWSS_ACK_FMT, req_msg_id, 200, dev_info);

        /* get security version */
        str_len = 0;
        str = json_get_value_by_name(buf, len, "security", &str_len, 0);
        if (str && str_len == 3 && !memcmp("2.0", str, str_len)) {
            awss_trace("security ver = %.*s\r\n", str_len, str);
            isRandomKey = 1;
        }

        str_len = 0;
        str = json_get_value_by_name(buf, len, "ssid", &str_len, 0);
        awss_trace("ssid, len:%u, %s\r\n", str_len, str != NULL ? str : "NULL");
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
            p_ranodm_str = str;
        } else {
            snprintf(msg, AWSS_DEV_AP_SWITCHA_RSP_LEN, AWSS_ACK_FMT, req_msg_id, -4, "\"random len error\"");
            success = 0;
            break;
        }

        str_len = 0;
        str = json_get_value_by_name(buf, len, "token", &str_len, 0);
        if (str && str_len ==  RANDOM_MAX_LEN * 2) {  /* token len equal to random len */
            utils_str_to_hex(str, str_len, (unsigned char *)token, RANDOM_MAX_LEN);
            token_found = 1;
        }

        str_len = 0;
        str = json_get_value_by_name(buf, len, "bssid", &str_len, 0);
        if (str) {
            os_wifi_str2mac(str, bssid);
        }

        str_len = 0;
        str = json_get_value_by_name(buf, len, "passwd", &str_len, 0);

        if (str_len < (PLATFORM_MAX_PASSWD_LEN * 2) - 1) {
            char encoded[PLATFORM_MAX_PASSWD_LEN * 2 + 1] = {0};
            memcpy(encoded, str, str_len);
            if (isRandomKey) {
                softap_decrypt_password((const char *)encoded, (const uint8_t *)p_ranodm_str, passwd);
            }
            else {
                aes_decrypt_string(encoded, passwd, str_len,
                                0, os_get_encrypt_type(), 1, random); /* 64bytes=2x32bytes */
            }
        } else {
            snprintf(msg, AWSS_DEV_AP_SWITCHA_RSP_LEN, AWSS_ACK_FMT, req_msg_id, -3, "\"passwd len error\"");
            success = 0;
            AWSS_UPDATE_STATIS(AWSS_STATIS_DAP_IDX, AWSS_STATIS_TYPE_PASSWD_ERR);
        }

        if (success && is_utf8(passwd, strlen(passwd)) == 0) {
            snprintf(msg, AWSS_DEV_AP_SWITCHA_RSP_LEN, AWSS_ACK_FMT, req_msg_id, -3, "\"passwd content error\"");
            success = 0;
            AWSS_UPDATE_STATIS(AWSS_STATIS_DAP_IDX, AWSS_STATIS_TYPE_PASSWD_ERR);
        }
    } while (0);

    awss_trace("Sending message to app: %s", msg);
    awss_trace("switch to ap: '%s'", ssid);
    char topic[TOPIC_LEN_MAX] = {0};
    uint16_t msgid = -1;
    awss_build_topic((const char *)TOPIC_AWSS_DEV_AP_SWITCHAP, topic, TOPIC_LEN_MAX);
    int result = awss_cmp_coap_send_resp(msg, strlen(msg), remote, topic, request, awss_dev_ap_switchap_resp, &msgid, 1);
    (void)result;  /* remove complier warnings */
    awss_trace("sending %s.", result == 0 ? "success" : "fail");

    if (!success) {
        goto DEV_AP_SWITCHAP_END;
    }

    ret = start_connect_ap(ssid, passwd, (uint8_t *)bssid, token_found ? token : NULL, msgid);
    awss_trace("ready connect ap '%s' %s\r\n", ssid, ret == 0 ? "success" : "fail");

DEV_AP_SWITCHAP_END:
    dev_ap_switchap_parsed = 0;
    if (dev_info) {
        os_free(dev_info);
    }
    if (msg) {
        os_free(msg);
    }
    return ret;
}

static void do_connect_ap(void)
{
    int ret;
    if (ap_info_ptr == NULL) {
        return;
    }

    if (ap_info_ptr->got_msg == 0) {
        return;
    }

    if (awss_dev_ap_ongoing == 0) {
        AWSS_UPDATE_STATIS(AWSS_STATIS_CONN_ROUTER_IDX, AWSS_STATIS_TYPE_TIME_START);
        awss_cmp_coap_cancel_packet(ap_info_ptr->msgid);
        return;
    }

    if (awss_dev_ap_switchap_resp_suc || ++ap_info_ptr->cnt == TIMEOUT_CNT) {
        awss_cmp_coap_cancel_packet(ap_info_ptr->msgid);
        AWSS_UPDATE_STATIS(AWSS_STATIS_CONN_ROUTER_IDX, AWSS_STATIS_TYPE_TIME_START);
        os_awss_close_ap();

        /*ret = os_awss_connect_ap(WLAN_CONNECTION_TIMEOUT_MS, info->ssid, info->passwd, 0, 0, info->bssid, 0);*/
        // ret = awss_connect(ap_info_ptr->ssid, ap_info_ptr->passwd, ap_info_ptr->bssid, ETH_ALEN,
        //                    ap_info_ptr->token_found == 1 ? ap_info_ptr->token : NULL,
        //                    ap_info_ptr->token_found == 1 ? RANDOM_MAX_LEN : 0);
        if (!connect_ap_cb) {
            awss_err("pair success callback none!\n");
            return;
        }

        ret = connect_ap_cb(1, ap_info_ptr->ssid, ap_info_ptr->passwd, WLAN_CONNECTION_TIMEOUT_MS);
        if (ret == 0) {
            AWSS_UPDATE_STATIS(AWSS_STATIS_CONN_ROUTER_IDX, AWSS_STATIS_TYPE_TIME_SUC);
            awss_dev_ap_switchap_done = 1;
            AWSS_UPDATE_STATIS(AWSS_STATIS_DAP_IDX, AWSS_STATIS_TYPE_TIME_SUC);
        } else {
            // awss_dev_ap_setup();
            awss_dev_ap_switchap_done = -1;
        }
        awss_trace("connect '%s' %s\r\n", ap_info_ptr->ssid, ret == 0 ? "success" : "fail");
        return;
    }

    awss_info("wait siwtchap resp ack,cnt = %d\r\n", ap_info_ptr->cnt);

}

static int start_connect_ap(char *ssid, char *pwd, uint8_t *bssid, uint8_t *token, uint16_t msgid)
{
    if (ap_info_ptr == NULL) {
        return -1;
    }

    memset(ap_info_ptr, 0, sizeof(ap_info_t));
    if (token != NULL) {
        memcpy(ap_info_ptr->token, token, sizeof(ap_info_ptr->token));
        ap_info_ptr->token_found = 1;
    }
    strncpy(ap_info_ptr->ssid, ssid, sizeof(ap_info_ptr->ssid) - 1);
    strncpy(ap_info_ptr->passwd, pwd, sizeof(ap_info_ptr->passwd) - 1);
    memcpy(ap_info_ptr->bssid, bssid, sizeof(ap_info_ptr->bssid));
    ap_info_ptr->msgid = msgid;
    ap_info_ptr->got_msg = 1;
    return 0;
}

#endif
