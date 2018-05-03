#ifndef _WIFI_API_H_
#define _WIFI_API_H_

#include "wificonf.h"
/**
 * @brief Get DUT current operation mode.
 * @return operation mode.
 */
WIFI_OPMODE get_DUT_wifi_mode();

/**
 * @brief Stop DUT activity and all connections will terminate.
 * @return none.
 */
void DUT_wifi_stop();

/**
 * @brief Active DUT functionality. DUT can run DUT_TWOSTA and DUT_CONCURRENT only when DUT has two different mac address.
 * @param mode   [IN] operation mode.
 * @return none.
 */
int DUT_wifi_start(WIFI_OPMODE mode);

#if (ENABLE_HK==1)
int homekit_softap_start(u8 *inIE, size_t inIELen);
int homekit_softap_stop();
#endif

/**
 * @brief Start scan task, only scan channel in 2.4G band. THe callback function will be excuted when scan task is ended.
 * @param callbackfn   [IN] callback function.
 * @return none.
 */
int scan_AP(void (*callbackfn)(void *));

/**
 * @brief Start scan task. THe callback function will be excuted when scan task is ended.
 * @param callbackfn      [IN] callback function.
 * @param channelindex    [IN] Set the specific 2.4G channels to scan.
 * @param channel5Gindex  [IN] Set the specific 5G channels to scan.
 * @param scantime_in_ms  [IN] Scan time for each channel.
 * @return none.
 */
int scan_AP_custom(void (*callbackfn)(void *), u16 channelindex, u32 channel5Gindex, u16 scantime_in_ms);

/**
 * @brief Start connect to AP. This API is used for connect to the AP with hidden SSID.
 *        In TWOSTA mode, it's working when there's no any connection. 
 *        Otherwise, it will return -1.
 * @param pssid		   [IN] AP's SSID.
 * @param ssidlen	   [IN] Length of SSID.
 * @param pkey         [IN] AP's Passphrase.
 * @param keylen       [IN] Length of passphrase.
 * @param sectype      [IN] The encryption method. 0 : NONO. 1 : WEP. 2 : TKIP. 3 : CCMP.
                                                   4 : TKIP or CCMP. 5 : Allow all encryption method.
 * @param channel      [IN] Set the specific channel to do connection, channel set 0 means all channels are possible.
 * @param mac          [IN] AP's mac address. Input NULL if do not want to connect to specific AP.
 * @param callbackfn   [IN] callback function.
 * @return the result. 0 : Successful, -1 : Failed.
 */
int wifi_connect_active_3 (u8 *pssid, u8 ssidlen, u8 *pkey, u8 keylen, u8 sectype, u8 channel, u8 *mac, void (*callbackfn)(WIFI_RSP*));

/**
 * @brief Start connect to AP. This API is used for connect to the AP with hidden SSID.
 *        In TWOSTA mode, it's working when there's no any connection. 
 *        Otherwise, it will return -1.
 * @param pssid		   [IN] AP's SSID.
 * @param ssidlen	   [IN] Length of SSID.
 * @param pkey         [IN] AP's Passphrase.
 * @param keylen       [IN] Length of passphrase.
 * @param callbackfn   [IN] callback function.
 * @param sectype      [IN] The encryption method. 0 : NONO. 1 : WEP. 2 : TKIP. 3 : CCMP.
                                                   4 : TKIP or CCMP. 5 : Allow all encryption method.
 * @return the result. 0 : Successful, -1 : Failed.
 */
int wifi_connect_active_2 (u8 *pssid, u8 ssidlen, u8 *pkey, u8 keylen, void (*callbackfn)(WIFI_RSP*), u8 sectype);

/**
 * @brief Start connect to AP and allow all encryption method. This API is used for connect to the AP with hidden SSID.
 *        In TWOSTA mode, it's working when there's no any connection. 
 * @param pssid		   [IN] AP's SSID.
 * @param ssidlen	   [IN] Length of SSID.
 * @param pkey         [IN] AP's Passphrase.
 * @param keylen       [IN] Length of passphrase.
 * @param callbackfn   [IN] callback function.
 * @return the result. 0 : Successful, -1 : Failed.
 */
int wifi_connect_active (u8 *pssid, u8 ssidlen, u8 *pkey, u8 keylen, void (*callbackfn)(WIFI_RSP*));

/**
 * @brief Start connect to AP. THe callback function will be excuted when wifi status is changed.
 *        if the staid is illegal, it will return -1.
 *        If it's doesn't allos run this api in DUT operation mode. it will return -2.
 *        Channel of the second connection must same as the channel of exsited connection.
 *        Otherwise, it will return -3.
 *        Mac address of the second connection must different from the mac address of exsited connection.
 *        Otherwise, it will return -4.
 * @param staid        [IN] The sta index will like to start connection(0 or 1).
 * @param callbackfn   [IN] callback function.
 * @return the result. 0 : Successful, others : Failed.
 */
int wifi_connect_2(u8 staid, void (*callbackfn)(WIFI_RSP*));

/**
 * @brief Start connect to AP. THe callback function will be excuted when wifi status is changed.
 *        In TWOSTA mode, it's only can start connection process of STA0. 
 *        If it's doesn't allos run this api in DUT operation mode. it will return -2.
 * @param callbackfn   [IN] callback function.
 * @return the result. 0 : Successful, -2 : Failed.
 */
int wifi_connect (void (*callbackfn)(WIFI_RSP*));

/**
 * @brief Disconnect with the current AP. 
 * @return none.
 */
void wifi_disconnect_2(u8 staid, void (*callbackfn)(WIFI_RSP*));

/**
 * @brief Disconnect with the current AP. 
 *        In TWOSTA mode, it's only can terminate connection process of STA0. 
 * @return none.
 */
void wifi_disconnect (void (*callbackfn)(WIFI_RSP*));

/**
 * @brief Configures the wifi settings. 
 * @param pssid		   [IN] AP's SSID.
 * @param ssidlen	   [IN] Length of SSID.
 * @param pkey         [IN] AP's Passphrase. It can be NULL if AP dosen't need passphrase.
 * @param keylen       [IN] Length of passphrase.
 * @param pmac     	   [IN] AP's mac address. It only need to be filled when there are several AP with the same SSID.
                                          STA will connect to specific AP when input mac address. If input NULL, STA will  
                                          connect to the AP with the strongest signal.
 * @param maclen       [IN] Length of mac address.
 * @param sectype      [IN] The encryption method. 0 : NONO. 1 : WEP. 2 : TKIP. 3 : CCMP.
                                                   4 : TKIP or CCMP. 5 : Allow all encryption method.
 * @return the result. 0 : Successful, -1 : Failed.
 */
int set_wifi_config_3(u8 staid, u8 *pssid, u8 ssidlen, u8 *pkey, u8 keylen, u8 *pmac, u8 maclen, u8 sectype);
int set_wifi_config_2(u8 *pssid, u8 ssidlen, u8 *pkey, u8 keylen, u8 *pmac, u8 maclen, u8 sectype);

/**
 * @brief Configures the wifi settings and allow all encryption method. 
 * @param pssid		   [IN] AP's SSID.
 * @param ssidlen	   [IN] Length of SSID.
 * @param pkey         [IN] AP's Passphrase. It can be NULL if AP dosen't need passphrase.
 * @param keylen       [IN] Length of passphrase.
 * @param pmac     	   [IN] AP's mac address. It only need to be filled when there are several AP with the same SSID.
                                          STA will connect to specific AP when input mac address. If input NULL, STA will  
                                          connect to the AP with the strongest signal.
 * @param maclen       [IN] Length of mac address.
 * @return the result. 0 : Successful, -1 : Failed.
 */
int set_wifi_config(u8 *pssid, u8 ssidlen, u8 *pkey, u8 keylen, u8 *pmac, u8 maclen);

/**
 * @brief Get the configuration of  the wifi settings. 
 * @param pssid		   [OUT] AP's SSID.
 * @param ssidlen	   [INOUT] IN : The length of pssid. OUT : Length of SSID.
 * @param pkey         [OUT] AP's Passphrase.
 * @param pkeylen      [INOUT] IN : The length of pkey. OUT : Length of passphrase.
 * @param pmac     	   [OUT] AP's mac address.
 * @param maclen       [INOUT] IN : The length of pMac. OUT : Length of mac address.
 * @return the result. 0 : Successful, -1 : Failed.
 */
int get_wifi_config_2(u8 staid, u8 *pssid, u8 *pssidlen, u8 *pkey, u8 *pkeylen, u8 *pMac, u8 maclen);
int get_wifi_config(u8 *pssid, u8 *pssidlen, u8 *pkey, u8 *pkeylen, u8 *pMac, u8 maclen);

/**
 * @brief Setting tx data rate. 
 * @param rate     	   [IN] The index of rate table. If the value is out of rate table, rate control task will restart.
 * @return none.
 */
//void set_data_rate(u8 rate);

/**
 * @brief Get the amount of AP list. 
 * @return The amount of AP list.
 */
u8 get_ap_lsit_total_num(void);

/**
 * @brief Get the AP information. 
 * @param plist		[OUT] A pointer to a buffer that receives AP information.
 * @param avaliable	[INOUT] IN : The avalible element of plist.
                            OUT : The number of element received.
 * @return the result. 0 : Successful, -1 : Failed.
 */
int get_ap_list(TAG_AP_INFO *plist, u8 *avaliable);

/**
 * @brief Get the configuration of network interfaces. 
 * @param pdhcpen	     [OUT] DHCP functionally. 0 : disable DHCP, 1 : enable DHCP.
 * @param pipaddr		 [OUT] The IP address of this host.
 * @param psubmask       [OUT] The IP network mask for this interface.
 * @param pgateway	     [OUT] The default router's IP address.
 * @param pdnsserver	 [OUT] The DNS server IP address.
 * @return the result.   0 : Successful, -1 : Failed.
 */
int get_if_config_2(char *netifname, u8 *pdhcpen, u32 *pipaddr, u32 *psubmask, u32 *pgateway, u32 *pdnsserver, u8 *pmac, u8 len);
int get_if_config(u8 *pdhcpen, u32 *pipaddr, u32 *psubmask, u32 *pgateway, u32 *pdnsserver);

int set_if_config_2(u8 staid, u8 dhcpen, u32 ipaddr, u32 submask, u32 gateway, u32 dnsserver);
int set_if_config(u8 dhcpen, u32 ipaddr, u32 submask, u32 gateway, u32 dnsserver);

/**
 * @brief Get the wifi status. 
 * @return 0 : Disconnected. 1 : Connected. -1 : Failed
 */
int get_wifi_status_2(u8 staid);

/**
 * @brief Get the wifi status. 
 * @return 0 : Disconnected. 1 : Connected.
 */
u8 get_wifi_status(void);

int get_connectap_info(u8 staid, u8 *pssid, u8 *pssidlen, u8 *pmac, u8 maclen, u8 *prssi, u8 *pch);
/**
 * @brief Configures the settings of sniffer mode. 
 * @param index	         [IN] set which kind of frame would like to receive.
 * @param sniffercb		 [IN] When receive the expect frame, it will call the callback function and offer the detail frame information.
                              The first parameter is pointer of the frame. The second parameter is the length of the frame.
 * @return none.
 */
void set_sniffer_config(SNIFFER_RECVINDEX index, void (*sniffercb)(packetinfo *));
void clear_rxque_buffer();
int set_power_mode(u8 enable, u8 opmode);
int set_highprio_sta(u8 staid);
int register_wifi_ifname(u8 id, char *ifname);
int get_local_mac(u8 staid, u8 *pmac, u8 len);
u8 get_operation_mode();
int wifi_set_channel(u32 ch, u32 type);
int set_country_code(COUNTRY_CODE code);

#endif
