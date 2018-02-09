/**
 * @file
 * Ethernet Interface Skeleton
 *
 */

/*
 * Copyright (c) 2001-2004 Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Adam Dunkels <adam@sics.se>
 *
 */

/*
 * This file is a skeleton for developing Ethernet network interface
 * drivers for lwIP. Add code to the low_level functions and do a
 * search-and-replace for the word "ethernetif" to replace it with
 * something that better describes your network interface.
 */

#include "lwip/opt.h"
#include "lwip/def.h"
#include "lwip/mem.h"
#include "lwip/pbuf.h"
#include "lwip/sys.h"
#include "lwip/tcpip.h"
#include "lwip/icmp.h"
#include "lwip/timeouts.h"
#include "netif/etharp.h"
#include "err.h"
#include "ethernetif.h"
//#include "queue.h"
#include "lwip_netconf.h"

//#include "lwip/ethip6.h" //Add for ipv6

#include <platform/platform_stdlib.h>
#include "platform_opts.h"

#if CONFIG_WLAN
#include <lwip_intf.h>
#endif

#if CONFIG_INIC_HOST
#include "freertos/inic_intf.h"
#endif

#define netifMTU                                (1500)
#define netifINTERFACE_TASK_STACK_SIZE		( 350 )
#define netifINTERFACE_TASK_PRIORITY		( configMAX_PRIORITIES - 1 )
#define netifGUARD_BLOCK_TIME			( 250 )
/* The time to block waiting for input. */
#define emacBLOCK_TIME_WAITING_FOR_INPUT	( ( portTickType ) 100 )


#ifdef CONFIG_CONCURRENT_MODE
#define IF2NAME0 'r'
#define IF2NAME1 '2'
#endif

static void arp_timer(void *arg);


/**
 * In this function, the hardware should be initialized.
 * Called from ethernetif_init().
 *
 * @param netif the already initialized lwip network interface structure
 *        for this ethernetif
 */

static void low_level_init(struct netif *netif)
{

	/* set netif MAC hardware address length */
	netif->hwaddr_len = ETHARP_HWADDR_LEN;

	/* set netif maximum transfer unit */
	netif->mtu = 1500;

	/* Accept broadcast address and ARP traffic */
	netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP;	     

#if LWIP_IGMP
	/* make LwIP_Init do igmp_start to add group 224.0.0.1 */
	netif->flags |= NETIF_FLAG_IGMP;
#endif

	/* Wlan interface is initialized later */
}


/**
 * This function should do the actual transmission of the packet. The packet is
 * contained in the pbuf that is passed to the function. This pbuf
 * might be chained.
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @param p the MAC packet to send (e.g. IP packet including MAC addresses and type)
 * @return ERR_OK if the packet could be sent
 *         an err_t value if the packet couldn't be sent
 *
 * @note Returning ERR_MEM here if a DMA queue of your MAC is full can lead to
 *       strange results. You might consider waiting for space in the DMA queue
 *       to become availale since the stack doesn't retry to send a packet
 *       dropped because of memory failure (except for the TCP timers).
 */

static err_t low_level_output(struct netif *netif, struct pbuf *p)
{


  /* Refer to eCos lwip eth_drv_send() */
	struct eth_drv_sg sg_list[MAX_ETH_DRV_SG];
	int sg_len = 0;
	struct pbuf *q;
#if CONFIG_WLAN
	if(!rltk_wlan_running(netif_get_idx(netif)))
		return ERR_IF;
#endif
	for (q = p; q != NULL && sg_len < MAX_ETH_DRV_SG; q = q->next) {
		sg_list[sg_len].buf = (unsigned int) q->payload;
		sg_list[sg_len++].len = q->len;
	}

	if (sg_len) {
#if CONFIG_WLAN
		if (rltk_wlan_send(netif_get_idx(netif), sg_list, sg_len, p->tot_len) == 0)
#elif CONFIG_INIC_HOST
		if(rltk_inic_send( sg_list, sg_len, p->tot_len) == 0)
#else
                if(1)
#endif
			return ERR_OK;
		else
			return ERR_BUF;	// return a non-fatal error
	}
	return ERR_OK;
}

/*for ethernet mii interface*/
static err_t low_level_output_mii(struct netif *netif, struct pbuf *p)
{
	struct eth_drv_sg sg_list[MAX_ETH_DRV_SG];
	int sg_len = 0;
	struct pbuf *q;
	for (q = p; q != NULL && sg_len < MAX_ETH_DRV_SG; q = q->next) {
		sg_list[sg_len].buf = (unsigned int) q->payload;
		sg_list[sg_len++].len = q->len;
	}
	if (sg_len) {
		 if(rltk_mii_send(sg_list, sg_len, p->tot_len) == 0)
			return ERR_OK;
		else
			return ERR_BUF;	// return a non-fatal error
	}
	return ERR_OK;
}

	
/**
 * Should allocate a pbuf and transfer the bytes of the incoming
 * packet from the interface into the pbuf.
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return a pbuf filled with the received packet (including MAC header)
 *         NULL on memory error
 */
//static struct pbuf * low_level_input(struct netif *netif){}


/**
 * This function is the ethernetif_input task, it is processed when a packet 
 * is ready to be read from the interface. It uses the function low_level_input() 
 * that should handle the actual reception of bytes from the network
 * interface. Then the type of the received packet is determined and
 * the appropriate input function is called.
 *
 * @param netif the lwip network interface structure for this ethernetif
 */
//void ethernetif_input( void * pvParameters )


/* Refer to eCos eth_drv_recv to do similarly in ethernetif_input */
void ethernetif_recv(struct netif *netif, int total_len)
{
	struct eth_drv_sg sg_list[MAX_ETH_DRV_SG];
	struct pbuf *p, *q;
	int sg_len = 0;
#if CONFIG_WLAN
	if(!rltk_wlan_running(netif_get_idx(netif)))
		return;
#endif
	if ((total_len > MAX_ETH_MSG) || (total_len < 0))
		total_len = MAX_ETH_MSG;

	// Allocate buffer to store received packet
	p = pbuf_alloc(PBUF_RAW, total_len, PBUF_POOL);
	if (p == NULL) {
		printf("\n\rCannot allocate pbuf to receive packet");
		return;
	}

	// Create scatter list
	for (q = p; q != NULL && sg_len < MAX_ETH_DRV_SG; q = q->next) {
   		sg_list[sg_len].buf = (unsigned int) q->payload;
		sg_list[sg_len++].len = q->len;
	}

	// Copy received packet to scatter list from wrapper rx skb
  	//printf("\n\rwlan:%c: Recv sg_len: %d, tot_len:%d", netif->name[1],sg_len, total_len);
#if CONFIG_WLAN
	rltk_wlan_recv(netif_get_idx(netif), sg_list, sg_len);
#elif CONFIG_INIC_HOST
	rltk_inic_recv(sg_list, sg_len);
#endif
	// Pass received packet to the interface
	if (ERR_OK != netif->input(p, netif))
		pbuf_free(p);

}

void ethernetif_mii_recv(struct netif *netif, int total_len)
{
	struct eth_drv_sg sg_list[MAX_ETH_DRV_SG];
	struct pbuf *p, *q;
	int sg_len = 0;

	if ((total_len > MAX_ETH_MSG) || (total_len < 0))
		total_len = MAX_ETH_MSG;

	// Allocate buffer to store received packet
	p = pbuf_alloc(PBUF_RAW, total_len, PBUF_POOL);
	if (p == NULL) {
		printf("\n\rCannot allocate pbuf to receive packet");
		return;
	}

	// Create scatter list
	for (q = p; q != NULL && sg_len < MAX_ETH_DRV_SG; q = q->next) {
   		sg_list[sg_len].buf = (unsigned int) q->payload;
		sg_list[sg_len++].len = q->len;
	}
	rltk_mii_recv(sg_list, sg_len);

	// Pass received packet to the interface
	if (ERR_OK != netif->input(p, netif))
		pbuf_free(p);

}
/**
 * Should be called at the beginning of the program to set up the
 * network interface. It calls the function low_level_init() to do the
 * actual setup of the hardware.
 *
 * This function should be passed as a parameter to netif_add().
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return ERR_OK if the loopif is initialized
 *         ERR_MEM if private data couldn't be allocated
 *         any other err_t on error
 */
err_t ethernetif_init(struct netif *netif)
{
	LWIP_ASSERT("netif != NULL", (netif != NULL));

#if LWIP_NETIF_HOSTNAME
	/* Initialize interface hostname */
	if(netif->name[1] == '0')
		netif->hostname = "lwip0";
	else if(netif->name[1] == '1')
		netif->hostname = "lwip1";
#endif /* LWIP_NETIF_HOSTNAME */

	netif->output = etharp_output;
//#if LWIP_IPV6
//	netif->output_ip6 = ethip6_output;
//#endif
	netif->linkoutput = low_level_output;

	/* initialize the hardware */
	low_level_init(netif);

	etharp_init();

	return ERR_OK;
}

err_t ethernetif_mii_init(struct netif *netif)
{
	LWIP_ASSERT("netif != NULL", (netif != NULL));

#if LWIP_NETIF_HOSTNAME
		netif->hostname = "lwip2";		
#endif /* LWIP_NETIF_HOSTNAME */

	netif->output = etharp_output;
//#if LWIP_IPV6
//	netif->output_ip6 = ethip6_output;
//#endif
	netif->linkoutput = low_level_output_mii;

	/* initialize the hardware */
	low_level_init(netif);

	etharp_init();

	return ERR_OK;
}

static void arp_timer(void *arg)
{
  etharp_tmr();
  sys_timeout(ARP_TMR_INTERVAL, arp_timer, NULL);
}

/*
 * For FreeRTOS tickless
 */
int lwip_tickless_used = 0;
#if 0
int arp_timeout_exist(void)
{
	struct sys_timeouts *timeouts;
	struct sys_timeo *t;

	timeouts = sys_arch_timeouts();

	for(t = timeouts->next; t != NULL;t = t->next)
		if(t->h == arp_timer)
			return 1;

	return 0;
}
#endif
//Called by rltk_wlan_PRE_SLEEP_PROCESSING()
void lwip_PRE_SLEEP_PROCESSING(void)
{
	//if(arp_timeout_exist()) {
	//	tcpip_untimeout(arp_timer, NULL);
	//}
	lwip_tickless_used = 1;
}

//Called in ips_leave() path, support tickless when wifi power wakeup due to ioctl or deinit
void lwip_POST_SLEEP_PROCESSING(void)
{
	if(lwip_tickless_used) {
		tcpip_timeout(ARP_TMR_INTERVAL, arp_timer, NULL);
	}
}

