/**
 * @file netmgr_wifi.h
 * netmgr Wi-Fi API header file.
 *
 * @version   V1.0
 * @date      2019-11-08
 * @copyright Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef NETMGR_WIFI_H
#define NETMGR_WIFI_H

#include <stdint.h>
#include <stdbool.h>

#if defined(__cplusplus)
extern "C"
{
#endif

/** @addtogroup aos_netmgr netmgr_wifi
 *  Interfaces of Wi-Fi net connection management.
 *
 *  @{
 */

#define MAX_SSID_SIZE  32
#define MAX_PWD_SIZE   64

#ifndef MAX_SSID_LEN
#define MAX_SSID_LEN (MAX_SSID_SIZE+1)
#endif
#ifndef ETH_ALEN
#define ETH_ALEN                        (6)
#endif

#define NETMGR_WIFI_KEY "wifi"

/**
 * netmgr AP configuration struct type
 */
typedef struct {
    char ssid[MAX_SSID_SIZE + 1];
    uint8_t bssid[ETH_ALEN];
    char pwd[MAX_PWD_SIZE + 1];
} netmgr_ap_config_t;

/**
 * autoconfig plugin type
 */
typedef struct autoconfig_plugin_s {
    struct autoconfig_plugin_s *next;
    const char                 *description;

    int  (*autoconfig_start)(void);
    void (*autoconfig_stop)(void);
    void (*config_result_cb)(int result, uint32_t ip);
} autoconfig_plugin_t;

/**
 * auth type
 */
enum NETMGR_AWSS_AUTH_TYPE {
    NETMGR_AWSS_AUTH_TYPE_OPEN,
    NETMGR_AWSS_AUTH_TYPE_SHARED,
    NETMGR_AWSS_AUTH_TYPE_WPAPSK,
    NETMGR_AWSS_AUTH_TYPE_WPA8021X,
    NETMGR_AWSS_AUTH_TYPE_WPA2PSK,
    NETMGR_AWSS_AUTH_TYPE_WPA28021X,
    NETMGR_AWSS_AUTH_TYPE_WPAPSKWPA2PSK,
    NETMGR_AWSS_AUTH_TYPE_MAX = NETMGR_AWSS_AUTH_TYPE_WPAPSKWPA2PSK,
    NETMGR_AWSS_AUTH_TYPE_INVALID = 0xff,
};

/**
 * encry type
 */
enum NETMGR_AWSS_ENC_TYPE {
    NETMGR_AWSS_ENC_TYPE_NONE,
    NETMGR_AWSS_ENC_TYPE_WEP,
    NETMGR_AWSS_ENC_TYPE_TKIP,
    NETMGR_AWSS_ENC_TYPE_AES,
    NETMGR_AWSS_ENC_TYPE_TKIPAES,
    NETMGR_AWSS_ENC_TYPE_MAX = NETMGR_AWSS_ENC_TYPE_TKIPAES,
    NETMGR_AWSS_ENC_TYPE_INVALID = 0xff,
};

/**
 * Netmgr wifi scan result callback.
 *
 * @note: refer to platform_wifi_scan_result_cb_t definition in alink.
 *
 * @param[in]  ssid       AP ssid.
 * @param[in]  bssid      AP bssid.
 * @param[in]  auth       auth type.
 * @param[in]  encry      encry type.
 * @param[in]  channel    channel id.
 * @param[in]  rssi       channel rssi.
 * @param[in]  is_last_ap whether is last AP.
 *
 * @return 0 on success, negative on failure
 */
typedef int (*netmgr_wifi_scan_result_cb_t)(const char ssid[MAX_SSID_LEN],
                                            const uint8_t bssid[ETH_ALEN],
                                            enum NETMGR_AWSS_AUTH_TYPE auth,
                                            enum NETMGR_AWSS_ENC_TYPE encry,
                                            uint8_t channel,
                                            char rssi,
                                            int is_last_ap);

/**
 * Get ip address
 *
 * @param[out] ip ip address.
 *
 * @return none
 */
void wifi_get_ip(char ips[16]);

/**
 * Set AP config
 *
 * @param[in] config AP config.
 *
 * @return 0 on success, negative on failure
 */
int netmgr_set_ap_config(netmgr_ap_config_t *config);

/**
 * Get AP config
 *
 * @param[out] config AP config.
 *
 * @return 0 on success, negative on failure
 */
int netmgr_get_ap_config(netmgr_ap_config_t *config);

/**
 * Clear AP config
 *
 * @return none
 */
void netmgr_clear_ap_config(void);

/**
 * Set smart config plugin
 *
 * @param[in] plugin autoconfig.
 *
 * @return none
 */
void netmgr_set_smart_config(autoconfig_plugin_t *plugin);

/**
 * Register wifi scan result callback
 *
 * @param[in] cb callback.
 *
 * @return none
 */
void netmgr_register_wifi_scan_result_callback(netmgr_wifi_scan_result_cb_t cb);

/**
 * Get whether scan callback finish
 *
 * @return true on finished, false on unfinished.
 */
bool netmgr_get_scan_cb_finished(void);

/**
 * Get ip status
 *
 * @return true on ip ready, false on ip not ready.
 */
bool netmgr_wifi_get_ip_state(void);

/**
 * Reconnect wifi
 *
 * @return none
 */
void netmgr_reconnect_wifi(void);

/**
 * Get ip address
 *
 * @return none
 */
void netmgr_wifi_get_ip(char *ip);

/**
 * Initialize netmgr wifi module
 *
 * @return 0 on success, negative on failure.
 */
int netmgr_wifi_init(void);

/**
 * Deinit netmgr wifi module
 *
 * @return none
 */
void netmgr_wifi_deinit(void);

/**
 * Start netmgr wifi module
 *
 * @param[in] autoconfig whether use autoconfig.
 *
 * @return 0 on success, negative on failure.
 */
int netmgr_wifi_start(bool autoconfig);

/** @} */

#if defined(__cplusplus)
}
#endif

#endif /* NETMGR_WIFI_H */
