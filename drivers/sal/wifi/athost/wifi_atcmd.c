/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "network/hal/base.h"
#include <hal/wifi.h>
#include "atparser.h"

#define TAG "wifi_port"

#define MAX_RSSI 256
#define MONITOR_PKT_MAX_LEN 2000 /* 2000? TBD later */

static int get_mac_helper(char *mac);
static int get_ip_stat_helper(hal_wifi_ip_stat_t *result);

static monitor_data_cb_t monitor_cb = NULL;
static aos_sem_t start_monitor_sem;
static aos_sem_t stop_monitor_sem;
static char mon_buf[MONITOR_PKT_MAX_LEN];
extern int at_dev_fd;

static void fetch_ip_stat(void *arg)
{
    hal_wifi_ip_stat_t ip_stat = {0};
    hal_wifi_module_t *m;

    if (!arg) {
        LOGE(TAG, "%s failed, invalid argument\r\n", __func__);
        return;
    }

    m = (hal_wifi_module_t *)arg;

    if (get_ip_stat_helper(&ip_stat) != 0) {
        LOGE(TAG, "%s failed to get ip info, exit now!", __func__);
        return;
    }

    if (m->ev_cb->ip_got != NULL) {
        m->ev_cb->ip_got(m, &ip_stat, NULL);
    }
}

static void wevent_up_handler()
{
    hal_wifi_module_t *m;

    m = hal_wifi_get_default_module();

    if (NULL == m) {
        return;
    }

    if (m->ev_cb->stat_chg != NULL) {
        m->ev_cb->stat_chg(m, NOTIFY_STATION_UP, NULL);
    }

    fetch_ip_stat(m);
}

static void wevent_down_handler()
{
    hal_wifi_module_t *m;

    m = hal_wifi_get_default_module();

    if (NULL == m) {
        return;
    }

    if (m->ev_cb->stat_chg != NULL) {
        m->ev_cb->stat_chg(m, NOTIFY_STATION_DOWN, NULL);
    }
}

/**
 * +WEVENT:STATION_UP
 * +WEVENT:STATION_DOWN
 */
static void wifi_event_handler(void *arg, char *buf, int buflen)
{
    char evt[16] = {0};

    at_read(at_dev_fd, evt, strlen("STATION_UP"));
    if (strcmp(evt, "STATION_UP") == 0) {
        LOGD(TAG, "STATION_UP event received.");

        aos_loop_schedule_call(NULL, bk7231_get_local_ip_addr, NULL);
    } else if (strcmp(evt, "STATION_DO") == 0) {
        memset(evt, 0, sizeof(evt));
        at_read(at_dev_fd, evt, 2);
        if (strcmp(evt, "WN") == 0) {
            LOGD(TAG, "STATION_DOWN event received.");
            aos_loop_schedule_call(NULL, wevent_down_handler, NULL);
        }
    } else {
        LOGE(TAG, "Invalid WEVENT event (%s) receviced.", evt);
    }
}

static int wifi_init(hal_wifi_module_t *m)
{
    LOGI(TAG, "wifi init success %d!!\n", __LINE__);
    return 0;
};

#define MAC_STR_LEN 12
// str: char[12], mac: hex[6]
static void mac_str_to_hex(char *str, uint8_t *mac)
{
    int i, k;
    char c;
    uint8_t j;

    if (!str || !mac) {
        return;
    }

    memset(mac, 0, MAC_STR_LEN >> 1);
    for (k = 0, i = 0; k < strlen(str); k++) {
        c = str[k];
        if (c == ':')
           continue;

        if (c >= '0' && c <= '9') {
            j = c - '0';
        } else if (c >= 'A' && c <= 'F') {
            j = c - 'A' + 0xa;
        } else if (c >= 'a' && c <= 'f') {
            j = c - 'a' + 0xa;
        } else {
            j = c % 0xf;
        }
        j <<= i & 1 ? 0 : 4;
        mac[i >> 1] |= j;

        i++;
    }
}

// mac - hex[6]
static void wifi_get_mac_addr(hal_wifi_module_t *m, uint8_t *mac)
{
    char mac_str[MAC_STR_LEN + 1] = {0};

    if (!mac) {
        return;
    }

    (void)m;
    LOGD(TAG, "wifi_get_mac_addr!!\n");

    get_mac_helper(mac_str);
    mac_str_to_hex(mac_str, mac);
    LOGD(TAG, "mac in hex: %02x%02x%02x%02x%02x%02x",
         mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
};

#define AT_CMD_CONNECT_AP "AT+WJAP"
#define WIFIEVENT_OOB_PREFIX "+WEVENT:"
static int wifi_start(hal_wifi_module_t *m, hal_wifi_init_type_t *init_para)
{
    char in[128] = {0};
    char out[128] = {0};
    static oob_cb wevt_oob = NULL;

    (void)init_para;

    if (!wevt_oob) {
        wevt_oob = wifi_event_handler;
        at_register_callback(at_dev_fd, WIFIEVENT_OOB_PREFIX, NULL, NULL, 0, wevt_oob, NULL);
    }

    if (strcmp(init_para->wifi_key, "open") == 0) {
        snprintf(in, sizeof(in), AT_CMD_CONNECT_AP"=%s",
                 init_para->wifi_ssid);
    } else {
        snprintf(in, sizeof(in), AT_CMD_CONNECT_AP"=%s,%s",
                 init_para->wifi_ssid, init_para->wifi_key);
    }

    LOGI(TAG, "Will connect via at cmd: %s\r\n", in);

    if (at_send_wait_reply(at_dev_fd, in, strlen(in), true, out, sizeof(out), NULL) == 0) {
        LOGI(TAG, "AT command %s succeed, rsp: %s\r\n", in, out);
    } else {
        LOGE(TAG, "AT command %s failed\r\n", in);
    }

    if (strstr(out, AT_RSP_FAIL)) {
        LOGE(TAG, "Connect wifi failed\r\n");
        return -1;
    }

    return 0;
}

static int wifi_start_adv(hal_wifi_module_t *m, hal_wifi_init_type_adv_t *init_para_adv)
{
    (void)init_para_adv;

    return 0;
}

#define AT_CMD_OBTAIN_MAC "AT+WMAC?"
#define AT_RSP_MAC_PREFIX "+WMAC"
// mac string, e.g. "BF01ADE2F5CE"
static int get_mac_helper(char *mac)
{
    static char saved_mac[MAC_STR_LEN + 1] = {0};
    static uint8_t mac_saved = 0;
    char out[128] = {0};

    if (!mac) {
        return -1;
    }

    if (mac_saved) {
        memcpy(mac, saved_mac, MAC_STR_LEN + 1);
        return 0;
    }

    if (at_send_wait_reply(at_dev_fd, AT_CMD_OBTAIN_MAC, strlen(AT_CMD_OBTAIN_MAC), true,
                           out, sizeof(out), NULL) == 0) {
        LOGI(TAG, "AT command %s succeed, rsp: %s", AT_CMD_OBTAIN_MAC, out);
    } else {
        LOGE(TAG, "AT command %s failed\r\n", AT_CMD_OBTAIN_MAC);
        return -1;
    }

    if (strstr(out, AT_RSP_FAIL) || !strstr(out, AT_RSP_MAC_PREFIX)) {
        LOGE(TAG, "Command %s executed with ERROR.", AT_CMD_OBTAIN_MAC);
        return -1;
    }

    sscanf(out, "%*[^:]:\"%[^\"]\"", saved_mac);
    LOGI(TAG, "mac result: %s\r\n", saved_mac);

    mac_saved = 1;
    memcpy(mac, saved_mac, MAC_STR_LEN + 1);

    return 0;
}

#define AT_CMD_OBTAIN_IP "AT+WJAPIP?"
#define AT_RSP_IP_PREFIX "+WJAPIP"
static int get_ip_stat_helper(hal_wifi_ip_stat_t *result)
{
    char out[128] = {0};
    int ret = 0;

    if (!result) {
        return -1;
    }

    if (at_send_wait_reply(at_dev_fd, AT_CMD_OBTAIN_IP, strlen(AT_CMD_OBTAIN_IP), true,
                           out, sizeof(out), NULL) == 0) {
        LOGI(TAG, "AT command %s succeed, rsp: %s", AT_CMD_OBTAIN_IP, out);
    } else {
        LOGE(TAG, "AT command %s failed\r\n", AT_CMD_OBTAIN_IP);
        return -1;
    }

    if (strstr(out, AT_RSP_FAIL) || !strstr(out, AT_RSP_IP_PREFIX)) {
        LOGE(TAG, "Command  %s executed with ERROR", AT_CMD_OBTAIN_IP);
        return -1;
    }

    sscanf(out, "%*[^:]:%[^,],%[^,],%[^,],%[^\r]",
           result->ip, result->mask, result->gate, result->dns);

    LOGD(TAG, "result: %s %s %s %s\r\n",
         result->ip, result->mask, result->gate, result->dns);

    ret = get_mac_helper(result->mac);

    return ret;
}

static int get_ip_stat(hal_wifi_module_t *m, hal_wifi_ip_stat_t *out_net_para, hal_wifi_type_t wifi_type)
{
    (void)wifi_type;
    (void)m;

    if (get_ip_stat_helper(out_net_para) != 0) {
        LOGE(TAG, "%s failed to get ip info!!!", __func__);
    }

    return 0;
}

static int get_link_stat(hal_wifi_module_t *m, hal_wifi_link_stat_t *out_stat)
{
    LOGE(TAG, "%s not implemented yet!\r\n", __func__);
    return 0;
}

static void start_scan(hal_wifi_module_t *m)
{
    LOGE(TAG, "%s not implemented yet!\r\n", __func__);
}

static void start_scan_adv(hal_wifi_module_t *m)
{
    LOGE(TAG, "%s not implemented yet!\r\n", __func__);
}

static int power_off(hal_wifi_module_t *m)
{
    return 0;
}

static int power_on(hal_wifi_module_t *m)
{
    return 0;
}

static int suspend(hal_wifi_module_t *m)
{
    LOGE(TAG, "%s not implemented yet!\r\n", __func__);
    return 0;
}

#define AT_CMD_SUSPEND_STA "AT+YWSSSUSPENDSTATION"
static int suspend_station(hal_wifi_module_t *m)
{
    char out[32] = {0};
    int ret = 0;

    if (at_send_wait_reply(at_dev_fd, AT_CMD_SUSPEND_STA, strlen(AT_CMD_SUSPEND_STA), true,
                           out, sizeof(out), NULL) == 0) {
        LOGI(TAG, "AT command %s succeed, rsp: %s", AT_CMD_SUSPEND_STA, out);
    } else {
        LOGE(TAG, "AT command %s failed\r\n", AT_CMD_SUSPEND_STA);
        return -1;
    }

    if (strstr(out, AT_RSP_FAIL)) {
        LOGE(TAG, "Command  %s executed with ERROR", AT_CMD_SUSPEND_STA);
        return -1;
    }

    return 0;
}

static int suspend_soft_ap(hal_wifi_module_t *m)
{
    LOGE(TAG, "%s not implemented yet!\r\n", __func__);
    return 0;
}

#define AT_CMD_SETCH "AT+YWSSSETCHANNEL"
static int set_channel(hal_wifi_module_t *m, int ch)
{
    char out[32] = {0}, cmd[sizeof(AT_CMD_SETCH) + 3];
    int ret = 0;

    snprintf(cmd, sizeof(cmd), "%s,%d", AT_CMD_SETCH, ch);

    if (at_send_wait_reply(at_dev_fd, cmd, strlen(cmd), true, out, sizeof(out), NULL) == 0) {
        LOGI(TAG, "AT command %s succeed, rsp: %s", cmd, out);
    } else {
        LOGE(TAG, "AT command %s failed\r\n", cmd);
        return -1;
    }

    if (strstr(out, AT_RSP_FAIL)) {
        LOGE(TAG, "Command  %s executed with ERROR", cmd);
        return -1;
    }

    return 0;
}

static void dump_hex(uint8_t *data, uint32_t len)
{
#if 0
#ifdef DEBUG
    LOGD(TAG, "\r\n");
    while (len--) {
        LOGD(TAG, "%02x ", *data++);
    }
    LOGD(TAG, "\r\n");
#endif
#endif
}

/**
 * ywss events:
 *     +YEVENT:MONITOR_UP
 *     +YEVENT:MONITOR_DOWN
 *     +YEVENT:rssi,len,data
 *
 *     Note: rssi and len value should be in dec format.
 */
static void ywss_cb(void *arg, char *buff, int bufflen)
{
    char c, buf[32] = {0};
    uint8_t *payload = NULL;
    int rssi = 0, nflag = 1;
    uint32_t len = 0;
    hal_wifi_link_info_t info;

    at_read(at_dev_fd, &c, 1);
    switch (c) {
        case 'M':
            memset(buf, 0, 32);
            at_read(at_dev_fd, buf, 9);
            if (strcmp(buf, "ONITOR_UP") == 0) {
                LOGD(TAG, "ywss monitor UP event received.");
                if (aos_sem_is_valid(&start_monitor_sem)) {
                    aos_sem_signal(&start_monitor_sem);
                }
            } else if (strcmp(buf, "ONITOR_DO") == 0) {
                memset(buf, 0, 32);
                at_read(at_dev_fd, buf, 2);
                if (strcmp(buf, "WN") == 0) {
                    LOGD(TAG, "ywss monitor DOWN event received.");
                    if (aos_sem_is_valid(&stop_monitor_sem)) {
                        aos_sem_signal(&stop_monitor_sem);
                    }
                } else {
                    LOGE(TAG, "invalid ywss event (expected: WN, but: %s)", buf);
                    assert(0);
                }
            } else {
                LOGE(TAG, "invalid ywss event (expected: MONITOR_xx, but: %s)", buf);
                assert(0);
            }
            break;
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        case '-':
            if (c == '-') {
                nflag = -1;
                at_read(at_dev_fd, &c, 1);
            }

            /* rssi */
            while (c != ',' && rssi < MAX_RSSI) {
                if (c > '9' || c < '0') {
                    LOGE(TAG, "ywss packet event reading rssi value failed (%c is not number char).", c);
                    assert(0);
                }
                rssi = (rssi << 3) + (rssi << 1) + (c - '0');
                at_read(at_dev_fd, &c, 1);
            }

            if (c != ',') {
                LOGE(TAG, "Invalid rssi info received.");
                assert(0);
            }

            /* len */
            at_read(at_dev_fd, &c, 1);
            while (c != ',' && len < MONITOR_PKT_MAX_LEN) {
                if (c > '9' || c < '0') {
                    LOGE(TAG, "ywss packet event reading len value failed (%c is not number char).", c);
                    assert(0);
                }
                len = (len << 3) + (len << 1) + (c - '0');
                at_read(at_dev_fd, &c, 1);
            }

            if (c != ',') {
                LOGE(TAG, "Invalid packet len info received.");
                assert(0);
            }

            rssi *= nflag;
            LOGD(TAG, "The rssi value is %d", rssi);

            payload = (uint8_t *)aos_malloc(len);
            if (!payload) {
                LOGE(TAG, "Failed to allocate memory for ywss packet.");
                assert(0);
            }

            LOGD(TAG, "The packet len value is %d", len);

            at_read(at_dev_fd, payload, len);
            dump_hex(payload, len);

            info.rssi = rssi;
            if (monitor_cb) {
                monitor_cb(payload, len, &info);
            }
            aos_free(payload);

            break;
        default:
            LOGE(TAG, "Invalid ywss event found (first char: %c)", c);
            assert(0);
            break;
    }
}

#define AT_CMD_START_MONITOR "AT+YWSSSTARTMONITOR"
static void start_monitor(hal_wifi_module_t *m)
{
    char out[32] = {0};
    static oob_cb cb = NULL;

    if (!cb) {
        cb = ywss_cb;
        at_register_callback(at_dev_fd, "+YEVENT:", NULL, mon_buf, MONITOR_PKT_MAX_LEN,
               cb, NULL);
    }

    if (aos_sem_new(&start_monitor_sem, 0) != 0) {
        LOGE(TAG, "%s failed to allocate semaphore", __func__);
        return;
    }

    if (at_send_wait_reply(at_dev_fd, AT_CMD_START_MONITOR, strlen(AT_CMD_START_MONITOR), true,
                           out, sizeof(out), NULL) == 0) {
        LOGI(TAG, "AT command %s succeed, rsp: %s", AT_CMD_START_MONITOR, out);
    } else {
        LOGE(TAG, "AT command %s failed\r\n", AT_CMD_START_MONITOR);
        aos_sem_free(&start_monitor_sem);
        return;
    }

    if (strstr(out, AT_RSP_FAIL)) {
        LOGE(TAG, "Command  %s executed with ERROR", AT_CMD_START_MONITOR);
        aos_sem_free(&start_monitor_sem);
        return;
    }

    aos_sem_wait(&start_monitor_sem, 5000);
    aos_sem_free(&start_monitor_sem);
}

#define AT_CMD_STOP_MONITOR "AT+YWSSSTOPMONITOR"
static void stop_monitor(hal_wifi_module_t *m)
{
    char out[32] = {0};
    int ret = 0;

    monitor_cb = NULL;

    if ((ret = aos_sem_new(&stop_monitor_sem, 0)) != 0) {
        LOGE(TAG, "%s failed to allocate semaphore", __func__);
        return;
    }

    if (at_send_wait_reply(at_dev_fd, AT_CMD_STOP_MONITOR, strlen(AT_CMD_STOP_MONITOR), true,
                           out, sizeof(out), NULL) == 0) {
        LOGI(TAG, "AT command %s succeed, rsp: %s", AT_CMD_STOP_MONITOR, out);
    } else {
        LOGE(TAG, "AT command %s failed\r\n", AT_CMD_STOP_MONITOR);
        aos_sem_free(&stop_monitor_sem);
        return;
    }

    if (strstr(out, AT_RSP_FAIL)) {
        LOGE(TAG, "Command  %s executed with ERROR", AT_CMD_STOP_MONITOR);
        aos_sem_free(&stop_monitor_sem);
        return;
    }

    aos_sem_wait(&stop_monitor_sem, 5000);
    aos_sem_free(&stop_monitor_sem);
}

static void register_monitor_cb(hal_wifi_module_t *m, monitor_data_cb_t fn)
{
    monitor_cb = fn;
}

static void register_wlan_mgnt_monitor_cb(hal_wifi_module_t *m, monitor_data_cb_t fn)
{
    LOGE(TAG, "%s not implemented yet!\r\n", __func__);
}

static int wlan_send_80211_raw_frame(hal_wifi_module_t *m, uint8_t *buf, int len)
{
    LOGE(TAG, "%s not implemented yet!\r\n", __func__);
    return 0;
}

hal_wifi_module_t aos_wifi_module_athost = {
    .base.name           = "aos_wifi_module_athost",
    .init                =  wifi_init,
    .get_mac_addr        =  wifi_get_mac_addr,
    .start               =  wifi_start,
    .start_adv           =  wifi_start_adv,
    .get_ip_stat         =  get_ip_stat,
    .get_link_stat       =  get_link_stat,
    .start_scan          =  start_scan,
    .start_scan_adv      =  start_scan_adv,
    .power_off           =  power_off,
    .power_on            =  power_on,
    .suspend             =  suspend,
    .suspend_station     =  suspend_station,
    .suspend_soft_ap     =  suspend_soft_ap,
    .set_channel         =  set_channel,
    .start_monitor       =  start_monitor,
    .stop_monitor        =  stop_monitor,
    .register_monitor_cb =  register_monitor_cb,
    .register_wlan_mgnt_monitor_cb = register_wlan_mgnt_monitor_cb,
    .wlan_send_80211_raw_frame = wlan_send_80211_raw_frame
};

