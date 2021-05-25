#include "FreeRTOS.h"
#include "task.h"

#include "utils/os.h"
#include <lwip/netif.h>
#include <wifi/wifi_conf.h>
#include "wps/wps_defs.h"

#if CONFIG_ENABLE_P2P
enum p2p_wps_method {
        WPS_NOT_READY, WPS_PIN_DISPLAY, WPS_PIN_KEYPAD, WPS_PBC
};

/*NETMASK*/
#define P2P_NETMASK_ADDR0   255
#define P2P_NETMASK_ADDR1   255
#define P2P_NETMASK_ADDR2   255
#define P2P_NETMASK_ADDR3   0

/*Gateway Address*/
#define P2P_GW_ADDR0   192
#define P2P_GW_ADDR1   168
#define P2P_GW_ADDR2   42
#define P2P_GW_ADDR3   1  

#define P2P_GO_NEGO_RESULT_SIZE	376//256

xqueue_handle_t queue_for_p2p_nego;

extern void dhcps_init(struct netif * pnetif);

static int hex2num(char c)
{
	if (c >= '0' && c <= '9')
		return c - '0';
	if (c >= 'a' && c <= 'f')
		return c - 'a' + 10;
	if (c >= 'A' && c <= 'F')
		return c - 'A' + 10;
	return -1;
}

/**
 * hwaddr_aton - Convert ASCII string to MAC address (colon-delimited format)
 * @txt: MAC address as a string (e.g., "00:11:22:33:44:55")
 * @addr: Buffer for the MAC address (ETH_ALEN = 6 bytes)
 * Returns: 0 on success, -1 on failure (e.g., string not a MAC address)
 */
int hwaddr_aton(const char *txt, u8 *addr)
{
	int i;

	for (i = 0; i < 6; i++) {
		int a, b;

		a = hex2num(*txt++);
		if (a < 0)
			return -1;
		b = hex2num(*txt++);
		if (b < 0)
			return -1;
		*addr++ = (a << 4) | b;
		if (i < 5 && *txt++ != ':')
			return -1;
	}

	return 0;
}

int wifi_start_p2p_go(char *ssid, char *passphrase, u8 channel)
{
	extern struct netif xnetif[NET_IF_NUM];
	struct netif * pnetif = &xnetif[0];
	struct ip_addr ipaddr;
	struct ip_addr netmask;
	struct ip_addr gw;

#if LWIP_VERSION_MAJOR >= 2
	IP4_ADDR(ip_2_ip4(&ipaddr), P2P_GW_ADDR0, P2P_GW_ADDR1, P2P_GW_ADDR2, P2P_GW_ADDR3);
	IP4_ADDR(ip_2_ip4(&netmask), P2P_NETMASK_ADDR0, P2P_NETMASK_ADDR1 , P2P_NETMASK_ADDR2, P2P_NETMASK_ADDR3);
	IP4_ADDR(ip_2_ip4(&gw), P2P_GW_ADDR0, P2P_GW_ADDR1, P2P_GW_ADDR2, P2P_GW_ADDR3);
	netif_set_addr(pnetif, ip_2_ip4(&ipaddr), ip_2_ip4(&netmask),ip_2_ip4(&gw));
#else
	IP4_ADDR(&ipaddr, P2P_GW_ADDR0, P2P_GW_ADDR1, P2P_GW_ADDR2, P2P_GW_ADDR3);
	IP4_ADDR(&netmask, P2P_NETMASK_ADDR0, P2P_NETMASK_ADDR1 , P2P_NETMASK_ADDR2, P2P_NETMASK_ADDR3);
	IP4_ADDR(&gw, P2P_GW_ADDR0, P2P_GW_ADDR1, P2P_GW_ADDR2, P2P_GW_ADDR3);
	netif_set_addr(pnetif, &ipaddr, &netmask,&gw);
#endif
	
	// start ap
	if(wifi_start_ap(ssid,
						 RTW_SECURITY_WPA2_AES_PSK,
						 passphrase,
						 strlen(ssid),
						 strlen(passphrase),
						 channel
						 ) != RTW_SUCCESS) {
		printf("\n\rERROR: Operation failed!");
		return -1;
	}

	netif_set_default(pnetif);
	
	// start dhcp server
	dhcps_init(pnetif);

	return 0;
}

void app_callback(char *msg)
{
	//From Application
}

void cmd_wifi_p2p_start(int argc, char **argv)
{
	extern struct netif xnetif[NET_IF_NUM];
	int listen_ch = 1;
	int op_ch = 5;
	int go_intent = 1;
#if 1	
	u32 r = 0;
	os_get_random((u8 *) &r, sizeof(r));
	go_intent = r%15+1; /*1-15*/
	
	os_get_random((u8 *) &r, sizeof(r));
	listen_ch = 1 + (r % 3) * 5;
	
	os_get_random((u8 *) &r, sizeof(r));
	op_ch = 1 + (r % 3) * 5;
#endif	
	wifi_off();
	os_sleep(0, 20000);
	wifi_on(RTW_MODE_P2P);
	wifi_p2p_init(xnetif[0].hwaddr, go_intent, listen_ch, op_ch);	
}

int cmd_wifi_p2p_auto_go_start(int argc, char **argv)
{
	u8 *passphrase = "12345678";
	u8 channel = 6;	// 1, 6, 11
	const char *ssid_in = "DIRECT-34-Ameba";
	const char *dev_name = "Ameba1234";	// max strlen 32
	const char *manufacturer = "by customer";	// max strlen 64
	const char *model_name = "customer";	// max strlen 32
	const char *model_number = "v2.0";	// max strlen 32
	const char *serial_number = "9";	// max strlen 32
	const u8 pri_dev_type[8] = {0x00,0x0A,0x00,0x50,0xF2,0x04,0x00,0x01};	// category ID:0x00,0x0A; sub category ID:0x00,0x01
	u8 res[P2P_GO_NEGO_RESULT_SIZE];
	u16 config_methods = WPS_CONFIG_DISPLAY | WPS_CONFIG_KEYPAD | WPS_CONFIG_PUSHBUTTON;

	if(!is_wifi_p2p_initialized())
		return -1;	

	wifi_p2p_set_dev_name(dev_name);
	wifi_p2p_set_manufacturer(manufacturer);
	wifi_p2p_set_model_name(model_name);
	wifi_p2p_set_model_number(model_number);
	wifi_p2p_set_serial_number(serial_number);
	wifi_p2p_set_pri_dev_type(pri_dev_type);
	wifi_p2p_set_ssid(ssid_in);
	wifi_p2p_set_config_methods(config_methods);
	wifi_p2p_init_auto_go_params(res, passphrase, channel);
	wifi_p2p_start_auto_go(res);
	return 0;
}
void cmd_wifi_p2p_stop(int argc, char **argv)
{
	wifi_p2p_deinit();
	wifi_off();
}

void cmd_p2p_listen(int argc, char **argv)
{
	u32 timeout = 0;

	if(argc == 2){
		timeout = os_atoi((u8*)argv[1]);
		printf("\r\n%s(): timeout=%d\n", __func__, timeout);
		if(timeout > 3600)
			timeout = 3600;
	}
	wifi_cmd_p2p_listen(timeout);
}

void cmd_p2p_find(int argc, char **argv)
{
	wifi_cmd_p2p_find();
}

void cmd_p2p_peers(int argc, char **argv)
{
	wifi_cmd_p2p_peers();
}

void cmd_p2p_info(int argc, char **argv)
{
	wifi_cmd_p2p_info();
}

void cmd_p2p_disconnect(int argc, char **argv)
{
	wifi_cmd_p2p_disconnect();
}

void cmd_p2p_connect(int argc, char **argv)
{
	enum p2p_wps_method config_method = WPS_PBC;
	char *pin = NULL;
	u8 dest[ETH_ALEN] = {0x44, 0x6d, 0x57, 0xd7, 0xce, 0x41};
	u8 res[P2P_GO_NEGO_RESULT_SIZE];
	int ret = 0, result = 0;

#if 1
	if((argc != 2) && (argc != 3) && (argc != 4)) {
		printf("\n\rUsage: p2p_connect DEST_ADDR [pbc|pin] [pin code]\n");
		printf("\n\rExample: p2p_connect 00:e0:4c:87:00:15 pin 12345678\n");
		return;
	}
	if (hwaddr_aton(argv[1], dest)){
		printf("\r\nP2P_CONNECT: dest address is not correct!\n");
		return;
	}
	
	//printf("\r\nDEST: %2.2x:%2.2x:%2.2x:%2.2x:%2.2x:%2.2x\n", dest[0], dest[1], dest[2], dest[3], dest[4], dest[5]);
	config_method = WPS_PBC;
	if(argc == 3) {
		if(os_strncmp(argv[2], "pbc", 3) == 0)
			config_method = WPS_PBC;
		else if(os_strncmp(argv[2], "pin", 3) == 0){
			config_method = WPS_PIN_DISPLAY;
		}else{
			printf("\n\rUnknown config method!\n");
			printf("\n\rUsage: p2p_connect DEST_ADDR [pbc|pin] \n");
			printf("\n\rExample: p2p_connect 00:e0:4c:87:00:15 pin\n");
			return;
		}
	}
	else if(argc == 4) {
		if(os_strncmp(argv[2], "pin", 3) == 0){
			config_method = WPS_PIN_KEYPAD;
			pin = argv[3];
		}else{
			printf("\n\rUnknown config method!\n");
			printf("\n\rUsage: p2p_connect DEST_ADDR [pbc|pin] [pin code]\n");
			printf("\n\rExample: p2p_connect 00:e0:4c:87:00:15 pin 12345678\n");
			return;
		}
	}
#else //For test
	u8 dest1[ETH_ALEN] = {0xea, 0x92, 0xa4, 0x9b, 0x61, 0xd6};  //NEXUS 4
	//u8 dest1[ETH_ALEN] = {0x0e, 0x37, 0xdc, 0xfc, 0xc4, 0x12}; //HUAWEI U9508_c001
	//u8 dest1[ETH_ALEN] = {0x42, 0xcb, 0xa8, 0xd3, 0x2c, 0x50}; //HUAWEI G610-T00
	os_memcpy(dest, dest1, ETH_ALEN);
	config_method = WPS_PBC;
#endif

	if (queue_for_p2p_nego!= NULL) {
		os_xqueue_delete(queue_for_p2p_nego);
		queue_for_p2p_nego = NULL;
	}
	queue_for_p2p_nego = os_xqueue_create(1, P2P_GO_NEGO_RESULT_SIZE);
	if(queue_for_p2p_nego != NULL) {
		ret = wifi_cmd_p2p_connect(dest, config_method, pin);
		if(ret == 0)
			result = os_xqueue_receive(queue_for_p2p_nego, res, 15);

		os_xqueue_delete(queue_for_p2p_nego);
		queue_for_p2p_nego = NULL;

		if((ret == 0) && (result == 0))
			wifi_p2p_start_wps(res);
	}
}

#endif //CONFIG_ENABLE_P2P
