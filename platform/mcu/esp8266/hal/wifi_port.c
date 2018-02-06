/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <hal/base.h>
#include <hal/wifi.h>

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

static void ip4_addr_u32_to_char(char *c, struct ip_addr *addr)
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
              LOG("error %d", __LINE__);
              return;
          }
          *rp++ = inv[i];
        }
        if (len++ >= 16) {
            LOG("error %d", __LINE__);
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
    ip4_addr_u32_to_char(_ip_stat.ip, addr);
    addr = &(event->event_info.got_ip.gw);
    ip4_addr_u32_to_char(_ip_stat.gate, addr);
    addr = &(event->event_info.got_ip.mask);
    ip4_addr_u32_to_char(_ip_stat.mask, addr);

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
    strcpy(config.ssid, ssid);
    if(pass){
        strcpy(config.password, pass);
    }
    if(!wifi_station_set_config(&config)){
        printf("Failed to set Station config!\n");
        return false;
    }

    if(!wifi_station_dhcpc_status()){
        printf("DHCP is not started. Starting it...\n");
        if(!wifi_station_dhcpc_start()){
            printf("DHCP start failed!\n");
            return false;
        }
    }
    return wifi_station_connect();
}

bool ICACHE_FLASH_ATTR stop_wifi_station(){
    WIFI_MODE mode = wifi_get_opmode();
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
    printf("Wifi disconnected (reason: %d)\n", reason);
}

static int wifi_init(hal_wifi_module_t *m)
{
    static int inited;

    if (inited) {
        printf("Wifi already started.\r\n");
        return 0;
    }

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
    printf("wifi_get_mac_addr!!\n");
};

static int wifi_start(hal_wifi_module_t *m, hal_wifi_init_type_t *init_para)
{
    stop_wifi_ap();
    start_wifi_station(init_para->wifi_ssid, init_para->wifi_key);
#if 0
    wifi_set_opmode(STATION_MODE);

    // set AP parameter
    struct station_config config;
    bzero(&config, sizeof(struct station_config));
    sprintf(config.ssid, "NotStable");
    sprintf(config.password, "20058111");
    wifi_station_set_config(&config);
#endif
    return 0;
}

static int wifi_start_adv(hal_wifi_module_t *m, hal_wifi_init_type_adv_t *init_para_adv)
{
    (void)init_para_adv;
 
    return 0;
}

static int get_ip_stat(hal_wifi_module_t *m, hal_wifi_ip_stat_t *out_net_para, hal_wifi_type_t wifi_type)
{
    memcpy(out_net_para, &_ip_stat, sizeof(_ip_stat));
    return 0;
}

static int get_link_stat(hal_wifi_module_t *m, hal_wifi_link_stat_t *out_stat)
{
    //out_stat->is_connected = 1;
    return 0;
}

static void start_scan(hal_wifi_module_t *m)
{
#if 0
    hal_wifi_scan_result_t scan_ret;
    ap_list_t aplist = {{0}, 0};
    scan_ret.ap_list = (ap_list_t *)&aplist;
    scan_ret.ap_num = 1;
    scan_ret.ap_list->ap_power = 80;
    memcpy(scan_ret.ap_list->ssid, "test", 5);

    if (m->ev_cb->scan_compeleted != NULL) {
        m->ev_cb->scan_compeleted(m, &scan_ret, NULL);
    }
#endif
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
    .register_monitor_cb =  register_monitor_cb,
    .register_wlan_mgnt_monitor_cb = register_wlan_mgnt_monitor_cb,
    .wlan_send_80211_raw_frame = wlan_send_80211_raw_frame
};

