/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <hal/base.h>
#include <hal/wifi.h>
#include "lega_rhino.h"
#include "lega_wlan_api.h"
#include "lega_wlan_api_aos.h"
#include <k_api.h>

hal_wifi_module_t sim_aos_wifi_lega;
monitor_data_cb_t aos_monitor_cb        = NULL;
monitor_data_cb_t aos_mgmt_monitor_cb   = NULL;

static int wifi_init(hal_wifi_module_t *m)
{
    return lega_wlan_init();
}

static void wifi_get_mac_addr(hal_wifi_module_t *m, uint8_t *mac)
{
    lega_wlan_get_mac_address(mac);
}

static void wifi_set_mac_addr(hal_wifi_module_t *m, const uint8_t *mac)
{
    lega_wlan_set_mac_address(mac);
}

extern uint8_t user_pmk[32];
static int wifi_start(hal_wifi_module_t *m, hal_wifi_init_type_t *init_para)
{
    int ret;
    lega_wlan_init_type_t conf;
    memset(&conf,0,sizeof(lega_wlan_init_type_t));
    if (init_para->wifi_mode == STATION)
    {
        memset(user_pmk, 0, sizeof(user_pmk));
    }

    conf.dhcp_mode = init_para->dhcp_mode;
    conf.wifi_mode = init_para->wifi_mode;
    memcpy(conf.wifi_ssid, init_para->wifi_ssid, 32);
    memcpy(conf.wifi_key, init_para->wifi_key, 64);
    memcpy(conf.local_ip_addr, init_para->local_ip_addr, 16);
    memcpy(conf.net_mask, init_para->net_mask, 16);
    memcpy(conf.gateway_ip_addr, init_para->gateway_ip_addr, 16);
    memcpy(conf.dns_server_ip_addr, init_para->dns_server_ip_addr, 16);
    memcpy(conf.reserved, init_para->reserved, 32);
    conf.wifi_retry_interval = init_para->wifi_retry_interval;
    ret = lega_wlan_open(&conf);

    return ret;
}

static int wifi_start_ap(hal_wifi_module_t *m, const char *ssid, const char *passwd, int interval, int hide)
{
    int ret;
    lega_wlan_ap_init_t conf;
    memset(&conf, 0, sizeof(lega_wlan_ap_init_t));
    printf("wifi_start_ap[%s %s %d]", ssid,passwd,strlen(passwd));
    memcpy(conf.ssid, ssid, strlen(ssid));
    memcpy(conf.pwd, passwd, strlen(passwd));
    conf.interval = interval;
    conf.hide = hide;

    ret = lega_wlan_ap_open(&conf);

    return ret;
}

static int wifi_stop_ap(hal_wifi_module_t *m)
{
    return lega_wlan_close();
}

static int wifi_start_adv(hal_wifi_module_t *m, hal_wifi_init_type_adv_t *init_para_adv)
{
    int ret = -1;
    lega_wlan_init_type_t conf;
    memset(&conf, 0, sizeof(lega_wlan_init_type_t));
    conf.wifi_mode = STATION;
    memcpy(conf.wifi_ssid, init_para_adv->ap_info.ssid, 32);
    memcpy(conf.mac_addr, init_para_adv->ap_info.bssid, 6);
    conf.channel = init_para_adv->ap_info.channel;
    conf.security = init_para_adv->ap_info.security;
    memcpy(conf.wifi_key, init_para_adv->key, 64);

    // Consider that this API must have PMK, TBD
    memcpy(user_pmk, init_para_adv->key, sizeof(user_pmk));

    conf.dhcp_mode = init_para_adv->dhcp_mode;
    memcpy(conf.local_ip_addr, init_para_adv->local_ip_addr, 16);
    memcpy(conf.net_mask, init_para_adv->net_mask, 16);
    memcpy(conf.gateway_ip_addr, init_para_adv->gateway_ip_addr, 16);
    memcpy(conf.dns_server_ip_addr, init_para_adv->dns_server_ip_addr, 16);
    memcpy(conf.reserved, init_para_adv->reserved, 32);
    conf.wifi_retry_interval = init_para_adv->wifi_retry_interval;

    ret = lega_wlan_open(&conf);

    return ret;
}

static int get_ip_stat(hal_wifi_module_t *m, hal_wifi_ip_stat_t *out_net_para, hal_wifi_type_t wifi_type)
{
    int ret;
    lega_wlan_ip_stat_t *in_status;

    if (wifi_type == SOFT_AP)
    {
        printf("%s SOFT_AP not implemeted yet!\r\n", __func__);
        return -1;
    }

    ret = lega_wlan_get_mac_address_inchar(out_net_para->mac);
    if (ret != 0)
    {
        printf("%s get mac addr failed!\r\n", __func__);
        return -1;
    }

    in_status = lega_wlan_get_ip_status();
    if (NULL == in_status)
    {
        printf("%s get ip status failed!\r\n", __func__);
        return -1;
    }

    out_net_para->dhcp   = in_status->dhcp;
    memcpy(out_net_para->ip, in_status->ip, sizeof(out_net_para->ip));
    memcpy(out_net_para->gate, in_status->gate, sizeof(out_net_para->gate));
    memcpy(out_net_para->mask, in_status->mask, sizeof(out_net_para->mask));
    memcpy(out_net_para->dns, in_status->dns, sizeof(out_net_para->dns));
    memcpy(out_net_para->broadcastip, in_status->ip, sizeof(out_net_para->broadcastip));

    return ret;
}

static int get_link_stat(hal_wifi_module_t *m, hal_wifi_link_stat_t *out_stat)
{
    int ret;
    lega_wlan_link_stat_t status;

    ret = lega_wlan_get_link_status(&status);
    if (0 == ret)
    {
        out_stat->is_connected  = status.is_connected;
        out_stat->wifi_strength = status.wifi_strength;
        out_stat->channel       = status.channel;
        memcpy(out_stat->ssid, status.ssid, 32);
        memcpy(out_stat->bssid, status.bssid, 6);
    }
    return ret;
}

static void start_scan(hal_wifi_module_t *m)
{
    lega_wlan_start_scan();
}

static void start_scan_adv(hal_wifi_module_t *m)
{
    lega_wlan_start_scan_adv();
}

static int power_off(hal_wifi_module_t *m)
{
    printf("WiFi HAL %s not implemeted yet!\r\n", __func__);
    return 0;
}

static int power_on(hal_wifi_module_t *m)
{
    printf("WiFi HAL %s not implemeted yet!\r\n", __func__);
    return 0;
}

static int suspend(hal_wifi_module_t *m)
{
    return lega_wlan_suspend();
}

static int suspend_station(hal_wifi_module_t *m)
{
    return lega_wlan_suspend_sta();
}

static int suspend_soft_ap(hal_wifi_module_t *m)
{

    return lega_wlan_suspend_ap();
}

static int set_channel(hal_wifi_module_t *m, int ch)
{
    return lega_wlan_monitor_set_channel(ch);
}

static void wlan_monitor_cb(uint8_t*data, int len)
{
    hal_wifi_link_info_t info;
    //rssi TBD
    info.rssi = -10;
    if (aos_monitor_cb) {
        aos_monitor_cb(data, len, &info);
    }
}

static void wlan_mgmt_monitor_cb(uint8_t*data, int len)
{
    hal_wifi_link_info_t info;
    //rssi TBD
    info.rssi = -10;
    if (aos_mgmt_monitor_cb) {
        aos_mgmt_monitor_cb(data, len, &info);
    }
}

static void start_monitor(hal_wifi_module_t *m)
{
    lega_wlan_start_monitor();
    lega_wlan_register_monitor_cb(wlan_monitor_cb);
}

static void stop_monitor(hal_wifi_module_t *m)
{
    lega_wlan_stop_monitor();
    lega_wlan_register_monitor_cb(NULL);
}

static void register_monitor_cb(hal_wifi_module_t *m, monitor_data_cb_t fn)
{
    aos_monitor_cb = fn;
}

static void register_wlan_mgnt_monitor_cb(hal_wifi_module_t *m, monitor_data_cb_t fn)
{
    aos_mgmt_monitor_cb = fn;
    lega_wlan_register_mgmt_monitor_cb(wlan_mgmt_monitor_cb);
}

static int wlan_send_80211_raw_frame(hal_wifi_module_t *m, uint8_t *buf, int len)
{
    return lega_wlan_send_raw_frame(buf, len);
}

void scan_done(lega_wlan_scan_result_t *p_scan_results)
{
    if (!p_scan_results->is_scan_adv)
    {
        hal_wifi_scan_result_t out_scan_results;
        memset(&out_scan_results, 0, sizeof(hal_wifi_scan_result_t));
        if (p_scan_results->ap_num != 0)
        {
            out_scan_results.ap_num  = p_scan_results->ap_num;
            out_scan_results.ap_list = lega_rtos_malloc(sizeof(ap_list_t)*out_scan_results.ap_num);
            for (int i=0; i<out_scan_results.ap_num; i++)
            {
                out_scan_results.ap_list[i].ap_power = p_scan_results->ap_list[i].ap_power;
                memcpy(out_scan_results.ap_list[i].ssid, p_scan_results->ap_list[i].ssid, 32);
            }
        }
        sim_aos_wifi_lega.ev_cb->scan_compeleted(&sim_aos_wifi_lega,
                &out_scan_results, NULL);
        if (out_scan_results.ap_list != NULL)
            lega_rtos_free(out_scan_results.ap_list);
    }
    else
    {
        hal_wifi_scan_result_adv_t out_scan_results;
        memset(&out_scan_results, 0, sizeof(hal_wifi_scan_result_adv_t));
        if (p_scan_results->ap_num != 0)
        {
            out_scan_results.ap_num  = p_scan_results->ap_num;
            out_scan_results.ap_list = lega_rtos_malloc(sizeof(ap_list_adv_t)*out_scan_results.ap_num);
            for (int i=0; i<out_scan_results.ap_num; i++)
            {
                out_scan_results.ap_list[i].ap_power = p_scan_results->ap_list[i].ap_power;
                out_scan_results.ap_list[i].channel = p_scan_results->ap_list[i].channel;
                out_scan_results.ap_list[i].security = p_scan_results->ap_list[i].security;
                memcpy(out_scan_results.ap_list[i].ssid, p_scan_results->ap_list[i].ssid, 32);
                memcpy(out_scan_results.ap_list[i].bssid, p_scan_results->ap_list[i].bssid, 6);
            }
        }
        sim_aos_wifi_lega.ev_cb->scan_adv_compeleted(&sim_aos_wifi_lega,
                &out_scan_results, NULL);
        if (out_scan_results.ap_list != NULL)
            lega_rtos_free(out_scan_results.ap_list);
    }
}

void wifi_event_cb(lega_wlan_event_e evt, void* info)
{
    if (sim_aos_wifi_lega.ev_cb == NULL)
        return;

    switch (evt)
    {
        case WLAN_EVENT_SCAN_COMPLETED:
            if (sim_aos_wifi_lega.ev_cb->scan_compeleted == NULL)
                return;
            lega_wlan_scan_result_t *p_in_scan_results = (lega_wlan_scan_result_t*)info;
            scan_done(p_in_scan_results);
            break;
        case WLAN_EVENT_ASSOCIATED:
            if (sim_aos_wifi_lega.ev_cb->para_chg == NULL)
                return;
            hal_wifi_ap_info_adv_t  out_ap_info;
            lega_wlan_ap_info_adv_t *p_in_ap_info;
            memset(&out_ap_info, 0, sizeof(hal_wifi_ap_info_adv_t));
            p_in_ap_info = lega_wlan_get_associated_apinfo();
            if (NULL != p_in_ap_info)
            {
                memcpy(out_ap_info.ssid, p_in_ap_info->ssid, p_in_ap_info->ssid_len);
                memcpy(out_ap_info.bssid, p_in_ap_info->bssid, 6);
                out_ap_info.channel     = p_in_ap_info->channel;
                out_ap_info.security    = p_in_ap_info->security;
                sim_aos_wifi_lega.ev_cb->para_chg(&sim_aos_wifi_lega, &out_ap_info, p_in_ap_info->pwd, p_in_ap_info->pwd_len, NULL);
            }
            break;
        case WLAN_EVENT_CONNECTED:
            if (sim_aos_wifi_lega.ev_cb->stat_chg == NULL)
                return;

            sim_aos_wifi_lega.ev_cb->stat_chg(&sim_aos_wifi_lega, NOTIFY_STATION_UP, NULL);
            break;
        case WLAN_EVENT_IP_GOT:
            if (sim_aos_wifi_lega.ev_cb->ip_got == NULL)
                return;
            hal_wifi_ip_stat_t ip_stat;
            lega_wlan_ip_stat_t *p_in_ip_stat = (lega_wlan_ip_stat_t *)info;
            ip_stat.dhcp = p_in_ip_stat->dhcp;
            memcpy(ip_stat.ip, p_in_ip_stat->ip, 16);
            memcpy(ip_stat.gate, p_in_ip_stat->gate, 16);
            memcpy(ip_stat.mask, p_in_ip_stat->mask, 16);
            memcpy(ip_stat.dns, p_in_ip_stat->dns, 16);
            memcpy(ip_stat.broadcastip, p_in_ip_stat->broadcastip, 16);
            lega_wlan_get_mac_address_inchar(ip_stat.mac);
            sim_aos_wifi_lega.ev_cb->ip_got(&sim_aos_wifi_lega, &ip_stat, NULL);
#if (WIFI_CONFIG_SUPPORT_LOWPOWER > 0)
            /* if config WIFI_CONFIG_SUPPORT_LOWPOWER enable wifi power save
             * default */
            lega_wlan_set_ps_mode(1);
#endif
            break;
        case WLAN_EVENT_DISCONNECTED:
            if (sim_aos_wifi_lega.ev_cb->stat_chg == NULL)
                return;

            sim_aos_wifi_lega.ev_cb->stat_chg(&sim_aos_wifi_lega, NOTIFY_STATION_DOWN, NULL);
            break;
        case WLAN_EVENT_AP_UP:
            if (sim_aos_wifi_lega.ev_cb->stat_chg == NULL)
                return;

            sim_aos_wifi_lega.ev_cb->stat_chg(&sim_aos_wifi_lega, NOTIFY_AP_UP, NULL);
            break;
        case WLAN_EVENT_AP_DOWN:
            if (sim_aos_wifi_lega.ev_cb->stat_chg == NULL)
                return;

            sim_aos_wifi_lega.ev_cb->stat_chg(&sim_aos_wifi_lega, NOTIFY_AP_DOWN, NULL);
            break;
        default:
            printf("WiFi HAL %s EVENT[%d] not implemeted yet!\r\n", __func__, evt);
            break;
    }
}

void start_debug_mode(hal_wifi_module_t *m)
{
    lega_wlan_start_debug_mode();
}

void stop_debug_mode(hal_wifi_module_t *m)
{
    lega_wlan_stop_debug_mode();
}

#if (WIFI_CONFIG_SUPPORT_LOWPOWER > 0)
static int set_listeninterval(hal_wifi_module_t *m, uint8_t listen_interval)
{
    int status = -1;

    if ((listen_interval != 1) && (listen_interval != 3) && (listen_interval != 10)) {
        printf("err: listen interval can only to be set:1 3 10\n");
        return -1;
    }

    status = lega_wlan_set_ps_options(WIFI_CONFIG_RECEIVE_DTIM, listen_interval);
    if (status != 0) {
        return -1;
    }
    return 0;
}

static int enter_powersave(hal_wifi_module_t *m, uint8_t recvDTIMs)
{
    int status = -1;
    printf("enter_powersave\n");

    status = lega_wlan_set_ps_mode(1);
    if (status != 0) {
        return -1;
    }

    return 0;
}

static int exit_powersave(hal_wifi_module_t *m)
{
    int status = -1;
    printf("exit_powersave\n");
    status = lega_wlan_set_ps_mode(0);
    if (status == 0) {
        return 0;
    } else {
        return -1;
    }
}
#endif

hal_wifi_module_t sim_aos_wifi_lega = {
    .base.name           = "sim_aos_wifi_lega",
    .init                =  wifi_init,
    .get_mac_addr        =  wifi_get_mac_addr,
    .set_mac_addr        =  wifi_set_mac_addr,
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
    .start_ap = wifi_start_ap,
    .stop_ap = wifi_stop_ap,
    .start_debug_mode = start_debug_mode,
    .stop_debug_mode = stop_debug_mode,
#if (WIFI_CONFIG_SUPPORT_LOWPOWER > 0)
    .set_listeninterval =  set_listeninterval,
    .enter_powersave    =  enter_powersave,
    .exit_powersave     =  exit_powersave,
#endif
};

