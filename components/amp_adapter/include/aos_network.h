/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef AOS_NETWORK_H
#define AOS_NETWORK_H

#include <stdbool.h>
#include "aos/kernel.h"
#ifndef _AOS_NETWORK_H_
#define _AOS_NETWORK_H_
#define SCANNED_WIFI_COUNT_MAX 32
#define SCANNED_LOCATOR_COUNT_MAX 32
typedef enum {
    AOS_ERR_WIFI_BASE = 0x3000,         /*!< WiFi ERR NUM BASE */
    AOS_ERR_WIFI_NOT_INIT,              /*!< WiFi driver was not installed by esp_wifi_init */
    AOS_ERR_WIFI_NOT_STARTED,           /*!< WiFi driver was not started by esp_wifi_start */
    AOS_ERR_WIFI_NOT_STOPPED,           /*!< WiFi driver was not stopped by esp_wifi_stop */
    AOS_ERR_WIFI_IF,                    /*!< WiFi interface error */
    AOS_ERR_WIFI_MODE,                  /*!< WiFi mode error */
    AOS_ERR_WIFI_STATE,                 /*!< WiFi internal state error */
    AOS_ERR_WIFI_CONN,                  /*!< WiFi internal control block of station or soft-AP error */
    AOS_ERR_WIFI_NVS,                   /*!< WiFi internal NVS module error */
    AOS_ERR_WIFI_MAC,                   /*!< MAC address is invalid */
    AOS_ERR_WIFI_SSID,                  /*!< SSID is invalid */
    AOS_ERR_WIFI_PASSWORD,              /*!< Password is invalid */
    AOS_ERR_WIFI_TIMEOUT,               /*!< Timeout error */
    AOS_ERR_WIFI_WAKE_FAIL,             /*!< WiFi is in sleep state(RF closed) and wakeup fail */
    AOS_ERR_WIFI_WOULD_BLOCK,           /*!< The caller would block */
    AOS_ERR_WIFI_NOT_CONNECT,           /*!< Station still in disconnect status */
    AOS_ERR_WIFI_POST,                  /*!< Failed to post the event to WiFi task */
    AOS_ERR_WIFI_INIT_STATE,            /*!< Invalod WiFi state when init/deinit is called */
    AOS_ERR_WIFI_STOP_STATE,            /*!< Returned when WiFi is stopping */
    AOS_ERR_WIFI_NOT_ASSOC,             /*!< The WiFi connection is not associated */
    AOS_ERR_WIFI_TX_DISALLOW,           /*!< The WiFi TX is disallowed */
    AOS_ERR_TCPIP_ADAPTER_INVALID_PARAMS,
    AOS_ERR_TCPIP_ADAPTER_IF_NOT_READY,
    AOS_ERR_TCPIP_ADAPTER_DHCPC_START_FAILED,
    AOS_ERR_TCPIP_ADAPTER_NO_MEM
} AOS_NETWORK_ERR_E;

typedef enum {
    AOS_NET_NOINIT = 0,
    AOS_NET_STA_STARTED,
    AOS_NET_STA_GOT_IP,
    AOS_NET_STA_LOST_IP,
    AOS_NET_STA_DISCONNECTED,
    AOS_NET_STA_CONNECTED,
    AOS_NET_STA_STOPED,
    AOS_NET_STATE_UNKNOWN
} AOS_NETWORK_STATE_E;

typedef enum {
    AOS_NETWORK_WIFI,
    AOS_NETWORK_WIFI_STA,
    AOS_NETWORK_WIFI_AP,
    AOS_NETWORK_CELLULAR,
    AOS_NETWORK_ETHERNET,
    AOS_NETWORK_UNKNOW,
} AOS_NETWORK_TYPE_E;

typedef enum {
    AOS_NETWORK_IPTYPE_IPV4,
    AOS_NETWORK_IPTYPE_IPV6,
    AOS_NETWORK_IPTYPE_IPV4V6,
    AOS_NETWORK_IPTYPE_INVALID
} AOS_NETWORK_IPTYPE_E;

typedef enum {
    AOS_NETWORK_SHAREMODE_RNDIS,
    AOS_NETWORK_SHAREMODE_ECM,
    AOS_NETWORK_SHAREMODE_INVALID
} AOS_NETWORK_SHAREMODE_E;

typedef enum {
    AOS_NETWORK_SHAREMODE_AUTHTYPE_NONE,
    AOS_NETWORK_SHAREMODE_AUTHTYPE_PAP,
    AOS_NETWORK_SHAREMODE_AUTHTYPE_CHAP,
    AOS_NETWORK_SHAREMODE_AUTHTYPE_PAPCHAP,
    AOS_NETWORK_SHAREMODE_AUTHTYPE_INVALID
} AOS_NETWORK_SHAREMODE_AUTHTYPE_E;

typedef struct {
    void (*cb)(int status, void *);
    int wifi_state;
    int wifi_mode;
    bool is_initialized;
    bool is_started;
    pthread_mutex_t network_mutex;
} aos_wifi_manager_t;

typedef struct aos_sim_info {
    char imsi[32];
    char imei[32];
    char iccid[32];
} aos_sim_info_t;

typedef struct aos_locator_info {
    char mcc[4];
    char mnc[4];
    int cellid;
    int lac;
    int signal;
} aos_locator_info_t;

typedef struct aos_scanned_locator_info {
    int num;
    aos_locator_info_t locator_info[SCANNED_LOCATOR_COUNT_MAX];
} aos_scanned_locator_info_t;

typedef struct aos_wifi_info {
    char ssid[128];
    char mac[6];
    char ip[16];
    int rssi;
} aos_wifi_info_t;


/** @brief  network ifconfig type */
#define IPADDR_STR_LEN 16
typedef struct aos_ifconfig_info {
   bool dhcp_en;                         /**< dhcp is enabled */
   char ip_addr[IPADDR_STR_LEN];         /**< ip address */
   char mask[IPADDR_STR_LEN];            /**< ip address mask */
   char gw[IPADDR_STR_LEN];              /**< gateway ip address */
   char dns_server[IPADDR_STR_LEN];      /**< dns server address */
   char mac[IPADDR_STR_LEN + 2];         /**< mac address */
   int  rssi;                            /**< rssi */
} aos_ifconfig_info_t;

typedef struct aos_scanned_wifi_info {
    int num;
    aos_wifi_info_t wifi_info[SCANNED_WIFI_COUNT_MAX];
} aos_scanned_wifi_info_t;

typedef struct aos_sharemode_info {
    int action;
    int auto_connect;
    char apn[99];
    char username[64];
    char password[64];
    AOS_NETWORK_IPTYPE_E ip_type;
    AOS_NETWORK_SHAREMODE_E share_mode;
} aos_sharemode_info_t;

typedef struct {
    char     ssid[33];          /**< The SSID of an access point. */
    char     ap_power;            /**< Received Signal Strength Indication, min: -110, max: 0 */
    unsigned char  bssid[6];      /**< The BSSID of an access point. */
    unsigned char  channel;       /**< The RF frequency, 1-13 */
    unsigned char  sec_type;      /**< details see netmgr_wifi_sec_type */
} aos_wifi_ap_list_t;

int aos_wifi_init(aos_wifi_manager_t *wifi_manager);

int aos_wifi_start(aos_wifi_manager_t *wifi_manager);

int aos_wifi_connect(const char *ssid, const char *passwd);

int aos_wifi_stop(aos_wifi_manager_t *wifi_manager);

int aos_wifi_disconnect();

int aos_wifi_get_info(aos_wifi_info_t *wifi_info);

int aos_wifi_get_state();

int aos_wifi_deinit(aos_wifi_manager_t *wifi_manager);

int aos_net_set_ifconfig(aos_ifconfig_info_t *info);

int aos_net_get_ifconfig(aos_ifconfig_info_t *info);

int aos_wifi_scan(aos_wifi_ap_list_t *ap_list, int ap_list_len);

/**
 * @brief       file close
 *
 * @param[out]  ip: ip pointer
 *
 * @return      0: success, -1: failed
 */
int aos_get_sim_info(aos_sim_info_t *sim_info);

/**
 * @brief       file close
 *
 * @param[out]  ip: ip pointer
 *
 * @return      0: success, -1: failed
 */
int aos_get_locator_info(aos_locator_info_t *locator_info);

/**
 * @brief       file close
 *
 * @param[out]  ip: ip pointer
 *
 * @return      0: success, -1: failed
 */
int aos_get_neighbor_locator_info(void (*cb)(aos_locator_info_t *, int));

/**
 * @brief       file close
 *
 * @param[out]  ip: ip pointer
 *
 * @return      0: success, -1: failed
 */
int aos_get_network_status(void);

/**
 * @brief       file close
 *
 * @param[out]  ip: ip pointer
 *
 * @return      0: success, -1: failed
 */
AOS_NETWORK_TYPE_E aos_get_network_type();

/* ECM */
/**
 * @brief       get net share mode
 *
 * @return      mode: 0-RNDIS  1-ECM
 */
AOS_NETWORK_SHAREMODE_E aos_get_netsharemode(void);

/**
 * @brief       set net share mode
 *
 * @param[out]  share_mode:  net mode
 *
 * @return      0: success, -1: failed
 */
int aos_set_netsharemode(AOS_NETWORK_SHAREMODE_E share_mode);

/**
 * @brief       net share action
 *
 * @param[out]  action_p: net share mode: 0-close 1-open
 *
 * @return      0: success, -1: failed
 */
int aos_get_netshareconfig(aos_sharemode_info_t *share_mode_info);

/**
 * @brief       file close
 *
 * @param[out]  ip: ip pointer
 *
 * @return      0: success, -1: failed
 */
int aos_set_netshareconfig(int ucid, int auth_type, aos_sharemode_info_t *share_mode_info);

int aos_location_access_wifi_info(aos_wifi_info_t *wifi_info);

int aos_location_scaned_wifi_info(aos_scanned_wifi_info_t *scaned_wifi);

#endif /* _AOS_NETWORK_H_ */

#endif
