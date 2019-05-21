
#if CFG_SUPPORT_ALIOS
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <aos/aos.h>

#include "hal/soc/soc.h"
#include <hal/wifi.h>
#include "ble_pub.h"
#include "vif_mgmt.h"
#include "bk7011_cal_pub.h"

#define TO_MEDIA_GENERAL_BIN            0x01
#define TO_MEDIA_ONLY_AP_BIN            0x02
#define TO_MEDIA_ONLY_STA_BIN           0x03
#define TO_MEDIA_ONLY_BLE_BIN           0x04
#define TO_MEDIA_AP_BLE_BIN             0x05
#define TO_MEDIA_STA_BLE_BIN            0x06
#define TO_MEDIA_AP_BLE_IDLE_BIN        0x07
#define TO_MEDIA_STA_BLE_IDLE_BIN       0x08

#define TO_MEDIA_BIN                    (TO_MEDIA_GENERAL_BIN)

static void softap_Command(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv);
static void sta_Command(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv);
static void ble_entry_Command(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv);
static void ap_get_info_Command(void);
static void sta_get_info_Command(void);
static void ble_get_info_Command(void);
static void linkkit_start_only_ap(void);
static void linkkit_start_only_sta(void);
static void linkkit_start_only_ble(void) ;

static const struct cli_command user_cmd[] = {
    {"softap", "softap ssid key", softap_Command},      
    {"sta", "sta ap_ssid key", sta_Command},    
    {"ble", "ble help/active/dut", ble_entry_Command},
};

#define NAME_DIF_LEN            4
#define MAX_AP_SSID_LEN         28
char linkkit_ap_ssid[MAX_AP_SSID_LEN + NAME_DIF_LEN] = {0};

#define MAX_BLE_NAME_LEN        9
char linkkit_ble_name[MAX_BLE_NAME_LEN + NAME_DIF_LEN] = {0};

char name_diff[NAME_DIF_LEN+1] = {0};

static void linkkit_generate_name_diff(void)
{
    uint8_t mac_addr[6], *ptr; 
    
    wifi_get_mac_address((char *)&mac_addr, 2);  // get sta's mac addr 

    ptr = &mac_addr[4];
    *ptr = ((*ptr & 0x0f) << 4) | ((*ptr & 0xf0) >> 4);
    sprintf(&name_diff[0], "%02x", *ptr);
    
    ptr = &mac_addr[5];
    *ptr = ((*ptr & 0x0f) << 4) | ((*ptr & 0xf0) >> 4);
    sprintf(&name_diff[2], "%02x", *ptr);
    
    name_diff[4] = '\0';

    aos_cli_printf("name_diff:%s\r\n", name_diff);
}

static void linkkit_generate_ap_ssid(char *org_ssid)
{
    uint32 len = 0;
    
    len = strlen(org_ssid);
    len = (len > MAX_AP_SSID_LEN) ? MAX_AP_SSID_LEN : len;

    os_memcpy(linkkit_ap_ssid, org_ssid, len);
    os_memcpy(&linkkit_ap_ssid[len], name_diff, NAME_DIF_LEN);
}

static void linkkit_generate_ble_name(char *org_name)
{
    uint32 len = 0;
        
    len = strlen(org_name);
    len = (len > MAX_BLE_NAME_LEN) ? MAX_BLE_NAME_LEN : len;

    os_memcpy(linkkit_ble_name, org_name, len);
    os_memcpy(&linkkit_ble_name[len], &name_diff[1], NAME_DIF_LEN-1);
}

static void linkkit_wlan_start_ap(const char *ap_ssid, int ap_key)
{
#define LINK_WLAN_DEFAULT_IP         "192.168.0.1"
#define LINK_WLAN_DEFAULT_GW         "192.168.0.1"
#define LINK_WLAN_DEFAULT_MASK       "255.255.255.0"
    
    hal_wifi_init_type_t type;
    int len;
    
    os_memset(&type, 0x0, sizeof(hal_wifi_init_type_t));

    len = os_strlen(ap_ssid);
    if(32 < len)
    {
        bk_printf("ssid name more than 32 Bytes\r\n");
        return;
    }

    os_strcpy((char *)type.wifi_ssid, ap_ssid);
    os_strcpy((char *)type.wifi_key, ap_key);
    
    type.wifi_mode = SOFT_AP;
    type.dhcp_mode = DHCP_SERVER;
    type.wifi_retry_interval = 100;
    os_strcpy((char *)type.local_ip_addr, LINK_WLAN_DEFAULT_IP);
    os_strcpy((char *)type.net_mask, LINK_WLAN_DEFAULT_MASK);
    os_strcpy((char *)type.dns_server_ip_addr, LINK_WLAN_DEFAULT_GW);
    
    aos_cli_printf("ssid:%s  key:%s\r\n", type.wifi_ssid, type.wifi_key);
    bk_wlan_start(&type);
}

static void linkkit_softap_usage(void)
{
    aos_cli_printf("softap help           - Help information\n");
    aos_cli_printf("softap info           - list softap info\n");
    aos_cli_printf("softap [ssid] [key]   - start wlan with softap with ssdi+key\n"); 
    aos_cli_printf("softap                - start wlan with BK7231AP-V1-xxxx,no key\r\n");     
}

static void softap_Command(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    char *ap_ssid = NULL;
    char *ap_key;

    aos_cli_printf("SOFTAP_COMMAND\r\n\r\n");
    if (argc == 2)
    {
        if(!os_strcasecmp(argv[1], "info")) {
            ap_get_info_Command();
            return;            
        } else if(!os_strcasecmp(argv[1], "help")){
            linkkit_softap_usage();
            return;
        }
        
        ap_ssid = argv[1];
        ap_key = "1";
    }
    else if (argc == 3)
    {
        ap_ssid = argv[1];
        ap_key = argv[2];
    } else if (argc == 1) {
        linkkit_start_only_ap();
    }

    if(ap_ssid)
    {
        linkkit_wlan_start_ap(ap_ssid, ap_key);
    }
}

static void linkkit_wlan_connect_ap(const char *oob_ssid, int connect_key)
{   
    hal_wifi_init_type_t type;

    int len;
	os_memset(&type, 0x0, sizeof(hal_wifi_init_type_t));

    len = os_strlen(oob_ssid);
    if(32 < len)
    {
        bk_printf("ssid name more than 32 Bytes\r\n");
        return;
    }
    
	os_strcpy((char *)type.wifi_ssid, oob_ssid);
	os_strcpy((char *)type.wifi_key, connect_key);
    
	type.wifi_mode = STATION;
	type.dhcp_mode = DHCP_CLIENT;
	type.wifi_retry_interval = 100;
    
	bk_printf("ssid:%s key:%s\r\n", type.wifi_ssid, type.wifi_key);
	bk_wlan_start(&type);
}

static void linkkit_sta_usage(void)
{
    aos_cli_printf("sta help           - Help information\n");
    aos_cli_printf("sta info           - list sta info\n");
    aos_cli_printf("sta [ssid] [key]   - start wlan as sta to connect to router with ssdi+key\n");
    aos_cli_printf("sta                - start wlan as sta to connect BK7231STA-V1,no key\n");    
}

static void sta_Command(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    char *oob_ssid = NULL;
    char *connect_key;

    aos_cli_printf("sta_Command\r\n");
    if (argc == 2)
    {
        if(!os_strcasecmp(argv[1], "info")) {
            sta_get_info_Command();
            return;            
        } else if(!os_strcasecmp(argv[1], "help")){
            linkkit_sta_usage();
            return;
        }

        oob_ssid = argv[1];
        connect_key = "1";
    }
    else if (argc == 3)
    {
        oob_ssid = argv[1];
        connect_key = argv[2];
    }
    else if (argc == 1) {
        linkkit_start_only_sta();
    }
    else
    {
        aos_cli_printf("parameter invalid\r\n");
    }

    if(oob_ssid)
    {
        linkkit_wlan_connect_ap(oob_ssid, connect_key);
    }
}

static void linkkit_ble_usage(void)
{
    aos_cli_printf("ble help           - Help information\n");
    aos_cli_printf("ble active [name]  - Active ble to with name\n");
    aos_cli_printf("ble active         - Active ble to with BK7231BTxxx\n");    
    aos_cli_printf("ble dut            - Active ble to do BLE DUT\n");
    aos_cli_printf("ble info           - get ble app info\n");     
}

static void ble_entry_Command(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    if ((argc == 1) || (strcmp(argv[1], "help") == 0))
    {
        linkkit_ble_usage();
        return 0;
    }
	else if (strcmp(argv[1], "active") == 0)
    {
        char *name = NULL;
        if(argc == 3) {
            name = argv[2];
            ble_activate(name);
        }else if(argc == 2){
            linkkit_start_only_ble();
        }
		
    }
    else if (strcmp(argv[1], "dut") == 0)
    {
		ble_dut_start();
    }
    else if (strcmp(argv[1], "info") == 0)
    {
		ble_get_info_Command();
    }
	
	return 0;
}

int linkkit_cli_init(void)
{
    int ret;

    if (aos_cli_register_commands(&user_cmd[0],
                              sizeof(user_cmd) / sizeof(struct cli_command)))
    {
        aos_cli_printf("linkkit_cli_init error!!!\r\n");
        return -1;
    }

    return 0;
}

void rwn_mgmt_show_vif_peer_sta_list(UINT8 role);
static void ap_get_info_Command(void)
{
    aos_cli_printf("\r\n***********softap information**********\r\n");
    if (bk_wlan_has_role(VIF_AP) == 0) {
        aos_cli_printf("no softap startup          \r\n");
        return;
    }
        
    aos_cli_printf("* channel     : %d, ch%2d      \r\n", bk_wlan_ap_get_frequency(), bk_wlan_ap_get_channel_config());
    aos_cli_printf("* mode        : 802.11b/g/n-Band:20MHZ \r\n");  
    aos_cli_printf("* tx power    : %d dbm         \r\n", manual_cal_get_cur_txpwr_dbm());
    aos_cli_printf("* country code: CN(2412-2472)  \r\n");
    aos_cli_printf("* sta list:                    \r\n"); 

    rwn_mgmt_show_vif_peer_sta_list(VIF_AP);
    aos_cli_printf("****************** end ****************\r\n");     
}

static void sta_get_info_Command(void)
{
    aos_cli_printf("\r\n********** sta information ************\r\n");
    if (bk_wlan_has_role(VIF_STA) == 0) {
        aos_cli_printf("no sta startup          \r\n");
        return;
    }
        
    aos_cli_printf("* channel     : %d, ch%2d      \r\n", bk_wlan_sta_get_frequency(), bk_wlan_sta_get_channel());
    aos_cli_printf("* mode        : 802.11b/g/n-Band:20MHZ \r\n");  
    aos_cli_printf("* tx power    : %d dbm       \r\n",  manual_cal_get_cur_txpwr_dbm());
    aos_cli_printf("* country code: CN(2412-2472)  \r\n");
    aos_cli_printf("* router info:                 \r\n"); 

    rwn_mgmt_show_vif_peer_sta_list(VIF_STA);
    aos_cli_printf("****************** end ****************\r\n");     
}

static void ble_get_info_Command(void)
{
    UINT8 *ble_mac;
    aos_cli_printf("\r\n****** ble information ************\r\n");

    if (ble_is_start() == 0) {
        aos_cli_printf("no ble startup          \r\n");
        return;
    }
    ble_mac = ble_get_mac();    
    aos_cli_printf("* name: %s             \r\n", ble_get_name());
    aos_cli_printf("* mac:%02x-%02x-%02x-%02x-%02x-%02x\r\n", ble_mac[0], ble_mac[1],ble_mac[2],ble_mac[3],ble_mac[4],ble_mac[5]);  
    aos_cli_printf("***********  end  *****************\r\n");     
}

static void linkkit_start_only_ap(void) 
{
    #define ONLY_AP_SSID         "BK7231AP-V1-"
    #define ONLY_AP_KEY          "1"  // no encryption

    linkkit_generate_ap_ssid(ONLY_AP_SSID);

    linkkit_wlan_start_ap(linkkit_ap_ssid, ONLY_AP_KEY);
}

static void linkkit_start_only_sta(void) 
{
    #define ONLY_STA_SSID         "BK7231STA-V1"
    #define ONLY_STA_KEY          "1" // no encryption

    linkkit_wlan_connect_ap(ONLY_STA_SSID, ONLY_STA_KEY);
}

static void linkkit_start_only_ble(void) 
{
    #define ONLY_BLE_NAME         "BK7231BT-"

    linkkit_generate_ble_name(ONLY_BLE_NAME);
    
    ble_activate(linkkit_ble_name);
}

static void linkkit_start_ap_ble(void) 
{
    #define AP_SSID         "BK7231AP-V2-"
    #define AP_KEY          "1"  // no encryption
    #define BLE_NAME        "BK7231BT"
    uint32 len = 0;

    linkkit_generate_ap_ssid(AP_SSID);
    linkkit_wlan_start_ap(linkkit_ap_ssid, AP_KEY);

    linkkit_generate_ble_name(BLE_NAME);
    
    ble_activate(linkkit_ble_name);    
}

static void linkkit_start_sta_ble(void) 
{
    #define STA_SSID         "BK7231STA-V2"
    #define STA_KEY          "1" // no encryption
    #define BLE_NAME        "BK7231BT"
    uint32 len = 0;
    
    linkkit_wlan_connect_ap(STA_SSID, STA_KEY);
    
    linkkit_generate_ble_name(BLE_NAME);
    
    ble_activate(linkkit_ble_name);  
}



static void linkkit_start_ap_ble_idle(void) 
{
    aos_cli_printf("Please implement ap_ble_idle\r\n");
}



static void linkkit_start_sta_ble_idle(void) 
{
    aos_cli_printf("Please implement sta_ble_idle\r\n");
}


int linkkit_application_start(void)
{
    linkkit_generate_name_diff();
    
    #if (TO_MEDIA_BIN == TO_MEDIA_ONLY_AP_BIN) 
    linkkit_start_only_ap();
    #elif (TO_MEDIA_BIN == TO_MEDIA_ONLY_STA_BIN)
    linkkit_start_only_sta();
    #elif (TO_MEDIA_BIN == TO_MEDIA_ONLY_BLE_BIN)
    linkkit_start_only_ble();
    #elif (TO_MEDIA_BIN == TO_MEDIA_AP_BLE_BIN)
    linkkit_start_ap_ble();
    #elif (TO_MEDIA_BIN == TO_MEDIA_STA_BLE_BIN)  
    linkkit_start_sta_ble();
    #elif (TO_MEDIA_BIN == TO_MEDIA_AP_BLE_IDLE_BIN)
    linkkit_start_ap_ble_idle();
    #elif (TO_MEDIA_BIN == TO_MEDIA_STA_BLE_IDLE_BIN)
    linkkit_start_sta_ble_idle();
    #else
    aos_cli_printf("select general bin for media\r\n");
    #endif

    linkkit_cli_init();
    
    return 0;
}

#endif

