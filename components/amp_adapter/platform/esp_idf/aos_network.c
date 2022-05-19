/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <string.h>
#include "amp_platform.h"
#include "aos_network.h"
#include "esp_eth.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_event.h"
#include "lwip/dns.h"
#include "mdns.h"

#if !MICROPY_ESP_IDF_4
#include "esp_wifi_types.h"
#include "esp_event_loop.h"
#endif

#include "modnetwork.h"

#if ESP_IDF_VERSION < ESP_IDF_VERSION_VAL(4, 1, 0)
#define DNS_MAIN TCPIP_ADAPTER_DNS_MAIN
#else
#define DNS_MAIN ESP_NETIF_DNS_MAIN
#endif

#define MODNETWORK_INCLUDE_CONSTANTS (1)


static int _remap_exceptions(esp_err_t e)
{
    int32_t err = 0;
    switch (e) {
    case ESP_ERR_WIFI_NOT_INIT:
        err = AOS_ERR_WIFI_NOT_INIT;
        break;
    case ESP_ERR_WIFI_NOT_STARTED:
        err = AOS_ERR_WIFI_NOT_STARTED;
        break;
    case ESP_ERR_WIFI_NOT_STOPPED:
        err = AOS_ERR_WIFI_NOT_STOPPED;
        break;
    case ESP_ERR_WIFI_IF:
        err = AOS_ERR_WIFI_IF;
        break;
    case ESP_ERR_WIFI_MODE:
        err = AOS_ERR_WIFI_MODE;
        break;
    case ESP_ERR_WIFI_STATE:
        err = AOS_ERR_WIFI_STATE;
        break;
    case ESP_ERR_WIFI_CONN:
        err = AOS_ERR_WIFI_CONN;
        break;
    case ESP_ERR_WIFI_NVS:
        err = AOS_ERR_WIFI_NVS;
        break;
    case ESP_ERR_WIFI_MAC:
        err = AOS_ERR_WIFI_MAC;
        break;
    case ESP_ERR_WIFI_SSID:
        err = AOS_ERR_WIFI_SSID;
        break;
    case ESP_ERR_WIFI_PASSWORD:
        err = AOS_ERR_WIFI_PASSWORD;
        break;
    case ESP_ERR_WIFI_TIMEOUT:
        err = ETIMEDOUT;
        break;
    case ESP_ERR_WIFI_WAKE_FAIL:
        err = AOS_ERR_WIFI_WAKE_FAIL;
        break;
    case ESP_ERR_WIFI_WOULD_BLOCK:
        err = AOS_ERR_WIFI_WOULD_BLOCK;
        break;
    case ESP_ERR_WIFI_NOT_CONNECT:
        err = AOS_ERR_WIFI_NOT_CONNECT;
        break;
    case ESP_ERR_TCPIP_ADAPTER_INVALID_PARAMS:
        err = AOS_ERR_TCPIP_ADAPTER_INVALID_PARAMS;
        break;
    case ESP_ERR_TCPIP_ADAPTER_IF_NOT_READY:
        err = AOS_ERR_TCPIP_ADAPTER_IF_NOT_READY;
        break;
    case ESP_ERR_TCPIP_ADAPTER_DHCPC_START_FAILED:
        err = AOS_ERR_TCPIP_ADAPTER_DHCPC_START_FAILED;
        break;
    case ESP_ERR_TCPIP_ADAPTER_NO_MEM:
        err = ENOMEM;
        break;
    }
    return err;
}

esp_err_t wifi_event_cb_t(void *ctx, system_event_t *event)
{
    int wifi_state = 0;
    aos_wifi_manager_t *wifi_manager = NULL;
    if (ctx != NULL) {
        wifi_manager = (aos_wifi_manager_t *)ctx;
    }
    switch (event->event_id) {
    case SYSTEM_EVENT_STA_START:
        ESP_LOGE("wifi", "STA_START");
        wifi_state = AOS_NET_STA_STARTED;
        break;
    case SYSTEM_EVENT_STA_CONNECTED:
        ESP_LOGE("network", "CONNECTED");
        wifi_state = AOS_NET_STA_CONNECTED;
        break;
    case SYSTEM_EVENT_STA_GOT_IP:
        ESP_LOGE("network", "GOT_IP");
        wifi_state = AOS_NET_STA_GOT_IP;
        break;
    case SYSTEM_EVENT_STA_LOST_IP:
        ESP_LOGE("network", "GOT_IP");
        wifi_state = AOS_NET_STA_LOST_IP;
        break;
    case SYSTEM_EVENT_STA_DISCONNECTED: {
        system_event_sta_disconnected_t *disconn = &event->event_info.disconnected;
        char *message = "";
        // wifi_sta_disconn_reason = disconn->reason;
        switch (disconn->reason) {
        case WIFI_REASON_BEACON_TIMEOUT:
            // AP has dropped out; try to reconnect.
            message = "\nbeacon timeout";
            break;
        case WIFI_REASON_NO_AP_FOUND:
            // AP may not exist, or it may have momentarily dropped out; try to reconnect.
            message = "\nno AP found";
            break;
        case WIFI_REASON_AUTH_FAIL:
            // Password may be wrong, or it just failed to connect; try to reconnect.
            message = "\nauthentication failed";
            break;
        default:
            // Let other errors through and try to reconnect.
            break;
        }
        wifi_state = AOS_NET_STA_DISCONNECTED;
        ESP_LOGE("wifi", "STA_DISCONNECTED, reason:%d%s", disconn->reason, message);
        break;
    }
    case SYSTEM_EVENT_STA_STOP:
        wifi_state = AOS_NET_STA_STOPED;
        break;
    case SYSTEM_EVENT_GOT_IP6:
        ESP_LOGE("network", "Got IPv6");
        break;
    case SYSTEM_EVENT_ETH_START:
        ESP_LOGE("ethernet", "start");
        break;
    case SYSTEM_EVENT_ETH_STOP:
        ESP_LOGE("ethernet", "stop");
        break;
    case SYSTEM_EVENT_ETH_CONNECTED:
        ESP_LOGE("ethernet", "LAN cable connected");
        break;
    case SYSTEM_EVENT_ETH_DISCONNECTED:
        ESP_LOGE("ethernet", "LAN cable disconnected");
        break;
    case SYSTEM_EVENT_ETH_GOT_IP:
        ESP_LOGE("ethernet", "Got IP");
        break;
    default:
        ESP_LOGE("network", "event %d", event->event_id);
        break;
    }

    if (wifi_manager != NULL) {
        wifi_manager->wifi_state = wifi_state;
    }

    if (wifi_manager->cb != NULL) {
        wifi_manager->cb(wifi_state, ctx);
    }
    return ESP_OK;
}

static esp_err_t init_wlan()
{
    esp_err_t ret = 0;
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ret = esp_wifi_init(&cfg);
    if (ret != 0) {
        return ret;
    }
    ret = esp_wifi_set_storage(WIFI_STORAGE_RAM);
    return _remap_exceptions(ret);
}

int aos_wifi_init(aos_wifi_manager_t *wifi_manager)
{
    esp_err_t ret = 0;
    if (wifi_manager == NULL) {
        return -1;
    }

    if (!wifi_manager->is_initialized) {
        tcpip_adapter_init();
        ret = esp_event_loop_init(wifi_event_cb_t, wifi_manager);
        if (ret != 0) {
            return _remap_exceptions(ret);
        }

        ret = init_wlan();
        if (ret != 0) {
            return _remap_exceptions(ret);
        }
        wifi_manager->is_initialized = true;
    }
    return _remap_exceptions(ret);
}

int aos_wifi_start(aos_wifi_manager_t *wifi_manager)
{
    esp_err_t ret = 0;
    wifi_mode_t mode = WIFI_MODE_NULL;

    switch (wifi_manager->wifi_mode) {
    case AOS_NETWORK_WIFI_STA:
        mode = WIFI_MODE_STA;
        break;
    case AOS_NETWORK_WIFI_AP:
        mode = WIFI_MODE_AP;
        break;
    }

    if (wifi_manager->is_started) {
        ESP_LOGE("ethernet", "wifi haas started");
        return _remap_exceptions(ret);
    }

    esp_wifi_set_mode(mode);
    ret = esp_wifi_start();
    if (ret != 0) {
        return _remap_exceptions(ret);
    }
    wifi_manager->is_started = true;

    return _remap_exceptions(ret);
}

int aos_wifi_stop(aos_wifi_manager_t *wifi_manager)
{
    esp_err_t ret = 0;
    if (!wifi_manager->is_started) {
        ESP_LOGE("wifi", "wifi not start");
        return _remap_exceptions(ret);
    }
    esp_wifi_set_mode(WIFI_MODE_NULL);
    esp_wifi_stop();
    wifi_manager->is_started = false;
    return _remap_exceptions(ret);
}

int aos_wifi_deinit(aos_wifi_manager_t *wifi_manager)
{
    aos_wifi_stop(wifi_manager);
    return esp_wifi_deinit();
}

int aos_wifi_set_ifconfig(void *info)
{
    return 0;
}

int aos_wifi_get_ifconfig(void *info)
{
    return 0;
}

AOS_NETWORK_TYPE_E aos_get_network_type()
{
    return AOS_NETWORK_WIFI;
}

static int min(int a, int b)
{
    return a < b ? a : b;
}

int aos_wifi_connect(const char *ssid, const char *passwd)
{
    int ret = 0;

    wifi_config_t wifi_sta_config = {0};
    if (passwd != NULL) {
        memcpy(wifi_sta_config.sta.ssid, ssid, min(strlen(ssid), sizeof(wifi_sta_config.sta.ssid)));
    }

    if (passwd != NULL) {
        memcpy(wifi_sta_config.sta.password, passwd, min(strlen(passwd), sizeof(wifi_sta_config.sta.password)));
    }
    ESP_LOGI("wifi", "esp_wifi_set_config %s, %s\n", wifi_sta_config.sta.ssid, wifi_sta_config.sta.password);
    ret = esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_sta_config);

    if (ret != 0) {
        ESP_LOGE("wifi", "esp_wifi_set_config failed, %4x", ret);
        return _remap_exceptions(ret);
    }

    _remap_exceptions(esp_wifi_connect());
}

int aos_wifi_get_info(aos_wifi_info_t *wifi_info)
{
    uint8_t mac[6];
    wifi_config_t cfg;
    esp_wifi_get_config(WIFI_MODE_STA, &cfg);
    esp_wifi_get_mac(WIFI_MODE_STA, wifi_info->mac);
    memcpy(wifi_info->ssid, cfg.sta.ssid, min(strlen((char *)cfg.sta.ssid), sizeof(wifi_info->ssid)));
    return 0;
}

int aos_wifi_disconnect()
{
    _remap_exceptions(esp_wifi_disconnect());
}

int aos_wifi_get_state()
{
    return 0;
}

static void net_parse_ipv4_addr(char *addr_in, uint8_t *out_ip)
{
    size_t addr_len;
    const char *s = addr_in;
    for (uint8_t i = 3; ; i--) {
        uint8_t val = 0;
        for (; *s != '\0' && *s != '.'; s++) {
            val = val * 10 + *s - '0';
        }
        out_ip[3 - i] = val;
        if (i == 0 && *s == '\0') {
            return;
        } else if (i > 0 && *s == '.') {
            s++;
        } else {
            ESP_LOGE("network", "invalid arguments\n");
        }
    }
}

int aos_net_set_ifconfig(aos_ifconfig_info_t *info)
{
    tcpip_adapter_ip_info_t ip_info;
    tcpip_adapter_dns_info_t dns_info;

    ESP_LOGE("network", "esp_ifconfig set\n");
    if (info == NULL) {
        ESP_LOGE("network", "info is NULL, return ESP_ERR\n");
        return -1;
    }

    if (info->dhcp_en) {
        ESP_LOGE("network", "enable dhcp mode for sta\n");
        tcpip_adapter_dhcps_start(WIFI_IF_STA);
        return 0;
    }

    /* ip_addr */
    net_parse_ipv4_addr(info->ip_addr, (uint8_t *)&ip_info.ip);

    /* mask */
    net_parse_ipv4_addr(info->mask, (uint8_t *)&ip_info.netmask);

    /* gw */
    net_parse_ipv4_addr(info->gw, (uint8_t *)&ip_info.gw);

    /* dns_server */
    net_parse_ipv4_addr(info->dns_server, (uint8_t *)&dns_info.ip);

    ESP_LOGE("network", "stop dhcp and enable static ip mode\n");
    tcpip_adapter_dhcpc_stop(WIFI_IF_STA);
    tcpip_adapter_set_ip_info(WIFI_IF_STA, &ip_info);
    tcpip_adapter_set_dns_info(WIFI_IF_STA, ESP_NETIF_DNS_MAIN, &dns_info);

    return 0;
}

int aos_net_get_ifconfig(aos_ifconfig_info_t *info)
{
    tcpip_adapter_ip_info_t ip_info;
    tcpip_adapter_dns_info_t dns_info;
    wifi_ap_record_t rssi_info;
    uint8_t *value = NULL;
    uint8_t mac[16] = {0};

    if (NULL == info) {
        ESP_LOGE("network", "info is NULL, return ESP_FAIL");
        return ESP_FAIL;
    }
    tcpip_adapter_get_ip_info(ESP_IF_WIFI_STA, &ip_info);
    tcpip_adapter_get_dns_info(ESP_IF_WIFI_STA, ESP_NETIF_DNS_MAIN, &dns_info);

    /* ip_addr */
    memset(info->ip_addr, 0, IPADDR_STR_LEN);
    value = &ip_info.ip;
    snprintf(info->ip_addr, IPADDR_STR_LEN, "%u.%u.%u.%u", value[0], value[1], value[2], value[3]);

    /* mask */
    memset(info->mask, 0, IPADDR_STR_LEN);
    value = &ip_info.netmask;
    snprintf(info->mask, IPADDR_STR_LEN, "%u.%u.%u.%u", value[0], value[1], value[2], value[3]);

    /* gw */
    memset(info->gw, 0, IPADDR_STR_LEN);
    value = &ip_info.gw;
    snprintf(info->gw, IPADDR_STR_LEN, "%u.%u.%u.%u", value[0], value[1], value[2], value[3]);

    /* dns_server */
    memset(info->dns_server, 0, IPADDR_STR_LEN);
    value = &dns_info.ip;
    snprintf(info->dns_server, IPADDR_STR_LEN, "%u.%u.%u.%u", value[0], value[1], value[2], value[3]);

    /* mac */
    extern esp_err_t esp_read_mac(uint8_t *mac, esp_mac_type_t type);
    memset(info->mac, 0, IPADDR_STR_LEN + 1);
    esp_read_mac(mac, ESP_MAC_WIFI_STA);
    snprintf(info->mac, IPADDR_STR_LEN + 1, "%02x-%02x-%02x-%02x-%02x-%02x",
            mac[0] & 0xff,
            mac[1] & 0xff,
            mac[2] & 0xff,
            mac[3] & 0xff,
            mac[4] & 0xff,
            mac[5] & 0xff);

    esp_wifi_sta_get_ap_info(&rssi_info);
    info->rssi = rssi_info.rssi;

    return 0;
}

int aos_get_network_status()
{
    int net_status = aos_wifi_get_state();
    return net_status == AOS_NET_STA_GOT_IP ? 1 : 0;
}
