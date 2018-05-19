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

#ifndef __ESP_SOFTAP_H__
#define __ESP_SOFTAP_H__

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

/** \defgroup SoftAP_APIs SoftAP APIs
  * @brief ESP8266 Soft-AP APIs
  * @attention To call APIs related to ESP8266 soft-AP has to enable soft-AP mode first (wifi_set_opmode)
  */

/** @addtogroup SoftAP_APIs
  * @{
  */

struct softap_config {
    uint8 ssid[32];         /**< SSID of ESP8266 soft-AP */
    uint8 password[64];     /**< Password of ESP8266 soft-AP */
    uint8 ssid_len;         /**< Length of SSID. If softap_config.ssid_len==0, check the SSID until there is a termination character; otherwise, set the SSID length according to softap_config.ssid_len. */
    uint8 channel;          /**< Channel of ESP8266 soft-AP */
    AUTH_MODE authmode;     /**< Auth mode of ESP8266 soft-AP. Do not support AUTH_WEP in soft-AP mode */
    uint8 ssid_hidden;      /**< Broadcast SSID or not, default 0, broadcast the SSID */
    uint8 max_connection;   /**< Max number of stations allowed to connect in, default 4, max 4 */
    uint16 beacon_interval; /**< Beacon interval, 100 ~ 60000 ms, default 100 */
};

struct station_info {
    STAILQ_ENTRY(station_info)     next;    /**< Information of next AP */

    uint8 bssid[6];                         /**< BSSID of AP */
    struct ip_addr ip;                      /**< IP address of AP */
};

/**
  * @brief  Get the current configuration of the ESP8266 WiFi soft-AP
  *
  * @param  struct softap_config *config : ESP8266 soft-AP configuration
  *
  * @return true  : succeed
  * @return false : fail
  */
bool wifi_softap_get_config(struct softap_config *config);

/**
  * @brief  Get the configuration of the ESP8266 WiFi soft-AP saved in the flash
  *
  * @param  struct softap_config *config : ESP8266 soft-AP configuration
  *
  * @return true  : succeed
  * @return false : fail
  */
bool wifi_softap_get_config_default(struct softap_config *config);

/**
  * @brief     Set the configuration of the WiFi soft-AP and save it to the Flash.
  *
  * @attention 1. This configuration will be saved in flash system parameter area if changed
  * @attention 2. The ESP8266 is limited to only one channel, so when in the soft-AP+station mode,
  *               the soft-AP will adjust its channel automatically to be the same as
  *               the channel of the ESP8266 station.
  *
  * @param     struct softap_config *config : ESP8266 soft-AP configuration
  *
  * @return    true  : succeed
  * @return    false : fail
  */
bool wifi_softap_set_config(struct softap_config *config);

/**
  * @brief     Set the configuration of the WiFi soft-AP; the configuration will
  *            not be saved to the Flash.
  *
  * @attention The ESP8266 is limited to only one channel, so when in the soft-AP+station mode,
  *            the soft-AP will adjust its channel automatically to be the same as
  *            the channel of the ESP8266 station.
  *
  * @param     struct softap_config *config : ESP8266 soft-AP configuration
  *
  * @return    true  : succeed
  * @return    false : fail
  */
bool wifi_softap_set_config_current(struct softap_config *config);

/**
  * @brief     Get the number of stations connected to the ESP8266 soft-AP.
  *
  * @attention The ESP8266 is limited to only one channel, so when in the soft-AP+station mode,
  *            the soft-AP will adjust its channel automatically to be the same as
  *            the channel of the ESP8266 station.
  *
  * @param     null
  *
  * @return    the number of stations connected to the ESP8266 soft-AP
  */
uint8 wifi_softap_get_station_num(void);

/**
  * @brief     Get the information of stations connected to the ESP8266 soft-AP,
  *            including MAC and IP.
  *
  * @attention wifi_softap_get_station_info depends on DHCP, it can only
  *            be used when DHCP is enabled, so it can not get the static IP.
  *
  * @param     null
  *
  * @return    struct station_info* : station information structure
  */
struct station_info *wifi_softap_get_station_info(void);

/**
  * @brief     Free the space occupied by station_info when wifi_softap_get_station_info is called.
  *
  * @attention The ESP8266 is limited to only one channel, so when in the soft-AP+station mode,
  *            the soft-AP will adjust its channel automatically to be the same as
  *            the channel of the ESP8266 station.
  *
  * @param     null
  *
  * @return    null
  */
void wifi_softap_free_station_info(void);

/**
  * @brief     Enable the ESP8266 soft-AP DHCP server.
  *
  * @attention 1. The DHCP is enabled by default.
  * @attention 2. The DHCP and the static IP related API (wifi_set_ip_info) influence
  *               each other, if the DHCP is enabled, the static IP will be disabled;
  *               if the static IP is enabled, the DHCP will be disabled.
  *               It depends on the latest configuration.
  *
  * @param     null
  *
  * @return    true  : succeed
  * @return    false : fail
  */
bool wifi_softap_dhcps_start(void);

/**
  * @brief  Disable the ESP8266 soft-AP DHCP server. The DHCP is enabled by default.
  *
  * @param  null
  *
  * @return true  : succeed
  * @return false : fail
  */
bool wifi_softap_dhcps_stop(void);

/**
  * @brief  Get the ESP8266 soft-AP DHCP server status.
  *
  * @param  null
  *
  * @return enum dhcp_status
  */
enum dhcp_status wifi_softap_dhcps_status(void);

/**
  * @brief     Query the IP range that can be got from the ESP8266 soft-AP DHCP server.
  *
  * @attention This API can only be called during ESP8266 soft-AP DHCP server enabled.
  *
  * @param     struct dhcps_lease *please : IP range of the ESP8266 soft-AP DHCP server.
  *
  * @return    true  : succeed
  * @return    false : fail
  */
bool wifi_softap_get_dhcps_lease(struct dhcps_lease *please);

/**
  * @brief     Set the IP range of the ESP8266 soft-AP DHCP server.
  *
  * @attention 1. The IP range should be in the same sub-net with the ESP8266
  *               soft-AP IP address.
  * @attention 2. This API should only be called when the DHCP server is disabled
  *               (wifi_softap_dhcps_stop).
  * @attention 3. This configuration will only take effect the next time when the
  *               DHCP server is enabled (wifi_softap_dhcps_start).
  *    - If the DHCP server is disabled again, this API should be called to set the IP range.
  *    - Otherwise, when the DHCP server is enabled later, the default IP range will be used.
  *
  * @param     struct dhcps_lease *please : IP range of the ESP8266 soft-AP DHCP server.
  *
  * @return    true  : succeed
  * @return    false : fail
  */
bool wifi_softap_set_dhcps_lease(struct dhcps_lease *please);

/**
  * @brief     Get ESP8266 soft-AP DHCP server lease time.
  *
  * @attention This API can only be called during ESP8266 soft-AP DHCP server enabled.
  *
  * @param     null
  *
  * @return    lease time, uint: minute.
  */
uint32 wifi_softap_get_dhcps_lease_time(void);

/**
  * @brief     Set ESP8266 soft-AP DHCP server lease time, default is 120 minutes.
  *
  * @attention This API can only be called during ESP8266 soft-AP DHCP server enabled.
  *
  * @param     uint32 minute : lease time, uint: minute, range:[1, 2880].
  *
  * @return    true  : succeed
  * @return    false : fail
  */
bool wifi_softap_set_dhcps_lease_time(uint32 minute);

/**
  * @brief     Reset ESP8266 soft-AP DHCP server lease time which is 120 minutes by default.
  *
  * @attention This API can only be called during ESP8266 soft-AP DHCP server enabled.
  *
  * @param     null
  *
  * @return    true  : succeed
  * @return    false : fail
  */
bool wifi_softap_reset_dhcps_lease_time(void);

/**
  * @brief  Set the ESP8266 soft-AP DHCP server option.
  *
  * Example:          
  * <pre> 
  *         uint8 mode = 0;
  *         wifi_softap_set_dhcps_offer_option(OFFER_ROUTER, &mode);
  * </pre>
  *
  * @param  uint8 level : OFFER_ROUTER, set the router option.
  * @param  void* optarg :
  *    - bit0, 0 disable the router information;
  *    - bit0, 1 enable the router information.
  *
  * @return true  : succeed
  * @return false : fail
  */
bool wifi_softap_set_dhcps_offer_option(uint8 level, void *optarg);

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
