#if !defined(CONFIG_PLATFOMR_CUSTOMER_RTOS)
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "main.h"
#include <lwip_netconf.h>
#include "tcpip.h"
#include <wlan/wlan_test_inc.h>
#include <dhcp/dhcps.h>
#endif

#include <osdep_service.h>
#include <wifi/wifi_conf.h>
#include <wifi/wifi_util.h>
#include <platform/platform_stdlib.h>

#ifndef CONFIG_INTERACTIVE_EXT
#define CONFIG_INTERACTIVE_EXT  0
#endif
#ifndef CONFIG_SSL_CLIENT
#define CONFIG_SSL_CLIENT       1
#endif

#ifndef CONFIG_GOOGLENEST
#define CONFIG_GOOGLENEST          0
#endif
#if CONFIG_LWIP_LAYER
#ifndef CONFIG_WEBSERVER
#define CONFIG_WEBSERVER        0
#endif
#endif
#ifndef CONFIG_OTA_UPDATE
#define CONFIG_OTA_UPDATE       0
#endif
#ifndef CONFIG_BSD_TCP
#define CONFIG_BSD_TCP          0
#endif
#define CONFIG_JD_SMART			0
#ifndef CONFIG_ENABLE_P2P
#define CONFIG_ENABLE_P2P       0
#endif
#define SCAN_WITH_SSID		0	

#ifdef CONFIG_WPS
#define STACKSIZE               1280
#else
#define STACKSIZE               1024
#endif

#ifndef WLAN0_NAME
  #define WLAN0_NAME		"wlan0"
#endif
#ifndef WLAN1_NAME
  #define WLAN1_NAME		"wlan1"
#endif
/* Give default value if not defined */
#ifndef NET_IF_NUM
#ifdef CONFIG_CONCURRENT_MODE
#define NET_IF_NUM 2
#else
#define NET_IF_NUM 1
#endif
#endif

/*Static IP ADDRESS*/
#ifndef IP_ADDR0
#define IP_ADDR0   192
#define IP_ADDR1   168
#define IP_ADDR2   1
#define IP_ADDR3   80
#endif

/*NETMASK*/
#ifndef NETMASK_ADDR0
#define NETMASK_ADDR0   255
#define NETMASK_ADDR1   255
#define NETMASK_ADDR2   255
#define NETMASK_ADDR3   0
#endif

/*Gateway Address*/
#ifndef GW_ADDR0
#define GW_ADDR0   192
#define GW_ADDR1   168
#define GW_ADDR2   1
#define GW_ADDR3   1
#endif

/*Static IP ADDRESS*/
#ifndef AP_IP_ADDR0
#define AP_IP_ADDR0   192
#define AP_IP_ADDR1   168
#define AP_IP_ADDR2   43
#define AP_IP_ADDR3   1
#endif
   
/*NETMASK*/
#ifndef AP_NETMASK_ADDR0
#define AP_NETMASK_ADDR0   255
#define AP_NETMASK_ADDR1   255
#define AP_NETMASK_ADDR2   255
#define AP_NETMASK_ADDR3   0
#endif

/*Gateway Address*/
#ifndef AP_GW_ADDR0
#define AP_GW_ADDR0   192
#define AP_GW_ADDR1   168
#define AP_GW_ADDR2   43
#define AP_GW_ADDR3   1  
#endif

static void cmd_help(int argc, char **argv);
#if CONFIG_SSL_CLIENT
extern void cmd_ssl_client(int argc, char **argv);
#endif

#if CONFIG_GOOGLENEST
extern void cmd_googlenest(int argc, char **argv);
#endif
#if CONFIG_JD_SMART
extern void cmd_jd_smart(int argc, char **argv);
#endif

#if CONFIG_BSD_TCP
extern void cmd_tcp(int argc, char **argv);
extern void cmd_udp(int argc, char **argv);
#endif

#if CONFIG_WLAN
static void cmd_wifi_on(int argc, char **argv);
static void cmd_wifi_off(int argc, char **argv);
static void cmd_wifi_disconnect(int argc, char **argv);
extern void cmd_promisc(int argc, char **argv);
extern void cmd_simple_config(int argc, char **argv);

#if CONFIG_OTA_UPDATE
extern void cmd_update(int argc, char **argv);
#endif

#if CONFIG_WEBSERVER
extern void  start_web_server(void);
extern void  stop_web_server(void);
#endif
extern void cmd_app(int argc, char **argv);
#if CONFIG_ENABLE_WPS
extern void cmd_wps(int argc, char **argv);
#endif
#if defined(CONFIG_ENABLE_WPS_AP) && CONFIG_ENABLE_WPS_AP
extern void cmd_ap_wps(int argc, char **argv);
extern int wpas_wps_dev_config(u8 *dev_addr, u8 bregistrar);
#endif //CONFIG_ENABLE_WPS_AP
#if CONFIG_ENABLE_P2P
extern void cmd_wifi_p2p_start(int argc, char **argv);
extern void cmd_wifi_p2p_stop(int argc, char **argv);
extern void cmd_p2p_listen(int argc, char **argv);
extern void cmd_p2p_find(int argc, char **argv);
extern void cmd_p2p_peers(int argc, char **argv);
extern void cmd_p2p_info(int argc, char **argv);
extern void cmd_p2p_disconnect(int argc, char **argv);
extern void cmd_p2p_connect(int argc, char **argv);
#endif //CONFIG_ENABLE_P2P
#if defined(CONFIG_RTL8195A) || defined(CONFIG_RTL8711B) || defined(CONFIG_RTL8721D)
extern u32 CmdDumpWord(IN u16 argc, IN u8 *argv[]);
extern u32 CmdWriteWord(IN u16 argc, IN u8 *argv[]);
#endif
#if CONFIG_LWIP_LAYER
extern struct netif xnetif[NET_IF_NUM]; 
#endif
#ifdef  CONFIG_CONCURRENT_MODE
static void cmd_wifi_sta_and_ap(int argc, char **argv)
{
	int timeout = 20;//, mode;
#if CONFIG_LWIP_LAYER
#if !defined(CONFIG_PLATFOMR_CUSTOMER_RTOS)
	struct netif * pnetiff = (struct netif *)&xnetif[1];
#endif
#endif
	int channel;

	if((argc != 3) && (argc != 4)) {
		printf("\n\rUsage: wifi_ap SSID CHANNEL [PASSWORD]");
		return;
	}

	if(atoi((const char *)argv[2]) > 14){
		printf("\n\r bad channel!Usage: wifi_ap SSID CHANNEL [PASSWORD]");
		return;
	}
#if CONFIG_LWIP_LAYER
#if defined(CONFIG_PLATFOMR_CUSTOMER_RTOS)
	//TODO
#else
	dhcps_deinit();
#endif
#endif
        
#if 0
	//Check mode
	wext_get_mode(WLAN0_NAME, &mode);

	switch(mode) {
		case IW_MODE_MASTER:	//In AP mode
			cmd_wifi_off(0, NULL);
			cmd_wifi_on(0, NULL);
			break;
		case IW_MODE_INFRA:		//In STA mode
			if(wext_get_ssid(WLAN0_NAME, ssid) > 0)
				cmd_wifi_disconnect(0, NULL);
	}
#endif
	wifi_off();

#if defined(CONFIG_PLATFOMR_CUSTOMER_RTOS)
	//TODO
	//delay 20 Ticks
#else
	vTaskDelay(20);
#endif
	if (wifi_on(RTW_MODE_STA_AP) < 0){
		printf("\n\rERROR: Wifi on failed!");
		return;
	}

	printf("\n\rStarting AP ...");
	channel = atoi((const char *)argv[2]);
	if(channel > 13){
		printf("\n\rChannel is from 1 to 13. Set channel 1 as default!\n");
		channel = 1;
	}

	if(argc == 4) {
		if(wifi_start_ap(argv[1],
							 RTW_SECURITY_WPA2_AES_PSK,
							 argv[3],
							 strlen((const char *)argv[1]),
							 strlen((const char *)argv[3]),
							 channel
							 ) != RTW_SUCCESS) {
			printf("\n\rERROR: Operation failed!");
			return;
		}
	}
	else {
		if(wifi_start_ap(argv[1],
							 RTW_SECURITY_OPEN,
							 NULL,
							 strlen((const char *)argv[1]),
							 0,
							 channel
							 ) != RTW_SUCCESS) {
			printf("\n\rERROR: Operation failed!");
			return;
		}
	}

	while(1) {
		char essid[33];

		if(wext_get_ssid(WLAN1_NAME, (unsigned char *) essid) > 0) {
			if(strcmp((const char *) essid, (const char *)argv[1]) == 0) {
				printf("\n\r%s started", argv[1]);
				break;
			}
		}

		if(timeout == 0) {
			printf("\n\rERROR: Start AP timeout!");
			break;
		}

#if defined(CONFIG_PLATFOMR_CUSTOMER_RTOS)
	//TODO
	//Delay 1s
#else
		vTaskDelay(1 * configTICK_RATE_HZ);
#endif	
		timeout --;
	}
#if CONFIG_LWIP_LAYER
#if defined(CONFIG_PLATFOMR_CUSTOMER_RTOS)
	//TODO
#else
	LwIP_UseStaticIP(&xnetif[1]);
#ifdef CONFIG_DONT_CARE_TP
	pnetiff->flags |= NETIF_FLAG_IPSWITCH;
#endif
	dhcps_init(pnetiff);
#endif
#endif
}
#endif

static void cmd_wifi_ap(int argc, char **argv)
{
	int timeout = 20;
#if CONFIG_LWIP_LAYER 
#if !defined(CONFIG_PLATFOMR_CUSTOMER_RTOS)
	struct ip_addr ipaddr;
	struct ip_addr netmask;
	struct ip_addr gw;
	struct netif * pnetif = &xnetif[0];
#endif
#endif
	int channel;

#ifdef CONFIG_IEEE80211W
	if((argc != 3) && (argc != 4) && (argc != 5)) {
		printf("\n\rUsage: wifi_ap SSID CHANNEL [PASSWORD] [MFP_SUPPORT]");
		return;
	}
#else
	if((argc != 3) && (argc != 4)) {
		printf("\n\rUsage: wifi_ap SSID CHANNEL [PASSWORD]");
		return;
	}
#endif

#if CONFIG_LWIP_LAYER
#if defined(CONFIG_PLATFOMR_CUSTOMER_RTOS)
	//TODO
#else
	dhcps_deinit();
#if LWIP_VERSION_MAJOR >= 2
	IP4_ADDR(ip_2_ip4(&ipaddr), GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);
	IP4_ADDR(ip_2_ip4(&netmask), NETMASK_ADDR0, NETMASK_ADDR1 , NETMASK_ADDR2, NETMASK_ADDR3);
	IP4_ADDR(ip_2_ip4(&gw), GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);
	netif_set_addr(pnetif, ip_2_ip4(&ipaddr), ip_2_ip4(&netmask),ip_2_ip4(&gw));
#else
	IP4_ADDR(&ipaddr, GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);
	IP4_ADDR(&netmask, NETMASK_ADDR0, NETMASK_ADDR1 , NETMASK_ADDR2, NETMASK_ADDR3);
	IP4_ADDR(&gw, GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);
	netif_set_addr(pnetif, &ipaddr, &netmask,&gw);
#endif
#ifdef CONFIG_DONT_CARE_TP
	pnetif->flags |= NETIF_FLAG_IPSWITCH;
#endif
#endif
#endif
#if 0
	//Check mode
	wext_get_mode(WLAN0_NAME, &mode);

	switch(mode) {
		case IW_MODE_MASTER:	//In AP mode
			wifi_off();
			wifi_on(1);
			break;
		case IW_MODE_INFRA:		//In STA mode
			if(wext_get_ssid(WLAN0_NAME, ssid) > 0)
				cmd_wifi_disconnect(0, NULL);
	}
#else
	wifi_off();

#if defined(CONFIG_PLATFOMR_CUSTOMER_RTOS)
	//TODO
	//Delay 20 Ticks
#else
	vTaskDelay(20);
#endif

	if (wifi_on(RTW_MODE_AP) < 0){
		printf("\n\rERROR: Wifi on failed!");
		return;
	}
#endif

	printf("\n\rStarting AP ...");
	channel = atoi((const char *)argv[2]);
	if(channel > 13){
		printf("\n\rChannel is from 1 to 13. Set channel 1 as default!\n");
		channel = 1;
	}
#if defined(CONFIG_ENABLE_WPS_AP) && CONFIG_ENABLE_WPS_AP
	wpas_wps_dev_config(pnetif->hwaddr, 1);
#endif

	if(argc == 4) {
		if(wifi_start_ap(argv[1],
							 RTW_SECURITY_WPA2_AES_PSK,
							 argv[3],
							 strlen((const char *)argv[1]),
							 strlen((const char *)argv[3]),
							 channel
							 ) != RTW_SUCCESS) {
			printf("\n\rERROR: Operation failed!");
			return;
		}
	}
#ifdef CONFIG_IEEE80211W
	else if(argc == 5) {
		
		rtw_security_t alg = RTW_SECURITY_WPA2_AES_CMAC;
		u8 mfp = atoi((const char *)argv[4]);
		
		if (mfp == 0 || mfp > 2)  {// not support
			alg = RTW_SECURITY_WPA2_AES_PSK;
			mfp = 0;
		}
		wifi_set_mfp_support(mfp);
		if (wifi_start_ap(argv[1],
						 alg,
						 argv[3],
						 strlen((const char *)argv[1]),
						 strlen((const char *)argv[3]),
						 channel
						 ) != RTW_SUCCESS) {
			printf("\n\rERROR: Operation failed!");
			return;
		}
	}
#endif
	else {
		if(wifi_start_ap(argv[1],
							 RTW_SECURITY_OPEN,
							 NULL,
							 strlen((const char *)argv[1]),
							 0,
							 channel
							 ) != RTW_SUCCESS) {
			printf("\n\rERROR: Operation failed!");
			return;
		}
	}

	while(1) {
		char essid[33];

		if(wext_get_ssid(WLAN0_NAME, (unsigned char *) essid) > 0) {
			if(strcmp((const char *) essid, (const char *)argv[1]) == 0) {
				printf("\n\r%s started\n", argv[1]);
				break;
			}
		}

		if(timeout == 0) {
			printf("\n\rERROR: Start AP timeout!");
			break;
		}

#if defined(CONFIG_PLATFOMR_CUSTOMER_RTOS)
		//TODO
		//Delay 1 Tick
#else
		vTaskDelay(1 * configTICK_RATE_HZ);
#endif
		timeout --;
	}
#if CONFIG_LWIP_LAYER
#if defined(CONFIG_PLATFOMR_CUSTOMER_RTOS)
	//TODO
#else
	//LwIP_UseStaticIP(pnetif);
	dhcps_init(pnetif);
#endif
#endif
}

static void cmd_wifi_connect(int argc, char **argv)
{
	int ret = RTW_ERROR;
	unsigned long tick1 = xTaskGetTickCount();
	unsigned long tick2, tick3;
	int mode;
	char 				*ssid;
	rtw_security_t	security_type;
	char 				*password;
	int 				ssid_len;
	int 				password_len;
	int 				key_id;
	void				*semaphore;

	if((argc != 2) && (argc != 3) && (argc != 4)) {
		printf("\n\rUsage: wifi_connect SSID [WPA PASSWORD / (5 or 13) ASCII WEP KEY] [WEP KEY ID 0/1/2/3]");
		return;
	}
	
	//Check if in AP mode
	wext_get_mode(WLAN0_NAME, &mode);

	if(mode == IW_MODE_MASTER) {
#if CONFIG_LWIP_LAYER
#if defined(CONFIG_PLATFOMR_CUSTOMER_RTOS)
		//TODO
#else
                dhcps_deinit();
#endif
#endif
		wifi_off();
		vTaskDelay(20);
		if (wifi_on(RTW_MODE_STA) < 0){
			printf("\n\rERROR: Wifi on failed!");
			return;
		}
	}

	ssid = argv[1];
	if(argc == 2){
		security_type = RTW_SECURITY_OPEN;
		password = NULL;
		ssid_len = strlen((const char *)argv[1]);
		password_len = 0;
		key_id = 0;
		semaphore = NULL;
	}else if(argc ==3){
		security_type = RTW_SECURITY_WPA2_AES_PSK;
		password = argv[2];
		ssid_len = strlen((const char *)argv[1]);
		password_len = strlen((const char *)argv[2]);
		key_id = 0;
		semaphore = NULL;
	}else{
		security_type = RTW_SECURITY_WEP_PSK;
		password = argv[2];
		ssid_len = strlen((const char *)argv[1]);
		password_len = strlen((const char *)argv[2]);
		key_id = atoi(argv[3]);
		if(( password_len != 5) && (password_len != 13)) {
			printf("\n\rWrong WEP key length. Must be 5 or 13 ASCII characters.");
			return;
		}
		if((key_id < 0) || (key_id > 3)) {
			printf("\n\rWrong WEP key id. Must be one of 0,1,2, or 3.");
			return;
		}
		semaphore = NULL;
	}

	ret = wifi_connect(ssid, 
					security_type, 
					password, 
					ssid_len, 
					password_len, 
					key_id,
					semaphore);

	if(ret != RTW_SUCCESS) {
		printf("\n\rERROR: Operation failed!");
		return;
	} else {
		tick2 = xTaskGetTickCount();
		printf("\r\nConnected after %dms.\n", (tick2-tick1));
	
#if CONFIG_LWIP_LAYER
#if defined(CONFIG_PLATFOMR_CUSTOMER_RTOS)
		//TODO
#else
		/* Start DHCPClient */
		LwIP_DHCP(0, DHCP_START);
#endif
#endif
	}
	tick3 = xTaskGetTickCount();
	printf("\r\n\nGot IP after %dms.\n", (tick3-tick1));
}

static void cmd_wifi_connect_bssid(int argc, char **argv)
{
	int ret = RTW_ERROR;
	unsigned long tick1 = xTaskGetTickCount();
	unsigned long tick2, tick3;
	int mode;
	unsigned char 	bssid[ETH_ALEN];
	char 			*ssid = NULL;
	rtw_security_t		security_type;
	char 			*password;
	int 				bssid_len;
	int 				ssid_len = 0;
	int 				password_len;
	int 				key_id;
	void				*semaphore;
	u32 				mac[ETH_ALEN];
	u32				i;
	u32				index = 0;
	
	if((argc != 3) && (argc != 4) && (argc != 5) && (argc != 6)) {
		printf("\n\rUsage: wifi_connect_bssid 0/1 [SSID] BSSID / xx:xx:xx:xx:xx:xx [WPA PASSWORD / (5 or 13) ASCII WEP KEY] [WEP KEY ID 0/1/2/3]");
		return;
	}
	
	//Check if in AP mode
	wext_get_mode(WLAN0_NAME, &mode);

	if(mode == IW_MODE_MASTER) {
#if CONFIG_LWIP_LAYER
#if defined(CONFIG_PLATFOMR_CUSTOMER_RTOS)
		//TODO
#else

        dhcps_deinit();
#endif
#endif
		wifi_off();
		vTaskDelay(20);
		if (wifi_on(RTW_MODE_STA) < 0){
			printf("\n\rERROR: Wifi on failed!");
			return;
		}
	}
	//check ssid
	if(memcmp(argv[1], "0", 1)){
		index = 1;
		ssid_len = strlen((const char *)argv[2]);
		if((ssid_len <= 0) || (ssid_len > 32)) {
			printf("\n\rWrong ssid. Length must be less than 32.");
			return;
		}
		ssid = argv[2];	
	}
	sscanf(argv[2 + index], MAC_FMT, mac, mac + 1, mac + 2, mac + 3, mac + 4, mac + 5);
	for(i=0; i<ETH_ALEN; i++)
		bssid[i] = (u8)mac[i]&0xFF;
	
	if(argc == 3 + (int)index){
		security_type = RTW_SECURITY_OPEN;
		password = NULL;
		bssid_len = ETH_ALEN;
		password_len = 0;
		key_id = 0;
		semaphore = NULL;
	}else if(argc ==4 + (int)index){
		security_type = RTW_SECURITY_WPA2_AES_PSK;
		password = argv[3 + index];
		bssid_len = ETH_ALEN;
		password_len = strlen((const char *)argv[3 + index]);
		key_id = 0;
		semaphore = NULL;
	}else{
		security_type = RTW_SECURITY_WEP_PSK;
		password = argv[3 + index];
		bssid_len = ETH_ALEN;
		password_len = strlen((const char *)argv[3 + index]);
		key_id = atoi(argv[4 + index]);
		if(( password_len != 5) && (password_len != 13)) {
			printf("\n\rWrong WEP key length. Must be 5 or 13 ASCII characters.");
			return;
		}
		if((key_id < 0) || (key_id > 3)) {
			printf("\n\rWrong WEP key id. Must be one of 0,1,2, or 3.");
			return;
		}
		semaphore = NULL;
	}

	ret = wifi_connect_bssid(bssid, 
					ssid,
					security_type, 
					password, 
					bssid_len, 
					ssid_len, 					
					password_len, 
					key_id,
					semaphore);

	if(ret != RTW_SUCCESS) {
		printf("\n\rERROR: Operation failed!");
		return;
	} else {
		tick2 = xTaskGetTickCount();
		printf("\r\nConnected after %dms.\n", (tick2-tick1));
		
#if CONFIG_LWIP_LAYER
#if defined(CONFIG_PLATFOMR_CUSTOMER_RTOS)
		//TODO
#else

		/* Start DHCPClient */
		LwIP_DHCP(0, DHCP_START);
#endif
#endif
	}
	tick3 = xTaskGetTickCount();
	printf("\r\n\nGot IP after %dms.\n", (tick3-tick1));
}

static void cmd_wifi_disconnect(int argc, char **argv)
{
	/* To avoid gcc warnings */
	( void ) argc;
	( void ) argv;
	
	int timeout = 20;
	char essid[33];

	printf("\n\rDeassociating AP ...");

	if(wext_get_ssid(WLAN0_NAME, (unsigned char *) essid) < 0) {
		printf("\n\rWIFI disconnected");
		return;
	}

	if(wifi_disconnect() < 0) {
		printf("\n\rERROR: Operation failed!");
		return;
	}

	while(1) {
		if(wext_get_ssid(WLAN0_NAME, (unsigned char *) essid) < 0) {
			printf("\n\rWIFI disconnected");
			break;
		}

		if(timeout == 0) {
			printf("\n\rERROR: Deassoc timeout!");
			break;
		}

#if defined(CONFIG_PLATFOMR_CUSTOMER_RTOS)
		//TODO
		//Delay 1 Tick
#else
		vTaskDelay(1 * configTICK_RATE_HZ);
#endif
		timeout --;
	}
}

static void cmd_wifi_info(int argc, char **argv)
{
	/* To avoid gcc warnings */
	( void ) argc;
	( void ) argv;
	
	int i = 0;
#if CONFIG_LWIP_LAYER
#if !defined(CONFIG_PLATFOMR_CUSTOMER_RTOS)
	u8 *mac = LwIP_GetMAC(&xnetif[0]);
	u8 *ip = LwIP_GetIP(&xnetif[0]);
	u8 *gw = LwIP_GetGW(&xnetif[0]);
#endif
#endif
	u8 *ifname[2] = {(u8*)WLAN0_NAME,(u8*)WLAN1_NAME};
#ifdef CONFIG_MEM_MONITOR
	extern int min_free_heap_size;
#endif
	
	rtw_wifi_setting_t setting;
	for(i=0;i<NET_IF_NUM;i++){
		if(rltk_wlan_running(i)){
#if CONFIG_LWIP_LAYER
#if defined(CONFIG_PLATFOMR_CUSTOMER_RTOS)
			//TODO
#else
			mac = LwIP_GetMAC(&xnetif[i]);
			ip = LwIP_GetIP(&xnetif[i]);
			gw = LwIP_GetGW(&xnetif[i]);
#endif			
#endif
			printf("\n\r\nWIFI %s Status: Running",  ifname[i]);
			printf("\n\r==============================");
			
			rltk_wlan_statistic(i);
			
			wifi_get_setting((const char*)ifname[i],&setting);
			wifi_show_setting((const char*)ifname[i],&setting);
#if CONFIG_LWIP_LAYER
#if defined(CONFIG_PLATFOMR_CUSTOMER_RTOS)
			//TODO
#else
			printf("\n\rInterface (%s)", ifname[i]);
			printf("\n\r==============================");
			printf("\n\r\tMAC => %02x:%02x:%02x:%02x:%02x:%02x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]) ;
			printf("\n\r\tIP  => %d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
			printf("\n\r\tGW  => %d.%d.%d.%d\n\r", gw[0], gw[1], gw[2], gw[3]);
#endif
#endif
			if(setting.mode == RTW_MODE_AP || i == 1)
			{
				int client_number;
				struct {
					int    count;
					rtw_mac_t mac_list[AP_STA_NUM];
				} client_info;

				client_info.count = AP_STA_NUM;
				wifi_get_associated_client_list(&client_info, sizeof(client_info));

				printf("\n\rAssociated Client List:");
				printf("\n\r==============================");

				if(client_info.count == 0)
					printf("\n\rClient Num: 0\n\r");
				else
				{
				      	printf("\n\rClient Num: %d", client_info.count);
				      	for( client_number=0; client_number < client_info.count; client_number++ )
				      	{
						printf("\n\rClient [%d]:", client_number);
						printf("\n\r\tMAC => "MAC_FMT"",
										MAC_ARG(client_info.mac_list[client_number].octet));
					}
					printf("\n\r");
				}
			}

			{
				int error = wifi_get_last_error();
				printf("\n\rLast Link Error");
				printf("\n\r==============================");
				switch(error)
				{
					case RTW_NO_ERROR:
						printf("\n\r\tNo Error");
						break;
					case RTW_NONE_NETWORK:
						printf("\n\r\tTarget AP Not Found");
						break;
					case RTW_CONNECT_FAIL:
						printf("\n\r\tAssociation Failed");
						break;
					case RTW_WRONG_PASSWORD:
						printf("\n\r\tWrong Password");
						break;
					case RTW_DHCP_FAIL:
						printf("\n\r\tDHCP Failed");
						break;
					default:
						printf("\n\r\tUnknown Error(%d)", error);
				}
				printf("\n\r");
			}
		}		
	}

#if defined(configUSE_TRACE_FACILITY) && (configUSE_TRACE_FACILITY == 1)
	{
		signed char pcWriteBuffer[1024];
		vTaskList((char*)pcWriteBuffer);
		printf("\n\rTask List: \n%s", pcWriteBuffer);
	}
#endif	
#ifdef CONFIG_MEM_MONITOR
	printf("\n\rMemory Usage");
	printf("\n\r==============================");
	printf("\r\nMin Free Heap Size:  %d", min_free_heap_size);
	printf("\r\nCur Free Heap Size:  %d\n", xPortGetFreeHeapSize());
#endif
}

static void cmd_wifi_on(int argc, char **argv)
{
	/* To avoid gcc warnings */
	( void ) argc;
	( void ) argv;
	
	if(wifi_on(RTW_MODE_STA)<0){
		printf("\n\rERROR: Wifi on failed!\n");
	}
}

static void cmd_wifi_off(int argc, char **argv)
{
	/* To avoid gcc warnings */
	( void ) argc;
	( void ) argv;
#if CONFIG_WEBSERVER
	stop_web_server();
#endif
#if CONFIG_ENABLE_P2P
	cmd_wifi_p2p_stop(0, NULL);
#else
	wifi_off();
#endif
}

static void print_scan_result( rtw_scan_result_t* record )
{
	RTW_API_INFO( "%s\t ", ( record->bss_type == RTW_BSS_TYPE_ADHOC ) ? "Adhoc" : "Infra" );
	RTW_API_INFO( MAC_FMT, MAC_ARG(record->BSSID.octet) );
	RTW_API_INFO( " %d\t ", record->signal_strength );
	RTW_API_INFO( " %d\t  ", record->channel );
	RTW_API_INFO( " %d\t  ", record->wps_type );
	RTW_API_INFO( "%s\t\t ", ( record->security == RTW_SECURITY_OPEN ) ? "Open" :
								 ( record->security == RTW_SECURITY_WEP_PSK ) ? "WEP" :
								 ( record->security == RTW_SECURITY_WPA_TKIP_PSK ) ? "WPA TKIP" :
								 ( record->security == RTW_SECURITY_WPA_AES_PSK ) ? "WPA AES" :
								 ( record->security == RTW_SECURITY_WPA_MIXED_PSK ) ? "WPA Mixed" :
								 ( record->security == RTW_SECURITY_WPA2_AES_PSK ) ? "WPA2 AES" :
								 ( record->security == RTW_SECURITY_WPA2_TKIP_PSK ) ? "WPA2 TKIP" :
								 ( record->security == RTW_SECURITY_WPA2_MIXED_PSK ) ? "WPA2 Mixed" :
								 ( record->security == RTW_SECURITY_WPA_WPA2_TKIP_PSK) ? "WPA/WPA2 TKIP" :
								 ( record->security == RTW_SECURITY_WPA_WPA2_AES_PSK) ? "WPA/WPA2 AES" :
								 ( record->security == RTW_SECURITY_WPA_WPA2_MIXED_PSK) ? "WPA/WPA2 Mixed" :
#ifdef CONFIG_SAE_SUPPORT
								 ( record->security == RTW_SECURITY_WPA3_AES_PSK) ? "WP3-SAE AES" :
#endif
								 "Unknown" );

	RTW_API_INFO( " %s ", record->SSID.val );
	RTW_API_INFO( "\r\n" );
}

static rtw_result_t app_scan_result_handler( rtw_scan_handler_result_t* malloced_scan_result )
{
	static int ApNum = 0;

	if (malloced_scan_result->scan_complete != RTW_TRUE) {
		rtw_scan_result_t* record = &malloced_scan_result->ap_details;
		record->SSID.val[record->SSID.len] = 0; /* Ensure the SSID is null terminated */

		RTW_API_INFO( "%d\t ", ++ApNum );

		print_scan_result(record);
	} else{
		ApNum = 0;
	}
	return RTW_SUCCESS;
}
#if SCAN_WITH_SSID
static void cmd_wifi_scan_with_ssid(int argc, char **argv)
{

	u8 *channel_list = NULL;
	u8 *pscan_config = NULL;
	
	char *ssid = NULL;
	int ssid_len = 0;
	//Fully scan
	int scan_buf_len = 500;	
	if(argc == 3 && argv[1] && argv[2]){
		ssid = argv[1];		
		ssid_len = strlen((const char *)argv[1]);
		if((ssid_len <= 0) || (ssid_len > 32)) {
			printf("\n\rWrong ssid. Length must be less than 32.");
			goto exit;
		}
		scan_buf_len = atoi(argv[2]);
		if(scan_buf_len < 36){
			printf("\n\rBUFFER_LENGTH too short\n\r");
			goto exit;
		}
	}else if(argc > 3){
		int i = 0;
		int num_channel = atoi(argv[2]);
		ssid = argv[1];		
		ssid_len = strlen((const char *)argv[1]);
		if((ssid_len <= 0) || (ssid_len > 32)) {
			printf("\n\rWrong ssid. Length must be less than 32.");
			goto exit;
		}		
		channel_list = (u8*)pvPortMalloc(num_channel);
		if(!channel_list){
			printf("\n\r ERROR: Can't malloc memory for channel list");
			goto exit;
		}
		pscan_config = (u8*)pvPortMalloc(num_channel);
		if(!pscan_config){
			printf("\n\r ERROR: Can't malloc memory for pscan_config");
			goto exit;
		}		
		//parse command channel list
		for(i = 3; i <= argc -1 ; i++){
			*(channel_list + i - 3) = (u8)atoi(argv[i]);	
			*(pscan_config + i - 3) = PSCAN_ENABLE;			
		}
		if(wifi_set_pscan_chan(channel_list, pscan_config, num_channel) < 0){
		    printf("\n\rERROR: wifi set partial scan channel fail");
		    goto exit;
		}
	}else{
	    printf("\n\r For Scan all channel Usage: wifi_scan_with_ssid ssid BUFFER_LENGTH");
	    printf("\n\r For Scan partial channel Usage: wifi_scan_with_ssid ssid num_channels channel_num1 ...");
	    return;
	}
	
	if(wifi_scan_networks_with_ssid(NULL, NULL, scan_buf_len, ssid, ssid_len) != RTW_SUCCESS){
		printf("\n\rERROR: wifi scan failed");
		goto exit;
	}

exit:
	if(argc > 2 && channel_list)
		vPortFree(channel_list);
	if(argc > 2 && pscan_config)
		vPortFree(pscan_config);
	
}
#endif
static void cmd_wifi_scan(int argc, char **argv)
{

	u8 *channel_list = NULL;
	u8 *pscan_config = NULL;

	if(argc > 2){
		int i = 0;
		int num_channel = atoi(argv[1]);

		channel_list = (u8*)pvPortMalloc(num_channel);
		if(!channel_list){
			printf("\n\r ERROR: Can't malloc memory for channel list");
			goto exit;
		}
		pscan_config = (u8*)pvPortMalloc(num_channel);
		if(!pscan_config){
			printf("\n\r ERROR: Can't malloc memory for pscan_config");
			goto exit;
		}
		//parse command channel list
		for(i = 2; i <= argc -1 ; i++){
			*(channel_list + i - 2) = (u8)atoi(argv[i]);		    
			*(pscan_config + i - 2) = PSCAN_ENABLE;
		}
		
		if(wifi_set_pscan_chan(channel_list, pscan_config, num_channel) < 0){
		    printf("\n\rERROR: wifi set partial scan channel fail");
		    goto exit;
		}
		
	}

	if(wifi_scan_networks(app_scan_result_handler, NULL ) != RTW_SUCCESS){
		printf("\n\rERROR: wifi scan failed");
		goto exit;
	}
exit:
	if(argc > 2 && channel_list)
		vPortFree(channel_list);
	if(argc > 2 && pscan_config)
		vPortFree(pscan_config);

}

#if CONFIG_WEBSERVER
static void cmd_wifi_start_webserver(int argc, char **argv)
{
	start_web_server();
}
#endif

static void cmd_wifi_iwpriv(int argc, char **argv)
{
	if(argc == 2 && argv[1]) {
		wext_private_command(WLAN0_NAME, argv[1], 1);
	}
	else {
		printf("\n\rUsage: iwpriv COMMAND PARAMETERS");
	}
}
#endif	//#if CONFIG_WLAN

static void cmd_ping(int argc, char **argv)
{
#if !defined(CONFIG_PLATFOMR_CUSTOMER_RTOS)
	if(argc == 2) {
		do_ping_call(argv[1], 0, 5);	//Not loop, count=5
	}
	else if(argc == 3) {
		if(strcmp(argv[2], "loop") == 0)
			do_ping_call(argv[1], 1, 0);	//loop, no count
		else
			do_ping_call(argv[1], 0, atoi(argv[2]));	//Not loop, with count
	}
	else {
		printf("\n\rUsage: ping IP [COUNT/loop]");
	}
#else
	printf("\n\r unsupported cmd for customer platform!!!!!!!!");
#endif	
}
#if ( configGENERATE_RUN_TIME_STATS == 1 )
static char cBuffer[ 512 ];
static void cmd_cpustat(int argc, char **argv)
{
	vTaskGetRunTimeStats( ( char * ) cBuffer );
	printf( cBuffer );
}
#endif
#if defined(CONFIG_RTL8195A) || defined(CONFIG_RTL8711B) || defined(CONFIG_RTL8721D)
static void cmd_dump_reg(int argc, char **argv)
{
	CmdDumpWord(argc-1, (u8**)(argv+1));
}
static void cmd_edit_reg(int argc, char **argv)
{
	CmdWriteWord(argc-1, (u8**)(argv+1));
}
#endif
static void cmd_exit(int argc, char **argv)
{
	/* To avoid gcc warnings */
	( void ) argc;
	( void ) argv;
	
	printf("\n\rLeave INTERACTIVE MODE");
	vTaskDelete(NULL);
}

static void cmd_debug(int argc, char **argv)
{
	if(strcmp(argv[1], "ready_trx") == 0) {
		printf("\r\n%d", wifi_is_ready_to_transceive((rtw_interface_t)rtw_atoi((u8*)argv[2])));
	} else if(strcmp(argv[1], "is_up") == 0) {
		printf("\r\n%d", wifi_is_up((rtw_interface_t)rtw_atoi((u8*)argv[2])));
	} else if(strcmp(argv[1], "set_mac") == 0) {
		printf("\r\n%d", wifi_set_mac_address(argv[2]));
	} else if(strcmp(argv[1], "get_mac") == 0) {
		u8 mac[18] = {0};
		wifi_get_mac_address((char*)mac);
		printf("\r\n%s", mac);
	} else if(strcmp(argv[1], "ps_on") == 0) {
		printf("\r\n%d", wifi_enable_powersave());
	} else if(strcmp(argv[1], "ps_off") == 0) {
		printf("\r\n%d", wifi_disable_powersave());
#if 0 //TODO		
	} else if(strcmp(argv[1], "get_txpwr") == 0) {
		int idx;
		wifi_get_txpower(&idx);
		printf("\r\n%d", idx);
	} else if(strcmp(argv[1], "set_txpwr") == 0) {
		printf("\r\n%d", wifi_set_txpower(rtw_atoi((u8*)argv[2])));
#endif		
	} else if(strcmp(argv[1], "get_clientlist") == 0) {
		int client_number;
		struct {
			int    count;
			rtw_mac_t mac_list[3];
		} client_info;

		client_info.count = 3;

		printf("\r\n%d\r\n", wifi_get_associated_client_list(&client_info, sizeof(client_info)));

        if( client_info.count == 0 )
        {
            RTW_API_INFO(("Clients connected 0..\r\n"));
        }
        else
        {
            RTW_API_INFO("Clients connected %d..\r\n", client_info.count);
            for( client_number=0; client_number < client_info.count; client_number++ )
            {
				RTW_API_INFO(("------------------------------------\r\n"));
				RTW_API_INFO("| %d | "MAC_FMT" |\r\n",
									client_number,
									MAC_ARG(client_info.mac_list[client_number].octet)
							);
            }
            RTW_API_INFO(("------------------------------------\r\n"));
        }
	} else if(strcmp(argv[1], "get_apinfo") == 0) {
		rtw_bss_info_t ap_info;
		rtw_security_t sec;
		if(wifi_get_ap_info(&ap_info, &sec) == RTW_SUCCESS) {
			RTW_API_INFO( "\r\nSSID  : %s\r\n",(char*)ap_info.SSID );
			RTW_API_INFO("BSSID : "MAC_FMT"\r\n", MAC_ARG(ap_info.BSSID.octet));
			RTW_API_INFO("RSSI  : %d\r\n", ap_info.RSSI);
			//RTW_API_INFO( ("SNR   : %d\r\n", ap_info.SNR) );
			RTW_API_INFO("Beacon period : %d\r\n", ap_info.beacon_period);
			RTW_API_INFO("Security : %s\r\n", ( sec == RTW_SECURITY_OPEN )           ? "Open" :
													( sec == RTW_SECURITY_WEP_PSK )        ? "WEP" :
													( sec == RTW_SECURITY_WPA_TKIP_PSK )   ? "WPA TKIP" :
													( sec == RTW_SECURITY_WPA_AES_PSK )    ? "WPA AES" :
													( sec == RTW_SECURITY_WPA_MIXED_PSK )  ? "WPA Mixed" :
													( sec == RTW_SECURITY_WPA2_AES_PSK )   ? "WPA2 AES" :
													( sec == RTW_SECURITY_WPA2_TKIP_PSK )  ? "WPA2 TKIP" :
													( sec == RTW_SECURITY_WPA2_MIXED_PSK ) ? "WPA2 Mixed" :
													( sec == RTW_SECURITY_WPA_WPA2_TKIP_PSK) ? "WPA/WPA2 TKIP" :
													( sec == RTW_SECURITY_WPA_WPA2_AES_PSK) ? "WPA/WPA2 AES" :
													( sec == RTW_SECURITY_WPA_WPA2_MIXED_PSK) ? "WPA/WPA2 Mixed" :
													"Unknown" );
		}
	} else if(strcmp(argv[1], "reg_mc") == 0) {
		rtw_mac_t mac;
		sscanf(argv[2], MAC_FMT, (int*)(mac.octet+0), (int*)(mac.octet+1), (int*)(mac.octet+2), (int*)(mac.octet+3), (int*)(mac.octet+4), (int*)(mac.octet+5));
		printf("\r\n%d", wifi_register_multicast_address(&mac));
	} else if(strcmp(argv[1], "unreg_mc") == 0) {
		rtw_mac_t mac;
		sscanf(argv[2], MAC_FMT, (int*)(mac.octet+0), (int*)(mac.octet+1), (int*)(mac.octet+2), (int*)(mac.octet+3), (int*)(mac.octet+4), (int*)(mac.octet+5));
		printf("\r\n%d", wifi_unregister_multicast_address(&mac));
	} else if(strcmp(argv[1], "get_rssi") == 0) {
		int rssi = 0;
		wifi_get_rssi(&rssi);
		printf("\n\rwifi_get_rssi: rssi = %d", rssi);
	}else if(strcmp(argv[1], "dbg") == 0) {
		char buf[32] = {0};
		char * copy = buf;
        int i = 0;
       	int len = 0;
		for(i=1;i<argc;i++){
			strcpy(&buf[len], argv[i]);
			len = strlen(copy);
			buf[len++] = ' ';
			buf[len] = '\0';
		}	
		wext_private_command(WLAN0_NAME, copy, 1);
#ifdef CONFIG_IEEE80211W
	} else if(strcmp(argv[1], "11w_sa") == 0) {
		rltk_wlan_tx_sa_query(atoi((const char *)argv[2]));
	} else if(strcmp(argv[1], "11w_deauth") == 0) {
		rltk_wlan_tx_deauth(atoi((const char *)argv[2]), atoi((const char *)argv[3]));
	} else if(strcmp(argv[1], "11w_auth") == 0) {
		rltk_wlan_tx_auth();
#endif
	} else {
		printf("\r\nUnknown CMD\r\n");
	}
}

typedef struct _cmd_entry {
	char *command;
	void (*function)(int, char **);
} cmd_entry;

static const cmd_entry cmd_table[] = {
#if CONFIG_WLAN
	{"wifi_connect", cmd_wifi_connect},
	{"wifi_connect_bssid", cmd_wifi_connect_bssid},
	{"wifi_disconnect", cmd_wifi_disconnect},
	{"wifi_info", cmd_wifi_info},
	{"wifi_on", cmd_wifi_on},
	{"wifi_off", cmd_wifi_off},
	{"wifi_ap", cmd_wifi_ap},
	{"wifi_scan", cmd_wifi_scan},
#if SCAN_WITH_SSID	
	{"wifi_scan_with_ssid", cmd_wifi_scan_with_ssid},
#endif
	{"iwpriv", cmd_wifi_iwpriv},
	{"wifi_promisc", cmd_promisc},
#if CONFIG_OTA_UPDATE
	{"wifi_update", cmd_update},
#endif
#if CONFIG_WEBSERVER
	{"wifi_start_webserver", cmd_wifi_start_webserver}, 
#endif
#if (CONFIG_INCLUDE_SIMPLE_CONFIG)	
	{"wifi_simple_config", cmd_simple_config},
#endif	
#ifdef CONFIG_WPS
#if CONFIG_ENABLE_WPS
	{"wifi_wps", cmd_wps},
#endif	
#if defined(CONFIG_ENABLE_WPS_AP) && CONFIG_ENABLE_WPS_AP
	{"wifi_ap_wps", cmd_ap_wps},
#endif
#if CONFIG_ENABLE_P2P
	{"wifi_p2p_start", cmd_wifi_p2p_start},
	{"wifi_p2p_stop", cmd_wifi_p2p_stop},
	{"p2p_find", cmd_p2p_find},
	{"p2p_info", cmd_p2p_info},
	{"p2p_disconnect", cmd_p2p_disconnect},
	{"p2p_connect", cmd_p2p_connect},
#endif
#endif	
#ifdef CONFIG_CONCURRENT_MODE
	{"wifi_sta_ap",cmd_wifi_sta_and_ap},
#endif	

#if CONFIG_SSL_CLIENT
	{"ssl_client", cmd_ssl_client},
#endif
#if CONFIG_GOOGLENEST
	{"gn", cmd_googlenest},
#endif

#endif
#if CONFIG_LWIP_LAYER
//	{"app", cmd_app},
	{"wifi_debug", cmd_debug},
#if CONFIG_BSD_TCP
	{"tcp", cmd_tcp},
	{"udp", cmd_udp},
#endif
#if CONFIG_JD_SMART
	{"jd_smart", cmd_jd_smart},
#endif
	{"ping", cmd_ping},
#endif
#if ( configGENERATE_RUN_TIME_STATS == 1 )
	{"cpu", cmd_cpustat},
#endif
#if defined(CONFIG_RTL8195A) || defined(CONFIG_RTL8711B) || defined(CONFIG_RTL8721D)
	{"dw", cmd_dump_reg},
	{"ew", cmd_edit_reg},
#endif
	{"exit", cmd_exit},
	{"help", cmd_help}
};

#if CONFIG_INTERACTIVE_EXT
/* must include here, ext_cmd_table in wifi_interactive_ext.h uses struct cmd_entry */
#include <wifi_interactive_ext.h>
#endif

static void cmd_help(int argc, char **argv)
{
	/* To avoid gcc warnings */
	( void ) argc;
	( void ) argv;
	
	u32 i;

	printf("\n\rCOMMAND LIST:");
	printf("\n\r==============================");

	for(i = 0; i < sizeof(cmd_table) / sizeof(cmd_table[0]); i ++)
		printf("\n\r    %s", cmd_table[i].command);
#if CONFIG_INTERACTIVE_EXT
	for(i = 0; i < sizeof(ext_cmd_table) / sizeof(ext_cmd_table[0]); i ++)
		printf("\n\r    %s", ext_cmd_table[i].command);
#endif
}

#define MAX_ARGC	6

static int parse_cmd(char *buf, char **argv)
{
	int argc = 0;

	memset(argv, 0, sizeof(argv)*MAX_ARGC);
	while((argc < MAX_ARGC) && (*buf != '\0')) {
		argv[argc] = buf;
		argc ++;
		buf ++;

		while((*buf != ' ') && (*buf != '\0'))
			buf ++;

		while(*buf == ' ') {
			*buf = '\0';
			buf ++;
		}
		// Don't replace space
		if(argc == 1){
			if(strcmp(argv[0], "iwpriv") == 0){
				if(*buf != '\0'){
					argv[1] = buf;
					argc ++;
				}
				break;
			}
		}
	}

	return argc;
}

char uart_buf[64];

void interactive_mode(void *param)
{
	/* To avoid gcc warnings */
	( void ) param;
	
	int argc;
	unsigned int i;
	char *argv[MAX_ARGC];
#if defined(CONFIG_PLATFOMR_CUSTOMER_RTOS)
	//TODO
	//Get content from uart
#else
	extern xSemaphoreHandle	uart_rx_interrupt_sema;

	printf("\n\rEnter INTERACTIVE MODE\n\r");
	printf("\n\r# ");

	while(1){
		while(xSemaphoreTake(uart_rx_interrupt_sema, portMAX_DELAY) != pdTRUE);

		if((argc = parse_cmd(uart_buf, argv)) > 0) {
			int found = 0;

			for(i = 0; i < sizeof(cmd_table) / sizeof(cmd_table[0]); i ++) {
				if(strcmp((const char *)argv[0], (const char *)(cmd_table[i].command)) == 0) {
					cmd_table[i].function(argc, argv);
					found = 1;
					break;
				}
			}
#if CONFIG_INTERACTIVE_EXT
			if(!found) {
				for(i = 0; i < sizeof(ext_cmd_table) / sizeof(ext_cmd_table[0]); i ++) {
					if(strcmp(argv[0], ext_cmd_table[i].command) == 0) {
						ext_cmd_table[i].function(argc, argv);
						found = 1;
						break;
					}
				}
			}
#endif
			if(!found)
				printf("\n\runknown command '%s'", argv[0]);
			printf("\n\r[MEM] After do cmd, available heap %d\n\r", xPortGetFreeHeapSize());
		}

		printf("\r\n\n# ");
		uart_buf[0] = '\0';		
	}
#endif	
}

void start_interactive_mode(void)
{
#ifdef SERIAL_DEBUG_RX
	if(xTaskCreate(interactive_mode, (char const *)"interactive_mode", STACKSIZE, NULL, tskIDLE_PRIORITY + 4, NULL) != pdPASS)
		printf("\n\r%s xTaskCreate failed", __FUNCTION__);
#else
	printf("\n\rERROR: No SERIAL_DEBUG_RX to support interactive mode!");
#endif
}

#if defined(CONFIG_PLATFORM_8195A) || defined(CONFIG_PLATFORM_8711B) || defined(CONFIG_PLATFORM_8721D)
VOID WlanNormal( IN  u16 argc, IN  u8  *argv[])
{
	u8 i, j= 0;
	u8*	pbuf = (u8*)uart_buf;
	extern xSemaphoreHandle	uart_rx_interrupt_sema;
	
	memset(uart_buf, 0, sizeof(uart_buf));

	printf("argc=%d\n", argc);
	for(i = 0 ;  i < argc ; i++)
	{
		printf("command element [%d] = %s\n", i, argv[i]);
		for(j = 0; j<strlen((char const*)argv[i]) ;j++)
		{
			*pbuf = argv[i][j];
			pbuf++;
		}
		if(i < (argc-1))
		{
			*pbuf = ' ';
			pbuf++;
		}
	}
	printf("command %s\n", uart_buf);

	xSemaphoreGive( uart_rx_interrupt_sema);

}
#endif

