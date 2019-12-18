/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <network/network.h>
#include "hal/wifi.h"
#include "aos/hal/wdg.h"
#include "esp_smartconfig.h"
#include "esp_wifi_types.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_timer.h"
#include "nvs.h"
#include "nvs_flash.h"

typedef enum {
    SCAN_NORMAL,
    SCAN_ADV
} scan_type_t;

extern esp_err_t esp_wifi_set_promiscous_autoack(bool, uint8_t *);
extern esp_err_t esp_wifi_set_auto_connect(bool c);

static uint8_t _bssid[6];

/**
    @brief Wifi scan ready
*/
static void scan_done(hal_wifi_module_t *m, scan_type_t t)
{
    uint16_t ap_num = 16;
    wifi_ap_record_t *ap_records;
    int i;
    hal_wifi_scan_result_t *result = NULL;
    hal_wifi_scan_result_adv_t *result_adv = NULL;

    if (t != SCAN_NORMAL && t != SCAN_ADV) return;

    ESP_ERROR_CHECK(esp_wifi_scan_get_ap_num(&ap_num));
    if (ap_num > 16)
        ap_num = 16;

    ap_records = malloc(ap_num * sizeof(*ap_records));
    if (!ap_records)
        return;

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

    ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(&ap_num, ap_records));

    for (i = 0; i < ap_num; i++) {
        wifi_ap_record_t *r = ap_records + i;

        if (t == SCAN_NORMAL) {
            ap_list_t *res = result->ap_list + i;
            memcpy(res->ssid, r->ssid, sizeof(res->ssid));
            res->ap_power = r->rssi;
        } else {
            ap_list_adv_t *res_adv = result_adv->ap_list + i;
            memcpy(res_adv->ssid, r->ssid, sizeof(res_adv->ssid));
            res_adv->ap_power = r->rssi;
            memcpy(res_adv->bssid, r->bssid, sizeof(res_adv->bssid));
            res_adv->channel = r->primary;
            res_adv->security = r->authmode;
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
    if (ap_records)
        free(ap_records);
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

static monitor_data_cb_t data_cb = NULL;
static monitor_data_cb_t mngt_data_cb = NULL;
static monitor_data_cb_t mesh_data_cb = NULL;

/**
    @brief Data RX Callback when in promiscuous mode
*/
static void promiscuous_rx_cb(void *buf, wifi_promiscuous_pkt_type_t type)
{
    hal_wifi_module_t *m = hal_wifi_get_default_module();
    wifi_promiscuous_pkt_t *pkt = (wifi_promiscuous_pkt_t *)buf;
    hal_wifi_link_info_t info;

    if (type != WIFI_PKT_DATA && type != WIFI_PKT_MGMT)
        return;

    info.rssi = pkt->rx_ctrl.rssi;
    if (mesh_data_cb)
        mesh_data_cb(pkt->payload, pkt->rx_ctrl.sig_len, &info);

    if (data_cb)
        data_cb(pkt->payload, pkt->rx_ctrl.sig_len - 4, &info);  // exclude wifi fcs

    if (type == WIFI_PKT_MGMT && mngt_data_cb)
        mngt_data_cb(pkt->payload, pkt->rx_ctrl.sig_len, &info);
}

static void esp_reconnect_wifi_helper(void *arg)
{
    ESP_ERROR_CHECK(esp_wifi_connect());
}

static esp_timer_handle_t handle = NULL;
static void esp_reconnect_wifi()
{
    esp_timer_create_args_t args = {
            .callback = &esp_reconnect_wifi_helper,
            .name = "reconnect_wifi"
    };
    if (!handle) esp_timer_create(&args, &handle);
    esp_timer_start_once(handle, 500 * 1000);
}

static struct {
    uint8_t wifi_started:1;
    uint8_t sta_connected:1;
} wifi_status;
static hal_wifi_ip_stat_t _ip_stat;
static esp_err_t handle_event_cb(void *ctx, system_event_t *evt)
{
    hal_wifi_module_t *m = hal_wifi_get_default_module();
    hal_wifi_ip_stat_t stat;
    hal_wifi_ap_info_adv_t info = {0};
    int eid = evt->event_id;

    printf("%s %d\n", __func__, eid);
    switch (eid) {
        case SYSTEM_EVENT_STA_START:
            wifi_status.wifi_started = 1;
            //ESP_ERROR_CHECK(esp_wifi_connect());
            break;
        case SYSTEM_EVENT_STA_GOT_IP: {
            memcpy(_ip_stat.ip,
                   ip4addr_ntoa(&evt->event_info.got_ip.ip_info.ip),
                   sizeof(_ip_stat.ip));
            memcpy(_ip_stat.mask,
                   ip4addr_ntoa(&evt->event_info.got_ip.ip_info.netmask),
                   sizeof(_ip_stat.mask));
            memcpy(_ip_stat.gate,
                   ip4addr_ntoa(&evt->event_info.got_ip.ip_info.gw),
                   sizeof(_ip_stat.gate));
            if (m->ev_cb && m->ev_cb->ip_got) {
                m->ev_cb->ip_got(m, &_ip_stat, NULL);
            }
            if (m->ev_cb && m->ev_cb->para_chg) {
                memcpy(info.bssid, _bssid, 6);
                m->ev_cb->para_chg(m, &info, NULL, 0, NULL);
            }
            break;
        }
        case SYSTEM_EVENT_SCAN_DONE: {
            break;
        }
        case SYSTEM_EVENT_STA_CONNECTED: {
            wifi_status.sta_connected = 1;
            memcpy(_bssid, evt->event_info.connected.bssid, 6);
            if (m->ev_cb && m->ev_cb->stat_chg) {
                m->ev_cb->stat_chg(m, NOTIFY_STATION_UP, NULL);
            }
            break;
        }
        case SYSTEM_EVENT_STA_DISCONNECTED: {
            wifi_status.sta_connected = 0;
            if (m->ev_cb && m->ev_cb->stat_chg) {
                m->ev_cb->stat_chg(m, NOTIFY_STATION_DOWN, NULL);
            }
            esp_reconnect_wifi();
            break;
        }
        case SYSTEM_EVENT_STA_AUTHMODE_CHANGE: {
            break;
        }
        case SYSTEM_EVENT_AP_STACONNECTED: {
            break;
        }
        case SYSTEM_EVENT_AP_STADISCONNECTED: {
            break;
        }
        case SYSTEM_EVENT_AP_PROBEREQRECVED: {
            break;
        }
        default: {
            break;
        }

    }

    return ESP_OK;
}

typedef enum {
    HAL_WIFI_MODE_STATION,
    HAL_WIFI_MODE_SOFTAP
} hal_wifi_mode_t;

static int wifi_set_mode(hal_wifi_module_t *m, hal_wifi_mode_t mode)
{
    int ret = -1;
    if (mode == HAL_WIFI_MODE_STATION)
        ret = esp_wifi_set_mode(WIFI_MODE_STA);
    else if (mode == HAL_WIFI_MODE_SOFTAP)
        ret = esp_wifi_set_mode(WIFI_MODE_AP);
    return ret;
}

static int wifi_connect_prepare(hal_wifi_module_t *m, char *ssid, char *password)
{
    int ret = -1;
    wifi_config_t config;
    memset(&config, 0, sizeof(wifi_config_t));

    /* Out of limit */
    if (strlen(ssid) > sizeof(config.sta.ssid))
        goto err;
    if (strlen(password) > sizeof(config.sta.password))
        goto err;
    strncpy((char *)(config.sta.ssid), ssid, sizeof(config.sta.ssid) - 1);
    strncpy((char *)(config.sta.password), password, sizeof(config.sta.password) - 1);
    /* Do connect */
    ret = esp_wifi_set_mode(WIFI_MODE_STA);
    ret = esp_wifi_set_config(WIFI_IF_STA, &config);
    ret = esp_wifi_start();
    ret = esp_wifi_connect();
    return 0;
err:
    return ret;
}

typedef enum {
    HAL_WIFI_SMART_CONFIG_START,
    HAL_WIFI_SMART_CONFIG_STOP,
    HAL_WIFI_CHECK_SUPPORT_SMART_CONFIG,
    HAL_WIFI_GET_CHANNEL,
    HAL_WIFI_SET_CHANNEL,
    HAL_WIFI_PROMISCUOUS_START,
    HAL_WIFI_PROMISCUOUS_STOP,
    HAL_WIFI_SET_AUTO_CONNECT,
    HAL_WIFI_GET_MAC_ADDRESS,
    HAL_WIFI_GET_AP_RSSI
} hal_wifi_getset_cmd_t;

static int wifi_getset_ops(hal_wifi_module_t *m, hal_wifi_getset_cmd_t cmd, ...)
{
    switch (cmd) {
    case HAL_WIFI_GET_CHANNEL: {
        uint8_t channel;
        wifi_second_chan_t sec_channel;
        ESP_ERROR_CHECK(esp_wifi_get_channel(&channel, &sec_channel));
        return channel;
    }
    case HAL_WIFI_SET_CHANNEL: {
        int c;
        va_list args;
        va_start(args, cmd);
        c = va_arg(args, int);
        uint8_t ch = c;
        printf("%s %d: channe - %d\r\n", __func__, __LINE__, ch);
        if (ch <= 13) // skip 14+ since those will fail per test result
            ESP_ERROR_CHECK(esp_wifi_set_channel(ch, 0));
        va_end(args);
        break;
    }
    case HAL_WIFI_PROMISCUOUS_START: {
        uint8_t mac[6];
        ESP_ERROR_CHECK(esp_wifi_get_mac(WIFI_IF_STA, mac));
        ESP_ERROR_CHECK(esp_wifi_set_promiscous_autoack(true, mac));
        ESP_ERROR_CHECK(esp_wifi_set_promiscuous(1));
        ESP_ERROR_CHECK(esp_wifi_set_promiscuous_rx_cb(promiscuous_rx_cb));
        break;
    }
    case HAL_WIFI_PROMISCUOUS_STOP: {
        ESP_ERROR_CHECK(esp_wifi_set_promiscuous(0));
        break;
    }
    case HAL_WIFI_SET_AUTO_CONNECT: {
        int c;
        va_list args;
        va_start(args, cmd);
        c = va_arg(args, int);
        /* Reconnect after power on */
        ESP_ERROR_CHECK(esp_wifi_set_auto_connect((bool)c));
        va_end(args);
        break;
    }
    case HAL_WIFI_GET_MAC_ADDRESS: {
        uint8_t *mac;
        int hal_mode;
        va_list args;
        va_start(args, cmd);
        hal_mode = va_arg(args, int);
        mac = va_arg(args, uint8_t*);
        va_end(args);
        if (hal_mode == HAL_WIFI_MODE_SOFTAP) {
            ESP_ERROR_CHECK(esp_wifi_get_mac(WIFI_IF_AP, mac));
        } else if (hal_mode == HAL_WIFI_MODE_STATION) {
            ESP_ERROR_CHECK(esp_wifi_get_mac(WIFI_IF_STA, mac));
        } else {
            return -1;
        }
        break;
    }
    case HAL_WIFI_GET_AP_RSSI: {
        wifi_ap_record_t ap_info;
        if (ESP_OK == esp_wifi_sta_get_ap_info(&ap_info))
            return ap_info.rssi;
        return -1;
    }
    default:{
        return -1;
    }
    }

    return 0;
}

static int wifi_init(hal_wifi_module_t *m)
{
    static int inited;
    int ret;
    if (inited)
        return 0;
    inited = 1;

    /* nvs flash initialization, this function may not used in aos, kv is similar function compared with nvs */
    ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    /* Hook Event */
    ret = esp_event_loop_init(handle_event_cb, NULL);
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_start();
    hal_wdg_finalize(NULL);
    return 0;
};

static void wifi_get_mac_addr(hal_wifi_module_t *m, uint8_t *mac)
{
    wifi_getset_ops(m, HAL_WIFI_GET_MAC_ADDRESS, HAL_WIFI_MODE_STATION, mac);
};

static int wifi_start(hal_wifi_module_t *m, hal_wifi_init_type_t *init_para)
{
    wifi_connect_prepare(m, init_para->wifi_ssid, init_para->wifi_key);
    return 0;
}

static int wifi_start_adv(hal_wifi_module_t *m,
                          hal_wifi_init_type_adv_t *init_para_adv)
{
    printf("WiFi HAL %s not implemeted yet!\r\n", __func__);
    return 0;
}

static int get_ip_stat(hal_wifi_module_t *m,
                       hal_wifi_ip_stat_t *out_net_para,
                       hal_wifi_type_t wifi_type)
{
    if (wifi_type == HAL_WIFI_MODE_STATION)
        memcpy(out_net_para, &_ip_stat, sizeof(hal_wifi_ip_stat_t));
    return 0;
}

static int get_link_stat(hal_wifi_module_t *m,
                         hal_wifi_link_stat_t *out_stat)
{
    wifi_ap_record_t ap_info;

    if (ESP_OK == esp_wifi_sta_get_ap_info(&ap_info)) {
        out_stat->is_connected = 1;
        out_stat->wifi_strength = ap_info.rssi;
        memcpy(out_stat->ssid, ap_info.ssid, sizeof(out_stat->ssid));
        memcpy(out_stat->bssid, ap_info.bssid, sizeof(out_stat->bssid));
        out_stat->channel = ap_info.primary;
    }

    return 0;
}

static void wifi_scan_helper(hal_wifi_module_t *m, scan_type_t t)
{
    wifi_scan_config_t scan_config;
    memset(&scan_config, 0, sizeof(scan_config));
    if (ESP_OK != esp_wifi_scan_start(&scan_config, true))
        return;
    scan_done(m, t);
}

static void wifi_scan(hal_wifi_module_t *m)
{
    wifi_scan_helper(m, SCAN_NORMAL);
}

static void start_scan_adv(hal_wifi_module_t *m)
{
    wifi_scan_helper(m, SCAN_ADV);
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
    wifi_mode_t mode;
    if (!wifi_status.wifi_started) return 0;
    ESP_ERROR_CHECK(esp_wifi_get_mode(&mode));
    if (mode == WIFI_MODE_STA && wifi_status.sta_connected) {
        ESP_ERROR_CHECK(esp_wifi_disconnect());
    }

    return 0;
}

static int suspend_station(hal_wifi_module_t *m)
{
    if (wifi_status.wifi_started && wifi_status.sta_connected)
        ESP_ERROR_CHECK(esp_wifi_disconnect());

    return 0;
}

static int suspend_soft_ap(hal_wifi_module_t *m)
{
    return 0;
}

static int set_channel(hal_wifi_module_t *m, int ch)
{
    wifi_getset_ops(m, HAL_WIFI_SET_CHANNEL, ch);
    return 0;
}

static int get_channel(hal_wifi_module_t *m)
{
    return wifi_getset_ops(m, HAL_WIFI_GET_CHANNEL);
}

static void start_monitor(hal_wifi_module_t *m)
{
    wifi_getset_ops(m, HAL_WIFI_PROMISCUOUS_START);
}

static void stop_monitor(hal_wifi_module_t *m)
{
    wifi_getset_ops(m, HAL_WIFI_PROMISCUOUS_STOP);
}

static void register_monitor_cb(hal_wifi_module_t *m, monitor_data_cb_t fn)
{
    data_cb = fn;
}

static void register_mesh_cb(hal_wifi_module_t *m, monitor_data_cb_t fn)
{
    mesh_data_cb = fn;
}

static void register_wlan_mgnt_monitor_cb(hal_wifi_module_t *m,
                                          monitor_data_cb_t fn)
{
    mngt_data_cb = fn;
    // Workaround for zero config <TODO>
    hal_wifi_register_monitor_cb(NULL, NULL);
    hal_wifi_start_wifi_monitor(NULL);
}

static int wlan_send_80211_raw_frame(hal_wifi_module_t *m,
                                     uint8_t *buf, int len)
{
    ESP_ERROR_CHECK(esp_wifi_80211_tx(ESP_IF_WIFI_STA, buf, len, 1));

    return 0;
}

static int mesh_enable(hal_wifi_module_t *module)
{
    bool enable;
    esp_err_t esp_err;
    uint8_t mac[6];

    esp_err = esp_wifi_get_promiscuous(&enable);
    if (enable == false) {
        ESP_ERROR_CHECK(esp_wifi_get_mac(WIFI_IF_STA, mac));
        ESP_ERROR_CHECK(esp_wifi_set_promiscous_autoack(true, mac));
        ESP_ERROR_CHECK(esp_wifi_set_promiscuous(1));
        ESP_ERROR_CHECK(esp_wifi_set_promiscuous_rx_cb(promiscuous_rx_cb));
    }
    return 0;
}

static int mesh_disable(hal_wifi_module_t *module)
{
    return 0;
}

static int get_wireless_info(hal_wifi_module_t *m, hal_wireless_info_t *wireless_info)
{
    hal_wireless_info_t *info = (hal_wireless_info_t *)wireless_info;

    printf("get wireless info\r\n");

    if (info == NULL)
        return -1;
    do {
        wifi_ap_record_t ap_info;
        if (ESP_OK != esp_wifi_sta_get_ap_info(&ap_info))
            return -1;
        if (ap_info.rssi > 0) {
            ap_info.rssi -= 128;
        }
        info->rssi = ap_info.rssi;
    } while (0);

    return 0;
}

hal_wifi_module_t sim_aos_wifi_eps32 = {
    .base.name           = "sim_aos_wifi_esp32",
    .init                =  wifi_init,
    .get_mac_addr        =  wifi_get_mac_addr,
    .start               =  wifi_start,
    .start_adv           =  wifi_start_adv,
    .get_ip_stat         =  get_ip_stat,
    .get_link_stat       =  get_link_stat,
    .start_scan          =  wifi_scan,
    .start_scan_adv      =  start_scan_adv,
    .power_off           =  power_off,
    .power_on            =  power_on,
    .suspend             =  suspend,
    .suspend_station     =  suspend_station,
    .suspend_soft_ap     =  suspend_soft_ap,
    .set_channel         =  set_channel,
    .get_channel         =  get_channel,
    .start_monitor       =  start_monitor,
    .stop_monitor        =  stop_monitor,
    .register_monitor_cb =  register_monitor_cb,
    .register_wlan_mgnt_monitor_cb = register_wlan_mgnt_monitor_cb,
    .wlan_send_80211_raw_frame = wlan_send_80211_raw_frame,

    .get_wireless_info   = get_wireless_info,
    /* mesh related */
    .mesh_register_cb    =  register_mesh_cb,
    .mesh_enable         =  mesh_enable,
    .mesh_disable        =  mesh_disable,
};

void esp32_wifi_mesh_register(void)
{
    hal_umesh_register_wifi(&sim_aos_wifi_eps32);
}
