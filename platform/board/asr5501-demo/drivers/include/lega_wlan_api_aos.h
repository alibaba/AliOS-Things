#ifndef _LEGA_WIFI_API_AOS_H_
#define _LEGA_WIFI_API_AOS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "lega_wlan_api.h"

typedef enum
{
    WLAN_EVENT_SCAN_COMPLETED,
    WLAN_EVENT_ASSOCIATED,
    WLAN_EVENT_CONNECTED,
    WLAN_EVENT_IP_GOT,
    WLAN_EVENT_DISCONNECTED,
    WLAN_EVENT_AP_UP,
    WLAN_EVENT_AP_DOWN,
    WLAN_EVENT_MAX,
}lega_wlan_event_e;

typedef enum {
    CONNECT_SUCC,           /* used in sta mode, indicate auth and assoc success */
    CONNECT_SCAN_FAIL,      /* used in sta mode, indicate scan ssid fail         */
    CONNECT_CONN_FAIL,      /* used in sta mode, indicate auth and assoc fail    */
} lega_start_adv_results_e;

/**
 *  @brief  Input network precise paras in lega_wlan_start_adv function.
 */
typedef struct
{
  lega_wlan_ap_info_adv_t ap_info;         /**< @ref apinfo_adv_t. */
  char  key[64];                /**< Security key or PMK of the wlan. */
  int   key_len;                /**< The length of the key. */
  char  local_ip_addr[16];      /**< Static IP configuration, Local IP address. */
  char  net_mask[16];           /**< Static IP configuration, Netmask. */
  char  gateway_ip_addr[16];    /**< Static IP configuration, Router IP address. */
  char  dns_server_ip_addr[16]; /**< Static IP configuration, DNS server IP address. */
  char  dhcp_mode;              /**< DHCP mode, @ref DHCP_Disable, @ref DHCP_Client and @ref DHCP_Server. */
  char  reserved[32];
  int   wifi_retry_interval;    /**< Retry interval if an error is occured when connecting an access point,
                                  time unit is millisecond. */
} lega_wlan_init_info_adv_st;

/** @brief  used in station and softap mode, get mac address(in char mode) of WIFI device
 *
 * @param mac_addr    : pointer to get the mac address
 *
 *  @return    0       : on success.
 *  @return    other   : error occurred
 */
int lega_wlan_get_mac_address_inchar(char *puc_mac);

/** @brief  used in station mode, scan cmd
 *  @return    0       : on success.
 *  @return    other   : error occurred
 */
int lega_wlan_start_scan_adv(void);

/*when use monitor-ap mode, user should register this type of callback function */
typedef void (*monitor_ap_cb_t)();

/** @brief  used in sniffer-ap mode, callback function for application
 *
 * @param fn    : refer to monitor_ap_cb_t
 *
 *  @return    0       : on success.
 *  @return    other   : error occurred
 */
int lega_wlan_register_monitor_ap_cb(monitor_ap_cb_t fn);

/* start adv callback function, notify the connect results*/
typedef void (*start_adv_cb_t)(lega_start_adv_results_e status);

/** @brief  used in sta mode, callback function to notify the connecting results
 *
 * @param fn    : refer to start_adv_cb_t
 *
 *  @return    0       : on success.
 *  @return    other   : error occurred
 */
int lega_wlan_register_start_adv_cb(start_adv_cb_t fn);

/** @brief  config to support smartconfig in MIMO scenario
 *
 */
void lega_wlan_smartconfig_mimo_enable(void);

/** @brief  start monitor and ap coexist mode
 *
 * @param init_info    : refer to lega_wlan_init_type_t
 *
 * @return    0       : on success.
 * @return    other   : error occurred
 */
int lega_wlan_start_monitor_ap(lega_wlan_init_type_t* init_info);

/** @brief  stop monitor and ap coexist mode
 *
 * @return    0       : on success.
 * @return    other   : error occurred
 */
int lega_wlan_stop_monitor_ap();


int lega_wlan_suspend_sta(void);
int lega_wlan_suspend_ap(void);
int lega_wlan_suspend(void);
void lega_wlan_register_mgmt_monitor_cb(monitor_cb_t fn);

/*Wifi event callback interface
*
* @return    void
*/
extern void wifi_event_cb(lega_wlan_event_e evt, void* info);
#ifdef __cplusplus
    }
#endif

#endif //_LEGA_WIFI_API_AOS_H_

