/************************************************************************/
/**	\file	BootThread.cpp
 *	\brief	Boot Thread for LwIP test
 *
 *	\date	2011/10/06
 *	\author	Stephane Lesage for LwIP
 *
 */
/************************************************************************/

/* Includes */
#include "BootThread.h"
#include <new>
#pragma file_attr("OS_Component=Threads")
#pragma file_attr("Threads")

#include <services\services.h>
#include "cpu.h"
#include "lwip/tcpip.h"
#include "lwip/netifapi.h"
#include "netif/etharp.h"
#include "lwip/autoip.h"
#include "lwip/dhcp.h"
#include "lwip/dns.h"
#include "lwip/sockets.h"
#include "netbios.h"
#include "netio.h"
#include "sntp.h"
#include "bfemac.h"
#include "bfphy.h"
#include "http_test.h"

/* Macros */

/* Types */

/* Externals */

/* Variables */
char adi_int_mem[16*ADI_INT_SECONDARY_MEMORY];

unsigned char MAC_Addr[6] = { 0x00,0x12,0x34,0x56,0x78,0x9A };

struct netif netif;

/* Functions */
 
void test_init(void * arg)
{
	sys_sem_t *init_sem;
	LWIP_ASSERT("arg != NULL", arg != NULL);
	init_sem = (sys_sem_t*)arg;

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
		//dns_init();
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

	sys_sem_signal(init_sem);
}

void BootThread::Run()
{
	/* Low-level Initializations */
	CPU_Init();
	VDK::SetClockFrequency(CPU_CCLK / 1000000);
	ADI_INT_RESULT res = adi_int_Init(&adi_int_mem, sizeof(adi_int_mem), NULL, NULL);

 	/* Initialize LwIP */
	sys_sem_t init_sem;
	err_t err = sys_sem_new(&init_sem, 0);
	tcpip_init(test_init, &init_sem);
	sys_sem_wait(&init_sem);
	sys_sem_free(&init_sem);

 	/* Simple HTTP server test */
 	httpd();
}

int BootThread::ErrorHandler()
{
	return (VDK::Thread::ErrorHandler());
}

BootThread::BootThread(VDK::Thread::ThreadCreationBlock &tcb) : VDK::Thread(tcb)
{
}

BootThread::~BootThread()
{
}

VDK::Thread* BootThread::Create(VDK::Thread::ThreadCreationBlock &tcb)
{
	return new BootThread(tcb);
}
