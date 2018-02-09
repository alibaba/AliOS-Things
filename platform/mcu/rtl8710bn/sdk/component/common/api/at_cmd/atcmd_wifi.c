//#include "FreeRTOS.h"
//#include "task.h"
//#include "semphr.h"
#include "log_service.h"
#include "atcmd_wifi.h"
#include <lwip_netconf.h>
#include "tcpip.h"
#include <dhcp/dhcps.h>
#if CONFIG_WLAN
//#include <wlan/wlan_test_inc.h>
#include <wifi/wifi_conf.h>
#include <wifi/wifi_util.h>
#endif

#if ATCMD_VER == ATVER_2
#include "flash_api.h"
#include "device_lock.h"
#include <wlan_fast_connect/example_wlan_fast_connect.h>
#endif

#if ATCMD_VER == ATVER_2 || WIFI_LOGO_CERTIFICATION_CONFIG
#include <lwip/sockets.h>
#endif

#include "hal/wifi.h"

/******************************************************************************/
#define	_AT_WLAN_SET_SSID_          "ATW0"
#define	_AT_WLAN_SET_PASSPHRASE_    "ATW1"
#define	_AT_WLAN_SET_KEY_ID_        "ATW2"
#define	_AT_WLAN_AP_SET_SSID_       "ATW3"
#define	_AT_WLAN_AP_SET_SEC_KEY_    "ATW4"
#define	_AT_WLAN_AP_SET_CHANNEL_    "ATW5"
#define _AT_WLAN_SET_BSSID_         "ATW6"
#define	_AT_WLAN_AP_ACTIVATE_       "ATWA"
#define _AT_WLAN_AP_STA_ACTIVATE_   "ATWB"
#define	_AT_WLAN_JOIN_NET_          "ATWC"
#define	_AT_WLAN_DISC_NET_          "ATWD"
#define	_AT_WLAN_WEB_SERVER_        "ATWE"
#define _AT_WLAN_P2P_FIND_          "ATWF"
#define _AT_WLAN_P2P_START_         "ATWG"
#define _AT_WLAN_P2P_STOP_          "ATWH"
#define _AT_WLAN_PING_TEST_         "ATWI"
#define _AT_WLAN_P2P_CONNECT_       "ATWJ"
#define _AT_WLAN_P2P_DISCONNECT_    "ATWK"
#define _AT_WLAN_SSL_CLIENT_        "ATWL"
#define _AT_WLAN_PROMISC_           "ATWM"
#define _AT_WLAN_P2P_INFO_          "ATWN"
#define _AT_WLAN_OTA_UPDATE_        "ATWO"
#define	_AT_WLAN_POWER_             "ATWP"
#define	_AT_WLAN_SIMPLE_CONFIG_     "ATWQ"
#define	_AT_WLAN_GET_RSSI_          "ATWR"
#define	_AT_WLAN_SCAN_              "ATWS"
#define _AT_WLAN_SCAN_WITH_SSID_    "ATWs"
#define _AT_WLAN_TCP_TEST_          "ATWT"
#define _AT_WLAN_UDP_TEST_          "ATWU"
#define _AT_WLAN_WPS_               "ATWW"
#define _AT_WLAN_AP_WPS_            "ATWw"
#define _AT_WLAN_AIRKISS_           "ATWX"
#define _AT_WLAN_IWPRIV_            "ATWZ"
#define	_AT_WLAN_INFO_              "ATW?"

#define	_AT_WLAN_EXTEND_POWER_MODE_        "ATXP"

#ifndef CONFIG_SSL_CLIENT
#define CONFIG_SSL_CLIENT       0
#endif
#ifndef CONFIG_WEBSERVER
#define CONFIG_WEBSERVER        0
#endif
#ifndef CONFIG_OTA_UPDATE
#define CONFIG_OTA_UPDATE       0
#endif
#ifndef CONFIG_BSD_TCP
#define CONFIG_BSD_TCP	        1
#endif
#ifndef CONFIG_ENABLE_P2P
#define CONFIG_ENABLE_P2P		0
#endif
#define SCAN_WITH_SSID		0
#if CONFIG_WEBSERVER
#define CONFIG_READ_FLASH	1
extern rtw_wifi_setting_t wifi_setting;
#endif

#ifndef CONFIG_WOWLAN_SERVICE
#define CONFIG_WOWLAN_SERVICE 0
#endif

#if CONFIG_LWIP_LAYER
extern void cmd_tcp(int argc, char **argv);
extern void cmd_udp(int argc, char **argv);
extern void cmd_ping(int argc, char **argv);
extern void cmd_ssl_client(int argc, char **argv);
#endif 

#if CONFIG_WLAN
extern void cmd_promisc(int argc, char **argv);
extern void cmd_update(int argc, char **argv);
extern void cmd_simple_config(int argc, char **argv);
#if CONFIG_ENABLE_WPS
extern void cmd_wps(int argc, char **argv);
#endif

#if defined(CONFIG_ENABLE_WPS_AP) && CONFIG_ENABLE_WPS_AP
extern void cmd_ap_wps(int argc, char **argv);
extern int wpas_wps_dev_config(u8 *dev_addr, u8 bregistrar);
#endif
#if CONFIG_ENABLE_P2P
extern void cmd_wifi_p2p_start(int argc, char **argv);
extern void cmd_wifi_p2p_stop(int argc, char **argv);
extern void cmd_p2p_listen(int argc, char **argv);
extern void cmd_p2p_find(int argc, char **argv);
extern void cmd_p2p_peers(int argc, char **argv);
extern void cmd_p2p_info(int argc, char **argv);
extern void cmd_p2p_disconnect(int argc, char **argv);
extern void cmd_p2p_connect(int argc, char **argv);
extern int cmd_wifi_p2p_auto_go_start(int argc, char **argv);
#endif //CONFIG_ENABLE_P2P
#if CONFIG_AIRKISS
extern int airkiss_start();
#endif
#if CONFIG_LWIP_LAYER
extern struct netif xnetif[NET_IF_NUM]; 
#endif
#if CONFIG_WOWLAN_SERVICE
extern void cmd_wowlan_service(int argc, char **argv);
#endif
#if CONFIG_INIC_CMD_RSP
extern void inic_c2h_wifi_info(const char *atcmd, char status);
extern void inic_c2h_msg(const char *atcmd, u8 status, char *msg, u16 msg_len);
#endif

/* fastconnect use wifi AT command. Not init_wifi_struct when log service disabled
 * static initialize all values for using fastconnect when log service disabled
 */
static rtw_network_info_t wifi = {
	{0},    // ssid
	{0},    // bssid
	0,      // security
	NULL,   // password
	0,      // password len
	-1      // key id
};

static rtw_ap_info_t ap = {0};
static unsigned char password[65] = {0};

#if ATCMD_VER == ATVER_2 || WIFI_LOGO_CERTIFICATION_CONFIG
unsigned char sta_ip[4] = {192,168,1,80}, sta_netmask[4] = {255,255,255,0}, sta_gw[4] = {192,168,1,1};
#endif

#if ATCMD_VER == ATVER_2
unsigned char dhcp_mode_sta = 1, dhcp_mode_ap = 1;
unsigned char  ap_ip[4] = {192,168,43,1},  ap_netmask[4] = {255,255,255,0},  ap_gw[4] = {192,168,43,1};
static void atcmd_wifi_disconn_hdl( char* buf, int buf_len, int flags, void* userdata);
#endif

rtw_mode_t wifi_mode = RTW_MODE_STA;

static void init_wifi_struct(void)
{
	memset(wifi.ssid.val, 0, sizeof(wifi.ssid.val));
	memset(wifi.bssid.octet, 0, ETH_ALEN);	
	memset(password, 0, sizeof(password));
	wifi.ssid.len = 0;
	wifi.password = NULL;
	wifi.password_len = 0;
	wifi.key_id = -1;
	memset(ap.ssid.val, 0, sizeof(ap.ssid.val));
	ap.ssid.len = 0;
	ap.password = NULL;
	ap.password_len = 0;
	ap.channel = 1;
}

static void print_scan_result( rtw_scan_result_t* record )
{
#if CONFIG_EXAMPLE_UART_ATCMD || CONFIG_EXAMPLE_SPI_ATCMD
    at_printf("%s,%d,%s,%d,"MAC_FMT"", record->SSID.val, record->channel,
    		( record->security == RTW_SECURITY_OPEN ) ? "Open" :
            ( record->security == RTW_SECURITY_WEP_PSK ) ? "WEP" :
            ( record->security == RTW_SECURITY_WPA_TKIP_PSK ) ? "WPA TKIP" :
            ( record->security == RTW_SECURITY_WPA_AES_PSK ) ? "WPA AES" :
            ( record->security == RTW_SECURITY_WPA2_AES_PSK ) ? "WPA2 AES" :
            ( record->security == RTW_SECURITY_WPA2_TKIP_PSK ) ? "WPA2 TKIP" :
            ( record->security == RTW_SECURITY_WPA2_MIXED_PSK ) ? "WPA2 Mixed" :
            ( record->security == RTW_SECURITY_WPA_WPA2_MIXED ) ? "WPA/WPA2 AES" : "Unknown",
            record->signal_strength, MAC_ARG(record->BSSID.octet)   );
#else
    RTW_API_INFO( ( "%s\t ", ( record->bss_type == RTW_BSS_TYPE_ADHOC ) ? "Adhoc" : "Infra" ) );
    RTW_API_INFO( ( MAC_FMT, MAC_ARG(record->BSSID.octet) ) );
    RTW_API_INFO( ( " %d\t ", record->signal_strength ) );
    RTW_API_INFO( ( " %d\t  ", record->channel ) );
    RTW_API_INFO( ( " %d\t  ", record->wps_type ) );
    RTW_API_INFO( ( "%s\t\t ", ( record->security == RTW_SECURITY_OPEN ) ? "Open" :
                                 ( record->security == RTW_SECURITY_WEP_PSK ) ? "WEP" :
                                 ( record->security == RTW_SECURITY_WPA_TKIP_PSK ) ? "WPA TKIP" :
                                 ( record->security == RTW_SECURITY_WPA_AES_PSK ) ? "WPA AES" :
                                 ( record->security == RTW_SECURITY_WPA2_AES_PSK ) ? "WPA2 AES" :
                                 ( record->security == RTW_SECURITY_WPA2_TKIP_PSK ) ? "WPA2 TKIP" :
                                 ( record->security == RTW_SECURITY_WPA2_MIXED_PSK ) ? "WPA2 Mixed" :
                                 ( record->security == RTW_SECURITY_WPA_WPA2_MIXED ) ? "WPA/WPA2 AES" :
                                 "Unknown" ) );

    RTW_API_INFO( ( " %s ", record->SSID.val ) );
    RTW_API_INFO( ( "\r\n" ) );
#endif
}

hal_wifi_scan_result_adv_t g_scan_ApAdvList;
ap_list_adv_t g_scan_ApAdv_info[20];
static void copy_scan_result( rtw_scan_result_t* record, int ap_index )
{
    if(ap_index<0 || ap_index > 19){
        printf("scan number full\r\n");
        return;
    }

    memcpy(g_scan_ApAdv_info[ap_index].ssid, record->SSID.val, record->SSID.len);
    memcpy(g_scan_ApAdv_info[ap_index].bssid, record->BSSID.octet, 6);
    g_scan_ApAdv_info[ap_index].ap_power = record->signal_strength;
    g_scan_ApAdv_info[ap_index].channel = record->channel;
    g_scan_ApAdv_info[ap_index].security = record->security;
    return;
}

static rtw_result_t app_scan_result_handler( rtw_scan_handler_result_t* malloced_scan_result )
{
	static int ApNum = 0;

	if (malloced_scan_result->scan_complete != RTW_TRUE) {
		rtw_scan_result_t* record = &malloced_scan_result->ap_details;
		record->SSID.val[record->SSID.len] = 0; /* Ensure the SSID is null terminated */

                copy_scan_result(record, ApNum);
#if CONFIG_EXAMPLE_UART_ATCMD || CONFIG_EXAMPLE_SPI_ATCMD
		at_printf("\r\nAP : %d,", ++ApNum);
#else
		RTW_API_INFO( ( "%d\t ", ++ApNum ) );
#endif
                
		print_scan_result(record);
#if CONFIG_INIC_CMD_RSP
		if(malloced_scan_result->user_data)
			memcpy((void *)((char *)malloced_scan_result->user_data+(ApNum-1)*sizeof(rtw_scan_result_t)), (char *)record, sizeof(rtw_scan_result_t));
#endif
	} else{

                g_scan_ApAdvList.ap_num = ApNum;
                g_scan_ApAdvList.ap_list = g_scan_ApAdv_info;
                ApListAdvCallback(&g_scan_ApAdvList);    
#if CONFIG_INIC_CMD_RSP
		inic_c2h_msg("ATWS", RTW_SUCCESS, (char *)malloced_scan_result->user_data, ApNum*sizeof(rtw_scan_result_t));
		if(malloced_scan_result->user_data)
			free(malloced_scan_result->user_data);
		inic_c2h_msg("ATWS", RTW_SUCCESS, NULL, 0);
#endif
#if CONFIG_EXAMPLE_UART_ATCMD || CONFIG_EXAMPLE_SPI_ATCMD
		at_printf("\r\n[ATWS] OK");
		at_printf(STR_END_OF_ATCMD_RET);
#endif
		ApNum = 0;
	}
	return RTW_SUCCESS;
}

void fATWD(void *arg){
	int timeout = 20;
	char essid[33];
	int ret = RTW_SUCCESS;
#if ATCMD_VER == ATVER_2
	int error_no = 0;
#endif

	printf("[ATWD]: _AT_WLAN_DISC_NET_\n\r");
	printf("\n\rDeassociating AP ...");

	if(wext_get_ssid(WLAN0_NAME, (unsigned char *) essid) < 0) {
		printf("\n\rWIFI disconnected");
		goto exit_success;
	}
#if ATCMD_VER == ATVER_2
	wifi_unreg_event_handler(WIFI_EVENT_DISCONNECT, atcmd_wifi_disconn_hdl);
#endif
	if((ret = wifi_disconnect()) < 0) {
		printf("[ATWD]ERROR: Operation failed!\n\r");
#if ATCMD_VER == ATVER_2
		error_no = 3;
#endif
		goto exit;
	}

	while(1) {
		if(wext_get_ssid(WLAN0_NAME, (unsigned char *) essid) < 0) {
			printf("\n\rWIFI disconnected");
			break;
		}

		if(timeout == 0) {
			printf("[ATWD]ERROR: Deassoc timeout!\n\r");
			ret = RTW_TIMEOUT;
#if ATCMD_VER == ATVER_2
			error_no = 4;
#endif
			break;
		}

		rtw_mdelay_os(1 * configTICK_RATE_HZ);
		timeout --;
	}
	printf("\n\r");

#if CONFIG_LWIP_LAYER
	LwIP_ReleaseIP(WLAN0_IDX);
#endif
exit:
#if CONFIG_INIC_CMD_RSP
	if(ret != RTW_SUCCESS)
		inic_c2h_msg("ATWD", ret, NULL, 0);
#endif
	init_wifi_struct( );
#if ATCMD_VER == ATVER_2
	if(error_no==0)
		at_printf("\r\n[ATWD] OK");
	else
		at_printf("\r\n[ATWD] ERROR:%d",error_no);
#endif
	return;
exit_success:
#if CONFIG_INIC_CMD_RSP
	inic_c2h_msg("ATWD", RTW_SUCCESS, NULL, 0);
#endif
	init_wifi_struct( );
#if ATCMD_VER == ATVER_2
	at_printf("\r\n[ATWD] OK");
#endif	
	return;
}

#if (CONFIG_INCLUDE_SIMPLE_CONFIG)
void fATWQ(void *arg){
	int argc=0;
	char *argv[2] = {0};
	printf("[ATWQ]: _AT_WLAN_SIMPLE_CONFIG_\n\r");
	argv[argc++] = "wifi_simple_config";
	if(arg){
		argv[argc++] = arg;
	}
	cmd_simple_config(argc, argv);
}
#endif

void fATWS(void *arg){
	char buf[32] = {0};
	u8 *channel_list = NULL;
	u8 *pscan_config = NULL;
	int num_channel = 0;
	int i, argc = 0;
	char *argv[MAX_ARGC] = {0};
	int ret = RTW_SUCCESS;
#if CONFIG_INIC_CMD_RSP
	u8 *inic_scan_buf = NULL;
#endif
#if ATCMD_VER == ATVER_2
	int error_no = 0;
#endif
	printf("[ATWS]: _AT_WLAN_SCAN_\n\r");
	if(arg){
		strcpy(buf, arg);
		argc = parse_param(buf, argv);
		if(argc < 2){
			ret = RTW_BADARG;
#if ATCMD_VER == ATVER_2
			error_no = 1;
#endif
			goto exit;
		}
		num_channel = atoi(argv[1]);
		channel_list = (u8*)malloc(num_channel);
		if(!channel_list){
			printf("[ATWS]ERROR: Can't malloc memory for channel list\n\r");
			ret = RTW_BUFFER_UNAVAILABLE_TEMPORARY;
#if ATCMD_VER == ATVER_2
			error_no = 2;
#endif
			goto exit;
		}
		pscan_config = (u8*)malloc(num_channel);
		if(!pscan_config){
			printf("[ATWS]ERROR: Can't malloc memory for pscan_config\n\r");
			ret = RTW_BUFFER_UNAVAILABLE_TEMPORARY;
#if ATCMD_VER == ATVER_2
			error_no = 3;
#endif
			goto exit;
		}
		//parse command channel list
		for(i = 2; i <= argc -1 ; i++){
			*(channel_list + i - 2) = (u8)atoi(argv[i]);
			*(pscan_config + i - 2) = PSCAN_ENABLE;
		}

		if((ret = wifi_set_pscan_chan(channel_list, pscan_config, num_channel)) < 0){
			printf("[ATWS]ERROR: wifi set partial scan channel fail\n\r");
#if ATCMD_VER == ATVER_2
			error_no = 4;
#endif
			goto exit;
		}
	}
#if CONFIG_INIC_CMD_RSP
	inic_scan_buf = malloc(65*sizeof(rtw_scan_result_t));
	if(inic_scan_buf == NULL){
		ret = RTW_BUFFER_UNAVAILABLE_TEMPORARY;
		goto exit;
	}
	memset(inic_scan_buf, 0, 65*sizeof(rtw_scan_result_t));
	if((ret = wifi_scan_networks(app_scan_result_handler, inic_scan_buf)) != RTW_SUCCESS){
		printf("[ATWS]ERROR: wifi scan failed\n\r");
		goto exit;
	}
#else

        memset(g_scan_ApAdv_info, 0, 20* sizeof(ap_list_adv_t));
        g_scan_ApAdvList.ap_list = g_scan_ApAdv_info;
        g_scan_ApAdvList.ap_num = 0;

	if((ret = wifi_scan_networks(app_scan_result_handler, NULL )) != RTW_SUCCESS){
		printf("[ATWS]ERROR: wifi scan failed\n\r");
#if ATCMD_VER == ATVER_2
		error_no = 5;
#endif
		goto exit;
	}
#endif
exit:
#if CONFIG_INIC_CMD_RSP
	if(ret != RTW_SUCCESS){
		if(inic_scan_buf)
			free(inic_scan_buf);
		inic_c2h_msg("ATWS", ret, NULL, 0);
	}
#endif
#if ATCMD_VER == ATVER_2
	if(error_no)
		at_printf("\r\n[ATWS] ERROR:%d",error_no);
#endif
	if(arg && channel_list)
		free(channel_list);
	if(arg && pscan_config)
		free(pscan_config);
}

void fATWx(void *arg){
	int i = 0;
#if CONFIG_LWIP_LAYER
	u8 *mac = LwIP_GetMAC(&xnetif[0]);
	u8 *ip = LwIP_GetIP(&xnetif[0]);
	u8 *gw = LwIP_GetGW(&xnetif[0]);
#endif
	u8 *ifname[2] = {WLAN0_NAME,WLAN1_NAME};
	rtw_wifi_setting_t setting;

	printf("[ATW?]: _AT_WLAN_INFO_\n\r");
#if CONFIG_INIC_CMD_RSP
	int ret = RTW_SUCCESS;
	int info_sz = 0;
	u8 *info = malloc(NET_IF_NUM*sizeof(rtw_wifi_setting_t)+3*sizeof(rtw_mac_t));
	if(info == NULL)
		ret = RTW_BUFFER_UNAVAILABLE_TEMPORARY;
#endif
	for(i=0;i<NET_IF_NUM;i++){
		if(rltk_wlan_running(i)){
#if CONFIG_LWIP_LAYER
			mac = LwIP_GetMAC(&xnetif[i]);
			ip = LwIP_GetIP(&xnetif[i]);
			gw = LwIP_GetGW(&xnetif[i]);
#endif
			printf("\n\r\nWIFI %s Status: Running",  ifname[i]);
			printf("\n\r==============================");

			rltk_wlan_statistic(i);

			wifi_get_setting((const char*)ifname[i],&setting);
			wifi_show_setting((const char*)ifname[i],&setting);
#if CONFIG_INIC_CMD_RSP
			if(info){
				memcpy(info+info_sz, (void *)&setting, sizeof(rtw_wifi_setting_t));
				info_sz += sizeof(rtw_wifi_setting_t);
			}
#endif

#if CONFIG_LWIP_LAYER
#if CONFIG_EXAMPLE_UART_ATCMD || CONFIG_EXAMPLE_SPI_ATCMD
			at_printf("%02x:%02x:%02x:%02x:%02x:%02x,", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]) ;
			at_printf("%d.%d.%d.%d,", ip[0], ip[1], ip[2], ip[3]);
			at_printf("%d.%d.%d.%d", gw[0], gw[1], gw[2], gw[3]);
#endif
			printf("\n\rInterface (%s)", ifname[i]);
			printf("\n\r==============================");
			printf("\n\r\tMAC => %02x:%02x:%02x:%02x:%02x:%02x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]) ;
			printf("\n\r\tIP  => %d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
			printf("\n\r\tGW  => %d.%d.%d.%d\n\r", gw[0], gw[1], gw[2], gw[3]);
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
					printf("\n\rClient Num: 0\n\r", client_info.count);
				else
				{
					printf("\n\rClient Num: %d", client_info.count);
					for( client_number=0; client_number < client_info.count; client_number++ )
					{
						printf("\n\rClient %d:", client_number + 1);
						printf("\n\r\tMAC => "MAC_FMT"",
										MAC_ARG(client_info.mac_list[client_number].octet));
#if CONFIG_EXAMPLE_UART_ATCMD || CONFIG_EXAMPLE_SPI_ATCMD
						at_printf("\r\nCLIENT : %d,"MAC_FMT"", client_number + 1, MAC_ARG(client_info.mac_list[client_number].octet));
#endif
#if CONFIG_INIC_CMD_RSP
						if(info){
							memcpy(info+info_sz, (void *)&client_info.mac_list[client_number], sizeof(rtw_mac_t));
							info_sz += sizeof(rtw_mac_t);
						}
#endif
					}
					printf("\n\r");
				}
			}
		}
// show the ethernet interface info
		else{
#if CONFIG_ETHERNET
			if(i == NET_IF_NUM - 1){
#if CONFIG_LWIP_LAYER
				mac = LwIP_GetMAC(&xnetif[i]);
				ip = LwIP_GetIP(&xnetif[i]);
				gw = LwIP_GetGW(&xnetif[i]);
				printf("\n\rInterface ethernet\n");
				printf("\n\r==============================");
				printf("\n\r\tMAC => %02x:%02x:%02x:%02x:%02x:%02x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]) ;
				printf("\n\r\tIP  => %d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
				printf("\n\r\tGW  => %d.%d.%d.%d\n\r", gw[0], gw[1], gw[2], gw[3]);
#endif // end CONFIG_LWIP_LAYER
			}
#endif // end CONFIG_ETHERNET
		}
	}

#if defined(configUSE_TRACE_FACILITY) && (configUSE_TRACE_FACILITY == 1) && (configUSE_STATS_FORMATTING_FUNCTIONS == 1)
	{
		signed char pcWriteBuffer[1024];
		vTaskList((char*)pcWriteBuffer);
		printf("\n\rTask List: \n\r%s", pcWriteBuffer);
	}
#endif

#if CONFIG_INIC_CMD_RSP
	if(ret != RTW_SUCCESS)
		inic_c2h_msg("ATW?", ret, NULL, 0);
	else
		inic_c2h_msg("ATW?", RTW_SUCCESS, (char *)info, info_sz);

	if(info)
		free(info);
	info = NULL;
#endif

#if CONFIG_EXAMPLE_UART_ATCMD || CONFIG_EXAMPLE_SPI_ATCMD
	at_printf("\r\n[ATW?] OK");
#endif

}

#if ATCMD_VER == ATVER_1
void fATW0(void *arg){
	int ret = RTW_SUCCESS;
	if(!arg){
		printf("[ATW0]Usage: ATW0=SSID(Maximum length is 32)\n\r");
		ret = RTW_BADARG;
		goto exit;
	}
	if(strlen((char*)arg) > 32){
		printf("[ATW0]Error: SSID length can't exceed 32\n\r");
		ret = RTW_BADARG;
		goto exit;
	}
	printf("[ATW0]: _AT_WLAN_SET_SSID_ [%s]\n\r", (char*)arg);
	strcpy((char *)wifi.ssid.val, (char*)arg);
	wifi.ssid.len = strlen((char*)arg);
exit:
#if CONFIG_INIC_CMD_RSP
	inic_c2h_msg("ATW0", ret, NULL, 0);
#endif
	return;
}

void fATW1(void *arg){
	int ret = RTW_SUCCESS;
        printf("[ATW1]: _AT_WLAN_SET_PASSPHRASE_ [%s]\n\r", (char*)arg); 
	strcpy((char *)password, (char*)arg);
	wifi.password = password;
	wifi.password_len = strlen((char*)arg);
exit:
#if CONFIG_INIC_CMD_RSP
	inic_c2h_msg("ATW1", ret, NULL, 0);
#endif
	return;	
}

void fATW2(void *arg){
	int ret = RTW_SUCCESS;
	printf("[ATW2]: _AT_WLAN_SET_KEY_ID_ [%s]\n\r", (char*)arg);
	if((strlen((const char *)arg) != 1 ) || (*(char*)arg <'0' ||*(char*)arg >'3')) {
		printf("[ATW2]Error: Wrong WEP key id. Must be one of 0,1,2, or 3.\n\r");
		ret = RTW_BADARG;
		goto exit;
	}
	wifi.key_id = atoi((const char *)(arg));
exit:
#if CONFIG_INIC_CMD_RSP
	inic_c2h_msg("ATW2", ret, NULL, 0);
#endif
	return;
}

void fATW3(void *arg){
	int ret = RTW_SUCCESS;
	if(!arg){
		printf("[ATW3]Usage: ATW3=SSID\n\r");
		ret = RTW_BADARG;
		goto exit;
	}
	if(strlen((char*)arg) > 32){
		printf("[ATW3]Error: SSID length can't exceed 32\n\r");
		ret = RTW_BADARG;
		goto exit;
	}
	strcpy((char *)ap.ssid.val, (char*)arg);
	ap.ssid.len = strlen((char*)arg);

	printf("[ATW3]: _AT_WLAN_AP_SET_SSID_ [%s]\n\r", ap.ssid.val); 
exit:
#if CONFIG_INIC_CMD_RSP
	inic_c2h_msg("ATW3", ret, NULL, 0);
#endif
	return;
}

void fATW4(void *arg){
	int ret = RTW_SUCCESS;

	if(!arg){
		printf("[ATW4]Usage: ATW4=PASSWORD\n\r");
		ret = RTW_BADARG;
		goto exit;
	}
	if(strlen((char*)arg) > 64){
		printf("[ATW4]Error: PASSWORD length can't exceed 64\n\r");
		ret = RTW_BADARG;
		goto exit;
	}

	strcpy((char *)password, (char*)arg);
	ap.password = password;
	ap.password_len = strlen((char*)arg);
	printf("[ATW4]: _AT_WLAN_AP_SET_SEC_KEY_ [%s]\n\r", ap.password);
exit:
#if CONFIG_INIC_CMD_RSP
	inic_c2h_msg("ATW4", ret, NULL, 0);
#endif
	return;
}

void fATW5(void *arg){
	int ret = RTW_SUCCESS;
	if(!arg){
		printf("[ATW5]Usage: ATW5=CHANNEL\n\r");
		ret = RTW_BADARG;
		goto exit;
	}
	ap.channel = (unsigned char) atoi((const char *)arg);
	printf("[ATW5]: _AT_WLAN_AP_SET_CHANNEL_ [channel %d]\n\r", ap.channel);
exit:
#if CONFIG_INIC_CMD_RSP
	inic_c2h_msg("ATW5", ret, NULL, 0);
#endif
	 return;
}

void fATW6(void *arg){
	u32		mac[ETH_ALEN];
	u32		i;
	int ret = RTW_SUCCESS;
	if(!arg){
		printf("[ATW6]Usage: ATW6=BSSID\n\r");
		ret =  RTW_BADARG;
		goto exit;
	}
	printf("[ATW6]: _AT_WLAN_SET_BSSID_ [%s]\n\r", (char*)arg);
	sscanf(arg, MAC_FMT, mac, mac + 1, mac + 2, mac + 3, mac + 4, mac + 5);
	for(i = 0; i < ETH_ALEN; i ++)
		wifi.bssid.octet[i] = (u8)mac[i] & 0xFF;
exit:
#if CONFIG_INIC_CMD_RSP
	inic_c2h_msg("ATW6", ret, NULL, 0);
#endif
	return;
}

void fATWA(void *arg){
#if CONFIG_LWIP_LAYER
	ip_addr_t ipaddr;
	ip_addr_t netmask;
	ip_addr_t gw;
	struct netif * pnetif = &xnetif[0];
#endif
	int timeout = 20;
	int ret = RTW_SUCCESS;
	printf("[ATWA]: _AT_WLAN_AP_ACTIVATE_\n\r"); 
	if(ap.ssid.val[0] == 0){
		printf("[ATWA]Error: SSID can't be empty\n\r");
		ret = RTW_BADARG;
		goto exit;
	}
	if(ap.password == NULL){
		ap.security_type = RTW_SECURITY_OPEN;
	}
	else{
		if(ap.password_len <= RTW_MAX_PSK_LEN &&
			ap.password_len >= RTW_MIN_PSK_LEN){ 
			ap.security_type = RTW_SECURITY_WPA2_AES_PSK;
		}
		else{
			printf("[ATWA]Error: password length is between 8 to 64 \n\r");
			ret = RTW_INVALID_KEY;
			goto exit;
		}
	}

#if CONFIG_WEBSERVER
	//store into flash
	memset(wifi_setting.ssid, 0, sizeof(wifi_setting.ssid));;
	memcpy(wifi_setting.ssid, ap.ssid.val, strlen((char*)ap.ssid.val));
	wifi_setting.ssid[ap.ssid.len] = '\0';
	wifi_setting.security_type = ap.security_type;
	if(ap.security_type !=0)
		wifi_setting.security_type = 1;
	else
		wifi_setting.security_type = 0;
	if (ap.password)
		memcpy(wifi_setting.password, ap.password, strlen((char*)ap.password));
	else
		memset(wifi_setting.password, 0, sizeof(wifi_setting.password));
	wifi_setting.channel = ap.channel;
#if CONFIG_READ_FLASH
	StoreApInfo();
#endif
#endif
	
#if CONFIG_LWIP_LAYER
	dhcps_deinit();
	IP4_ADDR(&ipaddr, GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);
	IP4_ADDR(&netmask, NETMASK_ADDR0, NETMASK_ADDR1 , NETMASK_ADDR2, NETMASK_ADDR3);
	IP4_ADDR(&gw, GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);
	netif_set_addr(pnetif, &ipaddr, &netmask,&gw);
#ifdef CONFIG_DONT_CARE_TP
	pnetif->flags |= NETIF_FLAG_IPSWITCH;
#endif
#endif
	wifi_off();
	rtw_mdelay_os(20);
	if (wifi_on(RTW_MODE_AP) < 0){
		printf("\n\rERROR: Wifi on failed!");
		ret = RTW_ERROR;
		goto exit;
	}
	printf("\n\rStarting AP ...");

#if defined(CONFIG_ENABLE_WPS_AP) && CONFIG_ENABLE_WPS_AP
	wpas_wps_dev_config(pnetif->hwaddr, 1);
#endif
	if((ret = wifi_start_ap((char*)ap.ssid.val, ap.security_type, (char*)ap.password, ap.ssid.len, ap.password_len, ap.channel) )< 0) {
		printf("\n\rERROR: Operation failed!");
		goto exit;
	}

	while(1) {
		char essid[33];

		if(wext_get_ssid(WLAN0_NAME, (unsigned char *) essid) > 0) {
			if(strcmp((const char *) essid, (const char *)ap.ssid.val) == 0) {
				printf("\n\r%s started\n", ap.ssid.val);
				ret = RTW_SUCCESS;
				break;
			}
		}

		if(timeout == 0) {
			printf("\n\rERROR: Start AP timeout!");
			ret = RTW_TIMEOUT;		
			break;
		}

		rtw_mdelay_os(1 * configTICK_RATE_HZ);
		timeout --;
	}

#if defined( CONFIG_ENABLE_AP_POLLING_CLIENT_ALIVE )&&( CONFIG_ENABLE_AP_POLLING_CLIENT_ALIVE == 1 )
	wifi_set_ap_polling_sta(1);
#endif

#if CONFIG_LWIP_LAYER
	//LwIP_UseStaticIP(pnetif);
	dhcps_init(pnetif);
#endif

exit:
#if CONFIG_INIC_CMD_RSP
	inic_c2h_wifi_info("ATWA", ret);
#endif
	init_wifi_struct( );
}

#if CONFIG_INIC_EN
static int _find_ap_from_scan_buf(char*buf, int buflen, char *target_ssid, void *user_data)
{
	rtw_wifi_setting_t *pwifi = (rtw_wifi_setting_t *)user_data;
	int plen = 0;
	
	while(plen < buflen){
		u8 len, ssid_len, security_mode;
		char *ssid;

		// len offset = 0
		len = (int)*(buf + plen);
		// check end
		if(len == 0) break;
		// ssid offset = 14
		ssid_len = len - 14;
		ssid = buf + plen + 14 ;
		if((ssid_len == strlen(target_ssid))
			&& (!memcmp(ssid, target_ssid, ssid_len)))
		{
			strcpy((char*)pwifi->ssid, target_ssid);
			// channel offset = 13
			pwifi->channel = *(buf + plen + 13);
			// security_mode offset = 11
			security_mode = (u8)*(buf + plen + 11);
			if(security_mode == IW_ENCODE_ALG_NONE)
				pwifi->security_type = RTW_SECURITY_OPEN;
			else if(security_mode == IW_ENCODE_ALG_WEP)
				pwifi->security_type = RTW_SECURITY_WEP_PSK;
			else if(security_mode == IW_ENCODE_ALG_CCMP)
				pwifi->security_type = RTW_SECURITY_WPA2_AES_PSK;
			break;
		}
		plen += len;
	}
	return 0;
}

static int _get_ap_security_mode(IN char * ssid, OUT rtw_security_t *security_mode, OUT u8 * channel)
{
	rtw_wifi_setting_t wifi;
	u32 scan_buflen = 1000;

	memset(&wifi, 0, sizeof(wifi));

	if(wifi_scan_networks_with_ssid(_find_ap_from_scan_buf, (void*)&wifi, scan_buflen, ssid, strlen(ssid)) != RTW_SUCCESS){
		printf("Wifi scan failed!\n");
		return 0;
	}

	if(strcmp(wifi.ssid, ssid) == 0){
		*security_mode = wifi.security_type;
		*channel = wifi.channel;
		return 1;
	}
	
	return 0;
}
#endif

void fATWC(void *arg){
	int mode, ret;
	unsigned long tick1 = rtw_get_current_time();
	unsigned long tick2, tick3;
	char empty_bssid[6] = {0}, assoc_by_bssid = 0;	
	
	printf("[ATWC]: _AT_WLAN_JOIN_NET_\n\r"); 
	if(memcmp (wifi.bssid.octet, empty_bssid, 6))
		assoc_by_bssid = 1;
	else if(wifi.ssid.val[0] == 0){
		printf("[ATWC]Error: SSID can't be empty\n\r");
		ret = RTW_BADARG;
		goto EXIT;
	}
	if(wifi.password != NULL){
		if((wifi.key_id >= 0)&&(wifi.key_id <= 3)) {
			wifi.security_type = RTW_SECURITY_WEP_PSK;
		}
		else{
			wifi.security_type = RTW_SECURITY_WPA2_AES_PSK;
		}
	}
	else{
		wifi.security_type = RTW_SECURITY_OPEN;
	}
	//Check if in AP mode
	wext_get_mode(WLAN0_NAME, &mode);
	if(mode == IW_MODE_MASTER) {
#if CONFIG_LWIP_LAYER
		dhcps_deinit();
#endif
		wifi_off();
		rtw_mdelay_os(20);
		if (wifi_on(RTW_MODE_STA) < 0){
			printf("\n\rERROR: Wifi on failed!");
                        ret = RTW_ERROR;
			goto EXIT;
		}
	}

#if CONFIG_INIC_EN //get security mode from scan list
	u8 connect_channel;
	u8 pscan_config;
	//the keyID may be not set for WEP which may be confued with WPA2 
	if((wifi.security_type == RTW_SECURITY_UNKNOWN)||(wifi.security_type == RTW_SECURITY_WPA2_AES_PSK))
	{
		int security_retry_count = 0;
		while (1) {
			if (_get_ap_security_mode((char*)wifi.ssid.val, &wifi.security_type, &connect_channel))
				break;
			security_retry_count++;
			if(security_retry_count >= 3){
				printf("Can't get AP security mode and channel.\n");
				ret = RTW_NOTFOUND;
				goto EXIT;
			}
		}
		if(wifi.security_type == RTW_SECURITY_WEP_PSK || wifi.security_type == RTW_SECURITY_WEP_SHARED)
			wifi.key_id = (wifi.key_id <0 || wifi.key_id >3)?0:wifi.key_id;
#if 0 //implemented in wifi_connect()
		//hex to ascii conversion
		if(wifi.security_type == RTW_SECURITY_WEP_PSK)
		{
			if(wifi.password_len == 10)
			{
				u32 p[5];
				u8 pwd[6], i = 0; 
				sscanf((const char*)wifi.password, "%02x%02x%02x%02x%02x", &p[0], &p[1], &p[2], &p[3], &p[4]);
				for(i=0; i< 5; i++)
					pwd[i] = (u8)p[i];
				pwd[5] = '\0';
				memset(wifi.password, 0, 65);
				strcpy((char*)wifi.password, (char*)pwd);
				wifi.password_len = 5;
			}else if(wifi.password_len == 26){
				u32 p[13];
				u8 pwd[14], i = 0;
				sscanf((const char*)wifi.password, "%02x%02x%02x%02x%02x%02x%02x"\
				"%02x%02x%02x%02x%02x%02x", &p[0], &p[1], &p[2], &p[3], &p[4],\
				&p[5], &p[6], &p[7], &p[8], &p[9], &p[10], &p[11], &p[12]);
				for(i=0; i< 13; i++)
					pwd[i] = (u8)p[i];
				pwd[13] = '\0';
				memset(wifi.password, 0, 65);
				strcpy((char*)wifi.password, (char*)pwd);
				wifi.password_len = 13;
			}
		}
#endif
	}
	pscan_config = PSCAN_ENABLE;
	if(connect_channel > 0 && connect_channel < 14)
		wifi_set_pscan_chan(&connect_channel, &pscan_config, 1);
#endif

	if(assoc_by_bssid){
		printf("\n\rJoining BSS by BSSID "MAC_FMT" ...\n\r", MAC_ARG(wifi.bssid.octet));
		ret = wifi_connect_bssid(wifi.bssid.octet, (char*)wifi.ssid.val, wifi.security_type, (char*)wifi.password, 
						ETH_ALEN, wifi.ssid.len, wifi.password_len, wifi.key_id, NULL);		
	} else {
		printf("\n\rJoining BSS by SSID %s...\n\r", (char*)wifi.ssid.val);
		ret = wifi_connect((char*)wifi.ssid.val, wifi.security_type, (char*)wifi.password, wifi.ssid.len,
						wifi.password_len, wifi.key_id, NULL);
	}
	
	if(ret!= RTW_SUCCESS){
		if(ret == RTW_INVALID_KEY)
			printf("\n\rERROR:Invalid Key ");
		
		printf("\n\rERROR: Can't connect to AP");
		goto EXIT;
	}
	tick2 = rtw_get_current_time();
	printf("\r\nConnected after %dms.\n", (tick2-tick1));
#if CONFIG_LWIP_LAYER
		/* Start DHCPClient */
		LwIP_DHCP(0, DHCP_START);
	tick3 = rtw_get_current_time();
	printf("\r\n\nGot IP after %dms.\n", (tick3-tick1));
#endif
	printf("\n\r");
EXIT:
#if CONFIG_INIC_CMD_RSP
	inic_c2h_wifi_info("ATWC", ret);
#endif

	init_wifi_struct( );
}

#if SCAN_WITH_SSID
void fATWs(void *arg){
	char buf[32] = {0};
	u8 *channel_list = NULL;
	u8 *pscan_config = NULL;
	int scan_buf_len = 500;
	int num_channel = 0;
	int i, argc = 0;
	char *argv[MAX_ARGC] = {0};
	printf("[ATWs]: _AT_WLAN_SCAN_WITH_SSID_ [%s]\n\r",  (char*)wifi.ssid.val);
	if(arg){
		strcpy(buf, arg);
		argc = parse_param(buf, argv);
		if(argc == 2){
			scan_buf_len = atoi(argv[1]);  	
			if(scan_buf_len < 36){
				printf("[ATWs] BUFFER_LENGTH too short\n\r");
				goto exit;
			}
		}else if(argc > 2){
			num_channel = atoi(argv[1]);
			channel_list = (u8*)malloc(num_channel);
			if(!channel_list){
				printf("[ATWs]ERROR: Can't malloc memory for channel list\n\r");
				goto exit;
			}
			pscan_config = (u8*)malloc(num_channel);
			if(!pscan_config){
				printf("[ATWs]ERROR: Can't malloc memory for pscan_config\n\r");
				goto exit;
			}			
			//parse command channel list
			for(i = 2; i <= argc -1 ; i++){
				*(channel_list + i - 2) = (u8)atoi(argv[i]);
				*(pscan_config + i - 2) = PSCAN_ENABLE;	
			}

			if(wifi_set_pscan_chan(channel_list, pscan_config, num_channel) < 0){
				printf("[ATWs]ERROR: wifi set partial scan channel fail\n\r");
				goto exit;
			}
		}
	}else{
		printf("[ATWs]For Scan all channel Usage: ATWs=BUFFER_LENGTH\n\r");          
		printf("[ATWs]For Scan partial channel Usage: ATWs=num_channels[channel_num1, ...]\n\r");
		goto exit;
	}

	if(wifi_scan_networks_with_ssid(NULL, NULL, scan_buf_len, (char*)wifi.ssid.val, wifi.ssid.len) != RTW_SUCCESS){
		printf("[ATWs]ERROR: wifi scan failed\n\r");
	}
exit:
	init_wifi_struct( );
	if(arg && channel_list)
		free(channel_list);
	if(arg && pscan_config)
		free(pscan_config);	
}
#endif

void fATWR(void *arg){
	int rssi = 0;
	printf("[ATWR]: _AT_WLAN_GET_RSSI_\n\r"); 
	wifi_get_rssi(&rssi);
	printf("\n\rwifi_get_rssi: rssi = %d", rssi);
	printf("\n\r");
}

void fATWP(void *arg){
	unsigned int parm = atoi((const char *)(arg));
	printf("[ATWP]: _AT_WLAN_POWER_[%s]\n\r", parm?"ON":"OFF");
	if(parm == 1){
		if(wifi_on(RTW_MODE_STA)<0){
			printf("\n\rERROR: Wifi on failed!\n");
		}
	}
	else if(parm == 0)
	{
#if CONFIG_WEBSERVER
		stop_web_server();
#endif
		wifi_off();		
	}
	else
		printf("[ATWP]Usage: ATWP=0/1\n\r");
}

#if CONFIG_WOWLAN_SERVICE
//for wowlan setting
void fATWV(void *arg){
	int argc;
	char *argv[MAX_ARGC] = {0};
	
	printf("[ATWV]: _AT_WLAN_WOWLAN_\r\n");

	argc = parse_param(arg, argv);

	cmd_wowlan_service(argc, argv);

	return;
}
#endif

#ifdef  CONFIG_CONCURRENT_MODE
void fATWB(void *arg)
{
	int timeout = 20;//, mode;
	int ret = RTW_SUCCESS;
#if CONFIG_LWIP_LAYER
	struct netif * pnetiff = (struct netif *)&xnetif[1];
#endif
	printf("[ATWB](_AT_WLAN_AP_STA_ACTIVATE_)\n\r"); 
	if(ap.ssid.val[0] == 0){
		printf("[ATWB]Error: SSID can't be empty\n\r");
		ret = RTW_BADARG;
		goto exit;
	}
	if(ap.channel > 14){
		printf("[ATWB]Error:bad channel! channel is from 1 to 14\n\r");
		ret = RTW_BADARG;
		goto exit;
	}
	if(ap.password == NULL){
		ap.security_type = RTW_SECURITY_OPEN;
	}
	else{
		ap.security_type = RTW_SECURITY_WPA2_AES_PSK;
	}

#if CONFIG_WEBSERVER
	//store into flash
	memset(wifi_setting.ssid, 0, sizeof(wifi_setting.ssid));;
	memcpy(wifi_setting.ssid, ap.ssid.val, strlen((char*)ap.ssid.val));
	wifi_setting.ssid[ap.ssid.len] = '\0';
	wifi_setting.security_type = ap.security_type;
	if(ap.security_type !=0)
		wifi_setting.security_type = 1;
	else
		wifi_setting.security_type = 0;
	if (ap.password)
		memcpy(wifi_setting.password, ap.password, strlen((char*)ap.password));
	else
		memset(wifi_setting.password, 0, sizeof(wifi_setting.password));;
	wifi_setting.channel = ap.channel;
#if CONFIG_READ_FLASH
	StoreApInfo();
#endif
#endif

#if CONFIG_LWIP_LAYER
	dhcps_deinit();
#endif

	wifi_off();
	rtw_mdelay_os(20);
	if ((ret = wifi_on(RTW_MODE_STA_AP)) < 0){
		printf("\n\rERROR: Wifi on failed!");
		ret = RTW_ERROR;
		goto exit;
	}

	printf("\n\rStarting AP ...");
	if((ret = wifi_start_ap((char*)ap.ssid.val, ap.security_type, (char*)ap.password, ap.ssid.len, ap.password_len, ap.channel)) < 0) {
		printf("\n\rERROR: Operation failed!");
		goto exit;
	}
	while(1) {
		char essid[33];

		if(wext_get_ssid(WLAN1_NAME, (unsigned char *) essid) > 0) {
			if(strcmp((const char *) essid, (const char *)ap.ssid.val) == 0) {
				printf("\n\r%s started\n", ap.ssid.val);
				ret = RTW_SUCCESS;
				break;
			}
		}

		if(timeout == 0) {
			printf("\n\rERROR: Start AP timeout!");
			ret = RTW_TIMEOUT;
			break;
		}

		rtw_mdelay_os(1 * configTICK_RATE_HZ);
		timeout --;
	}
#if CONFIG_LWIP_LAYER
	LwIP_UseStaticIP(&xnetif[1]);
#ifdef CONFIG_DONT_CARE_TP
	pnetiff->flags |= NETIF_FLAG_IPSWITCH;
#endif
	dhcps_init(pnetiff);
#endif

#if defined( CONFIG_ENABLE_AP_POLLING_CLIENT_ALIVE )&&( CONFIG_ENABLE_AP_POLLING_CLIENT_ALIVE == 1 )
	wifi_set_ap_polling_sta(1);
#endif
exit:
#if CONFIG_INIC_CMD_RSP
	inic_c2h_wifi_info("ATWB", ret);
#endif
	init_wifi_struct();
}
#endif

#ifdef CONFIG_PROMISC
void fATWM(void *arg){ 
	int argc;
	char *argv[MAX_ARGC] = {0};
	argv[0] = "wifi_promisc";        
	printf("[ATWM]: _AT_WLAN_PROMISC_\n\r");
	if(!arg){
		printf("[ATWM]Usage: ATWM=DURATION_SECONDS[with_len]");
#if CONFIG_INIC_CMD_RSP
		inic_c2h_msg("ATWM", RTW_BADARG, NULL, 0);
#endif
		return;
	}
	if((argc = parse_param(arg, argv)) > 1){
		cmd_promisc(argc, argv);
	}
}
#endif

#if CONFIG_WEBSERVER
void fATWE(void *arg){
	printf("[ATWE]: _AT_WLAN_START_WEB_SERVER_\n\r"); 
	start_web_server();
}
#endif

void fATWW(void *arg){
#if CONFIG_ENABLE_WPS
	int argc = 0;
	char *argv[4];
	printf("[ATWW]: _AT_WLAN_WPS_\n\r");
	if(!arg){
		printf("[ATWW]Usage: ATWW=pbc/pin\n\r");
#if CONFIG_INIC_CMD_RSP
		inic_c2h_msg("ATWW", RTW_BADARG, NULL, 0);
#endif
		return;
	}
	argv[argc++] = "wifi_wps";
	argv[argc++] = arg;
	cmd_wps(argc, argv);
#else
	printf("Please set CONFIG_ENABLE_WPS 1 in platform_opts.h to enable ATWW command\n");
#endif
}
void fATWw(void *arg){
#if defined(CONFIG_ENABLE_WPS_AP) && CONFIG_ENABLE_WPS_AP
	int argc = 0;
	char *argv[4];
	printf("[ATWw]: _AT_WLAN_AP_WPS_\n\r");
	if(!arg){
		printf("[ATWw]Usage: ATWw=pbc/pin\n\r");
		return;
	}
	argv[argc++] = "wifi_ap_wps";
	argv[argc++] = arg;
	cmd_ap_wps(argc, argv);
#endif		
}

#if CONFIG_ENABLE_P2P
void fATWG(void *arg){
	int argc = 0;
	char *argv[4];
	printf("[ATWG]: _AT_WLAN_P2P_START_\n\r");
	argv[argc++] = "p2p_start";
	cmd_wifi_p2p_start(argc, argv);
}

void fATWg(void *arg){
	int argc = 0;
	char *argv[4];
	int ret =0;
	printf("[ATWg]: _AT_WLAN_P2P_AUTO_GO_START_\n\r");
	argv[argc++] = "p2p_auto_go_start";
	ret = cmd_wifi_p2p_auto_go_start(argc, argv);
	if(ret < 0)
		printf("\r\n[ATWG]: Nothing to do. Please enter ATWG to initialize P2P.\n\r");
}

void fATWH(void *arg){
	int argc = 0;
	char *argv[4];
	printf("[ATWH]: _AT_WLAN_P2P_STOP_\n\r");
	argv[argc++] = "p2p_stop";
	cmd_wifi_p2p_stop(argc, argv);
}
void fATWJ(void *arg){
	int argc = 0;
	char *argv[4];
	printf("[ATWJ]: _AT_WLAN_P2P_CONNECT_\n\r");
	argv[0] = "p2p_connect";
	if(!arg){
		printf("ATWc=[DEST_MAC,pbc/pin]\n\r");
		return;
	}
	if((argc = parse_param(arg, argv)) > 1){
		cmd_p2p_connect(argc, argv);
	}
}
void fATWK(void *arg){
	int argc = 0;
	char *argv[4];
	printf("[ATWK]: _AT_WLAN_P2P_DISCONNECT_\n\r");
	argv[argc++] = "p2p_disconnect";
	cmd_p2p_disconnect(argc, argv);
}
void fATWN(void *arg){
	int argc = 0;
	char *argv[4];
	printf("[ATWN]: _AT_WLAN_P2P_INFO_\n\r");
	argv[argc++] = "p2p_info";
	cmd_p2p_info(argc, argv);
}
void fATWF(void *arg){
	int argc = 0;
	char *argv[4];
	printf("[ATWF]: _AT_WLAN_P2P_FIND_\n\r");
	argv[argc++] = "p2p_find";
	cmd_p2p_find(argc, argv);
}
#endif
#if CONFIG_OTA_UPDATE
void fATWO(void *arg){
	int argc = 0;
	char *argv[MAX_ARGC] = {0};
	printf("[ATWO]: _AT_WLAN_OTA_UPDATE_\n\r");
	if(!arg){
		printf("[ATWO]Usage: ATWO=IP[PORT] or ATWO= REPOSITORY[FILE_PATH]\n\r");
		return;
	}
	argv[0] = "update";
	if((argc = parse_param(arg, argv)) != 3){
		printf("[ATWO]Usage: ATWO=IP[PORT] or ATWO= REPOSITORY[FILE_PATH]\n\r");
		return;
	}
	cmd_update(argc, argv);
}
#endif

#if CONFIG_AIRKISS
void fATWX(void *arg)
{
	int ret = RTW_SUCCESS;
	
	rtw_network_info_t wifi;
	memset(&wifi, 0 , sizeof(rtw_network_info_t));
	wifi.password = (unsigned char *)malloc(65);
	memset(wifi.password, 0, 65);
	
	ret = airkiss_start(&wifi);
#if CONFIG_INIC_CMD_RSP
	if(ret != RTW_SUCCESS)
		inic_c2h_msg("ATWX", RTW_ERROR, NULL, 0);
#endif	
}
#endif

void fATWZ(void *arg){
	char buf[32] = {0};
	char *copy = buf;
	int i = 0;
	int len = 0;
	int ret = RTW_SUCCESS;

	printf("[ATWZ]: _AT_WLAN_IWPRIV_\n\r");
	if(!arg){
		printf("[ATWZ]Usage: ATWZ=COMMAND[PARAMETERS]\n\r");
		ret = RTW_BADARG;
		goto exit;
	}
	strcpy(copy, arg);
	len = strlen(copy);
	do{
		if((*(copy+i)=='['))
			*(copy+i)=' ';
		if((*(copy+i)==']')||(*(copy+i)=='\0')){
			*(copy+i)='\0';
			break;
		}
	}while((i++) < len);

	i = 0;
	do{
		if((*(copy+i)==',')) {
			*(copy+i)=' ';
			break;
		}
	}while((i++) < len);

#if CONFIG_INIC_CMD_RSP
	ret = wext_private_command_with_retval(WLAN0_NAME, copy, buf, 32);
	printf("\n\rPrivate Message: %s", (char *) buf);
	if(ret == RTW_SUCCESS)
		inic_c2h_msg("ATWZ", ret, buf, strlen(buf));
#else
	wext_private_command(WLAN0_NAME, copy, 1);
#endif
exit:
#if CONFIG_INIC_CMD_RSP
	if(ret != RTW_SUCCESS)
		inic_c2h_msg("ATWZ", ret, NULL, 0);
#endif
	return;	// exit label cannot be last statement
}

#ifdef CONFIG_POWER_SAVING
void fATXP(void *arg)
{
	int argc = 0;
	char *argv[MAX_ARGC] = {0};
	int ret = 0;
	int mode, dtim;
	int tdma_slot_period, tdma_rfon_period_len_1, tdma_rfon_period_len_2, tdma_rfon_period_len_3;
#if CONFIG_INIC_CMD_RSP
	char *res = NULL;
	int res_len = 0;
#endif

	printf("[ATXP]: _AT_WLAN_POWER_MODE_\r\n");

	if (!arg) {
		printf("[ATXP] Usage: ATXP=lps/ips/dtim/tdma[mode]\r\n");
		ret = RTW_BADARG;
		goto exit;
	} else {
		argc = parse_param(arg, argv);
		if (argc < 3) {
			printf("[ATXP] Usage: ATXP=lps/ips/dtim/tdma[mode]\r\n");
			ret = RTW_BADARG;
			goto exit;
		}
	}

	if (strcmp(argv[1], "lps") == 0) {
		mode = atoi(argv[2]);
		if (mode >= 0 && mode < 0xFF) {
			printf("lps mode:%d\r\n", mode);
			wifi_set_power_mode(0xff, mode);
		}
	}

	if (strcmp(argv[1], "ips") == 0) {
		mode = atoi(argv[2]);
		if (mode >= 0 && mode < 0xFF) {
			printf("ips mode:%d\r\n", mode);
			wifi_set_power_mode(mode, 0xFF);
		}
	}

	if (strcmp(argv[1], "tdma") == 0) {
		if (argc >= 6) {
			tdma_slot_period = atoi(argv[2]);
			tdma_rfon_period_len_1 = atoi(argv[3]);
			tdma_rfon_period_len_2 = atoi(argv[4]);
			tdma_rfon_period_len_3 = atoi(argv[5]);
			printf("tdma param: %d %d %d %d\r\n", tdma_slot_period, tdma_rfon_period_len_1, tdma_rfon_period_len_2, tdma_rfon_period_len_3);
			wifi_set_tdma_param(tdma_slot_period, tdma_rfon_period_len_1, tdma_rfon_period_len_2, tdma_rfon_period_len_3);
		}
	}

	if (strcmp(argv[1], "dtim") == 0) {
		dtim = atoi(argv[2]);
		printf("dtim: %d\r\n", dtim);
		wifi_set_lps_dtim(dtim);
	}

	if (strcmp(argv[1], "get") == 0) {
#if CONFIG_INIC_CMD_RSP
		char buf[32];
		int index = 0;
		memset(buf, 0, sizeof(buf));
		snprintf(buf, 32, "%s,%s,", argv[1], argv[2]);
		index = strlen(buf);
#endif
		if(strcmp(argv[2], "dtim") == 0){
			wifi_get_lps_dtim((unsigned char *)&dtim);
			printf("get dtim: %d\r\n", (unsigned char)dtim);
#if CONFIG_INIC_CMD_RSP
			sprintf(buf+index, "0x%02x", (unsigned char)dtim);
			res = (char *)buf;
			res_len = strlen(buf);
#endif
		}
	}
	
exit:
#if CONFIG_INIC_CMD_RSP
	inic_c2h_msg("ATXP", ret, res, res_len);
	res = NULL;
	res_len = 0;
#endif
	return;
}
#endif

void print_wlan_help(void *arg){
	printf("\n\rWLAN AT COMMAND SET:");
	printf("\n\r==============================");
	printf("\n\r1. Wlan Scan for Network Access Point");
	printf("\n\r   # ATWS");
	printf("\n\r2. Connect to an AES AP");
	printf("\n\r   # ATW0=SSID");
	printf("\n\r   # ATW1=PASSPHRASE");
	printf("\n\r   # ATWC");
	printf("\n\r3. Create an AES AP");
	printf("\n\r   # ATW3=SSID");
	printf("\n\r   # ATW4=PASSPHRASE");
	printf("\n\r   # ATW5=CHANNEL");
	printf("\n\r   # ATWA");
	printf("\n\r4. Ping");
	printf("\n\r   # ATWI=xxx.xxx.xxx.xxx");
}

#if WIFI_LOGO_CERTIFICATION_CONFIG
void fATPE(void *arg)
{
    int argc, error_no = 0;
    char *argv[MAX_ARGC] = {0};
    unsigned int ip_addr = 0;
    //unsigned char sta_ip[4] = {192,168,3,80}, sta_netmask[4] = {255,255,255,0}, sta_gw[4] = {192,168,3,1};

    if(!arg){
        AT_DBG_MSG(AT_FLAG_WIFI, AT_DBG_ERROR,
            "\r\n[ATPE] Usage : ATPE=<ip>(,<gateway>,<mask>)");
        error_no = 1;
        goto exit;
    }

    argc = parse_param(arg, argv);
    if( (argc > 4) || (argc < 2) ){
        //at_printf("\r\n[ATPE] ERROR : command format error");
        error_no = 1;
        goto exit;
    }

    if(argv[1] != NULL){
        ip_addr = inet_addr(argv[1]);
        sta_ip[0] = (unsigned char) ip_addr & 0xff;
        sta_ip[1] = (unsigned char) (ip_addr >> 8) & 0xff;
        sta_ip[2] = (unsigned char) (ip_addr >> 16) & 0xff;
        sta_ip[3] = (unsigned char) (ip_addr >> 24) & 0xff;
    }
    else{
        //at_printf("\r\n[ATPE] ERROR : parameter format error");
        error_no = 2;
        goto exit;
    }

    if(argv[2] != NULL){
        ip_addr = inet_addr(argv[2]);
        sta_gw[0] = (unsigned char) ip_addr & 0xff;
        sta_gw[1] = (unsigned char) (ip_addr >> 8) & 0xff;
        sta_gw[2] = (unsigned char) (ip_addr >> 16) & 0xff;
        sta_gw[3] = (unsigned char) (ip_addr >> 24) & 0xff;
    }
    else{
        sta_gw[0] = sta_ip[0];
        sta_gw[1] = sta_ip[1];
        sta_gw[2] = sta_ip[2];
        sta_gw[3] = 1;
    }

    if(argv[3] != NULL){
        ip_addr = inet_addr(argv[3]);
        sta_netmask[0] = (unsigned char) ip_addr & 0xff;
        sta_netmask[1] = (unsigned char) (ip_addr >> 8) & 0xff;
        sta_netmask[2] = (unsigned char) (ip_addr >> 16) & 0xff;
        sta_netmask[3] = (unsigned char) (ip_addr >> 24) & 0xff;
    }
    else{
        sta_netmask[0] = 255;
        sta_netmask[1] = 255;
        sta_netmask[2] = 255;
        sta_netmask[3] = 0;
    }

exit:
    if(error_no==0)
        at_printf("\r\n[ATPE] OK");
    else
        at_printf("\r\n[ATPE] ERROR:%d",error_no);

    return;

}
#endif

#elif ATCMD_VER == ATVER_2 // UART module at command

//ATPA=<ssid>,<pwd>,<chl>,<hidden>[,<max_conn>]
void fATPA(void *arg)
{
	int argc, error_no = 0;
	char *argv[MAX_ARGC] = {0};

#if CONFIG_LWIP_LAYER
	ip_addr_t ipaddr;
	ip_addr_t netmask;
	ip_addr_t gw;
	struct netif * pnetif;
#endif
	int timeout = 20;
	unsigned char hidden_ssid = 0;
	rtw_mode_t wifi_mode_copy;

	if(!arg){
		AT_DBG_MSG(AT_FLAG_WIFI, AT_DBG_ERROR,
			"\r\n[ATPA] Usage: ATPA=<ssid>,<pwd>,<chl>,<hidden>[,<max_conn>]");
		error_no = 1;
		goto exit;
	}

	argc = parse_param(arg, argv);
	if(argc < 5){
		//at_printf("\r\n[ATPA] ERROR : command format error");
		error_no = 1;
		goto exit;
	}

	if( (wifi_mode!=RTW_MODE_AP) && (wifi_mode!=RTW_MODE_STA_AP) ){
		//at_printf("\r\n[ATPA] ERROR : wifi mode error");
		error_no = 5;
		goto exit;
	}
	wifi_mode_copy = wifi_mode;

	//SSID
	if(argv[1] != NULL){
		ap.ssid.len = strlen((char*)argv[1]);
		if(ap.ssid.len > 32){
			//at_printf("\r\n[ATPA] ERROR : SSID length can't exceed 32");
			error_no = 2;
			goto exit;
		}
		strcpy((char *)ap.ssid.val, (char*)argv[1]);
	}
	else{
		//at_printf("\r\n[ATPA] ERROR : SSID can't be empty");
		error_no = 2;
		goto exit;
	}

	//PASSWORD
	if(argv[2] != NULL){
		if( (strlen(argv[2]) < 8) || (strlen(argv[2]) > 64)){
			//at_printf("\r\n[ATPA] ERROR : PASSWORD length error");
			error_no = 2;
			goto exit;
		}
		strcpy((char *)password, (char*)argv[2]);
		ap.password = password;
		ap.password_len = strlen((char*)argv[2]);
		ap.security_type = RTW_SECURITY_WPA2_AES_PSK;
	}
	else{
		ap.security_type = RTW_SECURITY_OPEN;
	}

	//CHANNEL
	if(argv[3] != NULL){
		ap.channel = (unsigned char) atoi((const char *)argv[3]);
		if( (ap.channel < 0) || (ap.channel > 11) ){
			//at_printf("\r\n[ATPA] ERROR : channel number error");
			error_no = 2;
			goto exit;
		}
	}

	//HIDDEN SSID
	if(argv[4] != NULL){
		if( (atoi(argv[4]) != 0) && (atoi(argv[4]) != 1)){
			//at_printf("\r\n[ATPA] ERROR : HIDDEN SSID must be 0 or 1");
			error_no = 2;
			goto exit;
		}
		hidden_ssid = (unsigned char) atoi((const char *)argv[4]);
	}

	//MAX NUMBER OF STATION
	if(argv[5] != NULL){
		unsigned char max_sta = atoi(argv[5]);
		if(wext_set_sta_num(max_sta) != 0){
			error_no = 2;
			goto exit;
		}
	}
	
#if CONFIG_WEBSERVER
	//store into flash
	memset(wifi_setting.ssid, 0, sizeof(wifi_setting.ssid));
	memcpy(wifi_setting.ssid, ap.ssid.val, strlen((char*)ap.ssid.val));
	wifi_setting.ssid[ap.ssid.len] = '\0';
	wifi_setting.security_type = ap.security_type;
	if(ap.security_type !=0)
		wifi_setting.security_type = 1;
	else
		wifi_setting.security_type = 0;
	if (ap.password)
		memcpy(wifi_setting.password, ap.password, strlen((char*)ap.password));
	else
		memset(wifi_setting.password, 0, sizeof(wifi_setting.password));
	wifi_setting.channel = ap.channel;
#if CONFIG_READ_FLASH
	StoreApInfo();
#endif
#endif

#if CONFIG_LWIP_LAYER
	dhcps_deinit();
#endif

	wifi_unreg_event_handler(WIFI_EVENT_DISCONNECT, atcmd_wifi_disconn_hdl);

	wifi_off();
	rtw_mdelay_os(20);

	if (wifi_on(wifi_mode_copy) < 0){
		//at_printf("\r\n[ATPA] ERROR : Wifi on failed");
		error_no = 3;
		goto exit;
	}

	if(hidden_ssid){
		if(wifi_start_ap_with_hidden_ssid((char*)ap.ssid.val, ap.security_type, (char*)ap.password, ap.ssid.len, ap.password_len, ap.channel) < 0) {
			//at_printf("\r\n[ATPA] ERROR : Start AP failed");
			error_no = 4;
			goto exit;
		}
	}
	else{
		if(wifi_start_ap((char*)ap.ssid.val, ap.security_type, (char*)ap.password, ap.ssid.len, ap.password_len, ap.channel) < 0) {
			//at_printf("\r\n[ATPA] ERROR : Start AP failed");
			error_no = 4;
			goto exit;
		}
	}

	while(1) {
		char essid[33];
		if(wifi_mode_copy == RTW_MODE_AP ){
			if(wext_get_ssid( WLAN0_NAME , (unsigned char *) essid) > 0) {
				if(strcmp((const char *) essid, (const char *)ap.ssid.val) == 0) {
					break;
				}
			}
		}
		else if(wifi_mode_copy == RTW_MODE_STA_AP ){
			if(wext_get_ssid( WLAN1_NAME , (unsigned char *) essid) > 0) {
				if(strcmp((const char *) essid, (const char *)ap.ssid.val) == 0) {
					break;
				}
			}
		}

		if(timeout == 0) {
			//at_printf("\r\n[ATPA] ERROR : Start AP timeout");
			error_no = 4;
			break;
		}

		rtw_mdelay_os(1 * configTICK_RATE_HZ);
		timeout --;
	}
#if CONFIG_LWIP_LAYER
	if(wifi_mode == RTW_MODE_STA_AP)
		pnetif = &xnetif[1];
	else
		pnetif = &xnetif[0];

	LwIP_UseStaticIP(pnetif);

	if(dhcp_mode_ap == 1)
		dhcps_init(pnetif);
#endif

exit:
	init_wifi_struct();

	if(error_no == 0)
		at_printf("\r\n[ATPA] OK");
	else
		at_printf("\r\n[ATPA] ERROR:%d",error_no);

	return;
}

/*find ap with "ssid" from scan list*/
static int _find_ap_from_scan_buf(char*buf, int buflen, char *target_ssid, void *user_data)
{
	rtw_wifi_setting_t *pwifi = (rtw_wifi_setting_t *)user_data;
	int plen = 0;
	
	while(plen < buflen){
		u8 len, ssid_len, security_mode;
		char *ssid;

		// len offset = 0
		len = (int)*(buf + plen);
		// check end
		if(len == 0) break;
		// ssid offset = 14
		ssid_len = len - 14;
		ssid = buf + plen + 14 ;
		if((ssid_len == strlen(target_ssid))
			&& (!memcmp(ssid, target_ssid, ssid_len)))
		{
			strcpy((char*)pwifi->ssid, target_ssid);
			// channel offset = 13
			pwifi->channel = *(buf + plen + 13);
			// security_mode offset = 11
			security_mode = (u8)*(buf + plen + 11);
			if(security_mode == IW_ENCODE_ALG_NONE)
				pwifi->security_type = RTW_SECURITY_OPEN;
			else if(security_mode == IW_ENCODE_ALG_WEP)
				pwifi->security_type = RTW_SECURITY_WEP_PSK;
			else if(security_mode == IW_ENCODE_ALG_CCMP)
				pwifi->security_type = RTW_SECURITY_WPA2_AES_PSK;
			break;
		}
		plen += len;
	}
	return 0;
}

/*get ap security mode from scan list*/
static int _get_ap_security_mode(IN char * ssid, OUT rtw_security_t *security_mode, OUT u8 * channel)
{
	rtw_wifi_setting_t wifi;
	u32 scan_buflen = 1000;

	memset(&wifi, 0, sizeof(wifi));

	if(wifi_scan_networks_with_ssid(_find_ap_from_scan_buf, (void*)&wifi, scan_buflen, ssid, strlen(ssid)) != RTW_SUCCESS){
		printf("Wifi scan failed!\n");
		return 0;
	}

	if(strcmp(wifi.ssid, ssid) == 0){
		*security_mode = wifi.security_type;
		*channel = wifi.channel;
		return 1;
	}
	
	return 0;
}

//ATPN=<ssid>,<pwd>[,<key_id>,<bssid>]
static void atcmd_wifi_disconn_hdl( char* buf, int buf_len, int flags, void* userdata)
{
	#if CONFIG_LOG_SERVICE_LOCK
	log_service_lock();
	#endif
	at_printf("\r\n[ATWD] OK");			
	at_printf(STR_END_OF_ATCMD_RET);
	#if CONFIG_LOG_SERVICE_LOCK
	log_service_unlock();
	#endif
}
void fATPN(void *arg)
{
	int argc, error_no = 0;
	int i,j;
	char *argv[MAX_ARGC] = {0};

	int mode, ret;
	unsigned long tick1 = rtw_get_current_time();
	unsigned long tick2, tick3;
	char empty_bssid[6] = {0}, assoc_by_bssid = 0;
	u8 connect_channel;
	u8 pscan_config;

	if(!arg){
		AT_DBG_MSG(AT_FLAG_WIFI, AT_DBG_ERROR,
			"\r\n[ATPN] Usage : ATPN=<ssid>,<pwd>[,<key_id>,<bssid>]");
		error_no = 1;
		goto exit;
	}

	argc = parse_param(arg, argv);
	if( (argc < 2) || (argc > 5) ){
		//at_printf("\r\n[ATPN] ERROR : command format error");
		error_no = 1;
		goto exit;
	}

	if( (wifi_mode!=RTW_MODE_STA) && (wifi_mode!=RTW_MODE_STA_AP) ){
		//at_printf("\r\n[ATPN] ERROR : wifi mode error");
		error_no = 5;
		goto exit;
	}

	//SSID
	if(argv[1] != NULL){
		strcpy((char *)wifi.ssid.val, (char*)argv[1]);
		wifi.ssid.len = strlen((char*)argv[1]);
	}else{
		//at_printf("\r\n[ATPN] ERROR : SSID can't be Empty");
		error_no = 2;
		goto exit;
	}
	wifi.security_type = RTW_SECURITY_OPEN;

	//PASSWORD
	if(argv[2] != NULL){
		int pwd_len = strlen(argv[2]);
		if(pwd_len > 64 || (pwd_len < 8 && pwd_len != 5)){
			//at_printf("\r\n[ATPN] ERROR : PASSWORD format error");
			error_no = 2;
			goto exit;
		}
		strcpy((char *)password, (char*)argv[2]);
		wifi.password = password;
		wifi.password_len = strlen((char*)argv[2]);
		wifi.security_type = RTW_SECURITY_WPA2_AES_PSK;
	}

	//KEYID
	if(argv[3] != NULL){
		if((strlen((const char *)argv[3]) != 1 ) || (*(char*)argv[3] <'0' ||*(char*)argv[3] >'3')) {
			//at_printf("\r\n[ATPN] ERROR : Wrong WEP key id. Must be one of 0,1,2, or 3");
			error_no = 2;
			goto exit;
		}
		wifi.key_id = atoi((const char *)(argv[3]));
		wifi.security_type = RTW_SECURITY_WEP_PSK;
	}

	//BSSID
	if(argv[4] != NULL){
		if(strlen(argv[4]) != 12){
			//at_printf("\r\n[ATPN] ERROR : BSSID format error");
			error_no = 2;
			goto exit;
		}
		for (i=0, j=0; i<ETH_ALEN; i++, j+=2){
			wifi.bssid.octet[i] = key_2char2num(argv[4][j], argv[4][j+1]);
			assoc_by_bssid = 1;
		}
	}

	//Check if in AP mode
	wext_get_mode(WLAN0_NAME, &mode);
	if(mode == IW_MODE_MASTER) {
#if CONFIG_LWIP_LAYER
		dhcps_deinit();
#endif
		wifi_off();
		rtw_mdelay_os(20);
		if (wifi_on(RTW_MODE_STA) < 0){
			//at_printf("\r\n[ATPN] ERROR: Wifi on failed");
			error_no = 3;
			goto exit;
		}
	}

#if 1
	/************************************************************
	*    Get security mode from scan list, if it's WEP and key_id isn't set by user,
	*    system will use default key_id = 0
	************************************************************/
	//the keyID may be not set for WEP which may be confued with WPA2 
	if((wifi.security_type == RTW_SECURITY_UNKNOWN)||(wifi.security_type == RTW_SECURITY_WPA2_AES_PSK))
	{
		int security_retry_count = 0;
		while (1) {
			if (_get_ap_security_mode((char*)wifi.ssid.val, &wifi.security_type, &connect_channel))
				break;
			security_retry_count++;
			if(security_retry_count >= 3){
				printf("Can't get AP security mode and channel.\n");
				error_no = 6;
				goto exit;
			}
		}
		if(wifi.security_type == RTW_SECURITY_WEP_PSK || wifi.security_type == RTW_SECURITY_WEP_SHARED)
			wifi.key_id = (wifi.key_id <0 || wifi.key_id >3)?0:wifi.key_id;
	}
	pscan_config = PSCAN_ENABLE;
	if(connect_channel > 0 && connect_channel < 14)
		wifi_set_pscan_chan(&connect_channel, &pscan_config, 1);
#endif

	wifi_unreg_event_handler(WIFI_EVENT_DISCONNECT, atcmd_wifi_disconn_hdl);
	if(assoc_by_bssid){
		ret = wifi_connect_bssid(wifi.bssid.octet, (char*)wifi.ssid.val, wifi.security_type, (char*)wifi.password,
						ETH_ALEN, wifi.ssid.len, wifi.password_len, wifi.key_id, NULL);
	} else {
		ret = wifi_connect((char*)wifi.ssid.val, wifi.security_type, (char*)wifi.password, wifi.ssid.len,
						wifi.password_len, wifi.key_id, NULL);
	}

	if(ret!= RTW_SUCCESS){
		//at_printf("\r\n[ATPN] ERROR: Can't connect to AP");
		error_no = 4;
		goto exit;
	}

#if CONFIG_LWIP_LAYER
	if (dhcp_mode_sta == 2){
		struct netif * pnetif = &xnetif[0];
		LwIP_UseStaticIP(pnetif);
		dhcps_init(pnetif);
	}
	else{
		ret = LwIP_DHCP(0, DHCP_START);
		if(ret != DHCP_ADDRESS_ASSIGNED)
			error_no = 7;
	}
#endif


exit:
	init_wifi_struct();
	if(error_no == 0){
		wifi_reg_event_handler(WIFI_EVENT_DISCONNECT, atcmd_wifi_disconn_hdl, NULL);
		at_printf("\r\n[ATPN] OK");
	}
	else
		at_printf("\r\n[ATPN] ERROR:%d",error_no);

	return;
}

//ATPH=<mode>,<enable>
void fATPH(void *arg)
{
	int argc, error_no = 0;
	char *argv[MAX_ARGC] = {0};
	int mode,enable;

	if(!arg){
		AT_DBG_MSG(AT_FLAG_WIFI, AT_DBG_ERROR,
			"\r\n[ATPH] Usage : ATPH=<mode>,<enable>");
		error_no = 1;
		goto exit;
	}

	argc = parse_param(arg, argv);
	if(argc != 3){
		//at_printf("\r\n[ATPH] ERROR : command format error");
		error_no = 1;
		goto exit;
	}

	if(argv[1] != NULL){
		mode = atoi((const char *)(argv[1]));
		if(mode != 1 && mode != 2){
			//at_printf("\r\n[ATPH] ERROR : parameter must be 1 or 2");
			error_no = 2;
			goto exit;
		}
	}

	if(argv[2] != NULL){
		enable = atoi((const char *)(argv[2]));
		if(enable != 1 && enable != 2){
			//at_printf("\r\n[ATPH] ERROR : parameter must be 1 or 2");
			error_no = 2;
			goto exit;
		}
		if(mode == 1)
			dhcp_mode_ap = enable;
		else if(mode == 2)
			dhcp_mode_sta = enable;
	}

exit:
	if(error_no==0)
		at_printf("\r\n[ATPH] OK");
	else
		at_printf("\r\n[ATPH] ERROR:%d",error_no);

	return;

}

//ATPE=<ip>(,<gateway>,<mask>)
void fATPE(void *arg)
{
	int argc, error_no = 0;
	char *argv[MAX_ARGC] = {0};
	unsigned int ip_addr = 0;
	//unsigned char sta_ip[4] = {192,168,3,80}, sta_netmask[4] = {255,255,255,0}, sta_gw[4] = {192,168,3,1};

	if(!arg){
		AT_DBG_MSG(AT_FLAG_WIFI, AT_DBG_ERROR,
			"\r\n[ATPE] Usage : ATPE=<ip>(,<gateway>,<mask>)");
		error_no = 1;
		goto exit;
	}

	argc = parse_param(arg, argv);
	if( (argc > 4) || (argc < 2) ){
		//at_printf("\r\n[ATPE] ERROR : command format error");
		error_no = 1;
		goto exit;
	}

	if(argv[1] != NULL){
		ip_addr = inet_addr(argv[1]);
		sta_ip[0] = (unsigned char) ip_addr & 0xff;
		sta_ip[1] = (unsigned char) (ip_addr >> 8) & 0xff;
		sta_ip[2] = (unsigned char) (ip_addr >> 16) & 0xff;
		sta_ip[3] = (unsigned char) (ip_addr >> 24) & 0xff;
	}
	else{
		//at_printf("\r\n[ATPE] ERROR : parameter format error");
		error_no = 2;
		goto exit;
	}

	if(argv[2] != NULL){
		ip_addr = inet_addr(argv[2]);
		sta_gw[0] = (unsigned char) ip_addr & 0xff;
		sta_gw[1] = (unsigned char) (ip_addr >> 8) & 0xff;
		sta_gw[2] = (unsigned char) (ip_addr >> 16) & 0xff;
		sta_gw[3] = (unsigned char) (ip_addr >> 24) & 0xff;
	}
	else{
		sta_gw[0] = sta_ip[0];
		sta_gw[1] = sta_ip[1];
		sta_gw[2] = sta_ip[2];
		sta_gw[3] = 1;
	}

	if(argv[3] != NULL){
		ip_addr = inet_addr(argv[3]);
		sta_netmask[0] = (unsigned char) ip_addr & 0xff;
		sta_netmask[1] = (unsigned char) (ip_addr >> 8) & 0xff;
		sta_netmask[2] = (unsigned char) (ip_addr >> 16) & 0xff;
		sta_netmask[3] = (unsigned char) (ip_addr >> 24) & 0xff;
	}
	else{
		sta_netmask[0] = 255;
		sta_netmask[1] = 255;
		sta_netmask[2] = 255;
		sta_netmask[3] = 0;
	}

exit:
	if(error_no==0)
		at_printf("\r\n[ATPE] OK");
	else
		at_printf("\r\n[ATPE] ERROR:%d",error_no);

	return;

}

//ATPF=<start_ip>,<end_ip>,<gateway>
void fATPF(void *arg)
{
	int argc, error_no = 0;
	char *argv[MAX_ARGC] = {0};
	unsigned int ip_addr = 0;
	ip_addr_t start_ip, end_ip;

	if(!arg){
		AT_DBG_MSG(AT_FLAG_WIFI, AT_DBG_ERROR,
			"\r\n[ATPF] Usage : ATPF=<start_ip>,<end_ip>,<ip>(,<gateway>,<mask>)");
		error_no = 1;
		goto exit;
	}

	argc = parse_param(arg, argv);
	if( (argc != 4) ){
		//at_printf("\r\n[ATPF] ERROR : command format error");
		error_no = 1;
		goto exit;
	}

	if(argv[1] != NULL){
		start_ip.addr = inet_addr(argv[1]);
	}
	else{
		//at_printf("\r\n[ATPF] ERROR : parameter format error");
		error_no = 2;
		goto exit;
	}

	if(argv[2] != NULL){
		end_ip.addr = inet_addr(argv[2]);
	}
	else{
		//at_printf("\r\n[ATPF] ERROR : parameter format error");
		error_no = 2;
		goto exit;
	}

	dhcps_set_addr_pool(1,&start_ip,&end_ip);

	if(argv[3] != NULL){
		ip_addr = inet_addr(argv[3]);
		ap_ip[0] = (unsigned char) ip_addr & 0xff;
		ap_ip[1] = (unsigned char) (ip_addr >> 8) & 0xff;
		ap_ip[2] = (unsigned char) (ip_addr >> 16) & 0xff;
		ap_ip[3] = (unsigned char) (ip_addr >> 24) & 0xff;
	}
	else{
		//at_printf("\r\n[ATPF] ERROR : parameter format error");
		error_no = 2;
		goto exit;
	}

	ap_gw[0] = ap_ip[0];
	ap_gw[1] = ap_ip[1];
	ap_gw[2] = ap_ip[2];
	ap_gw[3] = ap_ip[3];

	ap_netmask[0] = 255;
	ap_netmask[1] = 255;
	ap_netmask[2] = 255;
	ap_netmask[3] = 0;

exit:
	if(error_no==0)
		at_printf("\r\n[ATPF] OK");
	else
		at_printf("\r\n[ATPF] ERROR:%d",error_no);

	return;
}

int atcmd_wifi_read_info_from_flash(u8 *read_data, u32 read_len)
{
	atcmd_update_partition_info(AT_PARTITION_WIFI, AT_PARTITION_READ, read_data, read_len);
	return 0;
}

void atcmd_wifi_write_info_to_flash(rtw_wifi_setting_t *setting, int enable)
{
	struct atcmd_wifi_conf *data_to_flash;
	rtw_wifi_setting_t *old_setting;

	flash_t flash;
	u32 channel = 0, i, write_needed = 0;
	u8 index = 0;
	u32 data;

	data_to_flash = (struct atcmd_wifi_conf *)malloc(sizeof(struct atcmd_wifi_conf));
	
	if(data_to_flash) {	
		if(enable){
			memset((u8 *)data_to_flash, 0, sizeof(struct atcmd_wifi_conf));
			atcmd_update_partition_info(AT_PARTITION_WIFI, AT_PARTITION_READ, (u8 *)data_to_flash, sizeof(struct atcmd_wifi_conf));
			old_setting = &(data_to_flash->setting);
			if(memcmp((u8 *)old_setting, setting, sizeof(rtw_wifi_setting_t))){
				memcpy(old_setting, setting, sizeof(rtw_wifi_setting_t));
				write_needed = 1;
			}
			if(setting->mode == RTW_MODE_STA || setting->mode == RTW_MODE_STA_AP){
				struct wlan_fast_reconnect reconn;
				int found = 0;
				/*clean wifi ssid,key and bssid*/
				memset((u8 *)&reconn, 0, sizeof(struct wlan_fast_reconnect));

				channel = setting->channel;

				memset(psk_essid[index], 0, sizeof(psk_essid[index]));
				strncpy(psk_essid[index], setting->ssid, strlen(setting->ssid));
				switch(setting->security_type){
					case RTW_SECURITY_OPEN:
						memset(psk_passphrase[index], 0, sizeof(psk_passphrase[index]));
						memset(wpa_global_PSK[index], 0, sizeof(wpa_global_PSK[index]));
						reconn.security_type = RTW_SECURITY_OPEN;
						break;
					case RTW_SECURITY_WEP_PSK:
						channel |= (setting->key_idx) << 28;
						memset(psk_passphrase[index], 0, sizeof(psk_passphrase[index]));
						memset(wpa_global_PSK[index], 0, sizeof(wpa_global_PSK[index]));
						memcpy(psk_passphrase[index], setting->password, sizeof(psk_passphrase[index]));
						reconn.security_type = RTW_SECURITY_WEP_PSK;
						break;
					case RTW_SECURITY_WPA_TKIP_PSK:
						reconn.security_type = RTW_SECURITY_WPA_TKIP_PSK;
						break;
					case RTW_SECURITY_WPA2_AES_PSK:
						reconn.security_type = RTW_SECURITY_WPA2_AES_PSK;
						break;
					default:
						break;
				}

				memcpy(reconn.psk_essid, psk_essid[index], sizeof(reconn.psk_essid));
				if (strlen(psk_passphrase64) == 64) {
					memcpy(reconn.psk_passphrase, psk_passphrase64, sizeof(reconn.psk_passphrase));
				} else {
					memcpy(reconn.psk_passphrase, psk_passphrase[index], sizeof(reconn.psk_passphrase));
				}
				memcpy(reconn.wpa_global_PSK, wpa_global_PSK[index], sizeof(reconn.wpa_global_PSK));
				memcpy(&(reconn.channel), &channel, 4);

				if(data_to_flash->reconn_num < 0 || data_to_flash->reconn_num > ATCMD_WIFI_CONN_STORE_MAX_NUM ||
					data_to_flash->reconn_last_index < 0 || data_to_flash->reconn_last_index > ATCMD_WIFI_CONN_STORE_MAX_NUM
				){
					data_to_flash->reconn_num = 0;
					data_to_flash->reconn_last_index = -1;
				}

				reconn.enable = enable;
				for(i = 0; i < data_to_flash->reconn_num; i++){
					if(memcmp((u8 *)&reconn, (u8 *)&(data_to_flash->reconn[i]), sizeof(struct wlan_fast_reconnect)) == 0) {
						AT_DBG_MSG(AT_FLAG_WIFI, AT_DBG_ALWAYS,
							"the same profile found in flash");
						found = 1;
						break;
					}
				}
				if(!found){
					data_to_flash->reconn_last_index++;
					if(data_to_flash->reconn_last_index >= ATCMD_WIFI_CONN_STORE_MAX_NUM)
						data_to_flash->reconn_last_index -= ATCMD_WIFI_CONN_STORE_MAX_NUM;
					memcpy((u8 *)&data_to_flash->reconn[data_to_flash->reconn_last_index], (u8 *)&reconn, sizeof(struct wlan_fast_reconnect));
					data_to_flash->reconn_num++;
					if(data_to_flash->reconn_num > ATCMD_WIFI_CONN_STORE_MAX_NUM)
						data_to_flash->reconn_num = ATCMD_WIFI_CONN_STORE_MAX_NUM;
					write_needed = 1;
				}
			}
			if(write_needed || data_to_flash->auto_enable != enable){
				data_to_flash->auto_enable = enable;
				atcmd_update_partition_info(AT_PARTITION_WIFI, AT_PARTITION_WRITE, (u8 *)data_to_flash, sizeof(struct atcmd_wifi_conf));
			}
		}else{
			atcmd_update_partition_info(AT_PARTITION_WIFI, AT_PARTITION_ERASE, (u8 *)data_to_flash, sizeof(struct atcmd_wifi_conf));
		}
	}
	if(data_to_flash) {
		free(data_to_flash);
	}
}

int atcmd_wifi_restore_from_flash(void)
{
	flash_t		flash;
	struct atcmd_wifi_conf *data;
	rtw_wifi_setting_t *setting;
	struct wlan_fast_reconnect *reconn;
	uint32_t	channel;
	uint32_t    security_type;
	uint8_t     pscan_config;
	char key_id[2] = {0};
	int ret = -1, i;
	int mode;
	rtw_network_info_t wifi = {
		{0},    // ssid
		{0},    // bssid
		0,      // security
		NULL,   // password
		0,      // password len
		-1      // key id
	};

	data = (struct atcmd_wifi_conf *)rtw_zmalloc(sizeof(struct atcmd_wifi_conf));
	if(data){
		atcmd_update_partition_info(AT_PARTITION_WIFI, AT_PARTITION_READ, (u8 *)data, sizeof(struct atcmd_wifi_conf));
		if(data->auto_enable != 1)
			goto exit;
		setting = &data->setting;
		if(setting->mode == RTW_MODE_AP || setting->mode == RTW_MODE_STA_AP){
			//start AP here
			goto exit;
		}
		
		//Check if in AP mode
		wext_get_mode(WLAN0_NAME, &mode);
		if(mode == IW_MODE_MASTER) {
#if CONFIG_LWIP_LAYER
			dhcps_deinit();
#endif
			wifi_off();
			rtw_mdelay_os(20);
			if (wifi_on(RTW_MODE_STA) < 0){
				printf("\n\rERROR: Wifi on failed!");
				ret = -1;
				goto exit;
			}
		}
		
#if CONFIG_AUTO_RECONNECT
		//setup reconnection flag
		wifi_set_autoreconnect(0);
#endif
		int last_index = data->reconn_last_index;
		for(i = 0; i < data->reconn_num; i++){
			reconn = &data->reconn[last_index];
			last_index ++;
			if(last_index >= ATCMD_WIFI_CONN_STORE_MAX_NUM)
				last_index -= ATCMD_WIFI_CONN_STORE_MAX_NUM;
			if(reconn->enable != 1){
				continue;
			}
			memcpy(psk_essid, reconn->psk_essid, sizeof(reconn->psk_essid));
			memcpy(psk_passphrase, reconn->psk_passphrase, sizeof(reconn->psk_passphrase));
			memcpy(wpa_global_PSK, reconn->wpa_global_PSK, sizeof(reconn->wpa_global_PSK));
			channel = reconn->channel;
			sprintf(key_id,"%d",(char) (channel>>28));
			channel &= 0xff;
			security_type = reconn->security_type;
			pscan_config = PSCAN_ENABLE | PSCAN_FAST_SURVEY;
			//set partial scan for entering to listen beacon quickly
			wifi_set_pscan_chan((uint8_t *)&channel, &pscan_config, 1);

			wifi.security_type = security_type;
			//SSID
			strcpy((char *)wifi.ssid.val, (char*)psk_essid);
			wifi.ssid.len = strlen((char*)psk_essid);

			switch(security_type){
				case RTW_SECURITY_WEP_PSK:
					wifi.password = (unsigned char*) psk_passphrase;
					wifi.password_len = strlen((char*)psk_passphrase);
					wifi.key_id = atoi((const char *)key_id);
					break;
				case RTW_SECURITY_WPA_TKIP_PSK:
				case RTW_SECURITY_WPA2_AES_PSK:
					wifi.password = (unsigned char*) psk_passphrase;
					wifi.password_len = strlen((char*)psk_passphrase);
					break;
				default:
					break;
			}

			ret = wifi_connect((char*)wifi.ssid.val, wifi.security_type, (char*)wifi.password, wifi.ssid.len,
				wifi.password_len, wifi.key_id, NULL);
			if(ret == RTW_SUCCESS){
				LwIP_DHCP(0, DHCP_START);
				ret = 0;
				break;
			}
		}
	}

exit:
	if(ret == 0)
		wifi_reg_event_handler(WIFI_EVENT_DISCONNECT, atcmd_wifi_disconn_hdl, NULL);
	if(data)
		rtw_mfree((u8 *)data, sizeof(struct wlan_fast_reconnect));
	return ret;
}

//ATPG=<enable>
void fATPG(void *arg)
{
	int argc, error_no = 0;
	char *argv[MAX_ARGC] = {0};
//	flash_t flash;
//	struct wlan_fast_reconnect read_data = {0};

	if(!arg){
		AT_DBG_MSG(AT_FLAG_WIFI, AT_DBG_ERROR,
			"\r\n[ATPG] Usage : ATPG=<enable>");
		error_no = 1;
		goto exit;
	}
	argc = parse_param(arg, argv);
	if(argc != 2){
		//at_printf("\r\n[ATPG] ERROR : command format error");
		error_no = 1;
		goto exit;
	}

	//ENABLE FAST CONNECT
	if(argv[1] != NULL){
#if 0
		device_mutex_lock(RT_DEV_LOCK_FLASH);
		flash_stream_read(&flash, FAST_RECONNECT_DATA, sizeof(struct wlan_fast_reconnect), (u8 *) &read_data);
		read_data.enable = atoi((const char *)(argv[1]));
		if(read_data.enable != 0 && read_data.enable != 1){
			//at_printf("\r\n[ATPG] ERROR : parameter must be 0 or 1");
			error_no = 2;
			device_mutex_unlock(RT_DEV_LOCK_FLASH);
			goto exit;
		}
		flash_erase_sector(&flash, FAST_RECONNECT_DATA);
		flash_stream_write(&flash, FAST_RECONNECT_DATA, sizeof(struct wlan_fast_reconnect), (u8 *) &read_data);
		device_mutex_unlock(RT_DEV_LOCK_FLASH);
#else
		rtw_wifi_setting_t setting;
		int enable = atoi((const char *)(argv[1]));
		if(enable != 0 && enable != 1){
			error_no = 2;
			goto exit;
		}
		if(enable == 1){
			u8 *ifname[1] = {WLAN0_NAME};
			if(wifi_get_setting((const char*)ifname[0],&setting)){
				AT_DBG_MSG(AT_FLAG_WIFI, AT_DBG_ERROR,
					"wifi_get_setting fail");
				error_no = 3;
				goto exit;
			}
		}
		atcmd_wifi_write_info_to_flash(&setting, enable);
#endif
	}

exit:
	if(error_no==0)
		at_printf("\r\n[ATPG] OK");
	else
		at_printf("\r\n[ATPG] ERROR:%d",error_no);

	return;
}

//ATPM=<mac>
void fATPM(void *arg)
{
	int argc, error_no = 0;
	char *argv[MAX_ARGC] = {0};

	if(!arg){
		AT_DBG_MSG(AT_FLAG_WIFI, AT_DBG_ERROR,
			"\r\n[ATPM] Usage : ATPM=<mac>");
		error_no = 1;
		goto exit;
	}

	argc = parse_param(arg, argv);
	if(argc != 2){
		//at_printf("\r\n[ATPM] ERROR : command format error");
		error_no = 1;
		goto exit;
	}

	if(argv[1] != NULL){
		if(strlen(argv[1]) != 12){
			//at_printf("\r\n[ATPM] ERROR : BSSID format error");
			error_no = 2;
			goto exit;
		}
		wifi_set_mac_address(argv[1]);
	}

exit:
	if(error_no==0)
		at_printf("\r\n[ATPM] OK");
	else
		at_printf("\r\n[ATPM] ERROR:%d",error_no);

	return;

}

//ATPW=<mode>
void fATPW(void *arg)
{
	int argc, error_no = 0;
	char *argv[MAX_ARGC] = {0};

	if(!arg){
		AT_DBG_MSG(AT_FLAG_WIFI, AT_DBG_ERROR,
			"\r\n[ATPW] Usage : ATPW=<mode>");
		error_no = 1;
		goto exit;
	}

	argc = parse_param(arg, argv);
	if(argc != 2){
		//at_printf("\r\n[ATPW] ERROR : command format error");
		error_no = 1;
		goto exit;
	}

	if(argv[1] != NULL){
		wifi_mode = atoi((const char *)(argv[1]));
		if((wifi_mode!=RTW_MODE_STA) && (wifi_mode!=RTW_MODE_AP) && (wifi_mode!=RTW_MODE_STA_AP) ){
			//at_printf("\r\n[ATPW] ERROR : parameter must be 1 , 2 or 3");
			error_no = 2;
		}
	}

exit:
	if(error_no==0)
		at_printf("\r\n[ATPW] OK");
	else
		at_printf("\r\n[ATPW] ERROR:%d",error_no);

	return;
}

void print_wlan_help(void *arg){
	at_printf("\r\nWLAN AT COMMAND SET:");
	at_printf("\r\n==============================");
	at_printf("\r\n1. Wlan Scan for Network Access Point");
	at_printf("\r\n   # ATWS");
	at_printf("\r\n2. Connect to an AES AP");
	at_printf("\r\n   # ATPN=<ssid>,<pwd>,<key_id>(,<bssid>)");
	at_printf("\r\n3. Create an AES AP");
	at_printf("\r\n   # ATPA=<ssid>,<pwd>,<chl>,<hidden>");
}

#endif // end of #if ATCMD_VER == ATVER_1

#endif // end of #if CONFIG_WLAN

#if CONFIG_LWIP_LAYER
#if ATCMD_VER == ATVER_1
void fATWL(void *arg){
#if CONFIG_SSL_CLIENT
	int argc;
	char *argv[MAX_ARGC] = {0};
	printf("[ATWL]: _AT_WLAN_SSL_CLIENT_\n\r"); 
	argv[0] = "ssl_client";
	if(!arg){
		printf("ATWL=SSL_SERVER_HOST\n\r");
		return;
	}
	if((argc = parse_param(arg, argv)) > 1){
		if(argc != 2) {
			printf("ATWL=SSL_SERVER_HOST\n\r");
			return;
		}

		cmd_ssl_client(argc, argv);
	}
#else
	printf("Please set CONFIG_SSL_CLIENT 1 in platform_opts.h to enable ATWL command\n");
#endif
}

void fATWI(void *arg){
	int argc;
	char *argv[MAX_ARGC] = {0};

	printf("[ATWI]: _AT_WLAN_PING_TEST_\n\r");

	if(!arg){
		printf("\n\r[ATWI] Usage: ATWI=[host],[options]\n");
		printf("\n\r     -t        Ping the specified host until stopped\n");
		printf("  \r     -n    #   Number of echo requests to send (default 4 times)\n");
		printf("  \r     -l    #   Send buffer size (default 32 bytes)\n");
		printf("\n\r   Example:\n");
		printf("  \r     ATWI=192.168.1.2,-n,100,-l,5000\n");
		return;
	}

	argv[0] = "ping";

	if((argc = parse_param(arg, argv)) > 1){
		cmd_ping(argc, argv);
	}
}

void fATWT(void *arg)
{
#if CONFIG_BSD_TCP
    int argc;
    char *argv[MAX_ARGC] = {0};

    printf("[ATWT]: _AT_WLAN_TCP_TEST_\n\r");

    if(!arg){
        printf("\n\r[ATWT] Usage: ATWT=[-s|-c,host|stop],[options]\n");
        printf("\n\r   Client/Server:\n");
        printf("  \r     stop           terminate client & server\n");
		printf("  \r     -i    #        seconds between periodic bandwidth reports\n");
        printf("  \r     -l    #        length of buffer to read or write (default 1460 Bytes)\n");
        printf("  \r     -p    #        server port to listen on/connect to (default 5001)\n");
        printf("\n\r   Server specific:\n");
        printf("  \r     -s             run in server mode\n");
        printf("\n\r   Client specific:\n");
        printf("  \r     -c    <host>   run in client mode, connecting to <host>\n");
        printf("  \r     -d             do a bidirectional test simultaneously\n");
        printf("  \r     -t    #        time in seconds to transmit for (default 10 secs)\n");
        printf("  \r     -n    #[KM]    number of bytes to transmit (instead of -t)\n");
        printf("\n\r   Example:\n");
        printf("  \r     ATWT=-s,-p,5002\n");
        printf("  \r     ATWT=-c,192.168.1.2,-t,100,-p,5002\n");
        return;
    }

    argv[0] = "tcp";

    if((argc = parse_param(arg, argv)) > 1){
        cmd_tcp(argc, argv);
    }
#else
	printf("Please set CONFIG_BSD_TCP 1 in platform_opts.h to enable ATWT command\n");
#endif
}

void fATWU(void *arg)
{
#if CONFIG_BSD_TCP
	int argc;
	char *argv[MAX_ARGC] = {0};

	printf("[ATWU]: _AT_WLAN_UDP_TEST_\n\r");

	if(!arg){
        printf("\n\r[ATWU] Usage: ATWU=[-s|-c,host|stop][options]\n");
        printf("\n\r   Client/Server:\n");
        printf("  \r     stop           terminate client & server\n");
		printf("  \r     -i    #        seconds between periodic bandwidth reports\n");
        printf("  \r     -l    #        length of buffer to read or write (default 1460 Bytes)\n");
        printf("  \r     -p    #        server port to listen on/connect to (default 5001)\n");
        printf("\n\r   Server specific:\n");
        printf("  \r     -s             run in server mode\n");
        printf("\n\r   Client specific:\n");
        printf("  \r     -b    #[KM]    for UDP, bandwidth to send at in bits/sec (default 1 Mbit/sec)\n");
        printf("  \r     -c    <host>   run in client mode, connecting to <host>\n");
        printf("  \r     -d             do a bidirectional test simultaneously\n");
        printf("  \r     -t    #        time in seconds to transmit for (default 10 secs)\n");
        printf("  \r     -n    #[KM]    number of bytes to transmit (instead of -t)\n");
        printf("  \r     -S    #        set the IP 'type of service'\n");
        printf("\n\r   Example:\n");
        printf("  \r     ATWU=-s,-p,5002\n");
        printf("  \r     ATWU=-c,192.168.1.2,-t,100,-p,5002\n");
	    return;
	}

	argv[0] = "udp";

	if((argc = parse_param(arg, argv)) > 1){
	    cmd_udp(argc, argv);
    }
#else
	printf("Please set CONFIG_BSD_TCP 1 in platform_opts.h to enable ATWU command\n");
#endif
}
#elif ATCMD_VER == ATVER_2 // uart at command
//move to atcmd_lwip.c
#endif
#endif
log_item_t at_wifi_items[ ] = {
#if ATCMD_VER == ATVER_1
#if CONFIG_LWIP_LAYER
	{"ATWL", fATWL,},
	{"ATWI", fATWI,}, 
	{"ATWT", fATWT,},
	{"ATWU", fATWU,},
#endif
#if WIFI_LOGO_CERTIFICATION_CONFIG
	{"ATPE", fATPE,}, // set static IP for STA
#endif
#if CONFIG_WLAN
	{"ATW0", fATW0,},
	{"ATW1", fATW1,},
	{"ATW2", fATW2,},
	{"ATW3", fATW3,},
	{"ATW4", fATW4,},
	{"ATW5", fATW5,},
	{"ATW6", fATW6,},	
	{"ATWA", fATWA,},
#ifdef  CONFIG_CONCURRENT_MODE
    {"ATWB", fATWB,},
#endif
	{"ATWC", fATWC,},
	{"ATWD", fATWD,},
	{"ATWP", fATWP,},
#if CONFIG_WOWLAN_SERVICE
	{"ATWV", fATWV,},
#endif	
	{"ATWR", fATWR,},
	{"ATWS", fATWS,},
#if SCAN_WITH_SSID
	{"ATWs", fATWs,},
#endif
#ifdef CONFIG_PROMISC
	{"ATWM", fATWM,},
#endif
    {"ATWZ", fATWZ,},
#if CONFIG_OTA_UPDATE
	{"ATWO", fATWO,},
#endif
#if CONFIG_WEBSERVER
	{"ATWE", fATWE,}, 
#endif
#if (CONFIG_INCLUDE_SIMPLE_CONFIG)	
	{"ATWQ", fATWQ,},
#endif	
#ifdef CONFIG_WPS	
	{"ATWW", fATWW,},
	{"ATWw", fATWw,}, //wps registrar for softap
#if CONFIG_ENABLE_P2P
	{"ATWG", fATWG,},  //p2p start
	{"ATWH", fATWH,},  //p2p stop
	{"ATWJ", fATWJ,},  //p2p connect
	{"ATWK", fATWK,},  //p2p disconnect
	{"ATWN", fATWN,},  //p2p info
	{"ATWF", fATWF,},  //p2p find
	{"ATWg", fATWg,},  //p2p auto go start
#endif
#endif

#if CONFIG_AIRKISS
	{"ATWX", fATWX,}, 
#endif
	{"ATW?", fATWx,},
	{"ATW+ABC", fATWx,},
#ifdef CONFIG_POWER_SAVING
	{"ATXP", fATXP,},
#endif
#endif
#elif ATCMD_VER == ATVER_2 // uart at command
#if CONFIG_WLAN
	{"ATPA", fATPA,}, // set AP
	{"ATPN", fATPN,}, // connect to Network
	{"ATPH", fATPH,}, // set DHCP mode
	{"ATPE", fATPE,}, // set static IP for STA
	{"ATPF", fATPF,}, // set DHCP rule for AP
	{"ATPG", fATPG,}, // set auto connect
	{"ATPM", fATPM,}, // set MAC address
	{"ATPW", fATPW,}, // set Wifi mode
	{"ATWD", fATWD,},
	{"ATWS", fATWS,},
	{"ATW?", fATWx,},
#if (CONFIG_INCLUDE_SIMPLE_CONFIG)
	{"ATWQ", fATWQ,},
#endif // #if (CONFIG_INCLUDE_SIMPLE_CONFIG)
#endif // #if CONFIG_WLAN
#endif // end of #if ATCMD_VER == ATVER_1
};

#if ATCMD_VER == ATVER_2
void print_wifi_at(void *arg){
	int index;
	int cmd_len = 0;

	cmd_len = sizeof(at_wifi_items)/sizeof(at_wifi_items[0]);
	for(index = 0; index < cmd_len; index++)
		at_printf("\r\n%s", at_wifi_items[index].log_cmd);
}
#endif

void at_wifi_init(void)
{
#if CONFIG_WLAN
	init_wifi_struct();
#endif
	log_service_add_table(at_wifi_items, sizeof(at_wifi_items)/sizeof(at_wifi_items[0]));
}

#if SUPPORT_LOG_SERVICE
log_module_init(at_wifi_init);
#endif
