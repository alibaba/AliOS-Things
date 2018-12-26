/*
 * Copyright (C) 2016 YunOS Project. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef YUNOS_BSP_WIFI_H
#define YUNOS_BSP_WIFI_H

#include <stdint.h>

#define MAX_SSID_LEN 32
#define MAX_PSWD_LEN 32
#define IP_STRING_LEN 16

#ifndef bool
    #define bool int
#endif

#ifndef true
    #define true  0x01
#endif

#ifndef false
    #define false 0x00
#endif

typedef uint8_t wifi_mac_addr_t[6];

typedef enum _wifi_mode_s {
    WIFI_STA,
    WIFI_SOFTAP,
    WIFI_MONITOR,
    WIFI_UNKNOWN
} wifi_mode_t;

typedef enum {
    WIFI_CONN_SEC_OPEN               = 0,
    WIFI_CONN_SEC_WPA_AES_PSK        = 1,
    WIFI_CONN_SEC_WPA2_AES_PSK       = 2,
    WIFI_CONN_SEC_WEP_PSK            = 3,
    WIFI_CONN_SEC_WEP_SHARED         = 4,
    WIFI_CONN_SEC_WPA_TKIP_PSK       = 5,
    WIFI_CONN_SEC_WPA2_TKIP_PSK      = 6,
    WIFI_CONN_SEC_WPA2_MIXED_PSK     = 7,
    WIFI_CONN_SEC_UNKNOWN            = 0xffffffff
} wifi_conn_sectype_t;


typedef enum _wifi_apmode_sec_s {
    WIFI_APMODE_SEC_OPEN,
    WIFI_APMODE_SEC_WPA, ///< AES
    WIFI_APMODE_SEC_WPA2 ///< AES
} wifi_apmode_sectype_t;

typedef struct _wifi_ap_param_s {
    char ssid[MAX_SSID_LEN];
    char password[MAX_PSWD_LEN];
    wifi_apmode_sectype_t security;
    int32_t channel;
    char ipaddr[IP_STRING_LEN];
} wifi_ap_param_t;

typedef struct _wifi_scan_ap_s {
    char ssid[MAX_SSID_LEN];
    wifi_mac_addr_t bssid;
    int32_t channel;
    wifi_conn_sectype_t security;
    int32_t rssi;
} wifi_scan_ap_t;

typedef int (*wifi_recv_eth_cb_t)(uint8_t *pdata, uint16_t len);
typedef int (*wifi_recv_mon_cb_t)(uint8_t *pdata, uint16_t len, int arg);

/**
 * easy setup
 */
typedef struct _wifi_es_info_s {
    char ssid[MAX_SSID_LEN];
    char password[MAX_PSWD_LEN];
    wifi_conn_sectype_t security_type;
    int32_t channel;
    wifi_mac_addr_t bssid;
} wifi_es_info_t;

typedef void (*wifi_es_res_t)(bool success, wifi_es_info_t *esi);
typedef void (*wifi_link_status_t) (int status);

/**
 * wifi module init
 * @return      0 on success; -1 on failure
 */
int yunos_bsp_wifi_init(void);

/**
 * wifi module reset
 * @return      0 on success; -1 on failure
 */
int yunos_bsp_wifi_reset(void);

/**
 * Get wifi module soft version
 * @return      version string
 */
char * yunos_bsp_wifi_get_version(void);

/**
 * Get wifi module mac address
 * @param[in]   mac_addr  The location to return the MAC address
 * @return      0 on success; -1 on failure
 */
int yunos_bsp_wifi_get_mac_address(wifi_mac_addr_t mac_addr);

/**
 * Get current wifi run mode
 * @return      wifi run mode
 */
wifi_mode_t yunos_bsp_wifi_get_mode(void);

/**
 * Send data to wifi module
 * @param[out]  pdata   data context, 802.3 ignore Preamble and SFD, |dest mac|src mac|....
 * @param[out]  length  data length
 * @return      0 on success; -1 on failure
 */
int yunos_bsp_output_data( uint8_t* pdata, uint16_t length);

/**
 * Add socket multicast group need call this funtion
 * @param[in]   multicast_mac   multicast mac address
 * @return      0 on success; -1 on failure
 */
int yunos_bsp_wifi_add_multicast_address(const wifi_mac_addr_t multicast_mac);

/**
 * Remove socket multicast group need call this funtion
 * @param[in]   multicast_mac   multicast mac address
 * @return      0 on success; -1 on failure
 */
int yunos_bsp_wifi_remove_multicast_address(const wifi_mac_addr_t multicast_mac);

/**
 * Register callback function, for recv data and process by ip stack
 * @param[in]   recv_cb     recv callback funtion
 * @return      0 on success; -1 on failure
 */
int yunos_bsp_wifi_regsiter_input_cb(wifi_recv_eth_cb_t recv_cb);

/**
 * Connect to ap by ssid and pssword
 * @param[in]   ssid        ap ssid
 * @param[in]   password    ap password
 * @return      0 on success; -1 on failure
 */
int yunos_bsp_wifi_connect(const char *ssid, const char *password);

/**
 * Connect to ap by more detial info
 * @param[in]   ssid        ap ssid
 * @param[in]   password    ap password
 * @param[in]   bssid       ap macaddr
 * @param[in]   channel     ap channel 1-13, 0 ignore secrity
 * @return      0 on success; -1 on failure
 */
int yunos_bsp_wifi_connect_ext(const char *ssid, const char *password, 
                               const wifi_mac_addr_t bssid, uint8_t channel,
                               wifi_conn_sectype_t security);

/**
 * Disconnect from ap
 * @return      0 on success; -1 on failure
 */
int yunos_bsp_wifi_disconnect(void);

/**
 * Register callback function, for link status judge
 * @param[in]   link_cb     link_status callback funtion
 * @return      0 on success; <0 on failure
 */
int yunos_bsp_wifi_register_netlink_status_cb(wifi_link_status_t link_status_cb);

/**
 * Get current connect ap ssid
 * @return      return ssid string
 */
char * yunos_bsp_wifi_get_ssid(void);

/**
 * Check wifi module connected ap or not
 * @return      0 on success; <0 on failure
 */
int yunos_bsp_wifi_get_bssid(wifi_mac_addr_t mac_addr);

/**
 * Get current ap signal channel
 * @return      channel  1-13 else error
 */
int yunos_bsp_wifi_get_channel(void);

/**
 * Get current ap security type
 * @return      security type
 */
wifi_conn_sectype_t yunos_bsp_wifi_get_security(void);

/**
 * Get current connect signal rssi
 * @return      return rssi < 0
 */
int yunos_bsp_wifi_get_rssi(void);

/**
 * Get current connect speed rate
 * @return      speed rate Mbps
 */
int yunos_bsp_wifi_get_rate(void);

/**
 * Start soft ap mode
 * @param[in]   param    ap mode param
 * @return      0 on success; <0 on failure
 */
int yunos_bsp_wifi_softap_start(const wifi_ap_param_t *param);

/**
 * Stop soft ap mode
 * @return      0 on success; -1 on failure
 */
int yunos_bsp_wifi_softap_stop(void);

/**
 * start monitor mode and register monitor data callback
 * @param[in]   cb      the call back of monitor
 * @return      0 on success; <0 on failure
 */
int yunos_bsp_wifi_monitor_start(wifi_recv_mon_cb_t cb);

/**
 * Stop monitor mode
 * @return      0 on success; <0 on failure
 */
int yunos_bsp_wifi_monitor_stop(void);

/**
 * Set monitor recv channel
 * @param       channel  wifi channel 1~13
 * @return      0 on success; <0 on failure
 */
int yunos_bsp_wifi_set_channel(int32_t channel);

/**
 * Set monitor filter
 * @return      0 on success; <0 on failure
 */
int yunos_bsp_wifi_set_monitor_filter(void);

/**
 * Clear monitor filter 
 * @return      0 on success; <0 on failure
 */
int yunos_bsp_wifi_clear_monitor_filter(void);

/**
 * Be called when main cpu enter standby mode
 * @param       ipaddr      ip address
 * @return      0 on success; <0 on failure
 */
int yunos_bsp_wifi_before_sleep(uint8_t ipaddr[4]);

/**
 * After main cpu wakeup, call this function, let wifi module in right mode
 * @return      0 on success; -1 on failure
 */
int yunos_bsp_wifi_after_wakeup(void);

/**
 * Scan ap info
 * @return      return size, <0 on failure
 */
int yunos_bsp_wifi_scan(wifi_scan_ap_t *info, uint32_t  size);

#endif /* YUNOS_BSP_WIFI_H */
