/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "aos/kernel.h"
#include <atparser.h>
#include <hal/wifi.h>

#include "at_util.h"

#if 0
static void extract_frame_info(uint8_t *data, int len, frame_info_t *info)
{
    ieee80211_hdr_t *hdr = (ieee80211_hdr_t *)data;

    info->src = ieee80211_get_SA(hdr);
    info->dst = ieee80211_get_DA(hdr);
    info->bssid = ieee80211_get_BSSID(hdr);
}
#endif

#ifndef MONITOR_PKT_MAX_LEN
#define MONITOR_PKT_MAX_LEN 2000
#endif

static const char *at_ok_str = "\r\nOK\r\n";
static const char *at_err_str = "\r\nERROR\r\n";

/**
 * YWSS monitor AT data event:
 *    +YEVENT:rssi,len,data
 */
static void monitor_cb(uint8_t *data, int len, hal_wifi_link_info_t *info)
{
    char header[32] = { 0 };

    snprintf(header, 31, "+YEVENT:%d,%d,", info->rssi, len);
    if (len > MONITOR_PKT_MAX_LEN) {
        LOGI(TAG, "Packet length (%d) exceed limit (%d), will drop it.", len,
             MONITOR_PKT_MAX_LEN);
        return;
    }

    if (atcmd_write(header, data, strlen(header), len) != 0) {
        LOGE(TAG, "uart send packet header failed");
        return;
    }
}

static int at_ywss_start_monitor()
{
    char *at_moinit_up_str = "\r\n+YEVENT:MONITOR_UP\r\n";

    atcmd_write(at_ok_str, NULL, strlen(at_ok_str), 0);
    atcmd_write(at_moinit_up_str, NULL, strlen(at_moinit_up_str), 0)
    aos_msleep(200);
    hal_wifi_register_monitor_cb(NULL, monitor_cb);
    hal_wifi_start_wifi_monitor(NULL);
}

static int at_ywss_stop_monitor()
{
    char *at_moinit_down_str = "\r\n+YEVENT:MONITOR_DOWN\r\n";

    atcmd_write(at_ok_str, NULL, strlen(at_ok_str), 0);
    hal_wifi_register_monitor_cb(NULL, NULL);
    hal_wifi_stop_wifi_monitor(NULL);
    atcmd_write(at_moinit_down_str, NULL, strlen(at_moinit_down_str), 0);
}

static int at_ywss_set_channel()
{
    int  ch = 0, doswitch = 0, i = 0;
    char c, *sdelmiter = AT_SEND_DELIMITER,
            tmp[sizeof(AT_SEND_DELIMITER)] = { 0 };

    LOGD(TAG, "hello %s entry\r\n", __func__);

    while (1) {
        atcmd_read(&c, 1);
        if (c == sdelmiter[0]) {
            if (strlen(AT_SEND_DELIMITER) > 1) {
                atcmd_read(tmp, strlen(AT_SEND_DELIMITER) - 1);
                if (memcmp(tmp, &sdelmiter[1], strlen(AT_SEND_DELIMITER) - 1) !=
                    0) {
                    LOGE(TAG,
                         "invalid string (%s) found in ywss set channel cmd",
                         tmp);
                    break;
                }
            }
            doswitch = 1;
            break;
        }

        if (c > '9' || c < '0') {
            LOGE(TAG,
                 "invalid channel number found (%c) in ywss set channel cmd",
                 c);
            break;
        }

        ch = (ch << 3) + (ch << 1) + c - '0';
    }

    if (doswitch) {
        LOGD(TAG, "channel to switch to %d", ch);
        hal_wifi_set_channel(NULL, ch);
        atcmd_write(at_ok_str, NULL, strlen(at_ok_str), 0);
    } else {
        atcmd_write(at_err_str, NULL, strlen(at_err_str), 0);
    }

    LOGD(TAG, "hello %s exit\r\n", __func__);
}

static int at_ywss_suspend_sta()
{
    int ret = hal_wifi_suspend_station(NULL);

    if (ret == 0) {
        atcmd_write(at_ok_str, NULL, strlen(at_ok_str), 0);
    } else {
        atcmd_write(at_err_str, NULL, strlen(at_err_str), 0);
    }
}

enum
{
    ATCMD_YWSS_START_MONITOR = 0,
    ATCMD_YWSS_STOP_MONITOR,
    ATCMD_YWSS_SET_CHANNEL,
    ATCMD_YWSS_SUSPEND_STA,
};


static const atcmd_hdl_t at_wifi_ywss_cmds_table[] = {
    { .name     = "AT+YWSSSTARTMONITOR",
      .help     = "AT+YWSSSTARTMONITOR",
      .function = at_ywss_start_monitor },
    { .name     = "AT+YWSSSTOPMONITOR",
      .help     = "AT+YWSSSTOPMONITOR",
      .function = at_ywss_stop_monitor },
    { .name     = "AT+YWSSSETCHANNEL",
      .help     = "AT+YWSSETCHANNEL",
      .function = at_ywss_set_channel },
    { .name     = "AT+YWSSSUSPENDSTATION",
      .help     = "AT+YWSSSUSPENDSTATION",
      .function = at_ywss_suspend_sta },
};

static atcmd_hdl_ptr_t get_atcmd_ywss_handler()
{
    char  prefix[MAX_ATCMD_PREFIX_LEN] = { 0 };
    char *single;
    int   index = 0, len = 0, cmdidx = -1;
    ;

    LOGD(TAG, "Hello %s entry", __func__);

    len           = 1;
    prefix[index] = 'Y';
    index += len;

    len = strlen("WSS");
    atcmd_read(prefix + index, len);
    if (strcmp(prefix + index, "WSS") != 0) {
        LOGE(TAG, "invalid cmd prefix found (%s)", prefix);
        return NULL;
    }
    index += len;

    len    = 1;
    single = prefix + index;
    atcmd_read(single, len);
    switch (*single) {
        case 'S':
            index += len;
            len = 3;
            atcmd_read(prefix + index, len);
            if (strcmp(prefix + index, "TOP") == 0) { /* AT+YWSSSTOPMONITOR */
                index += len;

                len = strlen("MONITOR" AT_SEND_DELIMITER);
                atcmd_read(prefix + index, len);
                if (strcmp(prefix + index, "MONITOR" AT_SEND_DELIMITER) != 0) {
                    LOGE(TAG, "invalid cmd prefix found (%s)", prefix);
                    break;
                }

                index += len;
                cmdidx = ATCMD_YWSS_STOP_MONITOR;
            } else if (strcmp(prefix + index, "TAR") ==
                       0) { /* AT+YWSSSTARTMONITOR */
                index += len;

                len = strlen("TMONITOR" AT_SEND_DELIMITER);
                atcmd_read(prefix + index, len);
                if (strcmp(prefix + index, "TMONITOR" AT_SEND_DELIMITER) != 0) {
                    LOGE(TAG, "invalid cmd prefix found (%s)", prefix);
                    break;
                }

                index += len;
                cmdidx = ATCMD_YWSS_START_MONITOR;
            } else if (strcmp(prefix + index, "ETC") ==
                       0) { /* AT+YWSSSETCHANNEL,<ch> */
                index += len;

                len = strlen("HANNEL,");
                atcmd_read(prefix + index, len);
                if (strcmp(prefix + index, "HANNEL,") != 0) {
                    LOGE(TAG, "invalid cmd prefix found (%s)", prefix);
                    break;
                }

                index += len;
                cmdidx = ATCMD_YWSS_SET_CHANNEL;
            } else if (strcmp(prefix + index, "USP") ==
                       0) { // AT+YWSSSUSPENDSTATION
                index += len;

                len = strlen("ENDSTATION" AT_SEND_DELIMITER);
                atcmd_read(prefix + index, len);
                if (strcmp(prefix + index, "ENDSTATION" AT_SEND_DELIMITER) !=
                    0) {
                    LOGE(TAG, "invalid cmd prefix found (%s)", prefix);
                    break;
                }

                index += len;
                cmdidx = ATCMD_YWSS_SUSPEND_STA;
            } else {
                LOGE(TAG, "invalid cmd prefix found (%s)", prefix);
            }
            break;
        default:
            LOGE(TAG, "invalid cmd prefix found (%s)", prefix);
            break;
    }

    LOGD(TAG, "cmd index is %d", cmdidx);

    LOGD(TAG, "Hello %s exit", __func__);

    return cmdidx < 0 ? NULL : (atcmd_hdl_ptr_t) &at_wifi_ywss_cmds_table[cmdidx];
}

static int wifi_ywss_init()
{
    // do somthing
    return 0;
}

static void wifi_ywss_deinit()
{
    return;
}

atcmd_op_t wifi_ywss_op = {
    .name              = "wifi_ywss",
    .prefix            = "Y",
    .init              = wifi_ywss_init,
    .deinit            = wifi_ywss_deinit,
    .get_atcmd_handler = at_wifi_ywss_cmds_table,
};

int register_atcmd_wifi_ywss_opt()
{
    return atcmd_register_handlers(&wifi_ywss_op);
}
