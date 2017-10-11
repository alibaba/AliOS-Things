#ifndef _WLAN_UI_PUB_
#define _WLAN_UI_PUB_

#pragma once

#include "include.h"
#include "mico_rtos.h"

#include <hal/base.h>
#include <hal/wifi.h>

enum {
    ENC_OPEN,
    ENC_WEP,
    ENC_CCMP,
    ENC_TKIP,
};

enum {
    WLAN_RX_BEACON,    /* receive beacon packet */
    WLAN_RX_PROBE_REQ, /* receive probe request packet */
    WLAN_RX_PROBE_RES, /* receive probe response packet */
    WLAN_RX_ACTION,    /* receive action packet */
    WLAN_RX_MANAGEMENT,/* receive ALL management packet */
    WLAN_RX_DATA,      /* receive ALL data packet */
    WLAN_RX_MCAST_DATA,/* receive ALL multicast and broadcast packet */

    WLAN_RX_ALL,       /* receive ALL 802.11 packet */
};

/** @brief  Connect or establish a Wi-Fi network in normal mode (station or soft ap mode).
 * 
 *  @detail This function can establish a Wi-Fi connection as a station or create
 *          a soft AP that other staions can connect (4 stations Max). In station mode,  
 *          MICO first scan all of the supported Wi-Fi channels to find a wlan that 
 *          matchs the input SSID, and read the security mode. Then try to connect    
 *          to the target wlan. If any error occurs in the connection procedure or  
 *          disconnected after a successful connection, MICO start the reconnection 
 *          procedure in backgound after a time interval defined in inNetworkInitPara.
 *          Call this function twice when setup coexistence mode (staion + soft ap). 
 *          This function retruns immediately in station mode, and the connection will 
 *          be executed in background.
 *
 *  @param  inNetworkInitPara: Specifies wlan parameters. 
 *
 *  @return In station mode, allways retrurn kWlanNoErr.
 *          In soft ap mode, return kWlanXXXErr
 */
OSStatus bk_wlan_start(hal_wifi_init_type_t* inNetworkInitPara);

/** @brief  Connect to a Wi-Fi network with advantage settings (station mode only)
 * 
 *  @detail This function can connect to an access point with precise settings,
 *          that greatly speed up the connection if the input settings are correct
 *          and fixed. If this fast connection is failed for some reason, MICO 
 *          change back to normal: scan + connect mode refer to @ref bkWlanStart.
 *          This function returns after the fast connection try.
 *
 *  @note   This function cannot establish a soft ap, use StartNetwork() for this
 *          purpose. 
 *          If input SSID length is 0, MICO use BSSID to connect the target wlan.
 *          If both SSID and BSSID are all wrong, the connection will be failed.
 *
 *  @param  inNetworkInitParaAdv: Specifies the precise wlan parameters.
 *
 *  @retrun Allways return kWlanNoErr although error occurs in first fast try 
 *          kWlanTimeoutErr: DHCP client timeout
 */
OSStatus bk_wlan_start_adv(hal_wifi_init_type_adv_t* inNetworkInitParaAdv);

/** @brief  Read current IP status on a network interface.
 * 
 *  @param  outNetpara: Point to the buffer to store the IP address. 
 *  @param  inInterface: Specifies wlan interface. 
 *             @arg Soft_AP: The soft AP that established by bkWlanStart()
 *             @arg Station: The interface that connected to an access point
 *
 *  @return   kNoErr        : on success.
 *  @return   kGeneralErr   : if an error occurred
 */
OSStatus bk_wlan_get_ip_status(hal_wifi_ip_stat_t *outNetpara, hal_wifi_type_t inInterface);

/** @brief  Read current wireless link status on station interface.
 * 
 *  @param  outStatus: Point to the buffer to store the link status. 
 *
 *  @return   kNoErr        : on success.
 *  @return   kGeneralErr   : if an error occurred
 */
OSStatus bk_wlan_get_link_status(hal_wifi_link_stat_t *outStatus);

/** @brief  Start a wlan scanning in 2.4GHz in MICO backfround.
 *  
 *  @detail Once the scan is completed, MICO sends a notify: 
 *          bk_notify_WIFI_SCAN_COMPLETED, with callback function:
 *          void (*function)(ScanResult *pApList, bk_Context_t * const inContext)
 *          Register callback function using @ref bk_add_notification() before scan.
 */
void bk_wlan_start_scan(void);

/** @brief  Add the packet type which monitor should receive
 * 
 *  @detail This function can be called many times to receive different wifi packets.
 */
int bk_wlan_monitor_rx_type(int type);

/** @brief  Start wifi monitor mode
 * 
 *  @detail This function disconnect wifi station and softAP. 
 *       
 */
int bk_wlan_start_monitor(void);

/** @brief  Stop wifi monitor mode
 * 
 */
int bk_wlan_stop_monitor(void);

/** @brief  Set the monitor channel
 * 
 *  @detail This function change the monitor channel (from 1~13).
 *       it can change the channel dynamically, don't need restart monitor mode.
 */
int bk_wlan_set_channel(int channel);

/** @brief  Register the monitor callback function
 *        Once received a 802.11 packet call the registered function to return the packet.
 */
void bk_wlan_register_monitor_cb(monitor_data_cb_t fn);
monitor_data_cb_t bk_wlan_get_monitor_cb(void);
int bk_wlan_is_monitor_mode(void);
uint32_t bk_wlan_is_ap(void);
uint32_t bk_wlan_is_sta(void);
uint32_t bk_sta_cipher_is_open(void);
uint32_t bk_sta_cipher_is_wep(void);

#ifdef CONFIG_AOS_MESH
void wlan_register_mesh_monitor_cb(monitor_data_cb_t fn);
monitor_data_cb_t wlan_get_mesh_monitor_cb(void);
int wlan_is_mesh_monitor_mode(void);
int wlan_set_mesh_bssid(uint8_t *bssid);
uint8_t *wlan_get_mesh_bssid(void);
#endif

#endif// _WLAN_UI_PUB_
