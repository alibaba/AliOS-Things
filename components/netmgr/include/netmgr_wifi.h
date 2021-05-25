/**
 * @file netmgr_wifi.h
 * netmgr Wi-Fi API header file.
 *
 * @version   V2.0
 * @date      2020-11-22
 * @copyright Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef NETMGR_WIFI_H
#define NETMGR_WIFI_H

#include <stdint.h>
#include <stdbool.h>
#include <netmgr.h>

#if defined(__cplusplus)
extern "C"
{
#endif

typedef enum {
    RET_WIFI_OK                  = 0,
    RET_WIFI_COMMON_FAIL         = -1,
    RET_WIFI_INVALID_ARG         = -2,  //invalid argument
    RET_WIFI_INVALID_PASSWORD    = -3,  //invalid password
    RET_WIFI_MEMORY_ERROR        = -4,  //no memory to allocate resource
    RET_WIFI_INIT_FAIL           = -5,  //init wifi fail
    RET_WIFI_NOT_INITED          = -6,  //wifi is not initialized
    RET_WIFI_STATUS_ERROR        = -7,  //request in error STATUS
    RET_WIFI_SCAN_REQ_FAIL       = -8,  //scan fail to start
    RET_WIFI_SCAN_NO_AP_FOUND    = -9,  //scan result is NULL (didn't find any SSID)
    RET_WIFI_NO_SUITABLE_NETWORK = -10,  //no suitable network to connect
    RET_WIFI_CONN_REQ_FAIL       = -11, //connect fail to start
    RET_WIFI_CONN_FAIL           = -12, //connect procedure result in fail
    RET_WIFI_CONN_NO_SSID_CONFIG = -13, //no saved SSID config to connect
    RET_WIFI_DISC_FAIL           = -14, //disconnect procedure result in fail
    RET_WIFI_WPS_NOT_FOUND       = -15, //couldn't find WPS AP
    RET_WIFI_WPS_REQ_FAIL        = -16, //WPS fail to start
}wifi_result_t;

typedef struct {
    uint8_t dhcp;     /* DHCP mode: @ref DHCP_Disable, @ref DHCP_Client, @ref DHCP_Server. */
    char    ip[16];   /* Local IP address on the target wlan interface: @ref wlanInterfaceTypedef. */
    char    gate[16]; /* Router IP address on the target wlan interface: @ref wlanInterfaceTypedef. */
    char    mask[16]; /* Netmask on the target wlan interface: @ref wlanInterfaceTypedef. */
    char    dns[16];  /* DNS server IP address. */
    char    mac[16];  /* MAC address, example: "C89346112233". */
    char    broadcastip[16];
} netmgr_wifi_ip_stat_t;

typedef enum netmgr_wifi_ip_mode_e {
    NETMGR_WIFI_IP_MODE_AUTO,             /* Use dhcp to get ip address */
    NETMGR_WIFI_IP_MODE_STATIC,           /* Use static ip address */
} netmgr_wifi_ip_mode_t;

typedef enum netmgr_wifi_ssid_format_e {
    NETMGR_WIFI_SSID_FORMAT_UTF8,
    NETMGR_WIFI_SSID_FORMAT_GBK,
} netmgra_wifi_ssid_format_t;

typedef enum netmgr_wifi_scan_type_e {
    NETMGR_WIFI_SCAN_TYPE_FULL,
    NETMGR_WIFI_SCAN_TYPE_SPECIFIED,
    NETMGR_WIFI_SCAN_TYPE_FULL_MERGE
} netmgr_wifi_scan_type_t;


#define MAX_SSID_SIZE  32
#define MAX_PWD_SIZE   64

#ifndef MAX_SSID_LEN
#define MAX_SSID_LEN (MAX_SSID_SIZE+1)
#endif
#ifndef ETH_ALEN
#define ETH_ALEN                        (6)
#endif

/**
 * @brief net manager init
 * @return 0 on success, others on error
 */
int netmgr_wifi_init(netmgr_hdl_t hdl);

/**
 * @brief net manager deinit
 */
void netmgr_wifi_deinit(netmgr_hdl_t hdl);

/**
 * @brief get wifi netmgr handler
 * @param  [in] hdl
 * @return NULL on error
 */
netmgr_hdl_t netmgr_wifi_get_dev(const char* name);

/**
 * @brief  set if config information
 * @param  [in] hdl
 * @param  [in] if config information
 * @return 0 on success, others on error
 */
int netmgr_wifi_set_ifconfig(netmgr_hdl_t hdl, netmgr_ifconfig_info_t* info);

/**
 * @brief  get if config information
 * @param  [in] hdl
 * @param  [out] if config information
 * @return 0 on success, others on error
 */
int netmgr_wifi_get_ifconfig(netmgr_hdl_t hdl, netmgr_ifconfig_info_t* info);

/**
 * @brief  set auto reconnect flag
 * @param  [in] hdl
 * @param  [in] enable
 */
void netmgr_wifi_set_auto_reconnect(netmgr_hdl_t hdl, bool enable);

/**
 * @brief  get config information
 * @param  [in] hdl
 * @param  [out] config information
 * @return 0 on success, others on error
 */
int netmgr_wifi_get_config(netmgr_hdl_t hdl, netmgr_wifi_config_t* config);

/**
 * @brief  delete config information
 * @param  [in] hdl
 * @param  [in] ssid which config is to delete
 * @return 0 on success, others on error
 */
int netmgr_wifi_del_config(netmgr_hdl_t, const char* ssid);

/**
 * @brief  get state of hdl
 * @param  [in] hdl
 * @return >=0 on success, others on error
 */
int netmgr_wifi_get_state(netmgr_hdl_t hdl);

/**
 * @brief  netmgr connect network
 * @param  [in] hdl
 * @param  [in] params
 * @return 0 on success, others on error
 */
int netmgr_wifi_connect(netmgr_hdl_t hdl, netmgr_wifi_connect_params_t* params);

/**
 * @brief  netmgr disconnect network
 * @param  [in] hdl
 * @return 0 on success, others on error
 */
int netmgr_wifi_disconnect(netmgr_hdl_t hdl);

/**
 * @brief  netmgr save config
 * @param  [in] hdl
 * @return 0 on success, others on error
 */
int netmgr_wifi_save_config(netmgr_hdl_t hdl);

/**
 * @brief  netmgr set connect params
 * @param  [in] hdl
 * @param  [in] ap_info
 * @param  [in] num
 * @param  [in] type
 * @return 0 on success, others on error
 */
int netmgr_wifi_set_connect_params(netmgr_hdl_t hdl, netmgr_connect_params_t* params);

int netmgr_wifi_set_msg_cb(netmgr_hdl_t hdl, netmgr_msg_cb_t cb);

int netmgr_wifi_del_msg_cb(netmgr_hdl_t hdl, netmgr_msg_cb_t cb);

/**
 * @brief  netmgr set auto save ap config
 * @param  [in] auto_save_ap
 * @return 0 on success, others on error
 */
int netmgr_wifi_set_auto_save_ap(bool auto_save_ap);

/** @} */

#if defined(__cplusplus)
}
#endif

#endif /* NETMGR_WIFI_H */
