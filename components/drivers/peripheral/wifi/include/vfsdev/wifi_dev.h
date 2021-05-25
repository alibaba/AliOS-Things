/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef DEVICE_WIFI_H
#define DEVICE_WIFI_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define WIFI_NUMBER_WEP_KEYS     (4)    /**<  The group number of WEP keys. */
#define WIFI_MAX_WEP_KEY_LENGTH  (26)   /**<  The maximum length of each WEP key. */

#define WIFI_PROTOCOL_11B         1
#define WIFI_PROTOCOL_11G         2
#define WIFI_PROTOCOL_11N         4
#define WIFI_PROTOCOL_LR          8

#define MAX_SSID_SIZE             32
#define MAX_PASSWD_SIZE           64


typedef enum {
    /** base code */
    WIFI_ERR_OK = 0,
    WIFI_ERR_NO_MEM,         /*!< Out of memory */
    WIFI_ERR_ARG,            /*!< Invalid argument */
    WIFI_ERR_NOT_SUPPORT,   /*!< Indicates that API is not supported yet */
    WIFI_ERR_FAIL,          /*!< General fail code */

    /** general code */
    WIFI_ERR_NOT_INIT,      /*!< WiFi driver is not initialized by #hal_wifi_initialize */
    WIFI_ERR_INIT, /*!< WiFi driver was initialized by #hal_wifi_initialize */
    WIFI_ERR_NOT_START, /*!< WiFi driver is not started by #hal_wifi_start */
    WIFI_ERR_START, /*!< WiFi driver was started by #csi_wifi_start */
    WIFI_ERR_IF,/*!< WiFi interface error */
    WIFI_ERR_MODE, /*!< WiFi mode error */
    WIFI_ERR_STATE,/*!< WiFi internal state error */
    WIFI_ERR_NVS,/*!< WiFi internal NVS module error */
    WIFI_ERR_HANDLE, /*!< Invalid handle argument */
    WIFI_ERR_AUTHMODE, /*!< Invalid auth mode */
    WIFI_ERR_MAC,/*!< MAC address is invalid */
    WIFI_ERR_SSID,/*!< SSID is invalid */
    WIFI_ERR_PASSWORD,/*!< Password is invalid */
    WIFI_ERR_TIMEOUT,/*!< Timeout error */
    WIFI_ERR_WAKE_FAIL, /*!< WiFi is in sleep state(RF closed) and wakeup fail */
    WIFI_ERR_REGISTRAR,/*!< WPS registrar is not supported */
    WIFI_ERR_WPS_TYPE, /*!< WPS type error */
    WIFI_ERR_WPS_SM, /*!< WPS state machine is not initialized */

    /** connection code */
    WIFI_ERR_CONN_UNSPECIFIED,
    WIFI_ERR_CONN_AUTH_EXPIRE,
    WIFI_ERR_CONN_AUTH_LEAVE,
    WIFI_ERR_CONN_ASSOC_EXPIRE,
    WIFI_ERR_CONN_ASSOC_TOOMANY,
    WIFI_ERR_CONN_NOT_AUTHED,
    WIFI_ERR_CONN_NOT_ASSOCED,
    WIFI_ERR_CONN_ASSOC_LEAVE,
    WIFI_ERR_CONN_ASSOC_NOT_AUTHED,
    WIFI_ERR_CONN_DISASSOC_PWRCAP_BAD,
    WIFI_ERR_CONN_DISASSOC_SUPCHAN_BAD,
    WIFI_ERR_CONN_IE_INVALID,
    WIFI_ERR_CONN_MIC_FAILURE,
    WIFI_ERR_CONN_4WAY_HANDSHAKE_TIMEOUT,
    WIFI_ERR_CONN_GROUP_KEY_UPDATE_TIMEOUT,
    WIFI_ERR_CONN_IE_IN_4WAY_DIFFERS,
    WIFI_ERR_CONN_GROUP_CIPHER_INVALID,
    WIFI_ERR_CONN_PAIRWISE_CIPHER_INVALID,
    WIFI_ERR_CONN_AKMP_INVALID,
    WIFI_ERR_CONN_UNSUPP_RSN_IE_VERSION,
    WIFI_ERR_CONN_INVALID_RSN_IE_CAP,
    WIFI_ERR_CONN_802_1X_AUTH_FAILED,
    WIFI_ERR_CONN_CIPHER_SUITE_REJECTED,
    WIFI_ERR_CONN_BEACON_TIMEOUT,
    WIFI_ERR_CONN_NO_AP_FOUND,
    WIFI_ERR_CONN_AUTH_FAIL,
    WIFI_ERR_CONN_ASSOC_FAIL,
    WIFI_ERR_CONN_HANDSHAKE_TIMEOUT,

    WIFI_ERR_MAX,
} wifi_err_code_t;


typedef enum {
    WIFI_EVT_WIFI_READY = 0,               /**< WiFi ready */
    WIFI_EVT_SCAN_DONE,                    /**< finish scanning AP */
    WIFI_EVT_STA_START,                    /**< station start */
    WIFI_EVT_STA_STOP,                     /**< station stop */
    WIFI_EVT_STA_CONNECTED,                /**< station connected to AP */
    WIFI_EVT_STA_DISCONNECTED,             /**< station disconnected from AP */
    WIFI_EVT_STA_AUTHMODE_CHANGE,          /**< the auth mode of AP connected by station changed */
    WIFI_EVT_STA_WPS_ER_SUCCESS,           /**< station wps succeeds in enrollee mode */
    WIFI_EVT_STA_WPS_ER_FAILED,            /**< station wps fails in enrollee mode */
    WIFI_EVT_STA_WPS_ER_TIMEOUT,           /**< station wps timeout in enrollee mode */
    WIFI_EVT_STA_WPS_ER_PIN,               /**< station wps pin code in enrollee mode */

    WIFI_EVT_AP_START,                     /**< soft-AP start */
    WIFI_EVT_AP_STOP,                      /**< soft-AP stop */
    WIFI_EVT_AP_STACONNECTED,              /**< a station connected to soft-AP */
    WIFI_EVT_AP_STADISCONNECTED,           /**< a station disconnected from soft-AP */
    WIFI_EVT_AP_PROBEREQRECVED,            /**< Receive probe request packet in soft-AP interface */

    WIFI_EVT_P2P_DEVICE_FOUND,             /**< P2P device found */
    WIFI_EVT_P2P_DEVICE_LOST,              /**< P2P device lost */
    WIFI_EVT_P2P_GROUP_STARTED,            /**< P2P group set up */
    WIFI_EVT_P2P_GROUP_REMOVED,            /**< P2P group terminated */
    WIFI_EVT_P2P_GROUP_FORMATION_SUCCESS,  /**< P2P group formation has been completed successfully */
    WIFI_EVT_P2P_GROUP_FORMATION_FAILURE,  /**< P2P group formation failed */
    WIFI_EVT_P2P_GO_NEG_REQUEST,           /**< P2P Go negotiation request */
    WIFI_EVT_P2P_GO_NEG_SUCCESS,           /**< P2P Go negotiation success */
    WIFI_EVT_P2P_GO_NEG_FAILURE,           /**< P2P Go negotiation fail */
    WIFI_EVT_P2P_PROV_DISC_SHOW_PIN,       /**< P2P display a PIN that will be entered on the peer. */
    WIFI_EVT_P2P_PROV_DISC_ENTER_PIN,      /**< P2P enter a PIN displayed on the peer. */
    WIFI_EVT_P2P_PROV_DISC_PBC_REQ,        /**< P2P request from the peer for local p2p device to connect using PBC. */
    WIFI_EVT_P2P_PROV_DISC_PBC_RESP,       /**< P2P accept provision discovery request to connect using PBC. */
    WIFI_EVT_P2P_INVITATION_RECEIVED,      /**< P2P invitation request received */
    WIFI_EVT_P2P_INVITATION_RESULT,        /**< P2P invitation result */
    WIFI_EVT_MAX
} wifi_event_id_t;

typedef enum {
    WIFI_MODE_NULL = 0,     /**< null mode */
    WIFI_MODE_STA,          /**< WiFi station mode */
    WIFI_MODE_AP,           /**< WiFi soft-AP mode */
    WIFI_MODE_APSTA,        /**< WiFi station + soft-AP mode */
    WIFI_MODE_P2P,          /**< WiFi P2P mode */
    WIFI_MODE_MAX
} wifi_mode_t;

/*\brief
    This enumeration defines the wireless encryption type to indicate
    the Wi-Fi device’s encryption attribute.
*/
typedef enum {
    WIFI_ENC_WEP = 0,
    WIFI_ENC_OPEN = 1,
    WIFI_ENC_TKIP = 4,
    WIFI_ENC_AES = 6,
    WIFI_ENC_TKIP_AES_MIX = 8,
    WIFI_ENC_MAX
} wifi_encrypt_type_t;

typedef struct {
    uint8_t wep_key[WIFI_NUMBER_WEP_KEYS][WIFI_MAX_WEP_KEY_LENGTH];     /**< There are four WEP keys. */
    uint8_t wep_key_length[WIFI_NUMBER_WEP_KEYS];                       /**< WEP key length. */
    uint8_t wep_tx_key_index;                                           /**< Default key index for TX frames using WEP. */
} wifi_wep_key_t;

typedef enum {
    WIFI_AUTH_OPEN = 0,
    WIFI_AUTH_WEP,
    WIFI_AUTH_WPA_PSK,
    WIFI_AUTH_WPA2_PSK,
    WIFI_AUTH_WPA_WPA2_PSK,
    WIFI_AUTH_WPA2_ENTERPRISE,
    WIFI_AUTH_MAX
} wifi_auth_mode_t;

typedef enum {
    WIFI_SCAN_MODE_ACTIVE = 0,  /**< active scan */
    WIFI_SCAN_MODE_PASSIVE,     /**< passive scan */
} wifi_scan_mode_t;

typedef enum {
    WIFI_LPM_NONE,          /**< No power save */
    WIFI_LPM_KEEP_SOCKET,   /**< power save with socket maintain */
    WIFI_LPM_KEEP_LINK,     /**< power save with wifi link connecting maintain */
    WIFI_LPM_POWEROFF,      /**< power save with power off */
} wifi_lpm_mode_t;

typedef enum {
    WIFI_STATUS_LINK_DOWN = 0,
    WIFI_STATUS_LINK_UP,
    WIFI_STATUS_GOTIP,
} wifi_status_link_t;

typedef enum {
    WIFI_STORAGE_FLASH,  /**< all configuration will strore in both memory and flash */
    WIFI_STORAGE_RAM,    /**< all configuration will only store in the memory */
} wifi_storage_t;

typedef struct {
    char* partner;
    char* project;
    char* app_net;
    char* cloud;
    char* os;
    char* type;
} wifi_base_info_t;

typedef struct {
    uint32_t min;  /**< minimum active scan time per channel, units: millisecond */
    uint32_t max;  /**< maximum active scan time per channel, units: millisecond, values above 1500ms may
                        cause station to disconnect from AP and are not recommended.  */
} wifi_active_scan_time_t;

typedef union {
    wifi_active_scan_time_t active;  /**< active scan time per channel */
    uint32_t passive;                /**< passive scan time per channel, units: millisecond, values above 1500ms may
                                          cause station to disconnect from AP and are not recommended. */
} wifi_scan_time_t;

typedef struct {
    uint8_t ssid[MAX_SSID_SIZE + 1];               /**< SSID of AP */
    uint8_t bssid[6];              /**< MAC address of AP */
    uint8_t channel;             /**< channel, scan the specific channel */
    bool show_hidden;            /**< enable to scan AP whose SSID is hidden */
    wifi_scan_mode_t scan_mode;  /**< scan modE, active or passive */
    wifi_scan_time_t scan_time;  /**< scan time per channel */
} wifi_scan_config_t;

typedef enum {
    WIFI_SECOND_CHAN_NONE = 0,  /**< the channel width is HT20 */
    WIFI_SECOND_CHAN_ABOVE,     /**< the channel width is HT40 and the second channel is above the primary channel */
    WIFI_SECOND_CHAN_BELOW,     /**< the channel width is HT40 and the second channel is below the primary channel */
} wifi_second_chan_t;

typedef struct {
    wifi_status_link_t link_status;       /* if not connected, following data is not valid */
    uint8_t bssid[6];                     /* MAC address of AP */
    uint8_t ssid[MAX_SSID_SIZE + 1];      /* SSID of AP */
    uint8_t channel;                      /* channel of AP */
    int8_t  rssi;                         /* signal strength of AP */
    wifi_encrypt_type_t encryptmode;      /* encrypt mode of AP */
    wifi_auth_mode_t authmode;            /* authmode of AP */
} wifi_ap_record_t;

typedef enum {
    WIFI_BW_HT20 = 1,       /* Bandwidth is HT20 */
    WIFI_BW_HT40_MINUS,    /* Bandwidth is HT40 */
    WIFI_BW_HT40_PLUS,     /* Bandwidth is HT40 */
    WIFI_BW_HT40_AUTO,
} wifi_bandwidth_t;

typedef enum {
    WIFI_COUNTRY_CN = 0, /**< country China, channel range [1, 14] */
    WIFI_COUNTRY_JP,     /**< country Japan, channel range [1, 14] */
    WIFI_COUNTRY_US,     /**< country USA, channel range [1, 11] */
    WIFI_COUNTRY_EU,     /**< country Europe, channel range [1, 13] */
    WIFI_COUNTRY_MAX
} wifi_country_t;

typedef struct {
    uint8_t mac[6];  /**< mac address of sta that associated with soft-AP */
} wifi_sta_info_t;

#define HAL_WIFI_MAX_CONN_NUM  (10)       /**< max number of stations which can connect to soft-AP */

typedef struct {
    wifi_sta_info_t sta[HAL_WIFI_MAX_CONN_NUM]; /**< station list */
    int       num; /**< number of station that associated with soft-AP */
} wifi_sta_list_t;

typedef struct {
    int present; /* this config is activated, default to 0 */
    uint8_t listen_interval; /**  STA's listen interval, default to 0 */
    char bssid[6]; /** indicate Target AP's bssid, default to {0x00, 0x00, 0x00, 0x00, 0x00, 0x00} */
    uint8_t channel;/* set a channel that can speed up a connection procedure, default yo 0*/
} wifi_config_sta_adv_t;

typedef struct {
    int present; //this config is activated, default to 0
    uint8_t dtim_interval; //DTIM interval ,default to 0
    uint8_t channel; //ap's channel, default to 0
    uint8_t beacon_interval; //default to 0
    int hide_ssid; //hide the ssid, default to 0
    int max_sta_number; //max sta number, default to 0
} wifi_config_ap_adv_t;

typedef struct {
    int present; //this config is activated, default to 0
    wifi_auth_mode_t auth_mode; //wifi auth, WEP/WPA, ...
    wifi_encrypt_type_t encrypt_type; //wifi encrypt, TKIP, AES, ...
    wifi_wep_key_t wep_keys; //wep keys, if auth is not WEP, not need to fill
} wifi_config_sec_adv_t;


typedef struct {
    wifi_mode_t mode;              /* DHCP mode: @ref wlanInterfaceTypedef. */
    char ssid[MAX_SSID_SIZE + 1];          /* SSID of the wlan needs to be connected. */
    char password[MAX_PASSWD_SIZE + 1];           /* Security key of the wlan needs to be connected, ignored in an open system. */

    wifi_config_sta_adv_t sta_config; //if set to sta mode, may config this
    wifi_config_ap_adv_t ap_config; //if set to AP mode, may config this
    wifi_config_sec_adv_t sec_config;  //if have advanced security setting, config this
} wifi_config_t;

typedef struct {
    signed rssi: 8;           /**< signal intensity of packet */
    unsigned rate: 5;         /**< data rate */
    unsigned : 1;             /**< reserve */
    unsigned sig_mode: 2;     /**< 0:is not 11n packet; 1:is 11n packet */
    unsigned : 16;            /**< reserve */
    unsigned mcs: 7;          /**< if is 11n packet, shows the modulation(range from 0 to 76) */
    unsigned cwb: 1;          /**< if is 11n packet, shows if is HT40 packet or not */
    unsigned : 16;            /**< reserve */
    unsigned smoothing: 1;    /**< reserve */
    unsigned not_sounding: 1; /**< reserve */
    unsigned : 1;             /**< reserve */
    unsigned aggregation: 1;  /**< Aggregation */
    unsigned stbc: 2;         /**< STBC */
    unsigned fec_coding: 1;   /**< if is 11n packet, shows if is LDPC packet or not */
    unsigned sgi: 1;          /**< SGI */
    unsigned noise_floor: 8;  /**< noise floor */
    unsigned ampdu_cnt: 8;    /**< ampdu cnt */
    unsigned channel: 4;      /**< which channel this packet in */
    unsigned : 12;            /**< reserve */
    unsigned timestamp: 32;   /**< timestamp */
    unsigned : 32;            /**< reserve */
    unsigned : 32;            /**< reserve */
    unsigned sig_len: 12;     /**< It is really lenth of packet */
    unsigned : 12;            /**< reserve */
    unsigned rx_state: 8;     /**< rx state */
} wifi_pkt_rx_ctrl_t;

typedef struct {
    wifi_pkt_rx_ctrl_t rx_ctrl;
    uint8_t payload[0];       /**< ieee80211 packet buff, The length of payload is described by sig_len */
} wifi_promiscuous_pkt_t;

/**
  \brief     Promiscuous frame type
  */
typedef enum {
    WIFI_PKT_CTRL,  /**< control type, receive packet buf is wifi_pkt_rx_ctrl_t */
    WIFI_PKT_MGMT,  /**< management type, receive packet buf is wifi_promiscuous_pkt_t */
    WIFI_PKT_DATA,  /**< data type, receive packet buf is wifi_promiscuous_pkt_t */
    WIFI_PKT_MISC,  /**< other type, receive packet buf is wifi_promiscuous_pkt_t */
} wifi_promiscuous_pkt_type_t;

typedef enum {
    FRAME_FILTER_MODE_FORWARD = 1,                  /*!< Packet filter engine forwards matching packets, discards non-matching packets */
    FRAME_FILTER_MODE_DISCARD = 0,                  /*!< Packet filter engine discards matching packets, forwards non-matching packets */
} frame_filter_mode_t;

typedef enum {
    FRAME_FILTER_RULE_POSITIVE_MATCHING  = 0,       /*!< Specifies that a filter should match a given pattern     */
    FRAME_FILTER_RULE_NEGATIVE_MATCHING  = 1,       /*!< Specifies that a filter should NOT match a given pattern */
} frame_filter_rule_t;

struct wifi_frame_filter_list {
    struct wifi_frame_filter_list  *next;
    struct wifi_frame_filter_list  *prev;
};

typedef struct wifi_frame_filter_list wifi_frame_filter_list_t;

/**
 * Structure describing a frame filter list item
 */
typedef struct {
    wifi_frame_filter_list_t       filter_list;
    uint32_t                       id;             /*!< Unique identifier for a packet filter item */
    frame_filter_rule_t            rule;           /*!< Filter matches are either POSITIVE or NEGATIVE matching */
    uint16_t                       offset;         /*!< Offset in bytes to start filtering (referenced to the start of the 802.11 packet) */
    uint16_t                       mask_size;      /*!< Size of the mask in bytes */
    uint8_t                       *mask;           /*!< Pattern mask bytes to be ANDed with the pattern eg. "\xff00" (must be in network byte order) */
    uint8_t                       *pattern;        /*!< Pattern bytes used to filter eg. "\x0800"  (must be in network byte order) */
    bool                           enabled_status; /*!< Indicates if the filter is enabled */
} wifi_frame_filter_t;

typedef struct {
    uint8_t dhcp;     /* DHCP mode: @ref DHCP_Disable, @ref DHCP_Client, @ref DHCP_Server. */
    char    ip[16];   /* Local IP address on the target wlan interface: @ref wlanInterfaceTypedef. */
    char    gate[16]; /* Router IP address on the target wlan interface: @ref wlanInterfaceTypedef. */
    char    mask[16]; /* Netmask on the target wlan interface: @ref wlanInterfaceTypedef. */
    char    dns[16];  /* DNS server IP address. */
    char    mac[16];  /* MAC address, example: "C89346112233". */
    char    broadcastip[16];
} wifi_ip_stat_t;

typedef enum wifi_sec_type_e {
    SECURITY_TYPE_NONE,       /* Open system. */
    SECURITY_TYPE_WEP,        /* Wired Equivalent Privacy. WEP security. */
    SECURITY_TYPE_WPA_TKIP,   /* WPA /w TKIP */
    SECURITY_TYPE_WPA_AES,    /* WPA /w AES */
    SECURITY_TYPE_WPA2_TKIP,  /* WPA2 /w TKIP */
    SECURITY_TYPE_WPA2_AES,   /* WPA2 /w AES */
    SECURITY_TYPE_WPA2_MIXED, /* WPA2 /w AES or TKIP */
    SECURITY_TYPE_AUTO,       /* It is used when calling @ref micoWlanStartAdv, MICO read security type from scan result. */
} wifi_sec_type_t;

typedef struct {
    char    ssid[32 + 1];         /* The SSID of an access point. */
    int8_t  ap_power;             /* Received Signal Strength Indication, min: -110, max: 0 */
    char    bssid[6];             /* The BSSID of an access point. */
    uint8_t channel;              /* The RF frequency, 1-13 */
    wifi_sec_type_t sec_type;     /* Security type, @ref wlan_sec_type_t */
} ap_list_t;

/*
 *  Scan result using normal scan.
 */
typedef struct {
    char ap_num; /* The number of access points found in scanning. */

    ap_list_t *ap_list;
} wifi_scan_result_t, scan_ap_list_t;

typedef struct {
    int rssi; /* Received Signal Strength Indication */
    int snr;  /* Signal to Noise Ratio */
    int per;  /* Packet Error Rate (PPM)*/
} wifi_link_info_t;

typedef int32_t (*wifi_rxcb_t)(void *buffer, uint16_t len, void *ebuf);
typedef void (* wifi_promiscuous_cb_t)(wifi_promiscuous_pkt_t *buf, wifi_promiscuous_pkt_type_t type);
typedef void (*monitor_data_cb_t)(uint8_t *data, int len, wifi_link_info_t *info);

typedef struct {
    int netdev_type;
    void *drv_ops;
    void *priv;
} netdev_t;

typedef struct {
    void *data;
    int   len;
} raw_frame_t;
/*
 * The event call back function called at specific events occurred.
 *
 * @note For HAL implementors, these callbacks must be
 *       called under normal task context, not from interrupt.
 */
typedef struct {
    void (*sta_connect_fail)(netdev_t *dev, wifi_err_code_t err, void *arg);
    void (*status_change)(netdev_t *dev, wifi_event_id_t stat, void *arg);
    void (*scan_compeleted)(netdev_t *dev, uint16_t number, wifi_ap_record_t *ap_records);
    void (*fatal_err)(netdev_t *dev, void *arg);
} wifi_event_func;

typedef enum
{
    WIFI_DEV_CMD_GET_BASE_INFO,
    WIFI_DEV_CMD_SET_MODE,
    WIFI_DEV_CMD_GET_MODE,
    WIFI_DEV_CMD_INSTALL_EVENT_CB,
    WIFI_DEV_CMD_CANCEL_CONNECT,

    WIFI_DEV_CMD_SET_MAC,
    WIFI_DEV_CMD_GET_MAC,
    WIFI_DEV_CMD_SET_LPM,
    WIFI_DEV_CMD_GET_LPM,

    WIFI_DEV_CMD_NOTIFY_IP_STATE2DRV,

    WIFI_DEV_CMD_START_SCAN,
    WIFI_DEV_CMD_CONNECT,
    WIFI_DEV_CMD_DISCONNECT,
    WIFI_DEV_CMD_STA_GET_LINK_STATUS,
    WIFI_DEV_CMD_AP_GET_STA_LIST,
    WIFI_DEV_CMD_START_MONITOR,
    WIFI_DEV_CMD_STOP_MONITOR,
    WIFI_DEV_CMD_SEND_80211_RAW_FRAME,
    WIFI_DEV_CMD_SET_CHANNEL,
    WIFI_DEV_CMD_GET_CHANNEL,
    WIFI_DEV_CMD_START_SPECIFIED_SCAN,
    WIFI_DEV_CMD_REGISTER_MONITOR_CB,
    WIFI_DEV_CMD_START_MGNT_MONITOR,
    WIFI_DEV_CMD_STOP_MGNT_MONITOR,
    WIFI_DEV_CMD_REGISTER_MGNT_MONITOR_CB,
    WIFI_DEV_CMD_SET_SMARTCFG

} rt_wlan_cmd_t;


/** compatibility */

#ifdef __cplusplus
}
#endif

#endif

