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

//#include <stdio.h>
#include <string.h>
#include "lwip/opt.h"

#if 1 /* don't build, this is only a skeleton, see previous comment */

#include "lwip/def.h"
#include "lwip/mem.h"
#ifdef CONFIG_OS_RHINO
#include "lwip/pbuf.h"
#else
#include "lwip/lwip_pbuf.h"
#endif
#include "lwip/sys.h"
#include <lwip/stats.h>
#include <lwip/snmp.h>
#include "lwip/ethip6.h"
#include "netif/etharp.h"
//#include "netif/ppp_oe.h"
#ifdef CONFIG_OS_RHINO
#include "../sdk/components/net/tcpip/lwip-1.4.0/src/include/netif/ethernetif.h"
#else
#include "netif/ethernetif.h"
#endif
#include "wificonf.h"
//#include "cli/cli_cmd_socket_send.h"

/* Define those to better describe your network interface. */
#define IFNAME0 'e'
#define IFNAME1 't'

//#include <pbuf.h>
#include <osal.h>
#include "netstack.h"

/**
 * Helper struct to hold private data used to operate your ethernet interface.
 * Keeping the ethernet address of the MAC in this struct is not necessary
 * as it is already kept in the struct netif.
 * But this is only an example, anyway...
 */
struct ethernetif {
  struct eth_addr *ethaddr;
  /* Add whatever per-interface state that is needed here. */
};

/** The default network interface. */
extern struct netif *netif_default;
extern IEEE80211STATUS gwifistatus;

sys_mbox_t eth_mbox;
/* Forward declarations. */
//static void  ethernetif_input(struct netif *netif);

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
//  struct ethernetif *ethernetif = netif->state;
  /*#define configMAC_ADDR0 0x00;
  #define configMAC_ADDR1 0x11;
  #define configMAC_ADDR2 0x22;
  #define configMAC_ADDR3 0x33;
  #define configMAC_ADDR4 0x44;
  #define configMAC_ADDR5 0x55;*/
  /* set MAC hardware address length */
  netif->hwaddr_len = ETHARP_HWADDR_LEN;

  /* set MAC hardware address */

  /*netif->hwaddr[ 0 ] = configMAC_ADDR0;
  netif->hwaddr[ 1 ] = configMAC_ADDR1;
  netif->hwaddr[ 2 ] = configMAC_ADDR2;
  netif->hwaddr[ 3 ] = configMAC_ADDR3;
  netif->hwaddr[ 4 ] = configMAC_ADDR4;
  netif->hwaddr[ 5 ] = configMAC_ADDR5;*/
  /* maximum transfer unit */
  netif->mtu = 1500;
  
  /* device capabilities */
  /* don't set NETIF_FLAG_ETHARP if this device is not an ethernet one */
  netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_IGMP;
 
  /* Do whatever else is needed to initialize interface. */  
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


#if CONFIG_STATUS_CHECK
extern u32 g_l2_tx_packets;
extern u32 g_l2_tx_copy;
extern u32 g_notpool;
extern u32 g_dump_tx;
extern u32 g_tx_allocate_fail;
#endif



#if CONFIG_USE_LWIP_PBUF

extern void _packetdump(const char *title, const u8 *buf,
                             size_t len);


static err_t
low_level_output(struct netif *netif, struct pbuf *p)
{

  struct pbuf *q;
  static unsigned char *ucBuffer;  
  unsigned char *pucChar;
  u8 *frame;
  s8 retry_cnt = 5;


#if  CONFIG_STATUS_CHECK
    g_l2_tx_packets++;
#endif


  /* Initiate transfer. */
  
//    printf("low_level_output 2\n");
	if( p->len == p->tot_len && p->type == PBUF_POOL) {
		/* No pbuf chain,  just give it to layer 2 */
        
#if ETH_PAD_SIZE
        if (pbuf_header(p, 0 - ETH_PAD_SIZE) != ERR_OK)
        {
           LOG_PRINTF ("low_level_output: could not allocate room for header.\r\n");
           pbuf_free(p);
           return ERR_BUF;
        }
#endif
		frame = (void*)p;
        pbuf_ref(p);        
	}
	else {

#if CONFIG_STATUS_CHECK
        g_l2_tx_copy ++;
        if(p->type != PBUF_POOL)
            g_notpool++;
#endif

        
        //printf("****==Xmit more than one pbuf for one packet==****p->len[%d] p->tot_len[%d]\r\n", p->len, p->tot_len);


retry:        
        frame = os_frame_alloc(p->tot_len*sizeof(unsigned char ));
        if(NULL == frame){
            retry_cnt--;
            
#if CONFIG_STATUS_CHECK
            g_tx_allocate_fail++;
#endif

            //don't xmit this frame. no enough frame.
            if(0 == retry_cnt){
               return ERR_OK;
            }
            OS_TickDelay(1);
            goto retry;
        }
                            
        ucBuffer = OS_FRAME_GET_DATA(frame);        		
		pucChar = ucBuffer;
		for( q = p; q != NULL; q = q->next ) 
		{
			/* Send the data from the pbuf to the interface, one pbuf at a
			time. The size of the data in each pbuf is kept in the ->len
			variable. */
			/* send data from(q->payload, q->len); */
			LWIP_DEBUGF( NETIF_DEBUG, ("NETIF: send pucChar %p q->payload %p q->len %i q->next %p\r\n", pucChar, q->payload, ( int ) q->len, ( void* ) q->next ) );
			if( q == p ) 
			{
			
				MEMCPY( pucChar, &( ( char * ) q->payload )[ ETH_PAD_SIZE ], q->len - ETH_PAD_SIZE );
				pucChar += q->len - ETH_PAD_SIZE;					
			} 
			else 
			{					
				MEMCPY( pucChar, q->payload, q->len );
				pucChar += q->len;		
			}				
		}		


	}


#if CONFIG_MEMP_DEBUG
    PBUF_DBG_FLAGS(((struct pbuf *)frame), PBUF_DBG_FLAG_L2);
#endif


#if CONFIG_STATUS_CHECK
    if(g_dump_tx)
        _packetdump("low_level_output", OS_FRAME_GET_DATA(frame), OS_FRAME_GET_DATA_LEN(frame));
#endif

	ssv6xxx_wifi_send_ethernet(frame, OS_FRAME_GET_DATA_LEN(frame), 0);


  return ERR_OK;
}



#else//CONFIG_USE_LWIP_PBUF

static err_t
low_level_output(struct netif *netif, struct pbuf *p)
{
  struct ethernetif *ethernetif = netif->state;
  struct pbuf *q;
  int i=0;
  static unsigned char *ucBuffer;  
  unsigned char *pucBuffer = ucBuffer;
  unsigned char *pucChar;
  err_t xReturn = ERR_OK;
  u8 *frame = NULL;
  bool copy_frm = false;


  /* Initiate transfer. */
//  printf("low_level_output 1\n");
	if( p->len == p->tot_len ) 
	{
		/* No pbuf chain, don't have to copy -> faster. */
#if ETH_PAD_SIZE        
		pucBuffer = &( ( unsigned char * ) p->payload )[ ETH_PAD_SIZE ];
		MEMCPY(ucBuffer, pucBuffer, p->tot_len*sizeof(unsigned char ));
#endif
        frame = (u8 *)p->payload;
	} 
	else 
	{
        //printf("****==Xmit more than one pbuf for one packet==****\r\n");
        
		/* pbuf chain, copy into contiguous ucBuffer. */
		/*if( p->tot_len >= sizeof( ucBuffer ) ) 
		{
			
			LINK_STATS_INC( link.lenerr );
			LINK_STATS_INC( link.drop );
			snmp_inc_ifoutdiscards( pxNetIf );
			xReturn = ERR_BUF;
		}
		else
		{*/
				
		//}
Retry:        
        frame = malloc(p->tot_len*sizeof(unsigned char));
        if (frame == NULL)
        {
            //sleeps;
            OS_TickDelay(10);
            goto Retry;
        }
        copy_frm = true;
        pucChar = frame;
        for( q = p; q != NULL; q = q->next ) 
        {
            /* Send the data from the pbuf to the interface, one pbuf at a
            time. The size of the data in each pbuf is kept in the ->len
            variable. */
            /* send data from(q->payload, q->len); */
            LWIP_DEBUGF( NETIF_DEBUG, ("NETIF: send pucChar %p q->payload %p q->len %i q->next %p\r\n", pucChar, q->payload, ( int ) q->len, ( void* ) q->next ) );
            if( q == p ) 
            {

                MEMCPY( pucChar, &( ( char * ) q->payload )[ ETH_PAD_SIZE ], q->len - ETH_PAD_SIZE );
                pucChar += q->len - ETH_PAD_SIZE;					
            } 
            else 
            {					
                MEMCPY( pucChar, q->payload, q->len );
                pucChar += q->len;		
            }				
        }	

	}

#ifndef __TCPIP_TEST__
	{
		//struct cfg_host_txreq txreq={0};		
		//txreq.txreq0.c_type = SSV6XXX_TX_REQ0;
		//txreq.txreq0.f80211  = 0;
		//txreq.priority = 0;
#if 0
		LWIP_DEBUGF("p->tot_len=%d\r\n",p->tot_len);
#endif

        char name[6];
        int namelen;
            
#ifdef CONFIG_OS_RHINO
        if(netif->name[0] == 0)
            namelen = 0;
        else if(netif->name[1] == 0)
            namelen = 1;
        else
            namelen = 2;
        memcpy(name, netif->name, namelen);
        name[namelen] = netif->num + '0';
        name[namelen + 1] = 0;
#else
        memcpy(name, netif->name, sizeof(netif->name));
#endif
            
        NETSTACK_NETWORK.output(name, frame, p->tot_len);
		//netstack_output(netif, frame, p->tot_len);
        if(copy_frm == true)
            free(frame);
	}
#else//__TCPIP_TEST__

	if_sim_xmit(ucBuffer, p->tot_len);
	PBUF_MFree(ucBuffer);
#endif//__TCPIP_TEST__
  return ERR_OK;
}



#endif//CONFIG_USE_LWIP_PBUF

#if 0
u8* get_data_ptr(struct cfg_host_rxpkt *rxpkt)
{
	u16 nOffset = 0;
	switch (rxpkt->c_type)
	{
	case M0_RXEVENT:
		nOffset += sizeof(struct cfg_host_rxpkt);
		break;
	default:
		LWIP_ASSERT("get_data_ptr error\r\nn",FALSE);
		break;
	}
	if (rxpkt->use_4addr)
		nOffset+=6;

	if (rxpkt->ht)
		nOffset+=4;

	if (rxpkt->qos)	
		nOffset+=2;
	
#if 0
	if(rxpkt->align2==1)
	{
		LOG_PRINTF("there!!!!");
		nOffset+=2;
	}
#endif
	//return (((u8*)rxpkt)+nOffset);
		return (((u8*)rxpkt)+nOffset+2);
}
#endif


/**
 * This function should be called when a packet is ready to be read
 * from the interface. It uses the function low_level_input() that
 * should handle the actual reception of bytes from the network
 * interface. Then the type of the received packet is determined and
 * the appropriate input function is called.
 *
 * @param netif the lwip network interface structure for this ethernetif
 */
/*static void
ethernetif_input(struct netif *netif)*/



#if CONFIG_USE_LWIP_PBUF


extern void _packetdump(const char *title, const u8 *buf,
                             size_t len);
err_t ethernetif_input(void *dat, u32 len)
{
  struct eth_hdr *ethhdr;
  struct pbuf *p;
  struct netif *eth0;

#if ETH_PAD_SIZE
  s8 retry_cnt = 5;
  struct pbuf *r;
#endif

  p = (struct pbuf *)dat;
  
#if ETH_PAD_SIZE
  if (pbuf_header(p, ETH_PAD_SIZE) != ERR_OK)
  {
      LOG_PRINTF ("ethernetif_input: could not allocate room for header.\r\n");
      pbuf_free(p);
      return SSV6XXX_DATA_ACPT;
  }
  
  if ((unsigned int)(p->payload) & 3)
  {
retry:
      r = os_frame_alloc(p->tot_len);
      if (NULL == r)
      {
         retry_cnt--;
         if(0 == retry_cnt)
         {
            pbuf_free(p);
            return SSV6XXX_DATA_ACPT;
         }
         OS_TickDelay(1);
         goto retry;
      }
      pbuf_copy(r, p);
      pbuf_free(p);  
      p = r;
  }
#endif

  ethhdr = p->payload;
  
  LOG_DEBUGF(LOG_L2_DATA, ("RX Ether type[%x] len[%d]\r\n", htons(ethhdr->type), len)); 
  switch (htons(ethhdr->type)) {
  /* IP or ARP packet? */
  case ETHTYPE_IP:
  case ETHTYPE_ARP:
#if PPPOE_SUPPORT
  /* PPPoE packet? */
  case ETHTYPE_PPPOEDISC:
  case ETHTYPE_PPPOE:
#endif /* PPPOE_SUPPORT */
	eth0=netif_default;
	if(eth0==NULL){
		pbuf_free(p);
	}
	else{
		/* full packet send to tcpip_thread to process */
		if (eth0->input(p, eth0)!=ERR_OK)
		{
			LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_input: IP input error\r\n"));
		pbuf_free(p);
		p = NULL;
		}
	}
    break;
  default:
    pbuf_free(p);
    p = NULL;
    break;
  }

  return SSV6XXX_DATA_ACPT;

}



#else//CONFIG_USE_LWIP_PBUF



err_t ethernetif_input(struct netif *pif, void *dat, u32 len)
{
  struct eth_hdr *ethhdr;
  struct pbuf *p,*q;
//  static struct netif *eth0;
  err_t ret = ERR_OK;
  s8 count = 0;
  #ifndef __TCPIP_TEST__
  u8 *r=(u8 *)dat;
  u32 offset=0;
  #else
  u8 *r=dat;
  u32 offset=0;
  #endif //__LINUX_SIMULATOR_TEST__

  //int res;
  /*if (!sys_mbox_valid(&eth_mbox)) {
	  sys_mbox_new(&eth_mbox,20);
  }*/
  
  while ((p = pbuf_alloc(PBUF_RAW, len-offset, PBUF_POOL)) == NULL)
  {
      OS_TickDelay(1);
      count++;
      LWIP_DEBUGF( NETIF_DEBUG, ("%s,%d pbuf allocate fail %d\r\n",__func__,__LINE__, count));

	  //wait for 10ms or give up!!
	  if(count>=10){
	  	  //printf("<WARN!!!>wait for pbuf_alloc!!!\n");	
		  return ret;
	  }
	  
  }
   
  /* no packet could be read, silently ignore this */
  if (p == NULL) LWIP_ASSERT(0, "packet is null");
  /* points to packet payload, which starts with an Ethernet header */
  if(p->len!=p->tot_len) 
  {
	  for( q = p; q != NULL; q = q->next ) {
		  
		  /*memcpy (q->payload,dat,q->len);
		  dat=(char *)dat+q->len;*/
		  MEMCPY ((void*)q->payload,(void*)r,q->len);
		  r=(u8 *)r+q->len;
		  /*LOG_PRINTF("Before dat=%x",(char *)dat);
		  LOG_PRINTF("After dat=%x\r\n",(char *)dat+q->len);*/
		
	  }
	  
  }
  else{
	  //memcpy (p->payload,dat,p->len);
	  MEMCPY (p->payload,r,p->len);
	   //p->payload=dat;
  }
  ethhdr = p->payload;
  switch (htons(ethhdr->type)) {
  /* IP or ARP packet? */
  case ETHTYPE_IP:
  case ETHTYPE_ARP:
#if LWIP_IPV6
  case ETHTYPE_IPV6:	
#endif	//LWIP_IPV6
  	
#if PPPOE_SUPPORT
  /* PPPoE packet? */
  case ETHTYPE_PPPOEDISC:
  case ETHTYPE_PPPOE:
#endif /* PPPOE_SUPPORT */
	//eth0=netif_default;
	if(pif==NULL){
		pbuf_free(p);
	}
	else{
		/* full packet send to tcpip_thread to process */
		if ((ret = pif->input(p, pif))!=ERR_OK)
		{
			LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_input: IP input error\r\n"));
		pbuf_free(p);
		p = NULL;
		}
	}
    break;
  default:
    pbuf_free(p);
    p = NULL;
    break;
  }
#ifndef __TCPIP_TEST__
  //os_frame_free(dat);

  //todo:
  return ret;
#endif
}
#endif//CONFIG_USE_LWIP_PBUF

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
  //Modified by Zoe
  //Reason:Use New pbuf_alloc function
  ethernetif = (struct ethernetif *)mem_malloc(sizeof(struct ethernetif));
  
  if (ethernetif == NULL) {
    LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_init: out of memory\r\n"));
    return ERR_MEM;
  }

#if LWIP_NETIF_HOSTNAME
  /* Initialize interface hostname */
  //netif->hostname = "lwip";
#endif /* LWIP_NETIF_HOSTNAME */

  /*
   * Initialize the snmp variables and counters inside the struct netif.
   * The last argument should be replaced with your link speed, in units
   * of bits per second.
   */
  NETIF_INIT_SNMP(netif, snmp_ifType_ethernet_csmacd, LINK_SPEED_OF_YOUR_NETIF_IN_BPS);

  netif->state = ethernetif;

#if 0  // disabled by felix
  netif->name[0] = IFNAME0;
  netif->name[1] = IFNAME1;
#endif

  /* We directly use etharp_output() here to save a function call.
   * You can instead declare your own function an call etharp_output()
   * from it if you have to do some checks before sending (e.g. if link
   * is available...) */
  netif->output = etharp_output;
#if LWIP_IPV6
  netif->output_ip6 = ethip6_output;
#endif /* LWIP_IPV6 */
  netif->linkoutput = low_level_output;
  
  ethernetif->ethaddr = (struct eth_addr *)&(netif->hwaddr[0]);
  
  /* initialize the hardware */
  low_level_init(netif);

  return ERR_OK;
}
#endif /* 0 */
