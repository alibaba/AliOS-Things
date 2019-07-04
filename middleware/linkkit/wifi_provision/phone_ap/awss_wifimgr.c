/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
#include "wifi_provision_internal.h"

#if defined(AWSS_SUPPORT_AHA)
#define WIFI_APINFO_LIST_LEN    (512)
#define DEV_SIMPLE_ACK_LEN      (64)

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
extern "C"
{
#endif

int wifimgr_process_mcast_get_device_info(void *ctx, void *resource, void *remote, void *request)
{
    return process_get_device_info(ctx, resource, remote, request, 1, AWSS_NOTIFY_DEV_RAND_SIGN);
}

int wifimgr_process_ucast_get_device_info(void *ctx, void *resource, void *remote, void *request)
{
    return process_get_device_info(ctx, resource, remote, request, 0, AWSS_NOTIFY_DEV_RAND_SIGN);
}
#define WLAN_CONNECTION_TIMEOUT     (30 * 1000) /* 30 seconds */
int switch_ap_done = 0;

void zconfig_force_destroy(void);
int wifimgr_process_switch_ap_request(void *ctx, void *resource, void *remote, void *request)
{
    char ssid[PLATFORM_MAX_SSID_LEN * 2 + 1] = {0}, passwd[PLATFORM_MAX_PASSWD_LEN + 1] = {0};
    int str_len = 0, success = 1, i  = 0, len = 0, enc_lvl = SEC_LVL_OPEN;
    char req_msg_id[MSG_REQ_ID_LEN] = {0};
    char *str = NULL, *buf = NULL;
    char bssid[ETH_ALEN] = {0};
    char msg[128] = {0};
    char ssid_found = 0;
    char token_found = 0;
    char topic[TOPIC_LEN_MAX] = {0};
    uint8_t token[RANDOM_MAX_LEN + 1] = {0};
    static char switch_ap_parsed = 0;
    if (switch_ap_parsed != 0)
        return SHUB_ERR;

    switch_ap_parsed = 1;

    buf = awss_cmp_get_coap_payload(request, &len);
    str = json_get_value_by_name(buf, len, "id", &str_len, 0);
    memcpy(req_msg_id, str, str_len > MSG_REQ_ID_LEN - 1 ? MSG_REQ_ID_LEN - 1 : str_len);
    awss_debug("switch ap, len:%u, %s\r\n", len, buf);
    buf = json_get_value_by_name(buf, len, "params", &len, 0);

    do {
        HAL_Snprintf(msg, sizeof(msg) - 1, AWSS_ACK_FMT, req_msg_id, 200, "\"success\"");

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
                uint8_t decoded[OS_MAX_SSID_LEN] = {0};
                int len = str_len / 2;
                memcpy(ssid, str, str_len);
                utils_str_to_hex(ssid, str_len, decoded, OS_MAX_SSID_LEN);
                memcpy(ssid, (const char *)decoded, len);
                ssid[len] = '\0';
            } else {
                HAL_Snprintf(msg, sizeof(msg) - 1, AWSS_ACK_FMT, req_msg_id, -1, "\"ssid error\"");
                success = 0;
                break;
            }
        }

        str_len = 0;
        str = json_get_value_by_name(buf, len, "bssid", &str_len, 0);
        if (str) os_wifi_str2mac(str, bssid);

        str_len = 0;
        str = json_get_value_by_name(buf, len, "cipherType", &str_len, 0);
        if (!str) {
            success = 0;
            HAL_Snprintf(msg, sizeof(msg) - 1, AWSS_ACK_FMT, req_msg_id, -4, "\"no security level error\"");
            break;
        }

        enc_lvl = atoi(str);
        if (enc_lvl != awss_get_encrypt_type()) {
            success = 0;
            HAL_Snprintf(msg, sizeof(msg) - 1, AWSS_ACK_FMT, req_msg_id, -4, "\"security level error\"");
            break;
        }

        str_len = 0;
        str = json_get_value_by_name(buf, len, "token", &str_len, 0);
        if (str && str_len ==  RANDOM_MAX_LEN * 2) {  /*token len equal to random len*/
            utils_str_to_hex(str, str_len, (unsigned char *)token, RANDOM_MAX_LEN);
            token_found = 1;
        } 

        str_len = 0;
        str = json_get_value_by_name(buf, len, "passwd", &str_len, 0);
        /* TODO: empty passwd is allow? json parse "passwd":"" result is NULL? */
        switch (enc_lvl) {
            case SEC_LVL_AES256:
                HAL_Snprintf(msg, sizeof(msg) - 1, AWSS_ACK_FMT, req_msg_id, -4, "\"aes256 not support\"");
                success = 0;
                break;
            default:
                break;
        }

        if (success == 0)
            break;

        if (0 == enc_lvl) {
            if (str_len < PLATFORM_MAX_PASSWD_LEN) {
                memcpy(passwd, str, str_len);
            } else {
                HAL_Snprintf(msg, sizeof(msg) - 1, AWSS_ACK_FMT, req_msg_id, -2, "\"passwd len error\"");
                success = 0;
            }
        } else {
            if (str_len < (PLATFORM_MAX_PASSWD_LEN * 2) - 1) {
                char encoded[PLATFORM_MAX_PASSWD_LEN * 2 + 1] = {0};
                memcpy(encoded, str, str_len);
                aes_decrypt_string(encoded, passwd, str_len,
                        0, awss_get_encrypt_type(), 1, (const char *)aes_random);
            } else {
                HAL_Snprintf(msg, sizeof(msg) - 1, AWSS_ACK_FMT, req_msg_id, -3, "\"passwd len error\"");
                AWSS_UPDATE_STATIS(AWSS_STATIS_PAP_IDX, AWSS_STATIS_TYPE_PASSWD_ERR);
                success = 0;
            }
        }

        if (success && is_utf8(passwd, strlen(passwd)) == 0) {
            HAL_Snprintf(msg, sizeof(msg) - 1, AWSS_ACK_FMT, req_msg_id,
                     enc_lvl == SEC_LVL_OPEN ? -2 : -3 , "\"passwd content error\"");
            AWSS_UPDATE_STATIS(AWSS_STATIS_PAP_IDX, AWSS_STATIS_TYPE_PASSWD_ERR);
            success = 0;
        }
    } while (0);

    awss_devinfo_notify_stop();
#ifndef DEV_BIND_DISABLE_NOTIFY
    awss_dev_bind_notify_stop();
#endif
    awss_debug("Sending message to app: %s", msg);
    awss_debug("switch to ap: '%s'", ssid); 
    awss_build_topic((const char *)TOPIC_AWSS_SWITCHAP, topic, TOPIC_LEN_MAX);
    for (i = 0; i < 5; i ++) {
        if (0 != awss_cmp_coap_send_resp(msg, strlen(msg), remote, topic, request, NULL, NULL, 0)) {
            awss_debug("sending failed.");
        } else {
            awss_debug("sending succeeded.");
        }
    }

    HAL_SleepMs(1000);

    if (!success)
        goto SWITCH_AP_END;
#ifdef AWSS_SUPPORT_APLIST
    do {
        struct ap_info * aplist = NULL;
        aplist = zconfig_get_apinfo_by_ssid((uint8_t *)ssid);
        awss_debug("connect '%s'", ssid);
        if (aplist) {
            memcpy(bssid, aplist->mac, ETH_ALEN);
            awss_debug("bssid: %02x:%02x:%02x:%02x:%02x:%02x", \
                       bssid[0], bssid[1], bssid[2], bssid[3], bssid[4], bssid[5]);
        }
    } while (0);
#endif
    AWSS_UPDATE_STATIS(AWSS_STATIS_CONN_ROUTER_IDX, AWSS_STATIS_TYPE_TIME_START);

    if (0 != awss_connect(ssid, passwd,(uint8_t *)bssid, ETH_ALEN, token_found == 1 ? token : NULL, token_found == 1 ? RANDOM_MAX_LEN : 0)) {
    } else {
        AWSS_UPDATE_STATIS(AWSS_STATIS_CONN_ROUTER_IDX, AWSS_STATIS_TYPE_TIME_SUC);
        AWSS_UPDATE_STATIS(AWSS_STATIS_PAP_IDX, AWSS_STATIS_TYPE_TIME_SUC);
        switch_ap_done = 1;

        zconfig_force_destroy();
        if(token_found == 0) {
            produce_random(aes_random, sizeof(aes_random));
        }
    }
    awss_debug("connect '%s' %s\r\n", ssid, switch_ap_done == 1 ? "success" : "fail");

SWITCH_AP_END:
    switch_ap_parsed = 0;
    return SHUB_OK;
}

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
}
#endif
#endif
