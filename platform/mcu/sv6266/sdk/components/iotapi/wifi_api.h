#ifndef _WIFI_API_H_
#define _WIFI_API_H_

#include "wificonf.h"

#define WIFI_INIT() \
    PBUF_Init();    \
    NETSTACK_RADIO.init();  \
    drv_sec_init(); 
	
typedef struct CONNSTAINFO
{
    u8 mac[6];
    u8 ipaddr[4];
}CONNSTAINFO;

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
 * @brief Start scan task, only scan channel in 2.4G band. THe callback function will be excuted when scan task is ended.
 * @param ssid         [IN] Assign a specific ssid, the maximum length of ssid is 32, the maximum length doesn't include the terminal character 
 * @param callbackfn   [IN] callback function.
 * @return none.
 */
int scan_AP_ex(char *ssid, void (*callbackfn)(void *));

/**
 * @brief Start scan task. THe callback function will be excuted when scan task is ended.
 * @param ssid            [IN] Assign a specific ssid, the max length of ssid is 32 
 * @param callbackfn      [IN] callback function.
 * @param channelindex    [IN] Set the specific 2.4G channels to scan.
 * @param channel5Gindex  [IN] Set the specific 5G channels to scan.
 * @param scantime_in_ms  [IN] Scan time for each channel.
 * @return none.
 */
int scan_AP_custom(char *ssid, void (*callbackfn)(void *), u16 channelindex, u32 channel5Gindex, u16 scantime_in_ms);

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

/*
 * @set the fix rate 
 * @wsid
    If device is in STA mode, the value is always 0
    If device is in TWO-STA mode, the value is always 0 or 1    
    If device is in AP/CONCURRENT mode, the value is 1 ~ 4, user can get the value by wifi_softap_get_sta_idx_by_mac
 * @drate_idx   
    drate_idx=0xFF, it means disable the fix rate
*/
int wifi_set_fix_drate(u8 wsid, EN_DATA_RATE_IDX drate_idx);

/*
 * @get the current rate
 * @wsid
    If device is in STA mode, the value is always 0
    If device is in TWO-STA mode, the value is always 0 or 1    
    If device is in AP/CONCURRENT mode, the value is 1 ~ 4, user can get the value by wifi_softap_get_sta_idx_by_mac
*/
u8 wifi_get_current_drate(u8 wsid);

/*
 * @In AP mode, user can get the STA index by MAC
 * @mac. input parameter
 * @return -1: fail.
*/
int wifi_softap_get_sta_idx_by_mac(u8 *mac);

/**
 * @brief register the callback function and it will be execute when STA connect or disconnect to softap.
 * @param callbackfn   [IN] callback function.
 * @return the result. 0 : Successful, -1 : Failed(Only can operation when softap is not working).
 */
int wifi_register_softap_cb(void (*callbackfn)(STAINFO*));

int get_connectap_info(u8 staid, u8 *pssid, u8 *pssidlen, u8 *pmac, u8 maclen, u8 *prssi, u8 *pch);

/**
 * @brief Get the mac and ip address of connected STAs.
 * @param info		   [OUT] The array storage mac and ip address of connected STAs.
 * @param number	   [INOUT] IN : The amount of array - info. 
                               OUT : the avaiable number of sta information.
 * @return the result. 0 : Successful, -1 : Failed.
 */
int get_connectsta_info(CONNSTAINFO *info, u8 *number);

/**
 * @brief Configures the settings of sniffer mode. 
 * @param index	         [IN] set which kind of frame would like to receive.
 * @param sniffercb		 [IN] When receive the expect frame, it will call the callback function and offer the detail frame information.
                              The first parameter is pointer of the frame. The second parameter is the length of the frame.
 * @return none.
 */
void set_sniffer_config(SNIFFER_RECVINDEX index, void (*sniffercb)(packetinfo *));
void clear_rxque_buffer();

/**
 * @brief Configures the WIFI power saving mode
 * @param enable	    [IN] 1: enable wifi power saving , 0: disable wifi power savng 
 * @param opmode        [IN] 1: MCU off, 0:MCU ON. Now, Only support MCU on 
 * @return 0:success, -1:fail
 */
int set_power_mode(u8 enable, u8 opmode);
int set_highprio_sta(u8 staid);
int register_wifi_ifname(u8 id, char *ifname);
int get_local_mac(u8 staid, u8 *pmac, u8 len);
u8 get_operation_mode();
/**
 * @brief Configures the channel settings of sniffer mode. 
 * @param ch	    [IN] cannel number. 
 *                  1,2,3,4,5,6,7,8,9,10,11,12,13 on 2.4G Band. 
 *                  36,40,44,48,52,56,60,64,100,104,108,112,116,120,124,128,132,136,140,144,149,153,157,161,and 165 on 5G Band
 * @param type      [IN] channel type
 *                  0: None HT
 *                  1: HT20
 *                  2: HT40 minus
 *                  3. HT40 plus
 * @return 0:success, -1:fail
 */
int wifi_set_channel(u32 ch, u32 type);
int set_country_code(COUNTRY_CODE code);


int wifi_debug_enable(bool en);
int wifi_register_mac_send_cb(void (*mac_send_debug_cb)(void *data));
int wifi_unregister_mac_send_cb(void (*mac_send_debug_cb)(void *data));
int wifi_register_radio_send_cb(void (*radio_send_debug_cb)(void *data));
int wifi_unregister_radio_send_cb(void (*radio_send_debug_cb)(void *data));


/**
 *@ configure the amout of ap list. This function should be called before DUT_START
*/
int wifi_set_ap_list_amount(u32 amount);

/**
 *@ send null data to AP, this function only works when a STA interface is working
 *@ pwmode=0: STA is active, pwmode=1: STA is in sleeping.
 *@ id: interface id, id=0, it means IF0, id=1, it means IF1
    id=0 or 1, if DUT works on DUT_TWOSTA
    id=0, if DUT works on DUT_STA, DUT_CONCURRENT    
*/
int wifi_send_null_data(int pwmode, u8 id);

/**
 *@ send icmp with fix 1M or use auto rate
 *@ en=0, auto rate (default)
 *@ en=1, fix 1M   
*/
int wifi_set_icmp_1m(bool en);

/**
 *@ enable/disable 2M tx rate 
 *@ en=0, disable (default)
 *@ en=1, enable   
*/
int wifi_set_rate2m_enable(bool en);

#if 1
/**
 *@ send low rate with rts/cts
 *@ en=0, without rts/cts (default)
 *@ en=1, with rts/cts
*/
int wifi_set_low_rate_rts_cts(bool en);
#endif

/**
    RADIO_BAND_2G:      ch<=14
    RADIO_BAND_5100:    ch<36
    RADIO_BAND_5500:    ch>=36 && ch<100
    RADIO_BAND_5700:    ch>=100 && ch<140
    RADIO_BAND_5900:    ch>=140 
*/
int wifi_padpd_on_off(RADIO_BAND band,bool benable);

/**
 *@ on/off dcdc. on is default value. 
 *@ This function should be called before NETSTACK_RADIO.init()
 */
int wifi_dcdc_on_off(bool on);

/**
 * bit[0]=1M
 * bit[1]=2M
 * bit[2]=5.5M
 * bit[3]=11M
 * bit[4]=MCSO or 6M
 * bit[5]=MCS1 or 9M
 * bit[6]=MCS2 or 12M
 * bit[7]=MCS3 or 18M
 * bit[8]=MCS4 or 24M
 * bit[9]=MCS5 or 34M
 * bit[10]=MCS6 or 48M
 * bit[11]=MCS7 or 54M
 * If HT, rc_mask=0x2B1, it means that DUT only tx packets by MCS5, MCS3, MCS1, MCS0 and 1M
 * If Legacy, rc_mask=0x2B1, it means that DUT only tx packets by 34M, 18M, 9M, 6M and 1M
 * This function should be called before connect to AP or STA connect to SoftAP
 */
int wifi_set_rc_mask(u16 rc_mask);
#endif
