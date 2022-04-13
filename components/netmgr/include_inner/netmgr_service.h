/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef __NETMGR_SERVICE_H_
#define __NETMGR_SERVICE_H_

#include <aos/aos.h>

#include <uservice/uservice.h>
#include <uservice/event.h>

#include <netmgr.h>
#include <vfsdev/wifi_dev.h>

#ifdef __cplusplus
extern "C" {
#endif

#define API_NET_INIT        0
#define API_NET_GET_HDL     1
//#define API_NET_MACCONFIG   2
#define API_NET_IPCONFIG    3
#define API_NET_START       5
#define API_NET_STOP        6
#define API_NET_SUBSCRIBE   7
#define API_NET_INFO        8
#define API_NET_RESET       9
#define API_NET_DHCP_CHECK  10

#define API_WIFI_CONFIG_SSID_PSK    11

#define API_ETH_CONFIG      21

#define API_GPRS_CONFIG     31

#define API_WIFI_END        99

/* kv key */
#define KV_WIFI_EN          "wifi_en"
#define KV_WIFI_SSID        "wifi_ssid"
#define KV_WIFI_PSK         "wifi_psk"

#define KV_ETH_EN           "eth_en"
#define KV_ETH_DUPLEX       "eth_dup"
#define KV_ETH_SPEED        "eth_speed"

#define KV_DHCP_EN          "dhcp_en"
#define KV_IP_ADDR          "ip_addr"
#define KV_IP_NETMASK       "netmask"
#define KV_IP_GW            "gw"

#define KV_NBIOT_EN         "nbiot_en"

#define KV_GPRS_EN          "gprs_en"

#define WIFI_SSID_MAX_LEN 32
#define WIFI_PSK_MAX_LEN 64

#define WIFI_CHANNEL_MAX 14
#define WIFI_CHANNEL_ANY 255

typedef struct wifi_ssid_psk_s {
    char ssid[WIFI_SSID_MAX_LEN + 1];
    int ssid_length; /* Max 32 */

    char psk[WIFI_PSK_MAX_LEN + 1];
    int psk_length; /* Min 8 - Max 64 */

    uint8_t channel;
} wifi_ssid_psk_t;

typedef struct {
    wifi_ssid_psk_t ssid_psk;
} wifi_setting_t;

typedef struct eth_setting_s {
    eth_mac_speed_e speed;
    eth_mac_duplex_e duplex;
} eth_setting_t;

typedef struct gprs_setting_s {
    int foo;
} gprs_setting_t;

typedef struct {
    int dhcp_en;
    char *ipaddr;
    char *netmask;
    char *gw;
} ip_setting_t;

//members in node reflect data in nvram
#define NETMGR_NAME_LEN (16)
typedef struct netmgr_dev_s {
    slist_t     next;

    char name[NETMGR_NAME_LEN];
    union {
        wifi_setting_t wifi_config;
        eth_setting_t eth_config;
        gprs_setting_t gprs_config;
    } config;

    uint8_t dhcp_en;
    ip_addr_t ipaddr;
    ip_addr_t netmask;
    ip_addr_t gw;

    int (*provision)(struct netmgr_dev_s *node);
    int (*unprovision)(struct netmgr_dev_s *node);
    int (*info)(struct netmgr_dev_s *node);
    int (*reset)(struct netmgr_dev_s *node);

    aos_dev_t *dev;

    /* status */
    int is_gotip;

    /* link status */
    int is_linkup;
} netmgr_dev_t;

struct netmgr_uservice;
typedef int (*netmgr_srv_func)(struct netmgr_uservice *netmgr, rpc_t *rpc);
typedef struct netmgr_srv_s {
    slist_t     next;
    int cmd_id;
    netmgr_srv_func func;
} netmgr_srv_t;

struct netmgr_uservice {
    uservice_t *srv;
    utask_t *task;
    int      inited;

    slist_t dev_list;           /* struct netmgr_dev_t*/
    slist_t srv_list;           /* struct netmgr_srv_t*/

    //rpc_t *current_cmd;
};

/**
 * @brief  handle event of the netmgr uservice
 * @param  [in] event_id
 * @param  [in] param
 * @param  [in] context
 * @return
 */
void netmgr_event_cb(uint32_t event_id, const void *param, void *context);

/**
 * @brief  start dhcp by the network name
 * @param  [in] netmgr
 * @param  [in] name : network name
 * @return 0/-1
 */
int netmgr_start_dhcp(struct netmgr_uservice *netmgr, const char *name);

/**
 * @brief  find netmgr dev from netdev list by the network name
 * @param  [in] list : netmgr dev list
 * @param  [in] name
 * @return NULL on error
 */
netmgr_dev_t *netmgr_find_dev(slist_t *list, const char *name);

/**
 * @brief  regist service function
 * @param  [in] cmd_id : command id of the netmgr uservice
 * @param  [in] func : callback
 * @return 0/-1
 */
int netmgr_reg_srv_func(int cmd_id, netmgr_srv_func func);

/**
 * @brief  unregist service function
 * @param  [in] cmd_id : command id of the netmgr uservice
 * @param  [in] func : callback
 * @return 0/-1
 */
int netmgr_unreg_srv_func(int cmd_id, netmgr_srv_func func);

/**
 * @brief  subscribe event of the netmgr uservice
 * @param  [in] cmd_id : EVENT_WIFI_LINK_UP, etc
 * @return
 */
void netmgr_subscribe(int cmd_id);

/**
 * @brief  unsubscribe event of the netmgr uservice
 * @param  [in] cmd_id : EVENT_WIFI_LINK_UP, etc
 * @return
 */
void netmgr_unsubscribe(int cmd_id);

/**
 * @brief  set gotip flag by network name
 * @param  [in] name
 * @param  [in] gotip
 * @return -1 on error
 */
int netmgr_set_gotip(const char *name, int gotip);

/**
 * @brief  set link layer up flag by network name
 * @param  [in] name
 * @param  [in] linkup
 * @return -1 on error
 */
int netmgr_set_linkup(const char *name, int linkup);

/**
 * @brief  create & init the netmgr uservice
 * @param  [in] task
 * @return
 */
void netmgr_service_init(utask_t *task);

/**
 * @brief  destroy & uninit the netmgr uservice
 * @return
 */
void netmgr_service_deinit();

#ifdef CONFIG_KV_SMART
char *netmgr_kv_get(const char *key);
int netmgr_kv_getint(const char *key);
#endif

extern struct netmgr_uservice netmgr_svc;

#ifdef __cplusplus
}
#endif

#endif
