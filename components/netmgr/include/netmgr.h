/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef NETMGR_H
#define NETMGR_H
#include <sys/socket.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#include <uservice/uservice.h>

/**
 * @defgroup netmgr_api netmgr
 * @{
 */

/** @brief  ethernet speed type */
typedef enum {
    ETH_MAC_SPEED_10M,
    ETH_MAC_SPEED_100M,
    ETH_MAC_SPEED_1000M,
} eth_mac_speed_e;

/** @brief  ethernet duplex type */
typedef enum {
    ETH_MAC_DUPLEX_HALF,
    ETH_MAC_DUPLEX_FULL,
} eth_mac_duplex_e;

#define IPADDR_STR_LEN 16
/** @brief  this struct defines netmgr ifconfig info */
typedef struct netmgr_ifconfig_info {
   bool dhcp_en;                         /**< dhcp is enabled */
   char ip_addr[IPADDR_STR_LEN];         /**< ip address */
   char mask[IPADDR_STR_LEN];            /**< ip address mask */
   char gw[IPADDR_STR_LEN];              /**< gateway ip address */
   char dns_server[IPADDR_STR_LEN];      /**< dns server address */
   char mac[6];                          /**< mac address */
   int  rssi;                            /**< rssi */
} netmgr_ifconfig_info_t;

#define NETMGR_SSID_MAX_LEN         (32)
#define NETMGR_PWD_MAX_LEN          (64)
#ifdef ETH_ALEN
#define NETMGR_BSSID_MAX_LEN        (ETH_ALEN)
#else
#define NETMGR_BSSID_MAX_LEN        (6)
#endif

/** @brief  ssid string format */
typedef enum {
    NETMGR_SSID_FORMAT_UTF8,
    NETMGR_SSID_FORMAT_GBK,
} netmgr_ssid_format_e;

#define MAX_AP_CONFIG_NUM   (3)

/** @brief  this struct defines wifi ap info */
typedef struct {
    char     ssid[NETMGR_SSID_MAX_LEN+1];       /**< ssid of wifi ap */
    uint8_t  pwd[NETMGR_PWD_MAX_LEN+1];         /**< password of wifi ap */
    uint8_t  bssid[NETMGR_BSSID_MAX_LEN];       /**< bssid of wifi ap */
    int8_t   ap_power;                          /**< signal strength of wifi ap */
    uint8_t  channel;                           /**< signal channel of wifi ap */
    uint8_t  sec_type;                          /**< details see netmgr_wifi_sec_type */
    bool     contain_chinese;                   /**< true:contain chinese false:no chinese */
    netmgr_ssid_format_e  ssid_format;          /**< ssid string format */
    char     gbk_ssid[NETMGR_SSID_MAX_LEN+1];   /**< gbk ssid string */
} netmgr_wifi_ap_info_t;

/** @brief  this struct defines wifi ap config */
typedef struct {
    int ap_num;                                          /**< ap number of array */
    int used_ap;                                         /**< ap that is used in the array */
    netmgr_wifi_ap_info_t config[MAX_AP_CONFIG_NUM];     /**< The ap information array */
} netmgr_wifi_ap_config_t, netmgr_wifi_config_t;

/** @brief  network type */
typedef enum {
    NETMGR_TYPE_WIFI,
    NETMGR_TYPE_GPRS,
    NETMGR_TYPE_NBIOT,
    NETMGR_TYPE_ETH,
    NETMGR_TYPE_UNKNOWN,
    NETMGR_TYPE_MAX
} netmgr_type_t;

/** @brief  netmgr message type */
typedef enum {
    NETMGR_MSGID_MIN = 0,
    NETMGR_MSGID_WIFI_STATUS = NETMGR_MSGID_MIN,
    NETMGR_MSGID_WIFI_STATUS_FROM_IMPL,
    NETMGR_MSGID_WIFI_TRACE_FROM_IMPL,
    NETMGR_MSGID_NETWORK_STATUS,
    NETMGR_MSGID_ETH_STATUS_FROM_IMPL,
    NETMGR_MSGID_MAX
} netmgr_msgid_t;

/** @brief  netmgr config struct */
typedef struct netmgr_config {
    netmgr_type_t type;
    union {
        netmgr_wifi_config_t wifi_config;          /**< wifi config struct */
        //netmgr_gprs_config_t gprs_config;
        //netmgr_nbiot_config_t nbiot_config;
        //netmgr_eth_config_t eth_config;
    } config;
} netmgr_config_t;

/** @brief  netmgr delete config */
typedef struct netmgr_del_config {
    netmgr_type_t type;
    union {
        char ssid[NETMGR_SSID_MAX_LEN+1];      /**< wifi ssid to delete */
    } config;
} netmgr_del_config_t;

/** @brief  netmgr wifi connect params */
typedef struct netmgr_wifi_conenct_params {
    char        ssid[NETMGR_SSID_MAX_LEN+1];   /**< wifi ssid to connect*/
    char        pwd[NETMGR_PWD_MAX_LEN+1];     /**< wifi password to connect */
    char        bssid[NETMGR_BSSID_MAX_LEN];   /**< wifi bssid to connect */
    int         timeout;                       /**< wifi connect timeout */
} netmgr_wifi_connect_params_t;

/** @brief  netmgr connect params */
typedef struct netmgr_connect_params {
    netmgr_type_t type;
    union {
        netmgr_wifi_connect_params_t wifi_params;     /**< wifi connect params */
    } params;
} netmgr_connect_params_t;

#define NETMGR_WIFI_METHOD_MAX_LENGTH       (32)
#define NETMGR_WIFI_STATUS_MAX_LENGTH       (32)
#define NETMGR_WIFI_SSID_MAX_LENGTH         (32)
#define NETMGR_WIFI_PASSWORD_MAX_LENGTH     (64)

/** @brief  this struct defines netmgr wifi status change info */
typedef struct {
   char method[NETMGR_WIFI_METHOD_MAX_LENGTH+1];       /**< status change method */
   int  quantity;                                      /**< signal quantity */
   char status[NETMGR_WIFI_STATUS_MAX_LENGTH+1];       /**< current status */
   char ssid[NETMGR_WIFI_SSID_MAX_LENGTH+1];           /**< ssid of wifi */
   char password[NETMGR_WIFI_PASSWORD_MAX_LENGTH+1];   /**< password of wifi */
   uint8_t reason_code;                                /**< reason of status change */
} netmgr_wifi_network_status_change_t;

/** @brief  this struct defines netmgr message */
typedef struct {
    netmgr_msgid_t id;                        /**< netmgr msg id */
    union {
        int   status;                         /**< reason of status change */
        void *network_status_change;          /**< msg content of status change */
        void *trace;
    } data;
} netmgr_msg_t;

/** @brief  this struct defines netmgr message callback function */
typedef void (*netmgr_msg_cb_t)(netmgr_msg_t* msg);

typedef int netmgr_hdl_t;

/** @brief  this struct defines netmgr message callback function */
typedef struct {
    char     ssid[NETMGR_SSID_MAX_LEN+1];            /**< The SSID of an access point. */
    int8_t   ap_power;                               /**< Received Signal Strength Indication, min: -110, max: 0 */
    uint8_t  bssid[NETMGR_BSSID_MAX_LEN];            /**< The BSSID of an access point. */
    uint8_t  channel;                                /**< The RF frequency, 1-13 */
    uint8_t  sec_type;                               /**< details see netmgr_wifi_sec_type */
} netmgr_wifi_ap_list_t;

/** @brief  netmgr connection state */
typedef enum netmgr_conn_state{
    CONN_STATE_DISCONNECTING,
    CONN_STATE_DISCONNECTED,
    CONN_STATE_CONNECTING,
    CONN_STATE_CONNECTED,
    CONN_STATE_OBTAINING_IP,
    CONN_STATE_NETWORK_CONNECTED,
    CONN_STATE_FAILED,
    CONN_STATE_UNKNOWN
} netmgr_conn_state_t;

/**
 * @brief net manager init.
 *
 * @param[in]  task    NULL for current realize.
 *
 * @return  0 on success, negative error on failure.
 */
int netmgr_service_init(utask_t *task);

/**
 * @brief net manager deinit
 */
void netmgr_service_deinit();

/**
 * @brief add netmgr dev
 *
 * @param  [in] name    name of netmgr dev
 *
 * @return  0 on success, negative error on failure.
 */
int netmgr_add_dev(const char* name);

/**
 * @brief get netmgr handler
 *
 * @param  [in] name    name of netmgr dev
 *
 * @return  negative error on failure, others on success.
 */
netmgr_hdl_t netmgr_get_dev(const char* name);

/**
 * @brief  set if config information
 *
 * @param  [in] hdl     netmgr handle
 * @param  [in] info    config information
 *
 * @return >=0 on success, others on error
 */
int netmgr_set_ifconfig(netmgr_hdl_t hdl, netmgr_ifconfig_info_t* info);

/**
 * @brief  get if config information
 *
 * @param  [in]  hdl    netmgr handle
 * @param  [out] info   config information
 *
 * @return 0 on success, others on error
 */
int netmgr_get_ifconfig(netmgr_hdl_t hdl, netmgr_ifconfig_info_t* info);

/**
 * @brief  set auto reconnect flag
 *
 * @param  [in] hdl     netmgr handle
 * @param  [in] enable  enable auto reconnect
 */
void netmgr_set_auto_reconnect(netmgr_hdl_t hdl, bool enable);

/**
 * @brief  netmgr connect network
 *
 * @param  [in] hdl       netmgr handle
 * @param  [in] params    netmgr connect params
 *
 * @return 0 on success, others on error
 */
int netmgr_connect(netmgr_hdl_t hdl, netmgr_connect_params_t* params);

/**
 * @brief  netmgr disconnect network
 *
 * @param  [in] hdl       netmgr handle
 *
 * @return 0 on success, others on error
 */
int netmgr_disconnect(netmgr_hdl_t hdl);

/**
 * @brief  get state of hdl
 *
 * @param  [in] hdl      netmgr handle
 *
 * @return Please refer to #netmgr_conn_state_t
 */
netmgr_conn_state_t netmgr_get_state(netmgr_hdl_t hdl);

/**
 * @brief  netmgr save config
 *
 * @param  [in] hdl       netmgr handle
 *
 * @return 0 on success, others on error
 */
int netmgr_save_config(netmgr_hdl_t hdl);

/**
 * @brief  get config information
 *
 * @param  [in]  hdl      netmgr handle
 * @param  [out] config   netmgr config information
 *
 * @return 0 on success, others on error
 */
int netmgr_get_config(netmgr_hdl_t hdl, netmgr_config_t* config);

/**
 * @brief  delete config information
 *
 * @param  [in] hdl       netmgr handle
 * @param  [in] config    netmgr config information
 *
 * @return 0 on success, others on error
 */
int netmgr_del_config(netmgr_hdl_t, netmgr_del_config_t* config);

/**
 * @brief  netmgr set message callback
 *
 * @param  [in] hdl          netmgr handle
 * @param  [in] cb           netmgr msg callback
 *
 * @return 0 on success, others on error
 */
int netmgr_set_msg_cb(netmgr_hdl_t hdl, netmgr_msg_cb_t cb);

/**
 * @brief  netmgr delete message callback
 *
 * @param  [in] hdl          netmgr handle
 * @param  [in] cb           netmgr msg callback
 *
 * @return 0 on success, others on error
 */
int netmgr_del_msg_cb(netmgr_hdl_t hdl, netmgr_msg_cb_t cb);

#ifdef AOS_NETWORK_WITH_WIFI
/**
 * @brief  netmgr wifi get net interface
 *
 * @param  [in] hdl          netmgr handle
 *
 * @return net interface
 */
void* netmgr_wifi_get_netif(netmgr_hdl_t hdl);

/**
 * @brief  netmgr set connect params
 *
 * @param  [in] hdl          netmgr handle
 * @param  [out] ap_info     ap information
 * @param  [in] num          number of ap got
 * @param  [in] type         scan type
 *
 * @return 0 on success, others on error
 */
int netmgr_wifi_scan_result(netmgr_hdl_t hdl, netmgr_wifi_ap_list_t* ap_info, int num, netmgr_wifi_scan_type_t type);

/**
 * @brief  netmgr wifi start monitor
 *
 * @param  [in] hdl         netmgr handle
 */
void netmgr_wifi_start_monitor(netmgr_hdl_t hdl);

/**
 * @brief  netmgr wifi stop monitor
 *
 * @param  [in] hdl
 */
void netmgr_wifi_stop_monitor(netmgr_hdl_t hdl);

/**
 * @brief  netmgr wifi start management monitor
 *
 * @param  [in] hdl        netmgr handle
 */
void netmgr_wifi_start_mgnt_monitor(netmgr_hdl_t hdl);

/**
 * @brief  netmgr wifi stop management monitor
 *
 * @param  [in] hdl        netmgr handle
 */
void netmgr_wifi_stop_mgnt_monitor(netmgr_hdl_t hdl);

/**
 * @brief  netmgr register management monitory callback
 *
 * @param  [in] hdl        netmgr handle
 * @param  [in] fn         callback of management monitor
 */
void netmgr_wifi_register_mgnt_monitor_cb(netmgr_hdl_t hdl, monitor_data_cb_t fn);

/**
 * @brief  netmgr set connect params
 *
 * @param  [in] hdl        netmgr handle
 * @param  [in] buf        raw frame to send in 80211
 * @param  [in] len        length of raw frame
 *
 * @return 0 on success, others on error
 */
int netmgr_wifi_send_80211_raw_frame(netmgr_hdl_t hdl, uint8_t *buf, int len);
#endif

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif

