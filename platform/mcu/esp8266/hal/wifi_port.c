/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "network/hal/base.h"
#include <hal/wifi.h>

#include "ulog/ulog.h"

#include <stddef.h>
#include "espressif/c_types.h"
#include "lwipopts.h"
#include "lwip/ip_addr.h"
#include "espressif/esp_libc.h"
#include "espressif/esp_misc.h"
#include "espressif/esp_common.h"
#include "espressif/esp_wifi.h"
#include "espressif/esp_sta.h"
#include "espressif/esp_softap.h"

#include <stddef.h>
#include "espressif/c_types.h"
#include "espressif/esp_wifi.h"

typedef void (* wifi_state_cb_t)();
typedef void (* wifi_state_cb_t)();
typedef void (* wifi_disco_cb_t)(uint8_t reason);

void set_on_station_first_connect(wifi_state_cb_t cb);
void set_on_station_connect(wifi_state_cb_t cb);
void set_on_station_disconnect(wifi_disco_cb_t cb);
void set_on_client_connect(wifi_state_cb_t cb);
void set_on_client_disconnect(wifi_state_cb_t cb);

WIFI_MODE init_esp_wifi();
bool start_wifi_station(const char * ssid, const char * pass);
bool stop_wifi_station();
bool start_wifi_ap(const char * ssid, const char * pass);
bool stop_wifi_ap();

bool wifi_station_connected();
bool wifi_ap_enabled();


wifi_state_cb_t on_station_first_connect = NULL;
wifi_state_cb_t on_station_connect = NULL;
wifi_disco_cb_t on_station_disconnect = NULL;

wifi_state_cb_t on_client_connect = NULL;
wifi_state_cb_t on_client_disconnect = NULL;

volatile bool wifi_station_static_ip = false;
volatile bool wifi_station_is_connected = false;

static hal_wifi_ip_stat_t _ip_stat;

static monitor_data_cb_t data_cb = NULL;
static monitor_data_cb_t mngt_data_cb = NULL;

static void ip4_addr_uint32_t_to_char(char *c, struct ip_addr *addr)
{
    uint32_t s_addr;
    char inv[3];
    char *rp;
    uint8_t *ap;
    uint8_t rem;
    uint8_t n;
    uint8_t i;
    int len = 0;

    s_addr = addr->addr;

    rp = c;
    ap = (uint8_t *)&s_addr;
    for(n = 0; n < 4; n++) {
        i = 0;
        do {
            rem = *ap % (uint8_t)10;
            *ap /= (uint8_t)10;
            inv[i++] = '0' + rem;
        } while(*ap);
        while(i--) {
          if (len++ >= 16) {
              printf("error %d", __LINE__);
              return;
          }
          *rp++ = inv[i];
        }
        if (len++ >= 16) {
            printf("error %d", __LINE__);
            return;
        }
        *rp++ = '.';
        ap++;
    }
    *--rp = 0;
}

static void notify_got_ip(System_Event_t *event)
{
    hal_wifi_module_t *m = hal_wifi_get_default_module();
    struct ip_addr *addr;

    memset(&_ip_stat, 0, sizeof(_ip_stat));
    addr = &(event->event_info.got_ip.ip);
    ip4_addr_uint32_t_to_char(_ip_stat.ip, addr);
    addr = &(event->event_info.got_ip.gw);
    ip4_addr_uint32_t_to_char(_ip_stat.gate, addr);
    addr = &(event->event_info.got_ip.mask);
    ip4_addr_uint32_t_to_char(_ip_stat.mask, addr);

    if (m->ev_cb && m->ev_cb->ip_got) {
        m->ev_cb->ip_got(m, &_ip_stat, NULL);
    }
}

void ICACHE_FLASH_ATTR wifi_event_handler_cb(System_Event_t *event)
{
    static bool station_was_connected = false;
    if (event == NULL) {
        return;
    }

    printf("[WiFi] event %u\n", event->event_id);

    switch (event->event_id) {
        case EVENT_STAMODE_DISCONNECTED:
            wifi_station_is_connected = false;
            Event_StaMode_Disconnected_t *ev = (Event_StaMode_Disconnected_t *)&event->event_info;
            if(on_station_disconnect){
                on_station_disconnect(ev->reason);
            }
            break;
        case EVENT_STAMODE_CONNECTED:
            if(wifi_station_static_ip){
                wifi_station_is_connected = true;
                if(!station_was_connected){
                    station_was_connected = true;
                    if(on_station_first_connect){
                        on_station_first_connect();
                    }
                }
                if(on_station_connect){
                    on_station_connect();
                }
            }
            break;
        case EVENT_STAMODE_DHCP_TIMEOUT:
            if(wifi_station_is_connected){
                wifi_station_is_connected = false;
                if(on_station_disconnect){
                    on_station_disconnect(REASON_UNSPECIFIED);
                }
            }
            break;
        case EVENT_STAMODE_GOT_IP:
            wifi_station_is_connected = true;
            if(!station_was_connected){
                station_was_connected = true;
                if(on_station_first_connect){
                    on_station_first_connect();
                }
            }
            notify_got_ip(event);
            if(on_station_connect){
                on_station_connect();
            }
            break;

        case EVENT_SOFTAPMODE_STACONNECTED:
            if(on_client_connect){
                on_client_connect();
            }
            break;
        case EVENT_SOFTAPMODE_STADISCONNECTED:
            if(on_client_disconnect){
                on_client_disconnect();
            }
            break;
        default:
            break;
    }
}

void ICACHE_FLASH_ATTR set_on_station_first_connect(wifi_state_cb_t cb){
    on_station_first_connect = cb;
}

void ICACHE_FLASH_ATTR set_on_station_connect(wifi_state_cb_t cb){
    on_station_connect = cb;
}

void ICACHE_FLASH_ATTR set_on_station_disconnect(wifi_disco_cb_t cb){
    on_station_disconnect = cb;
}

void ICACHE_FLASH_ATTR set_on_client_connect(wifi_state_cb_t cb){
    on_client_connect = cb;
}

void ICACHE_FLASH_ATTR set_on_client_disconnect(wifi_state_cb_t cb){
    on_client_disconnect = cb;
}

bool ICACHE_FLASH_ATTR wifi_set_mode(WIFI_MODE mode){
    if(!mode){
        printf("enter modem sleep\r\n");
        bool s = wifi_set_opmode(mode);
        wifi_fpm_open();
        wifi_fpm_set_sleep_type(MODEM_SLEEP_T);
        wifi_fpm_do_sleep(0xFFFFFFFF);
        return s;
    }
    wifi_fpm_close();
    return wifi_set_opmode(mode);
}

WIFI_MODE ICACHE_FLASH_ATTR init_esp_wifi(){
    wifi_set_event_handler_cb(wifi_event_handler_cb);
    WIFI_MODE mode = wifi_get_opmode_default();
    wifi_set_mode(mode);
    return mode;
}

bool ICACHE_FLASH_ATTR start_wifi_station(const char * ssid, const char * pass){
    WIFI_MODE mode = wifi_get_opmode();

    wifi_station_disconnect();

    if((mode & STATION_MODE) == 0){
        mode |= STATION_MODE;
        if(!wifi_set_mode(mode)){
            printf("Failed to enable Station mode!\n");
            return false;
        }
    }
    if(!ssid){
        printf("No SSID Given. Will connect to the station saved in flash\n");
        return true;
    }
    struct station_config config;
    memset(&config, 0, sizeof(struct station_config));
    strncpy(config.ssid, ssid, sizeof(config.ssid) - 1);
    if(pass){
        strncpy(config.password, pass, sizeof(config.password) - 1);
    }
    if(!wifi_station_set_config(&config)){
        printf("Failed to set Station config!\n");
        return false;
    }
    wifi_set_phy_mode(PHY_MODE_11G);
    
    if(!wifi_station_dhcpc_status()){
        printf("DHCP is not started. Starting it...\n");
        if(!wifi_station_dhcpc_start()){
            printf("DHCP start failed!\n");
            return false;
        }
    }
    printf("[%s]line:[%d] start to connect!\n", __func__, __LINE__);
    return wifi_station_connect();
}

bool ICACHE_FLASH_ATTR stop_wifi_station(){
    WIFI_MODE mode = wifi_get_opmode();
    wifi_station_disconnect();
    mode &= ~STATION_MODE;
    if(!wifi_set_mode(mode)){
        printf("Failed to disable Station mode!\n");
        return false;
    }
    return true;
}

bool ICACHE_FLASH_ATTR start_wifi_ap(const char * ssid, const char * pass){
    WIFI_MODE mode = wifi_get_opmode();
    if((mode & SOFTAP_MODE) == 0){
        mode |= SOFTAP_MODE;
        if(!wifi_set_mode(mode)){
            printf("Failed to enable AP mode!\n");
            return false;
        }
    }
    if(!ssid){
        printf("No SSID Given. Will start the AP saved in flash\n");
        return true;
    }
    struct softap_config config;
    bzero(&config, sizeof(struct softap_config));
    sprintf(config.ssid, ssid);
    if(pass){
        sprintf(config.password, pass);
    }
    return wifi_softap_set_config(&config);
}

bool ICACHE_FLASH_ATTR stop_wifi_ap(){
    WIFI_MODE mode = wifi_get_opmode();
    mode &= ~SOFTAP_MODE;
    if(!wifi_set_mode(mode)){
        printf("Failed to disable AP mode!\n");
        return false;
    }
    return true;
}

bool ICACHE_FLASH_ATTR wifi_station_connected(){
    if(!wifi_station_is_connected){
        return false;
    }
    WIFI_MODE mode = wifi_get_opmode();
    if((mode & STATION_MODE) == 0){
        return false;
    }
    STATION_STATUS wifistate = wifi_station_get_connect_status();
    wifi_station_is_connected = (wifistate == STATION_GOT_IP || (wifi_station_static_ip && wifistate == STATION_CONNECTING));
    return wifi_station_is_connected;
}

bool ICACHE_FLASH_ATTR wifi_ap_enabled(){
    return !!(wifi_get_opmode() & SOFTAP_MODE);
}

LOCAL void ICACHE_FLASH_ATTR on_wifi_connect(){
    printf("Wifi connected.\n");
}

LOCAL void ICACHE_FLASH_ATTR on_wifi_disconnect(uint8_t reason){
    hal_wifi_module_t *m = hal_wifi_get_default_module();

    printf("Wifi disconnected (reason: %d)\n", reason);
    if (m->ev_cb && m->ev_cb->stat_chg) {
        m->ev_cb->stat_chg(m, NOTIFY_STATION_DOWN, NULL);
    }

}

static int wifi_init(hal_wifi_module_t *m)
{
    static int inited;

    if (inited) {
        printf("Wifi already started.\r\n");
        return 0;
    }

    wifi_station_set_auto_connect(false);
    set_on_station_connect(on_wifi_connect);
    set_on_station_disconnect(on_wifi_disconnect);
    init_esp_wifi();

    inited = 1;
    printf("Wifi init success!!\n");

    return 0;
};

static void wifi_get_mac_addr(hal_wifi_module_t *m, uint8_t *mac)
{
    (void)m;
    if (!mac) {printf("%s invalid argument.", __func__); return;}
    wifi_get_macaddr(STATION_IF, mac);
};

static int wifi_start(hal_wifi_module_t *m, hal_wifi_init_type_t *init_para)
{
    stop_wifi_ap();
    start_wifi_station(init_para->wifi_ssid, init_para->wifi_key);
    return 0;
}

static int wifi_start_adv(hal_wifi_module_t *m, hal_wifi_init_type_adv_t *init_para_adv)
{
    (void)init_para_adv;

    return 0;
}

static int get_ip_stat(hal_wifi_module_t *m, hal_wifi_ip_stat_t *out_net_para, hal_wifi_type_t wifi_type)
{
    if (wifi_station_connected() == false) {
        printf("WiFi station is not connected yet.\r\n");
        return -1;
    }
    memcpy(out_net_para, &_ip_stat, sizeof(_ip_stat));
    return 0;
}

static int get_link_stat(hal_wifi_module_t *m, hal_wifi_link_stat_t *out_stat)
{
    return 0;
}

static uint16_t get_scan_ap_num(struct bss_info *info)
{
    int ap_num = 0;

    if (info == NULL)
        return 0;

    for (; info->next.stqe_next; info = info->next.stqe_next) {
        ap_num++;
    }

    return ap_num;
}

typedef enum {
    SCAN_NORMAL,
    SCAN_ADV
} scan_type_t;

#define MAX_SCAN_AP 20

static void scan_cb_helper(hal_wifi_module_t *m, struct bss_info *info, scan_type_t t)
{
    uint16_t ap_num = 16;
    int i;
    hal_wifi_scan_result_t *result = NULL;
    hal_wifi_scan_result_adv_t *result_adv = NULL;

    if (t != SCAN_NORMAL && t != SCAN_ADV) return;

    ap_num = get_scan_ap_num(info);
    if (!ap_num) return;
    if (ap_num > MAX_SCAN_AP)
        ap_num = MAX_SCAN_AP;

    if (t == SCAN_NORMAL) {
        result = malloc(sizeof(hal_wifi_scan_result_t));
        if (!result) goto end;
        result->ap_num = ap_num;
        result->ap_list = malloc(ap_num * sizeof(ap_list_t));
        if (!result->ap_list) goto end;
    } else {
        result_adv = malloc(sizeof(hal_wifi_scan_result_adv_t));
        if (!result_adv) goto end;
        result_adv->ap_num = ap_num;
        result_adv->ap_list = malloc(ap_num * sizeof(ap_list_adv_t));
        if (!result_adv->ap_list) goto end;
    }

    for (i = 0; info->next.stqe_next && i < ap_num; info = info->next.stqe_next, i++) {
        LOGD("esp8266", "BSSID %02x:%02x:%02x:%02x:%02x:%02x channel %02d rssi %02d auth %02d %s\n",
            MAC2STR(info->bssid),
            info->channel,
            info->rssi,
            info->authmode,
            info->ssid
        );

        if (t == SCAN_NORMAL) {
            ap_list_t *res = result->ap_list + i;
            memcpy(res->ssid, info->ssid, sizeof(res->ssid));
            res->ap_power = info->rssi;
        } else {
            ap_list_adv_t *res_adv = result_adv->ap_list + i;
            memcpy(res_adv->ssid, info->ssid, sizeof(res_adv->ssid));
            res_adv->ap_power = info->rssi;
            memcpy(res_adv->bssid, info->bssid, sizeof(res_adv->bssid));
            res_adv->channel = info->channel;
            res_adv->security = info->authmode;
        }
    }

    if (t == SCAN_NORMAL) {
        if (m->ev_cb && m->ev_cb->scan_compeleted) {
            m->ev_cb->scan_compeleted(m, result, NULL);
        }
    } else {
        if (m->ev_cb && m->ev_cb->scan_adv_compeleted) {
            m->ev_cb->scan_adv_compeleted(m, result_adv, NULL);
        }
    }

end:
    if (result) {
        if (result->ap_list)
            free(result->ap_list);
        free(result);
    }
    if (result_adv) {
        if (result_adv->ap_list)
            free(result_adv->ap_list);
        free(result_adv);
    }
}

static void scan_cb(void *arg, STATUS status)
{
    struct bss_info *info = arg;
    hal_wifi_module_t *m;

    if (info == NULL)
        return;

    m = hal_wifi_get_default_module();

    scan_cb_helper(m, info, SCAN_NORMAL);
}

static void scan_cb_adv(void *arg, STATUS status)
{
    struct bss_info *info = arg;
    hal_wifi_module_t *m;

    if (info == NULL)
        return;

    m = hal_wifi_get_default_module();

    scan_cb_helper(m, info, SCAN_ADV);
}

static void start_scan(hal_wifi_module_t *m)
{
    wifi_station_scan(NULL, scan_cb);
}

static void start_scan_adv(hal_wifi_module_t *m)
{
    wifi_station_scan(NULL, scan_cb_adv);
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
    WIFI_MODE mode = wifi_get_opmode();
    if((mode & STATION_MODE) == 0){
        stop_wifi_station();
    } else {
        stop_wifi_ap();
    }

    return 0;
}

static int suspend_station(hal_wifi_module_t *m)
{
    wifi_station_disconnect();
    return 0;
}

static int suspend_soft_ap(hal_wifi_module_t *m)
{
    stop_wifi_ap();
    return 0;
}

static int set_channel(hal_wifi_module_t *m, int ch)
{
    wifi_set_channel(ch);
    return 0;
}

struct RxControl {
    signed rssi:8;
    unsigned rate:4;
    unsigned is_group:1;
    unsigned:1;
    unsigned sig_mode:2;
    unsigned legacy_length:12;
    unsigned damatch0:1;
    unsigned damatch1:1;
    unsigned bssidmatch0:1;
    unsigned bssidmatch1:1;
    unsigned MCS:7;
    unsigned CWB:1;
    unsigned HT_length:16;
    unsigned Smoothing:1;
    unsigned Not_Sounding:1;
    unsigned:1;
    unsigned Aggregation:1;
    unsigned STBC:2;
    unsigned FEC_CODING:1;
    unsigned SGI:1;
    unsigned rxend_state:8;
    unsigned ampdu_cnt:8;
    unsigned channel:4;
    unsigned:12;
};

struct Ampdu_Info
{
    uint16_t length;
    uint16_t seq;
    uint8_t  address3[6];
};

struct sniffer_buf {
    struct RxControl rx_ctrl;        //12Byte
    uint8_t  buf[36];
    uint16_t cnt;
    struct Ampdu_Info ampdu_info[1];
};

struct sniffer_buf2{
    struct RxControl rx_ctrl;
    //uint8_t buf[112];
    uint8_t buf[496];
    uint16_t cnt;
    uint16_t len; //length of packet
};

#ifndef ETH_ALEN
#define ETH_ALEN 6
#endif

struct ieee80211_hdr {
    uint16_t frame_control;
    uint16_t duration_id;
    uint8_t addr1[ETH_ALEN];
    uint8_t addr2[ETH_ALEN];
    uint8_t addr3[ETH_ALEN];
    uint16_t seq_ctrl;
    uint8_t addr4[ETH_ALEN];
};

void ICACHE_FLASH_ATTR sniffer_wifi_promiscuous_rx(uint8_t *buf, uint16_t buf_len)
{
    uint8_t *data;
    uint32_t data_len;
    hal_wifi_link_info_t info = {0};

    if (buf_len == sizeof(struct sniffer_buf2)) {/* managment frame */
        struct sniffer_buf2 *sniffer = (struct sniffer_buf2 *)buf;
        info.rssi = sniffer->rx_ctrl.rssi;
        //if (!mngt_data_cb) return;
        data_len = sniffer->len;
        if (data_len > sizeof(sniffer->buf))
            data_len = sizeof(sniffer->buf);
        data = sniffer->buf;
        if(data_cb)data_cb(data, data_len, &info);
        if(mngt_data_cb)mngt_data_cb(data, data_len, &info);
    }  else if (buf_len == sizeof(struct RxControl)) {/* mimo, HT40, LDPC */
    } else {
        if (!data_cb) return;
        struct sniffer_buf *sniffer = (struct sniffer_buf *)buf;
        data = buf + sizeof(struct RxControl);
        info.rssi = sniffer->rx_ctrl.rssi;

        struct ieee80211_hdr *hdr = (struct ieee80211_hdr *)data;

        if (sniffer->cnt == 1) {
            data_len = sniffer->ampdu_info[0].length - 4;
            if(data_cb) {
                data_cb(data, data_len, &info);
            } else {
                printf("[%s]%d, sniffer callback is NULL\n", __func__, __LINE__);
            }
        } else {
            int i;
            for (i = 1; i < sniffer->cnt; i++) {
                hdr->seq_ctrl = sniffer->ampdu_info[i].seq;
                memcpy(&hdr->addr3, sniffer->ampdu_info[i].address3, 6);

                data_len = sniffer->ampdu_info[i].length - 4;
                if (data_cb) {
                    data_cb(data, data_len, &info);
                } else {
                    printf("[%s]%d, sniffer callback is NULL\n", __func__, __LINE__);
                    break;                    
                }
            }
        }
    }
}

static void start_monitor(hal_wifi_module_t *m)
{
    wifi_set_mode(STATION_MODE);
    wifi_station_disconnect();
    wifi_promiscuous_enable(0);
    wifi_set_promiscuous_rx_cb(sniffer_wifi_promiscuous_rx);
    wifi_promiscuous_enable(1);
}

static void stop_monitor(hal_wifi_module_t *m)
{
    wifi_set_mode(STATION_MODE);
    wifi_promiscuous_enable(0);
    wifi_set_promiscuous_rx_cb(NULL);
    data_cb = NULL;
}

static void register_monitor_cb(hal_wifi_module_t *m, monitor_data_cb_t fn)
{
    data_cb = fn;
}

monitor_data_cb_t   g_mgnt_filter_callback = NULL;
hal_wifi_link_info_t    g_mgnt_link_info;

static void  esp_mgmt_filter(const uint8_t *buf, int buf_len, int rssi)
{
    g_mgnt_link_info.rssi = (int8_t)rssi;
        /* only deal with Probe Request*/
    if(g_mgnt_filter_callback && buf[0] == 0x40) {
        g_mgnt_filter_callback((u8*)buf, buf_len, &g_mgnt_link_info);
     }
}

static void register_wlan_mgnt_monitor_cb(hal_wifi_module_t *m, monitor_data_cb_t fn)
{
    g_mgnt_link_info.rssi = 0;
    g_mgnt_filter_callback = fn;
    if (g_mgnt_filter_callback) {
        wifi_set_sta_rx_probe_req(esp_mgmt_filter);
    } else{
        wifi_set_sta_rx_probe_req(NULL);
    }
}

static int wlan_send_80211_raw_frame(hal_wifi_module_t *m, uint8_t *buf, int len)
{
    wifi_send_pkt_freedom(buf, len, true);
    return 0;
}

static int start_ap(hal_wifi_module_t *m, const char *ssid, const char *passwd, int interval, int hide)
{
    printf("ESP8266 start_ap ssid:%s,pwd:%s\n",ssid,passwd);

    return start_wifi_ap(ssid,passwd);
}

static int stop_ap(hal_wifi_module_t *m)
{
    printf("ESP8266 stop_ap\n");
    stop_wifi_ap();
    return 0;
}

static int get_wireless_info(hal_wifi_module_t *m, void *wireless_info)
{
    hal_wireless_info_t *info = (hal_wireless_info_t *)wireless_info;

    LOGD("get wireless info\r\n");

    if (info == NULL)
        return -1;

    info->rssi = wifi_station_get_rssi();

    return 0;
}

hal_wifi_module_t aos_wifi_esp8266 = {
    .base.name           = "aos_wifi_esp8266",
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
    .start_ap            =  start_ap,
    .stop_ap             =  stop_ap,
    .register_monitor_cb =  register_monitor_cb,
    .register_wlan_mgnt_monitor_cb = register_wlan_mgnt_monitor_cb,
    .wlan_send_80211_raw_frame = wlan_send_80211_raw_frame,

    .get_wireless_info   = get_wireless_info,
};

