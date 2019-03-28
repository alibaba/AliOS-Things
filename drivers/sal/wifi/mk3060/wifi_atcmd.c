/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ulog/ulog.h"

#include "network/hal/base.h"
#include <hal/wifi.h>
#include "atparser.h"

#define TAG "wifi_port"

#define AT_RSP_SUCCESS "OK"
#define AT_RSP_FAIL "ERROR"

extern int at_dev_fd;
static int get_mac_helper(char *mac);
static int get_ip_stat_helper(hal_wifi_ip_stat_t *result);

static void fetch_ip_stat(void *arg)
{
    hal_wifi_ip_stat_t ip_stat = {0};
    hal_wifi_module_t *m;

    if (!arg) {
        LOGE(TAG, "%s failed, invalid argument\r\n", __func__);
        return;
    }

    m = (hal_wifi_module_t *)arg;

    get_ip_stat_helper(&ip_stat);

    if (m->ev_cb->ip_got != NULL) {
        m->ev_cb->ip_got(m, &ip_stat, NULL);
    }
}

void at_wevent_handler(void *arg, char *buf, int buflen)
{
    hal_wifi_module_t *m;

    if (NULL == arg){
        m = hal_wifi_get_default_module();
    } else {
        m = (hal_wifi_module_t *)arg;
    }

    if (NULL == m) {
        return;
    }

    if (NULL == m->ev_cb) {
        return;
    }

    if (m->ev_cb->stat_chg != NULL) {
        m->ev_cb->stat_chg(m, NOTIFY_STATION_UP, NULL);
    }

    fetch_ip_stat(m);
}

static int wifi_init(hal_wifi_module_t *m)
{
    LOGI(TAG, "wifi init success!!\n");
    return 0;
};

#define MAC_STR_LEN 12
// str: char[12], mac: hex[6]
static void mac_str_to_hex(char *str, uint8_t *mac)
{
    int i, k;
    char c;
    unsigned int j;

    if (!str || !mac) return;

    memset(mac, 0, MAC_STR_LEN >> 1);
    for (k = 0, i = 0; k < strlen(str); k++) {
        c = str[k];
        if (c == ':')
           continue;

        if (c >= '0' && c <= '9') j = c - '0';
        else if (c >= 'A' && c <= 'F') j = c - 'A' + 0xa;
        else if (c >= 'a' && c <= 'f') j = c - 'a' + 0xa;
        else j = c % 0xf;
        j <<= i & 1 ? 0 : 4;
        mac[i>>1] |= j;

        i++;
    }
}

// mac - hex[6]
static void wifi_get_mac_addr(hal_wifi_module_t *m, uint8_t *mac)
{
    char mac_str[MAC_STR_LEN+1] = {0};

    if (!mac) return;

    (void)m;
    LOGD(TAG, "wifi_get_mac_addr!!\n");

    get_mac_helper(mac_str);
    mac_str_to_hex(mac_str, mac);
    LOGI(TAG, "mac in hex: %02x%02x%02x%02x%02x%02x",
      mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
};

#define AT_CMD_CONNECT_AP "AT+WJAP"
static int wifi_start(hal_wifi_module_t *m, hal_wifi_init_type_t *init_para)
{
    char in[128] = {0};
    char out[128] = {0};

    (void)init_para;


    if (strcmp(init_para->wifi_key, "open") == 0) {
        snprintf(in, sizeof(in), AT_CMD_CONNECT_AP"=%s",
          init_para->wifi_ssid);
    } else {
        snprintf(in, sizeof(in), AT_CMD_CONNECT_AP"=%s,%s",
          init_para->wifi_ssid, init_para->wifi_key);
    }

    LOGI(TAG, "Will connect via at cmd: %s\r\n", in);

    if (at_send_wait_reply(at_dev_fd, in, strlen(in), true, NULL, 0, out, sizeof(out), NULL) == 0)
        LOGI(TAG, "AT command %s succeed, rsp: %s\r\n", in, out);
    else
        LOGE(TAG, "AT command %s failed\r\n", in);

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
    char out[128] = {0};
    char *at_mac_str = AT_CMD_OBTAIN_MAC;

    if (!mac) return -1;

    if (at_send_wait_reply(at_dev_fd, at_mac_str, strlen(at_mac_str), true,
                           NULL, 0, out, sizeof(out), NULL) == 0) {
        LOGI(TAG, "AT command %s succeed, rsp: %s", AT_CMD_OBTAIN_MAC, out);
    } else {
        LOGE(TAG, "AT command %s failed\r\n", AT_CMD_OBTAIN_MAC);
        return -1;
    }

    if (strstr(out, AT_RSP_FAIL) || !strstr(out, AT_RSP_MAC_PREFIX)) {
        LOGE(TAG, "Command %s executed with ERROR.", AT_CMD_OBTAIN_MAC);
        return -1;
    }

    sscanf(out, "%*[^:]:\"%[^\"]\"", mac);
    LOGI(TAG, "mac result: %s\r\n", mac);

    return 0;
}

#define AT_CMD_OBTAIN_IP "AT+WJAPIP?"
#define AT_RSP_IP_PREFIX "+WJAPIP"
static int get_ip_stat_helper(hal_wifi_ip_stat_t *result)
{
    char out[128] = {0};
    int ret = 0;
    char *at_ip_str = AT_CMD_OBTAIN_IP;

    if (!result) return -1;

    if (at_send_wait_reply(at_dev_fd, at_ip_str, strlen(at_ip_str), true,
                           NULL, 0, out, sizeof(out), NULL) == 0) {
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

    get_ip_stat_helper(out_net_para);

    return 0;
}

#define CURRENTAPINFO       "AT+WJAP?"
#define CURRENTAPINFOPREFIX "+WJAP"
static int _char2num(char a)
{
    if (a >= '0' && a <='9')
    {
        return (int)(a - '0');
    }

    if (a >= 'a' && a <= 'f')
    {
        return (int)(a - 'a') + 10;
    }

    if (a >= 'A' && a <= 'F')
    {
        return (int)(a - 'A') + 10;
    }

    return -1;
}

static int get_link_stat(hal_wifi_module_t *m, hal_wifi_link_stat_t *out_stat)
{
    /* CURRENTAPINFO */
    char out[128] = {0};
    int res;

    if (!out_stat)
    {
        LOGE(TAG, "%s: line:%d fail\r\n", __func__, __LINE__);
        return -1;
    }

    out_stat->is_connected = 0;

    memset(out, 0, sizeof(out));

    res = at_send_wait_reply(at_dev_fd, CURRENTAPINFO, strlen(CURRENTAPINFO), true,
                             NULL, 0, out, sizeof(out), NULL);

    if (res == 0)
    {
        LOGI(TAG, "AT command %s succeed, rsp: %s", CURRENTAPINFO, out);
    }
    else
    {
        LOGE(TAG, "AT command %s failed\r\n", CURRENTAPINFO);
        return -1;
    }

    if (strstr(out, AT_RSP_FAIL) || !strstr(out, CURRENTAPINFOPREFIX)) {
        LOGE(TAG, "Command  %s executed with ERROR", CURRENTAPINFO);
        return -1;
    }

    out_stat->is_connected = 1;

    char tmp_ssid[128];
    char tmp_bssid[16];

    sscanf(out, "%*[^:]:%[^,],%[^,],%d,%d",
           tmp_ssid, tmp_bssid, &out_stat->channel, &out_stat->wifi_strength);

    strncpy(out_stat->ssid, tmp_ssid, sizeof(out_stat->ssid));
    out_stat->ssid[sizeof(out_stat->ssid) - 1] = 0;

    out_stat->bssid[0] = _char2num(tmp_bssid[0])  * 16 + _char2num(tmp_bssid[1]);
    out_stat->bssid[1] = _char2num(tmp_bssid[2])  * 16 + _char2num(tmp_bssid[3]);
    out_stat->bssid[2] = _char2num(tmp_bssid[4])  * 16 + _char2num(tmp_bssid[5]);
    out_stat->bssid[3] = _char2num(tmp_bssid[6])  * 16 + _char2num(tmp_bssid[7]);
    out_stat->bssid[4] = _char2num(tmp_bssid[8])  * 16 + _char2num(tmp_bssid[9]);
    out_stat->bssid[5] = _char2num(tmp_bssid[10]) * 16 + _char2num(tmp_bssid[11]);

    return 0;

}

static char * str_get_line(char **p, char * dest)
{
    char * src = *p;
    char * old_dest = dest;

    //LOGI(TAG, "str_get_line src: %s\n", src);

    while (*src && (*src == '\r' || *src == '\n'))
    {
        src++;
    }

    while (*src && (*src != '\r' && *src != '\n'))
    {
        *dest++ = *src++;
    }

    //LOGI(TAG, "str_get_line dest: %s\n", old_dest);

    *p = src;
    return dest;
}

/*
int rltv = sscanf(tmp, "%[^,],%[^,],%d,%d,%d",
                  tmp_ssid, tmp_bssid, &en, &rlt->channel, &rlt->wifi_strength);

*/
#define COMANUM 4
static int scan_one_ap(char * ap_info,
                       char * ssid,
                       char * bssid,
                       int  * channel,
                       int  * signal_strength)
{
    /* HOME_WIFI,0C704AD8D4E4,5,11,-92 */

    if (!ap_info || !ssid || !bssid || !channel || !signal_strength)
    {
        return -1;
    }

    /*find reverse 4th , */

    int len = strlen(ap_info);
    int coma_num = 0;
    char * findcoma = ap_info + len;

    while (coma_num < COMANUM && (findcoma != ap_info)) {
        findcoma--;

        if (*findcoma == ',') {
            coma_num++;
        }
    }

    char *p = ap_info;

    /* get ssid */
    while (*p && p != findcoma) {
        if (*p == '\"') {
            *ssid++ = '\\';
            *ssid++ = '\"';
            p++;
        } else {
            *ssid++ = *p++;
        }
    }

    if (*p == ',')
    {
        p++;
    }
    else
    {
        LOGE(TAG, "%s: line:%d fail\r\n", __func__, __LINE__);
    }

    /* get bssid */
    while (*p && *p != ',')
    {
        *bssid++ = *p++;
    }

    if (*p == ',')
    {
        p++;
    }
    else
    {
        LOGE(TAG, "%s: line:%d fail\r\n", __func__, __LINE__);
    }

    int en;

    sscanf(p, "%d,%d,%d", &en, channel, signal_strength);

    return 0;
}

#define SCANALLINFO   "AT+WSCANOPT=1"
#define SCAN          "AT+WSCAN"
#define SCANPREFIX    "+WSCAN:"
static int scan_ap_list(hal_wifi_module_t *m, hal_wifi_link_stat_t *out_stat, uint8_t *ap_num)
{
    char out[2048] = {0};
    int res;

    if (!out_stat || !ap_num)
    {
        LOGE(TAG, "%s: line:%d fail\r\n", __func__, __LINE__);
        return -1;
    }

    res = at_send_wait_reply(at_dev_fd, SCANALLINFO, strlen(SCANALLINFO), true,
                             NULL, 0,
                             out, sizeof(out), NULL);

    if (res == 0)
    {
        LOGI(TAG, "AT command %s succeed, rsp: %s", SCANALLINFO, out);
    }
    else
    {
        LOGE(TAG, "AT command %s failed\r\n", SCANALLINFO);
        return -1;
    }

    if (strstr(out, AT_RSP_FAIL))
    {
        LOGE(TAG, "Command  %s executed with ERROR", SCANALLINFO);
        return -1;
    }

    res = at_send_wait_reply(at_dev_fd, SCAN, strlen(SCAN), true,
                             NULL, 0,
                             out, sizeof(out), NULL);

    if (res == 0)
    {
        LOGI(TAG, "AT command %s succeed, rsp: %s", SCAN, out);
    }
    else
    {
        LOGE(TAG, "AT command %s failed\r\n", SCAN);
        return -1;
    }

    char *p_ap_num = strstr(out, SCANPREFIX);

    if (strstr(out, AT_RSP_FAIL) || !p_ap_num)
    {
        LOGE(TAG, "Command  %s executed with ERROR", SCANPREFIX);
        return -1;
    }

    hal_wifi_link_stat_t *rlt = out_stat;
    int rlt_num;
    int scan_rlt_num = 0;

    char tmp[64];
    char *p = out;
    memset(tmp, 0, sizeof(tmp));

    sscanf(p_ap_num, "+WSCAN:%d", &scan_rlt_num);
    scan_rlt_num = scan_rlt_num > 0 ? scan_rlt_num : 0;
    scan_rlt_num = scan_rlt_num < *ap_num ? scan_rlt_num : *ap_num;

    *ap_num = scan_rlt_num;

    //LOGI(TAG, "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX scan_rlt_num = %d\n", scan_rlt_num);

    int idx = 0;

    while (idx < scan_rlt_num)
    {
        /*
        sscanf(p, "%s\r\n", tmp);
        p = p + strlen(tmp) + 2;
        */
        str_get_line(&p, tmp);

        LOGI(TAG, "%d: %s", idx, tmp);

        if (!strcmp(tmp, "OK"))
        {
            break;
        }

        if (strstr(tmp, SCANPREFIX))
        {
            /*
            sscanf(tmp, "+WSCAN:%d", &rlt_num);
            rlt_num = rlt_num < *ap_num ? rlt_num : *ap_num;
            */
        }
        else
        {
            char tmp_ssid[128];
            char tmp_bssid[16];

            /*
            int rltv = sscanf(tmp, "%[^,],%[^,],%d,%d,%d",
                              tmp_ssid, tmp_bssid, &en, &rlt->channel, &rlt->wifi_strength);
            */

            memset(tmp_ssid,  0, sizeof(tmp_ssid));
            memset(tmp_bssid, 0, sizeof(tmp_bssid));

            scan_one_ap(tmp, tmp_ssid, tmp_bssid, &rlt->channel, &rlt->wifi_strength);

            strncpy(rlt->ssid, tmp_ssid, sizeof(rlt->ssid));
            rlt->ssid[sizeof(rlt->ssid) - 1] = 0;

            rlt->bssid[0] = _char2num(tmp_bssid[0])  * 16 + _char2num(tmp_bssid[1]);
            rlt->bssid[1] = _char2num(tmp_bssid[2])  * 16 + _char2num(tmp_bssid[3]);
            rlt->bssid[2] = _char2num(tmp_bssid[4])  * 16 + _char2num(tmp_bssid[5]);
            rlt->bssid[3] = _char2num(tmp_bssid[6])  * 16 + _char2num(tmp_bssid[7]);
            rlt->bssid[4] = _char2num(tmp_bssid[8])  * 16 + _char2num(tmp_bssid[9]);
            rlt->bssid[5] = _char2num(tmp_bssid[10]) * 16 + _char2num(tmp_bssid[11]);

            //LOGI(TAG, "%s, %s, %d, %d", tmp_ssid, tmp_bssid, rlt->channel, rlt->wifi_strength);

            rlt++;
            idx++;
        }

        memset(tmp, 0, sizeof(tmp));

    }

    //LOGI(TAG, "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\n");
    LOGI(TAG, "%s", out);

    return 0;
}

static void start_scan(hal_wifi_module_t *m)
{

}

static void start_scan_adv(hal_wifi_module_t *m)
{
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
    return 0;
}

static int suspend_station(hal_wifi_module_t *m)
{
    return 0;
}

static int suspend_soft_ap(hal_wifi_module_t *m)
{

    return 0;
}

static int set_channel(hal_wifi_module_t *m, int ch)
{
    return 0;
}

static void start_monitor(hal_wifi_module_t *m)
{

}

static void stop_monitor(hal_wifi_module_t *m)
{

}

static void register_monitor_cb(hal_wifi_module_t *m, monitor_data_cb_t fn)
{

}

static void register_wlan_mgnt_monitor_cb(hal_wifi_module_t *m, monitor_data_cb_t fn)
{

}

static int wlan_send_80211_raw_frame(hal_wifi_module_t *m, uint8_t *buf, int len)
{
    return 0;
}

hal_wifi_module_t aos_wifi_module_mk3060 = {
    .base.name           = "aos_wifi_module_mk3060",
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
    .wlan_send_80211_raw_frame = wlan_send_80211_raw_frame,
    .scan_ap_list        =  scan_ap_list
};

