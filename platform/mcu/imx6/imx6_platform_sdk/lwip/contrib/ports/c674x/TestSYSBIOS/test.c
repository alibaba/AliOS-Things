/************************************************************************/
/**	\file	test.c
 *	\brief	Test Thread for LwIP test
 *
 *	\date	2011/10/04
 *	\author	Stephane Lesage / Pierre Audenard for LwIP
 *
 */
/************************************************************************/

/* Includes */
#include <string.h>
#include "c674emac.h"
#include "c674phy.h"
#include "cpu_c674.h"
#include "lwip/tcpip.h"
#include "lwip/netifapi.h"
#include "lwip/autoip.h"
#include "lwip/dhcp.h"
#include "lwip/dns.h"
#include "lwip/sockets.h"
#include "netif/etharp.h"
#include "netbios.h"
#include "netio.h"
#include "sntp.h"
#include "http_test.h"

/* Macros */

/* Types */

/* Externals */

/* Variables */
unsigned char MAC_Addr[6] = { 0x00,0x12,0x34,0x56,0x78,0x9A };

struct netif netif;

/* Functions */
 
void test_init(void * arg)
{
	sys_sem_t *init_sem;
	LWIP_ASSERT("arg != NULL", arg != NULL);
	init_sem = (sys_sem_t*)arg;

	c674phy_sysclk4	= CPU_SYSCLK/4;
	c674phy_addr	= 1;
//	c674phy_anar	= PHY_ANAR_10BASE_FULL|PHY_ANAR_10BASE;	// Force 10 Mbit/s

	ip_addr_t addr, netmask, gw;
	IP4_ADDR(&addr, 192, 168, 10, 243);
	IP4_ADDR(&netmask, 255, 255, 255, 0);
	IP4_ADDR(&gw, 192, 168, 100, 200);
	memcpy(netif.hwaddr, MAC_Addr, 6);
#if LWIP_NETIF_HOSTNAME
	netif.hostname = "lwip";
#endif
	netif_add(&netif, &addr, &netmask, &gw, 0, c674emac_init, ethernet_input);
	netif_set_default(&netif);

	netbios_init();
	netio_init();

	// DHCP will also retrieve DNS servers
	if (0)
	{
		dhcp_start(&netif);
		dns_init();
		sntp_init();
	}
	// Auto IP
	else if (0)
	{
		autoip_start(&netif);
	}
	// Static IP address
	else
	{
		netif_set_up(&netif);
//		sntp_init();
	}

	sys_sem_signal(init_sem);
}

void test(void *arg)
{
 	/* Initialize LwIP */
	sys_sem_t init_sem;
	err_t err = sys_sem_new(&init_sem, 0);
	tcpip_init(test_init, &init_sem);
	sys_sem_wait(&init_sem);
	sys_sem_free(&init_sem);

 	/* Simple HTTP server test */
 	httpd();
}
