/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */


#include "log.h"
#include "awss.h"
#include "awss_main.h"
#include "zconfig_utils.h"
#include "json_parser.h"
#include "awss_timer.h"
#include "awss_cmp.h"
#include "passwd.h"
#include "utils.h"

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
extern "C"
{
#endif

#define AWSS_REPORT_LEN_MAX       (256)
#define AWSS_TOKEN_TIMEOUT_MS     (45 * 1000)
#define MATCH_MONITOR_TIMEOUT_MS  (30 * 1000)
#define MATCH_REPORT_CNT_MAX      (2)

volatile char awss_report_token_suc = 0;
volatile char awss_report_token_cnt = 0;
static char awss_report_reset_suc = 0;
static char awss_report_id = 0;
static char switchap_ssid[OS_MAX_SSID_LEN] = {0};
static char switchap_passwd[OS_MAX_PASSWD_LEN] = {0};
static uint8_t switchap_bssid[ETH_ALEN] = {0};

static uint32_t awss_report_token_time = 0;

static void *report_reset_timer = NULL;
static void *report_token_timer = NULL;
static void *switchap_timer = NULL;

static int awss_report_token_to_cloud();
static int awss_report_reset_to_cloud();
static int awss_switch_ap_online();
static int awss_reboot_system();

int awss_token_remain_time()
{
    int remain = 0;
    uint32_t cur = os_get_time_ms();
    uint32_t diff = (uint32_t)(cur - awss_report_token_time);
    if (diff < AWSS_TOKEN_TIMEOUT_MS)
        remain = AWSS_TOKEN_TIMEOUT_MS - diff;
    return remain;
}

int awss_update_token()
{
    awss_report_token_time = 0;
    awss_report_token_cnt = 0;
    awss_report_token_suc = 0;

    if (report_token_timer == NULL) {
        report_token_timer = HAL_Timer_Create("rp_token", (void (*)(void *))awss_report_token_to_cloud, NULL);
    }
    HAL_Timer_Stop(report_token_timer);
    HAL_Timer_Start(report_token_timer, 10);
    produce_random(aes_random, sizeof(aes_random));
    return 0;
}

int awss_token_timeout()
{
    if (awss_report_token_time == 0)
        return 1;
    uint32_t cur = os_get_time_ms();
    if ((uint32_t)(cur - awss_report_token_time) > AWSS_TOKEN_TIMEOUT_MS)
        return 1;
    return 0;
}

int awss_report_token_reply(char *topic, int topic_len, void *payload, int payload_len, void *ctx)
{
    awss_debug("%s\r\n", __func__);

    awss_report_token_suc = 1;
    awss_stop_timer(report_token_timer);
    report_token_timer = NULL;

    return 0;
}

int awss_report_reset_reply(char *topic, int topic_len, void *payload, int payload_len, void *ctx)
{
    char rst = 0;

    awss_debug("%s\r\n", __func__);

    awss_report_reset_suc = 1;
    HAL_Kv_Set(AWSS_KV_RST, &rst, sizeof(rst), 0);

    awss_stop_timer(report_reset_timer);
    report_reset_timer = NULL;

    awss_event_post(AWSS_RESET);

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
        awss_cmp_mqtt_send(reply, packet, packet_len, 1);
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
            if (switchap_timer == NULL) {
                switchap_timer = HAL_Timer_Create("swichap_online", (void (*)(void *))awss_switch_ap_online, NULL);
            }
            HAL_Timer_Stop(switchap_timer);
            HAL_Timer_Start(switchap_timer, timeout);
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

    if (report_token_timer == NULL) {
        report_token_timer = HAL_Timer_Create("rp_token", (void (*)(void *))awss_report_token_to_cloud, NULL);
    }
    HAL_Timer_Stop(report_token_timer);
    HAL_Timer_Start(report_token_timer, MATCH_MONITOR_TIMEOUT_MS);

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

        awss_report_token_time = os_get_time_ms();

        snprintf(id_str, MSG_REQ_ID_LEN - 1, "\"%u\"", awss_report_id ++);
        utils_hex_to_str(aes_random, RANDOM_MAX_LEN, token_str, sizeof(token_str) - 1);
        snprintf(param, REPORT_TOKEN_PARAM_LEN - 1, "{\"token\":\"%s\"}", token_str);
        awss_build_packet(AWSS_CMP_PKT_TYPE_REQ, id_str, ILOP_VER, METHOD_MATCH_REPORT, param, 0, packet, &packet_len);
    }

    awss_debug("report token:%s\r\n", packet);
    char topic[TOPIC_LEN_MAX] = {0};
    awss_build_topic(TOPIC_MATCH_REPORT, topic, TOPIC_LEN_MAX);

    int ret = awss_cmp_mqtt_send(topic, packet, packet_len, 1);
    awss_debug("report token res:%d\r\n", ret);
    os_free(packet);

    return ret;
}

static int awss_switch_ap_online()
{
    os_awss_connect_ap(WLAN_CONNECTION_TIMEOUT_MS, switchap_ssid, switchap_passwd,
                       AWSS_AUTH_TYPE_INVALID, AWSS_ENC_TYPE_INVALID, switchap_bssid, 0);
    awss_stop_timer(switchap_timer);
    switchap_timer = NULL;
    memset(switchap_ssid, 0, sizeof(switchap_ssid));
    memset(switchap_bssid, 0, sizeof(switchap_bssid));
    memset(switchap_passwd, 0, sizeof(switchap_passwd));

    void *reboot_timer = HAL_Timer_Create("rb_timer", (void (*)(void *))awss_reboot_system, NULL);
    HAL_Timer_Start(reboot_timer, 1000);;

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
    if (awss_report_reset_suc) {
        return 0;
    }

    char topic[TOPIC_LEN_MAX] = {0};

    int ret = 0;
    int packet_len = AWSS_REPORT_LEN_MAX;

    if (report_reset_timer == NULL) {
        report_reset_timer = HAL_Timer_Create("report_rst", (void (*)(void *))awss_report_reset_to_cloud, NULL);
    }
    HAL_Timer_Stop(report_reset_timer);
    HAL_Timer_Start(report_reset_timer, MATCH_MONITOR_TIMEOUT_MS);

    char *packet = os_zalloc(packet_len + 1);
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
    ret = awss_cmp_mqtt_send(topic, packet, packet_len, 1);
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
    char rst = 0x01;

    awss_report_reset_suc = 0;

    HAL_Kv_Set(AWSS_KV_RST, &rst, sizeof(rst), 0);

    return awss_report_reset_to_cloud();
}

int awss_check_reset()
{
    int len = 1;
    char rst = 0;

    HAL_Kv_Get(AWSS_KV_RST, &rst, &len);

    if (rst != 0x01) { // reset flag is not set
        return 0;
    }

    awss_report_reset_suc = 0;

    return awss_report_reset_to_cloud();
}

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
}
#endif
