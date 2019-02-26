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

#if 0
#include "lwip/def.h"
#include "lwip/mem.h"
#include "lwip/pbuf.h"
#include "lwip/sys.h"
#include <lwip/stats.h>
#include <lwip/snmp.h>
#include "netif/etharp.h"
//#include "netif/ppp_oe.h"
#else
#include "aos/kernel.h"
#include "aos/yloop.h"
#include <k_api.h>
#include "network/network.h"
#include <lwip/tcpip.h>
#include "netif/etharp.h"

#include "ethernetif.h"
#include <string.h>
#endif
#include "netif/m480_eth.h"
#include "string.h"

/* The time to block waiting for input. */
#define TIME_WAITING_FOR_INPUT                 ( AOS_WAIT_FOREVER )
/* Stack size of the interface thread */
#define INTERFACE_THREAD_STACK_SIZE            ( 1024 )
#define INTERFACE_THREAD__PRIO                 ( 12 )

/* Define those to better describe your network interface. */
#define IFNAME0 'e'
#define IFNAME1 'n'

typedef struct {
    ip4_addr_t ip;
    ip4_addr_t netmask;
    ip4_addr_t gw;
} tcpip_ip_info_t;

/* Semaphore to signal incoming packets */
aos_sem_t s_xSemaphore;
struct netif lwip_netif;

tcpip_ip_info_t eth_ip_info = {0};

struct netif *_netif;
//extern u8_t my_mac_addr[6];

/**
 * Helper struct to hold private data used to operate your ethernet interface.
 * Keeping the ethernet address of the MAC in this struct is not necessary
 * as it is already kept in the struct netif.
 * But this is only an example, anyway...
 */
struct ethernetif
{
    struct eth_addr *ethaddr;
    /* Add whatever per-interface state that is needed here. */
};


/**
 * In this function, the hardware should be initialized.
 * Called from ethernetif_init().
 *
 * @param netif the already initialized lwip network interface structure
 *        for this ethernetif
 */
static void
low_level_init(struct netif *netif)
{
	uint8_t my_mac_addr[6]= { 0x00, 0x0c, 0x29, 0xcd, 0x94, 0x38 };

    /* set MAC hardware address length */
    netif->hwaddr_len = ETHARP_HWADDR_LEN;

    /* set MAC hardware address */
    netif->hwaddr[0] = my_mac_addr[0];
    netif->hwaddr[1] = my_mac_addr[1];
    netif->hwaddr[2] = my_mac_addr[2];
    netif->hwaddr[3] = my_mac_addr[3];
    netif->hwaddr[4] = my_mac_addr[4];
    netif->hwaddr[5] = my_mac_addr[5];

    /* maximum transfer unit */
    netif->mtu = 1500;

    /* device capabilities */
    netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_LINK_UP;
#ifdef LWIP_IGMP
    netif->flags |= NETIF_FLAG_IGMP;
#endif

	aos_sem_new(&s_xSemaphore, 0);

	/* create the task that handles the ETH_MAC */
	sys_thread_new("netif", ethernetif_input, &lwip_netif, INTERFACE_THREAD_STACK_SIZE, INTERFACE_THREAD__PRIO);

    // TODO: enable clock & configure GPIO function
    ETH_init(netif->hwaddr);
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

static err_t
low_level_output(struct netif *netif, struct pbuf *p)
{
    struct pbuf *q;
    u8_t *buf = NULL;
    u16_t len = 0;

    buf = ETH_get_tx_buf();
    if(buf == NULL)
        return ERR_MEM;
#if ETH_PAD_SIZE
    pbuf_header(p, -ETH_PAD_SIZE); /* drop the padding word */
#endif

    for(q = p; q != NULL; q = q->next)
    {
        memcpy((u8_t*)&buf[len], q->payload, q->len);
        len = len + q->len;
    }
#ifdef TIME_STAMPING
    ETH_trigger_tx(len, p->flags & PBUF_FLAG_GET_TXTS ? p : NULL);
#else
    ETH_trigger_tx(len, NULL);
#endif

#if ETH_PAD_SIZE
    pbuf_header(p, ETH_PAD_SIZE); /* reclaim the padding word */
#endif

    LINK_STATS_INC(link.xmit);

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
static struct pbuf *
low_level_input(struct netif *netif)
{
    struct pbuf *p, *q;
    struct eth_descriptor* RXValid =NULL;
    int len;
    u8_t * buf;

	if ( !(RXValid=(struct eth_descriptor*)EMAC_GetValidRXDescriptor()) )
		goto exit_low_level_input;

	len = RXValid->status1&0xFFFF;
	buf = (u8_t*)RXValid->buf;

#if ETH_PAD_SIZE
    len += ETH_PAD_SIZE; /* allow room for Ethernet padding */
#endif

    /* We allocate a pbuf chain of pbufs from the pool. */
    p = pbuf_alloc(PBUF_RAW, len, PBUF_POOL);
    if (p != NULL)
    {

#if ETH_PAD_SIZE
        pbuf_header(p, -ETH_PAD_SIZE); /* drop the padding word */
#endif

        len = 0;
        /* We iterate over the pbuf chain until we have read the entire
        * packet into the pbuf. */
        for(q = p; q != NULL; q = q->next)
        {
            memcpy((u8_t*)q->payload, (u8_t*)&buf[len], q->len);
            len = len + q->len;
        }


#if ETH_PAD_SIZE
        pbuf_header(p, ETH_PAD_SIZE); /* reclaim the padding word */
#endif

        LINK_STATS_INC(link.recv);
    }
    else
    {
        // do nothing. drop the packet
        LINK_STATS_INC(link.memerr);
        LINK_STATS_INC(link.drop);
    }
    
	EMAC_ReleaseRXDescriptor();	
    return p;
    
exit_low_level_input:
	EMAC_TriggerRX();
	return NULL;
}

/**
  * @brief  Ethernet Rx Transfer completed callback
  * @param  heth: ETH handle
  * @retval None
  */
void EMAC_Rx_Callback(void)
{
  aos_sem_signal(&s_xSemaphore);
}

/**
 * This function should be called when a packet is ready to be read
 * from the interface. It uses the function low_level_input() that
 * should handle the actual reception of bytes from the network
 * interface. Then the type of the received packet is determined and
 * the appropriate input function is called.
 *
 * @param netif the lwip network interface structure for this ethernetif
 */
err_t ethernetif_input(struct netif *netif)
{
  struct pbuf *p;

  for( ;; )
  {
    if (aos_sem_wait( &s_xSemaphore, TIME_WAITING_FOR_INPUT) == 0)
    {
      do
      {
        p = low_level_input( netif );
        if (p != NULL)
        {
          if (netif->input( p, netif) != ERR_OK )
          {
            pbuf_free(p);
          }
        }
      }while(p!=NULL);
    }
  }
}

#ifdef    TIME_STAMPING
void
ethernetif_loopback_input(struct pbuf *p)           // TODO: make sure packet not drop in input()
{
    struct eth_hdr *ethhdr;

    /* points to packet payload, which starts with an Ethernet header */
    ethhdr = p->payload;

    switch (htons(ethhdr->type))
    {
    /* IP or ARP packet? */
    case ETHTYPE_IP:
    case ETHTYPE_ARP:
#if PPPOE_SUPPORT
    /* PPPoE packet? */
    case ETHTYPE_PPPOEDISC:
    case ETHTYPE_PPPOE:
#endif /* PPPOE_SUPPORT */
        /* full packet send to tcpip_thread to process */
        if (_netif->input(p, _netif)!=ERR_OK)
        {
            LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_input: IP input error\n"));
            pbuf_free(p);
            p = NULL;
        }
        break;

    default:
        pbuf_free(p);
        p = NULL;
        break;
    }
}

#endif

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
err_t
ethernetif_init(struct netif *netif)
{
    struct ethernetif *ethernetif;

    LWIP_ASSERT("netif != NULL", (netif != NULL));

    _netif = netif;
    ethernetif = mem_malloc(sizeof(struct ethernetif));
    if (ethernetif == NULL)
    {
        LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_init: out of memory\n"));
        return ERR_MEM;
    }

#if LWIP_NETIF_HOSTNAME
    /* Initialize interface hostname */
    netif->hostname = "m480";
#endif /* LWIP_NETIF_HOSTNAME */

    /*
     * Initialize the snmp variables and counters inside the struct netif.
     * The last argument should be replaced with your link speed, in units
     * of bits per second.
     */
   // NETIF_INIT_SNMP(netif, snmp_ifType_ethernet_csmacd, LINK_SPEED_OF_YOUR_NETIF_IN_BPS);

    netif->state = ethernetif;
    netif->name[0] = IFNAME0;
    netif->name[1] = IFNAME1;
    /* We directly use etharp_output() here to save a function call.
     * You can instead declare your own function an call etharp_output()
     * from it if you have to do some checks before sending (e.g. if link
     * is available...) */
    netif->output = etharp_output;
    netif->linkoutput = low_level_output;

    ethernetif->ethaddr = (struct eth_addr *)&(netif->hwaddr[0]);

    /* initialize the hardware */
    low_level_init(netif);

    return ERR_OK;
}



static void tcpip_dhcpc_cb(struct netif *pstnetif)
{
    long long ts = aos_now();

    srand((unsigned int)ts);
    tcp_init();
    udp_init();

    if (!ip4_addr_cmp(ip_2_ip4(&pstnetif->ip_addr), IP4_ADDR_ANY4)) {
        //check whether IP is changed
        if (!ip4_addr_cmp(ip_2_ip4(&pstnetif->ip_addr), &eth_ip_info.ip) ||
                !ip4_addr_cmp(ip_2_ip4(&pstnetif->netmask), &eth_ip_info.netmask) ||
                !ip4_addr_cmp(ip_2_ip4(&pstnetif->gw), &eth_ip_info.gw)) {
            ip4_addr_set(&eth_ip_info.ip, ip_2_ip4(&pstnetif->ip_addr));
            ip4_addr_set(&eth_ip_info.netmask, ip_2_ip4(&pstnetif->netmask));
            ip4_addr_set(&eth_ip_info.gw, ip_2_ip4(&pstnetif->gw));
            printf("post got ip event ,ip is 0x%x \r\n", (unsigned int)eth_ip_info.ip.addr);
            aos_post_event(EV_WIFI, CODE_WIFI_ON_GOT_IP, 0xdeaddead);
        }
    }
    return;
}


err_t tcpip_dhcpc_start(struct netif *pstnetif)
{
    if (NULL == pstnetif){
        return -1;
    }

    if (netif_is_up(pstnetif)) {
        if (dhcp_start(pstnetif) != ERR_OK) {
            return -1;
        }
    }

    netif_set_status_callback(pstnetif, tcpip_dhcpc_cb);
}

static void tcpip_init_done(void *arg)
{
#if LWIP_IPV4
    ip4_addr_t ipaddr, netmask, gw;
    memset(&ipaddr, 0, sizeof(ipaddr));
    memset(&netmask, 0, sizeof(netmask));
    memset(&gw, 0, sizeof(gw));
    netif_add(&lwip_netif, &ipaddr, &netmask, &gw, NULL, ethernetif_init, tcpip_input);
#endif

#if LWIP_IPV6
#if !LWIP_IPV4
    netif_add(&lwip_netif, NULL, ethernetif_init, tcpip_input);
#endif
    netif_create_ip6_linklocal_address(&lwip_netif, 1);
    lwip_netif.ip6_autoconfig_enabled = 1;
#endif

    netif_set_default(&lwip_netif);
    netif_set_up(&lwip_netif);
    tcpip_dhcpc_start(&lwip_netif);
}

int lwip_tcpip_init(void)
{
    tcpip_init(NULL, NULL);
    tcpip_init_done(NULL);
    printf("TCP/IP initialized.\r\n");
    return 0;
}

