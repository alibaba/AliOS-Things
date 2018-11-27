/*
 * Copyright (c) 2014-2016 Alibaba Group. All rights reserved.
 *
 * Alibaba Group retains all right, title and interest (including all
 * intellectual property rights) in and to this computer program, which is
 * protected by applicable intellectual property laws.  Unless you have
 * obtained a separate written license from Alibaba Group., you are not
 * authorized to utilize all or a part of this computer program for any
 * purpose (including reproduction, distribution, modification, and
 * compilation into object code), and you must immediately destroy or
 * return to Alibaba Group all copies of this computer program.  If you
 * are licensed by Alibaba Group, your rights to utilize this computer
 * program are limited by the terms of that license.  To obtain a license,
 * please contact Alibaba Group.
 *
 * This computer program contains trade secrets owned by Alibaba Group.
 * and, unless unauthorized by Alibaba Group in writing, you agree to
 * maintain the confidentiality of this computer program and related
 * information and to not disclose this computer program and related
 * information to any other person or entity.
 *
 * THIS COMPUTER PROGRAM IS PROVIDED AS IS WITHOUT ANY WARRANTIES, AND
 * Alibaba Group EXPRESSLY DISCLAIMS ALL WARRANTIES, EXPRESS OR IMPLIED,
 * INCLUDING THE WARRANTIES OF MERCHANTIBILITY, FITNESS FOR A PARTICULAR
 * PURPOSE, TITLE, AND NONINFRINGEMENT.
 */
#include "awss_main.h"
#include "log.h"
#include "zconfig_utils.h"
#include "json_parser.h"
#include "work_queue.h"
#include "awss_cmp.h"
#include "passwd.h"
#include "utils.h"

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
extern "C"
{
#endif

#define AWSS_REPORT_LEN_MAX       (256)
#define MATCH_MONITOR_TIMEOUT_MS  (30 * 1000)
#define MATCH_REPORT_CNT_MAX      (2)

volatile char awss_report_token_suc = 0;
volatile char awss_report_token_cnt = 0;
static char awss_report_reset_suc = 0;
static char awss_report_reset_cnt = 0;
static char awss_report_id = 0;
static char switchap_ssid[OS_MAX_SSID_LEN + 1] = {0};
static char switchap_passwd[OS_MAX_PASSWD_LEN] = {0};
static uint8_t switchap_bssid[ETH_ALEN] = {0};

static int awss_report_token_to_cloud();
static int awss_report_reset_to_cloud();
static int awss_switch_ap_online();
static int awss_reboot_system();

static struct work_struct match_work = {
    .func = (work_func_t) &awss_report_token_to_cloud,
    .prio = DEFAULT_WORK_PRIO,
    .name = "match",
};

static struct work_struct reset_work = {
    .func = (work_func_t) &awss_report_reset_to_cloud,
    .prio = DEFAULT_WORK_PRIO,
    .name = "reset",
};

static struct work_struct switchap_work = {
    .func = (work_func_t) &awss_switch_ap_online,
    .prio = DEFAULT_WORK_PRIO,
    .name = "switchap",
};

static struct work_struct reboot_work = {
    .func = (work_func_t) &awss_reboot_system,
    .prio = DEFAULT_WORK_PRIO,
    .name = "reboot",
};

int awss_report_token_reply(char *topic, int topic_len, void *payload, int payload_len, void *ctx)
{
    awss_report_token_suc = 1;
    awss_debug("%s\r\n", __func__);
    return 0;
}

int awss_report_reset_reply(char *topic, int topic_len, void *payload, int payload_len, void *ctx)
{
    awss_report_reset_suc = 1;
    awss_debug("%s\r\n", __func__);
    return 0;
}

int awss_online_switchap(char *topic, int topic_len, void *payload, int payload_len, void *ctx)
{
#define SWITCHAP_RSP_LEN   (64)
#define AWSS_BSSID_STR_LEN (17)
#define AWSS_SSID          "ssid"
#define AWSS_PASSWD        "passwd"
#define AWSS_BSSID         "bssid"
#define AWSS_SWITCH_MODE   "switchMode"
#define AWSS_TIMEOUT       "timeout"

    int len = 0, timeout = 0;
    char *packet = NULL, *awss_info = NULL, *elem = NULL;
    int packet_len = SWITCHAP_RSP_LEN, awss_info_len = 0;

    awss_debug("%s\r\n", __func__);
    if (payload == NULL || payload_len == 0) {
        goto ONLINE_SWITCHAP_FAIL;
    }

    awss_debug("online switchap len:%u, payload:%s\r\n", payload_len, payload);
    packet = os_zalloc(packet_len + 1);
    if (packet == NULL) {
        goto ONLINE_SWITCHAP_FAIL;
    }

    awss_info = json_get_value_by_name(payload, payload_len, AWSS_JSON_PARAM, &awss_info_len, NULL);
    if (awss_info == NULL || awss_info_len == 0) {
        goto ONLINE_SWITCHAP_FAIL;
    }

    /*
     * get SSID , PASSWD, BSSID of router
     */
    elem = json_get_value_by_name(awss_info, awss_info_len, AWSS_SSID, &len, NULL);
    if (elem == NULL || len <= 0 || len >= OS_MAX_SSID_LEN) {
        goto ONLINE_SWITCHAP_FAIL;
    }
    memset(switchap_ssid, 0, sizeof(switchap_ssid));
    memcpy(switchap_ssid, elem, len);

    len = 0;
    elem = json_get_value_by_name(awss_info, awss_info_len, AWSS_PASSWD, &len, NULL);
    if (elem == NULL || len <= 0 || len >= OS_MAX_PASSWD_LEN) {
        goto ONLINE_SWITCHAP_FAIL;
    }
    memset(switchap_passwd, 0, sizeof(switchap_passwd));
    memcpy(switchap_passwd, elem, len);

    len = 0;
    memset(switchap_bssid, 0, sizeof(switchap_bssid));
    elem = json_get_value_by_name(awss_info, awss_info_len, AWSS_BSSID, &len, NULL);
    if (elem != NULL && len == AWSS_BSSID_STR_LEN) {
        uint8_t i = 0;
        char *bssid_str = elem;
        // convert bssid string to bssid value
        while (i < OS_ETH_ALEN) {
            switchap_bssid[i ++] = (uint8_t)strtol(bssid_str, &bssid_str, 16);
            ++ bssid_str;
            /*
             * fix the format of bssid string is not legal.
             */
            if ((uint32_t)((unsigned long)bssid_str - (unsigned long)elem) > AWSS_BSSID_STR_LEN) {
                memset(switchap_bssid, 0, sizeof(switchap_bssid));
                break;
            }
        }
    }

    len = 0;
    elem = json_get_value_by_name(awss_info, awss_info_len, AWSS_SWITCH_MODE, &len, NULL);
    if (elem != NULL && (elem[0] == '0' || elem[0] == 0)) {
        elem = json_get_value_by_name(awss_info, awss_info_len, AWSS_TIMEOUT, &len, NULL);
        if (elem) {
            timeout = (int)strtol(elem, &elem, 16);
        }
    }

    {
        // reduce stack used
        char *id = NULL;
        char id_str[MSG_REQ_ID_LEN] = {0};
        id = json_get_value_by_name(payload, payload_len, AWSS_JSON_ID, &len, NULL);
        memcpy(id_str, id, len > MSG_REQ_ID_LEN - 1 ? MSG_REQ_ID_LEN - 1 : len);
        awss_build_packet(AWSS_CMP_PKT_TYPE_RSP, id_str, ILOP_VER, METHOD_EVENT_ZC_SWITCHAP, "{}", 200, packet, &packet_len);
    }
    {
        char reply[TOPIC_LEN_MAX] = {0};
        awss_build_topic(TOPIC_SWITCHAP_REPLY, reply, TOPIC_LEN_MAX);
        awss_cmp_mqtt_send(reply, packet, packet_len);
        os_free(packet);
    }

    /*
     * make sure the response would been received
     */
    if (timeout < 1000) {
        timeout = 1000;
    }

    {
        uint8_t bssid[ETH_ALEN] = {0};
        char ssid[OS_MAX_SSID_LEN + 1] = {0}, passwd[OS_MAX_PASSWD_LEN + 1] = {0};
        os_wifi_get_ap_info(ssid, passwd, bssid);
        /*
         * switch ap when destination ap is differenct from current ap
         */
        if (strncmp(ssid, switchap_ssid, sizeof(ssid)) ||
            memcmp(bssid, switchap_bssid, sizeof(bssid)) ||
            strncmp(passwd, switchap_passwd, sizeof(passwd))) {
            cancel_work(&switchap_work);
            queue_delayed_work(&switchap_work, timeout);
        }
    }

    return 0;

ONLINE_SWITCHAP_FAIL:
    log_warn("ilop online switchap failed");
    memset(switchap_ssid, 0, sizeof(switchap_ssid));
    memset(switchap_bssid, 0, sizeof(switchap_bssid));
    memset(switchap_passwd, 0, sizeof(switchap_passwd));
    if (packet) {
        os_free(packet);
    }
    return -1;
}

static int awss_report_token_to_cloud()
{
#define REPORT_TOKEN_PARAM_LEN  (64)
    if (awss_report_token_suc || awss_report_token_cnt ++ > MATCH_REPORT_CNT_MAX) {
        return 0;
    }

    queue_delayed_work(&match_work, MATCH_MONITOR_TIMEOUT_MS);

    int packet_len = AWSS_REPORT_LEN_MAX;

    char *packet = os_zalloc(packet_len + 1);
    if (packet == NULL) {
        return -1;
    }

    {
        // reduce stack used
        unsigned char i;
        char id_str[MSG_REQ_ID_LEN] = {0};
        char param[REPORT_TOKEN_PARAM_LEN] = {0};
        char token_str[(RANDOM_MAX_LEN << 1) + 1] = {0};

        for (i = 0; i < sizeof(aes_random); i ++)
            if (aes_random[i] != 0x00) {
                break;
            }
        if (i >= sizeof(aes_random)) {
            produce_random(aes_random, sizeof(aes_random));
        }

        snprintf(id_str, MSG_REQ_ID_LEN - 1, "\"%u\"", awss_report_id ++);
        utils_hex_to_str(aes_random, RANDOM_MAX_LEN, token_str, sizeof(token_str) - 1);
        snprintf(param, REPORT_TOKEN_PARAM_LEN - 1, "{\"token\":\"%s\"}", token_str);
        awss_build_packet(AWSS_CMP_PKT_TYPE_REQ, id_str, ILOP_VER, METHOD_MATCH_REPORT, param, 0, packet, &packet_len);
    }

    awss_debug("report token:%s\r\n", packet);
    char topic[TOPIC_LEN_MAX] = {0};
    awss_build_topic(TOPIC_MATCH_REPORT, topic, TOPIC_LEN_MAX);

    int ret = awss_cmp_mqtt_send(topic, packet, packet_len);
    os_free(packet);
    awss_debug("report token result:%d\r\n", ret);

    return ret;
}

static int awss_switch_ap_online()
{
    os_awss_connect_ap(WLAN_CONNECTION_TIMEOUT_MS, switchap_ssid, switchap_passwd,
                       AWSS_AUTH_TYPE_INVALID, AWSS_ENC_TYPE_INVALID, switchap_bssid, 0);
    memset(switchap_ssid, 0, sizeof(switchap_ssid));
    memset(switchap_bssid, 0, sizeof(switchap_bssid));
    memset(switchap_passwd, 0, sizeof(switchap_passwd));
    queue_delayed_work(&reboot_work, 1000);

    return 0;
}

static int awss_reboot_system()
{
    os_reboot();
    while (1);
    return 0;
}

static int awss_report_reset_to_cloud()
{
    if (awss_report_reset_suc || awss_report_reset_cnt ++ >= MATCH_REPORT_CNT_MAX) {
        return 0;
    }

    char topic[TOPIC_LEN_MAX] = {0};

    int ret = 0;
    int packet_len = AWSS_REPORT_LEN_MAX;
    char *packet = os_zalloc(packet_len + 1);

    queue_delayed_work(&reset_work, MATCH_MONITOR_TIMEOUT_MS);

    if (packet == NULL) {
        return -1;
    }

    {
        char id_str[MSG_REQ_ID_LEN] = {0};
        snprintf(id_str, MSG_REQ_ID_LEN - 1, "\"%u\"", awss_report_id ++);
        awss_build_packet(AWSS_CMP_PKT_TYPE_REQ, id_str, ILOP_VER, METHOD_RESET_REPORT, "{}", 0, packet, &packet_len);
    }

    awss_debug("report reset:%s\r\n", packet);

    awss_build_topic(TOPIC_RESET_REPORT, topic, TOPIC_LEN_MAX);
    ret = awss_cmp_mqtt_send(topic, packet, packet_len);
    os_free(packet);
    awss_debug("report reset result:%d\r\n", ret);

    return ret;
}

int awss_report_token()
{
    awss_report_token_cnt = 0;
    awss_report_token_suc = 0;

    return awss_report_token_to_cloud();
}

int awss_report_reset()
{
    int ret = 0;
    awss_report_reset_cnt = 0;
    awss_report_reset_suc = 0;

    ret = awss_report_reset_to_cloud();
    if (ret != 0) {
        return ret;
    }

    while (1) {
        if (awss_report_reset_suc) {
            break;
        }
        if (awss_report_reset_cnt >= MATCH_REPORT_CNT_MAX) {
            return -1;
        }
        os_msleep(100);
    }
    cancel_work(&reset_work);

    return 0;
}

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
}
#endif
