/*
 * ESPRSSIF MIT License
 *
 * Copyright (c) 2015 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
 *
 * Permission is hereby granted for use on ESPRESSIF SYSTEMS ESP8266 only, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#ifndef __ESP_STA_H__
#define __ESP_STA_H__

#include "queue.h"

#ifdef __cplusplus
extern "C" {
#endif

/** \defgroup WiFi_APIs WiFi Related APIs
  * @brief WiFi APIs
  */

/** @addtogroup WiFi_APIs
  * @{
  */

/** \defgroup Station_APIs Station APIs
  * @brief     ESP8266 station APIs
  * @attention To call APIs related to ESP8266 station has to enable station mode
  *            first (wifi_set_opmode)
  */

/** @addtogroup Station_APIs
  * @{
  */

struct station_config {
    uint8 ssid[32];         /**< SSID of target AP*/
    uint8 password[64];     /**< password of target AP*/
    uint8 bssid_set;        /**< whether set MAC address of target AP or not. Generally, station_config.bssid_set needs to be 0; and it needs to be 1 only when users need to check the MAC address of the AP.*/
    uint8 bssid[6];         /**< MAC address of target AP*/
};

/**
  * @brief  Get the current configuration of the ESP8266 WiFi station.
  *
  * @param  struct station_config *config : ESP8266 station configuration
  *
  * @return true  : succeed
  * @return false : fail
  */
bool wifi_station_get_config(struct station_config *config);

/**
  * @brief  Get the configuration parameters saved in the Flash of the ESP8266 WiFi station.
  *
  * @param  struct station_config *config : ESP8266 station configuration
  *
  * @return true  : succeed
  * @return false : fail
  */
bool wifi_station_get_config_default(struct station_config *config);

/**
  * @brief     Set the configuration of the ESP8266 station and save it to the Flash.
  *
  * @attention 1. This API can be called only when the ESP8266 station is enabled.
  * @attention 2. If wifi_station_set_config is called in user_init , there is no
  *               need to call wifi_station_connect.
  *               The ESP8266 station will automatically connect to the AP (router)
  *               after the system initialization. Otherwise, wifi_station_connect should be called.
  * @attention 3. Generally, station_config.bssid_set needs to be 0; and it needs
  *               to be 1 only when users need to check the MAC address of the AP.
  * @attention 4. This configuration will be saved in the Flash system parameter area if changed.
  *
  * @param     struct station_config *config : ESP8266 station configuration
  *
  * @return    true  : succeed
  * @return    false : fail
  */
bool wifi_station_set_config(struct station_config *config);

/**
  * @brief     Set the configuration of the ESP8266 station. And the configuration
  *            will not be saved to the Flash.
  *
  * @attention 1. This API can be called only when the ESP8266 station is enabled.
  * @attention 2. If wifi_station_set_config_current is called in user_init , there
  *               is no need to call wifi_station_connect.
  *               The ESP8266 station will automatically connect to the AP (router)
  *               after the system initialization. Otherwise, wifi_station_connect
  *               should be called.
  * @attention 3. Generally, station_config.bssid_set needs to be 0; and it needs
  *               to be 1 only when users need to check the MAC address of the AP.
  *
  * @param     struct station_config *config : ESP8266 station configuration
  *
  * @return    true  : succeed
  * @return    false : fail
  */
bool wifi_station_set_config_current(struct station_config *config);

/**
  * @brief     Connect the ESP8266 WiFi station to the AP.
  *
  * @attention 1. This API should be called when the ESP8266 station is enabled,
  *               and the system initialization is completed. Do not call this API in user_init.
  * @attention 2. If the ESP8266 is connected to an AP, call wifi_station_disconnect to disconnect.
  *
  * @param     null
  *
  * @return    true  : succeed
  * @return    false : fail
  */
bool wifi_station_connect(void);

/**
  * @brief     Disconnect the ESP8266 WiFi station from the AP.
  *
  * @attention This API should be called when the ESP8266 station is enabled,
  *            and the system initialization is completed. Do not call this API in user_init.
  *
  * @param     null
  *
  * @return    true  : succeed
  * @return    false : fail
  */
bool wifi_station_disconnect(void);

struct scan_config {
    uint8 *ssid;            /**< SSID of AP */
    uint8 *bssid;           /**< MAC address of AP */
    uint8 channel;          /**< channel, scan the specific channel */
    uint8 show_hidden;      /**< enable to scan AP whose SSID is hidden */
};

struct bss_info {
    STAILQ_ENTRY(bss_info)     next;    /**< information of next AP */

    uint8 bssid[6];                     /**< MAC address of AP */
    uint8 ssid[32];                     /**< SSID of AP */
    uint8 ssid_len;                     /**< SSID length */
    uint8 channel;                      /**< channel of AP */
    sint8 rssi;                         /**< single strength of AP */
    AUTH_MODE authmode;                 /**< authmode of AP */
    uint8 is_hidden;                    /**< SSID of current AP is hidden or not. */
    sint16 freq_offset;                 /**< frequency offset */
    sint16 freqcal_val;
    uint8 *esp_mesh_ie;
};

/**
  * @brief  Callback function for wifi_station_scan.
  *
  * @param  void *arg : information of APs that are found; save them as linked list;
  *                     refer to struct bss_info
  * @param  STATUS status : status of scanning
  *
  * @return null
  */
typedef void (* scan_done_cb_t)(void *arg, STATUS status);

/**
  * @brief     Scan all available APs.
  *
  * @attention This API should be called when the ESP8266 station is enabled, and
  *            the system initialization is completed. Do not call this API in user_init.
  *
  * @param     struct scan_config *config : configuration of scanning
  * @param     struct scan_done_cb_t cb : callback of scanning
  *
  * @return    true  : succeed
  * @return    false : fail
  */
bool wifi_station_scan(struct scan_config *config, scan_done_cb_t cb);

/**
  * @brief  Check if the ESP8266 station will connect to the recorded AP automatically
  *         when the power is on.
  *
  * @param  null
  *
  * @return true  : connect to the AP automatically
  * @return false : not connect to the AP automatically
  */
bool wifi_station_get_auto_connect(void);

/**
  * @brief     Set whether the ESP8266 station will connect to the recorded AP
  *            automatically when the power is on. It will do so by default.
  *
  * @attention 1. If this API is called in user_init, it is effective immediately
  *               after the power is on. If it is called in other places, it will
  *               be effective the next time when the power is on.
  * @attention 2. This configuration will be saved in Flash system parameter area if changed.
  *
  * @param     bool set : If it will automatically connect to the AP when the power is on
  *    - true : it will connect automatically
  *    - false: it will not connect automatically
  *
  * @return    true  : succeed
  * @return    false : fail
  */
bool wifi_station_set_auto_connect(bool set);

/**
  * @brief  Check whether the ESP8266 station will reconnect to the AP after disconnection.
  *
  * @param  null
  *
  * @return true  : succeed
  * @return false : fail
  */
bool wifi_station_get_reconnect_policy(void);

/**
  * @brief     Set whether the ESP8266 station will reconnect to the AP after disconnection.
  *            It will do so by default.
  *
  * @attention If users want to call this API, it is suggested that users call this API in user_init.
  *
  * @param     bool set : if it's true, it will enable reconnection; if it's false,
  *                       it will disable reconnection.
  *
  * @return    true  : succeed
  * @return    false : fail
  */
bool wifi_station_set_reconnect_policy(bool set);

typedef enum {
    STATION_IDLE = 0,        /**< ESP8266 station idle */
    STATION_CONNECTING,      /**< ESP8266 station is connecting to AP*/
    STATION_WRONG_PASSWORD,  /**< the password is wrong*/
    STATION_NO_AP_FOUND,     /**< ESP8266 station can not find the target AP*/
    STATION_CONNECT_FAIL,    /**< ESP8266 station fail to connect to AP*/
    STATION_GOT_IP           /**< ESP8266 station got IP address from AP*/
} STATION_STATUS;

/**
  * @brief  Get the connection status of the ESP8266 WiFi station.
  *
  * @param  null
  *
  * @return the status of connection
  */
STATION_STATUS wifi_station_get_connect_status(void);

/**
  * @brief  Get the information of APs (5 at most) recorded by ESP8266 station.
  *
  * @param  struct station_config config[] : information of the APs, the array size should be 5.
  *
  * @return The number of APs recorded.
  */
uint8 wifi_station_get_current_ap_id(void);

/**
  * @brief  Switch the ESP8266 station connection to a recorded AP.
  *
  * @param  uint8 new_ap_id : AP's record id, start counting from 0.
  *
  * @return true  : succeed
  * @return false : fail
  */
bool wifi_station_ap_change(uint8 current_ap_id);

/**
  * @brief     Set the number of APs that can be recorded in the ESP8266 station.
  *            When the ESP8266 station is connected to an AP, the SSID and password
  *            of the AP will be recorded.
  *
  * @attention This configuration will be saved in the Flash system parameter area if changed.
  *
  * @param     uint8 ap_number : the number of APs that can be recorded (MAX: 5)
  *
  * @return    true  : succeed
  * @return    false : fail
  */
bool wifi_station_ap_number_set(uint8 ap_number);

/**
  * @brief  Get the information of APs (5 at most) recorded by ESP8266 station.
  *
  * Example:          
  * <pre> 
  *         struct station_config config[5];
  *         nt i = wifi_station_get_ap_info(config);
  * </pre>
  *
  * @param  struct station_config config[] : information of the APs, the array size should be 5.
  *
  * @return The number of APs recorded.
  */
uint8 wifi_station_get_ap_info(struct station_config config[]);

/**
  * @brief  Get rssi of the AP which ESP8266 station connected to.
  *
  * @param  null
  *
  * @return 31 : fail, invalid value.
  * @return others : succeed, value of rssi. In general, rssi value < 10
  */
sint8 wifi_station_get_rssi(void);

/**
  * @brief     Enable the ESP8266 station DHCP client.
  *
  * @attention 1. The DHCP is enabled by default.
  * @attention 2. The DHCP and the static IP API ((wifi_set_ip_info)) influence each other,
  *               and if the DHCP is enabled, the static IP will be disabled;
  *               if the static IP is enabled, the DHCP will be disabled.
  *               It depends on the latest configuration.
  *
  * @param  null
  *
  * @return true  : succeed
  * @return false : fail
  */
bool wifi_station_dhcpc_start(void);

/**
  * @brief     Disable the ESP8266 station DHCP client.
  *
  * @attention 1. The DHCP is enabled by default.
  * @attention 2. The DHCP and the static IP API ((wifi_set_ip_info)) influence each other,
  *               and if the DHCP is enabled, the static IP will be disabled;
  *               if the static IP is enabled, the DHCP will be disabled.
  *               It depends on the latest configuration.
  *
  * @param     null
  *
  * @return    true  : succeed
  * @return    false : fail
  */
bool wifi_station_dhcpc_stop(void);

/**
  * @brief   Get the ESP8266 station DHCP client status.
  *
  * @param  null
  *
  * @return  enum dhcp_status
  */
enum dhcp_status wifi_station_dhcpc_status(void);

/**
  * @brief    Set ESP8266 station DHCP hostname.
  *
  * @param  char *name : hostname of ESP8266 station
  *
  * @return    true  : succeed
  * @return    false : fail
  */
bool wifi_station_set_hostname(char *name);

/**
  * @brief  Get ESP8266 station DHCP hostname.
  *
  * @param  null
  *
  * @return the hostname of ESP8266 station
  */
char* wifi_station_get_hostname(void);

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif
