/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
#if defined(WIFI_PROVISION_ENABLED)

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <string.h>

#include "aos/kernel.h"
#include "ulog/ulog.h"
#ifdef FEATURE_UND_WITH_WIFI
#include "und/und.h"
#endif

#include <netmgr.h>

#include "linkkit/wrappers/wrappers_defs.h"
#ifdef AOS_COMP_PWRMGMT
#include "aos/pwrmgmt.h"
#endif

#include "aos/device_core.h"
#include "aos/hal/wifi.h"
#include <lwip/ip_addr.h>
#include "vfsdev/wifi_dev.h"
#define TAG "HALAWSS"
#include "vfs_types.h"
#include "vfs_inode.h"
#include "netmgr.h"
#include "uservice/eventid.h"

#define WIFI_SSID_KV_KEY     "wifi_ssid"
#define WIFI_PASSWORD_KV_KEY "wifi_password"

awss_recv_80211_frame_cb_t g_ieee80211_handler;

#define CONFIG_DEV_AOS
static void netmgr_comp_enable(const char *ssid, const char *password);

static void *get_wifi_dev()
{
    static netdev_t *wifi_dev = NULL;
    if (wifi_dev == NULL) {
        vfs_inode_t *node = vfs_inode_open("/dev/wifi0");
        wifi_dev = (netdev_t *)node->i_arg;
    }
    return wifi_dev;
}

static void get_mac(uint8_t mac[6])
{
    static uint8_t s_mac[6] = { 0 };

    if (s_mac[0] == 0 && s_mac[1] == 0 && s_mac[2] == 0 && s_mac[3] == 0 &&
        s_mac[4] == 0 && s_mac[5] == 0) {
        netdev_t *wifi_dev = get_wifi_dev();
        if (wifi_dev == NULL) {
            return;
        }
        hal_wifi_get_mac_addr(wifi_dev, s_mac);
    }

    memcpy(mac, s_mac, 6);
}

static void monitor_data_handler(wifi_promiscuous_pkt_t *buf,
                                 wifi_promiscuous_pkt_type_t type)
{
    if (!buf) {
        return;
    }
    // with_fcs 1, link_type AWSS_LINK_TYPE_NONE, AWSS_LINK_TYPE_80211_RADIO
    int with_fcs = 0;
    unsigned char rssi = buf->rx_ctrl.rssi;
    int link_type = AWSS_LINK_TYPE_NONE;
    (*g_ieee80211_handler)((char *)buf->payload, buf->rx_ctrl.sig_len,
                           link_type, with_fcs, rssi);
}

void HAL_Awss_Open_Monitor(_IN_ awss_recv_80211_frame_cb_t cb)
{
    netdev_t *wifi_dev = (netdev_t *)get_wifi_dev();
    if (wifi_dev == NULL) {
        return;
    }

#ifdef AOS_COMP_PWRMGMT
    aos_pwrmgmt_lowpower_suspend(PWRMGMT_NETMGR);
#endif
    g_ieee80211_handler = cb;

    hal_wifi_start_monitor(wifi_dev, monitor_data_handler);
    HAL_Awss_Switch_Channel(6, 0, NULL);
}

void HAL_Awss_Close_Monitor(void)
{
    netdev_t *wifi_dev = (netdev_t *)get_wifi_dev();
    if (wifi_dev == NULL) {
        return;
    }

    hal_wifi_stop_monitor(wifi_dev);
}

int HAL_Awss_Get_Channelscan_Interval_Ms(void)
{
    return 250;
}

int HAL_Awss_Get_Timeout_Interval_Ms(void)
{
    return 300 * 1000;
}

void HAL_Awss_Switch_Channel(char primary_channel, char secondary_channel,
                             uint8_t bssid[ETH_ALEN])
{
    int ret = -1;
    netdev_t *wifi_dev = (netdev_t *)get_wifi_dev();
    if (wifi_dev == NULL) {
        return;
    }

    ret = hal_wifi_set_channel(wifi_dev, (int)primary_channel);
}

int HAL_Awss_Open_Ap(const char *ssid, const char *passwd, int beacon_interval,
                     int hide)
{
    wifi_config_t config;

    netdev_t *wifi_dev = (netdev_t *)get_wifi_dev();
    if (wifi_dev == NULL) {
        return -1;
    }

    hal_wifi_deinit(wifi_dev);
    memset(&config, 0, sizeof(wifi_config_t));

    if (ssid != NULL) {
        strncpy(config.ssid, ssid, HAL_MAX_SSID_LEN);
    }

    if (passwd != NULL) {
        strncpy(config.password, passwd, HAL_MAX_PASSWD_LEN);
    }

    config.mode = WIFI_MODE_AP;
    config.ap_config.beacon_interval = beacon_interval;
    config.ap_config.hide_ssid = hide;
    return hal_wifi_init(wifi_dev, &config);
}

int HAL_Wifi_Send_80211_Raw_Frame(HAL_Awss_Frame_Type_t type, uint8_t *buffer,
                                  int len)
{
    netdev_t *wifi_dev = (netdev_t *)get_wifi_dev();
    if (wifi_dev == NULL)
        return -1;
    return hal_wifi_send_80211_raw_frame(wifi_dev, buffer, len);
}
int HAL_Awss_Close_Ap()
{
    netdev_t *wifi_dev = (netdev_t *)get_wifi_dev();
    if (wifi_dev == NULL) {
        return -1;
    }

    hal_wifi_deinit(wifi_dev);

    return 0;
}

int HAL_Awss_Connect_Ap(_IN_ uint32_t connection_timeout_ms,
                        _IN_ char ssid[HAL_MAX_SSID_LEN],
                        _IN_ char passwd[HAL_MAX_PASSWD_LEN],
                        _IN_OPT_ enum AWSS_AUTH_TYPE auth,
                        _IN_OPT_ enum AWSS_ENC_TYPE encry,
                        _IN_OPT_ uint8_t bssid[ETH_ALEN],
                        _IN_OPT_ uint8_t channel)
{
    int ret = -1;

    LOGE(TAG, "connect ap not implemented");

    printf("%s-%d ssid:%s paaswd:%s\r\n", __func__, __LINE__, ssid, passwd);
    ret = aos_kv_set(WIFI_SSID_KV_KEY, ssid, strlen(ssid), 1);
    if (ret) {
        printf("aos_kv_set %s ret:%d\r\n", WIFI_SSID_KV_KEY, ret);
    }
    ret = aos_kv_set(WIFI_PASSWORD_KV_KEY, passwd, strlen(passwd), 1);
    if (ret) {
        printf("aos_kv_set %s ret:%d\r\n", WIFI_PASSWORD_KV_KEY, ret);
    }
    netmgr_comp_enable(ssid, passwd);

    return 0;
}

uint32_t HAL_Wifi_Get_IP(char ip_str[NETWORK_ADDR_LEN], const char *ifname)
{
    ip_addr_t ipaddr;
    ip_addr_t netmask;
    ip_addr_t gw;
    uint8_t octet[4] = { 0, 0, 0, 0 };

    netdev_t *wifi_dev = (netdev_t *)get_wifi_dev();
    if (wifi_dev == NULL) {
        return 0;
    }

    /** ifconfig */
    // hal_net_get_ipaddr(wifi_dev, &ipaddr, &netmask, &gw);

    for (int i = 0; i < 4; i++) {
        octet[i] = (ipaddr.addr >> ((3 - i) * 8)) & 0xFF;
    }
    sprintf(ip_str, "%d.%d.%d.%d", octet[3], octet[2], octet[1], octet[0]);

    return 0;
}

char *HAL_Wifi_Get_Mac(_OU_ char mac_str[HAL_MAC_LEN])
{
    uint8_t mac[6] = { 0 };

    get_mac(mac);

    snprintf(mac_str, HAL_MAC_LEN, "%02x:%02x:%02x:%02x:%02x:%02x", mac[0],
             mac[1], mac[2], mac[3], mac[4], mac[5]);

    return mac_str;
}

int HAL_Wifi_Get_Ap_Info(char ssid[HAL_MAX_SSID_LEN],
                         char passwd[HAL_MAX_PASSWD_LEN],
                         uint8_t bssid[ETH_ALEN])
{
    if (ssid) {
        int ssid_len;
        aos_kv_get("wifi_ssid", ssid, &ssid_len);
    }

    if (passwd) {
        int psk_len;
        aos_kv_get("wifi_psk", ssid, &psk_len);
    }

    if (bssid) {
        memset(bssid, 0, ETH_ALEN);
    }

    return 0;
}

/*
 * @brief 检查Wi-Fi网卡、芯片或模组当前的IP地址是否有效，AliOS内部
 *        已经对接完成
 *
 * @param None.
 * @return, 0: IP地址无效; 1: IP地址有效，可以发起网络连接
 * @see None.
 * @note
 *    1）该API对接有问题，可能导致AWSS一致无法退出，有可能出现设
 *       备成功连接AP后又重新开始扫描
 *    2）各种配网都需要对接
 */
int HAL_Sys_Net_Is_Ready()
{
    netmgr_hdl_t hdl;
    netmgr_ifconfig_info_t info = { 0 };

    hdl = netmgr_get_dev("/dev/wifi0");
    if (hdl < 0) {
        return -1;
    }

    return (netmgr_get_state(hdl) == CONN_STATE_NETWORK_CONNECTED) ? 1 : 0;
}

static void wifi_event_cb(uint32_t event_id, const void *param, void *context)
{
    printf("wifi event cb\r\n");
}

static void netmgr_comp_enable(const char *ssid, const char *password)
{
    netmgr_hdl_t fd;
    netmgr_connect_params_t *params;
    printf("netmgr test \r\n");
    if (!ssid || !password) {
        return;
    }
    event_service_init(NULL);

    netmgr_service_init(NULL);

    event_subscribe(EVENT_NETMGR_DHCP_SUCCESS, wifi_event_cb, NULL);

    printf("%s-%d ssid:%s paaswd:%s\r\n", __func__, __LINE__, ssid,
           password);
    params = aos_malloc(sizeof(netmgr_connect_params_t));

    fd = netmgr_get_dev("/dev/wifi0");
    params->type = NETMGR_TYPE_WIFI;
    memcpy(params->params.wifi_params.ssid, ssid, strlen(ssid) + 1);
    memcpy(params->params.wifi_params.pwd, password, strlen(password) + 1);
    netmgr_connect(fd, params);
    aos_free(params);

    return;
}

#ifdef __cplusplus
}
#endif
#endif
