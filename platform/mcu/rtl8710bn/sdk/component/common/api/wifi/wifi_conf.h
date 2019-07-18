//----------------------------------------------------------------------------//
/**
  ******************************************************************************
  * @file    wifi_conf.h
  * @author
  * @version
  * @brief   This file provides user interface for Wi-Fi station and AP mode configuration 
  *             base on the functionalities provided by Realtek Wi-Fi driver.
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
  ****************************************************************************** 
  */
#ifndef __WIFI_API_H
#define __WIFI_API_H

/** @addtogroup nic NIC
 *  @ingroup    wlan
 *  @brief      NIC functions
 *  @{
 */

//#include "FreeRTOS.h"
#include "aos_osdep.h"
#include "wifi_constants.h"   
#include "wifi_structures.h"  
#include "wifi_util.h"    
#include "wifi_ind.h"    
//#include <platform/platform_stdlib.h>

#ifdef __cplusplus
  extern "C" {
#endif

/******************************************************
 *                    Macros
 ******************************************************/

#define RTW_ENABLE_API_INFO

#ifdef RTW_ENABLE_API_INFO
    #define RTW_API_INFO(args) do {printf args;} while(0)
#else
    #define RTW_API_INFO(args)
#endif

#define MAC_ARG(x) ((u8*)(x))[0],((u8*)(x))[1],((u8*)(x))[2],((u8*)(x))[3],((u8*)(x))[4],((u8*)(x))[5]
#define CMP_MAC( a, b )  (((a[0])==(b[0]))&& \
                          ((a[1])==(b[1]))&& \
                          ((a[2])==(b[2]))&& \
                          ((a[3])==(b[3]))&& \
                          ((a[4])==(b[4]))&& \
                          ((a[5])==(b[5])))

/******************************************************
 *                    Constants
 ******************************************************/
#define SCAN_LONGEST_WAIT_TIME  (4500)


#define MAC_FMT "%02x:%02x:%02x:%02x:%02x:%02x"

#define PSCAN_ENABLE 0x01      //enable for partial channel scan
#define PSCAN_FAST_SURVEY 0x02 //set to select scan time to FAST_SURVEY_TO, otherwise SURVEY_TO
#define PSCAN_SIMPLE_CONFIG   0x04 //set to select scan time to FAST_SURVEY_TO and resend probe request

/******************************************************
 *                 Type Definitions
 ******************************************************/

/** Scan result callback function pointer type
 *
 * @param result_ptr  : A pointer to the pointer that indicates where to put the next scan result
 * @param user_data   : User provided data
 */
typedef void (*rtw_scan_result_callback_t)( rtw_scan_result_t** result_ptr, void* user_data );
typedef rtw_result_t (*rtw_scan_result_handler_t)( rtw_scan_handler_result_t* malloced_scan_result );

/******************************************************
 *                    Structures
 ******************************************************/
typedef struct {
	char *buf;
	int buf_len;
} scan_buf_arg;

/******************************************************
 *                    Structures
 ******************************************************/
typedef struct internal_scan_handler{
	rtw_scan_result_t** pap_details;
	rtw_scan_result_t * ap_details;
	int	scan_cnt;
	rtw_bool_t	scan_complete;
	unsigned char	max_ap_size;
	rtw_scan_result_handler_t gscan_result_handler;
#if SCAN_USE_SEMAPHORE
	void *scan_semaphore;
#else
	int 	scan_running;
#endif
	void*	user_data;
	unsigned int	scan_start_time;
} internal_scan_handler_t;

typedef struct {
    rtw_network_info_t	network_info;
    _sema join_sema;
} internal_join_result_t;

/******************************************************
 *               Function Declarations
 ******************************************************/
/**
 * @brief  Initialize Realtek WiFi API System.
 * 			- Initialize the required parts of the software platform.
 *   			i.e. worker, event registering, semaphore, etc.
 * 			- Initialize the RTW API thread which handles the asynchronous event.
 * @return RTW_SUCCESS if initialization is successful, RTW_ERROR otherwise
 */
int wifi_manager_init(void);

/**
 * @brief  Join a Wi-Fi network.
 * 		Scan for, associate and authenticate with a Wi-Fi network.
 *		On successful return, the system is ready to send data packets.
 *
 * @param[in]  ssid: A null terminated string containing the SSID name of the network to join.
 * @param[in]  security_type: Authentication type:
 *                         - RTW_SECURITY_OPEN           - Open Security
 *                         - RTW_SECURITY_WEP_PSK        - WEP Security with open authentication
 *                         - RTW_SECURITY_WEP_SHARED     - WEP Security with shared authentication
 *                         - RTW_SECURITY_WPA_TKIP_PSK   - WPA Security
 *                         - RTW_SECURITY_WPA2_AES_PSK   - WPA2 Security using AES cipher
 *                         - RTW_SECURITY_WPA2_TKIP_PSK  - WPA2 Security using TKIP cipher
 *                         - RTW_SECURITY_WPA2_MIXED_PSK - WPA2 Security using AES and/or TKIP ciphers
 * @param[in]  password: A byte array containing either the cleartext security key for WPA/WPA2
 *  						 secured networks, or a pointer to an array of rtw_wep_key_t
 *  						 structures for WEP secured networks.
 * @param[in]  ssid_len: The length of the SSID in bytes.
 * @param[in]  password_len: The length of the security_key in bytes.
 * @param[in]  key_id: The index of the wep key (0, 1, 2, or 3). If not using it, leave it with value -1.
 * @param[in]  semaphore: A user provided semaphore that is flagged when the join is complete. If not using it, leave it with NULL value.
 * @return  RTW_SUCCESS: when the system is joined and ready to send data packets.
 * @return  RTW_ERROR: if an error occurred.
 * @note  Please make sure the Wi-Fi is enabled before invoking this function. (@ref wifi_on())
 */
int wifi_connect(
	char 				*ssid,
	rtw_security_t	security_type,
	char 				*password,
	int 				ssid_len,
	int 				password_len,
	int 				key_id,
	_sema 				*semaphore);

/**
 * @brief  Join a Wi-Fi network with specified BSSID.
 * 		Scan for, associate and authenticate with a Wi-Fi network.
 *		On successful return, the system is ready to send data packets.
 * @param[in]  bssid: The specified BSSID to connect.
 * @param[in]  ssid: A null terminated string containing the SSID name of the network to join.
 * @param[in]  security_type: Authentication type:
 *                         - RTW_SECURITY_OPEN           - Open Security
 *                         - RTW_SECURITY_WEP_PSK        - WEP Security with open authentication
 *                         - RTW_SECURITY_WEP_SHARED     - WEP Security with shared authentication
 *                         - RTW_SECURITY_WPA_TKIP_PSK   - WPA Security
 *                         - RTW_SECURITY_WPA2_AES_PSK   - WPA2 Security using AES cipher
 *                         - RTW_SECURITY_WPA2_TKIP_PSK  - WPA2 Security using TKIP cipher
 *                         - RTW_SECURITY_WPA2_MIXED_PSK - WPA2 Security using AES and/or TKIP ciphers
 * @param[in]  password: A byte array containing either the cleartext security key for WPA/WPA2
 *  						 secured networks, or a pointer to an array of rtw_wep_key_t
 *  						 structures for WEP secured networks.
 * @param[in]  ssid_len: The length of the SSID in bytes.
 * @param[in]  password_len: The length of the security_key in bytes.
 * @param[in]  key_id: The index of the wep key.
 * @param[in]  semaphore: A user provided semaphore that is flagged when the join is complete.
 * @return  RTW_SUCCESS: when the system is joined and ready to send data packets.
 * @return  RTW_ERROR: if an error occurred.
 * @note  Please make sure the Wi-Fi is enabled before invoking this function. (@ref wifi_on())
 * @note  The difference between @ref wifi_connect_bssid() and @ref wifi_connect() is that BSSID has higher priority as the basis of connection in @ref wifi_connect_bssid.
 */
int wifi_connect_bssid(
	unsigned char 		bssid[ETH_ALEN],
	char 				*ssid,
	rtw_security_t	security_type,
	char 				*password,
	int 				bssid_len,
	int 				ssid_len,
	int 				password_len,
	int 				key_id,
	_sema 				*semaphore);

/**
  * @brief  Disassociates from current Wi-Fi network.
  * @param  None
  * @return  RTW_SUCCESS: On successful disassociation from the AP.
  * @return  RTW_ERROR: If an error occurred.
  */
int wifi_disconnect(void);

/**
* @brief  Check if Wi-Fi has connected to AP before dhcp.
* @param  None
* @return  RTW_SUCCESS: If conneced.
* @return  RTW_ERROR: If not connect.
*/
int wifi_is_connected_to_ap(void);

/**
  * @brief  Check if the specified interface is up.
  * @param[in]  interface: The interface can be set as RTW_STA_INTERFACE or RTW_AP_INTERFACE. (@ref rtw_interface_t)
  * @return  If the function succeeds, the return value is 1. Otherwise, return 0.
  */
int wifi_is_up(rtw_interface_t interface);

/** Determines if a particular interface is ready to transceive ethernet packets
 *
 * @param     Radio interface to check, options are 
 *  				RTW_STA_INTERFACE, RTW_AP_INTERFACE
 * @return    RTW_SUCCESS  : if the interface is ready to 
 *  		  transceive ethernet packets
 * @return    RTW_NOTFOUND : no AP with a matching SSID was 
 *  		  found
 * @return    RTW_NOT_AUTHENTICATED: a matching AP was found but
 *  								   it won't let you
 *  								   authenticate. This can
 *  								   occur if this device is
 *  								   in the block list on the
 *  								   AP.
 * @return    RTW_NOT_KEYED: the device has authenticated and 
 *  						   associated but has not completed
 *  						   the key exchange. This can occur
 *  						   if the passphrase is incorrect.
 * @return    RTW_ERROR    : if the interface is not ready to 
 *  		  transceive ethernet packets
 */
int wifi_is_ready_to_transceive(rtw_interface_t interface);

/**
 * @brief  This function sets the current Media Access Control (MAC) address of the 802.11 device.
 * @param[in] mac: Wi-Fi MAC address.
 * @return    RTW_SUCCESS or RTW_ERROR
 */
int wifi_set_mac_address(char * mac);

/**
 * @brief  Retrieves the current Media Access Control (MAC) address
 *			(or Ethernet hardware address) of the 802.11 device.
 * @param[in]  mac: Point to the result of the mac address will be get.
 * @return    RTW_SUCCESS or RTW_ERROR
 */
int wifi_get_mac_address(char * mac);

/**
 * @brief Enable Wi-Fi powersave mode.
 * @param  None
 * @return RTW_SUCCESS or RTW_ERROR.
 */
int wifi_enable_powersave(void);

/**
 * @brief Disable Wi-Fi powersave mode.
 * @param  None
 * @return RTW_SUCCESS or RTW_ERROR.
 */
int wifi_disable_powersave(void);

/** Gets the tx power in index units
 *
 * @param dbm : The variable to receive the tx power in index.
 *
 * @return  RTW_SUCCESS : if successful
 *          RTW_ERROR   : if not successful
 */
int wifi_get_txpower(int *poweridx);

/**
 * @brief  Set the tx power in index units.
 * @param[in] poweridx: The desired tx power in index.
 * @return  RTW_SUCCESS: if tx power is successfully set
 * @return  RTW_ERROR: if tx power is not successfully set
 */
int wifi_set_txpower(int poweridx);

/**
 * @brief  Get the associated clients with SoftAP.
 * @param[out]  client_list_buffer: The location where the client list will be stored.
 * @param[in]  buffer_length: The buffer length.
 * @return  RTW_SUCCESS: The result is successfully got.
 * @return  RTW_ERROR: The result is not successfully got.
 */
int wifi_get_associated_client_list(void * client_list_buffer, unsigned short buffer_length);

/** 
 *@brief Get connected AP's BSSID
 * @param[out] bssid : the location where the AP BSSID will be stored
 * @return RTW_SUCCESS : if result was successfully get
 * @return RTW_ERROR : if result was not successfully get
 */
int wifi_get_ap_bssid(unsigned char *bssid);

/**
 * @brief  Get the SoftAP information.
 * @param[out]  ap_info: The location where the AP info will be stored.
 * @param[out]  security: The security type.
 * @return  RTW_SUCCESS: The result is successfully got.
 * @return  RTW_ERROR: The result is not successfully got.
 */
int wifi_get_ap_info(rtw_bss_info_t * ap_info, rtw_security_t* security);

/**
 * @brief  Set the country code to driver to determine the channel set.
 * @param[in]  country_code: Specify the country code.
 * @return  RTW_SUCCESS: If result is successfully set.
 * @return  RTW_ERROR: If result is not successfully set.
 */
int wifi_set_country(rtw_country_code_t country_code);

/**
 * @brief  retrieved sta mode MAX data rate.
 * @param[out]  inidata_rate: MAX data rate.
 * @return  RTW_SUCCESS: If the INIDATA_RATE is successfully retrieved.
 * @return  RTW_ERROR: If the INIDATA_RATE is not retrieved.
 * note: inidata_rate = 2 * (data rate), you need inidata_rate/2.0 to get the real rate
 */
int wifi_get_sta_max_data_rate(u8 * inidata_rate);

/**
 * @brief  Retrieve the latest RSSI value.
 * @param[out]  pRSSI: Points to the integer to store the RSSI value gotten from driver.
 * @return  RTW_SUCCESS: If the RSSI is succesfully retrieved.
 * @return  RTW_ERROR: If the RSSI is not retrieved.
 */
int wifi_get_rssi(int *pRSSI);

/**
 * @brief  Retrieve the latest SNR value.
 * @param[out]  pRSSI: Points to the integer to store the SNR value gotten from driver.
 * @return  RTW_SUCCESS: If the SNR is succesfully retrieved.
 * @return  RTW_ERROR: If the SNR is not retrieved.
 */
int wifi_get_snr(int *pSNR);

/**
 * @brief  Set the listening channel for promiscuous mode.
 * @param[in]  channel: The desired channel.
 * @return  RTW_SUCCESS: If the channel is successfully set.
 * @return  RTW_ERROR: If the channel is not successfully set.
 * @note  Do NOT need to call this function for STA mode wifi driver, since it will determine the channel from received beacon.
 */
int wifi_set_channel(int channel);

/**
 * @brief  Get the current channel on STA interface.
 * @param[out] channel: A pointer to the variable where the 
 *  				channel value will be written
 * @return  RTW_SUCCESS: If the channel is successfully read.
 * @return  RTW_ERROR: If the channel is not successfully read.
 */
int wifi_get_channel(int *channel);

/**
 * @brief  Register interest in a multicast address.\n
 * 		Once a multicast address has been registered, all packets detected on the
 * 		medium destined for that address are forwarded to the host.
 * 		Otherwise they are ignored.
 * @param[in] mac: Ethernet MAC address
 * @return  RTW_SUCCESS: If the address is registered successfully.
 * @return  RTW_ERROR: If the address is not registered.
 */
int wifi_register_multicast_address(rtw_mac_t *mac);

/** 
 * @brief  Unregister interest in a multicast address.\n
 * 		Once a multicast address has been unregistered, all packets detected on the
 * 		medium destined for that address are ignored.
 * @param[in] mac: Ethernet MAC address
 * @return  RTW_SUCCESS: If the address is unregistered successfully.
 * @return  RTW_ERROR: If the address is not unregistered.
 */
int wifi_unregister_multicast_address(rtw_mac_t *mac);

/**
 * @brief  Setup the adaptivity mode.
 * 		You can replace this weak function by the same name funcation to setup adaptivity mode you want.
 * @param  None
 * @return  If the function succeeds, the return value is 0.
 */
_WEAK void wifi_set_mib(void);

/**
 * @brief  Setup country code.
  * 		You can replace this weak function by the same name funcation to setup country code you want.
 * @param  None
 * @return  If the function succeeds, the return value is 0.
 */
//----------------------------------------------------------------------------//
_WEAK void wifi_set_country_code(void);

/**
 * @brief  Enable Wi-Fi RF.
 * @param  None
 * @return  If the function succeeds, the return value is 0.
 * @note  The difference between @ref wifi_rf_on() and @ref wifi_on() is that @ref wifi_rf_on() simply enable RF HAL, it does not enable the driver or allocate memory.
 */
int wifi_rf_on(void);

/**
 * @brief  Disable Wi-Fi RF.
 * @param  None
 * @return  If the function succeeds, the return value is 0.
 * @note  The difference between @ref wifi_rf_off() and @ref wifi_off() is that @ref wifi_rf_off() simply disable RF HAL, the driver and used heap memory will NOT be released.
 */
int wifi_rf_off(void);

/**
 * @brief  Enable Wi-Fi.
 * - Bring the Wireless interface "Up"
 * - Initialize the driver thread which arbitrates access
 *   to the SDIO/SPI bus
 *
 * @param[in]  mode: Decide to enable WiFi in which mode. The optional modes are enumerated in @ref rtw_mode_t.
 * @return  RTW_SUCCESS: if the WiFi chip was initialized successfully.
 * @return  RTW_ERROR: if the WiFi chip was not initialized successfully.
 */
int wifi_on(rtw_mode_t mode);

/**
 * @brief  Disable Wi-Fi.
 *  
 * @param  None
 * @return  RTW_SUCCESS: if deinitialization is successful.
 * @return  RTW_ERROR: otherwise.
 */
int wifi_off(void);

/**
 * Turn off the Wi-Fi device
 *
 * - Bring the Wireless interface "Down"
 * - De-Initialises the driver thread which arbitrates access
 *   to the SDIO/SPI bus
 *
 * @return RTW_SUCCESS if deinitialization is successful, 
 *  	   RTW_ERROR otherwise
 */
int wifi_off_fastly(void);

/**
 * @brief  Set IPS/LPS mode.
 * @param[in] ips_mode: The desired IPS mode. It becomes effective when wlan enter ips.\n
 *		@ref ips_mode is inactive power save mode. Wi-Fi automatically turns RF off if it is not associated to AP. Set 1 to enable inactive power save mode.
 * @param[in] lps_mode: The desired LPS mode. It becomes effective when wlan enter lps.\n
 *		@ref lps_mode is leisure power save mode. Wi-Fi automatically turns RF off during the association to AP is traffic is not busy while it also automatically turns RF on to listen to beacon. Set 1 to enable leisure power save mode.
 * @return  RTW_SUCCESS if setting LPS mode successful.
 * @return  RTW_ERROR otherwise.
 */
 
int wifi_set_power_mode(unsigned char ips_mode, unsigned char lps_mode);

/**
 * @brief  Set LPS mode.
 * @param[in] lps_mode: The desired LPS mode. It becomes effective when wlan enter lps.\n
 *		@ref lps_mode is leisure power save mode. Wi-Fi automatically turns RF off during the association to AP is traffic is not busy while it also automatically turns RF on to listen to beacon. Set 1 to enable leisure power save mode.
 * @return  RTW_SUCCESS if setting LPS mode successful.
 * @return  RTW_ERROR otherwise.
 */
int wifi_set_powersave_level(u8 level);

/**
 * Set TDMA parameters
 *
 * @param[in] slot_period  : We separate TBTT into 2 or 3 slots.
 *                           If we separate TBTT into 2 slots, then slot_period should be larger or equal to 50ms.
 *                           It means 2 slot period is
 *                               slot_period, 100-slot_period
 *                           If we separate TBTT into 3 slots, then slot_period should be less or equal to 33ms.
 *                           It means 3 slot period is
 *                               100 - 2 * slot_period, slot_period, slot_period
 * @param[in] rfon_period_len_1: rf on period of slot 1
 * @param[in] rfon_period_len_2: rf on period of slot 2
 * @param[in] rfon_period_len_3: rf on period of slot 3
 *
 * @return RTW_SUCCESS if setting TDMA parameters successful
 *         RTW_ERROR otherwise
 */
int wifi_set_tdma_param(unsigned char slot_period, unsigned char rfon_period_len_1, unsigned char rfon_period_len_2, unsigned char rfon_period_len_3);

/**
 * @brief Set LPS DTIM.
 * @param[in] dtim: In LPS, the package can be buffered at AP side.
 *                    STA leave LPS until dtim count of packages buffered at AP side.
 * @return  RTW_SUCCESS if setting LPS dtim successful.
 * @return  RTW_ERROR otherwise
 */
int wifi_set_lps_dtim(unsigned char dtim);

/**
 * @brief  Get LPS DTIM.
 * @param[out] dtim: In LPS, the package can be buffered at AP side.
 *                    STA leave LPS until dtim count of packages buffered at AP side.
 * @return  RTW_SUCCESS if getting LPS dtim successful.
 * @return  RTW_ERROR otherwise.
 */
int wifi_get_lps_dtim(unsigned char *dtim);

/**
 * @brief  Set Management Frame Protection Support.
 * @param[in] value:
 *				- NO_MGMT_FRAME_PROTECTION 		- not support
 *				- MGMT_FRAME_PROTECTION_OPTIONAL 	- capable
 *				- MGMT_FRAME_PROTECTION_REQUIRED 	- required
 * @return  RTW_SUCCESS if setting Management Frame Protection Support successful.
 * @return  RTW_ERROR otherwise.
 */
int wifi_set_mfp_support(unsigned char value);

/**
 * @brief  Trigger Wi-Fi driver to start an infrastructure Wi-Fi network.
 * @warning If a STA interface is active when this function is called, the softAP will
 *          start on the same channel as the STA. It will NOT use the channel provided!
 * @param[in]  ssid: A null terminated string containing the SSID name of the network.
 * @param[in]  security_type: 
 *                         - RTW_SECURITY_OPEN           - Open Security
 *                         - RTW_SECURITY_WPA_TKIP_PSK   - WPA Security
 *                         - RTW_SECURITY_WPA2_AES_PSK   - WPA2 Security using AES cipher
 *                         - RTW_SECURITY_WPA2_MIXED_PSK - WPA2 Security using AES and/or TKIP ciphers
 *                         - WEP security is NOT IMPLEMENTED. It is NOT SECURE!
 * @param[in]  password: A byte array containing the cleartext security key for the network.
 * @param[in]  ssid_len: The length of the SSID in bytes.
 * @param[in]  password_len: The length of the security_key in bytes.
 * @param[in]  channel: 802.11 channel number.
 * @return  RTW_SUCCESS: If successfully creates an AP.
 * @return  RTW_ERROR: If an error occurred.
 * @note  Please make sure the Wi-Fi is enabled before invoking this function. (@ref wifi_on())
 */
int wifi_start_ap(
	char 				*ssid,
	rtw_security_t		security_type,
	char 				*password,
	int 				ssid_len,
	int 				password_len,
	int					channel);

/**
 * @brief  Start an infrastructure Wi-Fi network with hidden SSID.
 * @warning If a STA interface is active when this function is called, the softAP will
 *          start on the same channel as the STA. It will NOT use the channel provided!
 *
 * @param[in]  ssid: A null terminated string containing
 *  	 the SSID name of the network to join.
 * @param[in]  security_type: Authentication type: \n
 *                         - RTW_SECURITY_OPEN           - Open Security
 *                         - RTW_SECURITY_WPA_TKIP_PSK   - WPA Security
 *                         - RTW_SECURITY_WPA2_AES_PSK   - WPA2 Security using AES cipher
 *                         - RTW_SECURITY_WPA2_MIXED_PSK - WPA2 Security using AES and/or TKIP ciphers
 *                         - WEP security is NOT IMPLEMENTED. It is NOT SECURE!
 * @param[in]  password: A byte array containing the cleartext
 *  	 security key for the network.
 * @param[in]  ssid_len: The length of the SSID in bytes.
 * @param[in]  password_len: The length of the security_key in bytes.
 * @param[in]  channel: 802.11 channel number
 *
 * @return  RTW_SUCCESS: If successfully creates an AP.
 * @return  RTW_ERROR: If an error occurred.
 */
int wifi_start_ap_with_hidden_ssid(
	char 				*ssid,
	rtw_security_t		security_type,
	char 				*password,
	int 				ssid_len,
	int 				password_len,
	int					channel);

/**
 * @brief  Initiate a scan to search for 802.11 networks.
 *
 * @param[in]  scan_type: Specifies whether the scan should 
 *                             be Active, Passive or scan 
 *                             Prohibited channels
 * @param[in]  bss_type: Specifies whether the scan should 
 *                             search for Infrastructure
 *                             networks (those using an Access
 *                             Point), Ad-hoc networks, or both
 *                             types.
 * @param[in]  result_ptr: Scan specific ssid. The first 4 
 *                             bytes is ssid lenth, and ssid name
 *                             append after it.
 *                             If no specific ssid need to scan,
 *                             PLEASE CLEAN result_ptr before pass
 *                             it into parameter.
 * @param[out]  result_ptr: a pointer to a pointer to a result 
 *                             storage structure.
 * @return  RTW_SUCCESS or RTW_ERROR
 * @note  The scan progressively accumulates results over time, and
 *  			may take between 1 and 3 seconds to complete. The results of
 *  			the scan will be individually provided to the callback
 *  			function. Note: The callback function will be executed in
 *  			the context of the RTW thread.
 * @note  When scanning specific channels, devices with a 
 *  	 strong signal strength on nearby channels may be
 *  	 detected
 */
int wifi_scan(rtw_scan_type_t                    scan_type,
				  rtw_bss_type_t                     bss_type,
				  void*                result_ptr);

/**
 * @brief  Initiate a scan to search for 802.11 networks, a higher level API based on wifi_scan
 *			to simplify the scan operation.
 * @param[in]  results_handler: The callback function which will receive and process the result data.
 * @param[in]  user_data: User specified data that will be passed directly to the callback function.
 * @return  RTW_SUCCESS or RTW_ERROR
 * @note  Callback must not use blocking functions, since it is called from the context of the RTW thread. 
 *			The callback, user_data variables will be referenced after the function returns. 
 *			Those variables must remain valid until the scan is completed.
 *			The usage of this api can reference ATWS in atcmd_wifi.c.
 */
int wifi_scan_networks(rtw_scan_result_handler_t results_handler, void* user_data);

/**
 * @brief  Initiate a scan to search for 802.11 networks with specified SSID.
 * @param[in]  results_handler: The callback function which will receive and process the result data.
 * @param[in]  user_data: User specified data that will be passed directly to the callback function.
 * @param[in]  scan_buflen: The length of the result storage structure.
 * @param[in]  ssid: The SSID of target network.
 * @param[in]  ssid_len: The length of the target network SSID.
 * @return  RTW_SUCCESS or RTW_ERROR
 * @note  Callback must not use blocking functions, since it is called from the context of the RTW thread. 
 *			The callback, user_data variables will be referenced after the function returns. 
 *			Those variables must remain valid until the scan is completed.
 */
int wifi_scan_networks_with_ssid(int (results_handler)(char*, int, char *, void *), void* user_data, int scan_buflen, char* ssid, int ssid_len);

/**
* @brief  Set the channel used to be partial scanned.
* @param[in]  channel_list: An array stores the channel list.
* @param[in]  pscan_config: the pscan_config of the channel set.
* @param[in]  length: The length of the channel_list.
* @return  RTW_SUCCESS or RTW_ERROR.
* @note  This function should be used with wifi_scan function. First, use @ref wifi_set_pscan_chan to
*			indicate which channel will be scanned, and then use @ref wifi_scan to get scanned results.
*/
int wifi_set_pscan_chan(__u8 * channel_list,__u8 * pscan_config, __u8 length);

/**
 * @brief  Get current Wi-Fi setting from driver.
 * @param[in]  ifname: the wlan interface name, can be WLAN0_NAME or WLAN1_NAME.
 * @param[out]  pSetting: Points to the rtw_wifi_setting_t structure to store the WIFI setting gotten from driver.
 * @return  RTW_SUCCESS or RTW_ERROR.
 */
int wifi_get_setting(const char *ifname,rtw_wifi_setting_t *pSetting);

/**
 * @brief  Show the network information stored in a rtw_wifi_setting_t structure.
 * @param[in]  ifname: the wlan interface name, can be WLAN0_NAME or WLAN1_NAME.
 * @param[in]  pSetting: Points to the rtw_wifi_setting_t structure which information is gotten by @ref wifi_get_setting().
 * @return  RTW_SUCCESS or RTW_ERROR.
 */
int wifi_show_setting(const char *ifname,rtw_wifi_setting_t *pSetting);

/**
 * @brief Set the network mode according to the data rate its supported.
 *			Driver works in BGN mode in default after driver initialization. This function is used to
 *			change wireless network mode for station mode before connecting to AP.
 * @param[in]  mode: Network mode to set. The value can be RTW_NETWORK_B/RTW_NETWORK_BG/RTW_NETWORK_BGN.
 * @return  RTW_SUCCESS or RTW_ERROR.
 */
int wifi_set_network_mode(rtw_network_mode_t mode);

/**
 * @brief	Get the network mode.
 *			Driver works in BGN mode in default after driver initialization. This function is used to
 *			get the current wireless network mode for station mode.
 * @param[in]  pmode: Network mode to get.
 * @return  RTW_SUCCESS or RTW_ERROR.
 */
int wifi_get_network_mode(rtw_network_mode_t *pmode);

/**
 * @brief  Set the chip to start or stop the promiscuous mode.
 * @param[in]  enabled: enabled can be set 0, 1 and 2. if enabled is zero, disable the promisc, else enable the promisc.
 *                    - 0 means disable the promisc.
 *                    - 1 means enable the promisc.
 *                    - 2 means enable the promisc special for length is used.
 * @param[in]  callback: the callback function which will 
 *  			  receive and process the netowork data.
 * @param[in]  len_used: specify if the the promisc length is used.
 *				If len_used set to 1, packet length will be saved and transferred to callback function.
 *
 * @return  RTW_SUCCESS or RTW_ERROR
 * @note  This function can be used to implement vendor specified simple configure.
 */
int wifi_set_promisc(rtw_rcr_level_t enabled, void (*callback)(unsigned char*, unsigned int, void*), unsigned char len_used);

/**
 * @brief  Let Wi-Fi enter promiscuous mode.
 * @param[in]  None
 * @return  None
 */
void wifi_enter_promisc_mode(void);

/** Set the wps phase
 *  
 * @param is_trigger_wps[in]   : to trigger wps function or not
 *
 * @return    RTW_SUCCESS or RTW_ERROR
 */
int wifi_set_wps_phase(unsigned char is_trigger_wps);

/**
 * @brief  Trigger Wi-Fi driver to restart an infrastructure Wi-Fi network.
 * @warning If a STA interface is active when this function is called, the softAP will
 *          start on the same channel as the STA. It will NOT use the channel provided!
 * @param[in]  ssid: A null terminated string containing the SSID name of the network.
 * @param[in]  security_type: 
 *                         - RTW_SECURITY_OPEN           - Open Security
 *                         - RTW_SECURITY_WPA_TKIP_PSK   - WPA Security
 *                         - RTW_SECURITY_WPA2_AES_PSK   - WPA2 Security using AES cipher
 *                         - RTW_SECURITY_WPA2_MIXED_PSK - WPA2 Security using AES and/or TKIP ciphers
 *                         - WEP security is NOT IMPLEMENTED. It is NOT SECURE!
 * @param[in]  password: A byte array containing the cleartext security key for the network.
 * @param[in]  ssid_len: The length of the SSID in bytes.
 * @param[in]  password_len: The length of the security_key in bytes.
 * @param[in]  channel: 802.11 channel number.
 * @return  RTW_SUCCESS: If successfully creates an AP.
 * @return  RTW_ERROR: If an error occurred.
 * @note  Please make sure the Wi-Fi is enabled before invoking this function. (@ref wifi_on())
 */
int wifi_restart_ap(
	unsigned char 		*ssid,
	rtw_security_t		security_type,
	unsigned char 		*password,
	int 				ssid_len,
	int 				password_len,
	int					channel);

/**
 * @brief  Set reconnection mode with configuration.
 * @param[in]  mode: Set 1/0 to enalbe/disable the reconnection mode.
 * @param[in]  retry_times: The number of retry limit.
 * @param[in]  timeout: The timeout value (in seconds).
 * @return  0 if success, otherwise return -1.
 * @note  Defining CONFIG_AUTO_RECONNECT in "autoconf.h" needs to be done before compiling,
 *			or this API won't be effective.
 * @note  The difference between @ref wifi_config_autoreconnect() and @ref wifi_set_autoreconnect() is that 
 *			user can specify the retry times and timeout value in @ref wifi_config_autoreconnect().
 *			But in @ref wifi_set_autoreconnect() these values are set with 3 retry limit and 5 seconds timeout as default.
 */
int wifi_config_autoreconnect(__u8 mode, __u8 retry_times, __u16 timeout);

/**
  * @brief  Set reconnection mode with 3 retry limit and 5 seconds timeout as default.
  * @param[in]  mode: Set 1/0 to enalbe/disable the reconnection mode.
  * @return  0 if success, otherwise return -1.
  * @note  Defining CONFIG_AUTO_RECONNECT in "autoconf.h" needs to be done before compiling,
  *			or this API won't be effective.
  * @note  The difference between @ref wifi_config_autoreconnect() and @ref wifi_set_autoreconnect() is that 
  *			user can specify the retry times and timeout value in @ref wifi_config_autoreconnect().
  *			But in @ref wifi_set_autoreconnect() these values are set with 3 retry limit and 5 seconds timeout as default.
  */
int wifi_set_autoreconnect(__u8 mode);

/**
  * @brief  Get the result of setting reconnection mode.
  * @param[out]  mode: Point to the result of setting reconnection mode.
  * @return  0 if success, otherwise return -1.
  * @note  Defining CONFIG_AUTO_RECONNECT in "autoconf.h" needs to be done before compiling,
  *			or this API won't be effective.
  */
int wifi_get_autoreconnect(__u8 *mode);

/**
  * @brief  Present the device disconnect reason while connecting.
  * @param  None
  * @return  @ref rtw_connect_error_flag_t
  *			- 0: RTW_NO_ERROR
  *			- 1: RTW_NONE_NETWORK
  *			- 2: RTW_CONNECT_FAIL
  *			- 3: RTW_WRONG_PASSWORD
  *			- 4: RTW_DHCP_FAIL
  *			- 5: RTW_UNKNOWN (initial status)
  */
int wifi_get_last_error(void);


#ifdef CONFIG_CUSTOM_IE
#ifndef BIT
#define BIT(x)	((__u32)1 << (x))
#endif

#ifndef _CUSTOM_IE_TYPE_
#define _CUSTOM_IE_TYPE_
/**
  * @brief  The enumeration is transmission type for wifi custom ie.
  */
typedef enum CUSTOM_IE_TYPE{
	PROBE_REQ = BIT(0),
	PROBE_RSP = BIT(1),
	BEACON	  = BIT(2),
}rtw_custom_ie_type_t;
#endif /* _CUSTOM_IE_TYPE_ */

/* ie format
 * +-----------+--------+-----------------------+
 * |element ID | length | content in length byte|
 * +-----------+--------+-----------------------+
 *
 * type: refer to CUSTOM_IE_TYPE
 */
#ifndef _CUS_IE_
#define _CUS_IE_
/**
  * @brief  The structure is used to set WIFI custom ie list, and type match CUSTOM_IE_TYPE.\n
  *			The ie will be transmitted according to the type.
  */
typedef struct _cus_ie{
	__u8 *ie;
	__u8 type;
}rtw_custom_ie_t, *p_rtw_custom_ie_t;
#endif /* _CUS_IE_ */

/**
  * @brief  Setup custom ie list.
  * @warning  This API can't be executed twice before deleting the previous custom ie list.
  * @param[in]  cus_ie: Pointer to WIFI CUSTOM IE list.
  * @param[in]  ie_num: The number of WIFI CUSTOM IE list.
  * @return  0 if success, otherwise return -1.
  * @note  Defininig CONFIG_CUSTOM_IE in "autoconf.h" needs to be done before compiling,
  *			or this API won't be effective.
  */
int wifi_add_custom_ie(void *cus_ie, int ie_num);

/**
  * @brief  Update the item in WIFI CUSTOM IE list.
  * @param[in]  cus_ie: Pointer to WIFI CUSTOM IE address.
  * @param[in]  ie_index: Index of WIFI CUSTOM IE list.
  * @return  0 if success, otherwise return -1.
  * @note  Defininig CONFIG_CUSTOM_IE in "autoconf.h" needs to be done before compiling,
  *			or this API won't be effective.
  */
int wifi_update_custom_ie(void *cus_ie, int ie_index);

/**
  * @brief  Delete WIFI CUSTOM IE list.
  * @param  None
  * @return  0 if success, otherwise return -1.
  * @note  Defininig CONFIG_CUSTOM_IE in "autoconf.h" needs to be done before compiling,
  *			or this API won't be effective.
  */
int wifi_del_custom_ie(void);
#endif

#ifdef CONFIG_PROMISC

/**
  * @brief  Initialize packet filter related data.
  * @param  None
  * @return  None
  */
void wifi_init_packet_filter(void);

/**
  * @brief  Add packet filter.
  * @param[in]  filter_id: The filter id.
  * @param[in]  patt: Point to the filter pattern.
  * @param[in]  rule: Point to the filter rule.
  * @return  0 if success, otherwise return -1.
  * @note  For now, the maximum number of filters is 5.
  */
int wifi_add_packet_filter(unsigned char filter_id, rtw_packet_filter_pattern_t *patt, rtw_packet_filter_rule_t rule);

/**
  * @brief  Enable the packet filter.
  * @param[in]  filter_id: The filter id.
  * @return  0 if success, otherwise return -1.
  * @note  The filter can be used only if it has been enabled.
  */
int wifi_enable_packet_filter(unsigned char filter_id);

/**
  * @brief  Disable the packet filter.
  * @param[in]  filter_id: The filter id.
  * @return  0 if success, otherwise return -1.
  */
int wifi_disable_packet_filter(unsigned char filter_id);

/**
  * @brief  Remove the packet filter.
  * @param[in]  filter_id: The filter id.
  * @return  0 if success, otherwise return -1.
  */
int wifi_remove_packet_filter(unsigned char filter_id);

/**
  * @brief: Filter out the retransmission MIMO packet in promisc mode.
  * @param[in]  enable: set 1 to enable filter retransmission pkt function, set 0 to disable this filter function.
  * @param[in]  filter_interval_ms: if 'enable' equals 0, it's useless; if 'enable' equals 1, this value
  *				indicate the time(ms) below which an adjacent pkt received will be claimed a retransmission pkt
  *				if it has the same length with the previous pkt, and driver will drop all retransmission pkts.
  *				For example, if the packet transmission time interval is 10ms, but driver receives two packets with
  *				the same length within 3ms then the second packet will be dropped if configed as wifi_retransmit_packet_filter(1,3).
  * @return 0 if success, otherwise return -1.
  */
int wifi_retransmit_packet_filter(u8 enable, u8 filter_interval_ms);

/**
  * @brief: Only receive the packets sent by the specified ap and phone in promisc mode.
  * @param[in]  enable: set 1 to enable filter, set 0 to disable this filter function.
  * @param[in]  ap_mac: if 'enable' equals 0, it's useless; if 'enable' equals 1, this value is the ap's mac address.
  * @param[in]  phone_mac: if 'enable' equals 0, it's useless; if 'enable' equals 1, this value is the phone's mac address.
  * @return  None.
  * @note  Please invoke this function as "wifi_filter_by_ap_and_phone_mac(0,NULL,NULL)" before exiting promisc mode if you enabled it during the promisc mode.
  */
void wifi_filter_by_ap_and_phone_mac(u8 enable, void *ap_mac, void *phone_mac);

/**
  * @brief:	config to report ctrl packet or not under promisc mode.
  * @param[in]	enable: set 1 to enable ctrl packet report, set 0 to disable ctrl packet report.
  *
  * @return	0 if success, otherwise return -1.
  * @note this function can only be used under promisc mode, i.e. between wifi_set_promisc(enable,...,...) and wifi_set_promisc(disable,...,...)
  */
int wifi_promisc_ctrl_packet_rpt(u8 enable);
#endif

/**
  * @brief  Get antenna infomation.
  * @param[in]  antenna: Points to store the antenna value gotten from driver, 0: main, 1: aux.
  * @return  0 if success, otherwise return -1.
  */
#ifdef CONFIG_ANTENNA_DIVERSITY
int wifi_get_antenna_info(unsigned char *antenna);
#endif // #ifdef CONFIG_ANTENNA_DIVERSITY

void wifi_set_indicate_mgnt(int enable);


/**
 * @brief  Get the information of MP driver
 * @param[out]  ability : 0x1 stand for mp driver, and 0x0 stand for normal driver
 * @return  RTW_SUCCESS
 */
int wifi_get_drv_ability(uint32_t *ability);

/**
 * @brief  Set channel plan into flash/efuse, must reboot after setting channel plan
 * @param[in]  channel_plan : the value of channel plan, define in wifi_constants.h
 * @return  RTW_SUCCESS or RTW_ERROR
 */
int wifi_set_channel_plan(uint8_t channel_plan);

/**
 * @brief  Get channel plan from calibration section
 * @param[out]  channel_plan : point to the value of channel plan, define in wifi_constants.h
 * @return  RTW_SUCCESS or RTW_ERROR
 */
int wifi_get_channel_plan(uint8_t *channel_plan);

#ifdef CONFIG_AP_MODE
/**
 * @brief  Enable packets forwarding in ap mode
 * @return  RTW_SUCCESS
 */
int wifi_enable_forwarding(void);

/**
 * @brief  Disable packets forwarding in ap mode
 * @return  RTW_SUCCESS
 */
int wifi_disable_forwarding(void);
#endif

#ifdef CONFIG_CONCURRENT_MODE
/**
 * @brief  Set flag for concurrent mode wlan1 issue_deauth when channel switched by wlan0
 *          usage: wifi_set_ch_deauth(0) -> wlan0 wifi_connect -> wifi_set_ch_deauth(1)
 * @param[in]  enable : 0 for disable and 1 for enable
 * @return  RTW_SUCCESS
 */
int wifi_set_ch_deauth(__u8 enable);
#endif

///@name Ameba1 Only 
///@{
/**
 * @brief enable AP sending QoS Null0 Data to poll Sta be alive
 * @param[in]  enabled: enabled can be set to 0,1.
 *			- 0	means enable.
 *			- 1	means disable.									
 * @return  None
 */
void wifi_set_ap_polling_sta(__u8 enabled);
///@}

#ifdef CONFIG_CONCURRENT_MODE
/**
 * @brief  Concurrent mode wlan1 to stop issueing beacon and processing rx packets
 * @param[in]  None
 * @return  None
 */
void wifi_suspend_softap(void);
#endif

/**
 * @brief  Close private log in driver, including ssid /security /mac addr. Default is opened.
 * @param[in]  None
 * @return  None
 */
void wifi_close_private_log(void);
/**
 * @brief  Open private log in driver, including ssid /security /mac addr. Default is opened.
 * @param[in]  None
 * @return  None
 */
void wifi_open_private_log(void);

/**
 * @brief  Enable to check if network is 802.11n(11n only and bgn mixed)
 * @param[in]  None
 * @return  None
 */
void wifi_enable_check_80211n(void);

/**
 * @brief  Disable to check if network is 802.11n(11n only and bgn mixed)
 * @param[in]  None
 * @return  None
 */
void wifi_enable_check_80211n(void);

/**
 * @brief  Get NHM ratio level in driver.
 * @param[out]  level: Points to the integer to store the level value gotten from driver.
				level == 0: Very good environment
				level == 1: good environment
				level == 2: normal environment
				level == 3: bad environment
				level == 4: very bad environment
 * @return  RTW_SUCCESS: If the level is succesfully retrieved.
 * @return  RTW_ERROR: If the level is not retrieved. */
int wifi_get_nhm_ratio_level(u32 *level);

/**
 * @brief  Get retry and drop packet num in firmware.
 * @param[out]  retry: Points to the structure to store the retry and drop packet num value gotten from driver.
 * @return  RTW_SUCCESS: If the value is succesfully retrieved.
 * @return  RTW_ERROR: If the value is not retrieved.
 */
int wifi_get_retry_drop_num(rtw_fw_retry_drop_t * retry);

/**
 * @brief  Get tx and rx statistics in driver.
 * @param[out]  stats: Points to the structure to store the trx statistics gotten from driver.
 * @return  RTW_SUCCESS: If the value is succesfully retrieved.
 * @return  RTW_ERROR: If the value is not retrieved.
 */
int wifi_get_sw_trx_statistics(rtw_net_device_stats_t *stats);


#ifdef __cplusplus
  }
#endif

/*\@}*/

#endif // __WIFI_API_H

//----------------------------------------------------------------------------//
