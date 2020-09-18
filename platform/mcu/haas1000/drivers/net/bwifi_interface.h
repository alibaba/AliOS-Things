/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef WIFI_INTERFACE_H
#define WIFI_INTERFACE_H
#include "plat_types.h"

#include "bwifi_sta.h"
#if LWIP_ETHERNETIF || LWIP_SUPPORT
#include "lwip/ip_addr.h"
#include "lwip/netif.h"
#endif
#include "net_debug.h"
#include "bwifi_event.h"
#include "nvrecord_wifi.h"

#ifdef  __cplusplus
extern "C"
{
#endif

/* BES SDK definition of wifi work mode */
typedef enum {
    WIFI_SNIFFER,        ///< wifi work in sniffer mode
    WIFI_STATION,        ///< wifi work in station mode
    WIFI_SOFTAP,         ///< wifi work in ap mode
    WIFI_STATIONAP,      ///< wifi work in station+ap mode
} BES_WIFI_MODE_T;

typedef enum {
    WIFI_IF_STATION,
    WIFI_IF_SOFTAP,
} BWIFI_INTF_TYPE_T;

typedef enum bwifi_security_type {
    SECURITY_NONE,       /*!< open access point */
    SECURITY_WEP40,      /*!< phrase conforms to WEP */
    SECURITY_WEP104,     /*!< phrase conforms to WEP EXTENDED*/
    SECURITY_WPA,        /*!< WPA-PSK */
    SECURITY_WPA2,       /*!< WPA2-PSK */
    SECURITY_WPA_WPA2,   /*!< WPA WPA2 mixed mode */
} BWIFI_SEC_TYPE_T;

#ifndef ETH_ALEN
#define ETH_ALEN 6
#endif

struct bwifi_bss_info {
    //struct list_head list;
    u8 bssid[ETH_ALEN];
    u8 ssid[32+1];
    //u8 ssid_len;
    u8 channel;
    s8 rssi;
    //BWIFI_SEC_TYPE_T security_type;
    //u8 hidden;
    //u16 capability;
    //u8 wmm;
    u8 *ie;//user program couldn't free(ie);
    u32 ie_length;
};

typedef enum {
    WIFI_SCAN_TYPE_ACTIVE = 0,  /**< active scan , default val*/
    WIFI_SCAN_TYPE_PASSIVE,     /**< passive scan */
} BWIFI_SCAN_TYPE_T;

/** Range of active scan times per channel */
typedef struct {
    uint32_t min;  /**< minimum active scan time per channel, units: millisecond, 0��use default val */
    uint32_t max;  /**< maximum active scan time per channel, units: millisecond, 0��use default val
                        values above 1500ms may cause station to disconnect from AP and are not recommended.  */
} BWIFI_ACTIVE_SCAN_TIME_T;

/** Aggregate of active & passive scan time per channel */
typedef union {
    BWIFI_ACTIVE_SCAN_TIME_T active;  /**< active scan time per channel, units: millisecond. 0��use default val */
    uint32_t passive;               /**< passive scan time per channel, units: millisecond, 0��use default val
                                        values above 1500ms may cause station to disconnect from AP and are not recommended. */
} BWIFI_SCAN_TIME_T;

struct bwifi_ssid {
    struct bwifi_ssid *next;
    u8 ssid[32+1]; //null-terminated string
};

struct bwifi_scan_config {
    struct bwifi_ssid *ssids;        /**< List of specified SSIDs */
    //u8 bssid[ETH_ALEN];
    int *channels;                   /**< Array of channels (zero-terminated) to scan or NULL for all channels */
    //u8 hidden;
    //BWIFI_SCAN_TYPE_T scan_type;     /**< scan type, active or passive */
    //BWIFI_SCAN_TIME_T scan_time;     /**< scan time per channel */
};

typedef enum bwifi_operating_status {
    BWIFI_STATUS_IDLE           = 0,
    BWIFI_STATUS_DISCONNECTING,
    BWIFI_STATUS_SCANNING,
    BWIFI_STATUS_CONNECTING,
    BWIFI_STATUS_WPS_CONNECTING,
    BWIFI_STATUS_CONNECTED,
    BWIFI_STATUS_DHCPING,
    BWIFI_STATUS_GOT_IP,
    BWIFI_STATUS_ONLINE_SCANNING,
    BWIFI_STATUS_END_DEF,  /* must be the last one */
} BWIFI_STATUS_T;

typedef enum bwifi_softap_status {
    BWIFI_SOFTAP_STATUS_OFF   = 0,
    BWIFI_SOFTAP_STATUS_ON,
} BWIFI_SOFTAP_STATUS_T;

struct ip_info {
#if LWIP_ETHERNETIF || LWIP_SUPPORT
    ip_addr_t ip;      /**< IP address */
    ip_addr_t netmask; /**< netmask */
    ip_addr_t gw;      /**< gateway */
#endif
};

enum {
    BWIFI_R_OK                  = 0,
    BWIFI_R_COMMON_FAIL         = -1,
    BWIFI_R_INVALID_ARG         = -2,  //invalid argument
    BWIFI_R_INVALID_PASSWORD    = -3,  //invalid password
    BWIFI_R_MEMORY_ERROR        = -4,  //no memory to allocate resource
    BWIFI_R_INIT_FAIL           = -5,  //init wifi fail
    BWIFI_R_NOT_INITED          = -6,  //wifi is not initialized
    BWIFI_R_STATUS_ERROR        = -7,  //request in error STATUS
    BWIFI_R_SCAN_REQ_FAIL       = -8,  //scan fail to start
    BWIFI_R_SCAN_NO_AP_FOUND    = -9,  //scan result is NULL (didn't find any SSID)
    BWIFI_R_NO_SUITABLE_NETWORK = -10, //no suitable network to connect
    BWIFI_R_CONN_REQ_FAIL       = -11, //connect fail to start
    BWIFI_R_CONN_FAIL           = -12, //connect procedure result in fail
    BWIFI_R_CONN_NO_SSID_CONFIG = -13, //no saved SSID config to connect
    BWIFI_R_DISC_FAIL           = -14, //disconnect procedure result in fail
    BWIFI_R_WPS_NOT_FOUND       = -15, //couldn't find WPS AP
    BWIFI_R_WPS_REQ_FAIL        = -16, //WPS fail to start
};


/*
 * set country code. if you don't set it, wifi will use default country code (0)
 */
int bwifi_set_country_code(char * country);
int bwifi_get_country_code(void);
int bwifi_set_band(uint8 band);
int bwifi_get_band(void);
void bwifi_band_switch(uint8 band);

typedef enum {
    WIFI_USER_EVT_CONN_INTER_STATE,
    WIFI_USER_EVT_CONNECTED,
    WIFI_USER_EVT_GOT_IP,
    WIFI_USER_EVT_DISCONNECTED,
    WIFI_USER_EVT_FATAL_ERROR,
    WIFI_USER_EVT_MAX
} WIFI_USER_EVT_ID;

typedef void (*user_evt_handler_t)(WIFI_USER_EVT_ID evt_id, void *arg);
void bwifi_reg_user_evt_handler(WIFI_USER_EVT_ID evt_id, user_evt_handler_t cb);
void bwifi_unreg_user_evt_handler(WIFI_USER_EVT_ID evt_id);

typedef enum {
	WIFI_PARAM_RETRY_SHORT = 0,
	WIFI_PARAM_RETRY_LONG,
	WIFI_PARAM_FRAG_THRESHOLD,
	WIFI_PARAM_RTS_THRESHOLD
}WIFI_MAC_PARAM;

/**
 * struct bwifi_trans_config - Configuration of the transceived frames's task.
 * @en: enable/disable the task which will output statistics results periodically
 * @interval_sec: period time
 * @thread_id: task's id.
 */
typedef struct bwifi_stat_cfg_t {
    osThreadId  thread_id;
    uint8_t     en;
    uint8_t     interval_sec;
} bwifi_stat_cfg;

/**
 * struct bwifi_trans_stat - Statistics of the transceived frames.
 * @tx_succ_cnt: Number of the frames that transmitted successfully.
 * @tx_fail_cnt: Number of the frames that transmitted failed.
 * @tx_retry_cnt: Number of the retried frames.
 * @rx_cnt: Number of the received frames.
 */
typedef struct bwifi_trans_stat_t {
    uint32_t tx_succ_cnt;
    uint32_t tx_fail_cnt;
    uint32_t tx_retry_cnt;
    uint32_t rx_cnt;
} bwifi_trans_stat;

/*
 * init wifi interface.
 */
int bwifi_init(void);


/* wifi_record interface */
int bwifi_find_record(const char *type, nvrec_wifidevicerecord *record);
int bwifi_add_record(nvrec_wifidevicerecord *record);
int bwifi_del_record(char *type);

/*
 * add AP config to wpa's configuration
 */
int bwifi_add_config(struct bwifi_station_config *config);
/*
 * get the number of AP configs in wpa's configuration
 */
int bwifi_count_configured_ssid(void);
/*
 * get current AP configs in wpa's configuration
 * return the number of APs
 */
int bwifi_get_current_config(struct bwifi_station_config *config, int count);
/*
 * save AP config to flash (max 8)
 */
int bwifi_save_config(struct bwifi_station_config *config);
/*
 * get AP configs saved in flash
 * return the number of APs
 */
int bwifi_get_saved_config(struct bwifi_station_config *config, int count);
/*
 * find AP config saved in flash who matches with the specified ssid
 */
int bwifi_find_saved_config_by_ssid(const char *ssid, struct bwifi_station_config *config);
/*
 * del AP config saved in wpa and flash. it will check ssid, passwd, hidden, web_keyid, bssid(if not zero)
 * if config == NULL, del all
 * if strlen(config->ssid)==0, del all
 */
int bwifi_del_config(struct bwifi_station_config *config);
/*
 * del all AP configs saved in wpa and flash
 */
int bwifi_del_all_config(void);

/*
 *  Callback function for wifi scan.
 *
 * @param  void *arg : information of APs that are found; save them as linked list;
 * @param  STATUS status : status of scanning
 */
typedef void(* BWIFI_SCAN_DONE_CB_T) (int status);

/*
 * scan for wildcard ssid and saved hidden ssids
 * you can get bss list from bwifi_get_scan_result
 * return bss number or error code
 */
int bwifi_scan(void);
/*
 * scan the specified ssid(if not NULL) and the specified channel(if no 0)
 * you can get bss list from bwifi_get_scan_result
 * return bss number or error code
 */
int bwifi_config_scan(struct bwifi_scan_config *config);

/*
 * get scan result
 */
int bwifi_get_scan_result(struct bwifi_bss_info *result, int count);

/*
 * clear saved scan list which is not in use
 */
void bwifi_flush_scan_result(void);

/*
 * connect to specified ssid which is assigned by bwifi_add_config()
 * if user wants to connect to another AP after connected:
 * 1.bwifi_disconnect(), 2.bwifi_connect(config);
 */
int bwifi_connect(struct bwifi_station_config *config);

/*
 * auto connect to an AP saved in wpa's configuration
 */
int bwifi_auto_connect(void);

/*
 * connect to ssid interface for compatibility.
 * this function will add the AP config to current configuration
 * passwd=NULL: unencrypted AP
 * bssid can be NULL
 */
int bwifi_connect_to_ssid(const char *ssid, const char *passwd, int8 wep_keyid, u8 hidden, u8 *bssid);

int bwifi_get_quick_connect_config(struct bwifi_quick_connect_config *quick_config);
int bwifi_set_quick_connect_config(struct bwifi_quick_connect_config *quick_config);
int bwifi_del_quick_connect_config();
int bwifi_quick_connect(struct bwifi_quick_connect_config *quick_config);

/*
 * disconnect to current connected AP or stop connecting to AP
 */
int bwifi_disconnect(void);

/*
 * connect an AP via WPS PBC
 */
int bwifi_connect_wps_pbc(void);

/*
 * set whether wifi will connect to the recorded ap automaticall after power on
 * default val true
 */
//int bwifi_set_auto_connect_policy(bool set);//user must call bwifi_auto_connect() to auto connect
/*
 * get auto connect policy
 */
//bool bwifi_get_auto_connect_policy();

/*
 * set whether wifi will reconnect to AP after disconnection
 * default val true
 */
void bwifi_set_reconnect_policy(bool enable);
/*
 * get reconnect policy
 */
bool bwifi_get_reconnect_policy(void);

/*
 * return wifi current status
 */
BWIFI_STATUS_T bwifi_get_current_status(void);
/*
 * get AP info which is connected now
 */
int bwifi_get_current_ssid(char *ssid);
int bwifi_get_current_bssid(u8 *bssid);
int bwifi_get_own_mac(u8 *addr);
u8 bwifi_get_current_channel(void);
int8 bwifi_get_current_rssi(void);

#if LWIP_SUPPORT
/*
 * enable or disable using the static IP for subsequent connection.
 *
 * The DHCP clent is enabled by default and collides with the static IP.
 * If this API is callbed with a valid pointer to the ip_info struct,
 * DHCP client will be disabled and the static IP in ip_info will be used;
 * if this API is called with NULL, then DHCP client will be enabled.
 * It depends on the latest configuration.
 */
int bwifi_set_static_ip(struct ip_info *ip);

/*
 * get ip addr of wifi station (dhcpc or static ip)
 */
int bwifi_get_current_ip(struct ip_info *ip);
#endif

#if LWIP_ETHERNETIF && !LWIP_SUPPORT
/*
 * get netif struct of wifi station or softap
 *
 * This API should only be called when LWIP_SUPPORT is turned off in SDK
 * but ethernet interface struct is added and initialized by us,
 * return the netif struct to upper layer LWIP stack for further management.
 */
struct netif *bwifi_get_netif(BWIFI_INTF_TYPE_T type);

/*
 * set netif ip addr to wifi mac layer for ARP filter feature.
 *
 * This API should only be called when LWIP_SUPPORT is turned off in SDK
 * and DHCP procedure is taken over by the upper layer LWIP stack,
 * we need upper layer to tell us the local ip addr.
 */
int bwifi_set_ip_addr(BWIFI_INTF_TYPE_T type, struct ip_info *ip);
#endif

/*
 * airkiss_start() use wechat's airkiss lib
 * bes_airkiss_start() use bes's airkiss lib. you should include "bes_sniffer.h" if you want to use bes_airkiss_start()
 */
int airkiss_start(uint8 * ssid, uint8 * pwd, uint8 * token);
void airkiss_notify(uint8 token);

/*
 * Set powersave mode for legacy Wi-Fi.
 * @ps: 0 = disable, 1 = enable
 * Returns: 0 on success or -1 on failure
 */
int bwifi_set_ps_mode(int ps);

#ifdef __AP_MODE__

int bwifi_softap_start(void);
void bwifi_softap_stop(void);
int bwifi_set_softap_config(char *ssid, u8 channel, u8 hidden, BWIFI_SEC_TYPE_T security, char *passwd);
void bwifi_set_softap_max_sta_num(int sta_num);
void bwifi_set_softap_country_code(char *country_code);

#endif

/*
 * The function return current softap status
 */
BWIFI_SOFTAP_STATUS_T bwifi_get_softap_status(void);

/*
* set mac layer parameters
*/
int bwifi_mac_params_set(uint8_t dev_idx, WIFI_MAC_PARAM type, uint32_t value);

/**
 * This function is used to enable/disable the statistics of the frames that
 * been sent out in the last interval_sec time.
 * @param[in]	en Set it to 1 to enable the statistics ,0 to disable it.
 * @param[in]	interval_sec Time of the statistics in seconds. 
 * @return	0 ,OK; negative value,  ERROR
 * @note	If enabled, the statistics information will be output throuth a default log uart.
 */
int8 bwifi_trans_stat_en(uint8_t en, uint8_t interval_sec);

/**
 * This function is used to get trans statistics which can be used to evaluate network traffic.
 * @param[out]  stat, pointer to a bwifi_trans_stat type variable.
 * @param[in]  clear, set 1 to clear & 0 to keep.
 * @return  0.
 * @note	Elements(succ_count,fail_count,retry_count) in trans_stat will be cleared every time
 *          this function is called if clear is set to 1.
 *
 *          Instructions for use:
 *          bwifi_trans_stat test;
 *          bwifi_trans_stat_get(&test, 0)/bwifi_trans_stat_get(&test, 1);
 */
int8 bwifi_trans_stat_get(bwifi_trans_stat *stat, int8 clear);

/*
 * This function is used to enable/disable Wi-Fi recovery mechanism on fatal error.
 * which is turned off by default for debug purpose.
 */
void bwifi_recovery_enable(bool en);

/**
  * @brief  Register the Wi-Fi event handler.
  *
  * @param  BWIFI_EVENT_HANDLER_CB_T cb : callback function
  */
//int bwifi_set_event_handler_cb(BWIFI_EVENT_HANDLER_CB_T cb);

/*
 * private interface, user app should NOT use it
 */
int wpas_wps_wildcard_ok(u8 *ie, u32 ie_len);
int wpa_interface_wps_pbc(u8 *bssid);
int wpa_interface_add_network(struct bwifi_station_config *config);
int wpa_interface_count_configured_ssid();
int wpa_interface_get_current_config(struct bwifi_station_config *config, int count);
int wpa_interface_del_config(u8 *ssid, u8 *passwd, int wep_keyid, u8 hidden, u8 *bssid);
int wpa_interface_nv_add_network(struct bwifi_station_config *config);
int wpa_interface_nv_read_network(struct bwifi_station_config *config, int count);
int wpa_interface_nv_find_network_by_ssid(const char *ssid, struct bwifi_station_config *config);
int wpa_interface_scan();
int wpa_interface_config_scan(struct bwifi_scan_config *scan_config);
void wpa_interface_flush_scan_result();
int wpa_interface_get_scan_result(struct bwifi_bss_info *result, int count);
int wpa_interface_auto_connect();
int wpa_interface_disconnect();
int wpa_interface_enable_all_network(int enable);
int wpa_interface_get_current_ssid(char *ssid);
int wpa_interface_get_current_bssid(u8 *bssid);
int wpa_interface_get_own_addr(u8 *addr);
u8 wpa_interface_get_current_channel();
int8 wpa_interface_get_current_rssi();
int wpa_interface_set_ps_mode(int ps);
void wpa_interface_fatal_error();

typedef struct
{
	uint8_t mac[6]; /* mac address */
}bwifimacaddr;
typedef void(*hostapd_rx_mgmt)(uint8_t *buf, int buf_len, bwifimacaddr *srcmac, bwifimacaddr *dstmac);

#ifdef  __cplusplus
}
#endif

#endif /*WIFI_INTERFACE_H*/
