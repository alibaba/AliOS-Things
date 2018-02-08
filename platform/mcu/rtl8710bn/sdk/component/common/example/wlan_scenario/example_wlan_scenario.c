#include <platform_opts.h>
#include "FreeRTOS.h"
#include "task.h"
#include <platform/platform_stdlib.h>
#include <lwip_netconf.h>
#include "wifi_constants.h"
#include "wifi_structures.h"
#include "lwip_netconf.h"

#if CONFIG_EXAMPLE_WLAN_SCENARIO

#if CONFIG_LWIP_LAYER
extern struct netif xnetif[NET_IF_NUM]; 
#endif

#define MAC_ARG(x) ((u8*)(x))[0],((u8*)(x))[1],((u8*)(x))[2],((u8*)(x))[3],((u8*)(x))[4],((u8*)(x))[5]
static rtw_result_t scan_result_handler(rtw_scan_handler_result_t* malloced_scan_result);

char *ssid;
char *password;

/**
 * @brief  Scan network
 * @note  Process Flow:
 *		- Enable Wi-Fi with STA mode
 *		- Scan network
 */
static void scan_network(void){
	printf("\n\n[WLAN_SCENARIO_EXAMPLE] Wi-Fi example for network scan...\n");
	
	/*********************************************************************************
	*	1. Enable Wi-Fi with STA mode		
	**********************************************************************************/
	printf("\n\r[WLAN_SCENARIO_EXAMPLE] Enable Wi-Fi\n");
	if(wifi_on(RTW_MODE_STA) < 0){
		printf("\n\r[WLAN_SCENARIO_EXAMPLE] ERROR: wifi_on failed\n");
		return;
	}
	
	
	/**********************************************************************************
	*	2. Scan network			
	**********************************************************************************/
	// Scan network and print them out.
	// Can refer to fATWS() in atcmd_wifi.c and scan_result_handler() below.
	printf("\n\r[WLAN_SCENARIO_EXAMPLE] Scan network\n");
	if(wifi_scan_networks(scan_result_handler, NULL) != RTW_SUCCESS){
		printf("\n\r[WLAN_SCENARIO_EXAMPLE] ERROR: wifi_scan_networks() failed\n");
		return;
	}
	// Wait for scan finished.
	vTaskDelay(5000);
}

/**
 * @brief  Authentication
 * @note  Process Flow:
 *		- Enable Wi-Fi with STA mode
 *		- Connect to AP by different authentications
 *			- WPS-PBC
 *			- WPS-PIN static PIN
 *			- WPS-PIN dynamic PIN
 *			- open
 *			- WEP open (64 bit)
 *			- WEP open (128 bit)
 *			- WEP shared (64 bit)
 *			- WEP shared (128 bit)
 *			- WPA-PSK (TKIP)
 *			- WPA-PSK (AES)
 *			- WPA2-PSK (TKIP)
 *			- WPA2-PSK (AES)
 *		- Show Wi-Fi information
 */
static void authentication(void){
	printf("\n\n[WLAN_SCENARIO_EXAMPLE] Wi-Fi example for authentication...\n");
	
	/*********************************************************************************
	*	1. Enable Wi-Fi with STA mode			
	*********************************************************************************/
	printf("\n\r[WLAN_SCENARIO_EXAMPLE] Enable Wi-Fi\n");
	if(wifi_on(RTW_MODE_STA) < 0){
		printf("\n\r[WLAN_SCENARIO_EXAMPLE] ERROR: wifi_on failed\n");
		return;
	}
	
	
	/*********************************************************************************
	*	2. Connect to AP by different authentications		
	*********************************************************************************/	
	printf("\n\r[WLAN_SCENARIO_EXAMPLE] Connect to AP\n");
	
	// By WPS-PBC.
	///*
	char *argv[2];
	argv[1] = "pbc";
	cmd_wps(2, &argv);
	//*/
	
	// By WPS-PIN static PIN. With specified PIN code 92402508 as example.
	/*
	char *argv[3];
	argv[1] = "pin";
	argv[2] = "92402508";
	cmd_wps(3, &argv);
	*/
	
	// By WPS-PIN dynamic PIN.
	/*
	char *argv[2];
	argv[1] = "pin";
	cmd_wps(2, &argv);
	*/
	
	// By open.
	/*
	ssid = "Test_ap";
	if(wifi_connect(ssid, RTW_SECURITY_OPEN, NULL, strlen(ssid), 0, -1, NULL) == RTW_SUCCESS)
		LwIP_DHCP(0, DHCP_START);
	*/
	
	// By WEP open (64 bit).
	/*
	ssid = "Test_ap";
	password = "pass1";	// 5 ASCII
	//password = "AAAAAAAAAA";	// 10 HEX
	int key_id = 2; 	// value of key_id is 0, 1, 2, or 3.
	if(wifi_connect(ssid, RTW_SECURITY_WEP_PSK, password, strlen(ssid), strlen(password), key_id, NULL) == RTW_SUCCESS)
		LwIP_DHCP(0, DHCP_START);
	*/
	
	// By WEP open (128 bit).
	/*
	ssid = "Test_ap";
	password = "password12345";	// 13 ASCII
	//password = "AAAAABBBBBCCCCCDDDDD123456";	// 26 HEX
	int key_id = 2; 	// value of key_id is 0, 1, 2, or 3.
	if(wifi_connect(ssid, RTW_SECURITY_WEP_PSK, password, strlen(ssid), strlen(password), key_id, NULL) == RTW_SUCCESS)
		LwIP_DHCP(0, DHCP_START);
	*/
	
	// By WEP shared (64 bit).
	/*
	ssid = "Test_ap";
	password = "pass1";	// 5 ASCII
	//password = "AAAAAAAAAA";	// 10 HEX
	int key_id = 2; 	// value of key_id is 0, 1, 2, or 3.
	if(wifi_connect(ssid, RTW_SECURITY_WEP_SHARED, password, strlen(ssid), strlen(password), key_id, NULL) == RTW_SUCCESS)
		LwIP_DHCP(0, DHCP_START);
	*/
	
	// By WEP shared (128 bit).
	/*
	ssid = "Test_ap";
	password = "password12345";	// 13 ASCII
	//password = "AAAAABBBBBCCCCCDDDDD123456";	// 26 HEX
	int key_id = 2; 	// value of key_id is 0, 1, 2, or 3.
	if(wifi_connect(ssid, RTW_SECURITY_WEP_SHARED, password, strlen(ssid), strlen(password), key_id, NULL) == RTW_SUCCESS)
		LwIP_DHCP(0, DHCP_START);
	*/
	
	// By WPA-PSK (TKIP)
	/*
	ssid = "Test_ap";
	password = "12345678";
	if(wifi_connect(ssid, RTW_SECURITY_WPA_TKIP_PSK, password, strlen(ssid), strlen(password), -1, NULL) == RTW_SUCCESS)
		LwIP_DHCP(0, DHCP_START);
	*/

	// By WPA-PSK (AES)
	/*
	ssid = "Test_ap";
	password = "12345678";
	if(wifi_connect(ssid, RTW_SECURITY_WPA_AES_PSK, password, strlen(ssid), strlen(password), -1, NULL) == RTW_SUCCESS)
		LwIP_DHCP(0, DHCP_START);
	*/
	
	// By WPA2-PSK (TKIP)
	/*
	ssid = "Test_ap";
	password = "12345678";
	if(wifi_connect(ssid, RTW_SECURITY_WPA2_TKIP_PSK, password, strlen(ssid), strlen(password), -1, NULL) == RTW_SUCCESS)
		LwIP_DHCP(0, DHCP_START);
	*/

	// By WPA2-PSK (AES)
	/*
	ssid = "Test_ap";
	password = "12345678";
	if(wifi_connect(ssid, RTW_SECURITY_WPA2_AES_PSK, password, strlen(ssid), strlen(password), -1, NULL) == RTW_SUCCESS)
		LwIP_DHCP(0, DHCP_START);
	*/
	
	
	/*********************************************************************************
	*	3. Show Wi-Fi information
	*********************************************************************************/
	printf("\n\r[WLAN_SCENARIO_EXAMPLE] Show Wi-Fi information\n");
	rtw_wifi_setting_t setting;
	wifi_get_setting(WLAN0_NAME,&setting);
	wifi_show_setting(WLAN0_NAME,&setting);
}

/**
 * @brief  Wi-Fi example for mode switch case 1: Switch to infrastructure AP mode.
 * @note  Process Flow:
 *		- Disable Wi-Fi
 *		- Enable Wi-Fi with AP mode
 *		- Start AP
 *		- Check AP running
 *		- Start DHCP server
 */
static void mode_switch_1(void){
	printf("\n\n[WLAN_SCENARIO_EXAMPLE] Wi-Fi example mode switch case 1...\n");
	
	/*********************************************************************************
	*	1. Disable Wi-Fi			
	*********************************************************************************/
	printf("\n\r[WLAN_SCENARIO_EXAMPLE] Disable Wi-Fi\n");
	wifi_off();
	vTaskDelay(20);	
	
	
	/*********************************************************************************
	*	2. Enable Wi-Fi with AP mode			
	*********************************************************************************/
	printf("\n\r[WLAN_SCENARIO_EXAMPLE] Enable Wi-Fi with AP mode\n");	
	if(wifi_on(RTW_MODE_AP) < 0){
		printf("\n\r[WLAN_SCENARIO_EXAMPLE] ERROR: wifi_on failed\n");
		return;
	}
	
	
	/*********************************************************************************
	*	3. Start AP			
	*********************************************************************************/
	printf("\n\r[WLAN_SCENARIO_EXAMPLE] Start AP\n");
	ssid = "RTL_AP";
	rtw_security_t security_type = RTW_SECURITY_WPA2_AES_PSK;
	password = "12345678";
	int channel = 6;
	if(wifi_start_ap(ssid, security_type, password, strlen(ssid), strlen(password), channel) < 0) {
		printf("\n\r[WLAN_SCENARIO_EXAMPLE] ERROR: wifi_start_ap failed\n");
		return;
	}
	
	
	/*********************************************************************************
	*	4. Check AP running		
	*********************************************************************************/	
	printf("\n\r[WLAN_SCENARIO_EXAMPLE] Check AP running\n");
	int timeout = 20;
	while(1) {
		char essid[33];
		if(wext_get_ssid(WLAN0_NAME, (unsigned char *) essid) > 0) {
			if(strcmp((const char *) essid, (const char *)ssid) == 0) {
				printf("\n\r[WLAN_SCENARIO_EXAMPLE] %s started\n", ssid);
				break;
			}
		}
		if(timeout == 0) {
			printf("\n\r[WLAN_SCENARIO_EXAMPLE] ERROR: Start AP timeout\n");	
			return;
		}
		vTaskDelay(1 * configTICK_RATE_HZ);
		timeout --;
	}
	
	
	/*********************************************************************************
	*	5. Start DHCP server
	*********************************************************************************/	
	printf("\n\r[WLAN_SCENARIO_EXAMPLE] Start DHCP server\n");
	// For more setting about DHCP, please reference fATWA in atcmd_wifi.c.
#if CONFIG_LWIP_LAYER
	dhcps_init(&xnetif[0]);
#endif
}

/**
 * @brief  Wi-Fi example for mode switch case 2: Switch to infrastructure STA mode.
 * @note  Process Flow:
 *		- Disable Wi-Fi
 *		- Enable Wi-Fi with STA mode
 *		- Connect to AP using STA mode
 *		- Show Wi-Fi information
 */
static void mode_switch_2(void){
	printf("\n\n[WLAN_SCENARIO_EXAMPLE] Wi-Fi example mode switch case 2...\n");
	
	/*********************************************************************************
	*	1. Disable Wi-Fi			
	*********************************************************************************/
	printf("\n\r[WLAN_SCENARIO_EXAMPLE] Disable Wi-Fi\n");
	wifi_off();
	vTaskDelay(20);	
	
	
	/*********************************************************************************
	*	2. Enable Wi-Fi with STA mode			
	*********************************************************************************/
	printf("\n\r[WLAN_SCENARIO_EXAMPLE] Enable Wi-Fi with STA mode\n");	
	if(wifi_on(RTW_MODE_STA) < 0){
		printf("\n\r[WLAN_SCENARIO_EXAMPLE] ERROR: wifi_on failed\n");
		return;
	}
	
	/*********************************************************************************
	*	3. Connect to AP using STA mode	
	*********************************************************************************/	
	printf("\n\r[WLAN_SCENARIO_EXAMPLE] Connect to AP using STA mode\n");
	ssid = "Test_ap";
	password = "12345678";
	if(wifi_connect(ssid, RTW_SECURITY_WPA2_AES_PSK, password, strlen(ssid), strlen(password), -1, NULL) == RTW_SUCCESS)
		LwIP_DHCP(0, DHCP_START);
	
	
	/*********************************************************************************
	*	4. Show Wi-Fi information
	*********************************************************************************/
	printf("\n\r[WLAN_SCENARIO_EXAMPLE] Show Wi-Fi information\n");
	rtw_wifi_setting_t setting;
	wifi_get_setting(WLAN0_NAME,&setting);
	wifi_show_setting(WLAN0_NAME,&setting);
}

/**
 * @brief  Wi-Fi example for mode switch case 3: Switch to infrastructure P2P Autonomous GO mode.
 * @note  Process Flow:
 *		- Enable Wi-Fi Direct mode
 *		- Enable Wi-Fi Direct Automonous GO
 *		- Show Wi-Fi Direct Information
 *		- Disable Wi-Fi Direct mode
 */
static void mode_switch_3(void){
	printf("\n\n[WLAN_SCENARIO_EXAMPLE] Wi-Fi example mode switch case 3...\n");
	
	/*********************************************************************************
	*	1. Enable Wi-Fi Direct mode			
	*********************************************************************************/
	printf("\n\r[WLAN_SCENARIO_EXAMPLE] Enable Wi-Fi Direct mode\n");
	// Start Wi-Fi Direct mode.
	// cmd_wifi_p2p_start() will re-enable the Wi-Fi with P2P mode and initialize P2P data.
	cmd_wifi_p2p_start(NULL, NULL);

	
	/*********************************************************************************
	*	2. Enable Wi-Fi Direct Automonous GO		
	*********************************************************************************/
	printf("\n\r[WLAN_SCENARIO_EXAMPLE] Enable Wi-Fi Direct Automonous GO\n");
	// Start Wi-Fi Direct Automonous GO.
	// The GO related parameters can be set in cmd_wifi_p2p_auto_go_start() function declaration.
	if(cmd_wifi_p2p_auto_go_start(NULL, NULL) < 0){
		printf("\n\r[WLAN_SCENARIO_EXAMPLE] ERROR: cmd_wifi_p2p_auto_go_start() failed\n");
		return;
	}
	
	
	/*********************************************************************************
	*	3. Show Wi-Fi Direct Information		
	*********************************************************************************/
	printf("\n\r[WLAN_SCENARIO_EXAMPLE] Show Wi-Fi Direct Information\n");
	// Show the Wi-Fi Direct Info.
	cmd_p2p_info();
	
	
	vTaskDelay(60000);	

	
	/*********************************************************************************
	*	4. Disable Wi-Fi Direct mode	
	*********************************************************************************/
	printf("\n\r[WLAN_SCENARIO_EXAMPLE] Disable Wi-Fi Direct mode\n");
	// Disable Wi-Fi Direct GO. Will disable Wi-Fi either.
	// This command has to be invoked to release the P2P resource.
	cmd_wifi_p2p_stop(NULL, NULL);	
}

/**
 * @brief  Wi-Fi example for mode switch case 4: Mode switching time between AP and STA.
 * @note  Process Flow:
 *		- Enable Wi-Fi with STA mode
 *		- Disable STA mode and start AP (First measurement)
 *		- Stop AP and enable STA mode (Second measurement)
 */
static void mode_switch_4(void){
	printf("\n\n[WLAN_SCENARIO_EXAMPLE] Wi-Fi example mode switch case 4...\n");
	// First measurement.
	unsigned long tick_STA_to_AP_begin;
	unsigned long tick_STA_to_AP_end;	
	// Second measurement.
	unsigned long tick_AP_to_STA_begin;
	unsigned long tick_AP_to_STA_end;
	
	/*********************************************************************************
	*	1. Enable Wi-Fi with STA mode
	*********************************************************************************/
	printf("\n\r[WLAN_SCENARIO_EXAMPLE] Enable Wi-Fi with STA mode\n");
	if(wifi_on(RTW_MODE_STA) < 0){
		printf("\n\r[WLAN_SCENARIO_EXAMPLE] ERROR: wifi_on failed\n");
		return;
	}
	
	/*********************************************************************************
	*	2. Disable STA mode and start AP (First measurement)
	*********************************************************************************/
	printf("\n\r[WLAN_SCENARIO_EXAMPLE] Disable STA mode and start AP (First measurement)\n");
	// Begin time.
	tick_STA_to_AP_begin = xTaskGetTickCount();
	
	wifi_off();
	
	if(wifi_on(RTW_MODE_AP) < 0){
		printf("\n\r[WLAN_SCENARIO_EXAMPLE] ERROR: wifi_on failed\n");
		return;
	}
	
	ssid = "RTL_AP";
	rtw_security_t security_type = RTW_SECURITY_WPA2_AES_PSK;
	password = "12345678";
	int channel = 6;
	if(wifi_start_ap(ssid, security_type, password, strlen(ssid), strlen(password), channel) < 0) {
		printf("\n\r[WLAN_SCENARIO_EXAMPLE] ERROR: wifi_start_ap failed\n");
		return;
	}
	
	int timeout = 20;
	while(1) {
		char essid[33];
		if(wext_get_ssid(WLAN0_NAME, (unsigned char *) essid) > 0) {
			if(strcmp((const char *) essid, (const char *)ssid) == 0) {
				printf("\n\r[WLAN_SCENARIO_EXAMPLE] %s started\n", ssid);
				break;
			}
		}
		if(timeout == 0) {
			printf("\n\r[WLAN_SCENARIO_EXAMPLE] ERROR: Start AP timeout\n");	
			return;
		}
		vTaskDelay(1 * configTICK_RATE_HZ);
		timeout --;
	}
	
#if CONFIG_LWIP_LAYER
	dhcps_init(&xnetif[0]);
#endif	
	// End time.
	tick_STA_to_AP_end = xTaskGetTickCount();
	printf("\n\r[WLAN_SCENARIO_EXAMPLE] Time diff switch from STA mode to start AP: %d ms\n",
	       (tick_STA_to_AP_end - tick_STA_to_AP_begin));
	
	
	vTaskDelay(60000);
	
	
	/*********************************************************************************
	*	3. Stop AP and enable STA mode (Second measurement)
	*********************************************************************************/
	printf("\n\r[WLAN_SCENARIO_EXAMPLE] Stop AP and enable STA mode (Second measurement)\n");
	// Begin time.
	tick_AP_to_STA_begin = xTaskGetTickCount();
	
	wifi_off();
	
	if(wifi_on(RTW_MODE_STA) < 0){
		printf("\n\r[WLAN_SCENARIO_EXAMPLE] ERROR: wifi_on failed\n");
		return;
	}
	
	// Connect to AP by WPA2-AES security
	ssid = "Test_ap";
	password = "12345678";
	if(wifi_connect(ssid, RTW_SECURITY_WPA2_AES_PSK, password, strlen(ssid), strlen(password), -1, NULL) == RTW_SUCCESS)
		LwIP_DHCP(0, DHCP_START);
	
	// End time.
	tick_AP_to_STA_end = xTaskGetTickCount();
	printf("\n\r[WLAN_SCENARIO_EXAMPLE] Time diff switch from stop AP to enable STA mode: %d ms\n",
	       (tick_AP_to_STA_end - tick_AP_to_STA_begin));
}

/**
 * @brief  Wi-Fi example for mode switch case 5: Mode switching time between P2P(autonomousGO) and STA.
 * @note  Process Flow:
 *		- Enable Wi-Fi with STA mode
 *		- Disable STA mode and start P2P GO (First measurement)
 *		- Stop P2P GO and enable STA mode (Second measurement)
 */
static void mode_switch_5(void){
	printf("\n\n[WLAN_SCENARIO_EXAMPLE] Wi-Fi example mode switch case 5...\n");
	// First measurement.
	unsigned long tick_STA_to_P2PGO_begin;
	unsigned long tick_STA_to_P2PGO_end;	
	// Second measurement.
	unsigned long tick_P2PGO_to_STA_begin;
	unsigned long tick_P2PGO_to_STA_end;
	
	/*********************************************************************************
	*	1. Enable Wi-Fi with STA mode
	*********************************************************************************/
	printf("\n\r[WLAN_SCENARIO_EXAMPLE] Enable Wi-Fi with STA mode\n");
	if(wifi_on(RTW_MODE_STA) < 0){
		printf("\n\r[WLAN_SCENARIO_EXAMPLE] ERROR: wifi_on failed\n");
		return;
	}
	
	/*********************************************************************************
	*	2. Disable STA mode and start P2P GO (First measurement)
	*********************************************************************************/
	printf("\n\r[WLAN_SCENARIO_EXAMPLE] Disable STA mode and start AP (First measurement)\n");
	// Begin time.
	tick_STA_to_P2PGO_begin = xTaskGetTickCount();
	
	wifi_off();
	
	cmd_wifi_p2p_start(NULL, NULL);
	
	if(cmd_wifi_p2p_auto_go_start(NULL, NULL) < 0){
		printf("\n\r[WLAN_SCENARIO_EXAMPLE] ERROR: cmd_wifi_p2p_auto_go_start() failed\n");
		return;
	}
	
	// End time.
	tick_STA_to_P2PGO_end = xTaskGetTickCount();
	printf("\n\r[WLAN_SCENARIO_EXAMPLE] Time diff switch from STA mode to start P2P GO: %d ms\n",
	       (tick_STA_to_P2PGO_end - tick_STA_to_P2PGO_begin));
	
	
	vTaskDelay(60000);
	
	
	/*********************************************************************************
	*	3. Stop P2P GO and enable STA mode (Second measurement)
	*********************************************************************************/
	printf("\n\r[WLAN_SCENARIO_EXAMPLE] Stop P2P GO and enable STA mode (Second measurement)\n");
	// Begin time.
	tick_P2PGO_to_STA_begin = xTaskGetTickCount();
	
	cmd_wifi_p2p_stop(NULL, NULL);
	
	if(wifi_on(RTW_MODE_STA) < 0){
		printf("\n\r[WLAN_SCENARIO_EXAMPLE] ERROR: wifi_on failed\n");
		return;
	}
	
	// Connect to AP by WPA2-AES security
	ssid = "Test_ap";
	password = "12345678";
	if(wifi_connect(ssid, RTW_SECURITY_WPA2_AES_PSK, password, strlen(ssid), strlen(password), -1, NULL) == RTW_SUCCESS)
		LwIP_DHCP(0, DHCP_START);
	
	// End time.
	tick_P2PGO_to_STA_end = xTaskGetTickCount();
	printf("\n\r[WLAN_SCENARIO_EXAMPLE] Time diff switch from stop P2P GO to enable STA mode: %d ms\n",
	       (tick_P2PGO_to_STA_end - tick_P2PGO_to_STA_begin));
}


/**
 * @brief  Wi-Fi example for mode switch case 6: Switch to infrastructure AP mode with hidden SSID.
 * @note  Process Flow:
 *		- Disable Wi-Fi
 *		- Enable Wi-Fi with AP mode
 *		- Start AP with hidden SSID
 *		- Check AP running
 *		- Start DHCP server
 */
static void mode_switch_6(void){
	printf("\n\n[WLAN_SCENARIO_EXAMPLE] Wi-Fi example mode switch case 6...\n");

	/*********************************************************************************
	*	1. Disable Wi-Fi
	*********************************************************************************/
	printf("\n\r[WLAN_SCENARIO_EXAMPLE] Disable Wi-Fi\n");
	wifi_off();
	vTaskDelay(20);


	/*********************************************************************************
	*	2. Enable Wi-Fi with AP mode
	*********************************************************************************/
	printf("\n\r[WLAN_SCENARIO_EXAMPLE] Enable Wi-Fi with AP mode\n");
	if(wifi_on(RTW_MODE_AP) < 0){
		printf("\n\r[WLAN_SCENARIO_EXAMPLE] ERROR: wifi_on failed\n");
		return;
	}


	/*********************************************************************************
	*	3. Start AP with hidden SSID
	*********************************************************************************/
	printf("\n\r[WLAN_SCENARIO_EXAMPLE] Start AP with hidden SSID\n");
	ssid = "RTL_AP";
	rtw_security_t security_type = RTW_SECURITY_WPA2_AES_PSK;
	password = "12345678";
	int channel = 6;
	if(wifi_start_ap_with_hidden_ssid(ssid, security_type, password, strlen(ssid), strlen(password), channel) < 0) {
		printf("\n\r[WLAN_SCENARIO_EXAMPLE] ERROR: wifi_start_ap_with_hidden_ssid failed\n");
		return;
	}


	/*********************************************************************************
	*	4. Check AP running
	*********************************************************************************/
	printf("\n\r[WLAN_SCENARIO_EXAMPLE] Check AP running\n");
	int timeout = 20;
	while(1) {
		char essid[33];
		if(wext_get_ssid(WLAN0_NAME, (unsigned char *) essid) > 0) {
			if(strcmp((const char *) essid, (const char *)ssid) == 0) {
				printf("\n\r[WLAN_SCENARIO_EXAMPLE] %s started\n", ssid);
				break;
			}
		}
		if(timeout == 0) {
			printf("\n\r[WLAN_SCENARIO_EXAMPLE] ERROR: Start AP timeout\n");
			return;
		}
		vTaskDelay(1 * configTICK_RATE_HZ);
		timeout --;
	}


	/*********************************************************************************
	*	5. Start DHCP server
	*********************************************************************************/
	printf("\n\r[WLAN_SCENARIO_EXAMPLE] Start DHCP server\n");
	// For more setting about DHCP, please reference fATWA in atcmd_wifi.c.
#if CONFIG_LWIP_LAYER
	dhcps_init(&xnetif[0]);
#endif
}

/**
 * @brief  Wi-Fi example for mode switch case 7: Mode switching between concurrent mode and STA.
 * @note  Process Flow:
 *		- Enable Wi-Fi with concurrent (STA + AP) mode
 *		  - Disable Wi-Fi
 *		  - Start AP
 *		  - Check AP running
 *		  - Start DHCP server
 *		  - Connect to AP using STA mode
 *		- Disable concurrent (STA + AP) mode and start STA mode
 */
static void mode_switch_7(void){
	printf("\n\n[WLAN_SCENARIO_EXAMPLE] Wi-Fi example mode switch case 7...\n");

	/*********************************************************************************
	*	1. Enable Wi-Fi with concurrent (STA + AP) mode
	*********************************************************************************/
	printf("\n\r[WLAN_SCENARIO_EXAMPLE] Enable Wi-Fi with concurrent (STA + AP) mode\n");

	/*********************************************************************************
	*	1-1. Disable Wi-Fi
	*********************************************************************************/
	printf("\n\r[WLAN_SCENARIO_EXAMPLE] Disable Wi-Fi\n");
	wifi_off();
	vTaskDelay(20);

	/*********************************************************************************
	*	1-2. Enable Wi-Fi with STA + AP mode
	*********************************************************************************/
	printf("\n\r[WLAN_SCENARIO_EXAMPLE] Enable Wi-Fi with STA + AP mode\n");
	if(wifi_on(RTW_MODE_STA_AP) < 0){
		printf("\n\r[WLAN_SCENARIO_EXAMPLE] ERROR: wifi_on failed\n");
		return;
	}

	/*********************************************************************************
	*	1-3. Start AP
	*********************************************************************************/
	printf("\n\r[WLAN_SCENARIO_EXAMPLE] Start AP\n");
	ssid = "RTL_AP";
	rtw_security_t security_type = RTW_SECURITY_WPA2_AES_PSK;
	password = "12345678";
	int channel = 6;
	if(wifi_start_ap(ssid, security_type, password, strlen(ssid), strlen(password), channel) < 0) {
		printf("\n\r[WLAN_SCENARIO_EXAMPLE] ERROR: wifi_start_ap failed\n");
		return;
	}

	/*********************************************************************************
	*	1-4. Check AP running
	*********************************************************************************/
	printf("\n\r[WLAN_SCENARIO_EXAMPLE] Check AP running\n");
	int timeout = 20;
	while(1) {
		char essid[33];
		if(wext_get_ssid(WLAN1_NAME, (unsigned char *) essid) > 0) {
			if(strcmp((const char *) essid, (const char *)ssid) == 0) {
				printf("\n\r[WLAN_SCENARIO_EXAMPLE] %s started\n", ssid);
				break;
			}
		}
		if(timeout == 0) {
			printf("\n\r[WLAN_SCENARIO_EXAMPLE] ERROR: Start AP timeout\n");
			return;
		}
		vTaskDelay(1 * configTICK_RATE_HZ);
		timeout --;
	}

	/*********************************************************************************
	*	1-5. Start DHCP server
	*********************************************************************************/
	printf("\n\r[WLAN_SCENARIO_EXAMPLE] Start DHCP server\n");
#if CONFIG_LWIP_LAYER
	dhcps_init(&xnetif[1]);
	vTaskDelay(1000);
#endif

	/*********************************************************************************
	*	1-6. Connect to AP using STA mode
	*********************************************************************************/
	printf("\n\r[WLAN_SCENARIO_EXAMPLE] Connect to AP\n");
	ssid = "Test_ap";
	password = "12345678";
	if(wifi_connect(ssid, RTW_SECURITY_WPA2_AES_PSK, password, strlen(ssid), strlen(password), -1, NULL) == RTW_SUCCESS)
		LwIP_DHCP(0, DHCP_START);

	/*********************************************************************************
	*	2. Disable concurrent (STA + AP) mode and start STA mode
	*********************************************************************************/
	printf("\n\r[WLAN_SCENARIO_EXAMPLE] Disable concurrent (STA + AP) mode and start STA mode\n");
	wifi_off();
	vTaskDelay(20);

	if(wifi_on(RTW_MODE_STA) < 0){
		printf("\n\r[WLAN_SCENARIO_EXAMPLE] ERROR: wifi_on failed\n");
		return;
	}

	ssid = "Test_ap";
	password = "12345678";
	if(wifi_connect(ssid, RTW_SECURITY_WPA2_AES_PSK, password, strlen(ssid), strlen(password), -1, NULL) == RTW_SUCCESS)
		LwIP_DHCP(0, DHCP_START);
}



/**
 * @brief  Wi-Fi example for scenario case 1.
 * @note  Process Flow:
 *		- Enable Wi-Fi with STA mode
 *		- Connect to AP by WPS enrollee static PIN mode (If failed, re-connect one time.)
 *		- Enable Wi-Fi Direct GO 
 *			(It will re-enable WiFi, the original connection to AP would be broken.)
 */
static void scenario_1(void){
	printf("\n\n[WLAN_SCENARIO_EXAMPLE] Wi-Fi example scenario case 1...\n");
	
	/*********************************************************************************
	*	1. Enable Wi-Fi with STA mode			
	*********************************************************************************/
	printf("\n\r[WLAN_SCENARIO_EXAMPLE] Enable Wi-Fi\n");
	if(wifi_on(RTW_MODE_STA) < 0){
		printf("\n\r[WLAN_SCENARIO_EXAMPLE] ERROR: wifi_on failed\n");
		return;
	}
	
	
	/*********************************************************************************
	*	2. Connect to AP by WPS enrollee PIN mode			
	*********************************************************************************/
	printf("\n\r[WLAN_SCENARIO_EXAMPLE] Connect to AP by WPS enrollee PIN mode with PIN code: 92402508\n");
	// Start static WPS-PIN enrollee with PIN code: 92402508.
	// As the process beginning, please enter the PIN code in AP side.
	// It will take at most 2 min to do the procedure.
	char *argv[3];
	argv[1] = "pin";
	argv[2] = "92402508";
	cmd_wps(3, &argv);
	
	// If not connected, retry one time.
	if(wifi_is_connected_to_ap() != RTW_SUCCESS){
		printf("\n\r[WLAN_SCENARIO_EXAMPLE] WPS enrollee failed, reconnect one time\n");
		cmd_wps(3, &argv);
	}
	
	
	/*********************************************************************************
	*	3. Enable Wi-Fi Direct GO			
	*********************************************************************************/
	printf("\n\r[WLAN_SCENARIO_EXAMPLE] Enable Wi-Fi Direct GO\n");
	// Start Wi-Fi Direct mode.
	// cmd_wifi_p2p_start() will re-enable the Wi-Fi with P2P mode and initialize P2P data.
	cmd_wifi_p2p_start(NULL, NULL);
	// Start Wi-Fi Direct Group Owner mode.
	// The GO related parameters can be set in cmd_wifi_p2p_auto_go_start() function declaration.
	if(cmd_wifi_p2p_auto_go_start(NULL, NULL) < 0){
		printf("\n\r[WLAN_SCENARIO_EXAMPLE] ERROR: cmd_wifi_p2p_auto_go_start() failed\n");
		return;
	}
	printf("\n\r[WLAN_SCENARIO_EXAMPLE] Wi-Fi Direct Group Owner mode enabled\n");
	
	// Show the Wi-Fi Direct Info.
	cmd_p2p_info();
}

/**
 * @brief  Wi-Fi example for scenario case 2.
 * @note  Process Flow:
 *		- Enable Wi-Fi Direct GO
 *		- Disable Wi-Fi Direct GO, and enable Wi-Fi with STA mode 
 *			(Disable Wi-Fi Direct GO must be done to release P2P resource.)
 *		- Connect to AP by WPS enrollee PBC mode (If failed, re-connect one time.)
 */
static void scenario_2(void){
	printf("\n\n[WLAN_SCENARIO_EXAMPLE] Wi-Fi example scenario case 2...\n");
	
	/*********************************************************************************
	*	1. Enable Wi-Fi Direct GO			
	*********************************************************************************/
	printf("\n\r[WLAN_SCENARIO_EXAMPLE] Enable Wi-Fi Direct GO\n");
	// Start Wi-Fi Direct mode.
	// cmd_wifi_p2p_start() will re-enable the Wi-Fi with P2P mode and initialize P2P data.
	cmd_wifi_p2p_start(NULL, NULL);
	// Start Wi-Fi Direct Group Owner mode.
	// The GO related parameters can be set in cmd_wifi_p2p_auto_go_start() function declaration.
	if(cmd_wifi_p2p_auto_go_start(NULL, NULL) < 0){
		printf("\n\r[WLAN_SCENARIO_EXAMPLE] ERROR: cmd_wifi_p2p_auto_go_start() failed\n");
		return;
	}
	printf("\n\r[WLAN_SCENARIO_EXAMPLE] Wi-Fi Direct Group Owner mode enabled\n");
	// Show the Wi-Fi Direct Info.
	cmd_p2p_info();
	
	
	/*********************************************************************************
	*	2. Disable Wi-Fi Direct GO, and enable Wi-Fi with STA mode		
	*********************************************************************************/
	printf("\n\r[WLAN_SCENARIO_EXAMPLE] Disable Wi-Fi Direct GO, and enable Wi-Fi\n");
	// Disable Wi-Fi Direct GO.
	// This command has to be invoked to release the P2P resource.
	cmd_wifi_p2p_stop(NULL, NULL);
	// Enable Wi-Fi with STA mode.
	if(wifi_on(RTW_MODE_STA) < 0){
		printf("\n\r[WLAN_SCENARIO_EXAMPLE] ERROR: wifi_on() failed\n");
		return;
	}
	
	
	/*********************************************************************************
	*	3. Connect to AP by WPS enrollee PBC mode			
	*********************************************************************************/
	printf("\n\r[WLAN_SCENARIO_EXAMPLE] Connect to AP by WPS enrollee PBC mode\n");
	// Start WPS-PBC enrollee.
	// As the process beginning, please push the WPS button on AP.
	// It will take at most 2 min to do the procedure.
	char *argv[2];
	argv[1] = "pbc";
	cmd_wps(2, &argv);
	
	// If not connected, retry one time.
	if(wifi_is_connected_to_ap() != RTW_SUCCESS){
		printf("\n\r[WLAN_SCENARIO_EXAMPLE] WPS enrollee failed, reconnect one time\n");
		cmd_wps(2, &argv);
	}
}

/**
 * @brief  Wi-Fi example for scenario case 3.
 * @note  Process Flow:
 *		- Enable Wi-Fi with STA mode
 *		- Scan network
 *		- Connect to AP use STA mode (If failed, re-connect one time.)
 *		- Enable Wi-Fi Direct GO 
 *			(It will re-enable WiFi, the original connection to AP would be broken.)
 */
static void scenario_3(void){
	printf("\n\n[WLAN_SCENARIO_EXAMPLE] Wi-Fi example scenario case 3...\n");
	
	/*********************************************************************************
	*	1. Enable Wi-Fi with STA mode		
	**********************************************************************************/
	printf("\n\r[WLAN_SCENARIO_EXAMPLE] Enable Wi-Fi\n");
	if(wifi_on(RTW_MODE_STA) < 0){
		printf("\n\r[WLAN_SCENARIO_EXAMPLE] ERROR: wifi_on failed\n");
		return;
	}
	
	
	/**********************************************************************************
	*	2. Scan network			
	**********************************************************************************/
	// Scan network and print them out.
	// Can refer to fATWS() in atcmd_wifi.c.
	printf("\n\r[WLAN_SCENARIO_EXAMPLE] Scan network\n");
	if(wifi_scan_networks(scan_result_handler, NULL) != RTW_SUCCESS){
		printf("\n\r[WLAN_SCENARIO_EXAMPLE] ERROR: wifi_scan_networks() failed\n");
		return;
	}
	// Wait for scan finished.
	vTaskDelay(5000);
	
	
	/**********************************************************************************
	*	3. Connect to AP use STA mode (If failed, re-connect one time.)			
	**********************************************************************************/
	printf("\n\r[WLAN_SCENARIO_EXAMPLE] Connect to AP use STA mode\n");
	
	// Set the auto-reconnect mode with retry 1 time(limit is 2) and timeout 5 seconds.
	// This command need to be set before invoke wifi_connect() to make reconnection work.
	wifi_config_autoreconnect(1, 2, 5);
	
	// Connect to AP with PSK-WPA2-AES.
	ssid = "Test_ap";
	password = "12345678";
	if(wifi_connect(ssid, RTW_SECURITY_WPA2_AES_PSK, password, strlen(ssid), strlen(password), -1, NULL) == RTW_SUCCESS)
		LwIP_DHCP(0, DHCP_START);
	
	// Show Wi-Fi info.
	rtw_wifi_setting_t setting;
	wifi_get_setting(WLAN0_NAME,&setting);
	wifi_show_setting(WLAN0_NAME,&setting);
	
	
	/**********************************************************************************
	*	4. Enable Wi-Fi Direct GO			
	**********************************************************************************/
	printf("\n\r[WLAN_SCENARIO_EXAMPLE] Enable Wi-Fi Direct GO\n");
	// Start Wi-Fi Direct mode.
	// cmd_wifi_p2p_start() will re-enable the Wi-Fi with P2P mode and initialize P2P data.
	cmd_wifi_p2p_start(NULL, NULL);
	// Start Wi-Fi Direct Group Owner mode.
	// The GO related parameters can be set in cmd_wifi_p2p_auto_go_start() function declaration.
	if(cmd_wifi_p2p_auto_go_start(NULL, NULL) < 0){
		printf("\n\r[WLAN_SCENARIO_EXAMPLE] ERROR: cmd_wifi_p2p_auto_go_start() failed\n");
		return;
	}
	printf("\n\r[WLAN_SCENARIO_EXAMPLE] Wi-Fi Direct Group Owner mode enabled\n");
	
	// Show the Wi-Fi Direct Info.
	cmd_p2p_info();
}

/**
 * @brief  Wi-Fi example for scenario case 4.
 * @note  Process Flow:
 *		- Enable Wi-Fi with STA mode
 *		- Connect to AP by WPS enrollee PBC mode (If failed, re-connect one time.)
 *		- Disconnect from AP
 *		- Enable Wi-Fi Direct GO
 *		- Disable Wi-Fi Direct GO, and enable Wi-Fi with STA mode
 *			(Disable Wi-Fi Direct GO must be done to release P2P resource.)
 *		- Connect to AP use STA mode (If failed, re-connect one time.)
 *		- Disconnect from AP
 *		- Disable Wi-Fi
 */
static void scenario_4(void){
	printf("\n\n[WLAN_SCENARIO_EXAMPLE] Wi-Fi example scenario case 4...\n");
	
	/**********************************************************************************
	*	1. Enable Wi-Fi with STA mode		
	**********************************************************************************/
	printf("\n\r[WLAN_SCENARIO_EXAMPLE] Enable Wi-Fi\n");
	if(wifi_on(RTW_MODE_STA) < 0){
		printf("\n\r[WLAN_SCENARIO_EXAMPLE] ERROR: wifi_on failed\n");
		return;
	}
	
	
	/**********************************************************************************
	*	2. Connect to AP by WPS enrollee PBC mode			
	**********************************************************************************/
	printf("\n\r[WLAN_SCENARIO_EXAMPLE] Connect to AP by WPS enrollee PBC mode\n");
	// Start WPS-PBC enrollee.
	// As the process beginning, please push the WPS button on AP.
	// It will take at most 2 min to do the procedure.
	char *argv[2];
	argv[1] = "pbc";
	cmd_wps(2, &argv);
	
	// If not connected, retry one time.
	if(wifi_is_connected_to_ap() != RTW_SUCCESS){
		printf("\n\r[WLAN_SCENARIO_EXAMPLE] WPS enrollee failed, reconnect one time\n");
		cmd_wps(2, &argv);
	}
	
	
	/**********************************************************************************
	*	3. Disconnect from AP			
	**********************************************************************************/
	printf("\n\r[WLAN_SCENARIO_EXAMPLE] Disconnect from AP\n");
	if(wifi_disconnect() < 0){
		printf("\n\r[WLAN_SCENARIO_EXAMPLE] ERROR: wifi_disconnect() failed\n");
		return;
	}
	
	// Show Wi-Fi info.
	rtw_wifi_setting_t setting;
	wifi_get_setting(WLAN0_NAME,&setting);
	wifi_show_setting(WLAN0_NAME,&setting);	

	
	/**********************************************************************************
	*	4. Enable Wi-Fi Direct GO			
	**********************************************************************************/
	printf("\n\r[WLAN_SCENARIO_EXAMPLE] Enable Wi-Fi Direct GO\n");
	// Start Wi-Fi Direct mode.
	// cmd_wifi_p2p_start() will re-enable the Wi-Fi with P2P mode and initialize P2P data.
	cmd_wifi_p2p_start(NULL, NULL);
	// Start Wi-Fi Direct Group Owner mode.
	// The GO related parameters can be set in cmd_wifi_p2p_auto_go_start() function declaration.
	if(cmd_wifi_p2p_auto_go_start(NULL, NULL) < 0){
		printf("\n\r[WLAN_SCENARIO_EXAMPLE] ERROR: cmd_wifi_p2p_auto_go_start() failed\n");
		return;
	}
	printf("\n\r[WLAN_SCENARIO_EXAMPLE] Wi-Fi Direct Group Owner mode enabled\n");
	
	// Show the Wi-Fi Direct Info.
	cmd_p2p_info();

	
	/**********************************************************************************
	*	5. Disable Wi-Fi Direct GO, and enable Wi-Fi with STA mode			
	**********************************************************************************/
	printf("\n\r[WLAN_SCENARIO_EXAMPLE] Disable Wi-Fi Direct GO, and enable Wi-Fi\n");
	// Disable Wi-Fi Direct GO.
	// This command has to be invoked to release the P2P resource.
	cmd_wifi_p2p_stop(NULL, NULL);
	// Enable Wi-Fi on STA mode.
	if(wifi_on(RTW_MODE_STA) < 0){
		printf("\n\r[WLAN_SCENARIO_EXAMPLE] ERROR: wifi_on() failed\n");
		return;
	}
	
	
	/**********************************************************************************
	*	6. Connect to AP use STA mode			
	**********************************************************************************/
	printf("\n\r[WLAN_SCENARIO_EXAMPLE] Connect to AP use STA mode\n");
	
	// Set the auto-reconnect mode with retry 1 time(limit is 2) and timeout 5 seconds.
	// This command need to be set before invoke wifi_connect() to make reconnection work.
	wifi_config_autoreconnect(1, 2, 5);
	
	// Connect to AP with Open mode.
	ssid = "Test_ap";
	if(wifi_connect(ssid, RTW_SECURITY_OPEN, NULL, strlen(ssid), 0, -1, NULL) == RTW_SUCCESS)
		LwIP_DHCP(0, DHCP_START);
	
	// Show Wi-Fi info.
	wifi_get_setting(WLAN0_NAME,&setting);
	wifi_show_setting(WLAN0_NAME,&setting);
	
	
	/**********************************************************************************
	*	7. Disconnect from AP			
	**********************************************************************************/
	printf("\n\r[WLAN_SCENARIO_EXAMPLE] Disconnect from AP\n");
	if(wifi_disconnect() < 0){
		printf("\n\r[WLAN_SCENARIO_EXAMPLE] ERROR: wifi_disconnect() failed\n");
		return;
	}
	
	// Show Wi-Fi info.
	wifi_get_setting(WLAN0_NAME,&setting);
	wifi_show_setting(WLAN0_NAME,&setting);	
	
	
	/**********************************************************************************
	*	8. Disable Wi-Fi			
	**********************************************************************************/	
	printf("\n\r[WLAN_SCENARIO_EXAMPLE] Disable Wi-Fi\n");
	if(wifi_off() != RTW_SUCCESS){
		printf("\n\r[WLAN_SCENARIO_EXAMPLE] ERROR: wifi_off() failed\n");
		return;
	}
}

// For processing the scanned result -> just output them.
// Can refer to fATWS() in atcmd_wifi.c.
static rtw_result_t scan_result_handler(rtw_scan_handler_result_t* malloced_scan_result){
	static int ApNum = 0;
	if (malloced_scan_result->scan_complete != RTW_TRUE) {
		rtw_scan_result_t* record = &malloced_scan_result->ap_details;
		record->SSID.val[record->SSID.len] = 0; /* Ensure the SSID is null terminated */

		printf("%d\t", ++ApNum);
		printf("%s\t ", ( record->bss_type == RTW_BSS_TYPE_ADHOC ) ? "Adhoc" : "Infra" );
    		printf("%02x:%02x:%02x:%02x:%02x:%02x", MAC_ARG(record->BSSID.octet));
    		printf(" %d\t ", record->signal_strength);
    		printf(" %d\t  ", record->channel);
    		printf(" %d\t  ", record->wps_type);
    		printf("%s\t\t ", ( record->security == RTW_SECURITY_OPEN ) ? "Open" :
                                 ( record->security == RTW_SECURITY_WEP_PSK ) ? "WEP" :
                                 ( record->security == RTW_SECURITY_WPA_TKIP_PSK ) ? "WPA TKIP" :
                                 ( record->security == RTW_SECURITY_WPA_AES_PSK ) ? "WPA AES" :
                                 ( record->security == RTW_SECURITY_WPA2_AES_PSK ) ? "WPA2 AES" :
                                 ( record->security == RTW_SECURITY_WPA2_TKIP_PSK ) ? "WPA2 TKIP" :
                                 ( record->security == RTW_SECURITY_WPA2_MIXED_PSK ) ? "WPA2 Mixed" :
                                 ( record->security == RTW_SECURITY_WPA_WPA2_MIXED ) ? "WPA/WPA2 AES" :
                                 "Unknown");
    		printf( " %s ", record->SSID.val);
    		printf("\r\n");
	}
	return RTW_SUCCESS;
}

static void example_wlan_scenario_thread(void* in_id){
	char *id = in_id;
	
	// Wait for other task stable.
	vTaskDelay(4000);
	
	if(strcmp(id, "S") == 0)
		// Scan network.
		scan_network();
	else if(strcmp(id, "A") == 0)
		// Authentication example.
		authentication();
	else if(strcmp(id, "M1") == 0)
		// Mode switch case 1.
		mode_switch_1();
	else if(strcmp(id, "M2") == 0)
		// Mode switch case 2.
		mode_switch_2();
	else if(strcmp(id, "M3") == 0)
		// Mode switch case 3.
		mode_switch_3();
	else if(strcmp(id, "M4") == 0)
		// Mode switch case 4.
		mode_switch_4();
	else if(strcmp(id, "M5") == 0)
		// Mode switch case 5.
		mode_switch_5();
	else if(strcmp(id, "M6") == 0)
		// Mode switch case 6.
		mode_switch_6();
	else if(strcmp(id, "M7") == 0)
		// Mode switch case 7.
		mode_switch_7();
	else if(strcmp(id, "S1") == 0)
		// Scenario case 1.
		scenario_1();
	else if(strcmp(id, "S2") == 0)
		// Scenario case 2.
		scenario_2();
	else if(strcmp(id, "S3") == 0)
		// Scenario case 3.
		scenario_3();
	else if(strcmp(id, "S4") == 0)
		// Scenario case 4.
		scenario_4();
	else
		printf("\n\r[WLAN_SCENARIO_EXAMPLE] ERROR: Invalid case identity\n");

	vTaskDelete(NULL);
}

void example_wlan_scenario(char* id){
	if(xTaskCreate(example_wlan_scenario_thread, ((const char*)"example_wlan_scenario_thread"), 1024,(void *const) id, tskIDLE_PRIORITY + 1, NULL) != pdPASS)
		printf("\n\r%s xTaskCreate failed\n", __FUNCTION__);
}

#endif /* CONFIG_EXAMPLE_WLAN_SCENARIO */