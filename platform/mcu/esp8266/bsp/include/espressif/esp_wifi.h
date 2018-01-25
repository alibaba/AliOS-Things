/*
 * ESPRESSIF MIT License
 *
 * Copyright (c) 2015-2017 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
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

#ifndef __ESP_WIFI_H__
#define __ESP_WIFI_H__

#ifdef __cplusplus
extern "C" {
#endif

/** \defgroup WiFi_APIs WiFi Related APIs
  * @brief WiFi APIs
  */

/** @addtogroup WiFi_APIs
  * @{
  */

/** \defgroup WiFi_Common_APIs Common APIs
  * @brief WiFi common APIs
  *
  *        The Flash system parameter area is the last 16KB of the Flash.
  *
  */

/** @addtogroup WiFi_Common_APIs
  * @{
  */

typedef enum {
    NULL_MODE = 0,      /**< null mode */
    STATION_MODE,       /**< WiFi station mode */
    SOFTAP_MODE,        /**< WiFi soft-AP mode */
    STATIONAP_MODE,     /**< WiFi station + soft-AP mode */
    MAX_MODE
} WIFI_MODE;

typedef enum {
    AUTH_OPEN = 0,      /**< authenticate mode : open */
    AUTH_WEP,           /**< authenticate mode : WEP */
    AUTH_WPA_PSK,       /**< authenticate mode : WPA_PSK */
    AUTH_WPA2_PSK,      /**< authenticate mode : WPA2_PSK */
    AUTH_WPA_WPA2_PSK,  /**< authenticate mode : WPA_WPA2_PSK */
    AUTH_MAX
} AUTH_MODE;

/**
  * @brief  Get the current operating mode of the WiFi.
  *
  * @param  null
  *
  * @return WiFi operating modes:
  *    - 0x01: station mode;
  *    - 0x02: soft-AP mode
  *    - 0x03: station+soft-AP mode
  */
WIFI_MODE wifi_get_opmode(void);

/**
  * @brief  Get the operating mode of the WiFi saved in the Flash.
  *
  * @param  null
  *
  * @return WiFi operating modes:
  *    - 0x01: station mode;
  *    - 0x02: soft-AP mode
  *    - 0x03: station+soft-AP mode
  */
WIFI_MODE wifi_get_opmode_default(void);

/**
  * @brief     Set the WiFi operating mode, and save it to Flash.
  *
  *            Set the WiFi operating mode as station, soft-AP or station+soft-AP,
  *            and save it to Flash. The default mode is soft-AP mode.
  *
  * @attention This configuration will be saved in the Flash system parameter area if changed.
  *
  * @param     uint8 opmode : WiFi operating modes:
  *    - 0x01: station mode;
  *    - 0x02: soft-AP mode
  *    - 0x03: station+soft-AP mode
  *
  * @return    true  : succeed
  * @return    false : fail
  */
bool wifi_set_opmode(WIFI_MODE opmode);

/**
  * @brief  Set the WiFi operating mode, and will not save it to Flash.
  *
  *         Set the WiFi operating mode as station, soft-AP or station+soft-AP, and
  *         the mode won't be saved to the Flash.
  *
  * @param  uint8 opmode : WiFi operating modes:
  *    - 0x01: station mode;
  *    - 0x02: soft-AP mode
  *    - 0x03: station+soft-AP mode
  *
  * @return true  : succeed
  * @return false : fail
  */
bool wifi_set_opmode_current(WIFI_MODE opmode);

typedef enum {
    STATION_IF = 0, /**< ESP8266 station interface */
    SOFTAP_IF,      /**< ESP8266 soft-AP interface */
    MAX_IF
} WIFI_INTERFACE;

struct ip_info {
    struct ip_addr ip;      /**< IP address */
    struct ip_addr netmask; /**< netmask */
    struct ip_addr gw;      /**< gateway */
};

/**
  * @brief     Get the IP address of the ESP8266 WiFi station or the soft-AP interface.
  *
  * @attention Users need to enable the target interface (station or soft-AP) by wifi_set_opmode first.
  *
  * @param     WIFI_INTERFACE if_index : get the IP address of the station or the soft-AP interface,
  *                                      0x00 for STATION_IF, 0x01 for SOFTAP_IF.
  * @param     struct ip_info *info : the IP information obtained.
  *
  * @return    true  : succeed
  * @return    false : fail
  */
bool wifi_get_ip_info(WIFI_INTERFACE if_index, struct ip_info *info);

/**
  * @brief     Set the IP address of the ESP8266 WiFi station or the soft-AP interface.
  *
  * @attention 1. Users need to enable the target interface (station or soft-AP) by
  *               wifi_set_opmode first.
  * @attention 2. To set static IP, users need to disable DHCP first (wifi_station_dhcpc_stop
  *               or wifi_softap_dhcps_stop):
  *    - If the DHCP is enabled, the static IP will be disabled; if the static IP is enabled,
  *      the DHCP will be disabled. It depends on the latest configuration.
  *
  * @param  WIFI_INTERFACE if_index : get the IP address of the station or the soft-AP interface,
  *                                   0x00 for STATION_IF, 0x01 for SOFTAP_IF.
  * @param  struct ip_info *info : the IP information obtained.
  *
  * @return true  : succeed
  * @return false : fail
  */
bool wifi_set_ip_info(WIFI_INTERFACE if_index, struct ip_info *info);

/**
  * @brief  Get MAC address of the ESP8266 WiFi station or the soft-AP interface.
  *
  * @param  WIFI_INTERFACE if_index : get the IP address of the station or the soft-AP interface,
  *                                   0x00 for STATION_IF, 0x01 for SOFTAP_IF.
  * @param  uint8 *macaddr : the MAC address.
  *
  * @return true  : succeed
  * @return false : fail
  */
bool wifi_get_macaddr(WIFI_INTERFACE if_index, uint8 *macaddr);

/**
  * @brief     Set MAC address of the ESP8266 WiFi station or the soft-AP interface.
  *
  * @attention 1. This API can only be called in user_init.
  * @attention 2. Users need to enable the target interface (station or soft-AP) by wifi_set_opmode first.
  * @attention 3. ESP8266 soft-AP and station have different MAC addresses, do not set them to be the same.
  *    - The bit0 of the first byte of ESP8266 MAC address can not be 1. For example, the MAC address
  *      can set to be "1a:XX:XX:XX:XX:XX", but can not be "15:XX:XX:XX:XX:XX".
  *
  * @param     WIFI_INTERFACE if_index : get the IP address of the station or the soft-AP interface,
  *                                   0x00 for STATION_IF, 0x01 for SOFTAP_IF.
  * @param     uint8 *macaddr : the MAC address.
  *
  * @return    true  : succeed
  * @return    false : fail
  */
bool wifi_set_macaddr(WIFI_INTERFACE if_index, uint8 *macaddr);

/**
  * @brief  Install the WiFi status LED.
  *
  * @param  uint8 gpio_id : GPIO ID
  * @param  uint8 gpio_name : GPIO mux name
  * @param  uint8 gpio_func : GPIO function
  *
  * @return null
  */
void wifi_status_led_install(uint8 gpio_id, uint32 gpio_name, uint8 gpio_func);

/**
  * @brief  Uninstall the WiFi status LED.
  *
  * @param null
  *
  * @return null
  */
void wifi_status_led_uninstall(void);

typedef enum {
    PHY_MODE_11B    = 1,    /**<  802.11b */
    PHY_MODE_11G    = 2,    /**<  802.11g */
    PHY_MODE_11N    = 3     /**<  802.11n */
} WIFI_PHY_MODE;

/**
  * @brief  Get the ESP8266 physical mode (802.11b/g/n).
  *
  * @param  null
  *
  * @return enum WIFI_PHY_MODE
  */
WIFI_PHY_MODE wifi_get_phy_mode(void);

/**
  * @brief     Set the ESP8266 physical mode (802.11b/g/n).
  *
  * @attention The ESP8266 soft-AP only supports bg.
  *
  * @param     WIFI_PHY_MODE mode : physical mode
  *
  * @return    true  : succeed
  * @return    false : fail
  */
bool wifi_set_phy_mode(WIFI_PHY_MODE mode);

typedef enum {
    EVENT_STAMODE_SCAN_DONE = 0,        /**< ESP8266 station finish scanning AP */
    EVENT_STAMODE_CONNECTED,            /**< ESP8266 station connected to AP */
    EVENT_STAMODE_DISCONNECTED,         /**< ESP8266 station disconnected to AP */
    EVENT_STAMODE_AUTHMODE_CHANGE,      /**< the auth mode of AP connected by ESP8266 station changed */
    EVENT_STAMODE_GOT_IP,               /**< ESP8266 station got IP from connected AP */
    EVENT_STAMODE_DHCP_TIMEOUT,         /**< ESP8266 station dhcp client got IP timeout */
    EVENT_SOFTAPMODE_STACONNECTED,      /**< a station connected to ESP8266 soft-AP */
    EVENT_SOFTAPMODE_STADISCONNECTED,   /**< a station disconnected to ESP8266 soft-AP */
    EVENT_SOFTAPMODE_PROBEREQRECVED,    /**< Receive probe request packet in soft-AP interface */
    EVENT_MAX
} SYSTEM_EVENT;

enum {
    REASON_UNSPECIFIED              = 1,
    REASON_AUTH_EXPIRE              = 2,
    REASON_AUTH_LEAVE               = 3,
    REASON_ASSOC_EXPIRE             = 4,
    REASON_ASSOC_TOOMANY            = 5,
    REASON_NOT_AUTHED               = 6,
    REASON_NOT_ASSOCED              = 7,
    REASON_ASSOC_LEAVE              = 8,
    REASON_ASSOC_NOT_AUTHED         = 9,
    REASON_DISASSOC_PWRCAP_BAD      = 10,
    REASON_DISASSOC_SUPCHAN_BAD     = 11,
    REASON_IE_INVALID               = 13,
    REASON_MIC_FAILURE              = 14,
    REASON_4WAY_HANDSHAKE_TIMEOUT   = 15,
    REASON_GROUP_KEY_UPDATE_TIMEOUT = 16,
    REASON_IE_IN_4WAY_DIFFERS       = 17,
    REASON_GROUP_CIPHER_INVALID     = 18,
    REASON_PAIRWISE_CIPHER_INVALID  = 19,
    REASON_AKMP_INVALID             = 20,
    REASON_UNSUPP_RSN_IE_VERSION    = 21,
    REASON_INVALID_RSN_IE_CAP       = 22,
    REASON_802_1X_AUTH_FAILED       = 23,
    REASON_CIPHER_SUITE_REJECTED    = 24,

    REASON_BEACON_TIMEOUT           = 200,
    REASON_NO_AP_FOUND              = 201,
    REASON_AUTH_FAIL                = 202,
    REASON_ASSOC_FAIL               = 203,
    REASON_HANDSHAKE_TIMEOUT        = 204,
};

typedef struct {
    uint32 status;          /**< status of scanning APs*/
    struct bss_info *bss;   /**< list of APs found*/
} Event_StaMode_ScanDone_t;

typedef struct {
    uint8 ssid[32];         /**< SSID of connected AP */
    uint8 ssid_len;         /**< SSID length of connected AP */
    uint8 bssid[6];         /**< BSSID of connected AP*/
    uint8 channel;          /**< channel of connected AP*/
} Event_StaMode_Connected_t;

typedef struct {
    uint8 ssid[32];         /**< SSID of disconnected AP */
    uint8 ssid_len;         /**< SSID length of disconnected AP */
    uint8 bssid[6];         /**< BSSID of disconnected AP */
    uint8 reason;           /**< reason of disconnection */
} Event_StaMode_Disconnected_t;

typedef struct {
    uint8 old_mode;         /**< the old auth mode of AP */
    uint8 new_mode;         /**< the new auth mode of AP */
} Event_StaMode_AuthMode_Change_t;

typedef struct {
    struct ip_addr ip;      /**< IP address that ESP8266 station got from connected AP */
    struct ip_addr mask;    /**< netmask that ESP8266 station got from connected AP */
    struct ip_addr gw;      /**< gateway that ESP8266 station got from connected AP */
} Event_StaMode_Got_IP_t;

typedef struct {
    uint8 mac[6];           /**< MAC address of the station connected to ESP8266 soft-AP */
    uint8 aid;              /**< the aid that ESP8266 soft-AP gives to the station connected to  */
} Event_SoftAPMode_StaConnected_t;

typedef struct {
    uint8 mac[6];           /**< MAC address of the station disconnects to ESP8266 soft-AP */
    uint8 aid;              /**< the aid that ESP8266 soft-AP gave to the station disconnects to  */
} Event_SoftAPMode_StaDisconnected_t;

typedef struct {
    int rssi;               /**< Received probe request signal strength */
    uint8 mac[6];           /**< MAC address of the station which send probe request */
} Event_SoftAPMode_ProbeReqRecved_t;

typedef union {
    Event_StaMode_ScanDone_t            scan_done;          /**< ESP8266 station scan (APs) done */
    Event_StaMode_Connected_t           connected;          /**< ESP8266 station connected to AP */
    Event_StaMode_Disconnected_t        disconnected;       /**< ESP8266 station disconnected to AP */
    Event_StaMode_AuthMode_Change_t     auth_change;        /**< the auth mode of AP ESP8266 station connected to changed */
    Event_StaMode_Got_IP_t              got_ip;             /**< ESP8266 station got IP */
    Event_SoftAPMode_StaConnected_t     sta_connected;      /**< a station connected to ESP8266 soft-AP */
    Event_SoftAPMode_StaDisconnected_t  sta_disconnected;   /**< a station disconnected to ESP8266 soft-AP */
    Event_SoftAPMode_ProbeReqRecved_t   ap_probereqrecved;  /**< ESP8266 softAP receive probe request packet */
} Event_Info_u;

typedef struct _esp_event {
    SYSTEM_EVENT event_id;      /**< even ID */
    Event_Info_u event_info;    /**< event information */
} System_Event_t;

/**
  * @brief      The Wi-Fi event handler.
  *
  * @attention  No complex operations are allowed in callback. 
  *             If users want to execute any complex operations, please post message to another task instead.  
  *
  * @param      System_Event_t *event : WiFi event
  *
  * @return     null
  */
typedef void (* wifi_event_handler_cb_t)(System_Event_t *event);

/**
  * @brief  Register the Wi-Fi event handler.
  *
  * @param  wifi_event_handler_cb_t cb : callback function
  *
  * @return true  : succeed
  * @return false : fail
  */
bool wifi_set_event_handler_cb(wifi_event_handler_cb_t cb);

/**
  * @brief  Callback of sending user-define 802.11 packets.
  *
  * @param  uint8 status : 0, packet sending succeed; otherwise, fail.
  *
  * @return null
  */
typedef void (*freedom_outside_cb_t)(uint8 status);

/**
  * @brief      Register a callback for sending user-define 802.11 packets.
  *
  * @attention  Only after the previous packet was sent, entered the freedom_outside_cb_t,
  *             the next packet is allowed to send.
  *
  * @param      freedom_outside_cb_t cb : sent callback
  *
  * @return     0, succeed;
  * @return    -1, fail.
  */
sint32 wifi_register_send_pkt_freedom_cb(freedom_outside_cb_t cb);

/**
  * @brief  Unregister the callback for sending user-define 802.11 packets.
  *
  * @param  null
  *
  * @return null
  */
void wifi_unregister_send_pkt_freedom_cb(void);

/**
  * @brief     Send user-define 802.11 packets.
  *
  * @attention 1. Packet has to be the whole 802.11 packet, does not include the FCS.
  *               The length of the packet has to be longer than the minimum length
  *               of the header of 802.11 packet which is 24 bytes, and less than 1400 bytes.
  * @attention 2. Duration area is invalid for user, it will be filled in SDK.
  * @attention 3. The rate of sending packet is same as the management packet which
  *               is the same as the system rate of sending packets.
  * @attention 4. Only after the previous packet was sent, entered the sent callback,
  *               the next packet is allowed to send. Otherwise, wifi_send_pkt_freedom
  *               will return fail.
  *
  * @param     uint8 *buf : pointer of packet
  * @param     uint16 len : packet length
  * @param     bool sys_seq : follow the system's 802.11 packets sequence number or not,
  *                           if it is true, the sequence number will be increased 1 every
  *                           time a packet sent.
  *
  * @return    0, succeed;
  * @return   -1, fail.
  */
sint32 wifi_send_pkt_freedom(uint8 *buf, uint16 len, bool sys_seq);

/**
  * @brief  Enable RFID LOCP (Location Control Protocol) to receive WDS packets.
  *
  * @param  null
  *
  * @return 0, succeed;
  * @return otherwise, fail.
  */
sint32 wifi_rfid_locp_recv_open(void);

/**
  * @brief  Disable RFID LOCP (Location Control Protocol) .
  *
  * @param  null
  *
  * @return null
  */
void wifi_rfid_locp_recv_close(void);

/**
  * @brief  RFID LOCP (Location Control Protocol) receive callback .
  *
  * @param  uint8 *frm : point to the head of 802.11 packet
  * @param  int len : packet length
  * @param  int rssi : signal strength
  *
  * @return null
  */
typedef void (*rfid_locp_cb_t)(uint8 *frm, int len, sint8 rssi);

/**
  * @brief  Register a callback of receiving WDS packets.
  *
  *         Register a callback of receiving WDS packets. Only if the first MAC
  *         address of the WDS packet is a multicast address.
  *
  * @param  rfid_locp_cb_t cb : callback
  *
  * @return 0, succeed;
  * @return otherwise, fail.
  */
sint32 wifi_register_rfid_locp_recv_cb(rfid_locp_cb_t cb);

/**
  * @brief  Unregister the callback of receiving WDS packets.
  *
  * @param  null
  *
  * @return null
  */
void wifi_unregister_rfid_locp_recv_cb(void);

typedef enum {
    NONE_SLEEP_T    = 0,
    LIGHT_SLEEP_T,
    MODEM_SLEEP_T
} sleep_type;

/**
  * @brief     Sets sleep type.
  *
  *            Set NONE_SLEEP_T to disable sleep. Default to be Modem sleep.
  *
  * @attention Sleep function only takes effect in station-only mode.
  *
  * @param     sleep_type type : sleep type
  *
  * @return    true  : succeed
  * @return    false : fail
  */
bool wifi_set_sleep_type(sleep_type type);

/**
  * @brief  Gets sleep type.
  *
  * @param  null
  *
  * @return sleep type
  */
sleep_type wifi_get_sleep_type(void);

/**
  * @}
  */


/** \defgroup WiFi_Force_Sleep_APIs Force Sleep APIs
  * @brief WiFi Force Sleep APIs
  */

/** @addtogroup WiFi_Force_Sleep_APIs
  * @{
  */

/**
  * @brief     Enable force sleep function.
  *
  * @attention Force sleep function is disabled by default.
  *
  * @param     null
  *
  * @return    null
  */
void wifi_fpm_open(void);

/**
  * @brief  Disable force sleep function.
  *
  * @param  null
  *
  * @return null
  */
void wifi_fpm_close(void);

/**
  * @brief     Wake ESP8266 up from MODEM_SLEEP_T force sleep.
  *
  * @attention This API can only be called when MODEM_SLEEP_T force sleep function
  *            is enabled, after calling wifi_fpm_open.
  *            This API can not be called after calling wifi_fpm_close.
  *
  * @param     null
  *
  * @return    null
  */
void wifi_fpm_do_wakeup(void);

typedef void (*fpm_wakeup_cb)(void);

/**
  * @brief     Set a callback of waken up from force sleep because of time out.
  *
  * @attention 1. This API can only be called when force sleep function is enabled,
  *               after calling wifi_fpm_open. This API can not be called after calling
  *               wifi_fpm_close.
  * @attention 2. fpm_wakeup_cb_func will be called after system woke up only if the
  *               force sleep time out (wifi_fpm_do_sleep and the parameter is not 0xFFFFFFF).
  * @attention 3. fpm_wakeup_cb_func will not be called if woke up by wifi_fpm_do_wakeup
  *               from MODEM_SLEEP_T type force sleep.
  *
  * @param     void (*fpm_wakeup_cb_func)(void) : callback of waken up
  *
  * @return    null
  */
void wifi_fpm_set_wakeup_cb(fpm_wakeup_cb cb);

/**
  * @brief     Force ESP8266 enter sleep mode, and it will wake up automatically when time out.
  *
  * @attention 1. This API can only be called when force sleep function is enabled, after
  *               calling wifi_fpm_open. This API can not be called after calling wifi_fpm_close.
  * @attention 2. If this API returned 0 means that the configuration is set successfully,
  *               but the ESP8266 will not enter sleep mode immediately, it is going to sleep
  *               in the system idle task. Please do not call other WiFi related function right
  *               after calling this API.
  *
  * @param     uint32 sleep_time_in_us : sleep time, ESP8266 will wake up automatically
  *                                      when time out. Unit: us. Range: 10000 ~ 268435455(0xFFFFFFF).
  *    - If sleep_time_in_us is 0xFFFFFFF, the ESP8266 will sleep till
  *    - if wifi_fpm_set_sleep_type is set to be LIGHT_SLEEP_T, ESP8266 can wake up by GPIO.
  *    - if wifi_fpm_set_sleep_type is set to be MODEM_SLEEP_T, ESP8266 can wake up by wifi_fpm_do_wakeup.
  *
  * @return   0, setting succeed;
  * @return  -1, fail to sleep, sleep status error;
  * @return  -2, fail to sleep, force sleep function is not enabled.
  */
sint8 wifi_fpm_do_sleep(uint32 sleep_time_in_us);

/**
  * @brief     Set sleep type for force sleep function.
  *
  * @attention This API can only be called before wifi_fpm_open.
  *
  * @param     sleep_type type : sleep type
  *
  * @return    null
  */
void wifi_fpm_set_sleep_type(sleep_type type);

/**
  * @brief  Get sleep type of force sleep function.
  *
  * @param  null
  *
  * @return sleep type
  */
sleep_type wifi_fpm_get_sleep_type(void);

/**
  * @}
  */

/** \defgroup WiFi_Rate_Control_APIs Rate Control APIs
  * @brief WiFi Rate Control APIs
  */

/** @addtogroup WiFi_Rate_Control_APIs
  * @{
  */

enum FIXED_RATE {
    PHY_RATE_48       = 0x8,
    PHY_RATE_24       = 0x9,
    PHY_RATE_12       = 0xA,
    PHY_RATE_6        = 0xB,
    PHY_RATE_54       = 0xC,
    PHY_RATE_36       = 0xD,
    PHY_RATE_18       = 0xE,
    PHY_RATE_9        = 0xF
};

#define FIXED_RATE_MASK_NONE    0x00
#define FIXED_RATE_MASK_STA     0x01
#define FIXED_RATE_MASK_AP      0x02
#define FIXED_RATE_MASK_ALL     0x03

/**
  * @brief     Set the fixed rate and mask of sending data from ESP8266.
  *
  * @attention 1. Only if the corresponding bit in enable_mask is 1, ESP8266 station
  *               or soft-AP will send data in the fixed rate.
  * @attention 2. If the enable_mask is 0, both ESP8266 station and soft-AP will not
  *               send data in the fixed rate.
  * @attention 3. ESP8266 station and soft-AP share the same rate, they can not be
  *               set into the different rate.
  *
  * @param     uint8 enable_mask : 0x00 - disable the fixed rate
  *    -  0x01 - use the fixed rate on ESP8266 station
  *    -  0x02 - use the fixed rate on ESP8266 soft-AP
  *    -  0x03 - use the fixed rate on ESP8266 station and soft-AP
  * @param     uint8 rate  : value of the fixed rate
  *
  * @return    0         : succeed
  * @return    otherwise : fail
  */
sint32 wifi_set_user_fixed_rate(uint8 enable_mask, uint8 rate);

/**
  * @brief  Get the fixed rate and mask of ESP8266.
  *
  * @param  uint8 *enable_mask : pointer of the enable_mask
  * @param  uint8 *rate : pointer of the fixed rate
  *
  * @return 0  : succeed
  * @return otherwise : fail
  */
int wifi_get_user_fixed_rate(uint8 *enable_mask, uint8 *rate);

enum support_rate {
    RATE_11B5M  = 0,
    RATE_11B11M = 1,
    RATE_11B1M  = 2,
    RATE_11B2M  = 3,
    RATE_11G6M  = 4,
    RATE_11G12M = 5,
    RATE_11G24M = 6,
    RATE_11G48M = 7,
    RATE_11G54M = 8,
    RATE_11G9M  = 9,
    RATE_11G18M = 10,
    RATE_11G36M = 11
};

/**
  * @brief     Set the support rate of ESP8266.
  *
  *            Set the rate range in the IE of support rate in ESP8266's beacon,
  *            probe req/resp and other packets.
  *            Tell other devices about the rate range supported by ESP8266 to limit
  *            the rate of sending packets from other devices.
  *            Example : wifi_set_user_sup_rate(RATE_11G6M, RATE_11G24M);
  *
  * @attention This API can only support 802.11g now, but it will support 802.11b in next version.
  *
  * @param     uint8 min : the minimum value of the support rate, according to enum support_rate.
  * @param     uint8 max : the maximum value of the support rate, according to enum support_rate.
  *
  * @return    0         : succeed
  * @return    otherwise : fail
  */
sint32 wifi_set_user_sup_rate(uint8 min, uint8 max);

enum RATE_11B_ID {
    RATE_11B_B11M   = 0,
    RATE_11B_B5M    = 1,
    RATE_11B_B2M    = 2,
    RATE_11B_B1M    = 3
};

enum RATE_11G_ID {
    RATE_11G_G54M   = 0,
    RATE_11G_G48M   = 1,
    RATE_11G_G36M   = 2,
    RATE_11G_G24M   = 3,
    RATE_11G_G18M   = 4,
    RATE_11G_G12M   = 5,
    RATE_11G_G9M    = 6,
    RATE_11G_G6M    = 7,
    RATE_11G_B5M    = 8,
    RATE_11G_B2M    = 9,
    RATE_11G_B1M    = 10
};

enum RATE_11N_ID {
    RATE_11N_MCS7S  = 0,
    RATE_11N_MCS7   = 1,
    RATE_11N_MCS6   = 2,
    RATE_11N_MCS5   = 3,
    RATE_11N_MCS4   = 4,
    RATE_11N_MCS3   = 5,
    RATE_11N_MCS2   = 6,
    RATE_11N_MCS1   = 7,
    RATE_11N_MCS0   = 8,
    RATE_11N_B5M    = 9,
    RATE_11N_B2M    = 10,
    RATE_11N_B1M    = 11
};

#define RC_LIMIT_11B        0
#define RC_LIMIT_11G        1
#define RC_LIMIT_11N        2
#define RC_LIMIT_P2P_11G    3
#define RC_LIMIT_P2P_11N    4
#define RC_LIMIT_NUM        5

#define LIMIT_RATE_MASK_NONE    0x00
#define LIMIT_RATE_MASK_STA     0x01
#define LIMIT_RATE_MASK_AP      0x02
#define LIMIT_RATE_MASK_ALL     0x03

/**
  * @brief     Limit the initial rate of sending data from ESP8266.
  *
  *            Example:
  *                wifi_set_user_rate_limit(RC_LIMIT_11G, 0, RATE_11G_G18M, RATE_11G_G6M);
  *
  * @attention The rate of retransmission is not limited by this API.
  *
  * @param     uint8 mode :  WiFi mode
  *    -  #define RC_LIMIT_11B  0
  *    -  #define RC_LIMIT_11G  1
  *    -  #define RC_LIMIT_11N  2
  * @param     uint8 ifidx : interface of ESP8266
  *    -  0x00 - ESP8266 station
  *    -  0x01 - ESP8266 soft-AP
  * @param     uint8 max : the maximum value of the rate, according to the enum rate
  *                        corresponding to the first parameter mode.
  * @param     uint8 min : the minimum value of the rate, according to the enum rate
  *                        corresponding to the first parameter mode.
  *
  * @return    0         : succeed
  * @return    otherwise : fail
  */
bool wifi_set_user_rate_limit(uint8 mode, uint8 ifidx, uint8 max, uint8 min);

/**
  * @brief  Get the interfaces of ESP8266 whose rate of sending data is limited by
  *         wifi_set_user_rate_limit.
  *
  * @param  null
  *
  * @return LIMIT_RATE_MASK_NONE - disable the limitation on both ESP8266 station and soft-AP
  * @return LIMIT_RATE_MASK_STA  - enable the limitation on ESP8266 station
  * @return LIMIT_RATE_MASK_AP   - enable the limitation on ESP8266 soft-AP
  * @return LIMIT_RATE_MASK_ALL  - enable the limitation on both ESP8266 station and soft-AP
  */
uint8 wifi_get_user_limit_rate_mask(void);

/**
  * @brief  Set the interfaces of ESP8266 whose rate of sending packets is limited by
  *         wifi_set_user_rate_limit.
  *
  * @param  uint8 enable_mask :
  *    -  LIMIT_RATE_MASK_NONE - disable the limitation on both ESP8266 station and soft-AP
  *    -  LIMIT_RATE_MASK_STA  - enable the limitation on ESP8266 station
  *    -  LIMIT_RATE_MASK_AP   - enable the limitation on ESP8266 soft-AP
  *    -  LIMIT_RATE_MASK_ALL  - enable the limitation on both ESP8266 station and soft-AP
  *
  * @return true  : succeed
  * @return false : fail
  */
bool wifi_set_user_limit_rate_mask(uint8 enable_mask);

/**
  * @}
  */

/** \defgroup WiFi_Vendor_IE_APIs Vendor IE APIs
  * @brief WiFi Vendor IE APIs
  */

/** @addtogroup WiFi_Vendor_IE_APIs
  * @{
  */

typedef enum {
    VND_IE_TYPE_BEACON = 0,  /**< beacon */
    VND_IE_TYPE_PROBE_REQ,   /**< probe request */
    VND_IE_TYPE_PROBE_RESP,  /**< probe response */
    VND_IE_TYPE_ASSOC_REQ,   /**< associate request */
    VND_IE_TYPE_ASSOC_RESP,  /**< associate response */
    VND_IE_TYPE_NUM,
} vendor_ie_type;

/**
  * @brief  Vendor IE received callback.
  *
  * @param  vendor_ie_type type :  type of vendor IE.
  * @param  const uint8 sa[6] : source address of the packet.
  * @param  uint8 *vendor_ie : pointer of vendor IE.
  * @param  sint32 rssi : signal strength.
  *
  * @return null
  */
typedef void (*vendor_ie_recv_cb_t)(vendor_ie_type type, const uint8 sa[6], const uint8 *vnd_ie, sint32 rssi);

/**
  * @brief  Set Vendor IE of ESP8266.
  *
  *         The Vendor IE will be added to the target packets of vendor_ie_type.
  *
  * @param  bool enable  :
  *    -  true, enable the corresponding vendor-specific IE function, all parameters below have to be set.
  *    -  false, disable the corresponding vendor-specific IE function and release the resource,
  *       only the parameter "type" below has to be set.
  * @param  uint8_t type : IE type. If it is VND_IE_TYPE_BEACON, please disable the IE function and enable
  *                        again to take the configuration effect immediately .
  * @param  uint8_t idx : vendor-specific IE index, 0 or 1. Only support two vendor-specific IEs in one frame.
  * @param  uint8_t *vnd_ie : vendor-specific information elements, need to input the whole 802.11 IE
  *                           including Element ID, Length, Organization Identifier and Vendor-specific Content.
  *
  * @return true  : succeed
  * @return false : fail
  */
bool wifi_set_vnd_ie(bool enable, vendor_ie_type type, uint8_t idx, uint8_t *vnd_ie);

/**
  * @brief   Register vendor IE received callback.
  *
  * @param   vendor_ie_recv_cb_t cb : callback
  *
  * @return  0 : succeed
  * @return -1 : fail
  */
sint32 wifi_register_vnd_ie_recv_cb(vendor_ie_recv_cb_t cb);

/**
  * @brief  Unregister vendor IE received callback.
  *
  * @param  null
  *
  * @return null
  */
void wifi_unregister_vnd_ie_recv_cb(void);

/**
  * @}
  */

/** \defgroup WiFi_User_IE_APIs User IE APIs
  * @brief WiFi User IE APIs
  */

/** @addtogroup WiFi_User_IE_APIs
  * @{
  */

typedef enum {
    USER_IE_BEACON = 0,
    USER_IE_PROBE_REQ,
    USER_IE_PROBE_RESP,
    USER_IE_ASSOC_REQ,
    USER_IE_ASSOC_RESP,
    USER_IE_MAX
} user_ie_type;

/**
  * @brief  User IE received callback.
  *
  * @param  user_ie_type type :  type of user IE.
  * @param  const uint8 sa[6] : source address of the packet.
  * @param  const uint8 m_oui[3] : factory tag.
  * @param  uint8 *user_ie : pointer of user IE.
  * @param  uint8 ie_len : length of user IE.
  * @param  sint32 rssi : signal strength.
  *
  * @return null
  */
typedef void (*user_ie_manufacturer_recv_cb_t)(user_ie_type type, const uint8 sa[6], const uint8 m_oui[3], uint8 *ie, uint8 ie_len, sint32 rssi);

/**
  * @brief  Set user IE of ESP8266.
  *
  *         The user IE will be added to the target packets of user_ie_type.
  *
  * @param  bool enable  :
  *    -  true, enable the corresponding user IE function, all parameters below have to be set.
  *    -  false, disable the corresponding user IE function and release the resource,
  *       only the parameter "type" below has to be set.
  * @param  uint8 *m_oui : factory tag, apply for it from Espressif System.
  * @param  user_ie_type type : IE type. If it is USER_IE_BEACON, please disable the
  *                             IE function and enable again to take the configuration
  *                             effect immediately .
  * @param  uint8 *user_ie : user-defined information elements, need not input the whole
  *                          802.11 IE, need only the user-define part.
  * @param  uint8 len : length of user IE, 247 bytes at most.
  *
  * @return true  : succeed
  * @return false : fail
  */

bool wifi_set_user_ie(bool enable, uint8 *m_oui, user_ie_type type, uint8 *user_ie, uint8 len);

/**
  * @brief   Register user IE received callback.
  *
  * @param   user_ie_manufacturer_recv_cb_t cb : callback
  *
  * @return  0 : succeed
  * @return -1 : fail
  */
sint32 wifi_register_user_ie_manufacturer_recv_cb(user_ie_manufacturer_recv_cb_t cb);

/**
  * @brief  Unregister user IE received callback.
  *
  * @param  null
  *
  * @return null
  */
void wifi_unregister_user_ie_manufacturer_recv_cb(void);

/**
  * @}
  */

/** \defgroup WiFi_Sniffer_APIs Sniffer APIs
  * @brief WiFi sniffer APIs
  */

/** @addtogroup WiFi_Sniffer_APIs
  * @{
  */

/**
  * @brief The RX callback function in the promiscuous mode.
  *
  *        Each time a packet is received, the callback function will be called.
  *
  * @param uint8 *buf : the data received
  * @param uint16 len : data length
  *
  * @return null
  */
typedef void (* wifi_promiscuous_cb_t)(uint8 *buf, uint16 len);

/**
  * @brief Register the RX callback function in the promiscuous mode.
  *
  *        Each time a packet is received, the registered callback function will be called.
  *
  * @param wifi_promiscuous_cb_t cb : callback
  *
  * @return null
  */
void wifi_set_promiscuous_rx_cb(wifi_promiscuous_cb_t cb);

/**
  * @brief  Get the channel number for sniffer functions.
  *
  * @param  null
  *
  * @return channel number
  */
uint8 wifi_get_channel(void);

/**
  * @brief  Set the channel number for sniffer functions.
  *
  * @param  uint8 channel :  channel number
  *
  * @return true  : succeed
  * @return false : fail
  */
bool wifi_set_channel(uint8 channel);

/**
  * @brief     Set the MAC address filter for the sniffer mode.
  *
  * @attention This filter works only for the current sniffer mode.
  *            If users disable and then enable the sniffer mode, and then enable
  *            sniffer, they need to set the MAC address filter again.
  *
  * @param     const uint8_t *address : MAC address
  *
  * @return    true  : succeed
  * @return    false : fail
  */
bool wifi_promiscuous_set_mac(const uint8_t *address);

/**
  * @brief     Enable the promiscuous mode.
  *
  * @attention 1. The promiscuous mode can only be enabled in the ESP8266 station mode. Do not call this API in user_init.
  * @attention 2. When in the promiscuous mode, the ESP8266 station and soft-AP are disabled.
  * @attention 3. Call wifi_station_disconnect to disconnect before enabling the promiscuous mode.
  * @attention 4. Don't call any other APIs when in the promiscuous mode. Call
  *               wifi_promiscuous_enable(0) to quit sniffer before calling other APIs.
  *
  * @param     uint8 promiscuous :
  *    - 0: to disable the promiscuous mode
  *    - 1: to enable the promiscuous mode
  *
  * @return    null
  */
void wifi_promiscuous_enable(uint8 promiscuous);

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
