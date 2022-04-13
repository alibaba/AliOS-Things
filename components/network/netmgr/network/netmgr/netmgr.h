/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef NETMGR_H
#define NETMGR_H
#include <network/network.h>

#if defined(__cplusplus)
extern "C"
{
#endif

#include <stdbool.h>

#ifndef SKIP_KV_SET
#define SKIP_KV_SET 0
#endif

typedef enum {
    ETH_MAC_SPEED_10M,
    ETH_MAC_SPEED_100M,
    ETH_MAC_SPEED_1000M,
} eth_mac_speed_e;

typedef enum {
    ETH_MAC_DUPLEX_HALF,
    ETH_MAC_DUPLEX_FULL,
} eth_mac_duplex_e;

#define IPADDR_STR_LEN 16
typedef struct netmgr_ifconfig_info {
   bool dhcp_en;
   char ip_addr[IPADDR_STR_LEN];
   char mask[IPADDR_STR_LEN];
   char gw[IPADDR_STR_LEN];
   char dns_server[IPADDR_STR_LEN];
   char mac[6];
   int  rssi;
} netmgr_ifconfig_info_t;

#define NETMGR_SSID_MAX_LEN         (32)
#define NETMGR_PWD_MAX_LEN          (64)
#ifdef ETH_ALEN
#define NETMGR_BSSID_MAX_LEN        (ETH_ALEN)
#else
#define NETMGR_BSSID_MAX_LEN        (6)
#endif
typedef enum {
    NETMGR_SSID_FORMAT_UTF8,
    NETMGR_SSID_FORMAT_GBK,
} netmgr_ssid_format_e;

#define MAX_AP_CONFIG_NUM   (3)

typedef struct {
    char     ssid[NETMGR_SSID_MAX_LEN+1];
    uint8_t  pwd[NETMGR_PWD_MAX_LEN+1];
    uint8_t  bssid[NETMGR_BSSID_MAX_LEN];
    int8_t   ap_power;
    uint8_t  channel;
    uint8_t  sec_type; /*  details see netmgr_wifi_sec_type */
    bool     contain_chinese; /*  true:contain chinese false:no chinese */
    netmgr_ssid_format_e  ssid_format;
    char     gbk_ssid[NETMGR_SSID_MAX_LEN+1];
} netmgr_wifi_ap_info_t;

typedef struct {
    int ap_num;
    int used_ap;
    netmgr_wifi_ap_info_t config[MAX_AP_CONFIG_NUM];     /*  The ap information array    */
} netmgr_wifi_ap_config_t, netmgr_wifi_config_t;

typedef enum {
    NETMGR_TYPE_WIFI,
    NETMGR_TYPE_GPRS,
    NETMGR_TYPE_NBIOT,
    NETMGR_TYPE_ETH,
    NETMGR_TYPE_MAX
} netmgr_type_t;

typedef enum {
    NETMGR_MSGID_MIN = 0,
    NETMGR_MSGID_WIFI_STATUS = NETMGR_MSGID_MIN,
    NETMGR_MSGID_WIFI_STATUS_FROM_IMPL,
    NETMGR_MSGID_WIFI_TRACE_FROM_IMPL,
    NETMGR_MSGID_NETWORK_STATUS,
    NETMGR_MSGID_ETH_STATUS_FROM_IMPL,
    NETMGR_MSGID_MAX
} netmgr_msgid_t;

typedef struct netmgr_config {
    union {
        netmgr_wifi_config_t wifi_config;
        //netmgr_gprs_config_t gprs_config;
        //netmgr_nbiot_config_t nbiot_config;
        //netmgr_eth_config_t eth_config;
    } config;
} netmgr_config_t;

typedef struct netmgr_del_config {
    union {
        char ssid[NETMGR_SSID_MAX_LEN+1];
    } config;
} netmgr_del_config_t;

typedef struct netmgr_wifi_conenct_params {
    char      ssid[NETMGR_SSID_MAX_LEN+1];
    uint8_t   pwd[NETMGR_PWD_MAX_LEN+1];
    uint8_t   bssid[NETMGR_BSSID_MAX_LEN];
    uint32_t  timeout;
} netmgr_wifi_connect_params_t;

typedef struct netmgr_connect_params {
    union {
        netmgr_wifi_connect_params_t wifi_params;
    } params;
} netmgr_connect_params_t;

#define NETMGR_WIFI_METHOD_MAX_LENGTH       (32)
#define NETMGR_WIFI_STATUS_MAX_LENGTH       (32)
#define NETMGR_WIFI_SSID_MAX_LENGTH         (32)
#define NETMGR_WIFI_PASSWORD_MAX_LENGTH     (64)

typedef struct {
   char method[NETMGR_WIFI_METHOD_MAX_LENGTH+1];
   int  quantity;
   char status[NETMGR_WIFI_STATUS_MAX_LENGTH+1];
   char ssid[NETMGR_WIFI_SSID_MAX_LENGTH+1];
   char password[NETMGR_WIFI_PASSWORD_MAX_LENGTH+1];
   uint8_t reason_code;
} netmgr_wifi_network_status_change_t;

typedef struct {
    netmgr_msgid_t id;
    union {
        int   status;
        void *network_status_change;
        void *trace;
    } data;
} netmgr_msg_t;

typedef void (*netmgr_msg_cb_t)(netmgr_msg_t* msg);

typedef int netmgr_hdl_t;

typedef struct {
    char     ssid[NETMGR_SSID_MAX_LEN+1];   /* The SSID of an access point. */
    int8_t   ap_power;                               /* Received Signal Strength Indication, min: -110, max: 0 */
    uint8_t  bssid[NETMGR_BSSID_MAX_LEN];                        /* The BSSID of an access point. */
    uint8_t  channel;                                /* The RF frequency, 1-13 */
    uint8_t  sec_type;                               /* details see netmgr_wifi_sec_type */
} netmgr_wifi_ap_list_t;
/**
 * @brief net manager init
 * @return 0 on success, others on error
 */
int netmgr_service_init();

/**
 * @brief net manager deinit
 */
void netmgr_service_deinit();

/**
 * @brief get wifi netmgr handler
 * @param  [in] hdl
 * @return NULL on error
 */
netmgr_hdl_t netmgr_get_dev(const char* name);

/**
 * @brief  set if config information
 * @param  [in] hdl
 * @param  [in] if config information
 * @return 0 on success, others on error
 */
int netmgr_set_ifconfig(netmgr_hdl_t hdl, netmgr_ifconfig_info_t* info);

/**
 * @brief  get if config information
 * @param  [in] hdl
 * @param  [out] if config information
 * @return 0 on success, others on error
 */
int netmgr_get_ifconfig(netmgr_hdl_t hdl, netmgr_ifconfig_info_t* info);

/**
 * @brief  set auto reconnect flag
 * @param  [in] hdl
 * @param  [in] enable
 */
void netmgr_set_auto_reconnect(netmgr_hdl_t hdl, bool enable);

/**
 * @brief  get config information
 * @param  [in] hdl
 * @param  [out] config information
 * @return 0 on success, others on error
 */
int netmgr_get_config(netmgr_hdl_t hdl, netmgr_config_t* config);

/**
 * @brief  delete config information
 * @param  [in] hdl
 * @param  [in] config
 * @return 0 on success, others on error
 */
int netmgr_del_config(netmgr_hdl_t, netmgr_del_config_t* config);

/**
 * @brief  get state of hdl
 * @param  [in] hdl
 * @return >=0 on success, others on error
 */
int netmgr_get_state(netmgr_hdl_t hdl);

/**
 * @brief  netmgr connect network
 * @param  [in] hdl
 * @param  [in] params
 * @return 0 on success, others on error
 */
int netmgr_connect(netmgr_hdl_t hdl, netmgr_connect_params_t* params);

/**
 * @brief  netmgr disconnect network
 * @param  [in] hdl
 * @return 0 on success, others on error
 */
int netmgr_disconnect(netmgr_hdl_t hdl);

/**
 * @brief  netmgr save config
 * @param  [in] hdl
 * @return 0 on success, others on error
 */
int netmgr_save_config(netmgr_hdl_t hdl);

/**
 * @brief  netmgr set connect params
 * @param  [in] hdl
 * @param  [in] ap_info
 * @param  [in] num
 * @param  [in] type
 * @return 0 on success, others on error
 */
int netmgr_set_connect_params(netmgr_hdl_t hdl, netmgr_connect_params_t* params);

int netmgr_set_msg_cb(netmgr_hdl_t hdl, netmgr_msg_cb_t cb);

int netmgr_del_msg_cb(netmgr_hdl_t hdl, netmgr_msg_cb_t cb);
#if AOS_NETWORK_WITH_WIFI
void* netmgr_wifi_get_netif(netmgr_hdl_t hdl);
int netmgr_wifi_scan_result(netmgr_hdl_t hdl, netmgr_wifi_ap_list_t* ap_info, int num, netmgr_wifi_scan_type_t type);

void netmgr_wifi_start_monitor(netmgr_hdl_t hdl);

void netmgr_wifi_stop_monitor(netmgr_hdl_t hdl);

void netmgr_wifi_start_mgnt_monitor(netmgr_hdl_t hdl);

void netmgr_wifi_stop_mgnt_monitor(netmgr_hdl_t hdl);

void netmgr_wifi_register_mgnt_monitor_cb(netmgr_hdl_t hdl, monitor_data_cb_t fn);

int netmgr_wifi_send_80211_raw_frame(netmgr_hdl_t hdl, uint8_t *buf, int len);
#endif
#ifdef __cplusplus
}
#endif

#endif

