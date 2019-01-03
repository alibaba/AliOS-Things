/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "aos/kernel.h"
#include "ulog/ulog.h"

#include <hal/wifi.h>

#include "at_util.h"

#define TAG "at_wifi_mgmt"

static hal_wifi_event_cb_t wifi_events;
static bool                ip_ready = 0;
static bool                inited   = false;

enum
{
    AP  = 0,
    STA = 1,

    WEVENT_STATUS_UP   = 0,
    WEVENT_STATUS_DOWN = 1,
};

/*
 * Wifi station event handler. include:
 *  +WEVENT:AP_UP
 *  +WEVENT:AP_DOWN
 *  +WEVENT:STATION_UP
 *  +WEVENT:STATION_DOWN
 */
#define MAX_ATCMD_AP_STA_STATUS_LEN 30
int notify_AP_STA_status_events(int type, int status)
{
    const char *prefix_wevent = "+WEVENT:";
    char       *status_str;
    char       *type_str;
    char        cmd[MAX_ATCMD_AP_STA_STATUS_LEN] = { 0 };
    int         offset                           = 0;

    if (type == AP) {
        type_str = "AP_";
    } else if (type == STA) {
        type_str = "STATION_";
    } else {
        LOGE(TAG, "Invalid type %d!\n", type);
        goto err;
    }

    if (status == WEVENT_STATUS_UP) {
        status_str = "UP";
    } else if (status == WEVENT_STATUS_DOWN) {
        status_str = "DOWN";
    } else {
        LOGE(TAG, "Invalid connect status %d!\n", status);
        goto err;
    }

    if (offset + strlen(prefix_wevent) < MAX_ATCMD_AP_STA_STATUS_LEN) {
        offset += snprintf(cmd + offset, MAX_ATCMD_AP_STA_STATUS_LEN - offset,
                           "\r\n%s", prefix_wevent);
    } else {
        LOGE(TAG, "at string too long %s\n", cmd);
        goto err;
    }

    if (offset + strlen(type_str) + strlen(status_str) <
        MAX_ATCMD_AP_STA_STATUS_LEN) {
        offset += snprintf(cmd + offset, MAX_ATCMD_AP_STA_STATUS_LEN - offset,
                           "%s%s\r\n", type_str, status_str);
    } else {
        LOGE(TAG, "at string too long %s\n", cmd);
        goto err;
    }

    if (insert_uart_send_msg(cmd, NULL, strlen(cmd), 0) != 0) {
        LOGE(TAG, "%s %d post send at uart task fail!\n", __func__, __LINE__);
        goto err;
    }

    return 0;

err:
    return -1;
}

static void ip_got_event(hal_wifi_module_t *m, hal_wifi_ip_stat_t *pnet,
                         void *arg)
{
    LOGD(TAG, "%s - ip: %s, gw: %s, mask: %s", __func__, pnet->ip, pnet->gate,
         pnet->mask);
}

static void stat_chg_event(hal_wifi_module_t *m, hal_wifi_event_t stat,
                           void *arg)
{
    switch (stat) {
        case NOTIFY_STATION_UP:
            ip_ready = true;
            notify_AP_STA_status_events(STA, WEVENT_STATUS_UP);
            break;
        case NOTIFY_STATION_DOWN:
            ip_ready = false;
            notify_AP_STA_status_events(STA, WEVENT_STATUS_DOWN);
            break;
        case NOTIFY_AP_UP:
            notify_AP_STA_status_events(AP, WEVENT_STATUS_UP);
            break;
        case NOTIFY_AP_DOWN:
            notify_AP_STA_status_events(AP, WEVENT_STATUS_DOWN);
            break;
        default:
            break;
    }
}

static int register_wifi_events()
{
    hal_wifi_module_t *m;

    m = hal_wifi_get_default_module();
    if (!m) {
        LOGE(TAG, "failed: no default wifi module.");
        return -1;
    }

    /* m->ev_cb is declared as const, can only be assigned once. */
    if (m->ev_cb == NULL) {
        m->ev_cb = &wifi_events;
    }

    return 0;
}

static int start_wifi(const char *ssid, const char *key)
{
    int                  ret = -1;
    hal_wifi_init_type_t type;

    if (!ssid || !key) {
        LOGE(TAG, "%s: invalid argument.", __func__);
        LOGE(TAG, "Starting wifi failed.");
        return -1;
    }

    if (register_wifi_events() != 0) {
        LOGE(TAG, "%s failed to register wifi events.", __func__);
        return -1;
    }
    wifi_events.ip_got   = ip_got_event;
    wifi_events.stat_chg = stat_chg_event;


    /*ret = aos_sem_new(&start_sem, 0);
    if (0 != ret) {
        LOGE(TAG, "%s failed to allocate sem.", __func__);
        return;
    }*/

    memset(&type, 0, sizeof(type));
    type.wifi_mode = STATION;
    type.dhcp_mode = DHCP_CLIENT;
    strncpy(type.wifi_ssid, ssid, sizeof(type.wifi_ssid) - 1);
    strncpy(type.wifi_key, key, sizeof(type.wifi_key) - 1);
    ret = hal_wifi_start(NULL, &type);
    if (ret != 0) {
        LOGE(TAG, "%s failed to start hal wifi.", __func__);
        return -1;
    }

    LOGD(TAG, "Wifi started (ssid: %s, password: %s').", ssid, key);
    // aos_sem_wait(&start_sem, 60000);
    // aos_sem_free(&start_sem);
    return 0;
}

#define MAX_WIFI_SSID_LEN 32
#define MAX_WIFI_KEY_LEN  64
// AT+WJAP=ssid,key
int atcmd_ap_connect()
{
    char ssid[MAX_WIFI_SSID_LEN + 1] = { 0 };
    char key[MAX_WIFI_KEY_LEN + 1]   = { 0 };
    int  ret;

    if (!inited) {
        LOGE(TAG, "at host not inited yet!");
        goto err;
    }

    // ssid
    ret = atcmd_socket_data_info_get(ssid, sizeof(ssid), NULL);
    if (ret < 0) {
        LOGE(TAG, "Invalid ssid %s !!!\r\n", ssid);
        goto err;
    }

    // key
    ret = atcmd_socket_data_info_get(key, sizeof(key), NULL);
    if (ret < 0) {
        LOGE(TAG, "Invalid key %s !!!\r\n", key);
        goto err;
    }

    ret = start_wifi(ssid, key);
    if (ret < 0) {
        LOGE(TAG, "Start wifi fail !!!\r\n");
        goto err;
    }

    notify_atcmd_recv_status(ATCMD_SUCCESS);
    return 0;

err:
    notify_atcmd_recv_status(ATCMD_FAIL);
    return -1;
}

#define MAX_WIFI_IPINFO_LEN 90
// AT+WJAPIP:<ip>,<msk>,<gateway>,<dns>
int atcmd_get_ip()
{
    const char        *prefix_wjapip                 = "+WJAPIP:";
    char               response[MAX_WIFI_IPINFO_LEN] = { 0 };
    hal_wifi_ip_stat_t ip_stat;
    int                ret;
    int                offset = 0;

    memset(&ip_stat, 0, sizeof(ip_stat));
    ret = hal_wifi_get_ip_stat(NULL, &ip_stat, STATION);
    if (ret != 0) {
        LOGE(TAG, "%s get ip fail\r\n", __func__);
        goto err;
    }

    // AT_RECV_PREFIX
    if (offset + strlen(AT_RECV_PREFIX) < MAX_WIFI_IPINFO_LEN) {
        offset += snprintf(response + offset, MAX_WIFI_IPINFO_LEN - offset,
                           "%s", AT_RECV_PREFIX);
    } else {
        LOGE(TAG, "at string too long %s\n", response);
        goto err;
    }

    // WJAPIP prefix
    if (offset + strlen(prefix_wjapip) < MAX_WIFI_IPINFO_LEN) {
        offset += snprintf(response + offset, MAX_WIFI_IPINFO_LEN - offset,
                           "%s", prefix_wjapip);
    } else {
        LOGE(TAG, "at string too long %s\n", response);
        goto err;
    }

    // ip info
    if (offset + strlen(ip_stat.ip) * 4 + 4 < MAX_WIFI_IPINFO_LEN) {
        offset += snprintf(response + offset, MAX_WIFI_IPINFO_LEN - offset,
                           "%s,%s,%s,%s\r", ip_stat.ip, ip_stat.mask,
                           ip_stat.gate, ip_stat.dns);
    } else {
        LOGE(TAG, "at string too long %s\n", response);
        goto err;
    }

    // AT_RECV_PREFIX
    if (offset + strlen(AT_RECV_PREFIX) < MAX_WIFI_IPINFO_LEN) {
        offset += snprintf(response + offset, MAX_WIFI_IPINFO_LEN - offset,
                           "%s", AT_RECV_PREFIX);
    } else {
        LOGE(TAG, "at string too long %s\n", response);
        goto err;
    }

    if (offset + strlen(AT_RECV_SUCCESS_POSTFIX) < MAX_WIFI_IPINFO_LEN) {
        offset += snprintf(response + offset, MAX_WIFI_IPINFO_LEN - offset,
                           "%s", AT_RECV_SUCCESS_POSTFIX);
    } else {
        LOGE(TAG, "at string too long %s\n", response);
        goto err;
    }

    if (insert_uart_send_msg(response, NULL, strlen(response), 0) != 0) {
        LOGE(TAG, "%s %d post send at uart task fail!\n", __func__, __LINE__);
        goto err;
    }

    return 0;

err:
    notify_atcmd_recv_status(ATCMD_FAIL);
    return -1;
}

#define MAX_WIFI_MACINFO_LEN 40
// AT+WMAC:<mac>
int atcmd_get_mac()
{
    const char        *prefix_wmac                    = "+WMAC";
    char               response[MAX_WIFI_MACINFO_LEN] = { 0 };
    hal_wifi_ip_stat_t ip_stat;
    int                ret;
    int                offset = 0;

    memset(&ip_stat, 0, sizeof(ip_stat));
    ret = hal_wifi_get_ip_stat(NULL, &ip_stat, STATION);
    if (ret != 0) {
        LOGE(TAG, "%s get ip fail\r\n", __func__);
        goto err;
    }

    // AT_RECV_PREFIX
    if (offset + strlen(AT_RECV_PREFIX) < MAX_WIFI_MACINFO_LEN) {
        offset += snprintf(response + offset, MAX_WIFI_MACINFO_LEN - offset,
                           "%s", AT_RECV_PREFIX);
    } else {
        LOGE(TAG, "at string too long %s\n", response);
        goto err;
    }

    // WJAPIP prefix
    if (offset + strlen(prefix_wmac) + 1 < MAX_WIFI_MACINFO_LEN) {
        offset += snprintf(response + offset, MAX_WIFI_MACINFO_LEN - offset,
                           "%s:", prefix_wmac);
    } else {
        LOGE(TAG, "at string too long %s\n", response);
        goto err;
    }

    // mac info
    if (offset + strlen(ip_stat.mac) < MAX_WIFI_MACINFO_LEN) {
        offset += snprintf(response + offset, MAX_WIFI_MACINFO_LEN - offset,
                           "%s\r", ip_stat.mac);
    } else {
        LOGE(TAG, "at string too long %s\n", response);
        goto err;
    }

    // AT_RECV_PREFIX
    if (offset + strlen(AT_RECV_PREFIX) < MAX_WIFI_MACINFO_LEN) {
        offset += snprintf(response + offset, MAX_WIFI_MACINFO_LEN - offset,
                           "%s", AT_RECV_PREFIX);
    } else {
        LOGE(TAG, "at string too long %s\n", response);
        goto err;
    }

    if (offset + strlen(AT_RECV_SUCCESS_POSTFIX) < MAX_WIFI_MACINFO_LEN) {
        offset += snprintf(response + offset, MAX_WIFI_MACINFO_LEN - offset,
                           "%s", AT_RECV_SUCCESS_POSTFIX);
    } else {
        LOGE(TAG, "at string too long %s\n", response);
        goto err;
    }

    if (insert_uart_send_msg(response, NULL, strlen(response), 0) != 0) {
        LOGE(TAG, "%s %d post send at uart task fail!\n", __func__, __LINE__);
        goto err;
    }

    return 0;

err:
    notify_atcmd_recv_status(ATCMD_FAIL);
    return -1;
}

enum
{
    ATCMD_WJAP_CONN = 0,
    ATCMD_WJAP_IP,
    ATCMD_WJAP_MAC,
};

static const atcmd_hdl_t at_wifi_mgmt_cmds_table[] = {
    { .name     = "AT+WJAP=",
      .help     = "AT+WJAP=<ssid>,<key>",
      .function = atcmd_ap_connect },
    { .name = "AT+WJAPIP?", .help = "AT+WJAPIP?", .function = atcmd_get_ip },
    { .name = "AT+WMAC?", .help = "AT+WMAC?", .function = atcmd_get_mac },
};

static atcmd_hdl_ptr_t get_atcmd_wifi_handler()
{
    char prefix[MAX_ATCMD_PREFIX_LEN] = { 0 };
    char single;
    int  index = -1;

    atcmd_read(&single, 1);

    switch (single) {
        case 'J':
            // Eat AP
            atcmd_read(prefix, 2);
            if (memcmp(prefix, "AP", 2) != 0) {
                LOGE(TAG, "invalid wifi prefix %s\n", prefix);
                break;
            }

            atcmd_read(&single, 1);
            if (single == '=') {
                index = ATCMD_WJAP_CONN;
            } else if (single == 'I') {
                atcmd_read(prefix, 2);
                if (memcmp(prefix, "P?", 2) == 0) {
                    index = ATCMD_WJAP_IP;
                } else {
                    LOGE(TAG, "invalid wifi prefix %s\n", prefix);
                }
            } else {
                LOGE(TAG, "invalid wifi prefix %c\n", single);
            }

            break;

        case 'M':
            atcmd_read(prefix, 3);
            if (memcmp(prefix, "AC?", 3) == 0) {
                index = ATCMD_WJAP_MAC;
            } else {
                LOGE(TAG, "invalid wifi prefix %s\n", prefix);
            }
            break;

        default: {
            LOGE(TAG, "invalid wifi prefix %c\n", single);
            break;
        }
    }

    if (index >= 0) {
        return (atcmd_hdl_ptr_t) &at_wifi_mgmt_cmds_table[index];
    }

    return NULL;
}

static int wifi_mgmt_init()
{
    // do somthing
    inited = true;

    return 0;
}

static void wifi_mgmt_deinit()
{
    inited = false;

    return;
}

atcmd_op_t wifi_mgmt_op = {
    .name              = "wifi_mgmt",
    .prefix            = "W",
    .init              = wifi_mgmt_init,
    .deinit            = wifi_mgmt_deinit,
    .get_atcmd_handler = get_atcmd_wifi_handler,
};

int register_atcmd_wifi_mgmt_opt()
{
    return atcmd_register_handlers(&wifi_mgmt_op);
}
