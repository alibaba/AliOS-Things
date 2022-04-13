

#ifndef __WIFI_API_H__
#define __WIFI_API_H__

/**@brief MAC address length.
*/
#define WIFI_MAC_ADDRESS_LENGTH             (6)
/**@brief The maximum length of passphrase used in WPA-PSK and WPA2-PSK encryption types.
*/
#define WIFI_LENGTH_PASSPHRASE              (64)
/**@brief The maximum length of PMK used in WPA-PSK and WPA2-PSK encryption types.
*/
#define WIFI_LENGTH_PMK                     (32)
/**@brief The maximum length of SSID.
*/
#define WIFI_MAX_LENGTH_OF_SSID             (32)
/**
* @brief Station operation mode. In this mode the device works as a Wi-Fi client.
*/
#define WIFI_MODE_STA_ONLY      (1)

/**
* @brief SoftAP operation mode. In AP mode, other client devices can connect to the Wi-Fi AP.
*/
#define WIFI_MODE_AP_ONLY       (2)

/**
* @brief Repeater mode. There are two virtual ports in repeater mode, one is #WIFI_PORT_AP, and the other is #WIFI_PORT_APCLI.
* Both ports should be configured to operate on the same channel with the same bandwidth, while their other settings can be different. For example, both ports can have different MAC addresses and security modes.
*/
#define WIFI_MODE_REPEATER          (3)

/**
* @brief This macro defines the monitoring mode. In this mode it can sniffer
* the Wi-Fi packet in the air given the specific channel and bandwidth.
* It is used to enter a hybrid mode and handle a raw packet.
* Call #wifi_config_register_rx_handler() to register a raw packet handler
* once this mode is set.
*/
#define WIFI_MODE_MONITOR       (4)

#define WIFI_NUMBER_WEP_KEYS     (4)    /**<  The group number of WEP keys. */

/**
* @brief Specifies the STA operation.
*/
#define WIFI_PORT_STA           (0)

/**
* @brief Specifies the AP client operation.
*/
#define WIFI_PORT_APCLI         WIFI_PORT_STA

/**
* @brief Specifies the AP operation.
*/
#define WIFI_PORT_AP            (1)

#define WIFI_MAX_WEP_KEY_LENGTH  (26)   /**<  The maximum length of each WEP key. */
/**
* @brief Supports Wi-Fi Direct feature.
*/
#define WIFI_MODE_P2P_ONLY          (5)

#define MAX_SCAN_ARRAY_ITEM 192
#define MAX_SCAN_TABLE_SIZE MAX_SCAN_ARRAY_ITEM

typedef enum {
    WIFI_AUTH_MODE_OPEN = 0,                        /**< Open mode.     */
    WIFI_AUTH_MODE_SHARED,                          /**< Not supported. */
    WIFI_AUTH_MODE_AUTO_WEP,                        /**< Not supported. */
    WIFI_AUTH_MODE_WPA,                             /**< Not supported. */
    WIFI_AUTH_MODE_WPA_PSK,                         /**< WPA_PSK.       */
    WIFI_AUTH_MODE_WPA_None,                        /**< Not supported. */
    WIFI_AUTH_MODE_WPA2,                            /**< Not supported. */
    WIFI_AUTH_MODE_WPA2_PSK,                        /**< WPA2_PSK.      */
    WIFI_AUTH_MODE_WPA_WPA2,                        /**< Not supported. */
    WIFI_AUTH_MODE_WPA_PSK_WPA2_PSK,                /**< Mixture mode.  */
} wifi_auth_mode_t;


typedef enum {
    WIFI_ENCRYPT_TYPE_WEP_ENABLED = 0,                                              /**< WEP encryption type.  */
    WIFI_ENCRYPT_TYPE_ENCRYPT1_ENABLED = WIFI_ENCRYPT_TYPE_WEP_ENABLED,             /**< WEP encryption type.  */
    WIFI_ENCRYPT_TYPE_WEP_DISABLED = 1,                                             /**< No encryption.     */
    WIFI_ENCRYPT_TYPE_ENCRYPT_DISABLED = WIFI_ENCRYPT_TYPE_WEP_DISABLED,            /**< No encryption.     */
    WIFI_ENCRYPT_TYPE_WEP_KEY_ABSENT = 2,                                           /**< Not supported.     */
    WIFI_ENCRYPT_TYPE_ENCRYPT_KEY_ABSENT = WIFI_ENCRYPT_TYPE_WEP_KEY_ABSENT,        /**< Not supported.     */
    WIFI_ENCRYPT_TYPE_WEP_NOT_SUPPORTED = 3,                                        /**< Not supported.     */
    WIFI_ENCRYPT_TYPE_ENCRYPT_NOT_SUPPORTED = WIFI_ENCRYPT_TYPE_WEP_NOT_SUPPORTED,  /**< Not supported.     */
    WIFI_ENCRYPT_TYPE_TKIP_ENABLED = 4,                                             /**< TKIP encryption.   */
    WIFI_ENCRYPT_TYPE_ENCRYPT2_ENABLED = WIFI_ENCRYPT_TYPE_TKIP_ENABLED,            /**< TKIP encryption.   */
    WIFI_ENCRYPT_TYPE_AES_ENABLED = 6,                                              /**< AES encryption.    */
    WIFI_ENCRYPT_TYPE_ENCRYPT3_ENABLED = WIFI_ENCRYPT_TYPE_AES_ENABLED,             /**< AES encryption.     */
    WIFI_ENCRYPT_TYPE_AES_KEY_ABSENT = 7,                                           /**< Not supported.     */
    WIFI_ENCRYPT_TYPE_TKIP_AES_MIX = 8,                                             /**< TKIP or AES mix.   */
    WIFI_ENCRYPT_TYPE_ENCRYPT4_ENABLED = WIFI_ENCRYPT_TYPE_TKIP_AES_MIX,            /**< TKIP or AES mix.   */
    WIFI_ENCRYPT_TYPE_TKIP_AES_KEY_ABSENT = 9,                                      /**< Not supported.     */
    WIFI_ENCRYPT_TYPE_GROUP_WEP40_ENABLED = 10,                                     /**< Not supported.     */
    WIFI_ENCRYPT_TYPE_GROUP_WEP104_ENABLED = 11,                                    /**< Not supported.     */
#ifdef WAPI_SUPPORT
    WIFI_ENCRYPT_TYPE_ENCRYPT_SMS4_ENABLED,                                         /**< WPI SMS4 support.  */
#endif /* WAPI_SUPPORT */
} wifi_encrypt_type_t;

/** @brief Wi-Fi WEP keys.
*/
typedef struct {
    uint8_t wep_key[WIFI_NUMBER_WEP_KEYS][WIFI_MAX_WEP_KEY_LENGTH];     /**< There are four WEP keys. */
    uint8_t wep_key_length[WIFI_NUMBER_WEP_KEYS];                       /**< WEP key length. */
    uint8_t wep_tx_key_index;                                           /**< Default key index for TX frames using WEP. */
} wifi_wep_key_t;

/**@defgroup WIFI_ENUM Enumeration
* @{
*/
/**@brief This enumeration defines the supported events generated by the Wi-Fi driver. The event will be sent to the upper layer handler registered in #wifi_connection_register_event_handler().
*/
typedef enum {
    WIFI_EVENT_IOT_CONNECTED = 0,               /**< Connected event. */
    WIFI_EVENT_IOT_SCAN_COMPLETE,               /**< Scan completed event.*/
    WIFI_EVENT_IOT_DISCONNECTED,                /**< Disconnected event.*/
    WIFI_EVENT_IOT_PORT_SECURE,                 /**< Secure event, mainly processed in Wi-Fi driver. It can be used at the DHCP start.*/
    WIFI_EVENT_IOT_REPORT_BEACON_PROBE_RESPONSE,/**< Sends notification, if beacon or probe request is received.*/
    WIFI_EVENT_IOT_WPS_COMPLETE,                /**< Credential event, sends notification when WPS process is complete.*/
    WIFI_EVENT_IOT_INIT_COMPLETE,               /**< Initialization complete event. */
    WIFI_EVENT_IOT_REPORT_FILTERED_FRAME,       /**< Report the expected packet frame. */
    WIFI_EVENT_IOT_CONNECTION_FAILED,          /**<  Connection has failed. For the reason code, please refer to #wifi_reason_code_t. */
    WIFI_EVENT_IOT_FLOW_CONTROL_NOTIFICATION,  /**<  Notify the application to send data slow or fast, please refer to #wifi_flow_control_event_t. */
    WIFI_EVENT_MAX_NUMBER
} wifi_event_t;

/** @brief This enumeration defines the reason code of the WIFI_EVENT_IOT_CONNECTION_FAILED event in #wifi_event_t.
* When receives WIFI_EVENT_IOT_CONNECTION_FAILED handler, the payload format for the WIFI_EVENT_IOT_CONNECTION_FAILED handler is port(1 byte) and reason code(2 bytes). 
* Find the details for the reason code below.
*/
typedef enum {
    WIFI_REASON_CODE_SUCCESS,                             /**<  0   Reserved. */
    WIFI_REASON_CODE_FIND_AP_FAIL,                        /**<  1   (Internal) No AP found. */
    WIFI_REASON_CODE_PREV_AUTH_INVALID,                   /**<  2   Previous authentication is no longer valid. */
    WIFI_REASON_CODE_DEAUTH_LEAVING_BSS,                  /**<  3   Deauthenticated because sending STA is leaving (or has left) IBSS or ES. */
    WIFI_REASON_CODE_DISASSOC_INACTIVITY,                 /**<  4   Disassociated due to inactivity. */
    WIFI_REASON_CODE_DISASSOC_AP_OVERLOAD,                /**<  5   Disassociated because AP is unable to handle all currently associated STAs. */
    WIFI_REASON_CODE_CLASS_2_ERR,                         /**<  6   Class 2 frame received from nonauthenticated STA. */
    WIFI_REASON_CODE_CLASS_3_ERR,                         /**<  7   Class 3 frame received from nonauthenticated STA. */
    WIFI_REASON_CODE_DISASSOC_LEAVING_BSS,                /**<  8   Disassociated because sending STA is leaving (or has left) BSS. */
    WIFI_REASON_CODE_ASSOC_BEFORE_AUTH,                   /**<  9   STA requesting (re)association is not authenticated with responding STA. */
    WIFI_REASON_CODE_DISASSOC_PWR_CAP_UNACCEPTABLE,       /**<  10  Disassociated because the information in the Power Capability element is unacceptable. */
    WIFI_REASON_CODE_DISASSOC_SUP_CHS_UNACCEPTABLE,       /**<  11  Disassociated because the information in the Supported Channels element is unacceptable. */
    WIFI_REASON_CODE_INVALID_INFO_ELEM = 13,              /**<  13  Invalid information element. */
    WIFI_REASON_CODE_MIC_FAILURE,                         /**<  14  Message integrity code (MIC) failure. */
    WIFI_REASON_CODE_4_WAY_HANDSHAKE_TIMEOUT,             /**<  15  4-Way Handshake time out. */
    WIFI_REASON_CODE_GROUP_KEY_UPDATE_TIMEOUT,            /**<  16  Group Key Handshake time out. */
    WIFI_REASON_CODE_DIFFERENT_INFO_ELEM,                 /**<  17  Information element in 4-Way Handshake different from (Re)Association Request/Probe Response/Beacon frame. */
    WIFI_REASON_CODE_MULTICAST_CIPHER_NOT_VALID,          /**<  18  Invalid group cipher. */
    WIFI_REASON_CODE_UNICAST_CIPHER_NOT_VALID,            /**<  19  Invalid pairwise cipher. */
    WIFI_REASON_CODE_AKMP_NOT_VALID,                      /**<  20  Invalid AKMP. */
    WIFI_REASON_CODE_UNSUPPORTED_RSNE_VERSION,            /**<  21  Unsupported RSN information element version. */
    WIFI_REASON_CODE_INVALID_RSNE_CAPABILITIES,           /**<  22  Invalid RSN information element capabilities. */
    WIFI_REASON_CODE_IEEE_802_1X_AUTH_FAILED,             /**<  23  IEEE 802.1X authentication failed. */
    WIFI_REASON_CODE_CIPHER_REJECT_SEC_POLICY,            /**<  24  Cipher suite rejected because of the security policy. */
    WIFI_REASON_CODE_DONT_SUPPORT_HT_OPERATION,           /**<  25  (Internal) Association denied due to requesting STA not supporting the Short Slot Time option. */
    WIFI_REASON_CODE_DONT_SUPPORT_DSS_OFDM,               /**<  26  (Internal) Association denied due to requesting STA not supporting the DSSS-OFDM option. */
    WIFI_REASON_CODE_DONT_SUPPORT_HT,                     /**<  27  (Internal) HT not supported. */
    WIFI_REASON_CODE_DISASSOC_UNSPECIFIED_QOS = 32,       /**<  32  Disassociated for unspecified, QoS-related reason. */
    WIFI_REASON_CODE_DISASSOC_LACK_OF_BANDWIDTH,          /**<  33  Disassociated because QoS AP lacks sufficient bandwidth for this QoS STA. */
    WIFI_REASON_CODE_DISASSOC_ACK_LOST_POOR_CHANNEL,      /**<  34  Disassociated because excessive number of frames need to be acknowledged, but are not acknowledged due to AP transmissions and/or poor channel conditions. */
    WIFI_REASON_CODE_DISASSOC_TX_OUTSIDE_TXOP_LIMIT,      /**<  35  Disassociated because STA is transmitting outside the limits of its TXOPs. */
    WIFI_REASON_CODE_PEER_WHILE_LEAVING,                  /**<  36  Requested from peer STA as the STA is leaving the BSS (or resetting). */
    WIFI_REASON_CODE_PEER_REFUSE_DLP,                     /**<  37  Requested from peer STA as it does not want to use the mechanism. */
    WIFI_REASON_CODE_PEER_SETUP_REQUIRED,                 /**<  38  Requested from peer STA as the STA received frames using the mechanism for which a setup is required. */
    WIFI_REASON_CODE_PEER_TIME_OUT,                       /**<  39  Requested from peer STA due to time out. */
    WIFI_REASON_CODE_INVALID_INFORMATION_ELEMENT,         /**<  40  (Internal) Invalid information element. */
    WIFI_REASON_CODE_INVALID_GROUP_CIPHER,                /**<  41  (Internal) Invalid group cipher. */
    WIFI_REASON_CODE_INVALID_PAIRWISE_CIPHER,             /**<  42  (Internal) Invalid pairwise cipher. */
    WIFI_REASON_CODE_INVALID_AKMP,                        /**<  43  (Internal) Invalid AKMP. */
    WIFI_REASON_CODE_DONT_SUPPORT_RSN_VERSION,            /**<  44  (Internal) Unsupported RSN information element version.*/
    WIFI_REASON_CODE_PEER_CIPHER_UNSUPPORTED,             /**<  45  Peer STA does not support the requested cipher suite. */
    WIFI_REASON_CODE_PASSWORD_ERROR = 100                 /**<  100 (Internal) Connection has failed, wrong password. */
} wifi_reason_code_t;

/** @brief This enumeration defines wireless security cipher suits.
*/
typedef enum {
        WIFI_CIPHER_NONE = 0,     /**<  0, No cipher. */
        WIFI_CIPHER_WEP64,        /**<  1, WEP64 cipher.  */
        WIFI_CIPHER_WEP128,       /**<  2, WEP128 cipher. */
        WIFI_CIPHER_TKIP,         /**<  3, TKIP cipher. */
        WIFI_CIPHER_AES,          /**<  4, AES cipher. */
        WIFI_CIPHER_CKIP64,       /**<  5, CKIP64 cipher. */
        WIFI_CIPHER_CKIP128,      /**<  6, CKIP128 cipher. */
        WIFI_CIPHER_CKIP152,      /**<  7, CKIP152 cipher. */
        WIFI_CIPHER_SMS4,         /**<  8, SMS4 cipher. */
        WIFI_CIPHER_WEP152,       /**<  9, WEP152 cipher. */
        WIFI_CIPHER_BIP,          /**< 10, BIP cipher. */
        WIFI_CIPHER_WAPI,         /**< 11, WAPI cipher. */
        WIFI_CIPHER_TKIP_NO_MIC   /**< 12, TKIP_NO_MIC cipher. */
} wifi_security_cipher_suits_t;

#define MAX_WEP_KEY_SIZE_CM4 32
typedef struct _wep_key_cm4 {
    uint8_t ucdefault_key_id;
    uint8_t auckey_0[MAX_WEP_KEY_SIZE_CM4];
    uint8_t uckey_0_len;
    uint8_t auckey_1[MAX_WEP_KEY_SIZE_CM4];
    uint8_t uckey_1_len;
    uint8_t auckey_2[MAX_WEP_KEY_SIZE_CM4];
    uint8_t uckey_2_len;
    uint8_t auckey_3[MAX_WEP_KEY_SIZE_CM4];
    uint8_t uckey_3_len;
} wep_key_cm4_t;

/** @brief This structure provides the detailed cipher information.
*/
typedef struct {
    uint8_t   key[16];            /**< Four keys are implemented, with a maximum length of 128bits.*/
    uint8_t   rx_mic[8];          /**< 8 bytes of RX MIC key.  */
    uint8_t   tx_mic[8];          /**< 8 bytes of TX MIC key. */
    uint8_t   tx_tsc[6];          /**< 48bit TSC value. */
    uint8_t   rx_tsc[6];          /**< 48bit TSC value. */
    uint8_t   cipher_type;        /**< 0:none, 1:WEP64, 2:WEP128, 3:TKIP, 4:AES, 5:CKIP64, 6:CKIP128.*/
    uint8_t   key_length;         /**< Key length for each key, 0: entry is invalid.*/
    uint8_t   bssid[6];           /**< AP's MAC address. */
    uint8_t   type;               /**< Indicates the type (pairwise or group) when reporting a MIC error.*/
} wifi_cipher_key_t;

 typedef struct __wpa_supplicant_api_callback {
	int (*start_connect)(uint8_t *ssid, uint8_t *pwd);
    int (*set_security)(uint8_t port, uint8_t auth_mode, uint8_t encrypt_type);
    int (*get_security)(uint8_t port, uint8_t *auth_mode, uint8_t *encrypt_type);

    int (*set_ssid)(uint8_t port, uint8_t *ssid, uint8_t ssid_length);
    int (*get_ssid)(uint8_t port, uint8_t *ssid, uint8_t *ssid_length);

    int (*set_bssid)(uint8_t *bssid);

    int (*set_pmk)(uint8_t port, uint8_t *pmk);
    int (*get_pmk)(uint8_t port, uint8_t *pmk);
    int (*cal_pmk)(uint8_t *passphrase, uint8_t *ssid, uint8_t ssid_length, uint8_t *psk);

    int (*set_wpapsk)(uint8_t port, uint8_t *passphrase, uint8_t passphrase_length);
    int (*get_wpapsk)(uint8_t port, uint8_t *passphrase, uint8_t *passphrase_length);

    int (*set_wep_key)(uint8_t port, wifi_wep_key_t *wep_keys);
    int (*get_wep_key)(uint8_t port, wifi_wep_key_t *wep_keys);

    int (*wpa_supplicant_entry_iface_change)(unsigned char op_mode);
    void (*wpa_supplicant_entry_op_mode_get)(unsigned char *op_mode);
    void (*wpa_supplicant_entry_stop_scan)(void);
    void (*wpa_supplicant_entry_start_scan)(void);
    void (*wpa_supplicant_entry_radio_onoff)(uint8_t radio_on);
    void (*wpa_supplicant_entry_req_scan)(int sec, int usec);
    void (*wpa_supplicant_entry_ap_onoff)(uint8_t ap_on);

    int (*do_wps_pbc)(char *cmd);
    int (*do_wps_pin)(char *cmd, char *buf, int buflen);
    int (*do_wps_reg_pin)(char *cmd);
    int (*wps_ap_pin)(char *cmd, char *buf, int buflen);
    void (*wpa_supplicant_check_dpid_from_wps_ap)(void *ie_buf, uint16_t *device_passwd_id);
    int32_t (*wpa_supplicant_query_connect_status)(uint8_t *link_status);
    int (*wps_generate_pin)(void);
    int (*wps_get_pin)(char *cmd, char *buf, int buflen);

    int (*get_ap_sta)(char *mac_addr);
    int (*get_sta_qos_bit)(int32_t inf_num, char *mac_addr);   /* return value - 1: WMM, 0: non-WMM, <0: station doesn't exist */

    void (*show_ifaces)(void);
    void (*wpa_supplicant_disable_trigger_scan)(void);
    void (*wpa_supplicant_enable_trigger_scan)(void);

    int (*get_bssid)(uint8_t *bssid);
    int (*set_ssid_bssid)(unsigned char *ssid, unsigned int ssidlen, uint8_t *bssid);
} wpa_supplicant_api_callback;

extern wpa_supplicant_api_callback __g_wpa_supplicant_api;
typedef int32_t (* wifi_event_handler_t)(wifi_event_t event, uint8_t *payload, uint32_t length);

#endif