/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef BWIFI_EVENT_H
#define BWIFI_EVENT_H
#if LWIP_SUPPORT
#include "lwip/ip_addr.h"
#endif
#ifdef  __cplusplus
extern "C"
{
#endif

typedef struct {
    int status;				/**< status of scanning APs */
    //struct dl_list *scan_result;	/**< list of APs found */
} BWIFI_EVENT_STAMODE_SCANDONE_T;

typedef enum {
    INTER_STATE_AUTHENTICATING,     /* Authentication start */
    INTER_STATE_AUTH_REJECT,        /* Authentication rejected by AP */
    INTER_STATE_AUTH_TIMEOUT,       /* Authentication timeout */
    INTER_STATE_ASSOCIATING,        /* Association start */
    INTER_STATE_ASSOC_REJECT,       /* Association request rejected by AP */
    INTER_STATE_ASSOC_TIMEOUT,      /* Association timeout */
    INTER_STATE_ASSOCIATED,         /* Associated to target AP */
    INTER_STATE_4WAY_HANDSHAKE,     /* 4way-handshake start */
    INTER_STATE_HANDSHAKE_FAIL,     /* 4way-handshake fail */
    INTER_STATE_GROUP_HANDSHAKE,    /* Group handshake start */
    /* CONNECTED */
} BWIFI_CONNECT_INTER_STATE;

typedef struct {
    u8 ssid[32];			/**< SSID of the connecting AP */
    u8 ssid_len;			/**< SSID length of the connecting AP */
    u8 bssid[6];			/**< BSSID of the connecting AP */
    BWIFI_CONNECT_INTER_STATE state;	/**< intermediate state during connection */
} BWIFI_EVENT_STAMODE_CONNECTING_T;

typedef struct {
    u8 ssid[32];			/**< SSID of connected AP */
    u8 ssid_len;			/**< SSID length of connected AP */
    u8 bssid[6];			/**< BSSID of connected AP*/
    u8 channel;				/**< channel of connected AP*/
} BWIFI_EVENT_STAMODE_CONNECTED_T;

typedef struct {
    u8 ssid[32];			/**< SSID of disconnected AP */
    u8 ssid_len;			/**< SSID length of disconnected AP */
    u8 bssid[6];			/**< BSSID of disconnected AP */
    u8 reason;				/**< reason of disconnection */
} BWIFI_EVENT_STAMODE_DISCONNECTED_T;

typedef struct {
    u8 old_mode;			/**< the old auth mode of AP */
    u8 new_mode;			/**< the new auth mode of AP */
} BWIFI_EVENT_STAMODE_AUTHMODE_CHANGE_T;

typedef struct {
#if LWIP_SUPPORT
    ip_addr_t ip;		/**< IP address that station got from connected AP */
    ip_addr_t mask;		/**< netmask that station got from connected AP */
    ip_addr_t gw;		/**< gateway that station got from connected AP */
#endif
} BWIFI_EVENT_STAMODE_GOT_IP_T;

typedef struct {
    u32 pac_rxc_rx_buf_in_ptr;
    u32 pac_rxc_rx_buf_out_ptr;
    u32 scheduler_events;
    u32 lmac_pc0;
    u32 lmac_pc1;
    u32 lmac_lr;
    u32 lmac_sp;
    u32 pac_ntd_status_peek;
    u32 pac_txc_status;
    u32 QUEUE_0_CONTROL;
    u32 QUEUE_1_CONTROL;
    u32 QUEUE_2_CONTROL;
    u32 QUEUE_3_CONTROL;
    u32 wlan_sw_override_1;
    u32 tsq_in_prog;
    u32 tsq_in_cmpl;
} BWIFI_LMAC_STATUS_DUMP_T;

typedef struct {
    u8 rst_flag;                         /**< reset flag to indicate status:
                                              0 - no reset; 1 - reset start; 2 - reset end.*/
    u16 error_cause;                     /**< error cause (bitmask) */
    BWIFI_LMAC_STATUS_DUMP_T dump_info;  /**< LMAC status dump on fatal error */
} BWIFI_FATAL_ERROR_RESET_T;

typedef struct {
    u16 error_cause;                     /**< error cause (bitmask) */
    BWIFI_LMAC_STATUS_DUMP_T dump_info;  /**< LMAC status dump on fatal error */
} BWIFI_EVENT_STAMODE_FATAL_ERROR_T;

//typedef struct {
//    u8 mac[6];           							/**< MAC address of the station connected to soft-AP */
//    u8 aid;           							/**< the aid that soft-AP gives to the station connected to */
//} BWIFI_EVENT_SOFTAPMODE_STACONNECTED_T;

//typedef struct {
//    u8 mac[6];		/**< MAC address of the station disconnects to soft-AP */
//    u8 aid;		/**< the aid that soft-AP gave to the station disconnects to */
//} BWIFI_EVENT_SOFTAPMODE_STADISCONNECTED_T;

//typedef struct {
//    int rssi;		/**< Received probe request signal strength */
//    u8 mac[6];		/**< MAC address of the station which send probe request */
//} BWIFI_EVENT_SOFTAPMODE_PROBEREQRECVED_T;

typedef union {
    BWIFI_EVENT_STAMODE_SCANDONE_T            scan_done;          /**< station scan (APs) done */
    BWIFI_EVENT_STAMODE_CONNECTING_T          connecting;         /**< station is connecting to AP */
    BWIFI_EVENT_STAMODE_CONNECTED_T           connected;          /**< station connected to AP */
    BWIFI_EVENT_STAMODE_DISCONNECTED_T        disconnected;       /**< station disconnected to AP */
    BWIFI_EVENT_STAMODE_AUTHMODE_CHANGE_T     auth_change;        /**< the auth mode of AP connected by station changed */
    BWIFI_EVENT_STAMODE_GOT_IP_T              got_ip;             /**< station got IP */
    BWIFI_EVENT_STAMODE_FATAL_ERROR_T         fatal_err;          /**< LMAC fatal error */
    //BWIFI_EVENT_SOFTAPMODE_STACONNECTED_T     sta_connected;      /**< a station connected to soft-AP */
    //BWIFI_EVENT_SOFTAPMODE_STADISCONNECTED_T  sta_disconnected;   /**< a station disconnected to soft-AP */
    //BWIFI_EVENT_SOFTAPMODE_PROBEREQRECVED_T   ap_probereqrecved;  /**< soft-AP received probe request packet */
} BWIFI_EVENT_INFO_U;

typedef enum {
    EVENT_STAMODE_SCAN_DONE = 0,        /**< station finish scanning AP */
    EVENT_STAMODE_CONNECTING = 1,       /**< station is connecting to AP */
    EVENT_STAMODE_CONNECTED = 2,        /**< station connected to AP */
    EVENT_STAMODE_DISCONNECTED = 3,     /**< station disconnected to AP */
    EVENT_STAMODE_AUTHMODE_CHANGE = 4,  /**< the auth mode of AP connected by station changed */
    EVENT_STAMODE_GOT_IP = 5,           /**< station got IP from connected AP */
    EVENT_STAMODE_DHCP_TIMEOUT = 6,     /**< station dhcp client got IP timeout */
    EVENT_STAMODE_FATAL_ERROR = 7,      /**< lower mac got a fatal error */
    EVENT_STAMODE_WPS_CONNECTED = 8,    /**< WPS connected to AP */
    //EVENT_SOFTAPMODE_STACONNECTED,      /**< a station connected to soft-AP */
    //EVENT_SOFTAPMODE_STADISCONNECTED,   /**< a station disconnected to soft-AP */
    //EVENT_SOFTAPMODE_PROBEREQRECVED,    /**< soft-AP received probe request packet */
    EVENT_MAX
} BWIFI_SYSTEM_EVENT;

typedef struct bwifi_event {
    BWIFI_SYSTEM_EVENT event_id;      /**< even ID */
    BWIFI_EVENT_INFO_U event_info;    /**< event information */
} BWIFI_SYSTEM_EVENT_T;

/**
  * The Wi-Fi event handler.
  *
  * No complex operations are allowed in callback.
  * If users want to execute any complex operations, please post message to another task instead.
  *
  * @param BWIFI_SYSTEM_EVENT_T *event : WiFi event
  *
  * @return null
  */
typedef void (*BWIFI_EVENT_HANDLER_CB_T)(BWIFI_SYSTEM_EVENT_T *event);

int bwifi_send_event(BWIFI_SYSTEM_EVENT_T* event);

#ifdef  __cplusplus
}
#endif
#endif /*BWIFI_STATEMACHINE_H*/
