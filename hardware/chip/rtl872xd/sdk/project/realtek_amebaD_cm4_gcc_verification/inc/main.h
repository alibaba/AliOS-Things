#ifndef MAIN_H
#define MAIN_H

#include <autoconf.h>
#include "platform_stdlib.h"

#ifndef CONFIG_WLAN
#define CONFIG_WLAN	1
#endif


/* Header file declaration*/
void wlan_network(void);
extern int mbedtls_platform_set_calloc_free( 
	void * (*calloc_func)( unsigned int, unsigned int ),
	void (*free_func)( void * )
);
extern void SystemSetCpuClk(unsigned char CpuClk);
extern void rtw_efuse_boot_write(void);
extern void pre_example_entry(void);
extern void example_entry(void);

/* Interactive Mode */
#define SERIAL_DEBUG_RX 1

/* WLAN and Netork */
#define STA_MODE_SSID	"ap"    /* Set SSID here */
#define AP_MODE_SSID		"wlan_ap_ssid"    /* Set SSID here */
#define AP_DEFAULT_CH	6
#define WLAN0_NAME		"wlan0"
#define WLAN1_NAME      	"wlan1"
#define WPA_PASSPHRASE	"1234567890" /* Max 32 cahracters */
#define WEP40_KEY		{0x12, 0x34, 0x56, 0x78, 0x90}

#define ATVER_1 1 // For First AT command
#define ATVER_2 2 // For UART Module AT command

#if CONFIG_EXAMPLE_UART_ATCMD
#define ATCMD_VER ATVER_2
#else
#define ATCMD_VER ATVER_1
#endif

#if ATCMD_VER == ATVER_2
#undef CONFIG_EXAMPLE_WLAN_FAST_CONNECT
#define CONFIG_EXAMPLE_WLAN_FAST_CONNECT	1

extern unsigned char sta_ip[4], sta_netmask[4], sta_gw[4];
extern unsigned char ap_ip[4], ap_netmask[4], ap_gw[4];

/*Static IP ADDRESS*/
#define IP_ADDR0   sta_ip[0]
#define IP_ADDR1   sta_ip[1]
#define IP_ADDR2   sta_ip[2]
#define IP_ADDR3   sta_ip[3]

/*NETMASK*/
#define NETMASK_ADDR0   sta_netmask[0]
#define NETMASK_ADDR1   sta_netmask[1]
#define NETMASK_ADDR2   sta_netmask[2]
#define NETMASK_ADDR3   sta_netmask[3]

/*Gateway Address*/
#define GW_ADDR0   sta_gw[0]
#define GW_ADDR1   sta_gw[1]
#define GW_ADDR2   sta_gw[2]
#define GW_ADDR3   sta_gw[3]

/*******************************************/

/*Static IP ADDRESS*/
#define AP_IP_ADDR0   ap_ip[0]
#define AP_IP_ADDR1   ap_ip[1]
#define AP_IP_ADDR2   ap_ip[2]
#define AP_IP_ADDR3   ap_ip[3]

/*NETMASK*/
#define AP_NETMASK_ADDR0   ap_netmask[0]
#define AP_NETMASK_ADDR1   ap_netmask[1]
#define AP_NETMASK_ADDR2   ap_netmask[2]
#define AP_NETMASK_ADDR3   ap_netmask[3]

/*Gateway Address*/
#define AP_GW_ADDR0   ap_gw[0]
#define AP_GW_ADDR1   ap_gw[1]
#define AP_GW_ADDR2   ap_gw[2]
#define AP_GW_ADDR3   ap_gw[3]

#else

/*Static IP ADDRESS*/
#define IP_ADDR0   192
#define IP_ADDR1   168
#define IP_ADDR2   1
#define IP_ADDR3   80

/*NETMASK*/
#define NETMASK_ADDR0   255
#define NETMASK_ADDR1   255
#define NETMASK_ADDR2   255
#define NETMASK_ADDR3   0

/*Gateway Address*/
#define GW_ADDR0   192
#define GW_ADDR1   168
#define GW_ADDR2   1
#define GW_ADDR3   1

/*******************************************/

/*Static IP ADDRESS*/
#define AP_IP_ADDR0   192
#define AP_IP_ADDR1   168
#define AP_IP_ADDR2   43
#define AP_IP_ADDR3   1
   
/*NETMASK*/
#define AP_NETMASK_ADDR0   255
#define AP_NETMASK_ADDR1   255
#define AP_NETMASK_ADDR2   255
#define AP_NETMASK_ADDR3   0

/*Gateway Address*/
#define AP_GW_ADDR0   192
#define AP_GW_ADDR1   168
#define AP_GW_ADDR2   43
#define AP_GW_ADDR3   1  

#endif //#if ATCMD_VER == ATVER_2


#endif
