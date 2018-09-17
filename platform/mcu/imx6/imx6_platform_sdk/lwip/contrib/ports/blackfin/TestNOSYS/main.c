/************************************************************************/
/**	\file	main.c
 *	\brief	LwIP NO_SYS test
 *
 *	\date	2011/10/06
 *	\author	Stephane Lesage for LwIP
 *
 */
/************************************************************************/

/* Includes */
#include <services\services.h>
#include <time.h>
#include "cpu.h"
#include "lwip/init.h"
#include "lwip/timers.h"
#include "netif/etharp.h"
#include "lwip/autoip.h"
#include "lwip/dhcp.h"
#include "lwip/dns.h"
#include "netbios.h"
#include "netio.h"
#include "sntp.h"
#include "bfemac.h"
#include "bfphy.h"

/* Macros */

/* Types */

/* Externals */

/* Variables */
char adi_int_mem[16*ADI_INT_SECONDARY_MEMORY];

unsigned char MAC_Addr[6] = { 0x00,0x12,0x34,0x56,0x78,0x9A };

struct netif netif;

/* Functions */

u32_t sys_now()
{
	clock_t c = clock(); 
	return c / (CLOCKS_PER_SEC/1000);
}

void test_init(void)
{
	bfemac_rmii = 0;
	bfphy_sclk	= CPU_SCLK;
	bfphy_addr	= 1;
//	bfphy_anar	= PHY_ANAR_10BASE_FULL|PHY_ANAR_10BASE;	// Force 10 Mbit/s

	ip_addr_t addr, netmask, gw;
	IP4_ADDR(&addr, 192, 168, 10, 159);
	IP4_ADDR(&netmask, 255, 255, 255, 0);
	IP4_ADDR(&gw, 192, 168, 10, 200);
	memcpy(netif.hwaddr, MAC_Addr, 6);
#if LWIP_NETIF_HOSTNAME
	netif.hostname = "lwip";
#endif
	netif_add(&netif, &addr, &netmask, &gw, 0, bfemac_init, ethernet_input);
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
	}
}

void main(void)
{
	/* Low-level Initializations */
	CPU_Init();
	ADI_INT_RESULT res = adi_int_Init(&adi_int_mem, sizeof(adi_int_mem), NULL, NULL);

 	/* Initialize LwIP */
 	lwip_init();
	test_init();

 	/* Main Loop */
 	while (1)
 	{
 		bfphy_poll(&netif);
 		bfemac_poll(&netif);
 		sys_check_timeouts();
 		
 		CPU_Wait(12 + (rand()%34));
 	}
}
