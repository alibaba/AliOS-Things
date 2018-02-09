#include <stdio.h>
#include "log_service.h"
#include "platform_opts.h"
#include <lwip_netconf.h>
#include "cmsis_os.h"
#include <platform/platform_stdlib.h>
#include <lwip/sockets.h>
#include <lwip/tcpip.h>


#define	_AT_DHCP_ETHERNET_MII_     "ATE0"
#define	_AT_SET_DEFAULT_INTERFACE  "ATE1"

#if CONFIG_ETHERNET
extern int dhcp_ethernet_mii;
extern int ethernet_if_default;
extern struct netif xnetif[NET_IF_NUM];

void fATE0(void *arg)
{
	int argc;
	char *argv[MAX_ARGC] = {0};
	printf("[ATE0]:DHCP configure for ethernet\n\r");
	if(!arg){
		printf("[ATE0]Usage to disable DHCP: ATE0=0\n");
		printf("[ATE0]Usage to enable DHCP: ATE0=1\n");
		return;
	}
	if('0' == *(char *)arg)
	{
		dhcp_ethernet_mii = 0;
	}
		
	else if('1' == *(char *)arg)
	{
		dhcp_ethernet_mii = 1;
		LwIP_DHCP(NET_IF_NUM - 1, DHCP_START);
	}
	
	else
	{
		printf("[ATE0]Usage to disable DHCP: ATE0=0\n");
		printf("[ATE0]Usage to enable DHCP: ATE0=1\n");
	}
}

void fATE1(void *arg)
{
	int argc;
	char *argv[MAX_ARGC] = {0};
	printf("[ATE1]:Set/check the default interface\n\r");
	if(!arg){
		if(ethernet_if_default)
			printf("Ethernet is the default interface\n");
		else
			printf("wlan is the default interface\n");
		return;
	}
	if('0' == *(char *)arg)
	{
		ethernet_if_default = 0;
		printf("wlan is set to the default interface\n");
	}
		
	else if('1' == *(char *)arg)
	{
		ethernet_if_default = 1;
		printf("ethernet is set to the default interface\n");
	}
	
	else
	{
		printf("[ATE0]Usage to check the default interface: ATE1\n");
		printf("[ATE0]Usage to set ethernet as default interface: ATE1=1\n");
		printf("[ATE0]Usage to set wlan as default interface: ATE1=0\n");
	}
}

log_item_t at_ethernet_items[ ] = {
	{"ATE0", fATE0,},
	{"ATE1", fATE1,}
};

void at_ethernet_init(void)
{
	log_service_add_table(at_ethernet_items, sizeof(at_ethernet_items)/sizeof(at_ethernet_items[0]));
}

log_module_init(at_ethernet_init);

#endif