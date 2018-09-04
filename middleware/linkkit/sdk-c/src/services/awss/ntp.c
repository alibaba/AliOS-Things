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
#include "log.h"
#include "awss.h"
#include "awss_main.h"
#include "zconfig_utils.h"
#include "json_parser.h"
#include "awss_cmp.h"
#include "ntp.h"

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
extern "C"
{
#endif

typedef void (*ntp_reply_cb_t)(const char *);
static ntp_reply_cb_t g_ntp_reply_cb = NULL;
static char g_ntp_time[NTP_TIME_STR_MAX_LEN + 1] = {0};

int linkkit_ntp_time_reply(char *topic, int topic_len, void *payload, int payload_len, void *ctx)
{
#define DEV_TX_TIME          "deviceSendTime"
#define SERVER_RX_TIME       "serverRecvTime"
#define SERVER_TX_TIME       "serverSendTime"

    int len = 0;
    int res = -1;
    char *elem = NULL;
    char server_rx_time[NTP_TIME_STR_MAX_LEN + 1] = {0};
    char server_tx_time[NTP_TIME_STR_MAX_LEN + 1] = {0};

    memset(g_ntp_time, 0, sizeof(g_ntp_time));

    if (payload == NULL || payload_len == 0) {
        goto NTP_FAIL;
    }

    awss_debug("ntp reply len:%u, payload:%s\r\n", payload_len, payload);

    /*
     * get deviceSendTime, serverRecvTime, serverSendTime
     */
    elem = json_get_value_by_name(payload, payload_len, SERVER_TX_TIME, &len, NULL);
    if (elem == NULL || len <= 0 || len > NTP_TIME_STR_MAX_LEN) {
        goto NTP_FAIL;
    }
    memcpy(server_tx_time, elem, len);

    elem = json_get_value_by_name(payload, payload_len, SERVER_RX_TIME, &len, NULL);
    if (elem == NULL || len <= 0 || len > NTP_TIME_STR_MAX_LEN) {
        goto NTP_FAIL;
    }
    memcpy(server_rx_time, elem, len);

    elem = json_get_value_by_name(payload, payload_len, DEV_TX_TIME, &len, NULL);
    if (elem == NULL || len <= 0 || len > NTP_TIME_STR_MAX_LEN) {
        goto NTP_FAIL;
    }
    /*
     * atoi fails to convert string to integer
     * so we convert manully
     */
    uint32_t tx = 0;
    while (len -- > 0) {
        tx *= 10;
        tx += elem[0] - '0';
        elem ++;
    }
    uint32_t rx = os_get_time_ms();
    uint32_t diff = (rx - tx) >> 1;
    if (diff >= 1000000) {
        goto NTP_FAIL;
    }

    len = strlen(server_tx_time);
    elem = &server_tx_time[len > 9 ? len - 9 : 0];
    tx = atoi(elem);
    tx += diff;
    if (tx > 999999999) {
        tx = 999999999;
    }
    sprintf(elem, "%u", tx);

    strncpy(g_ntp_time, server_tx_time, sizeof(g_ntp_time) - 1);
    res = 0;

NTP_FAIL:
    if (g_ntp_reply_cb != NULL) {
        g_ntp_reply_cb(g_ntp_time);
    }
    return res;
}

int linkkit_ntp_time_request(void (*ntp_reply)(const char *ntp_offset_time_ms))
{
    int packet_len = 64;
    int final_len = 0;

    char *packet = os_zalloc(packet_len + 1);
    if (packet == NULL) {
        return -1;
    }

    g_ntp_reply_cb = ntp_reply;
    final_len = snprintf(packet, packet_len, "{\"deviceSendTime\":\"%u\"}", os_get_time_ms());

    awss_debug("report ntp:%s\r\n", packet);
    char topic[TOPIC_LEN_MAX] = {0};
    awss_build_topic(TOPIC_NTP, topic, TOPIC_LEN_MAX);

    int ret = awss_cmp_mqtt_send(topic, packet, final_len, 0);
    os_free(packet);

    return ret;
}

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
}
#endif
